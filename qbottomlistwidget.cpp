#include "qbottomlistwidget.h"

QBottomListWidget::QBottomListWidget(QWidget *parent) :
    QListWidget(parent)
{
    m_vectorStr.clear();

    connect(this,SIGNAL(bottomLogEvent(int)),this,SLOT(dealLogEvent(int)));
}

QBottomListWidget::~QBottomListWidget()
{

}

void QBottomListWidget::AddLog(QString strLog,int MessageLevel)
{
    m_mutex.lock();
    m_vectorStr.push_back(strLog);
    m_mutex.unlock();

    emit bottomLogEvent(MessageLevel);
}

void QBottomListWidget::dealLogEvent(int MessageLevel)
{
    int nItemCount = this->count();

    //最多显示500 条
    if(nItemCount>1000)
    {
        this->clear();
    }

    m_mutex.lock();
    if(m_vectorStr.isEmpty())
    {
        m_mutex.unlock();
        return;
    }
    m_mutex.unlock();

    m_mutex.lock();
    QString strLog = m_vectorStr.at(0);
    m_vectorStr.remove(0);
    m_mutex.unlock();

    QDateTime tm = QDateTime::currentDateTime();
    QString strTime = tm.toString("yyyy-MM-dd hh:mm:ss");

    QString str;
    str.sprintf("%03d", nItemCount);
    str += " ";
    str += strTime;
    str += " ";
    str += strLog;
//    this->addItem(str);
    QListWidgetItem* pListWidgetItem = new QListWidgetItem(this);
    pListWidgetItem->setText(str);

    //错误的话字体标红
    if(MessageLevel == ERRORINFO)
    {
        pListWidgetItem->setForeground(QBrush(QColor(255,0,0)));
    }
    else if(MessageLevel == NORMALINFO)
    {
        pListWidgetItem->setForeground(QBrush(QColor(0,0,255)));
    }
    else
    {
        //不可能出现黑色字体的情况，除非用错
        pListWidgetItem->setForeground(QBrush(QColor(0,0,0)));
    }

    this->scrollToBottom();
}



