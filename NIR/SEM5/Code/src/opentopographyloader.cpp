
#include "opentopographyloader.hpp"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSaveFile>
#include <QUrl>
#include <QUrlQuery>
#include <qdebug.h>

OpenTopographyLoader::OpenTopographyLoader(QObject *parent)
    : QObject(parent), manager(new QNetworkAccessManager(this)) {}

bool OpenTopographyLoader::isGeoTiff(const QByteArray &data) {
  if (data.size() < 4)
    return false;

  const auto *p = reinterpret_cast<const unsigned char *>(data.constData());

  const bool littleEndian = p[0] == 'I' && p[1] == 'I';

  const bool bigEndian = p[0] == 'M' && p[1] == 'M';

  if (!littleEndian && !bigEndian)
    return false;

  if (littleEndian) {
    return (p[2] == 42 && p[3] == 0) || (p[2] == 43 && p[3] == 0);
  }

  return (p[2] == 0 && p[3] == 42) || (p[2] == 0 && p[3] == 43);
}

void OpenTopographyLoader::downloadDEM(double south, double north, double west,
                                       double east, const QString &outputFile) {
  qDebug() << "[DEM] downloadDEM() вызван";
  qDebug() << "[DEM] Bounds:" << south << north << west << east;
  qDebug() << "[DEM] Output file:" << outputFile;

  // Проверка координат
  if (south < -90.0 || north > 90.0 || west < -180.0 || east > 180.0 ||
      south >= north || west >= east) {
    emit downloadError("Некорректные координаты DEM.");
    return;
  }

  // Получение API-ключа
  const QString apiKey = qEnvironmentVariable("OPEN_TOPOGRAPHY_API_KEY");

  // Никогда не выводим сам ключ в лог!
  qDebug() << "[DEM] API key:" << (apiKey.isEmpty() ? "НЕ НАЙДЕН" : "НАЙДЕН");

  if (apiKey.isEmpty()) {
    emit downloadError("Не задан OPEN_TOPOGRAPHY_API_KEY.");
    return;
  }

  // Создание каталога
  const QFileInfo fileInfo(outputFile);

  if (!QDir().mkpath(fileInfo.absolutePath())) {
    emit downloadError("Не удалось создать каталог для DEM.");
    return;
  }

  // Формирование запроса
  QUrl url("https://portal.opentopography.org/API/globaldem");

  QUrlQuery query;

  query.addQueryItem("demtype", "COP30");
  query.addQueryItem("south", QString::number(south, 'f', 8));
  query.addQueryItem("north", QString::number(north, 'f', 8));
  query.addQueryItem("west", QString::number(west, 'f', 8));
  query.addQueryItem("east", QString::number(east, 'f', 8));
  query.addQueryItem("outputFormat", "GTiff");
  query.addQueryItem("API_Key", apiKey);

  url.setQuery(query);

  QNetworkRequest request(url);

  request.setHeader(QNetworkRequest::UserAgentHeader,
                    "BMSTU-NIR-DEM-Loader/1.0");

  request.setRawHeader("Accept", "image/tiff, application/octet-stream");

  request.setTransferTimeout(180000);

  // Отправка запроса
  QNetworkReply *reply = manager->get(request);

  qDebug() << "[DEM] HTTP-запрос отправлен";
  qDebug() << "[DEM] Host:" << request.url().host();
  qDebug() << "[DEM] Path:" << request.url().path();

  // Прогресс загрузки
  connect(reply, &QNetworkReply::downloadProgress, this,
          [this](qint64 received, qint64 total) {
            qDebug() << "[DEM] Download:" << received << "/" << total;
            emit downloadProgress(received, total);
          });

  // Ошибки сети
  connect(reply, &QNetworkReply::errorOccurred, this,
          [reply](QNetworkReply::NetworkError error) {
            Q_UNUSED(error);

            qDebug() << "[DEM] Network error:" << reply->errorString();
          });

  // Завершение загрузки
  connect(reply, &QNetworkReply::finished, this, [this, reply, outputFile]() {
    // Сначала получаем всю информацию из reply
    const int status =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    const QNetworkReply::NetworkError error = reply->error();

    const QString errorString = reply->errorString();

    const QByteArray data = reply->readAll();

    qDebug() << "[DEM] Ответ получен";
    qDebug() << "[DEM] HTTP status:" << status;
    qDebug() << "[DEM] Network error code:" << error;
    qDebug() << "[DEM] Network error:" << errorString;
    qDebug() << "[DEM] Response size:" << data.size();

    // После чтения всех данных удаляем reply
    reply->deleteLater();

    // Проверка HTTP и сетевой ошибки
    if (error != QNetworkReply::NoError || status != 200) {
      emit downloadError(QString("Ошибка загрузки DEM. HTTP %1: %2")
                             .arg(status)
                             .arg(errorString));
      return;
    }

    // Проверка формата GeoTIFF
    if (!isGeoTiff(data)) {
      qDebug() << "[DEM] Первые байты ответа:" << data.left(100);

      emit downloadError("Сервер не вернул корректный GeoTIFF. "
                         "Возможно, получено текстовое сообщение об ошибке.");
      return;
    }

    // Сохранение GeoTIFF
    QSaveFile file(outputFile);

    if (!file.open(QIODevice::WriteOnly)) {
      emit downloadError("Не удалось открыть файл: " + file.errorString());
      return;
    }

    if (file.write(data) != data.size()) {
      file.cancelWriting();

      emit downloadError("Ошибка записи GeoTIFF.");
      return;
    }

    if (!file.commit()) {
      emit downloadError("Не удалось завершить сохранение GeoTIFF.");
      return;
    }

    qDebug() << "[DEM] GeoTIFF сохранён:" << outputFile;
    qDebug() << "[DEM] Размер файла:" << data.size();

    emit downloadFinished(outputFile);
  });
}
