#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "CoinMap.h"

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QTableWidgetItem>
#include <QMessageBox>

QT_CHARTS_USE_NAMESPACE


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Crypto Tracker");
    setWindowIcon(QIcon(":/icons/myIcon.png"));

    QLabel *gitInfo = new QLabel("GitHub: https://github.com/Sc-Luka");
    statusBar()->addPermanentWidget(gitInfo);

    this->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "stop:0 #0d1117, stop:1 #1a1f2b); color: white;"
        );   

    ui->CryptoTrackerName->setStyleSheet("color: #80DFFF;");
    ui->UpdateInfo->setStyleSheet("color: #80DFFF;");
    ui->chartWidget->chart()->setBackgroundBrush(QBrush(QColor("#0d1117")));
    ui->chartWidget->chart()->setBackgroundVisible(true);


    ui->CryptoTable->setColumnWidth(0, 400); // Name (with Symbol)
    ui->CryptoTable->setColumnWidth(1, 180); // Price
    ui->CryptoTable->setColumnWidth(2, 180); // Change
    ui->CryptoTable->setColumnWidth(3, 180); // Volume
    ui->CryptoTable->setColumnWidth(4, 180); // Market Cap
    ui->CryptoTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->CryptoTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->CryptoTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->chartWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    cryptoManager=new CryptoDataManager(this);

    connect(cryptoManager, &CryptoDataManager::dataUpdated,this,&MainWindow::onDataUpdated);
    connect(cryptoManager, &CryptoDataManager::errorOccurred,this,&MainWindow::onErrorOccurred);
    connect(cryptoManager, &CryptoDataManager::requestMade, this, &MainWindow::onRequestMade);
    connect(cryptoManager, &CryptoDataManager::historicalDataReady,this,&MainWindow::onCryptoDataReady);

    cryptoManager->fetchTopCrypto();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onDataUpdated(const QVector<cryptoData> &crypto)
{
    ui->CryptoTable->clearContents();
    ui->CryptoTable->setRowCount(crypto.size());

    for(int row=0; row<crypto.size(); ++row){

        const cryptoData &data=crypto[row];

        QTableWidgetItem *coinNameItem=new QTableWidgetItem(data.coinName + " (" + data.symbol.toUpper() + ")" );
        ui->CryptoTable->setItem(row, 0, coinNameItem);

        QTableWidgetItem *priceItem=new QTableWidgetItem(QString("$%1").arg(data.price, 0, 'f', 2));
        priceItem->setTextAlignment(Qt::AlignCenter);
        ui->CryptoTable->setItem(row, 1, priceItem);

        QTableWidgetItem *changeItem=new QTableWidgetItem(QString("%1%").arg(data.change_24h, 0, 'f', 2));
        changeItem->setTextAlignment(Qt::AlignCenter);
        if (data.change_24h < 0){
            changeItem->setForeground(QBrush(QColor(255, 0, 0))); // Red
        } else {
                changeItem->setForeground(QBrush(QColor(0, 128, 0))); // Green
            }
        ui->CryptoTable->setItem(row, 2, changeItem);

        QTableWidgetItem *volumeItem=new QTableWidgetItem(formatNumber(data.volume_24h));
        volumeItem->setTextAlignment(Qt::AlignCenter);
        ui->CryptoTable->setItem(row, 3, volumeItem);

        QTableWidgetItem *marketCupItem=new QTableWidgetItem(formatNumber(data.marketCup));
        marketCupItem->setTextAlignment(Qt::AlignCenter);
        ui->CryptoTable->setItem(row, 4, marketCupItem);
    }
}

void MainWindow::onErrorOccurred(const QString &errorMessage){
    QMessageBox::warning(this, "Error", errorMessage);
}

void MainWindow::onRequestMade(const  QString &timeStamp){
    ui->UpdateInfo->setText(timeStamp);
}

void MainWindow::on_Refresh_clicked()
{
    cryptoManager->fetchTopCrypto();
}

void MainWindow::on_Search_clicked()
{
    QString searchData = ui->SearchLine->text().trimmed().toLower();

    if (!searchData.isEmpty()) {
        QVector<cryptoData> filteredData;

        for (const cryptoData &data : cryptoManager->getCryptoData()) {
            if (data.coinName.toLower().contains(searchData) ||
                data.symbol.toLower().contains(searchData)) {
                filteredData.append(data);
            }
        }
        onDataUpdated(filteredData);
    }
}

void MainWindow::on_Clear_clicked()
{
    ui->SearchLine->clear();
    onDataUpdated(cryptoManager->getCryptoData());
    QChart *emptyChart = new QChart();
    emptyChart->setTitle("Select a crypto to view historical data");
    emptyChart->setTitleBrush(QBrush(QColor("#80DFFF")));
    emptyChart->setBackgroundBrush(QBrush(QColor("#0d1117")));
    ui->chartWidget->setChart(emptyChart);

}

QString MainWindow::formatNumber(double num) {
    if (num >= 1e9) {
        return QString("$%1B").arg(num / 1e9, 0, 'f', 2);
    } else if (num >= 1e6) {
        return QString("$%1M").arg(num / 1e6, 0, 'f', 2);
    } else if (num >= 1e3) {
        return QString("$%1K").arg(num / 1e3, 0, 'f', 2);
    } else {
        return QString("$%1").arg(num, 0, 'f', 2);
    }
}

QChart* MainWindow::createLineChart(const QVector<QPointF> &dataPoints, const QString &title){
    QLineSeries *series=new QLineSeries();
    for(const auto point : dataPoints){
        series->append(point);
    }

    series->setColor(QColor("#80DFFF"));

    QChart *chart=new QChart();
    chart->addSeries(series);
    chart->setTitle(title);

    QValueAxis *axisX=new QValueAxis();
    axisX->setTitleText("Time");
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY=new QValueAxis();
    axisY->setTitleText("Price (USD)");
    chart->addAxis(axisY, Qt::AlignLeft);

    series->attachAxis(axisX);
    series->attachAxis(axisY);
    axisX->setGridLineVisible(false);
    axisY->setGridLineVisible(false);

    chart->legend()->hide();
    chart->setTitleBrush(QBrush(QColor("#80DFFF")));
    chart->setBackgroundBrush(QBrush(QColor("#0d1117")));
    axisX->setLabelsColor(QColor("#FFFFFF"));
    axisY->setLabelsColor(QColor("#FFFFFF"));
    chart->setBackgroundVisible(true);

    return chart;

}

void MainWindow::on_CryptoTable_cellClicked(int row, int /*column*/)
{
    QString cryptoName = ui->CryptoTable->item(row, 0)->text();
    cryptoName = cryptoName.split('(').first().trimmed();



    QString cryptoId = CoinMap::getNameToIdMap().value(cryptoName, "");
    if (cryptoId.isEmpty()) {
        QMessageBox::warning(this,tr("Warning"),
            tr("Unknown crypto name: %1").arg(cryptoName));

        return;
    }
    cryptoManager->fetchHistoricalData(cryptoId, 21);
}

void MainWindow::onCryptoDataReady(const QVector<QPair<QDateTime, double>> &historicalData){
    if(historicalData.isEmpty())
        return;

    QVector<QPointF> points;
    for (const auto &pair : historicalData)
        points.append(QPointF(pair.first.toMSecsSinceEpoch(), pair.second));

    int row = ui->CryptoTable->currentRow();
    QString cryptoName = ui->CryptoTable->item(row, 0)->text();
    QChart *chart = createLineChart(points, cryptoName + " Price Chart");
    ui->chartWidget->setChart(chart);
}


