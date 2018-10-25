#include "imagedata.h"
#include "param.h"
#include "assert.h"


ImageData* ImageData::m_pImageData=NULL;

ImageData* ImageData::Instance()
{
    if(m_pImageData==NULL)
    {
        m_pImageData = new ImageData();
    }

    return m_pImageData;
}


ImageData::ImageData()
{
    m_cameraID='0';

    m_listData.clear();
    m_listImage.clear();

    for(int i=0; i<IMAGEDATA_COUNT; i++)
    {
        char *pData = new char[IMAGEDATA_SIZE];

        m_listData.push_back(pData);
    }
}

void ImageData::SetCameraID(QString cameraID)
{
    m_cameraID=cameraID;
    clearCaffeImage();
}

void ImageData::PushCaffeImage(const char* pImg, int size, QString cameraID)
{
    if(size>IMAGEDATA_SIZE) return;
    if(m_cameraID != cameraID) return;

    m_mutexImage.lock();

    if(m_listImage.size() >= IMAGEDATA_COUNT)
    {
        tagImage image = m_listImage.front();
        m_listImage.pop_front();

        PushDataPool(image.pData);
    }

    if(m_listImage.size()<IMAGEDATA_COUNT)
    {
        tagImage image;
        image.pData=popDataPool();
        image.size=size;

        memcpy(image.pData, pImg, size);
        m_listImage.push_back(image);
    }

    m_mutexImage.unlock();
}

bool ImageData::PopCaffeImage(char** pImg, int &size)
{
    bool ret=false;

    m_mutexImage.lock();
    if(!m_listImage.empty())
    {
        tagImage image = m_listImage.front();
        m_listImage.pop_front();

        *pImg = image.pData;
        size= image.size;

        ret =true;
    }
    m_mutexImage.unlock();

    return ret;
}

int ImageData::CaffeImageSize()
{
    int ret=0;
    m_mutexImage.lock();
    ret = m_listImage.size();
    m_mutexImage.unlock();

    return ret;
}

//回收池数据
void ImageData::PushDataPool(char* pData)
{
    assert(pData!=NULL);
    if(pData==NULL) return;

    m_mutexData.lock();
    if(m_listData.size()<IMAGEDATA_COUNT)
        m_listData.push_back(pData);
    else
    {
        delete[] pData;
        pData=NULL;
    }

    m_mutexData.unlock();
}

//取池数据
char* ImageData::popDataPool()
{
    char* pData=NULL;
    m_mutexData.lock();
    if(!m_listData.empty())
    {
        pData=m_listData.front();
        m_listData.pop_front();
    }
    else
    {
        pData = new char[IMAGEDATA_SIZE];
    }
    m_mutexData.unlock();

    assert(pData!=NULL);
    return pData;
}

void ImageData::clearCaffeImage()
{
    m_mutexImage.lock();
    for(ImageList::iterator it = m_listImage.begin(); it!=m_listImage.end(); ++it)
    {
        PushDataPool((*it).pData);
    }

    m_listImage.clear();
    m_mutexImage.unlock();
}
