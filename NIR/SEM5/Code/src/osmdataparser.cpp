
#include "osmdataparser.hpp"
#include "osmclassifier.hpp"

#include <QJsonObject>
#include <QJsonValue>
#include <QString>

#include <algorithm>
#include <cmath>
#include <utility>

namespace {

double parseLength(const QJsonValue &value, bool *ok) {
  *ok = false;

  double result = 0.0;

  if (value.isDouble()) {
    result = value.toDouble();
    *ok = true;
    return result;
  }

  QString text = value.toString().trimmed().toLower();

  bool feet = text.endsWith("ft");

  if (feet)
    text.chop(2);
  else if (text.endsWith("m"))
    text.chop(1);

  result = text.trimmed().toDouble(ok);

  if (feet && *ok)
    result *= 0.3048;

  return result;
}

float parseBuildingHeight(const QJsonObject &tags) {
  bool ok = false;

  double height = parseLength(tags.value("height"), &ok);

  if (ok && height > 0.0)
    return static_cast<float>(std::clamp(height, 2.0, 300.0));

  const double levels = tags.value("building:levels").toString().toDouble(&ok);

  if (ok && levels > 0.0) {
    return static_cast<float>(std::clamp(levels * 3.0, 2.0, 300.0));
  }

  return 9.0f;
}

float parseRoadWidth(const QJsonObject &tags) {
  bool ok = false;

  double width = parseLength(tags.value("width"), &ok);

  if (ok && width > 0.0)
    return static_cast<float>(width);

  const double lanes = tags.value("lanes").toString().toDouble(&ok);

  if (ok && lanes > 0.0)
    return static_cast<float>(lanes * 3.5);

  const QString highway = tags.value("highway").toString();

  if (highway == "motorway" || highway == "trunk")
    return 12.0f;

  if (highway == "primary" || highway == "secondary")
    return 9.0f;

  if (highway == "residential")
    return 6.0f;

  if (highway == "footway" || highway == "path" || highway == "cycleway")
    return 2.0f;

  return 5.0f;
}

Colors getBuildingColors(const QJsonObject &tags) {
  const QString type = tags.value("building").toString();

  if (type == "industrial") {
    return Colors(0.55f, 0.55f, 0.55f, 0.35f, 0.35f, 0.35f);
  }

  if (type == "commercial" || type == "retail") {
    return Colors(0.72f, 0.62f, 0.48f, 0.35f, 0.32f, 0.28f);
  }

  if (type == "house") {
    return Colors(0.75f, 0.65f, 0.52f, 0.45f, 0.25f, 0.20f);
  }

  return Colors(0.58f, 0.63f, 0.70f, 0.28f, 0.36f, 0.28f);
}

bool samePoint(const GeoPoint &a, const GeoPoint &b) {
  constexpr double eps = 1e-9;

  return std::abs(a.lat - b.lat) < eps && std::abs(a.lon - b.lon) < eps;
}

// Читает координаты одного way из участника relation.
QVector<GeoPoint> readMemberGeometry(const QJsonObject &member) {
  QVector<GeoPoint> points;

  const QJsonArray geometry = member.value("geometry").toArray();

  for (const QJsonValue &value : geometry) {
    const QJsonObject point = value.toObject();

    if (!point.contains("lat") || !point.contains("lon"))
      continue;

    points.push_back(
        {point.value("lat").toDouble(), point.value("lon").toDouble()});
  }

  return points;
}

// Собирает внешние кольца relation из нескольких way.
QVector<QVector<GeoPoint>> readOuterRings(const QJsonObject &object) {
  QVector<QVector<GeoPoint>> segments;

  const QJsonArray members = object.value("members").toArray();

  for (const QJsonValue &value : members) {
    const QJsonObject member = value.toObject();

    if (member.value("type").toString() != "way")
      continue;

    if (member.value("role").toString() != "outer")
      continue;

    QVector<GeoPoint> points = readMemberGeometry(member);

    if (points.size() >= 2)
      segments.push_back(std::move(points));
  }

  QVector<QVector<GeoPoint>> rings;

  while (!segments.isEmpty()) {
    QVector<GeoPoint> ring = segments.takeFirst();

    bool changed = true;

    while (changed && !segments.isEmpty()) {
      changed = false;

      for (int i = 0; i < segments.size(); ++i) {
        QVector<GeoPoint> segment = segments[i];

        if (samePoint(ring.back(), segment.front())) {
          for (int j = 1; j < segment.size(); ++j)
            ring.push_back(segment[j]);

        } else if (samePoint(ring.back(), segment.back())) {
          std::reverse(segment.begin(), segment.end());

          for (int j = 1; j < segment.size(); ++j)
            ring.push_back(segment[j]);

        } else if (samePoint(ring.front(), segment.back())) {
          for (int j = 0; j < segment.size() - 1; ++j)
            ring.prepend(segment[j]);

        } else if (samePoint(ring.front(), segment.front())) {
          std::reverse(segment.begin(), segment.end());

          for (int j = 0; j < segment.size() - 1; ++j)
            ring.prepend(segment[j]);

        } else {
          continue;
        }

        segments.removeAt(i);
        changed = true;
        break;
      }
    }

    // Незамкнутые контуры не передаём рендереру.
    if (ring.size() < 4 || !samePoint(ring.front(), ring.back()))
      continue;

    // Убираем повтор последней точки.
    ring.removeLast();

    if (ring.size() >= 3)
      rings.push_back(std::move(ring));
  }

  return rings;
}

bool readGeometry(const QJsonObject &object, OSMFeature &feature) {
  const QString osmType = object.value("type").toString();

  if (osmType == "node") {

    if (!object.contains("lat") || !object.contains("lon")) {
      return false;
    }

    feature.geometryType = OSMGeometryType::Point;

    feature.geometry.push_back(
        {object.value("lat").toDouble(), object.value("lon").toDouble()});

    return true;
  }

  if (osmType == "way") {

    const QJsonArray geometry = object.value("geometry").toArray();

    for (const QJsonValue &value : geometry) {

      const QJsonObject point = value.toObject();

      if (!point.contains("lat") || !point.contains("lon")) {
        continue;
      }

      feature.geometry.push_back(
          {point.value("lat").toDouble(), point.value("lon").toDouble()});
    }

    if (feature.geometry.isEmpty())
      return false;

    const bool closed =
        feature.geometry.size() >= 4 &&
        std::abs(feature.geometry.front().lat - feature.geometry.back().lat) <
            1e-10 &&
        std::abs(feature.geometry.front().lon - feature.geometry.back().lon) <
            1e-10;

    const bool area = feature.tags.value("area").toString() == "yes" ||
                      (feature.tags.contains("building") && closed) ||
                      (feature.tags.contains("landuse") && closed) ||
                      (feature.tags.contains("leisure") && closed) ||
                      (feature.tags.contains("natural") && closed) ||
                      (feature.tags.contains("water") && closed);

    if (area) {
      feature.geometryType = OSMGeometryType::Polygon;

      if (closed)
        feature.geometry.pop_back();
    } else {
      feature.geometryType = OSMGeometryType::LineString;
    }

    return true;
  }

  // Relation пока не обрабатываем
  return false;
}

} // namespace

QVector<OSMFeature> OSMDataParser::parse(const QJsonArray &elements) {
  QVector<OSMFeature> result;

  result.reserve(elements.size());

  // ID way, которые уже войдут в геометрию relation.
  QSet<qint64> relationWayIds;

  // Первый проход: собираем ID внешних way поддерживаемых relation.
  for (const QJsonValue &value : elements) {
    if (!value.isObject())
      continue;

    const QJsonObject object = value.toObject();

    if (object.value("type").toString() != "relation")
      continue;

    const QJsonObject tags = object.value("tags").toObject();
    const QString relationType = tags.value("type").toString();

    if (relationType != "multipolygon" && relationType != "boundary" &&
        relationType != "building") {
      continue;
    }

    const QJsonArray members = object.value("members").toArray();

    for (const QJsonValue &memberValue : members) {
      const QJsonObject member = memberValue.toObject();

      if (member.value("type").toString() != "way")
        continue;

      if (member.value("role").toString() != "outer")
        continue;

      relationWayIds.insert(
          static_cast<qint64>(member.value("ref").toDouble()));
    }
  }

  // Второй проход: обрабатываем объекты.
  for (const QJsonValue &value : elements) {
    if (!value.isObject())
      continue;

    const QJsonObject object = value.toObject();

    const QString osmType = object.value("type").toString();

    // Не обрабатываем way повторно, если он уже вошёл в relation.
    if (osmType == "way") {
      const qint64 wayId = static_cast<qint64>(object.value("id").toDouble());

      if (relationWayIds.contains(wayId))
        continue;
    }

    OSMFeature feature;

    feature.id = static_cast<qint64>(object.value("id").toDouble());

    feature.tags = object.value("tags").toObject();

    feature.type = OSMClassifier::classify(feature.tags);

    if (feature.type == OSMFeatureType::Unknown)
      continue;

    // Relation обрабатываем отдельно.
    if (osmType == "relation") {
      const QString relationType = feature.tags.value("type").toString();

      if (relationType != "multipolygon" && relationType != "boundary" &&
          relationType != "building") {
        continue;
      }

      const auto rings = readOuterRings(object);

      for (const auto &ring : rings) {
        OSMFeature relationFeature = feature;

        relationFeature.geometryType = OSMGeometryType::Polygon;

        relationFeature.geometry = ring;

        if (relationFeature.type == OSMFeatureType::Building) {
          relationFeature.heightMeters =
              parseBuildingHeight(relationFeature.tags);

          relationFeature.colors = getBuildingColors(relationFeature.tags);
        }

        result.push_back(std::move(relationFeature));
      }

      continue;
    }

    // Обычные node и way.
    if (!readGeometry(object, feature))
      continue;

    if (feature.type == OSMFeatureType::Building) {
      feature.heightMeters = parseBuildingHeight(feature.tags);
      feature.colors = getBuildingColors(feature.tags);
    }

    if (feature.type == OSMFeatureType::Road) {
      feature.widthMeters = parseRoadWidth(feature.tags);
    }

    result.push_back(std::move(feature));
  }

  return result;
}
