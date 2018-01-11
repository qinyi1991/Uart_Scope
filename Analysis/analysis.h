#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <QArrayData>
#include <QByteArray>
#include <QVector>

typedef struct
{
    unsigned short x_value;
    unsigned short y_value;

}t_Dots;

typedef union
{
    struct
    {
        unsigned char LOW_BYTE;
        unsigned char HIGH_BYTE;

    }Data;

    short value;
}t_DataValue;

class TAnalysis
{
public:
    TAnalysis();
    ~TAnalysis();

    void AnalysisRecvData(QString &str);//协议数据解析
    bool uStrcmp(unsigned char *str1,char *str2);
    unsigned char HexToValue(QString s);

public:
    unsigned char m_StartBytes[4];  //起始字节
    unsigned char m_Channels;       //通道数
    unsigned short m_ChannelsLeg;   //通道总长
    unsigned short m_PositiveSpeed;
    unsigned short m_PositiveAmp;
    unsigned short m_NegativeSpeed;
    unsigned short m_NegativeAmp;
    unsigned short m_StopByte[4];
    unsigned short m_DotNum;
    unsigned short m_DisplayDotNum;

    double *m_Channel_x;
    double *m_Channel1_y;
    double *m_Channel2_y;
    double *m_Channel3_y;
    double *m_Channel4_y;
    double *m_Channel5_y;
    double *m_Channel6_y;
    double *m_Channel7_y;
    double *m_Channel8_y;

    QString m_AnalysisBuf;

};

#endif // ANALYSIS_H
