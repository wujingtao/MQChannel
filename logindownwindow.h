#ifndef LOGINDOWNWINDOW_H
#define LOGINDOWNWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QLabel>
class logindownwindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit logindownwindow(QWidget *parent = 0);

     void initWindow();  //初始化界面
};

#endif // LOGINDOWNWINDOW_H
