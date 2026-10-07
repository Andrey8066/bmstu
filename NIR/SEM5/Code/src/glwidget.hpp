#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLWidget>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
public:
  explicit GLWidget(QWidget *parent = nullptr);

protected:
  void initializeGL() override;
  void resizeGL(int width, int height) override;
  void paintGL() override;
};