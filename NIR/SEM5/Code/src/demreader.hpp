
#pragma once

#include <QString>
#include <vector>

struct DEMData {
  int width = 0;
  int height = 0;

  // Высоты: индекс = y * width + x
  std::vector<float> elevation;

  // Геотрансформация GDAL
  double geoTransform[6] = {};

  QString projectionWkt;

  float minElevation = 0.0f;
  float maxElevation = 0.0f;
};

class DEMReader {
public:
  static bool loadDem(const QString &fileName, DEMData &out);
};
