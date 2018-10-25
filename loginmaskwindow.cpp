#include "loginmaskwindow.h"
#include "qmyglobal.h"
loginmaskwindow::loginmaskwindow(QWidget *parent) : QWidget(parent)
{
  setMaximumSize(screenX,screenY);
  resize(screenX,screenY);
 // setMinimumSize(screenX,screenY);
  loginupwd=new loginUpWindow(this);
  logindownwd=new logindownwindow(this);
  loginupwd->setGeometry(0,0,loginupwd->width(),loginupwd->height());
  logindownwd->setGeometry(0,screenY/4*3,logindownwd->width(),logindownwd->height());
}
