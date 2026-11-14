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

  QNetworkRequest request(url);

  request.setHeader(QNetworkRequest::ContentTypeHeader,
                    "application/x-www-form-urlencoded");
  request.setHeader(
      QNetworkRequest::UserAgentHeader,
      "BMSTU-NIR-OSM-Loader/1.0 (+https://github.com/Andrey8066)");
  request.setRawHeader("Accept", "application/json");

  QByteArray postData = "data=";
  postData += QUrl::toPercentEncoding(finalQuery);

  reply = manager->post(request, postData);

  connect(reply, &QNetworkReply::finished, this, [this, area, queryType]() {
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

    const QJsonObject root = doc.object();
    const QJsonArray elements = root["elements"].toArray();

    emit dataLoaded(elements, queryType);
    int ways = 0;
    int relations = 0;
    int nodes = 0;

    int buildings = 0;
    int buildingParts = 0;

    for (const QJsonValue &value : elements) {
      QJsonObject object = value.toObject();
      QJsonObject tags = object["tags"].toObject();

      QString type = object["type"].toString();

      if (type == "way")
        ways++;

      if (type == "relation")
        relations++;

      if (type == "node")
        nodes++;

      if (tags.contains("building"))
        buildings++;

      if (tags.contains("building:part"))
        buildingParts++;
    }

    qDebug() << "========== OSM STATISTICS ==========";
    qDebug() << "Total elements:" << elements.size();
    qDebug() << "Ways:" << ways;
    qDebug() << "Relations:" << relations;
    qDebug() << "Nodes:" << nodes;
    qDebug() << "Building tags:" << buildings;
    qDebug() << "Building parts:" << buildingParts;

    int wayBuildings = 0;
    int relationBuildings = 0;

    int wayParts = 0;
    int relationParts = 0;

    for (const QJsonValue &value : elements) {
      QJsonObject object = value.toObject();
      QJsonObject tags = object["tags"].toObject();

      QString type = object["type"].toString();

      bool hasBuilding = tags.contains("building");
      bool hasPart = tags.contains("building:part");

      if (type == "way" && hasBuilding)
        wayBuildings++;

      if (type == "relation" && hasBuilding)
        relationBuildings++;

      if (type == "way" && hasPart)
        wayParts++;

      if (type == "relation" && hasPart)
        relationParts++;
    }

    qDebug() << "Way buildings:" << wayBuildings;
    qDebug() << "Relation buildings:" << relationBuildings;

    qDebug() << "Way building parts:" << wayParts;
    qDebug() << "Relation building parts:" << relationParts;
  });

  return requestResult;
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
                   nwr["highway"]({{bbox}});
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