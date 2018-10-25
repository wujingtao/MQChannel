#ifndef IMAGEDATA_H
#define IMAGEDATA_H
#include <qmutex.h>
#include <string.h>
#include <list>
#include <QVariant>
using std::list;

struct tagImage
{
    char* pData;
    int   size;
};
typedef list<char*> DataList;
typedef list<tagImage> ImageList;

class ImageData
{
public:
    static ImageData* Instance();

private:
    ImageData();
    ImageData(const ImageData&);
    ImageData& operator=(const ImageData&);

public:
    void SetCameraID(QString cameraID);

    //图像
    void PushCaffeImage(const char* pImg, int size, QString cameraID);
    bool PopCaffeImage(char** pImg, int &size);
    int CaffeImageSize();
    //回收池数据
    void PushDataPool(char* pData);

private:
    //取池数据
    char* popDataPool();

    void clearCaffeImage();

public:
    static ImageData *m_pImageData;

private:
    QString m_cameraID;
    //xml图像数据
    ImageList m_listImage;
    QMutex m_mutexImage;
    //数据池
    DataList m_listData;
    QMutex m_mutexData;
};

#define IMAGEDATA_INSTANCE ImageData::Instance()

#endif // IMAGEDATA_H
