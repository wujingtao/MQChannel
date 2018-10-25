#ifndef OBJECTDETECTOR_H
#define OBJECTDETECTOR_H

#define INPUT_SIZE_NARROW  512
#define INPUT_SIZE_LONG  512

#include <string>
#include <caffe/net.hpp>
#include <caffe/common.hpp>
#include <opencv2/core/core.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <qmutex.h>

using namespace caffe;
class ObjectDetector
{
public:
      ObjectDetector(const std::string &model_file, const std::string &weights_file);  //构造函数
    //对一张图片，进行检测，将结果保存进map数据结构里,分别表示每个类别对应的目标框，如果需要分数信息，则计算分数
      map<int,vector<cv::Rect> > detect(const cv::Mat& image, map<int,vector<float> >* score=NULL);

      //调用detect函数内，需手动设置
      static void SetMode()
      {
#ifdef CPU_ONLY
    Caffe::set_mode(Caffe::CPU);
#else
    Caffe::set_mode(Caffe::GPU);
#endif
      }
private:
    boost::shared_ptr< caffe::Net<float> > net_;
    QMutex m_mutex;
};

#endif
