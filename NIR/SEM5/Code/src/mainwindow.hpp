
#pragma once

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
  void onLoadOsmButtonClicked();
  void onLoadOsmJsonButtonClicked();

private:
  void loadOSMFeatures(const QJsonArray &elements);

  QDoubleSpinBox *latitudeInput = nullptr;
  QDoubleSpinBox *longitudeInput = nullptr;
  QDoubleSpinBox *radiusInput = nullptr;

  QPushButton *loadButton = nullptr;

  GLWidget *glWidget = nullptr;
  OSMDataLoader *loader = nullptr;
};
