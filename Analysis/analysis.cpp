#include "Analysis/analysis.h"
#include "qbytearray.h"
#include <QFile>
#include <QDebug>

TAnalysis::TAnalysis()
{
    m_Channels = 4;
    m_ChannelsLeg = 256;
    m_Channel_x = new double   [0xffff];
    m_Channel1_y = new double  [0xffff];
    m_Channel2_y = new double  [0xffff];
    m_Channel3_y = new double  [0xffff];
    m_Channel4_y = new double  [0xffff];
    m_Channel5_y = new double  [0xffff];
    m_Channel6_y = new double  [0xffff];
    m_Channel7_y = new double  [0xffff];
    m_Channel8_y = new double  [0xffff];

}

TAnalysis::~TAnalysis()
{
    delete [] m_Channel_x;
    delete [] m_Channel1_y;
    delete [] m_Channel2_y;
    delete [] m_Channel3_y;
    delete [] m_Channel4_y;
    delete [] m_Channel5_y;
    delete [] m_Channel6_y;
    delete [] m_Channel7_y;
    delete [] m_Channel8_y;
}

void TAnalysis::AnalysisRecvData(QString &str)
{
    /*
    //从文件中读取数据
    QFile file("test.txt");
    if(!file.open(QFile::ReadOnly | QIODevice::Text))
    {
        qDebug("can not open file!\n");
        return;
    }
    QByteArray DataArray;
    while (!file.atEnd())
    {
        DataArray = file.readAll();//读出所有字符
    }
    file.close();
    QString str(DataArray);
    */

    QString StartBytes = "E4 2C E4 2C ";
    QString StopBytes = "E4 8B E4 8B ";
    QString SSConnect="E4 8B E4 8B E4 2C E4 2C";
    if(m_AnalysisBuf.contains(SSConnect))
    {
         m_AnalysisBuf.section(StopBytes,1,1);
    }
  //  m_AnalysisBuf.insert(m_AnalysisBuf.end(),StartBytes);
    if(m_AnalysisBuf.contains(StartBytes)&&m_AnalysisBuf.contains(StopBytes))
    {
        if(m_AnalysisBuf.startsWith(StartBytes))
        {
            qDebug("Get a valid Frame!\n");
            qDebug()<<m_AnalysisBuf;
            int nFrameLen = m_AnalysisBuf.length();
            qDebug("A Frame Length=%d\n",nFrameLen);
            unsigned char temp[nFrameLen];
            int nDataCount=0;
            for(int i=0;i<nFrameLen;i+=3)
            {
                QString st = m_AnalysisBuf.mid(i,2);//从i开始截取2个字符
                temp[nDataCount++] = HexToValue(st);//将hex字符转成数字值
                //printf("%x,",temp[nDataCount-1]);
            }
            qDebug("A Frame Data Counts:%d\n",nDataCount);
            m_AnalysisBuf.clear();
            switch (char step=0)
            {
                case 0://截取起始字节
                {
                     if((temp[0]==0xE4)&&(temp[1]==0x2C)&&(temp[2]==0xE4)&&temp[3]==0x2C)
                     {
                         step = 1;
                     }
                     else
                     {
                         qDebug("StartBytes Error!\n");
                         qDebug("0x%x,0x%x,0x%x,0x%x\n",temp[0],temp[1],temp[2],temp[3]);
                         break;
                     }
                }

                case 1:
                {
                    if((temp[nDataCount-1]==0x8B)&&(temp[nDataCount-2]==0xE4)&&(temp[nDataCount-3]==0x8B)&&(temp[nDataCount-4]==0xE4))
                    {
                        step = 2;
                    }
                    else
                    {
                        qDebug("StopBytes Error!\n");
                        qDebug("0x%x,0x%x,0x%x,0x%x\n",temp[nDataCount-4],temp[nDataCount-3],temp[nDataCount-2],temp[nDataCount-1]);
                        break;
                    }
                }

                case 2://截取通道数
                {
                    m_Channels = temp[4];
                    qDebug("m_Channels = %x\n",m_Channels);
                    step = 3;
                }

                case 3: //截取通道长度
                {
                    t_DataValue channellen;
                    channellen.Data.HIGH_BYTE = temp[5];
                    channellen.Data.LOW_BYTE = temp[6];
                    m_ChannelsLeg = channellen.value;
                    m_DotNum = temp[5]*0xffff+temp[6];//从数据帧中获取的点数
                    m_DisplayDotNum = (nDataCount-19)/(2*m_Channels);//实际点数
                    qDebug("m_ChannelsLeg = %d, m_DotNum = %d, m_DisplayDotNum=%d\n",m_ChannelsLeg,m_DotNum,m_DisplayDotNum);
                    step = 4;
                }

                case 4: //截取坐标数据
                {
                    int pos = 7;
                    t_DataValue dotvalue;
                    double tmp[m_Channels];
                    int nValueCount = (nDataCount-19);
                    qDebug("A Frame Value Count:%d\n",nValueCount);
                    for(int n=0;n<m_DisplayDotNum;n++)
                    {
                        for(int m=0;m<m_Channels;m++)//通道的第一个点
                        {
                            dotvalue.Data.HIGH_BYTE = temp[pos++];
                            dotvalue.Data.LOW_BYTE = temp[pos++];
                            tmp[m] = (double)dotvalue.value;
                        }
                        m_Channel1_y[n] = tmp[0];
                        m_Channel2_y[n] = tmp[1];
                        m_Channel3_y[n] = tmp[2];
                        m_Channel4_y[n] = tmp[3];
                        m_Channel_x[n] = (double)(n*m_ChannelsLeg/m_DisplayDotNum-(m_ChannelsLeg/2-1));
                    }
                }
                break;

                default:
                    break;
            }
        }
        /*else
        {
            m_AnalysisBuf.section(StartBytes,-1,-1);
        }*/
    }
    else
    {
        //qDebug("Invalid Data Frame,Get More\n");
        m_AnalysisBuf.append(str);
        //qDebug()<<m_AnalysisBuf;
    }

    return;
}


bool TAnalysis::uStrcmp(unsigned char *str1, char *str2)
{
    while((*str1 != '\0')&&(*str2 != '\0'))
    {
        qDebug("str1=%x,str2=%x\n",*str1,(*str2&0xff));
        if(*str1 != (*str2&0xff))return false;
        str1++;
        str2++;
    }

    return true;
}

unsigned char TAnalysis::HexToValue(QString s)
{
       if(s.length()>2)
       {
           qDebug("The String is more than two charactors\n");
           return 0;
       }

       unsigned char n = 0;
       for (int i=0;i<2;i++)
       {
           if (s.at(i) > '9')
           {
               n = 16 * n + (10 + s.at(i).toLatin1() - 'A');
           }
           else
           {
               n = 16 * n +( s.at(i).toLatin1() - '0');
           }
       }

       return n;
}
