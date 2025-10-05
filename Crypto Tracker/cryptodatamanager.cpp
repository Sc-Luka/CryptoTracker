#include "cryptodatamanager.h"

#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>

CryptoDataManager::CryptoDataManager(QObject *parent) : QObject(parent) {
    networkManager=new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &CryptoDataManager::onNetworkReply);

    refreshTimer=new QTimer(this);
    connect(refreshTimer,&QTimer::timeout,this, &CryptoDataManager::refresData);
    refreshTimer->start(60000);
}

CryptoDataManager::~CryptoDataManager(){
    refreshTimer->stop();
    delete refreshTimer;
    delete networkManager;
}

void CryptoDataManager::refresData()
{
    fetchTopCrypto();
}

void CryptoDataManager::onNetworkReply(QNetworkReply *reply) {
    QByteArray response = reply->readAll();
    QString requestType = reply->property("requestType").toString();

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred("Network error: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    if (requestType == "historical") {
        parseHistoricalResponse(response);
    } else {
        parseTopCryptoResponse(response);
    }

    reply->deleteLater();
}

void CryptoDataManager::fetchHistoricalData(const QString &coinId, int days) {
    QUrl url(QString("https://api.coingecko.com/api/v3/coins/%1/market_chart").arg(coinId));
    QUrlQuery query;
    query.addQueryItem("vs_currency", "usd");
    query.addQueryItem("days", QString::number(days));
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = networkManager->get(request);
    reply->setProperty("requestType", "historical");
}

void CryptoDataManager::fetchTopCrypto(int limit)
{
    // CoinGecko API endpoint for top cryptocurrencies by market cap
    QUrl url("https://api.coingecko.com/api/v3/coins/markets");
    QUrlQuery query;
    query.addQueryItem("vs_currency", "usd");
    query.addQueryItem("order", "market_cap_desc");
    query.addQueryItem("per_page", QString::number(limit));
    query.addQueryItem("page", "1");
    query.addQueryItem("sparkline", "false");
    query.addQueryItem("price_change_percentage", "24h");
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    networkManager->get(request);

    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString formattedDateTime = currentDateTime.toString("yyyy-MM-dd HH:mm:ss");

    emit  requestMade("Last update" + formattedDateTime );
}

void CryptoDataManager::parseTopCryptoResponse(const QByteArray &response)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);

    if (!jsonDoc.isArray()) {
        qDebug() << "Raw response:" << QString(response);
        emit errorOccurred("Invalid top data format");
        return;
    }

    QJsonArray jsonArray = jsonDoc.array();
    cryptoDataList.clear();

    for (const QJsonValue &value : jsonArray) {
        QJsonObject obj = value.toObject();
        cryptoData data;
        data.coinName = obj["name"].toString();
        data.symbol = obj["symbol"].toString();
        data.price = obj["current_price"].toDouble();
        data.change_24h = obj["price_change_percentage_24h"].toDouble();
        data.volume_24h = obj["total_volume"].toDouble();
        data.marketCup = obj["market_cap"].toDouble();
        cryptoDataList.append(data);
    }

    emit dataUpdated(cryptoDataList);
}

void CryptoDataManager::parseHistoricalResponse(const QByteArray &response)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);

    if (!jsonDoc.isObject()) {
        qDebug() << "Raw response:" << QString(response);
        emit errorOccurred("Invalid historical data format");
        return;
    }

    QJsonObject jsonObject = jsonDoc.object();
    QJsonArray pricesArray = jsonObject["prices"].toArray();

    QVector<QPair<QDateTime, double>> historicalData;
    for (const QJsonValue &value : pricesArray) {
        QJsonArray point = value.toArray();
        if (point.size() >= 2) {
            qint64 timestamp = static_cast<qint64>(point[0].toDouble());
            double price = point[1].toDouble();
            QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(timestamp);
            historicalData.append(qMakePair(dateTime, price));
        }
    }

    emit historicalDataReady(historicalData);
}

QVector<cryptoData> CryptoDataManager::getCryptoData() const
{
    return cryptoDataList;
}
