#include "cameraserver.h"
#include "qmyglobal.h"

CameraServer::CameraServer(int channelID, const vector<mIPC> &vecIPC)
{
    m_channelID=channelID;
    m_vecIPC=vecIPC;
    m_pFfm=NULL;
}

void CameraServer::run()
{
    if(!init())
    {
        return;
    }

    QTime starttime=QTime::currentTime();
    while(true)
    {
        if(m_pFfm!=NULL)
        {
            if(!m_pFfm->readOneFrame())
            {
                ffm_init();
            }
        }
              
        //60s轮询一次
        QTime nowtime=QTime::currentTime();
        if(starttime.addSecs(60) < nowtime)
        {
            starttime=nowtime;
            loop(nowtime);
        }
    }
;
}

bool CameraServer::init()
{
    QTime curtime = QTime::currentTime();
    for(size_t i=0; i<m_vecIPC.size(); i++)
    {
        if(m_vecIPC[i].minutes == 0)
        {
            QTime start_time = m_vecIPC[i].start_time;
            QTime end_time = m_vecIPC[i].end_time;

            if(curtime>start_time && curtime<end_time)
            {
                switchCamera(i, curtime);
                return true;
            }
        }
        else
        {
            switchCamera(i, curtime);
            return true;
        }
    }

    return false;
}

void CameraServer::loop(const QTime &curtime)
{   
    int index=0;
    for(size_t i=0; i<m_vecIPC.size(); i++)
    {
        index = m_index+i;
        if(index==m_vecIPC.size())
        {
            index=0;
        }

        int minutes = m_vecIPC[index].minutes;

        if(minutes>0)
        {
            if(i==0)
            {
                if(m_startTime.addSecs(minutes*60) > curtime) break;
            }
            else
            {
                switchCamera(index, curtime);
                break;
            }
        }
        else
        {
            QTime start_time = m_vecIPC[index].start_time;
            QTime end_time = m_vecIPC[index].end_time;

            if(curtime>start_time && curtime<end_time)
            {
                if(i!=0)
                {
                    switchCamera(index, curtime);
                }
                break;
            }

        }
    }

}

void CameraServer::switchCamera(int index, const QTime &curtime)
{
    GLOBAL_DATA->SetCameraArgIndex(m_channelID, index);

    tagCameraArg cameraArg;
    if(GLOBAL_DATA->GetCameraArg(m_channelID, cameraArg))
    {
        m_pFfm = cameraArg.pFfm;
        m_rtsp = cameraArg.rtsp;

        m_index=index;

        ffm_init();

        m_startTime=curtime;
    }
    else
    {
       GLOBAL_DATA->SetCameraArgIndex(m_channelID, m_index);
    }
}

void CameraServer::ffm_init()
{
    int conncount = 0;
    int base = 2000;
    while(m_pFfm!=NULL && !m_pFfm->init(m_rtsp))
    {
        if(base>2000*1800)
            base==2000*1800;
        else
            base = base*2;

        LOG_WARN("rtsp:%s, ffm_init() error count:%d.\n", m_rtsp.toStdString().c_str(), ++conncount);
        msleep(base);
    }
}
