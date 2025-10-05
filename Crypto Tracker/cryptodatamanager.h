#ifndef CRYPTODATAMANAGER_H
#define CRYPTODATAMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>

struct cryptoData{
    QString coinName;
    QString symbol;
    double price;
    double marketCup;
    double volume_24h;
    double change_24h;
};

class CryptoDataManager : public QObject
{
    Q_OBJECT
public:
    explicit CryptoDataManager(QObject *parent = nullptr);
    ~CryptoDataManager();

    void fetchTopCrypto(int limit = 50);
    void fetchHistoricalData(const QString &coinId, int days = 21);
    QVector<cryptoData> getCryptoData() const;

signals:
    void dataUpdated(const QVector<cryptoData> &cryptoData);
    void CryptoTable_cellClicked(const QVector<QPair<QDateTime, double>> &data);
    void errorOccurred(const QString &errorMessage);
    void historicalDataReady(const QVector<QPair<QDateTime, double>> &data);
    void requestMade(const QString &timeStamp);

private slots:
    void onNetworkReply(QNetworkReply *reply);
    void refresData();

private:
    QNetworkAccessManager *networkManager;
    QTimer *refreshTimer;
    QVector<cryptoData> cryptoDataList;

    void parseTopCryptoResponse(const QByteArray &response);
    void parseHistoricalResponse(const QByteArray &response);
};

#endif // CRYPTODATAMANAGER_H
