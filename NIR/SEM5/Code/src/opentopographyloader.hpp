
#pragma once

#include <QNetworkAccessManager>
#include <QObject>
#include <QString>

class OpenTopographyLoader : public QObject {
  Q_OBJECT

public:
  explicit OpenTopographyLoader(QObject *parent = nullptr);

  void downloadDEM(double south, double north, double west, double east,
                   const QString &outputFile);

signals:
  void downloadFinished(const QString &fileName);
  void downloadError(const QString &message);
  void downloadProgress(qint64 received, qint64 total);

private:
  QNetworkAccessManager *manager;

  static bool isGeoTiff(const QByteArray &data);
};
