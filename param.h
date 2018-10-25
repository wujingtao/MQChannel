#ifndef PARAM_H
#define PARAM_H

//时间有关
const int COLLECT_TIME_SECOND = 60;   //间隔时间收集1张图片
const int COLLECT_KEEP_SECOND = 1800; //保留时间
const int COLLECT_DEL_SECOND = 300;   //间隔时间执行删除
const int CAFFE_DEALED_MSECOND = 1000;     //定值，相机1s取1张图片
const int REFRESH_DISCORRECT_ALARM_SECOND = 60;//获取不正确报警周期

//大小有关
const int OBJECT_CAFFE_COUNT=4;       //caffe模型数量4(1个占用内存1g)
const int CHANNEL_COUNT=16;            //支持相机数量最大值
const int LIST_MAXSIZE=3;             //队列相机图片数量
const int IMAGE_HEAD_COUNT=120;
const int MP4_IMAGE_COUNT=40;
//imageserver图像服务
const int PORT = 9999; //图像服务端口
const int BUFF_LENGTH = 4; //buff长度大小
const int CAMERA_ID_LENGTH = 8; //ipcid长度大小

//imagedata
const int IMAGEDATA_COUNT = 10;  //图像服务caffe图像数量
const int IMAGEDATA_SIZE = 1920*1024; //图片大小最大值

const int PIC_WIDTH=1280; //图片框
const int PIC_HEIGHT=720; //图片高

//imageanalyse
const bool DRAW_BOX=true; //是否画矩形框

//obejctdetector
const float CONF_THRESH = 0.9; //置信度阈值

const bool OPEN_DISCORRECT_ALARM=false;

#endif // PARAM_H
