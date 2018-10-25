#ifndef QDRAWLINE_H
#define QDRAWLINE_H

#include <QList>
#include <QPoint>
#include <QImage>
class qdrawline
{
public:
    qdrawline();

    static void drawLine(QVector<QPoint> pointlist,QImage &img,QColor pointColor,QColor lineColor);                //画线函数
};

#endif // QDRAWLINE_H
