#include "mainwindow.hpp"
#include "osm_data_loader.hpp"

#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("3D City");
  resize(400, 250);

  auto *centralWidget = new QWidget(this);
  auto *mainLayout = new QHBoxLayout(centralWidget);

  auto *formLayout = new QFormLayout();

  // Широта
  latitudeInput = new QDoubleSpinBox(this);
  latitudeInput->setRange(-90.0, 90.0);
  latitudeInput->setDecimals(10);
  latitudeInput->setSingleStep(0.000001);
  latitudeInput->setValue(55.7558);

  // Долгота
  longitudeInput = new QDoubleSpinBox(this);
  longitudeInput->setRange(-180.0, 180.0);
  longitudeInput->setDecimals(10);
  longitudeInput->setSingleStep(0.000001);
  longitudeInput->setValue(37.6173);

  // Радиус в метрах
  radiusInput = new QDoubleSpinBox(this);
  radiusInput->setRange(1.0, 100000.0);
  radiusInput->setDecimals(0);
  radiusInput->setSingleStep(100.0);
  radiusInput->setValue(1000.0);
  radiusInput->setSuffix(" м");

  formLayout->addRow("Широта:", latitudeInput);
  formLayout->addRow("Долгота:", longitudeInput);
  formLayout->addRow("Радиус:", radiusInput);

  glWidget = new GLWidget(this);

  loadButton = new QPushButton("Загрузить область", this);
  formLayout->addWidget(loadButton);

  mainLayout->addLayout(formLayout);

  mainLayout->addWidget(glWidget, 1);

  connect(loadButton, &QPushButton::clicked, this,
          &MainWindow::onLoadButtonClicked);

  setCentralWidget(centralWidget);

  loader = new OSMDataLoader(
      "https://maps.mail.ru/osm/tools/overpass/api/interpreter");
}

void MainWindow::onLoadButtonClicked() {
  double latitude = latitudeInput->value();
  double longitude = longitudeInput->value();
  double radius = radiusInput->value();

  qDebug() << "Latitude:" << latitude;
  qDebug() << "Longitude:" << longitude;
  qDebug() << "Radius:" << radius;

  loader->setArea(latitude, longitude, radius);
  loader->fetchOSMData("all");

  loader->waitData();

  return;
}