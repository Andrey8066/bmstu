#pragma once

#include "demreader.hpp"
#include "osmfeature.hpp"

#include <QString>
#include <QVector>

class ProjectSerializer {
public:
  static bool save(const QString &fileName, const DEMData &dem,
                   const QVector<OSMFeature> &features, float verticalScale,
                   QString *error = nullptr);

  static bool load(const QString &fileName, DEMData &dem,
                   QVector<OSMFeature> &features, float &verticalScale,
                   QString *error = nullptr);
};