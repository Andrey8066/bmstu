
#pragma once

#include "roadclassifier.hpp"
#include "roadgraph.hpp"

#include <QHash>
#include <QString>

class RoadGraphBuilder {
public:
  struct Settings {
    double coordinateToleranceMeters = 1.0;
  };

  RoadGraphBuilder();
  explicit RoadGraphBuilder(const Settings &settings);

  RoadGraph build(const QVector<RoadClassifier::ClassifiedRoad> &roads);

private:
  Settings settings;

  // Ключ координаты и слоя для поиска существующего узла.
  QHash<QString, quint32> nodeLookup;

  quint32 getOrCreateNode(RoadGraph &graph, const GeoPoint &point, int layer);

  double calculateDistance(const GeoPoint &a, const GeoPoint &b) const;

  int getLayer(const QJsonObject &tags) const;

  void processFeature(RoadGraph &graph,
                      const RoadClassifier::ClassifiedRoad &road);
};
