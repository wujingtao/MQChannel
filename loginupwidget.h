#ifndef LOGINUPWINDOW_H
#define LOGINUPWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include "qmyglobal.h"
class COneIPLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    COneIPLineEdit(QWidget *parent);
    ~COneIPLineEdit();
//signals:
//  void SignalFinished();
//
//public slots:
//  void SlotPreFinished();
//
//private:
//  void SlotTextChanged();
};
/*********************************************************************************/
class CIPLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    CIPLineEdit(QWidget *parent);
    ~CIPLineEdit();
    void setGeometry(int x, int y, int w, int h);
    bool SetIPText(int nIP, int nIndex);
    int GetIPText(int nIndex);
private:
    void paintEvent(QPaintEvent *e);

private:
    COneIPLineEdit *m_pLineEdit[4];
};



class loginUpWindow : public QMainWindow
{
     Q_OBJECT
public:
    explicit loginUpWindow(QWidget *parent = 0);

    QPushButton *buttonLogin;
    QPushButton *buttonExit;
    QPushButton *buttonConfig;

    QPushButton * saveConfigButton;    //保存配置

    main3Button * closeButton;    //关闭按钮
    main3Button * minButton;      //最小化按钮
    bool isLogin();
    QLabel *labMessage;          //登录消息反馈
private:

    QLabel *softwareName;
    QLabel *labLogo;
    QLabel *labLogoFont;
    QLabel *companyLogo;
    QLabel *companyTips;
    QLabel *companyLine;
    QLabel *labUserName;
    QLabel *labPasswd;
    QLabel *labsmallBox;

    QLineEdit *lineEdUserName;
    QLineEdit *lineEdPasswd;

    QLabel *labseverIp;          //ip
    CIPLineEdit  * lineEdIP;


    QLabel *labseverPort;        //port
    QLineEdit * lineEdPort;

    QLabel *labseverName;        //服务器ID
    QLineEdit * lineEdseverName; //



    QRect location;

    void initWindow();  //初始化界面
    void initConfig();  //配置界面初始化
private slots:
    void slot_ConfigButton();
    void slot_SaveConfig();
    void ConfigButton2();
};



#endif // LOGINUPWINDOW_H
