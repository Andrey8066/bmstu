#ifndef OSM_DATA_LOADER_HPP
#define OSM_DATA_LOADER_HPP

#include <QNetworkReply>
#include <QObject>
#include <QRect>
#include <QString>

struct OSMDataLoaderArea {
  double lat;
  double lon;
  double width;  // широта +/-
  double height; // долгота +/-
};

class OSMDataLoader : public QObject {
  Q_OBJECT

public:

  QString filePath = "osm.json";
  explicit OSMDataLoader(QObject *parent = nullptr, const QString apiUrl = "https://maps.mail.ru/osm/tools/overpass/api/interpreter");

  enum QueryType { Buildings, Roads, Crosswalks, AllElements };

signals:
  void dataLoaded(const QJsonArray &rawJSON, const QString &queryType);
  void errorOccurred(const QString &errorMessage);

public slots:
  // Загрузка данных из OpenStreetMap через Overpass API
  QByteArray fetchOSMData(const QString &queryType = "all",
                       const int &timeout = 30, 
                       const QRect &area = QRect());

  OSMDataLoaderArea getDefaultArea() const;
  void setArea(const OSMDataLoaderArea &area);

  // Формирование Overpass QL запроса
  QString buildOverpassQuery(const int &timout, const QString &queryType);

  // Сохранение сырых данных в файл
  QString saveToJSONFile(const QJsonArray &data, const QString &filename);

private:
  QNetworkAccessManager *manager;
  OSMDataLoaderArea defaultArea;
  QByteArray requestResult;
  QUrl url;

};

#endif // OSM_DATA_LOADER_HPP