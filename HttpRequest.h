#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H
/***************************************************************************
 * //http基础类
 * ************************************************************************/
#include <QObject>
#include <QEventLoop>
#include "qmyglobal.h"
#include <QNetworkCookie>
#define TIMEOUT 100000

class QNetworkReply;
class QNetworkAccessManager;
class QTimer;
class HttpRequest : public QObject
{
	Q_OBJECT
public:
	HttpRequest(QObject *parent = 0);
	~HttpRequest();
	QByteArray HttpSynGet(const QString&  url);
    QByteArray HttpSynGetLocation(const QString&  url,QString ua, QString referer);
    QString HttpSynPost(const QString&  url, const QString& data, QString & sessionID,QString &status_code);
    QString HttpSynMulitPart(const QString&  url,QString fileName ,QString alarm_id,QString alarm_rule_id, QString & sessionID,QString &status_code);
    QString HttpSynMulitPartJPG(const QString&  url, const mAlarmRecord_stu &data, QString & sessionID,QString &status_code);



private Q_SLOTS:
	void slotHttpFinished();
	void slotTimeOut();

private:
	QNetworkReply *mpReply;
	QNetworkAccessManager *mpNAMgr;
	QEventLoop mEventLoop;
	QTimer *mpTimer;

};
#endif
