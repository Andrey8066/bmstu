
#include "osmclassifier.hpp"

OSMFeatureType OSMClassifier::classify(const QJsonObject &tags) {
  // Здания
  if (tags.contains("building") || tags.contains("building:part"))
    return OSMFeatureType::Building;

  // Дороги
  if (tags.contains("highway")) {
    // Мосты
    if (tags.value("bridge").toString() != "" &&
        tags.value("bridge").toString() != "no") {
      return OSMFeatureType::Bridge;
    }
    if (tags.value("highway").toString() != "footway" &&
        tags.value("highway").toString() != "pedestrian" &&
        tags.value("highway").toString() != "path" &&
        tags.value("highway").toString() != "steps" &&
        tags.value("highway").toString() != "corridor") {
      return OSMFeatureType::PedestrianRoad;
    }
    return OSMFeatureType::Road;
  }

  // Железные дороги
  if (tags.contains("railway"))
    return OSMFeatureType::Railway;

  // Вода
  if (tags.contains("waterway") ||
      tags.value("natural").toString() == "water" || tags.contains("water")) {
    return OSMFeatureType::Water;
  }

  // Растительность
  const QString natural = tags.value("natural").toString();
  const QString landuse = tags.value("landuse").toString();
  const QString leisure = tags.value("leisure").toString();

  if (natural == "wood" || natural == "tree" || natural == "scrub" ||
      landuse == "forest" || leisure == "park") {
    return OSMFeatureType::Vegetation;
  }

  // Остальные территории
  if (tags.contains("landuse"))
    return OSMFeatureType::Landuse;

  // Общественные объекты
  if (tags.contains("amenity") || tags.contains("shop") ||
      tags.contains("office")) {
    return OSMFeatureType::Amenity;
  }

  return OSMFeatureType::Unknown;
}
