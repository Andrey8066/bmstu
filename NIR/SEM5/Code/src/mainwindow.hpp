#pragma once

#include <QDoubleSpinBox>
#include <QMainWindow>
#include <QPushButton>

#include <glwidget.hpp>
#include <osm_data_loader.hpp>

class MainWindow : public QMainWindow {
public:
  explicit MainWindow(QWidget *parent = nullptr);

private:
  QDoubleSpinBox *latitudeInput;
  QDoubleSpinBox *longitudeInput;
  QDoubleSpinBox *radiusInput;
  QPushButton *loadButton;
  GLWidget *glWidget;
  OSMDataLoader *loader;

  void onLoadButtonClicked();
};