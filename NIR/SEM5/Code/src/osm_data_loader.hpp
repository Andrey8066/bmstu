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
  explicit OSMDataLoader(QObject *parent = nullptr);

  enum QueryType { Buildings, Roads, Crosswalks, AllElements };

signals:
  void dataLoaded(const QString &rawJSON);
  void errorOccurred(const QString &errorMessage);

public slots:
  // Загрузка данных из OpenStreetMap через Overpass API
  QString fetchOSMData(const QString &queryType = "all",
                       const QRect &area = QRect());

  OSMDataLoaderArea getDefaultArea() const;
  void setArea(const OSMDataLoaderArea &area);

  // Формирование Overpass QL запроса
  QString buildOverpassQuery(const QString &queryType);

  // Сохранение сырых данных в файл
  QString saveToJSONFile(const QString &data, const QString &filename);

private:
  QNetworkAccessManager *manager;
  OSMDataLoaderArea defaultArea;
};

#endif // OSM_DATA_LOADER_HPP