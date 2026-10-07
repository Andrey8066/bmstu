#ifndef DEM_READER_HPP
#define DEM_READER_HPP

#include <gdal_priv.h>
#include <QObject>
#include <QDebug>


class DEMReader : public QObject {
  Q_OBJECT

public:
  explicit DEMReader(QObject *parent = nullptr);
  void loadDem(QString filePath);
  void readDem();
};

#endif