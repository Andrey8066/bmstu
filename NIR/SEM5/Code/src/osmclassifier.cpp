
#include "osmclassifier.hpp"

OSMFeatureType OSMClassifier::classify(const QJsonObject &tags) {
  // Здания
  if (tags.contains("building"))
    return OSMFeatureType::Building;

  // Дороги
  if (tags.contains("highway"))
    return OSMFeatureType::Road;

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
