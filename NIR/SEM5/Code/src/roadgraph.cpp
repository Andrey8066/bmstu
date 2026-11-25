
#include "roadgraph.hpp"

void RoadGraph::clear() {
  nodes.clear();
  edges.clear();
}

quint32 RoadGraph::addNode(double lat, double lon) {
  quint32 id = static_cast<quint32>(nodes.size());

  RoadNode node;
  node.id = id;
  node.lat = lat;
  node.lon = lon;

  nodes.append(node);

  return id;
}

quint32 RoadGraph::addEdge(quint32 from, quint32 to, double length) {
  if (from >= static_cast<quint32>(nodes.size()) ||
      to >= static_cast<quint32>(nodes.size()) || from == to) {
    return UINT32_MAX;
  }

  quint32 id = static_cast<quint32>(edges.size());

  RoadEdge edge;
  edge.id = id;
  edge.from = from;
  edge.to = to;
  edge.length = length;

  edges.append(edge);

  nodes[from].edges.append(id);

  return id;
}

int RoadGraph::nodeCount() const { return nodes.size(); }

int RoadGraph::edgeCount() const { return edges.size(); }

const QVector<quint32> &RoadGraph::getEdges(quint32 nodeId) const {
  static const QVector<quint32> empty;

  if (nodeId >= static_cast<quint32>(nodes.size()))
    return empty;

  return nodes[nodeId].edges;
}
