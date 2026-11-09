
#include "glwidget.hpp"

#include <QMouseEvent>
#include <QVector3D>
#include <QWheelEvent>
#include <QtMath>

#include <algorithm>
#include <array>
#include <cmath>
#include <numeric>
#include <qnamespace.h>

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

      indices.erase(indices.begin() + i);

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

} // namespace

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent) {
  setFocusPolicy(Qt::StrongFocus);
}

GLWidget::~GLWidget() {
  if (context() && context()->isValid()) {
    makeCurrent();

    vertexBuffer.destroy();
    buildingBuffer.destroy();

    doneCurrent();
  }
}

void GLWidget::initializeGL() {
  initializeOpenGLFunctions();

  glClearColor(0.08f, 0.10f, 0.13f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  // Совместимые GLSL-шейдеры для простого OpenGL.
  const char *vertexShader = R"(
        attribute vec3 position;
        attribute vec3 color;

        uniform mat4 mvp;

        varying vec3 terrainColor;

        void main()
        {
            gl_Position = mvp * vec4(position, 1.0);
            terrainColor = color;
        }
    )";

  const char *fragmentShader = R"(
        varying vec3 terrainColor;

        void main()
        {
            gl_FragColor = vec4(terrainColor, 1.0);
        }
    )";

  if (!shaderProgram.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                             vertexShader) ||
      !shaderProgram.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                             fragmentShader) ||
      !shaderProgram.link()) {
    qWarning() << "OpenGL shader error:" << shaderProgram.log();
    return;
  }

  vertexBuffer.create();
  vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);

  buildingBuffer.create();
  buildingBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);

  rebuildMesh();
  rebuildBuildingMesh();
}

void GLWidget::setDEM(const DEMData &data) {
  qDebug() << "setDEM called";
  qDebug() << "DEM size:" << data.width << data.height;
  qDebug() << "Elevation count:" << data.elevation.size();
  qDebug() << "Elevation range:" << data.minElevation << data.maxElevation;

  dem = data;
  rebuildMesh();
  rebuildBuildingMesh();
  update();
}

void GLWidget::rebuildMesh() {
  vertices.clear();
  meshReady = false;

  const int width = dem.width;
  const int height = dem.height;

  if (width < 2 || height < 2 ||
      dem.elevation.size() < static_cast<std::size_t>(width) * height) {
    update();
    return;
  }

  const float minZ = dem.minElevation;
  const float maxZ = dem.maxElevation;
  const float elevationRange = maxZ - minZ;

  // Сохраняем пропорции растра.
  const float aspect = static_cast<float>(width) / height;

  const float terrainWidth = aspect >= 1.0f ? 2.0f : 2.0f * aspect;

  const float terrainHeight = aspect >= 1.0f ? 2.0f / aspect : 2.0f;

  // Ограничиваем количество вершин для первого прототипа.
  // Большие DEM нужно предварительно уменьшать по разрешению.
  constexpr int maxDimension = 400;

  const int stepX =
      std::max(1, (width - 1 + maxDimension - 2) / (maxDimension - 1));

  const int stepY =
      std::max(1, (height - 1 + maxDimension - 2) / (maxDimension - 1));

  auto vertexAt = [&](int x, int y) -> Vertex {
    const float z = dem.elevation[static_cast<std::size_t>(y) * width + x];

    const float nx =
        (static_cast<float>(x) / (width - 1) - 0.5f) * terrainWidth;

    const float ny =
        (0.5f - static_cast<float>(y) / (height - 1)) * terrainHeight;

    // Вертикальное преувеличение для лучшей видимости рельефа.
    const float nz =
        elevationRange > 0.0f ? ((z - minZ) / elevationRange) * 0.12f : 0.0f;

    const float t = elevationRange > 0.0f
                        ? std::clamp((z - minZ) / elevationRange, 0.0f, 1.0f)
                        : 0.5f;

    // Цвет от зелёных низин к светлым вершинам.
    float r, g, b;

    if (t < 0.5f) {
      const float k = t * 2.0f;
      r = 0.12f + 0.30f * k;
      g = 0.35f + 0.30f * k;
      b = 0.12f + 0.05f * k;
    } else {
      const float k = (t - 0.5f) * 2.0f;
      r = 0.42f + 0.50f * k;
      g = 0.65f + 0.30f * k;
      b = 0.17f + 0.75f * k;
    }

    return {nx, nz, ny, r, g, b};
  };

  for (int y = 0; y < height - 1; y += stepY) {
    const int y2 = std::min(y + stepY, height - 1);

    for (int x = 0; x < width - 1; x += stepX) {
      const int x2 = std::min(x + stepX, width - 1);

      const Vertex a = vertexAt(x, y);
      const Vertex b = vertexAt(x2, y);
      const Vertex c = vertexAt(x, y2);
      const Vertex d = vertexAt(x2, y2);

      // Два треугольника на одну ячейку.
      vertices.insert(vertices.end(), {a, c, b, b, c, d});
    }
  }

  if (vertices.empty()) {
    update();
    return;
  }

  if (context() && context()->isValid()) {
    makeCurrent();

    if (!vertexBuffer.isCreated())
      vertexBuffer.create();

    vertexBuffer.bind();
    vertexBuffer.allocate(vertices.data(),
                          static_cast<int>(vertices.size() * sizeof(Vertex)));
    vertexBuffer.release();

    doneCurrent();
  }

  meshReady = true;
  update();
}

void GLWidget::resizeGL(int width, int height) {
  glViewport(0, 0, width, height);
}

void GLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  QMatrix4x4 projection;

  projection.perspective(
      45.0f, static_cast<float>(std::max(1, width())) / std::max(1, height()),
      0.01f, 100.0f);

  QMatrix4x4 view;

  view.translate(panX, panY, -zoom);

  view.rotate(rotationX, 1.0f, 0.0f, 0.0f);
  view.rotate(rotationZ, 0.0f, 1.0f, 0.0f);

  const QMatrix4x4 mvp = projection * view;

  auto drawBuffer = [&](QOpenGLBuffer &buffer, std::size_t vertexCount) {
    if (vertexCount == 0 || !buffer.isCreated() || !shaderProgram.isLinked())
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

  // Рисуем Terrain.
  if (meshReady)
    drawBuffer(vertexBuffer, vertices.size());

  // Рисуем здания.
  drawBuffer(buildingBuffer, buildingVertices.size());
}

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
        (2.0f * zoom * 0.41421356f) / std::max(1, height());

    panX += delta.x() * worldPerPixel;
    panY -= delta.y() * worldPerPixel;
    update();
  }

  lastMousePosition = current;
}

void GLWidget::wheelEvent(QWheelEvent *event) {
  zoom -= event->angleDelta().y() / 120.0f * 0.25f;
  zoom = std::clamp(zoom, 0.7f, 15.0f);
  update();
}

void GLWidget::setBuildings(
    const std::vector<BuildingFootprint> &newBuildings) {
  buildings = newBuildings;

  qDebug() << "setBuildings:" << buildings.size();

  rebuildBuildingMesh();

  update();
}

void GLWidget::rebuildBuildingMesh() {
  buildingVertices.clear();

  const int width = dem.width;
  const int height = dem.height;

  if (width < 2 || height < 2 ||
      dem.elevation.size() < static_cast<std::size_t>(width) * height) {
    qDebug() << "DEM ещё не загружен";
    return;
  }

  const double *gt = dem.geoTransform;

  const double determinant = gt[1] * gt[5] - gt[2] * gt[4];

  if (std::abs(determinant) < 1e-15)
    return;

  const float elevationRange = dem.maxElevation - dem.minElevation;

  const float verticalScale =
      elevationRange > 0.0f ? 0.12f / elevationRange : 0.001f;

  const float aspect = static_cast<float>(width) / height;

  const float terrainWidth = aspect >= 1.0f ? 2.0f : 2.0f * aspect;

  const float terrainHeight = aspect >= 1.0f ? 2.0f / aspect : 2.0f;

  // Географические координаты -> координаты пикселя.
  auto geoToPixel = [&](double lat, double lon, double &px, double &py) {
    const double dx = lon - gt[0];
    const double dy = lat - gt[3];

    px = (gt[5] * dx - gt[2] * dy) / determinant - 0.5;

    py = (-gt[4] * dx + gt[1] * dy) / determinant - 0.5;

    return px >= 0.0 && py >= 0.0 && px <= width - 1 && py <= height - 1;
  };

  // Получение высоты с билинейной интерполяцией.
  auto sampleElevation = [&](double px, double py, float &result) {
    if (px < 0.0 || py < 0.0 || px > width - 1 || py > height - 1)
      return false;

    const int x0 = static_cast<int>(std::floor(px));
    const int y0 = static_cast<int>(std::floor(py));

    const int x1 = std::min(x0 + 1, width - 1);
    const int y1 = std::min(y0 + 1, height - 1);

    const float fx = static_cast<float>(px - x0);
    const float fy = static_cast<float>(py - y0);

    const auto at = [&](int x, int y) {
      return dem.elevation[static_cast<std::size_t>(y) * width + x];
    };

    const float z00 = at(x0, y0);
    const float z10 = at(x1, y0);
    const float z01 = at(x0, y1);
    const float z11 = at(x1, y1);

    if (!std::isfinite(z00) || !std::isfinite(z10) || !std::isfinite(z01) ||
        !std::isfinite(z11))
      return false;

    const float z0 = z00 * (1.0f - fx) + z10 * fx;
    const float z1 = z01 * (1.0f - fx) + z11 * fx;

    result = z0 * (1.0f - fy) + z1 * fy;

    return true;
  };

  auto addVertex = [&](const QVector2D &p, float y, float r, float g, float b) {
    buildingVertices.push_back({p.x(), y, p.y(), r, g, b});
  };

  int skippedBuildings = 0;

  for (const BuildingFootprint &building : buildings) {
    if (building.outline.size() < 3)
      continue;

    std::vector<QVector2D> points;

    double elevationSum = 0.0;
    int elevationCount = 0;

    std::size_t pointCount = building.outline.size();

    // Убираем повтор первой точки в конце замкнутого контура.
    if (pointCount >= 2) {
      const auto &first = building.outline.front();
      const auto &last = building.outline.back();

      if (std::abs(first.lat - last.lat) < 1e-10 &&
          std::abs(first.lon - last.lon) < 1e-10) {
        --pointCount;
      }
    }

    bool valid = true;

    for (std::size_t i = 0; i < pointCount; ++i) {
      const GeoPoint &p = building.outline[i];

      double px = 0.0;
      double py = 0.0;

      if (!geoToPixel(p.lat, p.lon, px, py)) {
        valid = false;
        break;
      }

      float elevation = 0.0f;

      if (!sampleElevation(px, py, elevation)) {
        valid = false;
        break;
      }

      const float sceneX =
          (static_cast<float>(px) / (width - 1) - 0.5f) * terrainWidth;

      const float sceneZ =
          (0.5f - static_cast<float>(py) / (height - 1)) * terrainHeight;

      points.emplace_back(sceneX, sceneZ);

      elevationSum += elevation;
      ++elevationCount;
    }

    if (!valid || points.size() < 3 || elevationCount == 0) {
      ++skippedBuildings;
      continue;
    }

    // Убираем совпадающие соседние вершины.
    std::vector<QVector2D> cleanPoints;

    for (const QVector2D &p : points) {
      if (cleanPoints.empty() ||
          (p - cleanPoints.back()).lengthSquared() > 1e-12f) {
        cleanPoints.push_back(p);
      }
    }

    if (cleanPoints.size() < 3) {
      ++skippedBuildings;
      continue;
    }

    const auto roofTriangles = triangulatePolygon(cleanPoints);

    if (roofTriangles.empty()) {
      ++skippedBuildings;
      continue;
    }

    // Средняя высота земли под контуром.
    const float groundElevation =
        static_cast<float>(elevationSum / elevationCount);

    const float baseY =
        (groundElevation - dem.minElevation + 10) * verticalScale;

    const float roofY = baseY + building.heightMeters * verticalScale;

    // Цвета стен и крыш.
    constexpr float wallR = 0.58f;
    constexpr float wallG = 0.63f;
    constexpr float wallB = 0.70f;

    constexpr float roofR = 0.28f;
    constexpr float roofG = 0.36f;
    constexpr float roofB = 0.28f;

    const int n = static_cast<int>(cleanPoints.size());

    // Стены: два треугольника на каждую сторону.
    for (int i = 0; i < n; ++i) {
      const int next = (i + 1) % n;

      const auto &p0 = cleanPoints[i];
      const auto &p1 = cleanPoints[next];

      addVertex(p0, baseY, wallR, wallG, wallB);
      addVertex(p1, baseY, wallR, wallG, wallB);
      addVertex(p1, roofY, wallR, wallG, wallB);

      addVertex(p0, baseY, wallR, wallG, wallB);
      addVertex(p1, roofY, wallR, wallG, wallB);
      addVertex(p0, roofY, wallR, wallG, wallB);
    }

    // Треугольники крыши.
    for (const auto &triangle : roofTriangles) {
      for (int index : triangle) {
        addVertex(cleanPoints[index], roofY, roofR, roofG, roofB);
      }
    }
  }

  qDebug() << "Building vertices:" << buildingVertices.size();
  qDebug() << "Buildings skipped:" << skippedBuildings;

  // Загружаем геометрию в GPU.
  if (context() && context()->isValid()) {
    makeCurrent();

    if (!buildingBuffer.isCreated())
      buildingBuffer.create();

    if (!buildingVertices.empty()) {
      buildingBuffer.bind();

      buildingBuffer.allocate(
          buildingVertices.data(),
          static_cast<int>(buildingVertices.size() * sizeof(Vertex)));

      buildingBuffer.release();
    }

    doneCurrent();
  }

  update();
}