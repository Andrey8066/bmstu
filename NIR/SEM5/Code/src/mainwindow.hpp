
#pragma once

#include "opentopographyloader.hpp"
#include "roadclassifier.hpp"
#include "roadgraphbuilder.hpp"

#include <QJsonArray>
#include <QMainWindow>
#include <QSet>
#include <QVector>

class QDoubleSpinBox;
class QPushButton;
class QToolButton;
class QMenu;
class QAction;

class GLWidget;
class OSMDataLoader;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

private slots:
  void onLoadButtonClicked();
  void onLoadOsmJsonButtonClicked();
  void onSaveProjectButtonClicked();
  void onOpenProjectButtonClicked();

  void onRoadCategoryChanged();
  void onBuildGraphClicked();

private:
  void loadOSMFeatures(const QJsonArray &elements);

  void setupRoadMenu();
  void updateRoadMenuText();
  void updateRoadHighlight();

  QDoubleSpinBox *latitudeInput = nullptr;
  QDoubleSpinBox *longitudeInput = nullptr;
  QDoubleSpinBox *radiusInput = nullptr;
  QDoubleSpinBox *timeoutInput = nullptr;

  QPushButton *loadButton = nullptr;
  QPushButton *buildGraphButton = nullptr;

  QToolButton *roadCategoryButton = nullptr;
  QMenu *roadCategoryMenu = nullptr;

  QAction *allCategoriesAction = nullptr;
  QVector<QAction *> categoryActions;

  GLWidget *glWidget = nullptr;
  OSMDataLoader *loader = nullptr;
  OpenTopographyLoader *demLoader = nullptr;

  // Исходные OSM-объекты
  QVector<OSMFeature> osmFeatures;

  // Результат классификации
  QVector<RoadClassifier::ClassifiedRoad> classifiedRoads;

  // Выбранные категории
  QSet<int> selectedCategories;

  // Построенный граф
  RoadGraph roadGraph;
};
