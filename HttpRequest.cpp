#include  "HttpRequest.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include "qmyglobal.h"
#include <QByteArray>
#include <QHttpPart>
HttpRequest::HttpRequest(QObject *parent)
    : QObject(parent)
    , mpReply(nullptr)
    , mpNAMgr(new QNetworkAccessManager(this))
    , mpTimer(new QTimer(this))
{
    connect(mpTimer, &QTimer::timeout, this, &HttpRequest::slotTimeOut);
}

HttpRequest::~HttpRequest()
{
    if (mpReply)
    {
        mpReply->abort();
        mpReply->deleteLater();
        mpReply = nullptr;
    }
    if (mpNAMgr)
    {
        mpNAMgr->deleteLater();
        mpNAMgr = nullptr;
    }
}

QByteArray HttpRequest::HttpSynGet(const QString&  url)
{

    QByteArray result;
    mpReply = mpNAMgr->get(QNetworkRequest(url));
    connect(mpReply, &QNetworkReply::finished, this, &HttpRequest::slotHttpFinished);
    mpTimer->start(TIMEOUT);
    mEventLoop.exec();
    if (mpReply->error() == QNetworkReply::NoError)
    {
        result = mpReply->readAll();
        mpReply->abort();
        mpReply->deleteLater();
        mpReply = nullptr;
        qDebug() <<result;
    }
    else
    {
        mpReply->deleteLater();
        qDebug()<<"error"<< mpReply->errorString();

    }
    return result;
}

QByteArray HttpRequest::HttpSynGetLocation(const QString&  url,QString ua, QString referer)
{
    qDebug() << url;
    QByteArray result;
    QNetworkRequest  req;
    req.setUrl(url);
    req.setRawHeader("Connection", "keep-alive");
    req.setRawHeader("User-Agent", ua.toLatin1());
    if(referer!="0")
    {
        req.setRawHeader("Referer", referer.toLatin1());
    }
    req.setRawHeader("Accept-Encoding", "gzip,deflate");
    req.setRawHeader("Accept-Language", "zh-CN,zh;q=0.8");
    req.setRawHeader("Upgrade-In", "zh-CN,zh;q=0.8");

    mpReply = mpNAMgr->get(req);
    connect(mpReply, &QNetworkReply::finished, this, &HttpRequest::slotHttpFinished);
    mpTimer->start(TIMEOUT);
    mEventLoop.exec();
    if (mpReply->error() == QNetworkReply::NoError)
    {
        result = mpReply->rawHeader("Location");
        mpReply->abort();
        mpReply->deleteLater();
        mpReply = nullptr;
        qDebug() <<"get"<< result;
    }
    else
    {
        mpReply->deleteLater();
        qDebug()<<"error"<< mpReply->errorString();

    }
    return result;
}

QString HttpRequest::HttpSynPost(const QString&  url, const QString& data, QString & sessionID,QString &status_code)
{
    QString result;
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    if(!sessionID.isEmpty())
        request.setRawHeader("Cookie",sessionID.toAscii());
    request.setUrl(url);
    qDebug()<<url;

    mpReply = mpNAMgr->post(request, data.toLocal8Bit());
    connect(mpReply, &QNetworkReply::finished, this, &HttpRequest::slotHttpFinished);
    mpTimer->start(TIMEOUT);
    mEventLoop.exec();
    //取状态码，302或200放行
    status_code=mpReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString();
    //取cookie
    QVariant variantCookies=mpReply->header(QNetworkRequest::SetCookieHeader);
    QList<QNetworkCookie> cookies=qvariant_cast<QList<QNetworkCookie> >(variantCookies);
    QByteArray dataSession;
    for(int i=0;i<cookies.size();i++)
    {
        QNetworkCookie cookie=cookies[i];
        if(cookie.isSessionCookie())
        {
            dataSession.append(cookie.toRawForm());
            break;
        }
    }
    QStringList sessionIDList=QString(dataSession).split(";");
    for(int i=0;i<sessionIDList.size();i++)
    {
        if(sessionIDList[i].contains("JSESSIONID"))
        {
            sessionID=sessionIDList[i];
            break;
        }
    }
    //判断登陆是否超时，自定义设置状态码为999，在调用接口判断

    QString location=QString( mpReply->rawHeader("Location"));
    if(location.contains("login"))
    {
        status_code="999";
        return "";
    }

    if (mpReply->error() == QNetworkReply::NoError)
    {
        result = mpReply->readAll();

        mpReply->abort();
        mpReply = nullptr;
        //qDebug()<<result;
    }
    else
    {
        qDebug()<<"error"<< mpReply->errorString();

    }
    mpReply->deleteLater();
    return result;
}

QString HttpRequest::HttpSynMulitPartJPG(const QString&  url, const mAlarmRecord_stu &data, QString & sessionID,QString &status_code)
{
    QString result;
    QFile file(data.filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        file.close();
        return false;
    }

    QString rule_id = data.rule_id;

    QString crlf="\r\n";
    qsrand(QDateTime::currentDateTime().toTime_t());
    QString b=QVariant(qrand()).toString()+QVariant(qrand()).toString()+QVariant(qrand()).toString();
    QString boundary=b;
    QString endBoundary=crlf+"--"+boundary+"--"+crlf;
    QString contentType="multipart/form-data; boundary="+boundary;
    boundary="--"+boundary+crlf;
    QByteArray bond=boundary.toAscii();
    QByteArray send;

    send.append(bond);
    boundary = crlf + boundary;
    bond = boundary.toAscii();
    send.append(QString("Content-Disposition: form-data; name=\"org_no\""+crlf).toAscii());
    send.append(QString("Content-Transfer-Encoding: 8bit"+crlf).toAscii());
    send.append(crlf.toAscii());
    send.append(QString("3").toUtf8());

    send.append(bond);
    send.append(QString("Content-Disposition: form-data; name=\"access_key\""+crlf).toAscii());
    send.append(QString("Content-Transfer-Encoding: 8bit"+crlf).toAscii());
    send.append(crlf.toAscii());
    send.append(QString("").toUtf8());

    send.append(bond);
    send.append(QString("Content-Disposition: form-data; name=\"time\""+crlf).toAscii());
    send.append(QString("Content-Transfer-Encoding: 8bit"+crlf).toAscii());
    send.append(crlf.toAscii());
    send.append(data.alarmtime.toUtf8());

    send.append(bond);
    send.append(QString("Content-Disposition: form-data; name=\"alarm_rule_id\""+crlf).toAscii());
    send.append(QString("Content-Transfer-Encoding: 8bit"+crlf).toAscii());
    send.append(crlf.toAscii());
    send.append(rule_id.toUtf8());

    send.append(bond);
    send.append(QString("Content-Disposition: form-data; name=\"alarm_level\""+crlf).toAscii());
    send.append(QString("Content-Transfer-Encoding: 8bit"+crlf).toAscii());
    send.append(crlf.toAscii());
    send.append(QString("%1").arg(data.alarmLevel).toUtf8());

    send.append(bond);
    send.append(QString("Content-Disposition: form-data; name=\"head_points\""+crlf).toAscii());
    send.append(QString("Content-Transfer-Encoding: 8bit"+crlf).toAscii());
    send.append(crlf.toAscii());
    send.append(QString("%1").arg(data.head_points).toUtf8());

    send.append(bond);
    QString uploadName=data.filePath;
    uploadName.replace(" ","");
    uploadName.replace(":","");
    uploadName=QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    send.append(QString("Content-Disposition: form-data; name=\"JPGFile\"; filename=\""+uploadName+".jpg\""+crlf).toAscii());
    send.append(QString("Content-Transfer-Encoding: 8bit"+crlf).toAscii());
    send.append(crlf.toAscii());
    send.append(file.readAll());

    send.append(endBoundary.toAscii());

    file.close();

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, contentType.toAscii());
    request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(send.size()).toString());
    if(!sessionID.isEmpty())
        request.setRawHeader("Cookie",sessionID.toAscii());
    request.setUrl(url);
    qDebug()<<url;

    mpReply = mpNAMgr->post(request, send);
    connect(mpReply, &QNetworkReply::finished, this, &HttpRequest::slotHttpFinished);
    mpTimer->start(TIMEOUT);
    mEventLoop.exec();
    //取状态码，302或200放行
    status_code=mpReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString();
    //取cookie
    QVariant variantCookies=mpReply->header(QNetworkRequest::SetCookieHeader);
    QList<QNetworkCookie> cookies=qvariant_cast<QList<QNetworkCookie> >(variantCookies);
    QByteArray dataSession;
    for(int i=0;i<cookies.size();i++)
    {
        QNetworkCookie cookie=cookies[i];
        if(cookie.isSessionCookie())
        {
            dataSession.append(cookie.toRawForm());
            break;
        }
    }
    QStringList sessionIDList=QString(dataSession).split(";");
    for(int i=0;i<sessionIDList.size();i++)
    {
        if(sessionIDList[i].contains("JSESSIONID"))
        {
            sessionID=sessionIDList[i];
            break;
        }
    }
    //判断登陆是否超时，自定义设置状态码为999，在调用接口判断

    QString location=QString( mpReply->rawHeader("Location"));
    if(location.contains("login"))
    {
        status_code="999";
        return "";
    }

    if (mpReply->error() == QNetworkReply::NoError)
    {
        result = mpReply->readAll();

        mpReply->abort();
        mpReply = nullptr;
        qDebug()<<result;
    }
    else
    {
        qDebug()<<"error:               "<< mpReply->errorString();

    }
    mpReply->deleteLater();
    return result;
}


QString HttpRequest::HttpSynMulitPart(const QString&  url,QString fileName ,QString alarm_id,QString alarm_rule_id, QString & sessionID,QString &status_code)
{
    QString result;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        file.close();
        return false;
    }

    QString crlf="\r\n";
    qsrand(QDateTime::currentDateTime().toTime_t());
    QString b=QVariant(qrand()).toString()+QVariant(qrand()).toString()+QVariant(qrand()).toString();
    QString boundary=b;
    QString endBoundary=crlf+"--"+boundary+"--"+crlf;
    QString contentType="multipart/form-data; boundary="+boundary;
    boundary="--"+boundary+crlf;
    QByteArray bond=boundary.toAscii();
    QByteArray send;

    send.append(bond);
    boundary = crlf + boundary;
    bond = boundary.toAscii();

    send.append(QString("Content-Disposition: form-data; name=\"name\""+crlf).toAscii());
    send.append(QString("Content-Transfer-Encoding: 8bit"+crlf).toAscii());
    send.append(crlf.toAscii());
    send.append(QString("mp4").toUtf8());


    send.append(bond);
    send.append(QString("Content-Disposition: form-data; name=\"alarm_id\""+crlf).toAscii());
    send.append(QString("Content-Transfer-Encoding: 8bit"+crlf).toAscii());
    send.append(crlf.toAscii());
    send.append(QString("%1").arg(alarm_id).toUtf8());

    send.append(bond);
    send.append(QString("Content-Disposition: form-data; name=\"alarm_rule_id\""+crlf).toAscii());
    send.append(QString("Content-Transfer-Encoding: 8bit"+crlf).toAscii());
    send.append(crlf.toAscii());
    send.append(QString("%1").arg(alarm_rule_id).toUtf8());

    send.append(bond);
    QString uploadName=fileName;
    uploadName.replace(" ","");
    uploadName.replace(":","");
    uploadName=QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    send.append(QString("Content-Disposition: form-data; name=\"MP4File\"; filename=\""+uploadName+".mp4\""+crlf).toAscii());
    send.append(QString("Content-Transfer-Encoding: 8bit"+crlf).toAscii());
    send.append(crlf.toAscii());
    send.append(file.readAll());

    send.append(endBoundary.toAscii());

    file.close();

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, contentType.toAscii());
    request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(send.size()).toString());
    if(!sessionID.isEmpty())
        request.setRawHeader("Cookie",sessionID.toAscii());
    request.setUrl(url);
    qDebug()<<url;

    mpReply = mpNAMgr->post(request, send);
    connect(mpReply, &QNetworkReply::finished, this, &HttpRequest::slotHttpFinished);
    mpTimer->start(TIMEOUT);
    mEventLoop.exec();
    //取状态码，302或200放行,
    status_code=mpReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString();
    //取cookie
    QVariant variantCookies=mpReply->header(QNetworkRequest::SetCookieHeader);
    QList<QNetworkCookie> cookies=qvariant_cast<QList<QNetworkCookie> >(variantCookies);
    QByteArray dataSession;
    for(int i=0;i<cookies.size();i++)
    {
        QNetworkCookie cookie=cookies[i];
        if(cookie.isSessionCookie())
        {
            dataSession.append(cookie.toRawForm());
            break;
        }
    }
    QStringList sessionIDList=QString(dataSession).split(";");
    for(int i=0;i<sessionIDList.size();i++)
    {
        if(sessionIDList[i].contains("JSESSIONID"))
        {
            sessionID=sessionIDList[i];
            break;
        }
    }
    //判断登陆是否超时，自定义设置状态码为999，在调用接口判断

    QString location=QString( mpReply->rawHeader("Location"));
    if(location.contains("login"))
    {
        status_code="999";
        return "";
    }

    if (mpReply->error() == QNetworkReply::NoError)
    {
        result = mpReply->readAll();

        mpReply->abort();
        mpReply = nullptr;
        qDebug()<<result;
    }
    else
    {
        qDebug()<<"error"<< mpReply->errorString();

    }
    mpReply->deleteLater();
    return result;
}



void  HttpRequest::slotHttpFinished()
{
    mpTimer->stop();
    mEventLoop.quit();
    qDebug() << __FUNCTION__ << __LINE__ << "finished\n";
}

void HttpRequest::slotTimeOut()
{
    mpTimer->stop();
    mEventLoop.quit();
    qDebug() <<  __FUNCTION__ << __LINE__ << "timeout\n";
}
