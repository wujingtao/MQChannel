#ifndef LOGWIDGET_H
#define LOGWIDGET_H
#include <QWidget>
#include "qbottomlistwidget.h"

class logwidget:public QWidget
{
     Q_OBJECT
public:
    explicit logwidget(QWidget *parent = 0);

    QBottomListWidget *taskLogView;
    void initWindow();
};

#endif // LOGWIDGET_H
