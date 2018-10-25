#include "imageserver.h"
#include "imagedata.h"
#include "param.h"
#include "Logger.h"
#include <sstream>
using namespace std;

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

NewConnect::NewConnect(socket_t sock)
{
    m_sock=sock;
}

void NewConnect::run()
{
    LOG_INFO("newconnect run.\n");

    string cameraID = "";
    firstRecv(cameraID);

    IMAGEDATA_INSTANCE->SetCameraID(QString::fromStdString(cameraID));
    secondSend(cameraID);

    close(m_sock);
    IMAGEDATA_INSTANCE->SetCameraID(0);
}

void NewConnect::firstRecv(string &cameraID)
{ 
    int recvLen = 0;
    char *recvbuff = new char[CAMERA_ID_LENGTH];
    while (recvLen<CAMERA_ID_LENGTH)
    {
        int ret = recv(m_sock, recvbuff+recvLen, CAMERA_ID_LENGTH-recvLen, 0);
        if (ret>0)
        {
            recvLen += ret;
        }
        else if (ret<=0)
        {
            break;
        }
    }

    cameraID = string(recvbuff);

    LOG_INFO("recv cameraid lenth: %d.\n", recvLen);
}

void NewConnect::secondSend(const string &cameraID)
{
    char* lengthBuff = new char[BUFF_LENGTH];

    char* pBuff=NULL;
    int size=0;
    while (true)
    {
        if(!IMAGEDATA_INSTANCE->PopCaffeImage(&pBuff, size))
        {
            msleep(30);
            continue;
        }

        //发送大小
        memset(lengthBuff, 0, BUFF_LENGTH);
        memcpy(lengthBuff, &size, BUFF_LENGTH);

        int sendLen = 0;
        int ret = 0;
        while (sendLen < BUFF_LENGTH)
        {
            ret = send(m_sock, lengthBuff, BUFF_LENGTH, 0);

            if (ret>0)
            {
                sendLen += ret;
            }
            else if (ret==SOCKET_ERROR)
            {
                LOG_ERROR("send size length disconnect.\n");
                return;
            }
        }
        stringstream ss;
        ss<<"send size lenth:"<<sendLen;
        LOG_INFO("send size lenth: %d.\n", sendLen);

        //发送bufff
        sendLen = 0;
        while (sendLen<size)
        {
            ret = send(m_sock, pBuff+sendLen, size-sendLen, 0);

            if (ret > 0)
            {
                sendLen += ret;
                cout << "send length:" << sendLen << endl;
            }
            else if (ret == SOCKET_ERROR)
            {
                LOG_ERROR("send buff length disconnect.\n");
                return;
            }
        }

        LOG_INFO("send buff lenth: %d.\n", sendLen);

        IMAGEDATA_INSTANCE->PushDataPool(pBuff);
    }
}


ImageServer::ImageServer()
{
    m_skServer=INVALID_SOCKET;
    init();
}

void ImageServer::run()
{
    if(m_skServer==INVALID_SOCKET)
    {
        cout<<"imageserver invalid socket.\n"<<endl;
    }

    //4.accept
    sockaddr_in addrClient;
    socklen_t length = sizeof(addrClient);
    while (true)
    {
        LOG_INFO("start accept...\n");
        socket_t skClient = accept(m_skServer, (sockaddr*)&addrClient, &length);
        if (skClient == INVALID_SOCKET)
        {
            LOG_ERROR("accept error.\n");
            continue;
        }

        LOG_INFO( "clientsock: %d addr: %s.\n", skClient,  inet_ntoa(addrClient.sin_addr));

        //
        NewConnect *pConn = new NewConnect(skClient);
        pConn->start();
    }

    close(m_skServer);
}

void ImageServer::init()
{
    //1.socket
    m_skServer = socket(AF_INET, SOCK_STREAM, 0);
    if (m_skServer == INVALID_SOCKET)
    {
        LOG_ERROR("socket error.\n");
        return;
    }

    //2.bind
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(m_skServer, (sockaddr*)&addr, sizeof(sockaddr));
    if (ret != 0)
    {
        LOG_ERROR("bind error.\n");
        return;
    }

    //3.listen
    ret = listen(m_skServer, 5);
    if (ret != 0)
    {
        LOG_ERROR("listen error.\n");
        return;
    }
}
