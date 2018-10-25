#include "GlobalData.h"
#include "Logger.h"

GlobalData* GlobalData::Instance()
{
    static GlobalData s;
    return &s;
}

GlobalData::GlobalData()
{

}


void GlobalData::SaveCamera(const QString &cameraID)
{
    m_vecCamera.push_back(cameraID);
}

CameraVector& GlobalData::GetCamera()
{
    return m_vecCamera;
}

bool GlobalData::hasCamera(const QString &cameraID)
{
    for(size_t i=0; i<m_vecCamera.size(); i++)
    {
        if(m_vecCamera[i].compare(cameraID)==0)
            return true;
    }

    return false;
}

void GlobalData::SaveChannel(int channelID, mIPC ipc)
{
    IPCMap::iterator it = m_ipcs.find(channelID);
    if(it!=m_ipcs.end())
    {
        m_ipcs[channelID].push_back(ipc);
    }
    else if(m_ipcs.size() < CHANNEL_COUNT)
    {
        vector<mIPC> vecIPC;
        vecIPC.push_back(ipc);

        m_ipcs[channelID]=vecIPC;
    }
}

void GlobalData::ClearChannel()
{
    m_ipcs.clear();
}

IPCMap GlobalData::GetChannel()
{
    return m_ipcs;
}

QString GlobalData::getCameraRTSP(const QString &cameraID)
{
    for(IPCMap::iterator it = m_ipcs.begin(); it!=m_ipcs.end(); ++it)
    {
        vector<mIPC> &vecIPC = it->second;
        for(size_t i=0; i<vecIPC.size(); i++)
        {
            if(vecIPC[i].IPCId.compare(cameraID)==0)
            {
                return vecIPC[i].IPCAdress;
            }
        }
    }

    return "";
}


void GlobalData::SaveCameraArg(int channelID, const tagCameraArg &cameraArg)
{
    m_argIndexMutex.lock();
    CameraArgMap::iterator it  = m_cameraArgs.find(channelID);
    if(it!=m_cameraArgs.end())
    {
        it->second.push_back(cameraArg);
    }
    else
    {
        CameraArgVector vecCameraArg;
        vecCameraArg.push_back(cameraArg);
        m_cameraArgs[channelID]=vecCameraArg;
    }
    m_argIndexMutex.unlock();
}

bool GlobalData::GetCameraArg(int channelID, tagCameraArg &cameraArg)
{
    m_argIndexMutex.lock();
    bool ret=false;
    int index=0;
    CameraArgMap::iterator it = m_cameraArgs.find(channelID);
    ArgIndexMap::iterator itindex = m_argIndexs.find(channelID);
    if(it != m_cameraArgs.end() && itindex != m_argIndexs.end())
    {
        index = itindex->second;
        if(it->second.size() > index)
        {
            cameraArg = it->second[index];
            ret=true;
        }
    }
    m_argIndexMutex.unlock();

    return ret;
}

void GlobalData::SetCameraArgIndex(int channleID, int index)
{
    m_argIndexMutex.lock();
    m_argIndexs[channleID]=index;
    m_argIndexMutex.unlock();
}

