#ifndef IMGDISPLAY_H
#define IMGDISPLAY_H
#include "qthread.h"
#include "videowidget.h"
class imgDisplay:public QThread
{
public:
    imgDisplay();
    void run();
    void exit();
private:
    VideoWidget video;
};

#endif // IMGDISPLAY_H
