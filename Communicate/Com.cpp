
#include "Com.h"
#include "qt_windows.h"
#include <qsettings.h>

//构造
TCom::TCom()
{
    pSerialCom = NULL;

    m_HexRecFlag = true;
    m_AsciiRecFlag = false;
    m_HexSendFlag = true;
    m_AsciiSendFlag = false;
    m_Handle = false;

    return;
}

//析构
TCom::~TCom()
{
    delete pSerialCom;
    return;
}

bool TCom::SerialOpen()
{
    m_Handle = pSerialCom->open(QIODevice::ReadWrite);//以可读写方式打开
    if(m_Handle == false)
    {
        return false;
    }

    return true;
}

void TCom::SerialClose()
{
    if(m_Handle)
    {
        pSerialCom->close();
    }

    return;
}

void TCom::SerialConfig(QString *PortName,QString *BaudRate, QString *Databit, QString *StopBit, QString *ParityBit)
{
    pSerialCom = new QextSerialPort(*PortName);
    qDebug("Set Baud Rate:%d\n",(*BaudRate).toInt());
    pSerialCom->setBaudRate((BaudRateType)(*BaudRate).toInt());//设置串口波特率
    pSerialCom->setDataBits((DataBitsType)(*Databit).toInt());//设置串口数据位
    switch((*ParityBit).toInt())//设置串口校验位
    {
        case 0:pSerialCom->setParity(PAR_NONE);break;
        case 1:pSerialCom->setParity(PAR_ODD);break;
        case 2:pSerialCom->setParity(PAR_EVEN);break;
        default:pSerialCom->setParity(PAR_NONE);break;
    }
    qDebug("StopBit=%d\n",(*StopBit).toInt());
    switch((*StopBit).toInt())//设置串口停止位
    {
        case 1:pSerialCom->setStopBits(STOP_1);break;
        //case 1.5:pSerialCom->setStopBits(STOP_1_5);break;
        case 2:pSerialCom->setStopBits(STOP_2);break;
        default:pSerialCom->setStopBits(STOP_1);break;
    }

    pSerialCom->setFlowControl(FLOW_OFF);//设置控制流
    pSerialCom->setTimeout(500);//设置延时

    return;
}

void TCom::SerialRecData(QString *RecDataAscii)
{
    QByteArray RecDataBuf = pSerialCom->readAll();

    if(m_HexRecFlag)//ture
    {
        QString str = RecDataBuf.toHex().data();
        str = str.toUpper(); //小写转大写
        for(int i=0;i<str.length();i+=2)
        {
            QString st = str.mid(i,2);//从i开始截取2个字符
            *RecDataAscii += st;
            *RecDataAscii += " ";
        }
    }
    else if(m_AsciiRecFlag)
    {
        *RecDataAscii = QString(RecDataBuf);
    }
    else
    {
        *RecDataAscii = "";
    }

    return;
}

void TCom::SerialSendData(QString *SendData)
{
    if(!pSerialCom->isOpen())return;
    if((*SendData).isEmpty())return;
    QByteArray SendDataArray = "";
    if(m_HexSendFlag)
    {
        SendDataArray = QByteArray::fromHex((*SendData).toLatin1().data());
    }
    else if(m_AsciiSendFlag)
    {
        SendDataArray = (*SendData).toLatin1();
    }
    else
    {
        SendDataArray = "";
    }

    pSerialCom->write(SendDataArray);

    return;
}

void TCom::SerialEnum()
{
    QString path = "HKEY_LOCAL_MACHINE\\HARDWARE\\DEVICEMAP\\SERIALCOMM";
    QSettings *setting = new QSettings(path,QSettings::NativeFormat);
    QStringList keys = setting->allKeys();
    int num = (int)keys.size();
    for(int index=0;index<num;index++)
    {
        HKEY hKey;
        if(::RegOpenKeyExW(HKEY_LOCAL_MACHINE,TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"),0,KEY_READ,&hKey) != 0)//打开注册表
        {
            qDebug("Can not open regedit!\n");
            return;
        }
        else
        {
            QString KeyNameMessage;
            QString Message;
            QString KeyValueMessage;
            DWORD keyname_size,keyvalue_size,type;
            unsigned char keyvalue[256];
            wchar_t keyname[256];

            keyname_size = sizeof(keyname);
            keyvalue_size = sizeof(keyvalue);

            if(::RegEnumValueW(hKey,index,keyname,&keyname_size,NULL,&type,keyvalue,&keyvalue_size) == 0)
            {
                for(unsigned int i=0;i<keyname_size;i++)
                {
                    Message = keyname[i];
                    KeyNameMessage.append(Message);
                    //qDebug("%s\n",KeyNameMessage.toStdString().data());
                }
                for(unsigned int j=0;j<keyvalue_size;j++)
                {
                    if(keyvalue[j]!=0x00)
                    {
                        KeyValueMessage.append(keyvalue[j]);
                        //qDebug("%s\n",KeyValueMessage.toStdString().data());
                    }
                }
            }

            m_ComListName<<KeyValueMessage;
            RegCloseKey(hKey);
        }
    }
    delete(setting);
    return;
}






