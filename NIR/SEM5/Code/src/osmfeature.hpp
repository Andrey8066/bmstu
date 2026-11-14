
#pragma once

#include <QJsonObject>
#include <QVector>
#include <QtGlobal>

struct GeoPoint {
  double lat = 0.0;
  double lon = 0.0;
};

enum class OSMGeometryType { Point, LineString, Polygon };

enum class OSMFeatureType {
  Unknown,
  Building,
  Road,
  Water,
  Vegetation,
  Landuse,
  Railway,
  Amenity
};

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

  // Все исходные теги OSM
  QJsonObject tags;

  // Параметры геометрии
  float heightMeters = 0.0f;
  float widthMeters = 0.0f;

  Colors colors;
};
