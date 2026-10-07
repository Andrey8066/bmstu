#include "glwidget.hpp"

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent) {}

void GLWidget::initializeGL() {
  initializeOpenGLFunctions();

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void GLWidget::resizeGL(int width, int height) {
  glViewport(0, 0, width, height);
}

void GLWidget::paintGL() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }