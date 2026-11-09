#pragma once

#include "dem_reader.hpp"

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QPoint>

#include <vector>

struct GeoPoint {
  double lat;
  double lon;
};

struct BuildingFootprint {
  std::vector<GeoPoint> outline;
  float heightMeters = 9.0f;
};

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
public:
  explicit GLWidget(QWidget *parent = nullptr);
  ~GLWidget() override;

  void setDEM(const DEMData &dem);
  void setBuildings(const std::vector<BuildingFootprint> &buildings);

protected:
  void initializeGL() override;
  void resizeGL(int width, int height) override;
  void paintGL() override;

  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;

private:
  struct Vertex {
    float x, y, z;
    float r, g, b;
  };

  void rebuildMesh();

  QOpenGLShaderProgram shaderProgram;
  QOpenGLBuffer vertexBuffer{QOpenGLBuffer::VertexBuffer};

  std::vector<Vertex> vertices;
  DEMData dem;

  bool meshReady = false;
  float rotationX = 35.0f;
  float rotationZ = -25.0f;
  float zoom = 3.0f;
  float panX = 0.0f;
  float panY = 0.0f;

  QPoint lastMousePosition;

  void rebuildBuildingMesh();

  QOpenGLBuffer buildingBuffer{QOpenGLBuffer::VertexBuffer};

  std::vector<Vertex> buildingVertices;
  std::vector<BuildingFootprint> buildings;
};