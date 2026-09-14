#include "osm_data_loader.hpp"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonArray>

OSMDataLoader::OSMDataLoader(QObject *parent)
    : QObject(parent),
      manager(new QNetworkAccessManager(this)) {

  // Координаты центра карты по умолчанию (Москва)
  defaultArea.lat = 55.766863; //55.766863, 37.681677
  defaultArea.lon = 37.681677;
  defaultArea.width = 0.002;   // +/- 0.2 градуса по широте
  defaultArea.height = 0.003;  // +/- 0.3 градуса по долготе
}

QString OSMDataLoader::fetchOSMData(const QString &queryType,
                                    const QRect &area)
{
    Q_UNUSED(area);

    // Формируем Overpass QL
    QString finalQuery = buildOverpassQuery(queryType);

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
    const QUrl url("https://maps.mail.ru/osm/tools/overpass/api/interpreter"); 
    

   QNetworkRequest request(url);

request.setHeader(
    QNetworkRequest::ContentTypeHeader,
    "application/x-www-form-urlencoded"
);

QByteArray postData = "data=";
postData += QUrl::toPercentEncoding(finalQuery);

QNetworkReply *reply = manager->post(request, postData);

    connect(reply, &QNetworkReply::finished,
            this, [this, area, reply]() {

        const QVariant status =
            reply->attribute(
                QNetworkRequest::HttpStatusCodeAttribute);

        qDebug() << "HTTP status:" << status;

        if (reply->error() != QNetworkReply::NoError) {

            qDebug() << "Ошибка загрузки OSM данных:"
                     << reply->errorString();

            const QByteArray serverResponse = reply->readAll();

            qDebug() << "Ответ сервера:"
                     << serverResponse;

            emit errorOccurred(
                "Не удалось загрузить данные из OSM");

            reply->deleteLater();
            return;
        }

        const QByteArray data = reply->readAll();

        qDebug() << "Получено байт:" << data.size();

        QJsonParseError parseError;

        const QJsonDocument doc =
            QJsonDocument::fromJson(data, &parseError);

        if (parseError.error != QJsonParseError::NoError) {

            qDebug() << "Ошибка парсинга JSON:"
                     << parseError.errorString();

            qDebug() << "Ответ сервера:" << data;

            reply->deleteLater();
            return;
        }

        qDebug() << "OSM JSON успешно получен.";

        emit dataLoaded(doc.toJson());


qDebug() << "Получено байт:" << data.size();
qDebug().noquote() << "Ответ сервера:";
qDebug().noquote() << QString::fromUtf8(data);

saveToJSONFile(data, filePath);
const QJsonObject root = doc.object();
const QJsonArray elements = root["elements"].toArray();

qDebug() << "Всего OSM объектов:" << elements.size();
    });

    // Возвращаем URL API, поскольку функция имеет QString
    // в качестве возвращаемого типа.
    
    return url.toString();
}

OSMDataLoaderArea OSMDataLoader::getDefaultArea() const {
  return defaultArea;
}

void OSMDataLoader::setArea(const OSMDataLoaderArea &area) {
  defaultArea = area;
}

// Формирование Overpass QL запроса - правильная syntax
QString OSMDataLoader::buildOverpassQuery(const QString &queryType) {
if (queryType == "buildings") {
  return R"([out:json][timeout:30];
node["building"](building:height,levels,storeys)({{bbox}});
way["building"](building:height,levels,storeys)({{bbox}});
relation["building"](building:height,levels,storeys)({{bbox}});
out geom;)";
  } else if (queryType == "roads") {
    return R"([out:json][timeout:30];
node["highway"]({{bbox}});
way["highway"]({{bbox}});
relation["highway"]({{bbox}});
out center;)";
  } else if (queryType == "crosswalks") {
    return R"([out:json][timeout:30];
node["highway=crossing"]({{bbox}});
way["highway=crossing"]({{bbox}});
relation["highway=crossing"]({{bbox}});
out center;)";
  } else if (queryType == "all") {
    return R"([out:json][timeout:30];
node({{bbox}});
way({{bbox}});
relation({{bbox}});
out body;)";
  }

  return "";
}

// Сохранение JSON в файл
QString OSMDataLoader::saveToJSONFile(const QString &data,
                                      const QString &filename) {
  QFile file(filename);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug() << "Ошибка открытия файла для записи:"
             << filename;
    return "";
  }

  file.write(data.toUtf8());
  file.close();

  return filename;
}

