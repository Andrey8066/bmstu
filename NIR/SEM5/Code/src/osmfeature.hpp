
#pragma once

#include <QJsonObject>
#include <QVector>
#include <QtGlobal>
#include <qcontainerfwd.h>
#include <qjsonvalue.h>
#include <qobject.h>
#include <qtypes.h>

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

class OSMFeature {
private:
  qint64 id = 0;
  OSMFeatureType type = OSMFeatureType::Unknown;
  OSMGeometryType geometryType = OSMGeometryType::Point;

  QVector<GeoPoint> geometry;
  QVector<QVector<GeoPoint>> innerRings;

  QJsonObject tags;

  float heightMeters = 0.0f;
  float minHeightMeters = 0.0f;
  float widthMeters = 0.0f;

  Colors colors;

public:
  qint64 getId() const { return id; }

  void setId(qint64 value) { id = value; }

  OSMFeatureType getType() const { return type; }

  void setType(OSMFeatureType value) { type = value; }

  void addType(OSMFeatureType value) { type |= value; }

  bool hasType(OSMFeatureType flag) const {
    return ::hasType(this->type, flag);
  }

  OSMGeometryType getGeometryType() const { return geometryType; }

  void setGeometryType(OSMGeometryType value) { geometryType = value; }

  const QVector<GeoPoint> &getGeometry() const { return geometry; }

  void pushBackGeometry(const GeoPoint &value) { geometry.push_back(value); }
  void popBackGeometry() { geometry.pop_back(); }

  void setGeometry(const QVector<GeoPoint> &value) { geometry = value; }

  size_t getGeometrySize() const { return geometry.size(); }

  QVector<GeoPoint> &getGeometry() { return geometry; }

  const QVector<QVector<GeoPoint>> &getInnerRings() const { return innerRings; }

  void pushBackInnerRings(const QVector<GeoPoint> &value) {
    innerRings.push_back(value);
  }
  void popBackInnerRings() { innerRings.pop_back(); }
  void clearInnerRings() { innerRings.clear(); }
  void reserveInnerRings(qsizetype ringCount) { innerRings.reserve(ringCount); }

  void setInnerRings(const QVector<QVector<GeoPoint>> &value) {
    innerRings = value;
  }

  const QJsonObject &getTags() const { return tags; }

  const bool isTag(QString tagName, QString tagValue) const {
    return this->tags.value(tagName).toString() == tagValue;
  }

  const bool isTagsContains(QString tagName) const {
    return tags.contains(tagName);
  }

  const QString getTag(QString tagName) const {
    return tags.value(tagName).toString();
  }

  void setTags(const QJsonObject &value) { tags = value; }

  float getHeightMeters() const { return heightMeters; }

  void setHeightMeters(float value) { heightMeters = value; }

  float getMinHeightMeters() const { return minHeightMeters; }

  void setMinHeightMeters(float value) { minHeightMeters = value; }

  float getWidthMeters() const { return widthMeters; }

  void setWidthMeters(float value) { widthMeters = value; }

  Colors getColors() const { return colors; }

  void setColors(const Colors &value) { colors = value; }

  friend QDataStream &operator>>(QDataStream &in, OSMFeature &f) {
    in >> f.heightMeters >> f.minHeightMeters >> f.widthMeters;

    in >> f.colors.wallR >> f.colors.wallG >> f.colors.wallB;
    in >> f.colors.roofR >> f.colors.roofG >> f.colors.roofB;

    return in;
  }
};
