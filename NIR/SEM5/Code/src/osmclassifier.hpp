
#pragma once

#include "osmfeature.hpp"

#include <QJsonObject>

class OSMClassifier {
public:
  static OSMFeatureType classify(const QJsonObject &tags);
};
