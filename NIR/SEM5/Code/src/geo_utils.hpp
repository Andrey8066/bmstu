
#pragma once

#include <QPointF>
#include <QtMath>
#include <cmath>

constexpr double earthRadius = 6371000.0;

inline QPointF geoToLocal(double lat, double lon, double centerLat,
                          double centerLon) {
  const double x = earthRadius * qDegreesToRadians(lon - centerLon) *
                   std::cos(qDegreesToRadians(centerLat));

  const double y = earthRadius * qDegreesToRadians(lat - centerLat);

  return QPointF(x, y);
}
