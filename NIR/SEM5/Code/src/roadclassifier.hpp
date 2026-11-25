
#pragma once

#include "osmfeature.hpp"
#include "roadcategory.hpp"

#include <QVector>

class RoadClassifier {
public:
  struct ClassifiedRoad {
    OSMFeature feature;
    RoadCategory category = RoadCategory::Unknown;
    int lanes = 0;
  };

  QVector<ClassifiedRoad> classify(const QVector<OSMFeature> &features) const;

private:
  RoadCategory classifyFeature(const OSMFeature &feature, int &lanes) const;

  int getLanes(const QJsonObject &tags) const;
};
