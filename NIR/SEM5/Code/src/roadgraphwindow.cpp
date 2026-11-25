
#include "roadgraphwindow.hpp"

#include <QBrush>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <QPen>
#include <QResizeEvent>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QtMath>
#include <qnamespace.h>

RoadGraphWindow::RoadGraphWindow(const RoadGraph &graph, QWidget *parent)
    : QMainWindow(parent) {
  setWindowTitle("Визуализация дорожного графа");
  resize(1100, 800);

  scene = new QGraphicsScene(this);

  view = new QGraphicsView(scene, this);

  view->setRenderHint(QPainter::Antialiasing);
  view->setRenderHint(QPainter::SmoothPixmapTransform);

  view->setDragMode(QGraphicsView::NoDrag);
  view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

  view->setResizeAnchor(QGraphicsView::AnchorViewCenter);

  view->setBackgroundBrush(QColor(245, 245, 245));

  view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  view->viewport()->installEventFilter(this);
  view->setMouseTracking(true);
  view->viewport()->setMouseTracking(true);

  setCentralWidget(view);

  drawGraph(graph);
}

void RoadGraphWindow::drawGraph(const RoadGraph &graph) {
  scene->clear();

  if (graph.nodes.isEmpty())
    return;

  double minLat = graph.nodes.first().lat;
  double maxLat = minLat;

  double minLon = graph.nodes.first().lon;
  double maxLon = minLon;

  for (const RoadNode &node : graph.nodes) {
    minLat = qMin(minLat, node.lat);
    maxLat = qMax(maxLat, node.lat);

    minLon = qMin(minLon, node.lon);
    maxLon = qMax(maxLon, node.lon);
  }

  const double centerLat = (minLat + maxLat) / 2.0;
  const double longitudeScale = qCos(qDegreesToRadians(centerLat));

  // Координаты переводятся в локальную плоскость.
  auto toScene = [&](const RoadNode &node) -> QPointF {
    double x = (node.lon - minLon) * longitudeScale;

    double y = -(node.lat - minLat);

    return QPointF(x, y);
  };

  // Определяем размер точек относительно размеров графа.
  double width = qMax((maxLon - minLon) * longitudeScale, 0.000001);

  double height = qMax(maxLat - minLat, 0.000001);

  double nodeRadius = qMax(width, height) * 0.003;
  nodeRadius = qMax(nodeRadius, 0.0000001);

  // Рисуем рёбра.
  QPen edgePen(QColor(40, 110, 210));
  edgePen.setWidthF(0);
  edgePen.setCosmetic(true);

  for (const RoadEdge &edge : graph.edges) {
    if (edge.from >= static_cast<quint32>(graph.nodes.size()) ||
        edge.to >= static_cast<quint32>(graph.nodes.size())) {
      continue;
    }

    const RoadNode &from = graph.nodes[edge.from];
    const RoadNode &to = graph.nodes[edge.to];

    QPointF p1 = toScene(from);
    QPointF p2 = toScene(to);

    scene->addLine(QLineF(p1, p2), edgePen);
  }

  // Определяем уникальных соседей каждой вершины.
  QVector<QSet<quint32>> neighbors(graph.nodes.size());

  for (const RoadEdge &edge : graph.edges) {
    if (edge.from >= static_cast<quint32>(graph.nodes.size()) ||
        edge.to >= static_cast<quint32>(graph.nodes.size())) {
      continue;
    }

    neighbors[edge.from].insert(edge.to);
    neighbors[edge.to].insert(edge.from);
  }

  // Рисуем вершины поверх рёбер.
  QPen nodePen(QColor(180, 40, 40));
  nodePen.setWidthF(0);
  nodePen.setCosmetic(true);

  QBrush nodeBrush(QColor(220, 55, 55));

  for (const RoadNode &node : graph.nodes) {

    int degree = neighbors[node.id].size();

    // Показываем только концы дорог и перекрёстки.
    if (degree != 1 && degree < 3)
      continue;

    QPointF p = toScene(node);

    scene->addEllipse(p.x() - nodeRadius, p.y() - nodeRadius, nodeRadius * 2.0,
                      nodeRadius * 2.0, nodePen, nodeBrush);
  }

  QRectF bounds = scene->itemsBoundingRect();

  if (bounds.isValid()) {
    bounds.adjust(-nodeRadius * 5, -nodeRadius * 5, nodeRadius * 5,
                  nodeRadius * 5);

    scene->setSceneRect(bounds);
    view->fitInView(bounds, Qt::KeepAspectRatio);
  }
}

bool RoadGraphWindow::eventFilter(QObject *watched, QEvent *event) {
  if (watched != view->viewport())
    return QMainWindow::eventFilter(watched, event);

  switch (event->type()) {

  case QEvent::MouseButtonPress: {
    auto *mouseEvent = static_cast<QMouseEvent *>(event);

    if (mouseEvent->button() == Qt::RightButton) {
      dragging = true;
      lastMousePosition = mouseEvent->position().toPoint();

      view->viewport()->setCursor(Qt::ClosedHandCursor);

      return true;
    }

    return true;
  }

  case QEvent::MouseMove: {
    auto *mouseEvent = static_cast<QMouseEvent *>(event);

    if (dragging) {
      QPoint currentPosition = mouseEvent->position().toPoint();

      QPoint delta = currentPosition - lastMousePosition;

      lastMousePosition = currentPosition;

      view->horizontalScrollBar()->setValue(
          view->horizontalScrollBar()->value() - delta.x());

      view->verticalScrollBar()->setValue(view->verticalScrollBar()->value() -
                                          delta.y());
    }

    return true;
  }

  case QEvent::MouseButtonRelease: {
    auto *mouseEvent = static_cast<QMouseEvent *>(event);

    if (mouseEvent->button() == Qt::RightButton) {
      dragging = false;
      view->viewport()->unsetCursor();

      return true;
    }

    return true;
  }

  case QEvent::Wheel: {
    auto *wheelEvent = static_cast<QWheelEvent *>(event);

    constexpr double factor = 1.15;

    if (wheelEvent->angleDelta().y() > 0) {
      view->scale(factor, factor);
    } else if (wheelEvent->angleDelta().y() < 0) {
      view->scale(1.0 / factor, 1.0 / factor);
    }

    return true;
  }

  default:
    break;
  }

  return QMainWindow::eventFilter(watched, event);
}