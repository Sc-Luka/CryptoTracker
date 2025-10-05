#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "cryptodatamanager.h"

#include <QMainWindow>
#include <QtCharts>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onDataUpdated(const QVector<cryptoData> &cryptoData);
    void onErrorOccurred(const QString &errorMessage);
    void onRequestMade(const QString &timeStamp);
    void onCryptoDataReady(const QVector<QPair<QDateTime, double>> &historicalData);

    void on_Refresh_clicked();
    void on_Search_clicked();
    void on_Clear_clicked();
    void on_CryptoTable_cellClicked(int row, int);

private:
    Ui::MainWindow *ui;
    CryptoDataManager *cryptoManager;
    QString formatNumber(double num);
    QChart* createLineChart(const QVector<QPointF> &dataPoints, const QString &title);

};
#endif // MAINWINDOW_H
