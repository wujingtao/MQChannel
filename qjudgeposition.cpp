#include "qjudgeposition.h"
#include <QVector>
#include "math.h"
#include <QDebug>
#define eps 1e-5

inline double max(double a,double b) {
    return a>b?a:b;
}
inline double min(double a,double b) {
    return a>b?b:a;
}

QJudgePosition::QJudgePosition()
{

}

double getDistance(QPoint p1,QPoint p2,QPoint p3)  //p2为点， P1P3为线段
{
    double x=p1.x();
    double y=p1.y();
    double x1=p2.x();
    double y1=p2.y();
    double x2=p3.x();
    double y2=p3.y();

    double px = x2 - x;
    double py = y2-  y;
    double som = px * px + py * py;
    double u =  ((x1 - x) * px + (y1- y) * py) / som;
    if (u > 1) {
        u = 1;
    }
    if (u < 0) {
        u = 0;
    }
    //the closest point
    double xa = x + u * px;
    double ya = y + u * py;
    double dx = xa - x1;
    double dy = ya - y1;
    double dist = sqrt(dx*dx + dy*dy);

    return dist;
}

bool QJudgePosition::online(QPoint p1,QPoint p2,QPoint p3)
{
    double distance=getDistance(p1,p2,p3);
    if(distance<5)
    {
        return true;
    }
    return false;
}

bool QJudgePosition::onPolygonLine(QPoint p,QVector<QPoint> points)
{
    int counter=0;
    double xinters;
    QPoint p1,p2;
    if(points.empty()){
        return false;
    }
    p1=points[0];
    int n=points.size();
    for(int i=1; i<=n; i++) {
        p2=points[i%n];
        if(online(p1,p,p2)) return true;
        p1=p2;
    }
    return false;
}

bool QJudgePosition::insidepolygon(QPoint p,QVector<QPoint> points)
{
    int counter=0;
    double xinters;
    QPoint p1,p2;
    if(points.empty()){
        return false;
    }
    p1=points[0];
    int n=points.size();
    for(int i=1; i<=n; i++) {
        p2=points[i%n];
        if(online(p1,p,p2)) return true;
        if(p.y()>min(p1.y(),p2.y())) {
            if(p.y()<=max(p1.y(),p2.y())) {
                if(p.x()<=max(p1.x(),p2.x())) {
                    if(p1.y()!=p2.y()) {
                        xinters=(p.y()-p1.y())*(p2.x()-p1.x())/(p2.y()-p1.y())+p1.x();
                        if(p1.x()==p2.x()||p.x()<=xinters) counter++;
                    }
                }
            }
        }
        p1=p2;
    }
    if(counter%2==0)
        return false;
    return true;
}

double QJudgePosition::disOfTwoPoints(QPoint a,QPoint b)
{
    double dis = 0.0;
    double e=a.x() - b.x();
    double d=a.y() - b.y();
    dis = sqrt(e*e+ d*d);
    return dis;
}
