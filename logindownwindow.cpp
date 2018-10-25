#include "logindownwindow.h"
#include "qmyglobal.h"
logindownwindow::logindownwindow(QWidget *parent) : QMainWindow(parent)
{
  initWindow();
}

void logindownwindow::initWindow()
{
  //  setMinimumSize(screenX,screenY/4*1);
    setMaximumSize(screenX,screenY/4*1);
    resize(screenX,screenY/4*1);
    setWindowFlags(Qt::FramelessWindowHint);
    this->setStyleSheet("logindownwindow{border-image:url(:/image/bg_2.png);}");
}
