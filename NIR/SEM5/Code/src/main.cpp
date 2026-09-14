#include "osm_data_loader.hpp"
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  OSMDataLoader loader;

  // Пример загрузки данных для зданий в центре Москвы
  QString query = R"(
        [out:json][timeout:30];
        node["building"]({{bbox}});
        way["building"]({{bbox}});
        relation["building"]({{bbox}});
        out center;
    )";

  qDebug() << "Загрузка данных OSM...";
  QString url = loader.fetchOSMData("buildings");
  qDebug() << "URL запроса:" << url;

  // Пример формирования запроса для всех элементов
  QString allQuery = loader.buildOverpassQuery("all");
  qDebug() << "Весь запрос:" << allQuery;

  

  return a.exec();
}