#include "qdrawline.h"
#include <QDebug>
using namespace std;
#include "qmyglobal.h"

qdrawline::qdrawline()
{

}

void qdrawline::drawLine(QVector<QPoint> pointlist,QImage &img,QColor pointColor,QColor lineColor)                //画线函数
{
    if(pointlist.size()==0) return;
    Mat srcImage;
    cv::Mat image=QImageToMat(img);
   // cv::cvtColor(image, image, CV_BGR2RGBA);
    srcImage=image.clone();

    Point pt1, pt2;//两点坐标
    QRgb rgb;
    for(int i=0;i<pointlist.size();i++)
    {
        if (i == 0)
        {
        pt1 = Point(pointlist[i].x(), pointlist[i].y());
        rgb=pointColor.rgb();
        circle(srcImage, pt1, 1, Scalar(CV_RGB(qRed(rgb), qGreen(rgb), qBlue(rgb))),2,2);//描第一个点
        }
        else
        {
        pt2 = Point(pointlist[i].x(), pointlist[i].y());
        rgb=pointColor.rgb();
        circle(srcImage, pt2, 1, Scalar(CV_RGB(qRed(rgb), qGreen(rgb), qBlue(rgb))),2,2);//描点
        rgb=lineColor.rgb();
        line(srcImage, pt1, pt2, Scalar(CV_RGB(qRed(rgb), qGreen(rgb), qBlue(rgb))),2,2);//与前一点进行连线
        pt1 = pt2;
        }
    }
    img=MatToQImage(srcImage);
}
