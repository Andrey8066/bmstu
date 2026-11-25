
#pragma once

#include "roadgraph.hpp"

#include <QMainWindow>

class QGraphicsScene;
class QGraphicsView;

class RoadGraphWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit RoadGraphWindow(const RoadGraph &graph, QWidget *parent = nullptr);

protected:
  bool eventFilter(QObject *watched, QEvent *event) override;

private:
  QGraphicsScene *scene;
  QGraphicsView *view;
  bool dragging = false;
  QPoint lastMousePosition;

  void drawGraph(const RoadGraph &graph);
};
