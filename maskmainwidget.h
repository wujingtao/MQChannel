#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QObject>
#include <QWidget>
#include "mainupwidget.h"
#include "mainlowwidget.h"
class maskmainwidget : public QWidget
{
    Q_OBJECT
public:
    explicit maskmainwidget(QWidget *parent = 0);
   * m_upmain;
   lowMainWindow* m_lowmain;
signals:

public slots:
};

#endif // LOGINWINDOW_H
