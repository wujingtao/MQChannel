#ifndef QBOTTOMLISTWIDGET_H
#define QBOTTOMLISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QVector>
#include <QDateTime>
#include <QMutex>

#include "qmyglobal.h"

class QBottomListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit QBottomListWidget(QWidget *parent = 0);
    ~QBottomListWidget();
    

signals:
    void bottomLogEvent(int);

public slots:
    void AddLog(QString strLog,int MessageLevel);
    void dealLogEvent(int MessageLevel);

private:
    QMutex m_mutex;
    QVector<QString> m_vectorStr;
    
};

#endif // QBOTTOMLISTWIDGET_H
