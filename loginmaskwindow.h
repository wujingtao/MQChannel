#ifndef LOGINMASKWINDOW_H
#define LOGINMASKWINDOW_H

#include "logindownwindow.h"
#include "loginupwidget.h"
#include <QObject>
#include <QWidget>
class loginmaskwindow: public QWidget
{
    Q_OBJECT
public:
    explicit loginmaskwindow(QWidget *parent = 0);
    loginUpWindow *loginupwd;
    logindownwindow *logindownwd;
};

#endif // LOGINMASKWINDOW_H
