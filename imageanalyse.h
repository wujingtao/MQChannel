#ifndef IMAGEANALYSE_H
#define IMAGEANALYSE_H
/***************************************************************************
 * //图片处理类(主要的事务流程控制)
 * //具体功能：
 * //1.线程轮询自己的图片文件夹，获取图片
 * //2.获取的图片交由caffe处理，返回坐标
 * //3.坐标集和规则集去匹配，发现异常情况
 * //4.保存异常情况到本地（循环存储）
 * //5.异常情况通知服务器后台
 * ************************************************************************/

#include <QVector>
#include <QRect>
#include <QDomComment>
#include "qmyglobal.h"
#include "qdrawline.h"

class imageAnalyse
{
public:
    imageAnalyse(QString cameraID, const QString &savePath);

    void analyse(cv::Mat &inputimage, const tagDetectAll &detectAll);

private:        
    void collectImage(const vector<uchar> &vecbuf, int width,int height, const QDateTime &nowtime, const tagDetectAll &detectAll);

    void writexml(QString autoWriteXML, const tagDetectAll &dectetAll);
    QDomElement xmlElement(QDomDocument &domDocument, const QString &element, int minx, int miny, int maxx, int maxy);

    bool saveimage(cv::Mat &image, string filename);
    void deleteImagesCircle(QString path, const QDateTime &nowTime);

    //分析画框
    void xmlToMat(cv::Mat &mat, const tagDetectAll &detectAll);
    void drawBox(const QString &txtFlag, const tagCaffeObject &caffeObj, cv::Mat &mat);
    //dealed图像
    void dealedImage(const cv::Mat &mat, const QString &filefullpath, const QDateTime &nowtime);


private:   
    QString m_cameraID;                           //当前摄像头ID

    AlarmRecordMap m_alarmRecords;
    QString m_savepath;
    QDateTime m_delTime;
    QDateTime m_collectTime;
};

#endif // IMAGEANALYSE_H
