#include "projectserializer.hpp"

#include <QDataStream>
#include <QFile>
#include <QJsonDocument>
#include <QSaveFile>

#include <cmath>
#include <limits>

namespace {

constexpr char Magic[] = "OSM3D";
constexpr quint16 FormatVersion = 1;

constexpr quint64 MaxDemCells = 100'000'000;
constexpr quint32 MaxFeatures = 1'000'000;
constexpr quint32 MaxGeometryPoints = 10'000'000;
constexpr quint32 MaxInnerRings = 100'000;
constexpr quint32 MaxTagBytes = 64 * 1024 * 1024;
constexpr quint32 MaxProjectionBytes = 16 * 1024 * 1024;

void setError(QString *error, const QString &message) {
  if (error)
    *error = message;
}

void configureStream(QDataStream &stream) {
  stream.setVersion(QDataStream::Qt_6_0);
  stream.setByteOrder(QDataStream::LittleEndian);
}

bool writeRaw(QDataStream &stream, const QByteArray &data) {
  if (data.isEmpty())
    return true;

  return stream.writeRawData(data.constData(), data.size()) == data.size();
}

bool readRaw(QDataStream &stream, quint32 size, QByteArray &data,
             quint32 maxSize) {
  if (size > maxSize)
    return false;

  data.resize(static_cast<qsizetype>(size));

  if (size == 0)
    return true;

  return stream.readRawData(data.data(), static_cast<int>(size)) ==
         static_cast<int>(size);
}

bool writePoints(QDataStream &out, const QVector<GeoPoint> &points) {
  if (points.size() < 0 ||
      static_cast<quint64>(points.size()) > MaxGeometryPoints) {
    return false;
  }

  out << static_cast<quint32>(points.size());

  for (const GeoPoint &point : points) {
    if (!std::isfinite(point.lat) || !std::isfinite(point.lon))
      return false;

    out << point.lat << point.lon;
  }

  return out.status() == QDataStream::Ok;
}

bool readPoints(QDataStream &in, QVector<GeoPoint> &points) {
  quint32 count = 0;
  in >> count;

  if (in.status() != QDataStream::Ok || count > MaxGeometryPoints) {
    return false;
  }

  points.clear();
  points.reserve(static_cast<qsizetype>(count));

  for (quint32 i = 0; i < count; ++i) {
    GeoPoint point;

    in >> point.lat >> point.lon;

    if (in.status() != QDataStream::Ok || !std::isfinite(point.lat) ||
        !std::isfinite(point.lon)) {
      return false;
    }

    points.push_back(point);
  }

  return true;
}

bool writeFeature(QDataStream &out, const OSMFeature &feature) {
  const quint8 type = static_cast<quint8>(feature.getType());
  const quint8 geometryType = static_cast<quint8>(feature.getGeometryType());

  if (type > static_cast<quint8>(OSMFeatureType::Amenity) ||
      geometryType > static_cast<quint8>(OSMGeometryType::Polygon)) {
    return false;
  }

  if (!std::isfinite(feature.getHeightMeters()) ||
      !std::isfinite(feature.getMinHeightMeters()) ||
      !std::isfinite(feature.getWidthMeters())) {
    return false;
  }

  out << feature.getId();
  out << type;
  out << geometryType;

  if (!writePoints(out, feature.getGeometry()))
    return false;

  if (static_cast<quint64>(feature.getInnerRings().size()) > MaxInnerRings) {
    return false;
  }

  out << static_cast<quint32>(feature.getInnerRings().size());

  for (const auto &ring : feature.getInnerRings()) {
    if (!writePoints(out, ring))
      return false;
  }

  const QByteArray tags =
      QJsonDocument(feature.getTags()).toJson(QJsonDocument::Compact);

  if (static_cast<quint64>(tags.size()) > MaxTagBytes)
    return false;

  out << static_cast<quint32>(tags.size());

  if (!writeRaw(out, tags))
    return false;

  out << feature.getHeightMeters();
  out << feature.getMinHeightMeters();
  out << feature.getWidthMeters();

  out << feature.getColors().wallR;
  out << feature.getColors().wallG;
  out << feature.getColors().wallB;

  out << feature.getColors().roofR;
  out << feature.getColors().roofG;
  out << feature.getColors().roofB;

  return out.status() == QDataStream::Ok;
}

bool readFeature(QDataStream &in, OSMFeature &feature) {
  quint8 type = 0;
  quint8 geometryType = 0;
  qint64 id;

  in >> id;
  feature.setId(id);
  in >> type;
  in >> geometryType;

  if (in.status() != QDataStream::Ok ||
      type > static_cast<quint8>(OSMFeatureType::Amenity) ||
      geometryType > static_cast<quint8>(OSMGeometryType::Polygon)) {
    return false;
  }

  feature.setType(static_cast<OSMFeatureType>(type));
  feature.setGeometryType(static_cast<OSMGeometryType>(geometryType));

  if (!readPoints(in, feature.getGeometry()))
    return false;

  quint32 ringCount = 0;
  in >> ringCount;

  if (in.status() != QDataStream::Ok || ringCount > MaxInnerRings) {
    return false;
  }

  feature.clearInnerRings();
  feature.reserveInnerRings(static_cast<qsizetype>(ringCount));

  for (quint32 i = 0; i < ringCount; ++i) {
    QVector<GeoPoint> ring;

    if (!readPoints(in, ring))
      return false;

    feature.pushBackInnerRings(std::move(ring));
  }

  quint32 tagsSize = 0;
  in >> tagsSize;

  QByteArray tagsData;

  if (in.status() != QDataStream::Ok ||
      !readRaw(in, tagsSize, tagsData, MaxTagBytes)) {
    return false;
  }

  QJsonParseError jsonError;
  const QJsonDocument tagsDocument =
      QJsonDocument::fromJson(tagsData, &jsonError);

  if (jsonError.error != QJsonParseError::NoError || !tagsDocument.isObject()) {
    return false;
  }

  feature.setTags(tagsDocument.object());

  in >> feature;

  if (in.status() != QDataStream::Ok)
    return false;

  return std::isfinite(feature.getHeightMeters()) &&
         std::isfinite(feature.getMinHeightMeters()) &&
         std::isfinite(feature.getWidthMeters()) &&
         std::isfinite(feature.getColors().wallR) &&
         std::isfinite(feature.getColors().wallG) &&
         std::isfinite(feature.getColors().wallB) &&
         std::isfinite(feature.getColors().roofR) &&
         std::isfinite(feature.getColors().roofG) &&
         std::isfinite(feature.getColors().roofB);
}

} // namespace

bool ProjectSerializer::save(const QString &fileName, const DEMData &dem,
                             const QVector<OSMFeature> &features,
                             float verticalScale, QString *error) {

  if (dem.width < 2 || dem.height < 2) {
    setError(error, "DEM не загружена или имеет некорректный размер.");
    return false;
  }

  const quint64 expectedCells =
      static_cast<quint64>(dem.width) * static_cast<quint64>(dem.height);

  if (expectedCells > MaxDemCells ||
      dem.elevation.size() != static_cast<qsizetype>(expectedCells)) {
    setError(error, "Некорректный массив высот DEM.");
    return false;
  }

  if (!std::isfinite(verticalScale) || verticalScale <= 0.0f) {
    setError(error, "Некорректный масштаб высот.");
    return false;
  }

  if (static_cast<quint64>(features.size()) > MaxFeatures) {
    setError(error, "Слишком много OSM-объектов.");
    return false;
  }

  QSaveFile file(fileName);

  if (!file.open(QIODevice::WriteOnly)) {
    setError(error, "Не удалось создать файл проекта: " + file.errorString());
    return false;
  }

  QDataStream out(&file);
  configureStream(out);

  if (out.writeRawData(Magic, sizeof(Magic) - 1) != sizeof(Magic) - 1) {
    setError(error, "Ошибка записи заголовка проекта.");
    return false;
  }

  out << FormatVersion;

  out << static_cast<qint32>(dem.width);
  out << static_cast<qint32>(dem.height);
  out << expectedCells;

  for (float elevation : dem.elevation)
    out << elevation;

  for (double value : dem.geoTransform)
    out << value;

  const QByteArray projection = dem.projectionWkt.toUtf8();

  if (static_cast<quint64>(projection.size()) > MaxProjectionBytes) {
    setError(error, "Слишком длинное описание проекции DEM.");
    return false;
  }

  out << static_cast<quint32>(projection.size());

  if (!writeRaw(out, projection)) {
    setError(error, "Ошибка записи проекции DEM.");
    return false;
  }

  out << dem.minElevation;
  out << dem.maxElevation;

  out << static_cast<quint32>(features.size());

  for (const OSMFeature &feature : features) {
    if (!writeFeature(out, feature)) {
      setError(error, "Ошибка записи одного из OSM-объектов.");
      return false;
    }
  }

  out << verticalScale;

  if (out.status() != QDataStream::Ok) {
    setError(error, "Ошибка записи данных проекта.");
    return false;
  }

  if (!file.commit()) {
    setError(error, "Не удалось завершить запись файла: " + file.errorString());
    return false;
  }

  return true;
}

bool ProjectSerializer::load(const QString &fileName, DEMData &dem,
                             QVector<OSMFeature> &features,
                             float &verticalScale, QString *error) {

  QFile file(fileName);

  if (!file.open(QIODevice::ReadOnly)) {
    setError(error, "Не удалось открыть файл проекта: " + file.errorString());
    return false;
  }

  QDataStream in(&file);
  configureStream(in);

  char magic[sizeof(Magic) - 1] = {};

  if (in.readRawData(magic, sizeof(magic)) != sizeof(magic) ||
      QByteArray(magic, sizeof(magic)) !=
          QByteArray(Magic, sizeof(Magic) - 1)) {
    setError(error, "Файл не является проектом OSM3D.");
    return false;
  }

  quint16 version = 0;
  in >> version;

  if (in.status() != QDataStream::Ok || version != FormatVersion) {
    setError(error, "Неподдерживаемая версия проекта.");
    return false;
  }

  DEMData loadedDem;

  qint32 width = 0;
  qint32 height = 0;
  quint64 cellCount = 0;

  in >> width >> height >> cellCount;

  if (in.status() != QDataStream::Ok || width < 2 || height < 2 ||
      cellCount != static_cast<quint64>(width) * static_cast<quint64>(height) ||
      cellCount > MaxDemCells) {
    setError(error, "Некорректные размеры DEM в файле проекта.");
    return false;
  }

  loadedDem.width = width;
  loadedDem.height = height;
  loadedDem.elevation.resize(static_cast<qsizetype>(cellCount));

  for (float &elevation : loadedDem.elevation) {
    in >> elevation;

    if (in.status() != QDataStream::Ok || !std::isfinite(elevation)) {
      setError(error, "Повреждён массив высот DEM.");
      return false;
    }
  }

  for (double &value : loadedDem.geoTransform) {
    in >> value;

    if (in.status() != QDataStream::Ok || !std::isfinite(value)) {
      setError(error, "Повреждена геотрансформация DEM.");
      return false;
    }
  }

  quint32 projectionSize = 0;
  in >> projectionSize;

  QByteArray projection;

  if (in.status() != QDataStream::Ok ||
      !readRaw(in, projectionSize, projection, MaxProjectionBytes)) {
    setError(error, "Повреждено описание проекции DEM.");
    return false;
  }

  loadedDem.projectionWkt = QString::fromUtf8(projection);

  in >> loadedDem.minElevation;
  in >> loadedDem.maxElevation;

  if (in.status() != QDataStream::Ok ||
      !std::isfinite(loadedDem.minElevation) ||
      !std::isfinite(loadedDem.maxElevation)) {
    setError(error, "Некорректные минимальная или максимальная высоты DEM.");
    return false;
  }

  quint32 featureCount = 0;
  in >> featureCount;

  if (in.status() != QDataStream::Ok || featureCount > MaxFeatures) {
    setError(error, "Некорректное количество OSM-объектов.");
    return false;
  }

  QVector<OSMFeature> loadedFeatures;
  loadedFeatures.reserve(static_cast<qsizetype>(featureCount));

  for (quint32 i = 0; i < featureCount; ++i) {
    OSMFeature feature;

    if (!readFeature(in, feature)) {
      setError(error, "Повреждены данные OSM-объекта.");
      return false;
    }

    loadedFeatures.push_back(std::move(feature));
  }

  float loadedVerticalScale = 0.0f;
  in >> loadedVerticalScale;

  if (in.status() != QDataStream::Ok || !std::isfinite(loadedVerticalScale) ||
      loadedVerticalScale <= 0.0f) {
    setError(error, "Некорректный масштаб высот в проекте.");
    return false;
  }

  if (!file.atEnd()) {
    setError(error, "В файле обнаружены лишние или повреждённые данные.");
    return false;
  }

  // Обновляем переданные данные только после успешной загрузки всего файла.
  dem = std::move(loadedDem);
  features = std::move(loadedFeatures);
  verticalScale = loadedVerticalScale;

  return true;
}