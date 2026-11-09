#include "mainwindow.hpp"
#include "dem_reader.hpp"
#include "osm_data_loader.hpp"
#include <QDebug>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

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
          &MainWindow::onLoadOsmButtonClicked);

  setCentralWidget(centralWidget);

  loader = new OSMDataLoader("https://overpass-api.de/api/interpreter", 55.7558,
                             37.6173, 1000.0, this);

  auto *demButton = new QPushButton("Загрузить DEM", this);
  formLayout->addWidget(demButton);

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

    // Следующий шаг: передать dem в GLWidget.
    glWidget->setDEM(dem);
  });
  connect(loader, &OSMDataLoader::dataLoaded, this,
          [this](const QJsonArray &elements, const QString &queryType) {
            if (queryType != "buildings")
              return;

            std::vector<BuildingFootprint> buildingList;

            for (const QJsonValue &value : elements) {
              const QJsonObject object = value.toObject();

              const QJsonObject tags = object["tags"].toObject();

              if (!tags.contains("building") ||
                  tags["building"].toString() == "no")
                continue;

              const QJsonArray geometry = object["geometry"].toArray();

              if (geometry.size() < 4)
                continue;

              BuildingFootprint building;

              // 1. Получаем высоту из building:height / height.
              bool heightOk = false;
              double buildingHeight = 0.0;

              const QJsonValue heightValue = tags.value("height");

              if (heightValue.isDouble()) {
                buildingHeight = heightValue.toDouble();
                heightOk = true;
              } else {
                QString heightText = heightValue.toString().trimmed().toLower();

                const bool feet = heightText.endsWith("ft");

                if (feet)
                  heightText.chop(2);
                else if (heightText.endsWith("m"))
                  heightText.chop(1);

                buildingHeight = heightText.trimmed().toDouble(&heightOk);

                if (feet && heightOk)
                  buildingHeight *= 0.3048;
              }

              // 2. Если высота неизвестна, используем этажность.
              if (!heightOk || buildingHeight <= 0.0) {
                const QJsonValue levelsValue = tags.value("building:levels");

                double levels = 0.0;

                if (levelsValue.isDouble()) {
                  levels = levelsValue.toDouble();
                  heightOk = true;
                } else {
                  levels = levelsValue.toString().toDouble(&heightOk);
                }

                if (heightOk && levels > 0.0)
                  buildingHeight = levels * 3.0;
                else
                  buildingHeight = 9.0;
              }

              building.heightMeters =
                  static_cast<float>(std::clamp(buildingHeight, 2.0, 300.0));

              // 3. Получаем географические координаты контура.
              for (const QJsonValue &pointValue : geometry) {
                const QJsonObject point = pointValue.toObject();

                if (!point.contains("lat") || !point.contains("lon"))
                  continue;

                building.outline.push_back(
                    {point["lat"].toDouble(), point["lon"].toDouble()});
              }

              // Удаляем повтор первой точки в конце контура.
              if (building.outline.size() >= 2) {
                const auto &first = building.outline.front();
                const auto &last = building.outline.back();

                if (std::abs(first.lat - last.lat) < 1e-10 &&
                    std::abs(first.lon - last.lon) < 1e-10) {
                  building.outline.pop_back();
                }
              }

              if (building.outline.size() < 3)
                continue;

              buildingList.push_back(std::move(building));
            }

            qDebug() << "Получено зданий:" << buildingList.size();

            // Передаём геометрию в OpenGL.
            glWidget->setBuildings(buildingList);
          });
}

void MainWindow::onLoadOsmButtonClicked() {
  double latitude = latitudeInput->value();
  double longitude = longitudeInput->value();
  double radius = radiusInput->value();

  qDebug() << "Latitude:" << latitude;
  qDebug() << "Longitude:" << longitude;
  qDebug() << "Radius:" << radius;

  loader->setArea(latitude, longitude, radius);
  loader->fetchOSMData("buildings", 60);

  return;
}
