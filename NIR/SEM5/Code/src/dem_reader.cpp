#include "dem_reader.hpp"
#include <vector>
#include <iostream>

void DEMReader::loadDem(const QString fileName) {
  GDALAllRegister();

  GDALDataset *dataset =
      static_cast<GDALDataset *>(GDALOpen(fileName.toLocal8Bit(), GA_ReadOnly));

  double gt[6];

  if (dataset->GetGeoTransform(gt) == CE_None) {
    qDebug() << "Origin X:" << gt[0];
    qDebug() << "Origin Y:" << gt[3];
    qDebug() << "Pixel width:" << gt[1];
    qDebug() << "Pixel height:" << gt[5];
  }
  int width = dataset->GetRasterXSize();
  int height = dataset->GetRasterYSize();
  GDALRasterBand *band = dataset->GetRasterBand(1);

  std::vector<float> elevation(width * height);

  CPLErr result = band->RasterIO(GF_Read, 0, 0, width, height, elevation.data(),
                                 width, height, GDT_Float32, 0, 0);


    qDebug() << "DEM успешно прочитан";
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            std::cout << elevation[i*height + j] << " ";
        }
        std::cout << "\n";
    }
}

DEMReader::DEMReader(QObject *parent) : QObject(parent) {}