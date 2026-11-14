
#include "glwidget.hpp"

#include <QDebug>
#include <QMouseEvent>
#include <QVector2D>
#include <QWheelEvent>
#include <QtMath>

#include <algorithm>
#include <array>
#include <cmath>
#include <numeric>

namespace {

float cross2D(const QVector2D &a, const QVector2D &b, const QVector2D &c) {
  return (b.x() - a.x()) * (c.y() - a.y()) - (b.y() - a.y()) * (c.x() - a.x());
}

bool pointInTriangle(const QVector2D &p, const QVector2D &a, const QVector2D &b,
                     const QVector2D &c) {
  constexpr float eps = 1e-7f;

  return cross2D(a, b, p) >= -eps && cross2D(b, c, p) >= -eps &&
         cross2D(c, a, p) >= -eps;
}

std::vector<std::array<int, 3>>
triangulatePolygon(const std::vector<QVector2D> &points) {
  std::vector<std::array<int, 3>> triangles;

  if (points.size() < 3)
    return triangles;

  const int n = static_cast<int>(points.size());

  double area = 0.0;

  for (int i = 0; i < n; ++i) {
    const auto &a = points[i];
    const auto &b = points[(i + 1) % n];

    area += a.x() * b.y() - b.x() * a.y();
  }

  if (std::abs(area) < 1e-10)
    return triangles;

  std::vector<int> indices(n);
  std::iota(indices.begin(), indices.end(), 0);

  if (area < 0.0)
    std::reverse(indices.begin(), indices.end());

  constexpr float eps = 1e-7f;

  while (indices.size() > 3) {
    bool clipped = false;

    for (std::size_t i = 0; i < indices.size(); ++i) {
      const int prev = indices[(i + indices.size() - 1) % indices.size()];

      const int curr = indices[i];
      const int next = indices[(i + 1) % indices.size()];

      const auto &a = points[prev];
      const auto &b = points[curr];
      const auto &c = points[next];

      if (cross2D(a, b, c) <= eps)
        continue;

      bool containsPoint = false;

      for (int index : indices) {
        if (index == prev || index == curr || index == next)
          continue;

        if (pointInTriangle(points[index], a, b, c)) {
          containsPoint = true;
          break;
        }
      }

      if (containsPoint)
        continue;

      triangles.push_back({prev, curr, next});
      indices.erase(indices.begin() + static_cast<long>(i));

      clipped = true;
      break;
    }

    if (!clipped)
      return {};
  }

  if (indices.size() == 3) {
    triangles.push_back({indices[0], indices[1], indices[2]});
  }

  return triangles;
}

bool sameGeoPoint(const GeoPoint &a, const GeoPoint &b) {
  return std::abs(a.lat - b.lat) < 1e-10 && std::abs(a.lon - b.lon) < 1e-10;
}

} // namespace

// -----------------------------------------------------------------------------
// Constructor / destructor
// -----------------------------------------------------------------------------

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent) {
  setFocusPolicy(Qt::StrongFocus);
}

GLWidget::~GLWidget() {
  if (context() && context()->isValid()) {
    makeCurrent();

    vertexBuffer.destroy();
    featureBuffer.destroy();

    doneCurrent();
  }
}

// -----------------------------------------------------------------------------
// OpenGL initialization
// -----------------------------------------------------------------------------

void GLWidget::initializeGL() {
  initializeOpenGLFunctions();

  glClearColor(0.08f, 0.10f, 0.13f, 1.0f);

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  const char *vertexShader = R"(
        attribute vec3 position;
        attribute vec3 color;

        uniform mat4 mvp;

        varying vec3 vertexColor;

        void main()
        {
            gl_Position = mvp * vec4(position, 1.0);
            vertexColor = color;
        }
    )";

  const char *fragmentShader = R"(
        varying vec3 vertexColor;

        void main()
        {
            gl_FragColor = vec4(vertexColor, 1.0);
        }
    )";

  if (!shaderProgram.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                             vertexShader)) {
    qWarning() << "Vertex shader error:" << shaderProgram.log();
    return;
  }

  if (!shaderProgram.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                             fragmentShader)) {
    qWarning() << "Fragment shader error:" << shaderProgram.log();
    return;
  }

  if (!shaderProgram.link()) {
    qWarning() << "Shader link error:" << shaderProgram.log();
    return;
  }

  shaderReady = true;

  vertexBuffer.create();
  vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);

  featureBuffer.create();
  featureBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);

  rebuildMesh();
  rebuildFeatureMesh();
}

// -----------------------------------------------------------------------------
// Public setters
// -----------------------------------------------------------------------------

void GLWidget::setDEM(const DEMData &data) {
  dem = data;

  qDebug() << "DEM loaded:" << dem.width << "x" << dem.height
           << "elevation range:" << dem.minElevation << dem.maxElevation;

  rebuildMesh();
  rebuildFeatureMesh();

  update();
}

void GLWidget::setFeatures(const QVector<OSMFeature> &newFeatures) {
  features = newFeatures;

  qDebug() << "OSM features:" << features.size();

  rebuildFeatureMesh();

  update();
}

void GLWidget::addFeature(const OSMFeature &feature) {
  features.push_back(feature);

  rebuildFeatureMesh();
  update();
}

// -----------------------------------------------------------------------------
// FeatureDispatcher compatibility methods
// -----------------------------------------------------------------------------

void GLWidget::addBuildingFeature(const OSMFeature &feature) {
  addFeature(feature);
}

void GLWidget::addRoadFeature(const OSMFeature &feature) {
  addFeature(feature);
}

void GLWidget::addRoadAreaFeature(const OSMFeature &feature) {
  addFeature(feature);
}

void GLWidget::addWaterAreaFeature(const OSMFeature &feature) {
  addFeature(feature);
}

void GLWidget::addWaterLineFeature(const OSMFeature &feature) {
  addFeature(feature);
}

void GLWidget::addVegetationAreaFeature(const OSMFeature &feature) {
  addFeature(feature);
}

void GLWidget::addVegetationPointFeature(const OSMFeature &feature) {
  addFeature(feature);
}

void GLWidget::addLanduseAreaFeature(const OSMFeature &feature) {
  addFeature(feature);
}

void GLWidget::addRailwayFeature(const OSMFeature &feature) {
  addFeature(feature);
}

void GLWidget::addAmenityPointFeature(const OSMFeature &feature) {
  addFeature(feature);
}

void GLWidget::addAmenityAreaFeature(const OSMFeature &feature) {
  addFeature(feature);
}

// -----------------------------------------------------------------------------
// Terrain mesh
// -----------------------------------------------------------------------------

void GLWidget::rebuildMesh() {
  vertices.clear();
  meshReady = false;

  const int width = dem.width;
  const int height = dem.height;

  if (width < 2 || height < 2) {
    uploadTerrainBuffer();
    update();
    return;
  }

  const std::size_t required = static_cast<std::size_t>(width) * height;

  if (dem.elevation.size() < required) {
    qWarning() << "Invalid DEM elevation array";
    uploadTerrainBuffer();
    update();
    return;
  }

  const float elevationRange = dem.maxElevation - dem.minElevation;

  const TerrainSize terrainSize = calculateTerrainSize();

  if (terrainSize.width <= 0.0f || terrainSize.height <= 0.0f) {
    uploadTerrainBuffer();
    update();
    return;
  }

  constexpr int maxDimension = 400;

  const int stepX =
      std::max(1, (width - 1 + maxDimension - 2) / (maxDimension - 1));

  const int stepY =
      std::max(1, (height - 1 + maxDimension - 2) / (maxDimension - 1));

  auto vertexAt = [&](int x, int y, Vertex &result) -> bool {
    const float elevation =
        dem.elevation[static_cast<std::size_t>(y) * width + x];

    if (!std::isfinite(elevation))
      return false;

    const float nx =
        (static_cast<float>(x) / (width - 1) - 0.5f) * terrainSize.width;

    const double *gt = dem.geoTransform;

    const float rowDirection = gt[5] < 0.0 ? -1.0f : 1.0f;

    const float nz = -rowDirection *
                     (static_cast<float>(y) / (height - 1) - 0.5f) *
                     terrainSize.height;

    const float normalized =
        elevationRange > 0.0f
            ? std::clamp((elevation - dem.minElevation) / elevationRange, 0.0f,
                         1.0f)
            : 0.5f;

    const float vertical = elevationRange > 0.0f ? normalized * 0.12f : 0.0f;

    float r, g, b;

    if (normalized < 0.5f) {
      const float t = normalized * 2.0f;

      r = 0.12f + 0.30f * t;
      g = 0.35f + 0.30f * t;
      b = 0.12f + 0.05f * t;
    } else {
      const float t = (normalized - 0.5f) * 2.0f;

      r = 0.42f + 0.50f * t;
      g = 0.65f + 0.30f * t;
      b = 0.17f + 0.75f * t;
    }

    result = {nx, vertical, nz, r, g, b};

    return true;
  };

  for (int y = 0; y < height - 1; y += stepY) {
    const int y2 = std::min(y + stepY, height - 1);

    for (int x = 0; x < width - 1; x += stepX) {
      const int x2 = std::min(x + stepX, width - 1);

      Vertex a, b, c, d;

      if (!vertexAt(x, y, a) || !vertexAt(x2, y, b) || !vertexAt(x, y2, c) ||
          !vertexAt(x2, y2, d)) {
        continue;
      }

      vertices.insert(vertices.end(), {a, c, b, b, c, d});
    }
  }

  meshReady = !vertices.empty();

  uploadTerrainBuffer();

  qDebug() << "Terrain vertices:" << vertices.size();
}

void GLWidget::uploadTerrainBuffer() {
  if (!context() || !context()->isValid())
    return;

  makeCurrent();

  if (!vertexBuffer.isCreated())
    vertexBuffer.create();

  if (vertexBuffer.isCreated()) {
    vertexBuffer.bind();

    if (vertices.empty())
      vertexBuffer.allocate(0);
    else
      vertexBuffer.allocate(vertices.data(),
                            static_cast<int>(vertices.size() * sizeof(Vertex)));

    vertexBuffer.release();
  }

  doneCurrent();
}

// -----------------------------------------------------------------------------
// Geographic coordinates -> DEM pixels -> scene coordinates
// -----------------------------------------------------------------------------

bool GLWidget::sampleElevation(double pixelX, double pixelY,
                               float &elevation) const {
  const int width = dem.width;
  const int height = dem.height;

  if (width < 2 || height < 2)
    return false;

  if (pixelX < 0.0 || pixelY < 0.0 || pixelX > width - 1 ||
      pixelY > height - 1) {
    return false;
  }

  const int x0 = static_cast<int>(std::floor(pixelX));
  const int y0 = static_cast<int>(std::floor(pixelY));

  const int x1 = std::min(x0 + 1, width - 1);
  const int y1 = std::min(y0 + 1, height - 1);

  const float fx = static_cast<float>(pixelX - x0);
  const float fy = static_cast<float>(pixelY - y0);

  const auto at = [&](int x, int y) -> float {
    return dem.elevation[static_cast<std::size_t>(y) * width + x];
  };

  const float z00 = at(x0, y0);
  const float z10 = at(x1, y0);
  const float z01 = at(x0, y1);
  const float z11 = at(x1, y1);

  if (!std::isfinite(z00) || !std::isfinite(z10) || !std::isfinite(z01) ||
      !std::isfinite(z11)) {
    return false;
  }

  const float z0 = z00 * (1.0f - fx) + z10 * fx;
  const float z1 = z01 * (1.0f - fx) + z11 * fx;

  elevation = z0 * (1.0f - fy) + z1 * fy;

  return true;
}

bool GLWidget::geoToScene(const GeoPoint &point, ScenePoint &result) const {
  const int width = dem.width;
  const int height = dem.height;

  if (width < 2 || height < 2)
    return false;

  const double *gt = dem.geoTransform;

  const double determinant = gt[1] * gt[5] - gt[2] * gt[4];

  if (std::abs(determinant) < 1e-15)
    return false;

  const double dx = point.lon - gt[0];
  const double dy = point.lat - gt[3];

  const double pixelX = (gt[5] * dx - gt[2] * dy) / determinant - 0.5;

  const double pixelY = (-gt[4] * dx + gt[1] * dy) / determinant - 0.5;

  float elevation = 0.0f;

  if (!sampleElevation(pixelX, pixelY, elevation))
    return false;

  const TerrainSize terrainSize = calculateTerrainSize();

  const float sceneX =
      (static_cast<float>(pixelX) / (width - 1) - 0.5f) * terrainSize.width;

  const float rowDirection = gt[5] < 0.0 ? -1.0f : 1.0f;

  const float sceneZ = -rowDirection *
                       (static_cast<float>(pixelY) / (height - 1) - 0.5f) *
                       terrainSize.height;

  const float elevationRange = dem.maxElevation - dem.minElevation;

  const float verticalScale =
      elevationRange > 0.0f ? 0.12f / elevationRange : 0.001f;

  result.position = QVector2D(sceneX, sceneZ);

  result.groundY = (elevation - dem.minElevation) * verticalScale;

  return true;
}

// -----------------------------------------------------------------------------
// Common vertex helper
// -----------------------------------------------------------------------------

void GLWidget::addVertex(std::vector<Vertex> &target, const QVector2D &position,
                         float y, float r, float g, float b) {
  target.push_back({position.x(), y, position.y(), r, g, b});
}

// -----------------------------------------------------------------------------
// Building geometry: walls + roof
// -----------------------------------------------------------------------------

void GLWidget::appendBuilding(const OSMFeature &feature) {
  if (feature.geometry.size() < 3)
    return;

  std::vector<ScenePoint> scenePoints;
  scenePoints.reserve(feature.geometry.size());

  for (const GeoPoint &geo : feature.geometry) {
    ScenePoint point;

    if (!geoToScene(geo, point))
      return;

    scenePoints.push_back(point);
  }

  if (scenePoints.size() >= 2 &&
      sameGeoPoint(feature.geometry.first(), feature.geometry.last())) {
    scenePoints.pop_back();
  }

  if (scenePoints.size() < 3)
    return;

  std::vector<QVector2D> polygon;
  polygon.reserve(scenePoints.size());

  float groundSum = 0.0f;

  for (const ScenePoint &point : scenePoints) {
    polygon.push_back(point.position);
    groundSum += point.groundY;
  }

  const auto triangles = triangulatePolygon(polygon);

  if (triangles.empty())
    return;

  const float baseY = groundSum / static_cast<float>(scenePoints.size());

  const float elevationRange = dem.maxElevation - dem.minElevation;

  const float verticalScale =
      elevationRange > 0.0f ? 0.12f / elevationRange : 0.001f;

  const float buildingHeight = feature.heightMeters > 0.0
                                   ? static_cast<float>(feature.heightMeters)
                                   : 6.0f;

  const float roofY = baseY + buildingHeight * verticalScale;

  const Colors &color = feature.colors;

  const int count = static_cast<int>(polygon.size());

  // Стены.
  for (int i = 0; i < count; ++i) {
    const int next = (i + 1) % count;

    const QVector2D &a = polygon[i];
    const QVector2D &b = polygon[next];

    addVertex(featureVertices, a, baseY, color.wallR, color.wallG, color.wallB);

    addVertex(featureVertices, b, baseY, color.wallR, color.wallG, color.wallB);

    addVertex(featureVertices, b, roofY, color.wallR, color.wallG, color.wallB);

    addVertex(featureVertices, a, baseY, color.wallR, color.wallG, color.wallB);

    addVertex(featureVertices, b, roofY, color.wallR, color.wallG, color.wallB);

    addVertex(featureVertices, a, roofY, color.wallR, color.wallG, color.wallB);
  }

  // Крыша.
  for (const auto &triangle : triangles) {
    for (int index : triangle) {
      addVertex(featureVertices, polygon[index], roofY, color.roofR,
                color.roofG, color.roofB);
    }
  }
}

// -----------------------------------------------------------------------------
// Flat polygon geometry: water, vegetation, landuse, amenity, road areas
// -----------------------------------------------------------------------------

void GLWidget::appendPolygon(const OSMFeature &feature, float heightOffset) {
  if (feature.geometry.size() < 3)
    return;

  std::vector<QVector2D> polygon;
  polygon.reserve(feature.geometry.size());

  float groundSum = 0.0f;
  int validCount = 0;

  for (const GeoPoint &geo : feature.geometry) {
    ScenePoint point;

    if (!geoToScene(geo, point))
      return;

    polygon.push_back(point.position);
    groundSum += point.groundY;
    ++validCount;
  }

  if (polygon.size() >= 2 &&
      sameGeoPoint(feature.geometry.first(), feature.geometry.last())) {
    polygon.pop_back();
  }

  if (polygon.size() < 3 || validCount == 0)
    return;

  const auto triangles = triangulatePolygon(polygon);

  if (triangles.empty())
    return;

  const float y = groundSum / static_cast<float>(validCount) + heightOffset;

  const Colors &color = feature.colors;

  for (const auto &triangle : triangles) {
    for (int index : triangle) {
      addVertex(featureVertices, polygon[index], y, color.roofR, color.roofG,
                color.roofB);
    }
  }
}

// -----------------------------------------------------------------------------
// Line geometry: roads, rivers, railway
// Each line segment becomes a flat-width quad.
// -----------------------------------------------------------------------------

void GLWidget::appendLine(const OSMFeature &feature, float widthMeters,
                          float heightOffset) {
  if (feature.geometry.size() < 2)
    return;

  std::vector<ScenePoint> points;
  points.reserve(feature.geometry.size());

  for (const GeoPoint &geo : feature.geometry) {
    ScenePoint point;

    if (!geoToScene(geo, point)) {
      // Не соединяем участки через точки вне DEM.
      points.clear();
      return;
    }

    points.push_back(point);
  }

  const TerrainSize terrainSize = calculateTerrainSize();

  const double *gt = dem.geoTransform;

  const double centerLat =
      gt[3] + 0.5 * (dem.width - 1) * gt[4] + 0.5 * (dem.height - 1) * gt[5];

  const double metersPerDegreeLat = 111132.0;

  const double metersPerDegreeLon =
      111320.0 * std::cos(qDegreesToRadians(centerLat));

  const double physicalWidth =
      std::hypot((dem.width - 1) * gt[1] * metersPerDegreeLon,
                 (dem.width - 1) * gt[4] * metersPerDegreeLat);

  const double physicalHeight =
      std::hypot((dem.height - 1) * gt[2] * metersPerDegreeLon,
                 (dem.height - 1) * gt[5] * metersPerDegreeLat);

  const double maxPhysicalDimension = std::max(physicalWidth, physicalHeight);

  if (maxPhysicalDimension <= 0.0)
    return;

  const float metersToScene = static_cast<float>(2.0 / maxPhysicalDimension);

  const float halfWidth = std::max(0.0001f, widthMeters * metersToScene * 0.5f);

  const Colors &color = feature.colors;

  for (std::size_t i = 0; i + 1 < points.size(); ++i) {
    const ScenePoint &a = points[i];
    const ScenePoint &b = points[i + 1];

    const QVector2D direction = b.position - a.position;

    const float length = direction.length();

    if (length < 1e-7f)
      continue;

    const QVector2D normal(-direction.y() / length, direction.x() / length);

    const QVector2D offset = normal * halfWidth;

    const QVector2D aLeft = a.position + offset;
    const QVector2D aRight = a.position - offset;

    const QVector2D bLeft = b.position + offset;
    const QVector2D bRight = b.position - offset;

    const float yA = a.groundY + heightOffset;
    const float yB = b.groundY + heightOffset;

    // Первый треугольник.
    addVertex(featureVertices, aLeft, yA, color.roofR, color.roofG,
              color.roofB);

    addVertex(featureVertices, aRight, yA, color.roofR, color.roofG,
              color.roofB);

    addVertex(featureVertices, bLeft, yB, color.roofR, color.roofG,
              color.roofB);

    // Второй треугольник.
    addVertex(featureVertices, aRight, yA, color.roofR, color.roofG,
              color.roofB);

    addVertex(featureVertices, bRight, yB, color.roofR, color.roofG,
              color.roofB);

    addVertex(featureVertices, bLeft, yB, color.roofR, color.roofG,
              color.roofB);
  }
}

// -----------------------------------------------------------------------------
// Point geometry: amenity and other OSM nodes
// -----------------------------------------------------------------------------

void GLWidget::appendPoint(const OSMFeature &feature, float sizeMeters,
                           float heightOffset) {
  if (feature.geometry.isEmpty())
    return;

  ScenePoint point;

  if (!geoToScene(feature.geometry.first(), point))
    return;

  const TerrainSize terrainSize = calculateTerrainSize();

  const double *gt = dem.geoTransform;

  const double centerLat =
      gt[3] + 0.5 * (dem.width - 1) * gt[4] + 0.5 * (dem.height - 1) * gt[5];

  const double metersPerDegreeLat = 111132.0;

  const double metersPerDegreeLon =
      111320.0 * std::cos(qDegreesToRadians(centerLat));

  const double physicalWidth =
      std::hypot((dem.width - 1) * gt[1] * metersPerDegreeLon,
                 (dem.width - 1) * gt[4] * metersPerDegreeLat);

  const double physicalHeight =
      std::hypot((dem.height - 1) * gt[2] * metersPerDegreeLon,
                 (dem.height - 1) * gt[5] * metersPerDegreeLat);

  const double maxDimension = std::max(physicalWidth, physicalHeight);

  if (maxDimension <= 0.0)
    return;

  const float halfSize =
      std::max(0.0002f, static_cast<float>(sizeMeters / maxDimension));

  const QVector2D p = point.position;

  const QVector2D a(p.x() - halfSize, p.y() - halfSize);
  const QVector2D b(p.x() + halfSize, p.y() - halfSize);
  const QVector2D c(p.x() + halfSize, p.y() + halfSize);
  const QVector2D d(p.x() - halfSize, p.y() + halfSize);

  const float y = point.groundY + heightOffset;

  const Colors &color = feature.colors;

  addVertex(featureVertices, a, y, color.roofR, color.roofG, color.roofB);
  addVertex(featureVertices, b, y, color.roofR, color.roofG, color.roofB);
  addVertex(featureVertices, c, y, color.roofR, color.roofG, color.roofB);

  addVertex(featureVertices, a, y, color.roofR, color.roofG, color.roofB);
  addVertex(featureVertices, c, y, color.roofR, color.roofG, color.roofB);
  addVertex(featureVertices, d, y, color.roofR, color.roofG, color.roofB);
}

// -----------------------------------------------------------------------------
// OSM feature dispatcher
// -----------------------------------------------------------------------------

void GLWidget::rebuildFeatureMesh() {
  featureVertices.clear();

  if (dem.width < 2 || dem.height < 2) {
    uploadFeatureBuffer();
    update();
    return;
  }

  int buildings = 0;
  int roads = 0;
  int water = 0;
  int vegetation = 0;
  int railways = 0;
  int other = 0;

  for (const OSMFeature &feature : features) {
    if (feature.geometry.isEmpty())
      continue;

    switch (feature.type) {
    case OSMFeatureType::Building:
      if (feature.geometryType == OSMGeometryType::Polygon)
        appendBuilding(feature);
      else if (feature.geometryType == OSMGeometryType::Point)
        appendPoint(feature, 5.0f);

      ++buildings;
      break;

    case OSMFeatureType::Road:
      if (feature.geometryType == OSMGeometryType::Polygon) {
        appendPolygon(feature, 0.0015f);
      } else if (feature.geometryType == OSMGeometryType::LineString) {
        const float width = feature.widthMeters > 0.0
                                ? static_cast<float>(feature.widthMeters)
                                : 4.0f;

        appendLine(feature, width, 0.0015f);
      }

      ++roads;
      break;

    case OSMFeatureType::Water:
      if (feature.geometryType == OSMGeometryType::Polygon) {
        appendPolygon(feature, 0.0025f);
      } else if (feature.geometryType == OSMGeometryType::LineString) {
        const float width = feature.widthMeters > 0.0
                                ? static_cast<float>(feature.widthMeters)
                                : 5.0f;

        appendLine(feature, width, 0.0025f);
      }

      ++water;
      break;

    case OSMFeatureType::Vegetation:
      if (feature.geometryType == OSMGeometryType::Polygon)
        appendPolygon(feature, 0.001f);
      else if (feature.geometryType == OSMGeometryType::Point)
        appendPoint(feature, 4.0f, 0.001f);

      ++vegetation;
      break;

    case OSMFeatureType::Landuse:
      if (feature.geometryType == OSMGeometryType::Polygon)
        appendPolygon(feature, 0.0008f);

      ++other;
      break;

    case OSMFeatureType::Railway:
      if (feature.geometryType == OSMGeometryType::LineString) {
        const float width = feature.widthMeters > 0.0
                                ? static_cast<float>(feature.widthMeters)
                                : 4.5f;

        appendLine(feature, width, 0.002f);
      }

      ++railways;
      break;

    case OSMFeatureType::Amenity:
      if (feature.geometryType == OSMGeometryType::Polygon)
        appendPolygon(feature, 0.0015f);
      else if (feature.geometryType == OSMGeometryType::Point)
        appendPoint(feature, 5.0f, 0.002f);

      ++other;
      break;

    case OSMFeatureType::Unknown:
    default:
      break;
    }
  }

  uploadFeatureBuffer();

  qDebug() << "OSM mesh rebuilt:"
           << "buildings =" << buildings << "roads =" << roads
           << "water =" << water << "vegetation =" << vegetation
           << "railways =" << railways << "other =" << other
           << "vertices =" << featureVertices.size();
}

void GLWidget::uploadFeatureBuffer() {
  if (!context() || !context()->isValid())
    return;

  makeCurrent();

  if (!featureBuffer.isCreated())
    featureBuffer.create();

  if (featureBuffer.isCreated()) {
    featureBuffer.bind();

    if (featureVertices.empty())
      featureBuffer.allocate(0);
    else
      featureBuffer.allocate(
          featureVertices.data(),
          static_cast<int>(featureVertices.size() * sizeof(Vertex)));

    featureBuffer.release();
  }

  doneCurrent();
}

// -----------------------------------------------------------------------------
// Rendering
// -----------------------------------------------------------------------------

void GLWidget::resizeGL(int width, int height) {
  glViewport(0, 0, width, height);
}

void GLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (!shaderReady)
    return;

  QMatrix4x4 projection;

  projection.perspective(45.0f,
                         static_cast<float>(std::max(1, width())) /
                             static_cast<float>(std::max(1, height())),
                         0.01f, 100.0f);

  QMatrix4x4 view;

  view.translate(panX, panY, -zoom);

  view.rotate(rotationX, 1.0f, 0.0f, 0.0f);
  view.rotate(rotationZ, 0.0f, 1.0f, 0.0f);

  const QMatrix4x4 mvp = projection * view;

  auto drawBuffer = [&](QOpenGLBuffer &buffer, std::size_t vertexCount) {
    if (vertexCount == 0 || !buffer.isCreated())
      return;

    shaderProgram.bind();
    shaderProgram.setUniformValue("mvp", mvp);

    buffer.bind();

    const int positionLocation = shaderProgram.attributeLocation("position");

    const int colorLocation = shaderProgram.attributeLocation("color");

    shaderProgram.enableAttributeArray(positionLocation);
    shaderProgram.enableAttributeArray(colorLocation);

    shaderProgram.setAttributeBuffer(positionLocation, GL_FLOAT,
                                     offsetof(Vertex, x), 3, sizeof(Vertex));

    shaderProgram.setAttributeBuffer(colorLocation, GL_FLOAT,
                                     offsetof(Vertex, r), 3, sizeof(Vertex));

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexCount));

    shaderProgram.disableAttributeArray(positionLocation);
    shaderProgram.disableAttributeArray(colorLocation);

    buffer.release();
    shaderProgram.release();
  };

  // Сначала рельеф.
  if (meshReady)
    drawBuffer(vertexBuffer, vertices.size());

  // Затем здания и остальные OSM-объекты.
  drawBuffer(featureBuffer, featureVertices.size());
}

// -----------------------------------------------------------------------------
// Camera controls
// -----------------------------------------------------------------------------

void GLWidget::mousePressEvent(QMouseEvent *event) {
  lastMousePosition = event->position().toPoint();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
  const QPoint current = event->position().toPoint();

  const QPoint delta = current - lastMousePosition;

  if (event->buttons() & Qt::LeftButton) {
    rotationZ += delta.x() * 0.5f;
    rotationX += delta.y() * 0.5f;

    update();
  }

  if (event->buttons() & Qt::RightButton) {
    const float worldPerPixel =
        (2.0f * zoom * 0.41421356f) / static_cast<float>(std::max(1, height()));

    panX += delta.x() * worldPerPixel;
    panY -= delta.y() * worldPerPixel;

    update();
  }

  lastMousePosition = current;
}

void GLWidget::wheelEvent(QWheelEvent *event) {
  zoom -= event->angleDelta().y() / 120.0f * 0.25f;

  zoom = std::clamp(zoom, 0.01f, 15.0f);

  update();
}

// -----------------------------------------------------------------------------
// DEM physical dimensions
// -----------------------------------------------------------------------------

GLWidget::TerrainSize GLWidget::calculateTerrainSize() const {
  TerrainSize result;

  if (dem.width < 2 || dem.height < 2)
    return result;

  const double *gt = dem.geoTransform;

  const double centerLat =
      gt[3] + 0.5 * (dem.width - 1) * gt[4] + 0.5 * (dem.height - 1) * gt[5];

  const double metersPerDegreeLat = 111132.0;

  const double metersPerDegreeLon =
      111320.0 * std::cos(qDegreesToRadians(centerLat));

  const double dxLon = (dem.width - 1) * gt[1];
  const double dxLat = (dem.width - 1) * gt[4];

  const double dyLon = (dem.height - 1) * gt[2];
  const double dyLat = (dem.height - 1) * gt[5];

  const double physicalWidth =
      std::hypot(dxLon * metersPerDegreeLon, dxLat * metersPerDegreeLat);

  const double physicalHeight =
      std::hypot(dyLon * metersPerDegreeLon, dyLat * metersPerDegreeLat);

  const double maxDimension = std::max(physicalWidth, physicalHeight);

  if (maxDimension <= 0.0)
    return result;

  result.width = static_cast<float>(2.0 * physicalWidth / maxDimension);

  result.height = static_cast<float>(2.0 * physicalHeight / maxDimension);

  return result;
}
