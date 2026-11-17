
#include "demreader.hpp"

#include <gdal_priv.h>

#include <algorithm>
#include <cmath>
#include <limits>

#include <QDebug>

bool DEMReader::loadDem(const QString &fileName, DEMData &out) {
  GDALAllRegister();
  out = DEMData{};

  GDALDataset *dataset = static_cast<GDALDataset *>(
      GDALOpen(fileName.toLocal8Bit().constData(), GA_ReadOnly));

  if (!dataset) {
    qWarning() << "Не удалось открыть DEM:" << fileName;
    return false;
  }

  DEMData result;

  const int width = dataset->GetRasterXSize();
  const int height = dataset->GetRasterYSize();

  if (width < 2 || height < 2 || dataset->GetRasterCount() < 1) {
    GDALClose(dataset);
    return false;
  }

  GDALRasterBand *band = dataset->GetRasterBand(1);

  if (!band) {
    GDALClose(dataset);
    return false;
  }

  const char *projection = dataset->GetProjectionRef();
  result.projectionWkt = projection ? QString::fromUtf8(projection) : QString();

  if (dataset->GetGeoTransform(result.geoTransform) != CE_None) {
    GDALClose(dataset);
    return false;
  }

  result.width = width;
  result.height = height;

  int hasNoData = 0;
  const double noData = band->GetNoDataValue(&hasNoData);

  const double scale = band->GetScale();
  const double offset = band->GetOffset();

  const double actualScale = band->GetScale() == 0.0 ? 1.0 : scale;

  const double actualOffset = band->GetOffset();

  qDebug() << "DEM:" << width << "x" << height;
  qDebug() << "Projection:" << result.projectionWkt;
  qDebug() << "NoData:" << hasNoData << noData;
  qDebug() << "Scale:" << actualScale << "Offset:" << actualOffset;

  result.elevation.resize(static_cast<std::size_t>(width) * height);

  const CPLErr readResult =
      band->RasterIO(GF_Read, 0, 0, width, height, result.elevation.data(),
                     width, height, GDT_Float32, 0, 0);

  GDALClose(dataset);

  if (readResult != CE_None) {
    return false;
  }

  float minZ = std::numeric_limits<float>::max();
  float maxZ = std::numeric_limits<float>::lowest();

  bool hasValidHeight = false;

  for (float &z : result.elevation) {
    if (!std::isfinite(z) || (hasNoData && z == static_cast<float>(noData))) {
      z = 0.0f;
      continue;
    }

    z = static_cast<float>(static_cast<double>(z) * actualScale + actualOffset);

    minZ = std::min(minZ, z);
    maxZ = std::max(maxZ, z);

    hasValidHeight = true;
  }

  if (!hasValidHeight) {
    return false;
  }

  result.minElevation = minZ;
  result.maxElevation = maxZ;

  out = std::move(result);

  qDebug() << "DEM loaded:" << out.width << "x" << out.height
           << "elevation range:" << out.minElevation << out.maxElevation;

  return true;
}
