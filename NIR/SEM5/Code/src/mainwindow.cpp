
#include "mainwindow.hpp"

#include "demreader.hpp"
#include "glwidget.hpp"
#include "osmdataloader.hpp"
#include "osmdataparser.hpp"

#include <QDebug>
#include <QDoubleSpinBox>
#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMessageBox>
#include <QPushButton>
#include <QWidget>

#define defaultLatitude 55.766863
#define defaultLongitude 37.681677

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("3D City");
  resize(800, 500);

  auto *centralWidget = new QWidget(this);
  auto *mainLayout = new QHBoxLayout(centralWidget);

  auto *formLayout = new QFormLayout();

  // Широта
  latitudeInput = new QDoubleSpinBox(this);
  latitudeInput->setRange(-90.0, 90.0);
  latitudeInput->setDecimals(10);
  latitudeInput->setSingleStep(0.000001);
  latitudeInput->setValue(defaultLatitude);

  // Долгота
  longitudeInput = new QDoubleSpinBox(this);
  longitudeInput->setRange(-180.0, 180.0);
  longitudeInput->setDecimals(10);
  longitudeInput->setSingleStep(0.000001);
  longitudeInput->setValue(defaultLongitude);

  // Радиус
  radiusInput = new QDoubleSpinBox(this);
  radiusInput->setRange(1.0, 100000.0);
  radiusInput->setDecimals(0);
  radiusInput->setSingleStep(100.0);
  radiusInput->setValue(1000.0);
  radiusInput->setSuffix(" м");

  formLayout->addRow("Широта:", latitudeInput);
  formLayout->addRow("Долгота:", longitudeInput);
  formLayout->addRow("Радиус:", radiusInput);

  // OpenGL
  glWidget = new GLWidget(this);

  // Кнопки
  loadButton = new QPushButton("Загрузить область", this);

  auto *osmJsonButton = new QPushButton("Загрузить OSM JSON", this);

  auto *demButton = new QPushButton("Загрузить DEM", this);

  formLayout->addWidget(loadButton);
  formLayout->addWidget(osmJsonButton);
  formLayout->addWidget(demButton);

  mainLayout->addLayout(formLayout);
  mainLayout->addWidget(glWidget, 1);

  setCentralWidget(centralWidget);

  // Подключение кнопок
  connect(loadButton, &QPushButton::clicked, this,
          &MainWindow::onLoadOsmButtonClicked);

  connect(osmJsonButton, &QPushButton::clicked, this,
          &MainWindow::onLoadOsmJsonButtonClicked);

  // DEM
  connect(demButton, &QPushButton::clicked, this, [this]() {
    const QString fileName = QFileDialog::getOpenFileName(
        this, "Выбрать цифровую модель рельефа", QString(),
        "DEM (*.tif *.tiff);;Все файлы (*)");

    if (fileName.isEmpty())
      return;

    DEMData dem;

    if (!DEMReader::loadDem(fileName, dem)) {
      QMessageBox::warning(this, "Ошибка", "Не удалось загрузить DEM-файл.");
      return;
    }

    qDebug() << "DEM:" << dem.width << "x" << dem.height;

    qDebug() << "Высоты:" << dem.minElevation << "—" << dem.maxElevation;

    glWidget->setDEM(dem);
  });

  // Overpass API
  loader = new OSMDataLoader("https://overpass-api.de/api/interpreter",
                             defaultLatitude, defaultLongitude, 1000.0, this);

  connect(loader, &OSMDataLoader::dataLoaded, this,
          [this](const QJsonArray &elements, const QString &queryType) {
            Q_UNUSED(queryType);

            loadOSMFeatures(elements);
          });
}

// Загрузка из Overpass API
void MainWindow::onLoadOsmButtonClicked() {
  const double latitude = latitudeInput->value();
  const double longitude = longitudeInput->value();
  const double radius = radiusInput->value();

  qDebug() << "Latitude:" << latitude;
  qDebug() << "Longitude:" << longitude;
  qDebug() << "Radius:" << radius;

  loader->setArea(latitude, longitude, radius);

  // Пока загружаем только здания.
  // Запрос можно расширить для других типов объектов.
  loader->fetchOSMData("buildings", 60);
}

// Загрузка локального JSON
void MainWindow::onLoadOsmJsonButtonClicked() {
  const QString fileName =
      QFileDialog::getOpenFileName(this, "Выбрать файл OSM JSON", QString(),
                                   "JSON files (*.json);;Все файлы (*)");

  if (fileName.isEmpty())
    return;

  QFile file(fileName);

  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл OSM JSON.");
    return;
  }

  const QByteArray jsonData = file.readAll();
  file.close();

  QJsonParseError parseError;

  const QJsonDocument document = QJsonDocument::fromJson(jsonData, &parseError);

  if (parseError.error != QJsonParseError::NoError) {

    QMessageBox::warning(this, "Ошибка JSON", parseError.errorString());

    return;
  }

  QJsonArray elements;

  // Формат с корневым массивом
  if (document.isArray()) {

    elements = document.array();
  }
  // Стандартный формат Overpass
  else if (document.isObject()) {

    const QJsonObject root = document.object();

    if (!root.value("elements").isArray()) {

      QMessageBox::warning(this, "Ошибка",
                           "В JSON отсутствует массив elements.");

      return;
    }

    elements = root.value("elements").toArray();
  } else {

    QMessageBox::warning(this, "Ошибка", "Некорректная структура OSM JSON.");

    return;
  }

  if (elements.isEmpty()) {

    QMessageBox::information(this, "Информация",
                             "В файле отсутствуют OSM-объекты.");

    return;
  }

  loadOSMFeatures(elements);
}

// Общая загрузка объектов OSM
void MainWindow::loadOSMFeatures(const QJsonArray &elements) {
  const QVector<OSMFeature> features = OSMDataParser::parse(elements);

  if (features.isEmpty()) {

    QMessageBox::information(this, "Информация",
                             "Не удалось найти поддерживаемые OSM-объекты.");

    return;
  }

  // Статистика
  int buildings = 0;
  int roads = 0;
  int water = 0;
  int vegetation = 0;
  int other = 0;

  for (const OSMFeature &feature : features) {

    switch (feature.type) {

    case OSMFeatureType::Building:
      ++buildings;
      break;

    case OSMFeatureType::Road:
      ++roads;
      break;

    case OSMFeatureType::Water:
      ++water;
      break;

    case OSMFeatureType::Vegetation:
      ++vegetation;
      break;

    default:
      ++other;
      break;
    }
  }

  qDebug() << "Загружено OSM объектов:" << features.size();

  qDebug() << "Здания:" << buildings;
  qDebug() << "Дороги:" << roads;
  qDebug() << "Вода:" << water;
  qDebug() << "Растительность:" << vegetation;
  qDebug() << "Другие:" << other;

  // Передаём все объекты в OpenGL
  glWidget->setFeatures(features);
}