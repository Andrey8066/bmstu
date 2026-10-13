#include "dem_reader.hpp"
#include "osm_data_loader.hpp"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QSettings>
#include <mainwindow.hpp>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  MainWindow window;
  window.show();

  return app.exec();
}
