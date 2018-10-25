#ifndef UPLOADSERVER_H
#define UPLOADSERVER_H

#include <qthread.h>

#include "qmyglobal.h"

class UploadServer : public QThread
{
public:
    UploadServer();

    void run();

private:
    bool upload(mAlarmRecord_stu &record);
};

#endif // UPLOADSERVER_H
