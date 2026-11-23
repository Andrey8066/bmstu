#include "osmdataloader.hpp"

#include <QDebug>
#include <QEventLoop>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <cmath>

OSMDataLoader::OSMDataLoader(const QString apiUrl, double lat, double lon,
                             double radius, QObject *parent)
    : QObject(parent), manager(new QNetworkAccessManager(this)), url(apiUrl) {

  // TODO: прописать преобразование радиуса в отклонение координат
  //  Координаты центра карты по умолчанию (Москва)

  defaultArea.lat = lat; // 55.766863, 37.681677
  defaultArea.lon = lon;
  defaultArea.width = radToLat(radius);  // +/- 0.2 градуса по широте
  defaultArea.height = radToLon(radius); // +/- 0.3 градуса по долготе

  connect(this, &OSMDataLoader::dataLoaded, this,
          [&](const QJsonArray &data, const QString &queryType) {
            saveToJSONFile(data, QString("osm_%1.json").arg(queryType));
          });
}

QByteArray OSMDataLoader::fetchOSMData(const QString &queryType,
                                       const int &timeout, const QRect &area) {
  Q_UNUSED(area);

  OSMDataLoader::timeout = timeout;

  activeQuery = buildOverpassQuery(timeout, queryType);

  if (activeQuery.isEmpty()) {
    qDebug() << "Неизвестный тип запроса:" << queryType;
    return {};
  }

  activeQueryType = queryType;

  const double minLat = defaultArea.lat - defaultArea.width;
  const double maxLat = defaultArea.lat + defaultArea.width;
  const double minLon = defaultArea.lon - defaultArea.height;
  const double maxLon = defaultArea.lon + defaultArea.height;

  const QString bbox = QString("%1,%2,%3,%4")
                           .arg(minLat, 0, 'f', 6)
                           .arg(minLon, 0, 'f', 6)
                           .arg(maxLat, 0, 'f', 6)
                           .arg(maxLon, 0, 'f', 6);

  activeQuery.replace("{{bbox}}", bbox);

  qDebug() << "Тип запроса:" << queryType;
  qDebug() << "BBox:" << bbox;
  qDebug() << "Весь запрос:" << activeQuery;

  // Основной и резервные серверы Overpass API.
  servers = {"https://overpass-api.de/api/interpreter",
             "https://overpass.private.coffee/api/interpreter",
             "https://maps.mail.ru/osm/tools/overpass/api/interpreter"};

  requestAttempt = 0;

  // Не допускаем одновременного выполнения
  // нескольких запросов этим загрузчиком.
  if (reply && !reply->isFinished()) {
    disconnect(reply, nullptr, this, nullptr);
    reply->abort();
  }

  reply = nullptr;

  sendNextRequest();

  // Загрузка асинхронная.
  // Результат будет передан через dataLoaded.
  return {};
}

void OSMDataLoader::sendNextRequest() {
  if (requestAttempt >= maxAttempts) {
    qDebug() << "Все попытки загрузки OSM исчерпаны";

    emit errorOccurred("Не удалось загрузить OSM после всех попыток");

    return;
  }

  const int serverIndex = requestAttempt % servers.size();
  const QString serverUrl = servers[serverIndex];

  qDebug() << "================================";
  qDebug() << "Попытка загрузки:" << requestAttempt + 1;
  qDebug() << "Сервер:" << serverUrl;

  QNetworkRequest request{QUrl(serverUrl)};

  request.setTransferTimeout(timeout * 1000 + 10);

  request.setHeader(QNetworkRequest::ContentTypeHeader,
                    "application/x-www-form-urlencoded");

  request.setHeader(QNetworkRequest::UserAgentHeader,
                    "BMSTU-NIR-OSM-Loader/1.0");

  request.setRawHeader("Accept", "application/json");

  QByteArray postData = "data=";
  postData += QUrl::toPercentEncoding(activeQuery);

  QNetworkReply *currentReply = manager->post(request, postData);

  reply = currentReply;

  connect(currentReply, &QNetworkReply::finished, this, [this, currentReply]() {
    const int status =
        currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute)
            .toInt();

    const auto networkError = currentReply->error();

    qDebug() << "HTTP status:" << status;

    if (networkError != QNetworkReply::NoError) {

      const QString error = currentReply->errorString();

      const QByteArray response = currentReply->readAll();

      qDebug() << "Ошибка загрузки:" << error;
      qDebug() << "Ответ сервера:" << response;

      const bool retryable = status == 0 || status == 429 || status == 502 ||
                             status == 503 || status == 504 ||
                             networkError == QNetworkReply::TimeoutError;

      if (reply == currentReply)
        reply = nullptr;

      currentReply->deleteLater();

      if (!retryable) {
        emit errorOccurred("Ошибка загрузки OSM: " + error);
        return;
      }

      const int delay = 1000 * (1 << requestAttempt);

      requestAttempt++;

      if (requestAttempt >= maxAttempts) {
        sendNextRequest();
        return;
      }

      qDebug() << "Повторная попытка через" << delay << "мс";

      QTimer::singleShot(delay, this, [this]() { sendNextRequest(); });

      return;
    }

    const QByteArray response = currentReply->readAll();

    if (reply == currentReply)
      reply = nullptr;

    currentReply->deleteLater();

    QJsonParseError parseError;

    const QJsonDocument doc = QJsonDocument::fromJson(response, &parseError);

    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {

      qDebug() << "Ошибка парсинга JSON:" << parseError.errorString();

      emit errorOccurred("Сервер вернул некорректный JSON");

      return;
    }

    const QJsonObject root = doc.object();

    if (!root.contains("elements") || !root["elements"].isArray()) {

      emit errorOccurred("В ответе отсутствует массив elements");

      return;
    }

    requestResult = response;

    const QJsonArray elements = root["elements"].toArray();

    qDebug() << "OSM JSON успешно получен";
    qDebug() << "Получено байт:" << response.size();
    qDebug() << "Количество объектов:" << elements.size();

    emit dataLoaded(elements, activeQueryType);
  });
}

OSMDataLoaderArea OSMDataLoader::getDefaultArea() const { return defaultArea; }

void OSMDataLoader::setArea(const OSMDataLoaderArea &area) {
  defaultArea = area;
}

void OSMDataLoader::setArea(double lat, double lon, double radius) {

  defaultArea.lat = lat; // 55.766863, 37.681677
  defaultArea.lon = lon;
  defaultArea.width = radToLat(radius);  // +/- 0.2 градуса по широте
  defaultArea.height = radToLon(radius); // +/- 0.3 градуса по долготе
}

// Формирование Overpass QL запроса - правильная syntax

QString OSMDataLoader::buildOverpassQuery(const int &timeout,
                                          const QString &queryType) {
  if (queryType == "buildings") {
    return QString(R"(
            [out:json][timeout:%1];

            (
                nwr["building"]({{bbox}});
                nwr["building:part"]({{bbox}});
                relation["type"="building"]({{bbox}});
            );

            out geom;
        )")
        .arg(timeout);
  }

  if (queryType == "all") {
    return QString(R"([out:json][timeout:%1];
                   (
                  nwr["building"]({{bbox}});
                  nwr["building:part"]({{bbox}});
                  nwr["highway"]({{bbox}});

                  nwr["waterway"]({{bbox}});
                  nwr["natural"="water"]({{bbox}});
                  nwr["water"]({{bbox}});
                   );
                   out geom;
                   )")
        .arg(timeout);
  }

  qWarning() << "Неизвестный тип запроса:" << queryType;
  return QString();
}

// Сохранение JSON в файл
QString OSMDataLoader::saveToJSONFile(const QJsonArray &data,
                                      const QString &filename) {
  QFile file(filename);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug() << "Ошибка открытия файла для записи:" << filename;
    return "";
  }

  file.write(QJsonDocument(data).toJson());
  file.close();

  return filename;
}

void OSMDataLoader::waitData() {
  if (!reply) {
    qDebug() << "Нет активного запроса";
    return;
  }

  if (!reply->isFinished()) {
    QEventLoop loop;

    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    loop.exec();
  }

  qDebug() << "waitData(): запрос завершён";
}

double OSMDataLoader::radToLat(double radius) {
  return radius / (earthRadius * M_PI) * 180;
}
double OSMDataLoader::radToLon(double radius) {
  const double latRadians = defaultArea.lat * M_PI / 180.0;

  return radius / (earthRadius * std::cos(latRadians)) * 180.0 / M_PI;
}