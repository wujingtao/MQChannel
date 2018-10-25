#ifndef IMAGESERVER_H
#define IMAGESERVER_H
#include <qthread.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>

#include <string>
#include <iostream>
using namespace std;

typedef int socket_t;

class NewConnect : public QThread
{
public:
    NewConnect(socket_t sock);

public:
    void run();

    void firstRecv(string &cameraID);
    void secondSend(const string &cameraID);

private:
    socket_t m_sock;
};

class ImageServer : public QThread
{
public:
    ImageServer();

public:
    void run();

private:
    void init();

    socket_t m_skServer;
};

#endif // IMAGESERVER_H
