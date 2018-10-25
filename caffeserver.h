#ifndef CAFFESERVER_H
#define CAFFESERVER_H
#include <qmyglobal.h>
#include <qthread.h>
#include "objectdetector.h"
#include "GlobalData.h"

class CaffeServer : public QThread
{
public:
    CaffeServer(const ChannelVector &vecChannel);

    void run();

private:
    void getRectsFromImage(const cv::Mat &inputimage, ObjectDetector *pDetector, tagDetectAll &detectAll);

private:
    ChannelVector m_vecChannel;
};

#endif // CAFFESERVER_H
