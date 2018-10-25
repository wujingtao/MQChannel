#include "logwidget.h"
logwidget::logwidget(QWidget *parent) :
    QWidget(parent)
{
    initWindow();
}

void logwidget::initWindow()
{
    setMaximumSize(screenX,screenY*0.915);
    resize(screenX,screenY*0.915);
    taskLogView=new QBottomListWidget(this);
    taskLogView->setFixedSize(screenX*0.845,screenY*0.915);
    taskLogView->setGeometry(screenX*0.15599,screenY*0.0006,taskLogView->width(),taskLogView->height());
    //connect(g_pLogFile,SIGNAL(logToWidgetEvent(QString,int)),taskLogView,SLOT(AddLog(QString,int)),Qt::QueuedConnection);
}
