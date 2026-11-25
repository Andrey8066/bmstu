
#pragma once

#include <QJsonObject>
#include <QVector>
#include <QtGlobal>

struct GeoPoint {
  double lat = 0.0;
  double lon = 0.0;
};

enum class OSMGeometryType { Point, LineString, Polygon };

enum class OSMFeatureType : quint32 {
  Unknown = 0,
  Building = 1 << 0,
  Road = 1 << 1,
  PedestrianRoad = 1 << 2,
  Bridge = 1 << 3,
  Water = 1 << 4,
  Vegetation = 1 << 5,
  Landuse = 1 << 6,
  Railway = 1 << 7,
  Amenity = 1 << 8
};

inline OSMFeatureType operator|(OSMFeatureType a, OSMFeatureType b) {
  return static_cast<OSMFeatureType>(static_cast<quint32>(a) |
                                     static_cast<quint32>(b));
}

inline OSMFeatureType operator&(OSMFeatureType a, OSMFeatureType b) {
  return static_cast<OSMFeatureType>(static_cast<quint32>(a) &
                                     static_cast<quint32>(b));
}

inline OSMFeatureType &operator|=(OSMFeatureType &a, OSMFeatureType b) {
  a = a | b;
  return a;
}

inline bool hasType(OSMFeatureType value, OSMFeatureType flag) {
  return (value & flag) != OSMFeatureType::Unknown;
}

struct Colors {
  float wallR = 1.0f;
  float wallG = 1.0f;
  float wallB = 1.0f;

  float roofR = 1.0f;
  float roofG = 1.0f;
  float roofB = 1.0f;

  Colors() = default;

  Colors(float wr, float wg, float wb, float rr, float rg, float rb)
      : wallR(wr), wallG(wg), wallB(wb), roofR(rr), roofG(rg), roofB(rb) {}
};

struct OSMFeature {
  qint64 id = 0;

  OSMFeatureType type = OSMFeatureType::Unknown;
  OSMGeometryType geometryType = OSMGeometryType::Point;

  QVector<GeoPoint> geometry;

  // Внутренние кольца (отверстия), используются только зданиями.
  QVector<QVector<GeoPoint>> innerRings;

  // Все исходные теги OSM
  QJsonObject tags;

  // Параметры геометрии
  float heightMeters = 0.0f;
  float minHeightMeters = 0.0f;
  float widthMeters = 0.0f;

  Colors colors;

  bool hasType(OSMFeatureType flag) const {
    return ::hasType(this->type, flag);
  };
};
