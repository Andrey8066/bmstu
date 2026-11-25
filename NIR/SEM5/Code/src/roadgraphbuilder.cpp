
#include "roadgraphbuilder.hpp"

#include <QJsonObject>
#include <QJsonValue>
#include <QtMath>

RoadGraphBuilder::RoadGraphBuilder() = default;

RoadGraphBuilder::RoadGraphBuilder(const Settings &settings)
    : settings(settings) {}

int RoadGraphBuilder::getLayer(const QJsonObject &tags) const {
  bool ok = false;

  int layer = tags.value("layer").toString().toInt(&ok);

  if (!ok)
    layer = static_cast<int>(tags.value("layer").toDouble(0));

  return layer;
}

// -----------------------------------------------------------------------------
// Distance (Haversine)
// -----------------------------------------------------------------------------

double RoadGraphBuilder::calculateDistance(const GeoPoint &a,
                                           const GeoPoint &b) const {
  constexpr double EarthRadius = 6371000.0;

  double lat1 = qDegreesToRadians(a.lat);
  double lat2 = qDegreesToRadians(b.lat);

  double dLat = lat2 - lat1;
  double dLon = qDegreesToRadians(b.lon - a.lon);

  double h = qPow(qSin(dLat / 2.0), 2) +
             qCos(lat1) * qCos(lat2) * qPow(qSin(dLon / 2.0), 2);

  double c = 2.0 * qAtan2(qSqrt(h), qSqrt(1.0 - h));

  return EarthRadius * c;
}

// -----------------------------------------------------------------------------
// Node creation
// -----------------------------------------------------------------------------

quint32 RoadGraphBuilder::getOrCreateNode(RoadGraph &graph,
                                          const GeoPoint &point, int layer) {
  // Approximate coordinate quantization.
  // Latitude degrees converted to meters.
  constexpr double EarthRadius = 6371000.0;

  double latMeters = qDegreesToRadians(point.lat) * EarthRadius;

  double lonMeters = qDegreesToRadians(point.lon) * EarthRadius *
                     qCos(qDegreesToRadians(point.lat));

  double tolerance = qMax(settings.coordinateToleranceMeters, 0.01);

  qint64 x = qRound64(lonMeters / tolerance);
  qint64 y = qRound64(latMeters / tolerance);

  // Search nearby cells to avoid missing points
  // located on opposite sides of a quantization boundary.

  for (int dx = -1; dx <= 1; ++dx) {
    for (int dy = -1; dy <= 1; ++dy) {

      QString key = QString("%1_%2_%3").arg(x + dx).arg(y + dy).arg(layer);

      auto it = nodeLookup.constFind(key);

      if (it == nodeLookup.constEnd())
        continue;

      quint32 id = it.value();

      if (id >= static_cast<quint32>(graph.nodes.size()))
        continue;

      const RoadNode &node = graph.nodes[id];

      GeoPoint existing;
      existing.lat = node.lat;
      existing.lon = node.lon;

      if (calculateDistance(point, existing) <= tolerance)
        return id;
    }
  }

  quint32 id = graph.addNode(point.lat, point.lon);

  QString key = QString("%1_%2_%3").arg(x).arg(y).arg(layer);

  nodeLookup.insert(key, id);

  return id;
}

// -----------------------------------------------------------------------------
// Process feature
// -----------------------------------------------------------------------------

void RoadGraphBuilder::processFeature(
    RoadGraph &graph, const RoadClassifier::ClassifiedRoad &road) {
  const OSMFeature &feature = road.feature;

  const QVector<GeoPoint> &geometry = feature.getGeometry();

  if (geometry.size() < 2)
    return;

  const QJsonObject &tags = feature.getTags();

  QString highway = tags.value("highway").toString();

  QString name = tags.value("name").toString();

  int layer = getLayer(tags);

  QString oneway = tags.value("oneway").toString();

  bool oneWay =
      oneway == "yes" || oneway == "1" || oneway == "true" || oneway == "-1";

  bool reverse = oneway == "-1";

  QString bridgeTag = tags.value("bridge").toString();
  QString tunnelTag = tags.value("tunnel").toString();

  bool bridge = bridgeTag == "yes" || bridgeTag == "true";

  bool tunnel = tunnelTag == "yes" || tunnelTag == "true";

  for (int i = 0; i < geometry.size() - 1; ++i) {
    const GeoPoint &a = geometry[i];
    const GeoPoint &b = geometry[i + 1];

    double length = calculateDistance(a, b);

    if (length < 0.01)
      continue;

    quint32 from = getOrCreateNode(graph, a, layer);
    quint32 to = getOrCreateNode(graph, b, layer);

    if (from == to)
      continue;

    auto addRoadEdge = [&](quint32 edgeFrom, quint32 edgeTo, bool isReverse) {
      quint32 edgeId = graph.addEdge(edgeFrom, edgeTo, length);

      if (edgeId == UINT32_MAX)
        return;

      RoadEdge &edge = graph.edges[edgeId];

      edge.featureId = feature.getId();
      edge.highway = highway;
      edge.name = name;

      edge.layer = layer;

      edge.oneWay = oneWay;
      edge.isBridge = bridge;
      edge.isTunnel = tunnel;

      edge.category = road.category;
      edge.lanes = road.lanes;

      edge.isPedestrian = road.category == RoadCategory::Pedestrian;

      edge.isReverse = isReverse;
    };

    if (reverse) {
      addRoadEdge(to, from, true);
    } else {
      addRoadEdge(from, to, false);
    }

    if (!oneWay) {
      addRoadEdge(to, from, true);
    }
  }
}

// -----------------------------------------------------------------------------
// Build graph
// -----------------------------------------------------------------------------

RoadGraph
RoadGraphBuilder::build(const QVector<RoadClassifier::ClassifiedRoad> &roads) {
  RoadGraph graph;

  nodeLookup.clear();

  for (const RoadClassifier::ClassifiedRoad &road : roads) {
    processFeature(graph, road);
  }

  return graph;
}
