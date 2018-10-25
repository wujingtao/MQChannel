#-------------------------------------------------
#
# Project created by QtCreator 2017-06-06T11:00:56
#
#-------------------------------------------------

QT       += core gui sql xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MQVIDEO
TEMPLATE = app

win32{
INCLUDEPATH += F:\opencv\install\include
LIBS += -L F:\opencv\install\x86\mingw\lib\libopencv_highgui310.dll.a \
           F:\opencv\install\x86\mingw\lib\libopencv_core310.dll.a \
           F:\opencv\install\x86\mingw\lib\libopencv_imgproc310.dll.a
}

unix{
INCLUDEPATH += /usr/local/include \
                /usr/local/include/opencv \
                /usr/local/include/opencv2 \
                include \
                /usr/local/include  \
                /usr/local/cuda/include  \
                /usr/include \
                /usr/include/python2.7
SOURCES +=     \
               imageanalyse.cpp

HEADERS  +=    \
               imageanalyse.h

LIBS += -L"/usr/local/lib"
LIBS += -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswresample -lswscale

LIBS += -L /usr/local/lib/libopencv_highgui.so \
         /usr/local/lib/libopencv_highgui.so.2.4 \
        /usr/local/lib/libopencv_core.so   \
        /usr/local/lib/libopencv_imgproc.so \
        /usr/local/lib/libcaffe.so.1.0.0-rc3   \
        /usr/lib/x86_64-linux-gnu/libglog.so \
       /usr/lib/x86_64-linux-gnu/libboost_system.so \
       /usr/lib/x86_64-linux-gnu/libboost_python.so \
       /usr/local/lib/gpu_nms.so \
       /usr/lib/x86_64-linux-gnu/libpython2.7.so

}
include(src/qtpropertybrowser.pri)

SOURCES += main.cpp\
        mainwindow.cpp \
       qmyglobal.cpp \
    loginmaskwindow.cpp \
    logindownwindow.cpp \
    loginupwidget.cpp \
    configwidget.cpp \
    qdrawline.cpp \
    picturebox.cpp \
    qffmpeg_rtsp_image.cpp \
    qjudgeposition.cpp \
    datatranslate.cpp \
    HttpRequest.cpp \
    qbottomlistwidget.cpp \
    qlogfile.cpp \
    logwidget.cpp \
    objectdetector.cpp \
    videowidget.cpp \
    NetWorkManager.cpp \
    imagecutconvert.cpp \
    ffmpegdecode.cpp \
    imageserver.cpp \
    imagedata.cpp \
    Logger.cpp \
    caffeserver.cpp \
    cameraserver.cpp \
    uploadserver.cpp \
    AlarmManager.cpp \
    GlobalData.cpp \
    AlarmServer.cpp





HEADERS  += mainwindow.h \
    qmyglobal.h \
    loginmaskwindow.h \
    logindownwindow.h \
    loginupwidget.h \
    configwidget.h \
    qdrawline.h \
    picturebox.h \
    qffmpeg_rtsp_image.h \
    qjudgeposition.h \
    datatranslate.h \
    HttpRequest.h \
    NetWorkManager.h \
    qbottomlistwidget.h \
    qlogfile.h \
    logwidget.h \
    objectdetector.h \
    videowidget.h \
    imagecutconvert.h \
    ffmpegdecode.h \
    imageserver.h \
    imagedata.h \
    param.h \
    Logger.h \
    caffeserver.h \
    cameraserver.h \
    uploadserver.h \
    AlarmManager.h \
    GlobalData.h \
    AlarmServer.h





include(singleapplication/singleapplication.pri)
DEFINES += QAPPLICATION_CLASS=QApplication \

RESOURCES += \
    mq.qrc

FORMS +=
