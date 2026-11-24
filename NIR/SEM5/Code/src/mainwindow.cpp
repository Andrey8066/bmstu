
#include "mainwindow.hpp"

#include "demreader.hpp"
#include "glwidget.hpp"
#include "osmdataloader.hpp"
#include "osmdataparser.hpp"
#include "projectserializer.hpp"

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
#include <QStandardPaths>
#include <QWidget>

#define defaultLatitude 55.766863
#define defaultLongitude 37.681677

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("3D City");
  resize(800, 500);

  auto *centralWidget = new QWidget(this);
  auto *mainLayout = new QHBoxLayout(centralWidget);

  auto *formLayout = new QFormLayout();

  demLoader = new OpenTopographyLoader(this);

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

  // Таймаут
  timeoutInput = new QDoubleSpinBox(this);
  timeoutInput->setRange(1.0, 600.0);
  timeoutInput->setDecimals(0);
  timeoutInput->setSingleStep(1.0);
  timeoutInput->setValue(1000.0);
  timeoutInput->setSuffix(" c");

  formLayout->addRow("Широта:", latitudeInput);
  formLayout->addRow("Долгота:", longitudeInput);
  formLayout->addRow("Радиус:", radiusInput);
  formLayout->addRow("Время ожидания:", timeoutInput);

  // OpenGL
  glWidget = new GLWidget(this);

  // Кнопки
  loadButton = new QPushButton("Загрузить область", this);

  auto *osmJsonButton = new QPushButton("Загрузить OSM JSON", this);

  auto *demButton = new QPushButton("Загрузить DEM", this);

  auto *saveProjectButton = new QPushButton("Сохранить проект", this);

  auto *openProjectButton = new QPushButton("Открыть проект", this);

  formLayout->addWidget(loadButton);
  formLayout->addWidget(osmJsonButton);
  formLayout->addWidget(demButton);
  formLayout->addWidget(saveProjectButton);
  formLayout->addWidget(openProjectButton);

  mainLayout->addLayout(formLayout);
  mainLayout->addWidget(glWidget, 1);

  setCentralWidget(centralWidget);

  // Подключение кнопок
  connect(loadButton, &QPushButton::clicked, this,
          &MainWindow::onLoadButtonClicked);

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
  connect(saveProjectButton, &QPushButton::clicked, this,
          &MainWindow::onSaveProjectButtonClicked);

  connect(openProjectButton, &QPushButton::clicked, this,
          &MainWindow::onOpenProjectButtonClicked);

  // Overpass API
  loader = new OSMDataLoader("https://overpass-api.de/api/interpreter",
                             defaultLatitude, defaultLongitude, 1000.0, this);

  connect(loader, &OSMDataLoader::dataLoaded, this,
          [this](const QJsonArray &elements, const QString &queryType) {
            Q_UNUSED(queryType);

            loadOSMFeatures(elements);
          });
  connect(demLoader, &OpenTopographyLoader::downloadFinished, this,
          [this](const QString &fileName) {
            qDebug() << "[DEM] Начинаем чтение GeoTIFF";

            DEMData dem;

            if (!DEMReader::loadDem(fileName, dem)) {
              QMessageBox::warning(this, "Ошибка DEM",
                                   "Файл скачан, но не удалось прочитать DEM.");
              return;
            }

            qDebug() << "[DEM] Размер:" << dem.width << "x" << dem.height;

            qDebug() << "[DEM] Высоты:" << dem.minElevation << "—"
                     << dem.maxElevation;

            glWidget->setDEM(dem);

            QMessageBox::information(
                this, "DEM", "Цифровая модель рельефа успешно загружена.");
          });

  connect(demLoader, &OpenTopographyLoader::downloadError, this,
          [this](const QString &error) {
            qDebug() << "[DEM] Ошибка:" << error;

            QMessageBox::warning(this, "Ошибка загрузки DEM", error);
          });
}

// Загрузка из Overpass API
void MainWindow::onLoadButtonClicked() {
  const double latitude = latitudeInput->value();
  const double longitude = longitudeInput->value();
  const double radius = radiusInput->value();
  const double timeout = timeoutInput->value();

  qDebug() << "Latitude:" << latitude;
  qDebug() << "Longitude:" << longitude;
  qDebug() << "Radius:" << radius;
  qDebug() << "Timeout:" << timeout;

  // ------------------------------------------------
  // 1. Загрузка OSM
  // ------------------------------------------------

  loader->setArea(latitude, longitude, radius);
  loader->fetchOSMData("all", timeout);

  // ------------------------------------------------
  // 2. Вычисление границ области для DEM
  // ------------------------------------------------

  constexpr double metersPerDegree = 111320.0;
  constexpr double pi = 3.14159265358979323846;

  const double latRad = latitude * pi / 180.0;

  const double latOffset = radius / metersPerDegree;
  const double cosLatitude = std::cos(latRad);

  if (std::abs(cosLatitude) < 1e-6) {
    QMessageBox::warning(
        this, "Ошибка координат",
        "Невозможно рассчитать область DEM для данной широты.");
    return;
  }

  const double lonOffset = radius / (metersPerDegree * cosLatitude);

  const double south = latitude - latOffset;
  const double north = latitude + latOffset;
  const double west = longitude - lonOffset;
  const double east = longitude + lonOffset;

  if (south < -90.0 || north > 90.0 || west < -180.0 || east > 180.0) {
    QMessageBox::warning(
        this, "Ошибка координат",
        "Рассчитанная область выходит за допустимые границы координат.");
    return;
  }

  qDebug() << "DEM bounds:";
  qDebug() << "South:" << south;
  qDebug() << "North:" << north;
  qDebug() << "West:" << west;
  qDebug() << "East:" << east;

  // ------------------------------------------------
  // 3. Подготовка пути сохранения DEM
  // ------------------------------------------------

  const QString dataDirectory =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
      "/dem";

  if (!QDir().mkpath(dataDirectory)) {
    QMessageBox::warning(this, "Ошибка",
                         "Не удалось создать каталог для сохранения DEM.");
    return;
  }

  const QString outputFile = dataDirectory + "/opentopography_eu_dtm.tif";

  // ------------------------------------------------
  // 4. Запуск загрузки DEM
  // ------------------------------------------------

  qDebug() << "Проверка DEM loader:" << demLoader;

  qDebug() << "Запускаем загрузку DEM...";

  demLoader->downloadDEM(south, north, west, east, outputFile);

  qDebug() << "Вызов downloadDEM выполнен";
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

  // Передаём все объекты в OpenGL
  glWidget->setFeatures(features);
}

void MainWindow::onSaveProjectButtonClicked() {
  const QString fileName = QFileDialog::getSaveFileName(
      this, "Сохранить проект", QString(), "Проекты 3D City (*.osm3d)");

  if (fileName.isEmpty())
    return;

  QString path = fileName;

  if (!path.endsWith(".osm3d", Qt::CaseInsensitive))
    path += ".osm3d";

  QString error;

  const bool success =
      ProjectSerializer::save(path, glWidget->getDEM(), glWidget->getFeatures(),
                              glWidget->getVerticalScale(), &error);

  if (!success) {
    QMessageBox::warning(this, "Ошибка сохранения", error);
    return;
  }

  QMessageBox::information(this, "Сохранение проекта",
                           "Проект успешно сохранён.");
}

void MainWindow::onOpenProjectButtonClicked() {
  const QString fileName = QFileDialog::getOpenFileName(
      this, "Открыть проект", QString(), "Проекты 3D City (*.osm3d)");

  if (fileName.isEmpty())
    return;

  DEMData dem;
  QVector<OSMFeature> features;
  float verticalScale = 0.12f;

  QString error;

  if (!ProjectSerializer::load(fileName, dem, features, verticalScale,
                               &error)) {
    QMessageBox::warning(this, "Ошибка открытия проекта", error);
    return;
  }

  // Сначала устанавливаем масштаб, затем данные сцены.
  glWidget->setVerticalScale(verticalScale);
  glWidget->setDEM(dem);
  glWidget->setFeatures(features);

  QMessageBox::information(this, "Открытие проекта",
                           "Проект успешно загружен.");
}