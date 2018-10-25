#ifndef QLOGFILE_H
#define QLOGFILE_H

#include <QDir>
#include <QFile>
#include <QMutex>
#include <QDateTime>
#include <QTextStream>

class QLogFile: public QFile
{
    Q_OBJECT
public:
    QLogFile();
    ~QLogFile();

public:

    void InitLogFile(QString strPathName);
    void SaveLogFile(QString strLog,int MessageLevel);

signals:
    void logToWidgetEvent(QString,int);

private:
    QString m_strPathName;
    QMutex m_mutex;
};

#endif // QLOGFILE_H
