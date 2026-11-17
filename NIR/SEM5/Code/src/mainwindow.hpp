
#pragma once

#include "demreader.hpp"
#include "opentopographyloader.hpp"
#include <QJsonArray>
#include <QMainWindow>

class QDoubleSpinBox;
class QPushButton;
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

private:
  void loadOSMFeatures(const QJsonArray &elements);

  QDoubleSpinBox *latitudeInput = nullptr;
  QDoubleSpinBox *longitudeInput = nullptr;
  QDoubleSpinBox *radiusInput = nullptr;
  QDoubleSpinBox *timeoutInput = nullptr;

  QPushButton *loadButton = nullptr;

  GLWidget *glWidget = nullptr;
  OSMDataLoader *loader = nullptr;
  OpenTopographyLoader *demLoader = nullptr;
};
