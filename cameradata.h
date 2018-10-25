#ifndef CAMERADATA_H
#define CAMERADATA_H

#include <opencv2/core/core.hpp>
#include <qvector.h>
#include <QRect>
#include <list>
#include <vector>
using namespace std;

#include "param.h"

class ffmpegDecode;
class imageAnalyse;
struct tagCameraData
{
    ffmpegDecode* pFfm;
    imageAnalyse* pAnalyse;
    int ipcid;
};
typedef std::vector<tagCameraData> CameraDataVector;

//报警得分和框
typedef struct detection_all{
    QVector<QRect> nodelist;
    QVector<float> scores;
}detect_all;

struct tagDetectCaffe
{
    detect_all detect;
    cv::Mat mat;
};
typedef list<tagDetectCaffe> DetectCaffeList;
typedef list<cv::Mat> MatList;

class CameraData
{
public:
    CameraData()
    {
        m_listMat.clear();
        m_listDetectCaffe.clear();
    }

public:
    void PushMat(const cv::Mat &mat)
    {
        m_mutexMat.lock();
        if(m_listMat.size()>=LIST_MAXSIZE)
            m_listMat.pop_front();

        m_listMat.push_back(mat);
        m_mutexMat.unlock();
    }

    bool PopMat(cv::Mat &mat)
    {
        bool ret = false;
        m_mutexMat.lock();
        if(!m_listMat.empty())
        {
            mat = m_listMat.front();
            m_listMat.pop_front();
            ret=true;
        }
        m_mutexMat.unlock();

        return ret;
    }

    int MatSize()
    {
        int ret=0;
        m_mutexMat.lock();
        ret = m_listMat.size();
        m_mutexMat.unlock();

        return ret;
    }

    void PushDetectCaffe(const cv::Mat &mat, const detect_all &detect)
    {
       m_mutexDetectAll.lock();
       //大于缓存值则删除最前面后再加入
       if(m_listDetectCaffe.size() >= LIST_MAXSIZE)
       {
           m_listDetectCaffe.pop_front();
       }

       tagDetectCaffe detectCaffe;
       detectCaffe.mat=mat;
       detectCaffe.detect=detect;
       m_listDetectCaffe.push_back(detectCaffe);

       m_mutexDetectAll.unlock();
    }

    bool PopDetectCaffe(cv::Mat &mat, detect_all &detect)
    {
        bool ret=false;
        m_mutexDetectAll.lock();
        if(!m_listDetectCaffe.empty())
        {
            tagDetectCaffe &detectCaffe = m_listDetectCaffe.front();
            mat = detectCaffe.mat;
            detect = detectCaffe.detect;

            m_listDetectCaffe.pop_front();

            ret=true;
        }
        m_mutexDetectAll.unlock();

        return ret;
    }

    int DetectCaffeSize()
    {
        int ret=0;
        m_mutexDetectAll.lock();
        ret = m_listDetectCaffe.size();
        m_mutexDetectAll.unlock();

        return ret;
    }

private:
    //原始图像    
    MatList m_listMat;
    QMutex m_mutexMat;
    //原始图像，xml
    DetectCaffeList m_listDetectCaffe;
    QMutex m_mutexDetectAll;
};

#endif // CAMERADATA_H
