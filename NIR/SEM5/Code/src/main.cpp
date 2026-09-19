#include "osm_data_loader.hpp"
#include <QCoreApplication>
#include <QDebug>
#include <QSettings>

int main(int argc, char *argv[]) {
  // Настройка логирования Qt через .ini конфиг
  QCoreApplication app(argc, argv);
  // https://overpass-api.de/api/interpreter
  OSMDataLoader loader(nullptr, "https://overpass-api.de/api/interpreter");

  qDebug() << "Загрузка данных OSM...";
  QByteArray buildings = loader.fetchOSMData("building");
  QByteArray highways = loader.fetchOSMData(QString("highway"));
  // QByteArray crosswalks = loader.fetchOSMData("footway");

  return app.exec();
}
