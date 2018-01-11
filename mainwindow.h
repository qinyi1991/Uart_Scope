#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "Communicate/Com.h"
#include "Analysis/analysis.h"
#include "Qcustomplot/qcustomplot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    //按钮处理
    void on_RecClearPushButton_clicked();
    void on_SendPushButton_clicked();
    void on_StartPushButton_clicked();
    void on_SaveScanRangePushButton_clicked();
    void on_SaveSerialCfgPushButton_clicked();

    //复选框处理
    void on_HEXRecCheckBox_toggled(bool checked);
    void on_AsciiRecCheckBox_toggled(bool checked);
    void on_HEXSendCheckBox_toggled(bool checked);
    void on_AsciiSendCheckBox_toggled(bool checked);

    //接收数据
    void ReceiveData();
    void ShowWave();

private:
    Ui::MainWindow *ui;
    TCom *pCom;//串口
    TAnalysis *pAnalysis;
    bool m_StartButtonState;//开始按钮标志
    QPen Pen;//画笔
    QString PATH;
    QSettings *pSetting;
    int index;
};

#endif // MAINWINDOW_H
