#include "osmclassifier.hpp"

OSMFeatureType OSMClassifier::classify(const QJsonObject &tags) {
  OSMFeatureType type = OSMFeatureType::Unknown;

  // Здания
  if (tags.contains("building") || tags.contains("building:part"))
    type |= OSMFeatureType::Building;

  // Дороги
  if (tags.contains("highway")) {

    type |= OSMFeatureType::Road;
  }

  // Пешеходные дороги
  const QString highway = tags.value("highway").toString();
  if (highway == "footway" || highway == "pedestrian" || highway == "path" ||
      highway == "steps" || highway == "corridor") {
    type |= OSMFeatureType::PedestrianRoad;
  }

  // Мосты
  const QString bridge = tags.value("bridge").toString();

  if (tags.contains("bridge") ||
      tags.value("man_made").toString() == "bridge") {
    type |= OSMFeatureType::Bridge;
  }

  // Железные дороги
  if (tags.contains("railway"))
    type |= OSMFeatureType::Railway;

  // Вода
  if ((tags.contains("waterway") ||
       tags.value("natural").toString() == "water" || tags.contains("water")) &&
      tags.value("tunnel").toString() != "culvert") {
    type |= OSMFeatureType::Water;
  }

  // Растительность
  const QString natural = tags.value("natural").toString();
  const QString landuse = tags.value("landuse").toString();
  const QString leisure = tags.value("leisure").toString();

  if (natural == "wood" || natural == "tree" || natural == "scrub" ||
      landuse == "forest" || leisure == "park") {
    type |= OSMFeatureType::Vegetation;
  }

  // Остальные территории
  if (tags.contains("landuse"))
    type |= OSMFeatureType::Landuse;

  // Общественные объекты
  if (tags.contains("amenity") || tags.contains("shop") ||
      tags.contains("office")) {
    type |= OSMFeatureType::Amenity;
  }

  return type;
}