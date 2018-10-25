#include "objectdetector.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <fstream>
#include "qmyglobal.h"
#include <QDebug>

using std::string;
using std::vector;
using namespace caffe;
using  std::max;
using std::min;



ObjectDetector::ObjectDetector(const std::string &model_file,const std::string &weights_file){
#ifdef CPU_ONLY
    Caffe::set_mode(Caffe::CPU);
#else
    Caffe::set_mode(Caffe::GPU);
#endif

    net_.reset(new Net<float>(model_file, TEST));
    net_->CopyTrainedLayersFrom(weights_file);
}

//对一张图片，进行检测，将结果保存进map数据结构里,分别表示每个类别对应的目标框，如果需要分数信息，则计算分数
map<int,vector<cv::Rect> > ObjectDetector::detect(const cv::Mat& image, map<int,vector<float> >* objectScore)
{

    if(objectScore!=NULL)   //如果需要保存置信度
        objectScore->clear();

    map<int,vector<cv::Rect> > label_objs;    //每个类别，对应的检测目标框
    int height = 512;
    int width = 512;

    cv::Mat cv_resized;
    image.convertTo(cv_resized, CV_32FC3);
    cv::resize(cv_resized, cv_resized, cv::Size(width, height));
    cv::Mat mean(height, width, cv_resized.type(), cv::Scalar(102.9801, 115.9465, 122.7717));
    cv::Mat normalized;
    subtract(cv_resized, mean, normalized);

    m_mutex.lock();
    boost::shared_ptr<Blob<float>>  input_layer = net_->blob_by_name("data");
    input_layer->Reshape(1, 3, height, width);
    net_->Reshape();

    float* input_data = input_layer->mutable_cpu_data();
    vector<cv::Mat> input_channels;
    for (int i = 0; i < input_layer->channels(); ++i) {
        cv::Mat channel(height, width, CV_32FC1, input_data);
        input_channels.push_back(channel);
        input_data += height * width;
    }
    cv::split(normalized, input_channels);
    net_->ForwardFrom(0);   //进行网络前向传播，//耗时多

    Blob<float>* result_blob = net_->output_blobs()[0];    

    const float* result = result_blob->cpu_data();
    const int num_det = result_blob->height();
    vector<vector<float> > detections;
    map<int,vector<float> > objectScore1;
    for (int k = 0; k < num_det; ++k) {
        if (result[0] == -1) {
            // Skip invalid detection.
            result += 7;
            continue;
        }
        vector<float> detection(result, result + 7);
        detections.push_back(detection);
        result += 7;
    }
    m_mutex.unlock();

    for(int i=0;i<detections.size();i++)
    {
    if(detections[i][2]>CONF_THRESH)
    {
    int class_no=detections[i][1];
    float xmin=detections[i][3]*image.cols;
    float xmax=detections[i][5]*image.cols;
    float ymin=detections[i][4]*image.rows;
    float ymax=detections[i][6]*image.rows;
    float score=detections[i][2];

    label_objs[class_no].push_back(cv::Rect(cv::Point(xmin,ymin),cv::Point(xmax,ymax)));
    objectScore1[class_no].push_back(score);
    }
    }

    for(int j=0;j<objectScore1.size();++j)
    {
        vector<float> tmp;
        tmp=objectScore1[j];
        objectScore->insert(pair<int,vector<float> >(j,tmp));
    }

    return label_objs;
}
