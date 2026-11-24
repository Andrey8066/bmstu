
#include "glwidget.hpp"

#include <QDebug>
#include <QMouseEvent>
#include <QVector2D>
#include <QWheelEvent>
#include <QtMath>

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <mapbox/earcut.hpp>
#include <numeric>
#include <unordered_map>
#include <vector>

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

// Расстояние между двумя географическими точками в метрах.
double geoDistanceMeters(const GeoPoint &a, const GeoPoint &b) {
  constexpr double earthRadius = 6371000.0;

  const double lat1 = qDegreesToRadians(a.lat);
  const double lat2 = qDegreesToRadians(b.lat);

  const double dLat = lat2 - lat1;
  const double dLon = qDegreesToRadians(b.lon - a.lon);

  const double sinLat = std::sin(dLat * 0.5);
  const double sinLon = std::sin(dLon * 0.5);

  const double h =
      sinLat * sinLat + std::cos(lat1) * std::cos(lat2) * sinLon * sinLon;

  return earthRadius * 2.0 * std::asin(std::sqrt(std::clamp(h, 0.0, 1.0)));
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

const DEMData &GLWidget::getDEM() const { return dem; }

const QVector<OSMFeature> &GLWidget::getFeatures() const { return features; }

float GLWidget::getVerticalScale() const { return verticalScaleCoefficient; }

void GLWidget::setVerticalScale(float value) {
  if (!std::isfinite(value) || value <= 0.0f)
    return;

  verticalScaleCoefficient = value;

  rebuildMesh();
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

  constexpr int maxDimension = 10000;

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

    const float vertical = elevationRange > 0.0f
                               ? (elevation - dem.minElevation) *
                                     (verticalScaleCoefficient / elevationRange)
                               : 0.0f;

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

  const float z00 = at(x0, y0); // a
  const float z10 = at(x1, y0); // b
  const float z01 = at(x0, y1); // c
  const float z11 = at(x1, y1); // d

  if (!std::isfinite(z00) || !std::isfinite(z10) || !std::isfinite(z01) ||
      !std::isfinite(z11)) {
    return false;
  }

  // Интерполяция по той же диагонали b-c,
  // по которой строятся треугольники рельефа.
  if (fx + fy <= 1.0f) {
    elevation = z00 * (1.0f - fx - fy) + z10 * fx + z01 * fy;
  } else {
    elevation = z10 * (1.0f - fy) + z01 * (1.0f - fx) + z11 * (fx + fy - 1.0f);
  }

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

  const float verticalScale = elevationRange > 0.0f
                                  ? verticalScaleCoefficient / elevationRange
                                  : 0.001f;

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

  // Преобразует географическое кольцо в координаты сцены.
  auto convertRing = [&](const QVector<GeoPoint> &geoRing,
                         std::vector<ScenePoint> &sceneRing) -> bool {
    sceneRing.clear();

    for (const GeoPoint &geo : geoRing) {
      ScenePoint point;

      if (!geoToScene(geo, point))
        return false;

      sceneRing.push_back(point);
    }

    // Убираем повтор последней точки замкнутого кольца.
    if (sceneRing.size() >= 2 &&
        sameGeoPoint(geoRing.first(), geoRing.last())) {
      sceneRing.pop_back();
    }

    return sceneRing.size() >= 3;
  };

  std::vector<ScenePoint> outerScene;

  if (!convertRing(feature.geometry, outerScene))
    return;

  // Все кольца: сначала outer, затем inner.
  std::vector<std::vector<ScenePoint>> sceneRings;
  sceneRings.push_back(outerScene);

  for (const auto &innerGeo : feature.innerRings) {
    std::vector<ScenePoint> innerScene;

    if (!convertRing(innerGeo, innerScene))
      continue;

    sceneRings.push_back(std::move(innerScene));
  }

  // Формат для Mapbox Earcut.
  std::vector<std::vector<std::array<double, 2>>> polygon;

  // Плоский массив координат для последующего получения вершин крыши.
  std::vector<QVector2D> roofPoints;

  for (const auto &ring : sceneRings) {
    std::vector<std::array<double, 2>> earcutRing;

    for (const ScenePoint &point : ring) {
      earcutRing.push_back({static_cast<double>(point.position.x()),
                            static_cast<double>(point.position.y())});

      roofPoints.push_back(point.position);
    }

    polygon.push_back(std::move(earcutRing));
  }

  if (roofPoints.size() < 3)
    return;

  const std::vector<unsigned int> indices =
      mapbox::earcut<unsigned int>(polygon);

  if (indices.empty())
    return;

  // Высота основания по внешнему контуру.
  float groundSum = 0.0f;

  for (const ScenePoint &point : outerScene)
    groundSum += point.groundY;

  const float baseY = groundSum / static_cast<float>(outerScene.size());

  const float elevationRange = dem.maxElevation - dem.minElevation;

  const float verticalScale = elevationRange > 0.0f
                                  ? verticalScaleCoefficient / elevationRange
                                  : 0.001f;

  const bool isRoof = feature.tags.value("building").toString() == "roof";

  const float buildingHeight =
      feature.heightMeters > 0.0f ? feature.heightMeters : 6.0f;

  const float baseBuildingY = baseY + feature.minHeightMeters * verticalScale;

  // Для building=roof верхняя поверхность находится
  // непосредственно на уровне building:min_level.
  const float roofY =
      isRoof ? baseBuildingY : baseBuildingY + buildingHeight * verticalScale;

  const Colors &color = feature.colors;

  // Стены внешнего и внутренних контуров.
  if (!isRoof) {
    for (const auto &ring : sceneRings) {
      const int count = static_cast<int>(ring.size());

      for (int i = 0; i < count; ++i) {
        const ScenePoint &a = ring[i];
        const ScenePoint &b = ring[(i + 1) % count];

        // Индивидуальная высота основания каждой вершины.
        const float bottomA =
            a.groundY + feature.minHeightMeters * verticalScale;

        const float bottomB =
            b.groundY + feature.minHeightMeters * verticalScale;

        const QVector2D &pa = a.position;
        const QVector2D &pb = b.position;

        // Первый треугольник.
        addVertex(featureVertices, pa, bottomA, color.wallR, color.wallG,
                  color.wallB);

        addVertex(featureVertices, pb, bottomB, color.wallR, color.wallG,
                  color.wallB);

        addVertex(featureVertices, pb, roofY, color.wallR, color.wallG,
                  color.wallB);

        // Второй треугольник.
        addVertex(featureVertices, pa, bottomA, color.wallR, color.wallG,
                  color.wallB);

        addVertex(featureVertices, pb, roofY, color.wallR, color.wallG,
                  color.wallB);

        addVertex(featureVertices, pa, roofY, color.wallR, color.wallG,
                  color.wallB);
      }
    }
  }

  // Крыша с отверстиями.
  for (unsigned int index : indices) {
    if (index >= roofPoints.size())
      continue;

    addVertex(featureVertices, roofPoints[index], roofY, color.roofR,
              color.roofG, color.roofB);
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
                          float heightOffset,
                          const BridgeProfile *bridgeProfile) {
  if (feature.geometry.size() < 2)
    return;

  std::vector<ScenePoint> points;
  points.reserve(feature.geometry.size());

  std::vector<double> bridgeAlong;
  bridgeAlong.reserve(feature.geometry.size());

  for (const GeoPoint &geo : feature.geometry) {
    ScenePoint point;

    if (!geoToScene(geo, point)) {
      continue;
    }

    points.push_back(point);

    double along = 0.0;

    if (bridgeProfile && bridgeProfile->valid) {
      const double metersPerLon =
          111320.0 * std::cos(qDegreesToRadians(bridgeProfile->refLat));

      const double localX = (geo.lon - bridgeProfile->refLon) * metersPerLon;

      const double localY = (geo.lat - bridgeProfile->refLat) * 111132.0;

      along = localX * bridgeProfile->axisX + localY * bridgeProfile->axisY;
    }

    bridgeAlong.push_back(along);
  }

  if (points.size() < 2)
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

  const double maxPhysicalDimension = std::max(physicalWidth, physicalHeight);

  if (maxPhysicalDimension <= 0.0)
    return;

  const float metersToScene = static_cast<float>(2.0 / maxPhysicalDimension);

  // Параметры моста в метрах.
  constexpr float bridgeHeightMeters = 5.0f;
  constexpr float bridgeThicknessMeters = 0.35f;

  // Вертикальный масштаб DEM.
  const float elevationRange = dem.maxElevation - dem.minElevation;

  const float verticalScale = elevationRange > 0.0f
                                  ? verticalScaleCoefficient / elevationRange
                                  : 0.001f;

  const float halfWidth = std::max(0.0001f, widthMeters * metersToScene * 0.5f);

  const float rowDirection = gt[5] < 0.0 ? -1.0f : 1.0f;

  const bool isBridge = feature.type == OSMFeatureType::Bridge;

  auto getBridgeProfile = [&](double along) -> float {
    if (!isBridge || !bridgeProfile || !bridgeProfile->valid)
      return 0.0f;

    const double start = bridgeProfile->minAlong;
    const double end = bridgeProfile->maxAlong;

    const double rampLength = 20.0;

    // Подъём в начале моста.
    if (along < start + rampLength) {
      const double t = std::clamp((along - start) / rampLength, 0.0, 1.0);

      return static_cast<float>(t * t * (3.0 - 2.0 * t));
    }

    // Спуск в конце моста.
    if (along > end - rampLength) {
      const double t = std::clamp((end - along) / rampLength, 0.0, 1.0);

      return static_cast<float>(t * t * (3.0 - 2.0 * t));
    }

    // Ровная центральная часть.
    return 1.0f;
  };

  auto getGroundY = [&](const QVector2D &pos, float &y) -> bool {
    const double pixelX = (pos.x() / terrainSize.width + 0.5) * (dem.width - 1);

    const double pixelY =
        (0.5 - pos.y() / (rowDirection * terrainSize.height)) *
        (dem.height - 1);

    float elevation = 0.0f;

    if (!sampleElevation(pixelX, pixelY, elevation))
      return false;

    const float elevationRange = dem.maxElevation - dem.minElevation;

    const float verticalScale = elevationRange > 0.0f
                                    ? verticalScaleCoefficient / elevationRange
                                    : 0.001f;

    y = (elevation - dem.minElevation) * verticalScale;

    return true;
  };

  const Colors &color = feature.colors;

  // Размер одного пикселя DEM в координатах сцены.
  const float pixelSizeX =
      terrainSize.width / static_cast<float>(dem.width - 1);

  const float pixelSizeY =
      terrainSize.height / static_cast<float>(dem.height - 1);

  // Максимальная длина одного участка дороги.
  // Чем резче рельеф, тем меньше должен быть этот шаг.
  const float maxSegmentLength =
      std::max(0.0001f, std::min(pixelSizeX, pixelSizeY) * 1.5f);

  for (std::size_t i = 0; i + 1 < points.size(); ++i) {
    const ScenePoint &a = points[i];
    const ScenePoint &b = points[i + 1];

    const QVector2D direction = b.position - a.position;
    const float length = direction.length();

    if (length < 1e-7f)
      continue;

    const QVector2D normal(-direction.y() / length, direction.x() / length);

    const QVector2D offset = normal * halfWidth;

    // Разбиваем длинный сегмент на короткие участки.
    const int steps =
        std::max(1, static_cast<int>(std::ceil(length / maxSegmentLength)));

    for (int j = 0; j < steps; ++j) {
      const float t0 = static_cast<float>(j) / steps;
      const float t1 = static_cast<float>(j + 1) / steps;

      const QVector2D centerA = a.position * (1.0f - t0) + b.position * t0;

      const QVector2D centerB = a.position * (1.0f - t1) + b.position * t1;

      const QVector2D aLeft = centerA + offset;
      const QVector2D aRight = centerA - offset;

      const QVector2D bLeft = centerB + offset;
      const QVector2D bRight = centerB - offset;

      float yALeft, yARight;
      float yBLeft, yBRight;

      if (!getGroundY(aLeft, yALeft) || !getGroundY(aRight, yARight) ||
          !getGroundY(bLeft, yBLeft) || !getGroundY(bRight, yBRight)) {
        continue;
      }

      const double alongA =
          bridgeAlong[i] * (1.0 - t0) + bridgeAlong[i + 1] * t0;

      const double alongB =
          bridgeAlong[i] * (1.0 - t1) + bridgeAlong[i + 1] * t1;

      const float profileA = getBridgeProfile(alongA);
      const float profileB = getBridgeProfile(alongB);

      const float bridgeOffsetA = bridgeHeightMeters * verticalScale * profileA;

      const float bridgeOffsetB = bridgeHeightMeters * verticalScale * profileB;

      const float thicknessA = bridgeThicknessMeters * verticalScale * profileA;

      const float thicknessB = bridgeThicknessMeters * verticalScale * profileB;

      yALeft += heightOffset + bridgeOffsetA;
      yARight += heightOffset + bridgeOffsetA;

      yBLeft += heightOffset + bridgeOffsetB;
      yBRight += heightOffset + bridgeOffsetB;

      if (isBridge) {
        const float bottomALeft = yALeft - thicknessA;
        const float bottomARight = yARight - thicknessA;

        const float bottomBLeft = yBLeft - thicknessB;
        const float bottomBRight = yBRight - thicknessB;

        // Более тёмный цвет нижней части моста.
        const float sideR = color.roofR * 0.65f;
        const float sideG = color.roofG * 0.65f;
        const float sideB = color.roofB * 0.65f;

        // Левая боковая грань.
        addVertex(featureVertices, aLeft, yALeft, sideR, sideG, sideB);
        addVertex(featureVertices, bLeft, yBLeft, sideR, sideG, sideB);
        addVertex(featureVertices, bLeft, bottomBLeft, sideR, sideG, sideB);

        addVertex(featureVertices, aLeft, yALeft, sideR, sideG, sideB);
        addVertex(featureVertices, bLeft, bottomBLeft, sideR, sideG, sideB);
        addVertex(featureVertices, aLeft, bottomALeft, sideR, sideG, sideB);

        // Правая боковая грань.
        addVertex(featureVertices, aRight, yARight, sideR, sideG, sideB);
        addVertex(featureVertices, bRight, bottomBRight, sideR, sideG, sideB);
        addVertex(featureVertices, bRight, yBRight, sideR, sideG, sideB);

        addVertex(featureVertices, aRight, yARight, sideR, sideG, sideB);
        addVertex(featureVertices, bRight, bottomBRight, sideR, sideG, sideB);
        addVertex(featureVertices, aRight, bottomARight, sideR, sideG, sideB);

        // Нижняя поверхность моста.
        addVertex(featureVertices, aLeft, bottomALeft, sideR, sideG, sideB);
        addVertex(featureVertices, aRight, bottomARight, sideR, sideG, sideB);
        addVertex(featureVertices, bLeft, bottomBLeft, sideR, sideG, sideB);

        addVertex(featureVertices, aRight, bottomARight, sideR, sideG, sideB);
        addVertex(featureVertices, bRight, bottomBRight, sideR, sideG, sideB);
        addVertex(featureVertices, bLeft, bottomBLeft, sideR, sideG, sideB);
      }

      // Первый треугольник.
      addVertex(featureVertices, aLeft, yALeft, color.roofR, color.roofG,
                color.roofB);

      addVertex(featureVertices, aRight, yARight, color.roofR, color.roofG,
                color.roofB);

      addVertex(featureVertices, bLeft, yBLeft, color.roofR, color.roofG,
                color.roofB);

      // Второй треугольник.
      addVertex(featureVertices, aRight, yARight, color.roofR, color.roofG,
                color.roofB);

      addVertex(featureVertices, bRight, yBRight, color.roofR, color.roofG,
                color.roofB);

      addVertex(featureVertices, bLeft, yBLeft, color.roofR, color.roofG,
                color.roofB);
    }
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
  int bridges = 0;
  int water = 0;
  int vegetation = 0;
  int railways = 0;
  int other = 0;

  // Строим общий профиль для каждой группы мостовых линий.
  std::unordered_map<const OSMFeature *, BridgeProfile> bridgeProfiles;

  const auto bridgeGroups = groupBridges();

  for (const auto &group : bridgeGroups) {
    const BridgeProfile profile = buildBridgeProfile(group);

    for (const OSMFeature *feature : group) {
      if (feature)
        bridgeProfiles.emplace(feature, profile);
    }
  }

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
    case OSMFeatureType::Bridge: {
      if (feature.geometryType == OSMGeometryType::Polygon) {
        appendPolygon(feature, 0.0015f);
      } else if (feature.geometryType == OSMGeometryType::LineString) {
        const float width = feature.widthMeters > 0.0
                                ? static_cast<float>(feature.widthMeters)
                                : 4.0f;

        // Одна функция строит и дорогу, и мост.
        // Разница определяется внутри appendLine по feature.type.
        const BridgeProfile *profile = nullptr;

        if (feature.type == OSMFeatureType::Bridge) {
          const auto it = bridgeProfiles.find(&feature);

          if (it != bridgeProfiles.end())
            profile = &it->second;
        }
        appendLine(feature, width, 0.0015f, profile);
      }

      if (feature.type == OSMFeatureType::Bridge)
        ++bridges;
      else
        ++roads;

      break;
    }

    case OSMFeatureType::Water:
      if (feature.geometryType == OSMGeometryType::Polygon) {
        appendPolygon(feature, 0.0025f);
      } else if (feature.geometryType == OSMGeometryType::LineString) {
        const QString waterway = feature.tags.value("waterway").toString();

        float width = 5.0f;

        if (waterway == "river")
          width = 20.0f;
        else if (waterway == "stream")
          width = 2.0f;
        else if (waterway == "canal")
          width = 8.0f;
        else if (waterway == "ditch" || waterway == "drain")
          width = 1.5f;

        if (feature.widthMeters > 0.0)
          width = static_cast<float>(feature.widthMeters);

        appendLine(feature, width, 0.0025f, nullptr);
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

        appendLine(feature, width, 0.002f, nullptr);
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
           << "bridges =" << bridges << "water =" << water
           << "vegetation =" << vegetation << "railways =" << railways
           << "other =" << other << "vertices =" << featureVertices.size();
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
  zoom -= event->angleDelta().y() / 120.0f * 0.05f;

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

QVector<QVector<const OSMFeature *>> GLWidget::groupBridges() const {
  QVector<QVector<const OSMFeature *>> groups;
  QVector<bool> assigned(features.size(), false);

  constexpr double maxGroupDistanceMeters = 25.0;

  for (int i = 0; i < features.size(); ++i) {
    const OSMFeature &first = features[i];

    if (assigned[i] || first.type != OSMFeatureType::Bridge ||
        first.geometryType != OSMGeometryType::LineString ||
        first.geometry.size() < 2) {
      continue;
    }

    QVector<const OSMFeature *> group;
    group.push_back(&first);
    assigned[i] = true;

    const QString bridgeName = first.tags.value("bridge:name").toString();

    const QString layer = first.tags.value("layer").toString();

    // Без имени моста не пытаемся объединять линии:
    // оставляем отдельную линию самостоятельной группой.
    if (bridgeName.isEmpty()) {
      groups.push_back(group);
      continue;
    }

    // Пространственный центр первой линии.
    double refLat = 0.0;
    double refLon = 0.0;

    for (const GeoPoint &p : first.geometry) {
      refLat += p.lat;
      refLon += p.lon;
    }

    refLat /= first.geometry.size();
    refLon /= first.geometry.size();

    const double metersPerLon = 111320.0 * std::cos(qDegreesToRadians(refLat));

    auto bounds = [&](const OSMFeature &feature) {
      std::array<double, 4> b = {std::numeric_limits<double>::max(),
                                 std::numeric_limits<double>::max(),
                                 std::numeric_limits<double>::lowest(),
                                 std::numeric_limits<double>::lowest()};

      for (const GeoPoint &p : feature.geometry) {
        const double x = (p.lon - refLon) * metersPerLon;
        const double y = (p.lat - refLat) * 111132.0;

        b[0] = std::min(b[0], x);
        b[1] = std::min(b[1], y);
        b[2] = std::max(b[2], x);
        b[3] = std::max(b[3], y);
      }

      return b;
    };

    const auto firstBounds = bounds(first);

    // Добавляем только линии с тем же именем и layer,
    // расположенные рядом с первой линией.
    for (int j = i + 1; j < features.size(); ++j) {
      const OSMFeature &candidate = features[j];

      if (assigned[j] || candidate.type != OSMFeatureType::Bridge ||
          candidate.geometryType != OSMGeometryType::LineString ||
          candidate.geometry.size() < 2) {
        continue;
      }

      if (candidate.tags.value("bridge:name").toString() != bridgeName ||
          candidate.tags.value("layer").toString() != layer) {
        continue;
      }

      const auto candidateBounds = bounds(candidate);

      const double dx = std::max({0.0, firstBounds[0] - candidateBounds[2],
                                  candidateBounds[0] - firstBounds[2]});

      const double dy = std::max({0.0, firstBounds[1] - candidateBounds[3],
                                  candidateBounds[1] - firstBounds[3]});

      if (std::hypot(dx, dy) > maxGroupDistanceMeters)
        continue;

      group.push_back(&candidate);
      assigned[j] = true;
    }

    groups.push_back(group);
  }

  return groups;
}

GLWidget::BridgeProfile
GLWidget::buildBridgeProfile(const QVector<const OSMFeature *> &group) const {

  BridgeProfile profile;

  if (group.isEmpty())
    return profile;

  // Находим самую длинную линию группы.
  // По ней определяем общее направление моста.
  const OSMFeature *axisFeature = nullptr;
  double longestLength = 0.0;

  for (const OSMFeature *feature : group) {
    if (!feature || feature->geometry.size() < 2)
      continue;

    double length = 0.0;

    for (int i = 1; i < feature->geometry.size(); ++i)
      length +=
          geoDistanceMeters(feature->geometry[i - 1], feature->geometry[i]);

    if (length > longestLength) {
      longestLength = length;
      axisFeature = feature;
    }
  }

  if (!axisFeature || longestLength <= 0.0)
    return profile;

  profile.refLat = axisFeature->geometry.first().lat;
  profile.refLon = axisFeature->geometry.first().lon;

  const double metersPerLon =
      111320.0 * std::cos(qDegreesToRadians(profile.refLat));

  auto localPoint = [&](const GeoPoint &p) {
    return QVector2D(
        static_cast<float>((p.lon - profile.refLon) * metersPerLon),
        static_cast<float>((p.lat - profile.refLat) * 111132.0));
  };

  const QVector2D axisStart = localPoint(axisFeature->geometry.first());

  const QVector2D axisEnd = localPoint(axisFeature->geometry.last());

  QVector2D axis = axisEnd - axisStart;

  if (axis.lengthSquared() < 1e-8f)
    return profile;

  axis.normalize();

  profile.axisX = axis.x();
  profile.axisY = axis.y();

  profile.minAlong = std::numeric_limits<double>::max();
  profile.maxAlong = std::numeric_limits<double>::lowest();

  // Общие начало и конец профиля для всех линий группы.
  for (const OSMFeature *feature : group) {
    if (!feature)
      continue;

    for (const GeoPoint &p : feature->geometry) {
      const QVector2D local = localPoint(p);

      const double along = QVector2D::dotProduct(local, axis);

      profile.minAlong = std::min(profile.minAlong, along);
      profile.maxAlong = std::max(profile.maxAlong, along);
    }
  }

  if (profile.maxAlong <= profile.minAlong)
    return profile;

  profile.heightMeters = 5.0f;
  profile.rampLength = 20.0f;
  profile.valid = true;

  return profile;
}