#ifndef GLOBALDATA_H
#define GLOBALDATA_H
#include <vector>
#include <map>
using namespace std;

#include <QTime>
#include <qstring.h>
#include "param.h"

#include <qmutex.h>

struct mIPC
{
    QString AreaId;         //所属Area关联编号
    QString IPCId;         //IPC编号
    QString IPCName;   //IPC名称
    QString IPCAdress; //IPC地址
    QString UserName;  //用户名
    QString PassWD;    //密码
    QString sourcePicturePath; //流的截图文件夹
    QString alarmPicturePath;  //报警图片文件夹

    int channel_id;
    int num_id;
    QString stay_time;
    int minutes;
    QTime start_time;
    QTime end_time;
    //int IPCstate;
};
typedef vector<QString> CameraVector;//cameraID
typedef vector<mIPC> IPCVector;
typedef map<int, IPCVector> IPCMap;//key1:channelID

class ffmpegDecode;
class imageAnalyse;
struct tagCameraArg
{
    ffmpegDecode* pFfm;
    imageAnalyse *pAnalyse;
    QString ipcid;
    QString rtsp;
};
typedef vector<tagCameraArg> CameraArgVector;
typedef map<int, CameraArgVector> CameraArgMap;
typedef map<int, int> ArgIndexMap;//channelID+index
typedef vector<int> ChannelVector;//用户caffe循环

class GlobalData
{
public:
    static GlobalData* Instance();

private:
    GlobalData();

public:

    //相机id
    void SaveCamera(const QString &cameraID);
    CameraVector& GetCamera();
    bool hasCamera(const QString &cameraID);

    //通道相机IPC
    void SaveChannel(int channelID, mIPC ipc);
    void ClearChannel();
    IPCMap GetChannel();
    QString getCameraRTSP(const QString &cameraID);

    //通道相机arg
    void SaveCameraArg(int channelID, const tagCameraArg &cameraArg);
    bool GetCameraArg(int channelID, tagCameraArg &cameraArg);
    void SetCameraArgIndex(int channleID, int index);

private:
    CameraVector m_vecCamera;
    IPCMap m_ipcs;

    CameraArgMap m_cameraArgs;
    ArgIndexMap m_argIndexs;

    QMutex m_argIndexMutex;
};
#define GLOBAL_DATA GlobalData::Instance()

#endif // GLOBALDATA_H
