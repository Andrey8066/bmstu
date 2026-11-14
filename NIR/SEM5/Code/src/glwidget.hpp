
#pragma once

#include "demreader.hpp"
#include "osmfeature.hpp"

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QPoint>
#include <QVector>

#include <vector>

class QMouseEvent;
class QWheelEvent;

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
public:
  explicit GLWidget(QWidget *parent = nullptr);
  ~GLWidget() override;

  void setDEM(const DEMData &data);
  void setFeatures(const QVector<OSMFeature> &newFeatures);

  // Совместимость с FeatureDispatcher.
  void addBuildingFeature(const OSMFeature &feature);
  void addRoadFeature(const OSMFeature &feature);
  void addRoadAreaFeature(const OSMFeature &feature);

  void addWaterAreaFeature(const OSMFeature &feature);
  void addWaterLineFeature(const OSMFeature &feature);

  void addVegetationAreaFeature(const OSMFeature &feature);
  void addVegetationPointFeature(const OSMFeature &feature);

  void addLanduseAreaFeature(const OSMFeature &feature);
  void addRailwayFeature(const OSMFeature &feature);

  void addAmenityPointFeature(const OSMFeature &feature);
  void addAmenityAreaFeature(const OSMFeature &feature);

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

  struct TerrainSize {
    float width = 0.0f;
    float height = 0.0f;
  };

  struct ScenePoint {
    QVector2D position;
    float groundY = 0.0f;
  };

  TerrainSize calculateTerrainSize() const;

  bool geoToScene(const GeoPoint &point, ScenePoint &result) const;

  bool sampleElevation(double pixelX, double pixelY, float &elevation) const;

  void rebuildMesh();
  void rebuildFeatureMesh();

  void appendBuilding(const OSMFeature &feature);

  void appendPolygon(const OSMFeature &feature, float heightOffset = 0.0f);

  void appendLine(const OSMFeature &feature, float widthMeters,
                  float heightOffset = 0.0f);

  void appendPoint(const OSMFeature &feature, float sizeMeters,
                   float heightOffset = 0.0f);

  void uploadTerrainBuffer();
  void uploadFeatureBuffer();

  void addFeature(const OSMFeature &feature);

  void addVertex(std::vector<Vertex> &target, const QVector2D &position,
                 float y, float r, float g, float b);

  QOpenGLShaderProgram shaderProgram;

  QOpenGLBuffer vertexBuffer{QOpenGLBuffer::VertexBuffer};

  QOpenGLBuffer featureBuffer{QOpenGLBuffer::VertexBuffer};

  std::vector<Vertex> vertices;
  std::vector<Vertex> featureVertices;

  DEMData dem;
  QVector<OSMFeature> features;

  bool meshReady = false;
  bool shaderReady = false;

  float rotationX = 35.0f;
  float rotationZ = -25.0f;

  float zoom = 3.0f;

  float panX = 0.0f;
  float panY = 0.0f;

  QPoint lastMousePosition;
};
