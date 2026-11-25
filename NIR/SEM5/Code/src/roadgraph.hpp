
#pragma once

#include <QString>
#include <QVector>
#include <QtGlobal>

#include "roadcategory.hpp"

struct RoadNode {
  quint32 id = 0;

  double lat = 0.0;
  double lon = 0.0;

  QVector<quint32> edges;
};

struct RoadEdge {
  quint32 id = 0;

  quint32 from = 0;
  quint32 to = 0;

  qint64 featureId = 0;

  double length = 0.0;

  QString highway;
  QString name;

  int layer = 0;

  bool oneWay = false;
  bool isBridge = false;
  bool isTunnel = false;
  bool isPedestrian = false;
  bool isReverse = false;

  RoadCategory category = RoadCategory::Unknown;
  int lanes = 0;
};

class RoadGraph {
public:
  QVector<RoadNode> nodes;
  QVector<RoadEdge> edges;

  void clear();

  quint32 addNode(double lat, double lon);

  quint32 addEdge(quint32 from, quint32 to, double length);

  int nodeCount() const;
  int edgeCount() const;

  const QVector<quint32> &getEdges(quint32 nodeId) const;
};
