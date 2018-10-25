#include "caffeserver.h"
#include "imagecutconvert.h"
#include "imageanalyse.h"
#include "ffmpegdecode.h"

CaffeServer::CaffeServer(const ChannelVector &vecChannel)
{
    m_vecChannel = vecChannel;
}

void CaffeServer::run()
{
    string model_file = "/usr/bin/mcpt";
    string weights_file = "/usr/bin/mcdel";

    ObjectDetector *pDetector = new ObjectDetector(model_file, weights_file);

    QDateTime startTime = QDateTime::currentDateTime();
    QDateTime endTime = QDateTime::currentDateTime();
    unsigned int msecond=0;
    while(true)
    {
        for(size_t i=0; i<m_vecChannel.size(); i++)
        {
            tagCameraArg cameraArg;
            if(!GLOBAL_DATA->GetCameraArg(m_vecChannel[i], cameraArg))
            {
                continue;
            }

            ffmpegDecode *pFfm = cameraArg.pFfm;
            QString ipcid = cameraArg.ipcid;
            imageAnalyse *pAnalyse = cameraArg.pAnalyse;

            cv::Mat mattemp;

            cv::Mat mat;
            pFfm->getOneFrame(mat);
            if(!mat.empty())
            {                
                tagDetectAll detectAll;
                if(imageCutConvert::isCurretCameraNeedCut(ipcid)) //如果需要裁剪
                {
                    mattemp=imageCutConvert::getCutImage(mat,ipcid);
                    getRectsFromImage(mattemp, pDetector, detectAll);
                }
                else
                    getRectsFromImage(mat, pDetector, detectAll);

                //TODO:
                /*
                if(imageCutConvert::isCurretCameraNeedCut(ipcid))
                    points.nodelist=imageCutConvert::recoverToOriImage(points.nodelist,ipcid);
                */

                pAnalyse->analyse(mat, detectAll);
            }
        }

        endTime = QDateTime::currentDateTime();
        msecond=startTime.msecsTo(endTime);
        if(msecond < CAFFE_DEALED_MSECOND)
        {
            msleep(CAFFE_DEALED_MSECOND-msecond);
            startTime=QDateTime::currentDateTime();
        }
    }
}

void CaffeServer::getRectsFromImage(const cv::Mat &inputimage, ObjectDetector *pDetector, tagDetectAll &detectAll) //caffe返回图片分析结果
{
    map<int,vector<cv::Rect> > detection_result;
    map<int,vector<float> > detection_score;

    detection_result=pDetector->detect(inputimage,&detection_score);

    QVector<QRect> nodelist;
    QVector<float> scores;    

    tagCaffeObject caffeObj;
    for(int i=0;i < detection_result[1].size(); i++){
        QRect rect;
        rect.setX(detection_result[1][i].x);
        rect.setY(detection_result[1][i].y);
        rect.setWidth(detection_result[1][i].width);
        rect.setHeight(detection_result[1][i].height);


        caffeObj.rect=rect;
        caffeObj.score=detection_score[1][i];
        detectAll.heads.push_back(caffeObj);
    }

    //添加犯人
    auto itBegin= detection_result.find(2);
    if (itBegin != detection_result.end())
    {
        QRect rectTmp(0, 0, 0, 0);
        nodelist.append(rectTmp);
        scores.append(0);
        for (int i = 0; i < detection_result[2].size(); i++)
        {
            QRect rect;
            rect.setX(detection_result[2][i].x);
            rect.setY(detection_result[2][i].y);
            rect.setWidth(detection_result[2][i].width);
            rect.setHeight(detection_result[2][i].height);

            caffeObj.rect=rect;
            caffeObj.score=detection_score[2][i];
            detectAll.bodys.push_back(caffeObj);
        }
    }

    //添加警察
    nodelist.clear();
    scores.clear();
    auto itBeginP = detection_result.find(3);
    if (itBeginP != detection_result.end())
    {
        QRect rectTmp(1, 1, 1, 1);
        nodelist.append(rectTmp);
        scores.append(0);
        for (int i = 0; i < detection_result[3].size(); i++)
        {
            QRect rect;
            rect.setX(detection_result[3][i].x);
            rect.setY(detection_result[3][i].y);
            rect.setWidth(detection_result[3][i].width);
            rect.setHeight(detection_result[3][i].height);

            caffeObj.rect=rect;
            caffeObj.score=detection_score[3][i];
            detectAll.polices.push_back(caffeObj);
        }
    }
}
