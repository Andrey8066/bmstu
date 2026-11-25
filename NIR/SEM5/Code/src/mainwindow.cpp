
#include "mainwindow.hpp"

#include "demreader.hpp"
#include "glwidget.hpp"
#include "osmdataloader.hpp"
#include "osmdataparser.hpp"
#include "projectserializer.hpp"
#include "roadcategory.hpp"
#include "roadgraphwindow.hpp"

#include <QAction>
#include <QDebug>
#include <QMenu>
#include <QSignalBlocker>
#include <QToolButton>

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

  setupRoadMenu();

  // Кнопка построения графа
  buildGraphButton = new QPushButton("Построить граф", this);

  formLayout->addRow("Категории дорог:", roadCategoryButton);
  formLayout->addWidget(buildGraphButton);

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

  connect(buildGraphButton, &QPushButton::clicked, this,
          &MainWindow::onBuildGraphClicked);
}

// Загрузка из Overpass API
void MainWindow::onLoadButtonClicked() {
  const double latitude = latitudeInput->value();
  const double longitude = longitudeInput->value();
  const double radius = radiusInput->value();
  const double timeout = timeoutInput->value();

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

  demLoader->downloadDEM(south, north, west, east, outputFile);
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
  osmFeatures = OSMDataParser::parse(elements);

  if (osmFeatures.isEmpty()) {
    QMessageBox::information(this, "Информация",
                             "Не удалось найти поддерживаемые OSM-объекты.");

    return;
  }

  // Передаём исходные объекты в OpenGL
  glWidget->setFeatures(osmFeatures);

  // Выполняем классификацию один раз
  RoadClassifier classifier;

  classifiedRoads = classifier.classify(osmFeatures);

  qDebug() << "[RoadClassifier]"
           << "Всего объектов:" << osmFeatures.size()
           << "Дорог:" << classifiedRoads.size();

  // Сбрасываем построенный граф
  roadGraph = RoadGraph();

  // Сбрасываем фильтр на все категории
  {
    const QSignalBlocker blocker(roadCategoryMenu);

    allCategoriesAction->setChecked(true);

    selectedCategories.clear();

    for (int i = 0; i < categoryActions.size(); ++i) {
      categoryActions[i]->setChecked(true);
      selectedCategories.insert(i);
    }
  }

  updateRoadMenuText();
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

void MainWindow::setupRoadMenu() {
  roadCategoryButton = new QToolButton(this);
  roadCategoryButton->setText("Все категории");
  roadCategoryButton->setPopupMode(QToolButton::InstantPopup);

  roadCategoryButton->setToolButtonStyle(Qt::ToolButtonTextOnly);

  roadCategoryMenu = new QMenu(roadCategoryButton);

  roadCategoryButton->setMenu(roadCategoryMenu);

  // Все категории
  allCategoriesAction = roadCategoryMenu->addAction("Все категории");

  allCategoriesAction->setCheckable(true);
  allCategoriesAction->setChecked(true);

  roadCategoryMenu->addSeparator();

  const QStringList names = {
      "Пешеходные дороги",          "Автомобильные — 1 полоса",
      "Автомобильные — 2 полосы",   "Автомобильные — 3 полосы",
      "Автомобильные — 4 полосы",   "Автомобильные — 5+ полос",
      "Автомобильные — неизвестно", "Автомагистрали"};

  for (int i = 0; i < names.size(); ++i) {
    QAction *action = roadCategoryMenu->addAction(names[i]);

    action->setCheckable(true);
    action->setChecked(true);
    action->setData(i);

    categoryActions.append(action);
  }

  // По умолчанию выбраны все категории
  for (int i = 0; i < categoryActions.size(); ++i) {
    selectedCategories.insert(i);
  }

  connect(allCategoriesAction, &QAction::triggered, this,
          &MainWindow::onRoadCategoryChanged);

  for (QAction *action : categoryActions) {
    connect(action, &QAction::triggered, this,
            &MainWindow::onRoadCategoryChanged);
  }
}

void MainWindow::onRoadCategoryChanged() {
  QAction *action = qobject_cast<QAction *>(sender());

  if (!action)
    return;

  if (action == allCategoriesAction) {
    bool checked = allCategoriesAction->isChecked();

    selectedCategories.clear();

    for (int i = 0; i < categoryActions.size(); ++i) {
      categoryActions[i]->setChecked(checked);

      if (checked)
        selectedCategories.insert(i);
    }
  } else {
    int index = action->data().toInt();

    if (action->isChecked())
      selectedCategories.insert(index);
    else
      selectedCategories.remove(index);

    bool allSelected = selectedCategories.size() == categoryActions.size();

    allCategoriesAction->setChecked(allSelected);
  }

  updateRoadMenuText();

  // Немедленно обновляем подсветку
  updateRoadHighlight();
}

void MainWindow::updateRoadHighlight() {
  QSet<qint64> highlightedIds;

  const QVector<RoadCategory> categories = {
      RoadCategory::Pedestrian,      RoadCategory::Car1Lane,
      RoadCategory::Car2Lanes,       RoadCategory::Car3Lanes,
      RoadCategory::Car4Lanes,       RoadCategory::Car5PlusLanes,
      RoadCategory::CarUnknownLanes, RoadCategory::Motorway};

  for (const auto &road : classifiedRoads) {
    for (int index : selectedCategories) {
      if (index < 0 || index >= categories.size())
        continue;

      if (road.category == categories[index]) {
        highlightedIds.insert(road.feature.getId());
        break;
      }
    }
  }

  glWidget->setHighlightedFeatures(highlightedIds);
}

void MainWindow::updateRoadMenuText() {
  if (allCategoriesAction->isChecked()) {
    roadCategoryButton->setText("Все категории");
  } else if (selectedCategories.isEmpty()) {
    roadCategoryButton->setText("Категории не выбраны");
  } else {
    roadCategoryButton->setText(
        QString("Выбрано категорий: %1").arg(selectedCategories.size()));
  }
}

void MainWindow::onBuildGraphClicked() {
  if (classifiedRoads.isEmpty()) {
    QMessageBox::warning(this, "Построение графа",
                         "Сначала загрузите дорожные данные.");
    return;
  }

  if (selectedCategories.isEmpty()) {
    QMessageBox::warning(this, "Построение графа",
                         "Выберите хотя бы одну категорию дорог.");
    return;
  }

  const QVector<RoadCategory> categories = {
      RoadCategory::Pedestrian,      RoadCategory::Car1Lane,
      RoadCategory::Car2Lanes,       RoadCategory::Car3Lanes,
      RoadCategory::Car4Lanes,       RoadCategory::Car5PlusLanes,
      RoadCategory::CarUnknownLanes, RoadCategory::Motorway};

  QVector<RoadClassifier::ClassifiedRoad> roadsToBuild;

  for (const auto &road : classifiedRoads) {

    int categoryIndex = -1;

    for (int i = 0; i < categories.size(); ++i) {
      if (road.category == categories[i]) {
        categoryIndex = i;
        break;
      }
    }

    if (categoryIndex >= 0 && selectedCategories.contains(categoryIndex)) {
      roadsToBuild.append(road);
    }
  }

  if (roadsToBuild.isEmpty()) {
    QMessageBox::information(this, "Построение графа",
                             "В выбранных категориях дороги не найдены.");
    return;
  }

  RoadGraphBuilder builder;

  roadGraph = builder.build(roadsToBuild);

  qDebug() << "[RoadGraph]"
           << "Исходных дорог:" << roadsToBuild.size();

  QMessageBox::information(
      this, "Построение графа",
      QString("Граф построен.\nОбработано дорожных объектов: %1")
          .arg(roadsToBuild.size()));

  // Открываем отдельное окно.
  auto *window = new RoadGraphWindow(roadGraph);
  window->setAttribute(Qt::WA_DeleteOnClose);
  window->show();
}
