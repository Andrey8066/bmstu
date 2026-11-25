
#include "roadclassifier.hpp"

#include <QJsonObject>
#include <QJsonValue>

int RoadClassifier::getLanes(const QJsonObject &tags) const {
  QJsonValue value = tags.value("lanes");

  bool ok = false;

  int lanes = value.toString().toInt(&ok);

  if (!ok && value.isDouble()) {
    lanes = value.toInt();
    ok = true;
  }

  if (!ok || lanes <= 0)
    return 0;

  return lanes;
}

RoadCategory RoadClassifier::classifyFeature(const OSMFeature &feature,
                                             int &lanes) const {
  const QJsonObject &tags = feature.getTags();

  QString highway = tags.value("highway").toString();

  lanes = getLanes(tags);

  if (highway.isEmpty())
    return RoadCategory::Unknown;

  // Motorways
  if (highway == "motorway" || highway == "motorway_link") {
    return RoadCategory::Motorway;
  }

  // Pedestrian roads
  if (highway == "footway" || highway == "pedestrian" || highway == "path" ||
      highway == "steps" || highway == "bridleway") {
    return RoadCategory::Pedestrian;
  }

  // Other recognized road types
  static const QStringList carRoads = {
      "trunk",        "trunk_link",     "primary",       "primary_link",
      "secondary",    "secondary_link", "tertiary",      "tertiary_link",
      "unclassified", "residential",    "living_street", "service",
      "road"};

  if (!carRoads.contains(highway))
    return RoadCategory::Unknown;

  if (lanes == 0)
    return RoadCategory::CarUnknownLanes;

  if (lanes == 1)
    return RoadCategory::Car1Lane;

  if (lanes == 2)
    return RoadCategory::Car2Lanes;

  if (lanes == 3)
    return RoadCategory::Car3Lanes;

  if (lanes == 4)
    return RoadCategory::Car4Lanes;

  return RoadCategory::Car5PlusLanes;
}

QVector<RoadClassifier::ClassifiedRoad>
RoadClassifier::classify(const QVector<OSMFeature> &features) const {
  QVector<ClassifiedRoad> result;

  result.reserve(features.size());

  for (const OSMFeature &feature : features) {
    int lanes = 0;

    RoadCategory category = classifyFeature(feature, lanes);

    if (category == RoadCategory::Unknown)
      continue;

    ClassifiedRoad road;
    road.feature = feature;
    road.category = category;
    road.lanes = lanes;

    result.append(road);
  }

  return result;
}
