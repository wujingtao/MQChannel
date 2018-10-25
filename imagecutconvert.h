#ifndef IMAGECUTCONVERT_H
#define IMAGECUTCONVERT_H
#include "qmyglobal.h"
#include <opencv2/core/core.hpp>
#include "opencv2/highgui/highgui.hpp"
using namespace cv;
class imageCutConvert
{
public:
    imageCutConvert();

    static bool isCurretCameraNeedCut(QString cameraID); //判断裁剪队列里是否有规则需要裁剪
    static cv::Mat getCutImage(cv::Mat inputImage,QString cameraID);
    static QVector<QRect> recoverToOriImage(QVector<QRect> pointsNow,QString cameraID);

};

#endif // IMAGECUTCONVERT_H
