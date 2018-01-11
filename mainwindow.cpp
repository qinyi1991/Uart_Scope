#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("CECPort雷达调试助手");
    setWindowIcon(QIcon(":/new/images/Resouce/image.png"));

    PATH = "config.xml";
    pSetting = new QSettings(PATH, QSettings::IniFormat);
    index=0;
    pCom = new TCom;
    pAnalysis = new TAnalysis;

    //开始按钮样式
    m_StartButtonState = false;
    ui->StartPushButton->setText("START");
    ui->StartPushButton->setStyleSheet("font-size:30px");

    //信号按钮样式
    ui->SignalPushButton->setStyleSheet("background-color:rgb(255,50,50)");

    //预装端口号
    pCom->SerialEnum();
    ui->PortComboBox->addItems(pCom->m_ComListName);

    //预装波特率
    ui->BaudComboBox->addItem("9600");
    ui->BaudComboBox->addItem("19200");
    ui->BaudComboBox->addItem("56000");
    ui->BaudComboBox->addItem("115200");
    ui->BaudComboBox->addItem("128000");
    ui->BaudComboBox->addItem("256000");

    //预装数据位
    ui->DataBitComboBox->addItem("8");
    ui->DataBitComboBox->addItem("7");
    ui->DataBitComboBox->addItem("6");
    ui->DataBitComboBox->addItem("5");

    //预装停止位
    ui->StopBitComboBox->addItem("1");
    ui->StopBitComboBox->addItem("2");

    //预装校验位
    ui->ParityBitComboBox->addItem("0");
    ui->ParityBitComboBox->addItem("1");
    ui->ParityBitComboBox->addItem("2");

    //选择配置
    index = pSetting->value("PortIndex").toInt();
    ui->PortComboBox->setCurrentIndex(index);
    index = pSetting->value("BaudIndex").toInt();
    ui->BaudComboBox->setCurrentIndex(index);
    index = pSetting->value("DatabitIndex").toInt();
    ui->DataBitComboBox->setCurrentIndex(index);
    index = pSetting->value("StopbitIndex").toInt();
    ui->StopBitComboBox->setCurrentIndex(index);
    index = pSetting->value("ParitybitIndex").toInt();
    ui->ParityBitComboBox->setCurrentIndex(0);
    index = pSetting->value("XRange").toInt();
    ui->XRangeLineEdit->setText(QString::number(index,10));
    index = pSetting->value("YRange").toInt();
    ui->YRangeLineEdit->setText(QString::number(index,10));
    index = pSetting->value("DisplayDotsNum").toInt();
    ui->PointNumLineEdit->setText(QString::number(index,10));

    //复选框默认设置
    ui->HEXRecCheckBox->setChecked(true);
    ui->HEXSendCheckBox->setChecked(true);
    ui->AsciiRecCheckBox->setChecked(false);
    ui->AsciiSendCheckBox->setChecked(false);

    //波形图坐标设置
    //ui->ChannelWidget->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom|QCP::iSelectAxes|QCP::iSelectLegend|QCP::iSelectPlottables);
    //ui->ResultWidget->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom|QCP::iSelectAxes|QCP::iSelectLegend|QCP::iSelectPlottables);
    QBrush qBrushColor(QColor(50,50,50));
    ui->ChannelWidget->setBackground(qBrushColor);
    ui->ChannelWidget->legend->setVisible(false);
    //ui->ChannelWidget->legend->setBrush(QColor(50,50,50,0));//图例透明

    ui->ChannelWidget->xAxis->setRange(-127,127);
    ui->ChannelWidget->xAxis->setLabelColor(QColor(0,160,230));
    ui->ChannelWidget->xAxis->setTickLabelColor(Qt::white);
    ui->ChannelWidget->xAxis->setBasePen(QColor(32,178,170));
    ui->ChannelWidget->xAxis->setTickPen(QColor(255,165,0));
    ui->ChannelWidget->xAxis->setSubTicks(true);

    ui->ChannelWidget->yAxis->setRange(-65535,65535);
    ui->ChannelWidget->yAxis->setLabelColor(QColor(0,160,230));
    ui->ChannelWidget->yAxis->setTickLabelColor(Qt::white);
    ui->ChannelWidget->yAxis->setBasePen(QColor(32,178,170));
    ui->ChannelWidget->yAxis->setTickPen(QColor(255,165,0));
    ui->ChannelWidget->yAxis->setSubTicks(true);

    //ui->ChannelWidget->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignBottom | Qt::AlignRight);
    ui->ChannelWidget->xAxis->setRange(-(pAnalysis->m_ChannelsLeg/2-1),(pAnalysis->m_ChannelsLeg/2-1));
    ui->ChannelWidget->yAxis->setRange(-65535,65535);
    for(int i=0;i<pAnalysis->m_Channels;i++)//根据通道数添加图层
    {
        ui->ChannelWidget->addGraph();
    }

    ui->ResultWidget->setBackground(qBrushColor);
    ui->ResultWidget->legend->setVisible(false);

    ui->ResultWidget->xAxis->setRange(-127,127);
    ui->ResultWidget->xAxis->setLabelColor(QColor(0,160,230));
    ui->ResultWidget->xAxis->setTickLabelColor(Qt::white);
    ui->ResultWidget->xAxis->setBasePen(QColor(32,178,170));
    ui->ResultWidget->xAxis->setTickPen(QColor(255,165,0));

    ui->ResultWidget->yAxis->setRange(-10,10);
    ui->ResultWidget->yAxis->setLabelColor(QColor(0,160,230));
    ui->ResultWidget->yAxis->setTickLabelColor(Qt::white);
    ui->ResultWidget->yAxis->setBasePen(QColor(32,178,170));
    ui->ResultWidget->yAxis->setTickPen(QColor(255,165,0));

    ui->ResultWidget->addGraph();

    QVector<double> x(256),y(256);
    for(int i=0;i<256;i++)
    {
        x[i]=i-127;
        y[i]=0;//5*sin(x[i]);
    }
    Pen.setWidth(1);
    Pen.setColor(Qt::green);
    ui->ResultWidget->graph(0)->setPen(Pen);
    ui->ResultWidget->graph(0)->setData(x,y);
    ui->ResultWidget->replot();

}

MainWindow::~MainWindow()
{
    delete pCom;
    delete pAnalysis;
    delete ui;
    delete pSetting;
}

void MainWindow::on_RecClearPushButton_clicked()
{
    ui->RecDataTextBrowser->setText("");

    return;
}

void MainWindow::on_SaveScanRangePushButton_clicked()
{
    index = ui->XRangeLineEdit->displayText().toInt();
    pSetting->setValue("XRange",index);
    index = ui->YRangeLineEdit->displayText().toInt();
    pSetting->setValue("YRange",index);
    index = ui->PointNumLineEdit->displayText().toInt();
    pSetting->setValue("DisplayDotsNum",index);

    //pAnalysis->AnalysisRecvData();
    //ShowWave();

    return;
}

void MainWindow::on_SaveSerialCfgPushButton_clicked()
{
    index = ui->PortComboBox->currentIndex();
    pSetting->setValue("PortIndex",index);
    index = ui->BaudComboBox->currentIndex();
    pSetting->setValue("BaudIndex",index);
    index = ui->DataBitComboBox->currentIndex();
    pSetting->setValue("DatabitIndex",index);
    index = ui->StopBitComboBox->currentIndex();
    pSetting->setValue("StopbitIndex",index);
    index = ui->ParityBitComboBox->currentIndex();
    pSetting->setValue("ParitybitIndex",index);

    return;
}

void MainWindow::on_SendPushButton_clicked()
{
    QString SendData = ui->SendDatalineEdit->displayText();
    pCom->SerialSendData(&SendData);

    return;
}

void MainWindow::on_StartPushButton_clicked()
{
    if(!m_StartButtonState)
    {
        QString PortName = ui->PortComboBox->currentText();//获取当前串口号字符串
        QString BaudRate = ui->BaudComboBox->currentText();
        QString DataBit = ui->DataBitComboBox->currentText();
        QString StopBit = ui->StopBitComboBox->currentText();
        QString ParityBit = ui->ParityBitComboBox->currentText();
        pCom->SerialConfig(&PortName,&BaudRate,&DataBit,&StopBit,&ParityBit);
        if(!pCom->SerialOpen())
        {
            QMessageBox::warning(this,tr("Warning"),tr("There are no any available serials!"),QMessageBox::Abort);
            return;
        }
        connect(pCom->pSerialCom, SIGNAL(readyRead()), this, SLOT(ReceiveData()));//连接串口到显示区
        //connect(pCom->pSerialCom,SIGNAL(readyRead()),this,SLOT(ShowWave()));
        m_StartButtonState = true;
        ui->StartPushButton->setText("STOP");
        ui->SignalPushButton->setStyleSheet("background-color:rgb(50,255,50)");
    }
    else
    {
        m_StartButtonState = false;
        pCom->SerialClose();
        ui->StartPushButton->setText("START");
        ui->SignalPushButton->setStyleSheet("background-color:rgb(255,50,50)");
    }

    return;
}

void MainWindow::on_HEXRecCheckBox_toggled(bool checked)
{
    if(pCom->m_AsciiRecFlag)
    {
        pCom->m_AsciiRecFlag=false;
        ui->AsciiRecCheckBox->setChecked(false);
    }
    pCom->m_HexRecFlag = checked;

    return;
}

void MainWindow::on_AsciiRecCheckBox_toggled(bool checked)
{
    if(pCom->m_HexRecFlag)
    {
        pCom->m_HexRecFlag=false;
        ui->HEXRecCheckBox->setChecked(false);
    }
    pCom->m_AsciiRecFlag = checked;

    return;
}

void MainWindow::on_HEXSendCheckBox_toggled(bool checked)
{
    if(pCom->m_AsciiSendFlag)
    {
        pCom->m_AsciiSendFlag=false;
        ui->AsciiSendCheckBox->setChecked(false);
    }
    pCom->m_HexSendFlag = checked;

    return;
}

void MainWindow::on_AsciiSendCheckBox_toggled(bool checked)
{
    if(pCom->m_HexSendFlag)
    {
        pCom->m_HexSendFlag=false;
        ui->HEXSendCheckBox->setChecked(false);
    }
    pCom->m_AsciiSendFlag = checked;

    return;

}

void MainWindow::ReceiveData()
{
    QString RecDataAscii;
    pCom->SerialRecData(&RecDataAscii);
    pAnalysis->AnalysisRecvData(RecDataAscii);
    //qDebug()<<RecDataAscii;
    ui->RecDataTextBrowser->insertPlainText(RecDataAscii);
    ShowWave();

    return;
}

void MainWindow::ShowWave()
{
    unsigned short nSize = pAnalysis->m_DisplayDotNum ;
    QVector<double> x(nSize),y(nSize);

    for(int i=0;i<nSize;i++)
    {
        x[i] = pAnalysis->m_Channel_x[i];
        y[i] = pAnalysis->m_Channel1_y[i];
    }
    Pen.setWidth(1);
    Pen.setColor(Qt::green);
    ui->ChannelWidget->graph(0)->setPen(Pen);

    ui->ChannelWidget->graph(0)->setData(x,y);
    ui->ChannelWidget->replot();

    for(int j=0;j<nSize;j++)
    {
        x[j] = pAnalysis->m_Channel_x[j];
        y[j] = pAnalysis->m_Channel2_y[j];
    }
    Pen.setWidth(1);
    Pen.setColor(Qt::blue);
    ui->ChannelWidget->graph(1)->setPen(Pen);

    ui->ChannelWidget->graph(1)->setData(x,y);
    ui->ChannelWidget->replot();

    for(int k=0;k<nSize;k++)
    {
        x[k] = pAnalysis->m_Channel_x[k];
        y[k] = pAnalysis->m_Channel3_y[k];
    }
    Pen.setWidth(1);
    Pen.setColor(Qt::yellow);
    ui->ChannelWidget->graph(2)->setPen(Pen);

    ui->ChannelWidget->graph(2)->setData(x,y);
    ui->ChannelWidget->replot();

    for(int l=0;l<nSize;l++)
    {
        x[l] = pAnalysis->m_Channel_x[l];
        y[l] = pAnalysis->m_Channel4_y[l];
    }
    Pen.setWidth(1);
    Pen.setColor(Qt::red);
    ui->ChannelWidget->graph(3)->setPen(Pen);

    ui->ChannelWidget->graph(3)->setData(x,y);
    ui->ChannelWidget->replot();

    return;
}


