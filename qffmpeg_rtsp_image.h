#ifndef QFFMPEG_RTSP_IMAGE_H
#define QFFMPEG_RTSP_IMAGE_H
#include <QString>
#include <QProcess>
#include "ffmpegdecode.h"
/*------------------------------------------------------------------
 * 2017-6-23 11：04
 * 利用ffmpeg对rtsp流进行截图处理
 * 基本命令： ffmpeg  -i  rtsp://admin:12345@192.168.1.65:554/h264/ch1/main/av_strea5  -r 2/3 -y -f image2 /home/mq/images%05d.png -c copy -map 0 -f segment -segment_time 60 -segment_format mp4 /home/mq/out%03d.mp4
 * ffmpeg -i  /home/mq/111/001.mp4 -r 2/3  -y -f image2 /home/mq/images%05d.png
 * ffmpeg  -i  rtsp://admin:12345@192.168.1.65:554/h264/ch1/main/av_strea5 -i  rtsp://admin:12345@192.168.1.88:554/h264/ch1/main/av_strea5 -i  rtsp://admin:12345@192.168.1.65:554/h264/ch1/main/av_strea5 -i  rtsp://admin:12345@192.168.1.88:554/h264/ch1/main/av_strea5 -map 0:0 -r 3  -y  -s 720x480 -f image2 /home/mq/0/images%05d.jpeg -map 1:0 -r 3  -y  -s 720x480 -f image2 /home/mq/1/images%05d.jpeg -map 2:0 -r 3  -y  -s 720x480 -f image2 /home/mq/2/images%05d.jpeg -map 3:0 -r 3  -y  -s 720x480 -f image2 /home/mq/3/images%05d.jpeg
 * 参数解释:-i  输入文件   -y  覆盖  -f  生成图片格式  -s  图片宽高比  -r fps 设置帧率,也就是每秒截取图片的数量(默认25)  b-%3d.jpg 格式化文件命名,会生成 b-001.jpg，b-002.jpg 等。
 *--------------------------------------------------------------------*/
class qffmpeg_rtsp_image
{
public:
    qffmpeg_rtsp_image();

    void setRtspAddress(QString address);      //rtsp路径
    QString getRtspAddress();

    void setImageDirPath(QString imagePath);    //截图文件夹路径
    QString getImageDirPath();

    void setImageDealFrequency(int second);     //每秒保存的图片张数
    int  getImageDealFrequency();

    bool executeGetImages(int i,int screenshotSelection);                    //
    bool startGetImages(int i,int screenshotSelection);                      //开始截图
    void stopGetImages();                       //停止截图

private:
    QString m_rtspAddress;
    QString m_imageDirPath;
    int     m_secondOfImageDealFrequency;
    QProcess getImageProcess;
    QProcess getImageProcess2;
    int     ffmpegPID;
    int     ffmpegPID2;
    QString admin;                            //摄像头用户名
    QString passWD;                           //摄像头密码
    QString sleepTime;                        //睡眠时间

    ffmpegDecode *m_ffm;
};

#endif // QFFMPEG_RTSP_IMAGE_H
