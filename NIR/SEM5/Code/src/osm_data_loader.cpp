#include "osm_data_loader.hpp"

#include <QDebug>
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

OSMDataLoader::OSMDataLoader(QObject *parent, const QString apiUrl)
    : QObject(parent), manager(new QNetworkAccessManager(this)), url(apiUrl) {

  // Координаты центра карты по умолчанию (Москва)
  defaultArea.lat = 55.766863; // 55.766863, 37.681677
  defaultArea.lon = 37.681677;
  defaultArea.width = 0.002;  // +/- 0.2 градуса по широте
  defaultArea.height = 0.003; // +/- 0.3 градуса по долготе

  connect(this, &OSMDataLoader::dataLoaded, this,
          [&](const QJsonArray &data, const QString &queryType) {
            saveToJSONFile(data, QString("osm_%1.json").arg(queryType));
          });
}

QByteArray OSMDataLoader::fetchOSMData(const QString &queryType,
                                       const int &timeout, const QRect &area) {
  Q_UNUSED(area);

  // Формируем Overpass QL
  QString finalQuery = buildOverpassQuery(timeout, queryType);

  if (finalQuery.isEmpty()) {
    qDebug() << "Неизвестный тип запроса:" << queryType;
    return "";
  }

  // Границы области:
  // south, west, north, east
  const double minLat = defaultArea.lat - defaultArea.width;
  const double maxLat = defaultArea.lat + defaultArea.width;
  const double minLon = defaultArea.lon - defaultArea.height;
  const double maxLon = defaultArea.lon + defaultArea.height;

  const QString bbox = QString("%1,%2,%3,%4")
                           .arg(minLat, 0, 'f', 6)
                           .arg(minLon, 0, 'f', 6)
                           .arg(maxLat, 0, 'f', 6)
                           .arg(maxLon, 0, 'f', 6);

  // Подставляем реальные координаты вместо {{bbox}}
  finalQuery.replace("{{bbox}}", bbox);

  qDebug() << "Тип запроса:" << queryType;
  qDebug() << "BBox:" << bbox;
  qDebug() << "Весь запрос:" << finalQuery;

  // Адрес Overpass API
  // const QUrl url("https://overpass-api.de/api/interpreter");
  // const QUrl url("https://maps.mail.ru/osm/tools/overpass/api/interpreter");

  QNetworkRequest request(url);

  request.setHeader(QNetworkRequest::ContentTypeHeader,
                    "application/x-www-form-urlencoded");

  // ВАЖНО: Overpass API отклоняет запросы без User-Agent (HTTP 406).
  // Qt не устанавливает User-Agent автоматически, поэтому задаём его явно.
  request.setHeader(
      QNetworkRequest::UserAgentHeader,
      "BMSTU-NIR-OSM-Loader/1.0 (+https://github.com/Andrey8066)");

  // Явно сообщаем, что ожидаем JSON — некоторые инстансы Overpass
  // проверяют Accept и иначе могут вернуть 406.
  request.setRawHeader("Accept", "application/json");

  QByteArray postData = "data=";
  postData += QUrl::toPercentEncoding(finalQuery);

  QNetworkReply *reply = manager->post(request, postData);

  connect(reply, &QNetworkReply::finished, this,
          [this, area, reply, queryType]() {
            const QVariant status =
                reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

            qDebug() << "HTTP status:" << status;

            if (reply->error() != QNetworkReply::NoError) {

              qDebug() << "Ошибка загрузки OSM данных:" << reply->errorString();

              const QByteArray serverResponse = reply->readAll();

              qDebug() << "Ответ сервера:" << serverResponse;

              emit errorOccurred("Не удалось загрузить данные из OSM");

              reply->deleteLater();
              return;
            }

            requestResult = reply->readAll();

            QJsonParseError parseError;

            const QJsonDocument doc =
                QJsonDocument::fromJson(requestResult, &parseError);

            if (parseError.error != QJsonParseError::NoError) {

              qDebug() << "Ошибка парсинга JSON:" << parseError.errorString();

              qDebug() << "Ответ сервера:" << requestResult;

              reply->deleteLater();
              return;
            }

            qDebug() << "OSM JSON успешно получен.";

            qDebug() << "Получено байт:" << requestResult.size();
            qDebug().noquote() << "Ответ сервера:";
            qDebug().noquote() << QString::fromUtf8(requestResult);

            const QJsonObject root = doc.object();
            const QJsonArray elements = root["elements"].toArray();

            emit dataLoaded(elements, queryType);

            qDebug() << "Всего OSM объектов:" << elements.size();
          });

  return requestResult;
}

OSMDataLoaderArea OSMDataLoader::getDefaultArea() const { return defaultArea; }

void OSMDataLoader::setArea(const OSMDataLoaderArea &area) {
  defaultArea = area;
}

// Формирование Overpass QL запроса - правильная syntax
QString OSMDataLoader::buildOverpassQuery(const int &timeout,
                                          const QString &queryType) {
  if (queryType == "all") {
    return QString(R"([out:json][timeout: %1];
    (
node["building"]({{bbox}});
way["building"]({{bbox}});
relation["building"]({{bbox}});
    );
out geom;)")
        .arg(timeout);
  } else {
    return QString(R"([out:json][timeout: %1];
    (
node["%2"]({{bbox}});
way["%2"]({{bbox}});
relation["%2"]({{bbox}});
    );
out geom;)")
        .arg(timeout)
        .arg(queryType);
  }
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
