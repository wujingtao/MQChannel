#include "imagecutconvert.h"

imageCutConvert::imageCutConvert()
{

}


bool imageCutConvert::isCurretCameraNeedCut(QString cameraId)
{
    if(map_CutImageRule.contains(cameraId))
        return true;
    else return false;
}


cv::Mat imageCutConvert::getCutImage(cv::Mat inputImage,QString cameraID)
{
    if(map_CutImageRule.contains(cameraID))
    {
        QVector<QPoint> points= map_CutImageRule[cameraID].points;
        int max_x,max_y,min_x,min_y;
        for(int i=0;i<points.size();i++)
        {
            QPoint point=points.at(i);
            if(i==0)
            {
            max_x=point.x();
            min_x=point.x();
            max_y=point.y();
            min_y=point.y();
            }
            else
            {
                if(point.x()>max_x) max_x=point.x();
                else if(point.x()<min_x) min_x=point.x();

                if(point.y()>max_y) max_y=point.y();
                else if(point.y()<min_y) min_y=point.y();
            }
        }
        cv::Mat outImage;
        Rect rect(min_x,min_y, max_x-min_x, max_y-min_y);
        outImage = inputImage(rect);
       // cv::imwrite("/home/mq/111/111.jpg",outImage);
        return outImage;
    }
    return inputImage;
}

QVector<QRect> imageCutConvert::recoverToOriImage(QVector<QRect> pointsNow,QString cameraID)
{
    int max_x,max_y,min_x,min_y;
    if(map_CutImageRule.contains(cameraID))
    {
        QVector<QPoint> points= map_CutImageRule[cameraID].points;
        for(int i=0;i<points.size();i++)
        {
            QPoint point=points.at(i);
            if(i==0)
            {
            max_x=point.x();
            min_x=point.x();
            max_y=point.y();
            min_y=point.y();
            }
            else
            {
                if(point.x()>max_x) max_x=point.x();
                else if(point.x()<min_x) min_x=point.x();

                if(point.y()>max_y) max_y=point.y();
                else if(point.y()<min_y) min_y=point.y();
            }
        }
    }
    else
        return pointsNow;

    QVector<QRect> pointsBack;
    pointsBack.clear();
    for(int i=0;i<pointsNow.size();i++)
    {
        QRect rec=pointsNow.at(i);
        //人头/警察/犯人的分割点
        if ((rec.x() == 0 && rec.y() == 0 && rec.width() == 0 && rec.height() == 0)||
            (rec.x() == 1 && rec.y() == 1 && rec.width() == 1 && rec.height() == 1))
        {
           pointsBack.push_back(rec);
           continue;
        }
        else
        {
        int x_cut=rec.x();
        int y_cut=rec.y();

        int x_now=x_cut+min_x;
        int y_now=y_cut+min_y;

        QRect rec_now(x_now,y_now,rec.width(),rec.height());
        pointsBack.push_back(rec_now);
        }
    }
    return pointsBack;
}
