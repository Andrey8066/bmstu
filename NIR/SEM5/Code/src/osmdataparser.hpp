
#pragma once

#include "osmfeature.hpp"

#include <QJsonArray>
#include <QVector>

class OSMDataParser {
public:
  static QVector<OSMFeature> parse(const QJsonArray &elements);
};
