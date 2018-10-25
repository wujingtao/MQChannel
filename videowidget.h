#ifndef WIDGET2_H
#define WIDGET2_H

#include <QWidget>
#include <QLabel>
#include <QTreeWidget>
#include "qmyglobal.h"
#include<qtimer.h>

class VideoWidget :public QWidget
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = 0);

    QRect location;
    QPushButton * refreshTreeAndConfig; //刷新树列表和配置
    QTreeWidget * videoTreewidget;
    QLabel * video_1;
    void initWindow();

    void closeImg();
    QTimer *timer;
    QString url;
    void showCamera();
public slots:
        void readImg();
        void loadCameraTree();
        void on_videoTreewidget_doubleClicked(const QModelIndex &index);
        static QFileInfoList GetFileList(QString path);

};

#endif // WIDGET2_H
