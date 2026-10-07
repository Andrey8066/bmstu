#include "dem_reader.hpp"
#include "osm_data_loader.hpp"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QSettings>
#include <mainwindow.hpp>

int main(int argc, char *argv[]) {
  // Настройка логирования Qt через .ini конфиг
  QApplication app(argc, argv);

  // https://overpass-api.de/api/interpreter
  // OSMDataLoader loader(nullptr, "https://overpass-api.de/api/interpreter");

  // qDebug() << "Загрузка данных OSM...";
  // QByteArray buildings = loader.fetchOSMData("all");
  // QByteArray highways = loader.fetchOSMData(QString("highway"));
  // QByteArray crosswalks = loader.fetchOSMData("footway");

  MainWindow window;
  window.show();

  // DEMReader demReader;
  // demReader.loadDem("/home/andrey/Documents/BMSTU/NIR/SEM5/Code/rasters_GEDTM30/output_be.tif");

  return app.exec();
}
