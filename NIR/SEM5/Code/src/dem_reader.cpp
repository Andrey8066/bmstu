
#include "dem_reader.hpp"

#include <cpl_conv.h>
#include <gdal_priv.h>

#include <algorithm>
#include <limits>

#include <QDebug>

bool DEMReader::loadDem(const QString &fileName, DEMData &out) {
  GDALAllRegister();

  GDALDataset *dataset = static_cast<GDALDataset *>(
      GDALOpen(fileName.toLocal8Bit().constData(), GA_ReadOnly));

  if (!dataset)
    return false;

  const int width = dataset->GetRasterXSize();
  const int height = dataset->GetRasterYSize();

  if (width < 2 || height < 2 || dataset->GetRasterCount() < 1) {
    GDALClose(dataset);
    return false;
  }

  GDALRasterBand *band = dataset->GetRasterBand(1);

  out.width = width;
  out.height = height;

  qDebug() << "Размер DEM:" << width << "x" << height;

  qDebug() << "Projection:" << dataset->GetProjectionRef();

  qDebug() << "Raster type:" << GDALGetDataTypeName(band->GetRasterDataType());

  int hasNoData = 0;
  double noData = band->GetNoDataValue(&hasNoData);

  qDebug() << "NoData exists:" << hasNoData << "value:" << noData;

  qDebug() << "Scale:" << band->GetScale() << "Offset:" << band->GetOffset();

  if (dataset->GetGeoTransform(out.geoTransform) != CE_None) {
    GDALClose(dataset);

    const char *projection = dataset->GetProjectionRef();

    out.projectionWkt = projection ? QString::fromUtf8(projection) : QString();

    return false;
  }

  out.elevation.resize(static_cast<std::size_t>(width) * height);

  const CPLErr result =
      band->RasterIO(GF_Read, 0, 0, width, height, out.elevation.data(), width,
                     height, GDT_Float32, 0, 0);

  if (result != CE_None) {
    GDALClose(dataset);
    out = DEMData{};
    return false;
  }

  float minZ = std::numeric_limits<float>::max();
  float maxZ = std::numeric_limits<float>::lowest();

  for (float &z : out.elevation) {
    if (!std::isfinite(z) || (hasNoData && z == static_cast<float>(noData))) {
      // Для первого прототипа заполняем пропуски нулём.
      // Для точной модели NoData лучше интерполировать.
      z = 0.0f;
    }

    minZ = std::min(minZ, z);
    maxZ = std::max(maxZ, z);
  }

  out.minElevation = minZ;
  out.maxElevation = maxZ;

  GDALClose(dataset);
  return true;
}
