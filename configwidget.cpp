      #include "configwidget.h"
#include "qjudgeposition.h"
#include <QLabel>
#include <QTreeWidget>
#include <QHeaderView>
#include "qmyglobal.h"
#include <QMessageBox>
#include "qdrawline.h"
#include "qtvariantproperty.h"
#include <QPixmap>
#include "qttreepropertybrowser.h"
#include <QDockWidget>
#include <QDebug>
#include <QDir>
#include "datatranslate.h"
#include "qffmpeg_rtsp_image.h"
//#include "windows.h"
#include <QApplication>
#include <QMovie>
#include"videowidget.h"
using namespace cv;
using namespace std;

#include "AlarmManager.h"
#include "GlobalData.h"



ConfigWidget::ConfigWidget(QWidget *parent) :
        QWidget(parent)
{
    initWindow();
    isDraw=false;
    ishighlight=false;
    isSaveLock=false;
    pointsKey="0";
    currentPointKey="0";
    buttonCreateRule->setEnabled(false);
    buttonStartDraw->setEnabled(false);
    buttonRestLast->setEnabled(false);
    buttonRedo->setEnabled(false);
    buttonEndDraw->setEnabled(false);
    buttonEditCurrentRule->setEnabled(false);
    buttonClearCurrentRule->setEnabled(false);

    m_isGetImage=false;
    m_pFfm = new ffmpegDecode(PIC_WIDTH, PIC_HEIGHT);
}

//初始化窗口元素
void ConfigWidget::initWindow()
{
    setMaximumSize(screenX,screenY*0.91);
    resize(screenX,screenY*0.91);

    location=this->geometry();
    leftWindowWidth=location.width()*0.16;
    middleWindowWidth=location.width()*0.64;
    rightWindowWidth=location.width()*0.2;
    middleUpWindowheight=middleWindowWidth/20*1;
    middleDownWindowheight=middleWindowWidth/20*18;
    windowWidth=location.width();
    windowHeight=location.height();
    initCameraTree();
    initPainTool();
    initPictureMap();
    initProperWindow();
    setAttributeVisibelFalse();

}

//初始化树形列表
void ConfigWidget::initCameraTree()
{

    refreshTreeAndConfig=new QPushButton(this);
    refreshTreeAndConfig->setFixedSize(screenX*0.09143,screenY*0.04);
    refreshTreeAndConfig->setText("同步摄像头信息");
    refreshTreeAndConfig->setObjectName("refreshTreeAndConfig");
    refreshTreeAndConfig->setStyleSheet("#refreshTreeAndConfig{border-image: url(:/image/button2.png);}"
                               "#refreshTreeAndConfig:hover{border-image: url(:/image/button.png);}"
                               "#refreshTreeAndConfig:pressed{border-image: url(:/image/button3.png);}");
    refreshTreeAndConfig->setGeometry(5,5,screenX*0.08,screenY*0.04);
    //--------------------------------------------摄像头树
    videoTreewidget=new QTreeWidget(this);
    videoTreewidget->setFixedSize(screenX*0.157,screenY*0.91);
    videoTreewidget->setWindowTitle("摄像头列表");
    videoTreewidget->header()->setVisible(false);
    videoTreewidget->setStyleSheet("background:transparent; font-size:17px; color: white;");
    videoTreewidget->setWindowFlags(Qt::FramelessWindowHint);
    QHeaderView *head=videoTreewidget->header();
    head->setSectionResizeMode(QHeaderView::ResizeToContents );
    videoTreewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    videoTreewidget->setGeometry(0,screenY*0.04,screenX*0.157,screenY*0.91);
    connect(refreshTreeAndConfig,SIGNAL(clicked()),this,SLOT(loadCameraTree()));
    connect(videoTreewidget,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(on_videoTreewidget_doubleClicked(QModelIndex)));
}

//初始化画图控件
void ConfigWidget::initPainTool()
{
    buttonCreateRule=new QPushButton(this);
    buttonCreateRule->setFixedSize(screenX*0.09143,screenY*0.04);
    buttonCreateRule->setText("新建规则");
    buttonCreateRule->setGeometry(leftWindowWidth,windowHeight*0.01,screenX*0.09143,screenY*0.04);
    buttonCreateRule->setObjectName("buttonCreateRule");
    buttonCreateRule->setStyleSheet("#buttonCreateRule{border-image: url(:/image/button.png);font-size:17px; color: black;}"
                               "#buttonCreateRule:hover{border-image: url(:/image/button2.png);}"
                               "#buttonCreateRule:pressed{border-image: url(:/image/button3.png);}");

    buttonStartDraw=new QPushButton(this);
    buttonStartDraw->setFixedSize(screenX*0.09143,screenY*0.04);
    buttonStartDraw->setText("开始画线");
    buttonStartDraw->setGeometry(leftWindowWidth+buttonCreateRule->width(),windowHeight*0.01,screenX*0.09143,screenY*0.04);
    buttonStartDraw->setObjectName("buttonStartDraw");
    buttonStartDraw->setStyleSheet("#buttonStartDraw{border-image: url(:/image/button.png);font-size:17px; color: black;}"
                               "#buttonStartDraw:hover{border-image: url(:/image/button2.png);}"
                               "#buttonStartDraw:pressed{border-image: url(:/image/button3.png);}");


    buttonRestLast=new QPushButton(this);
    buttonRestLast->setFixedSize(screenX*0.09143,screenY*0.04);
    buttonRestLast->setText("上一步");
    buttonRestLast->setGeometry(leftWindowWidth+buttonCreateRule->width()+buttonStartDraw->width(),windowHeight*0.01,screenX*0.09143,screenY*0.04);
    buttonRestLast->setObjectName("buttonRestLast");
    buttonRestLast->setStyleSheet("#buttonRestLast{border-image: url(:/image/button.png);font-size:17px; color: black;}"
                               "#buttonRestLast:hover{border-image: url(:/image/button2.png);}"
                               "#buttonRestLast:pressed{border-image: url(:/image/button3.png);}");

    buttonRedo=new QPushButton(this);
    buttonRedo->setFixedSize(screenX*0.09143,screenY*0.04);
    buttonRedo->setText("下一步");
    buttonRedo->setGeometry(leftWindowWidth+buttonCreateRule->width()+buttonStartDraw->width()+buttonRestLast->width(),windowHeight*0.01,screenX*0.09143,screenY*0.04);
    buttonRedo->setObjectName("buttonRedo");
    buttonRedo->setStyleSheet("#buttonRedo{border-image: url(:/image/button.png);font-size:17px; color: black;}"
                               "#buttonRedo:hover{border-image: url(:/image/button2.png);}"
                               "#buttonRedo:pressed{border-image: url(:/image/button3.png);}");

    buttonEndDraw=new QPushButton(this);
    buttonEndDraw->setFixedSize(screenX*0.09143,screenY*0.04);
    buttonEndDraw->setText("保存");
    buttonEndDraw->setGeometry(leftWindowWidth+buttonCreateRule->width()+buttonStartDraw->width()+buttonRestLast->width()+buttonRedo->width(),windowHeight*0.01,screenX*0.09143,screenY*0.04);
    buttonEndDraw->setObjectName("buttonEndDraw");
    buttonEndDraw->setStyleSheet("#buttonEndDraw{border-image: url(:/image/button.png);font-size:17px; color: black;}"
                               "#buttonEndDraw:hover{border-image: url(:/image/button2.png);}"
                               "#buttonEndDraw:pressed{border-image: url(:/image/button3.png);}");

    buttonEditCurrentRule=new QPushButton(this);
    buttonEditCurrentRule->setFixedSize(screenX*0.09143,screenY*0.04);
    buttonEditCurrentRule->setText("编辑选中");
    buttonEditCurrentRule->setGeometry(leftWindowWidth+buttonCreateRule->width()+buttonStartDraw->width()+buttonRestLast->width()+buttonRedo->width()+buttonEndDraw->width(),windowHeight*0.01,screenX*0.09143,screenY*0.04);
    buttonEditCurrentRule->setObjectName("buttonEditCurrentRule");
    buttonEditCurrentRule->setStyleSheet("#buttonEditCurrentRule{border-image: url(:/image/button.png);font-size:17px; color: black;}"
                               "#buttonEditCurrentRule:hover{border-image: url(:/image/button2.png);}"
                               "#buttonEditCurrentRule:pressed{border-image: url(:/image/button3.png);}");

    buttonClearCurrentRule=new QPushButton(this);
    buttonClearCurrentRule->setFixedSize(screenX*0.09143,screenY*0.04);
    buttonClearCurrentRule->setText("删除选中");
    buttonClearCurrentRule->setGeometry(leftWindowWidth+buttonCreateRule->width()+buttonStartDraw->width()+buttonRestLast->width()+buttonRedo->width()+buttonEditCurrentRule->width()+buttonEndDraw->width(),windowHeight*0.01,screenX*0.09143,screenY*0.04);
    buttonClearCurrentRule->setObjectName("buttonClearCurrentRule");
    buttonClearCurrentRule->setStyleSheet("#buttonClearCurrentRule{border-image: url(:/image/button.png);font-size:17px; color: black;}"
                               "#buttonClearCurrentRule:hover{border-image: url(:/image/button2.png);}"
                               "#buttonClearCurrentRule:pressed{border-image: url(:/image/button3.png);}");



    connect(buttonCreateRule,SIGNAL(clicked()),this,SLOT(slot_createNewRule()));
    connect(buttonStartDraw,SIGNAL(clicked()),this,SLOT(slot_begindraw()));
    connect(buttonRestLast,SIGNAL(clicked()),this,SLOT(slot_undolastline()));
    connect(buttonRedo,SIGNAL(clicked()),this,SLOT(slot_redolastline()));
    connect(buttonEditCurrentRule,SIGNAL(clicked()),this,SLOT(slot_buttonEditCurrentRule()));
    connect(buttonClearCurrentRule,SIGNAL(clicked()),this,SLOT(slot_buttonClearCurrentRule()));
    connect(buttonEndDraw,SIGNAL(clicked()),this,SLOT(slot_enddraw()));

}

//初始化图片展示窗口
void ConfigWidget::initPictureMap()
{
    labloading=new QLabel(this);
    labloading->setFixedSize(middleWindowWidth,middleDownWindowheight*0.82);
    labloading->setGeometry(leftWindowWidth,middleUpWindowheight,labloading->width(),labloading->height()*0.95);
    labloading->setVisible(false);
    //--------------------------------------------图片展示区
    video_1=new PictureBox(this);
    video_1->setFixedSize(middleWindowWidth,middleDownWindowheight*0.82);
    video_1->setGeometry(leftWindowWidth,middleUpWindowheight,video_1->width(),video_1->height()*0.95);
    window_width_video=video_1->width();
    window_height_video=video_1->height();
}


//初始化规则属性窗口
void ConfigWidget::initProperWindow()
{
    configheight=30;
    variantManager = new QtVariantPropertyManager(this);

    connect(variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
            this, SLOT(valueChanged(QtProperty *, const QVariant &)));

    QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory(this);

    QDockWidget *dock = new QDockWidget(this);
    dock->setFixedSize(rightWindowWidth,windowHeight*0.4);
    dock->setFeatures(QDockWidget::NoDockWidgetFeatures); //没有相关的权限
    dock->setGeometry(leftWindowWidth+middleWindowWidth,0,dock->width(),dock->height());

    propertyEditor = new QtTreePropertyBrowser(dock);
    propertyEditor->setFactoryForManager(variantManager, variantFactory);
    dock->setWidget(propertyEditor);


    connect(this, SIGNAL(itemClicked(QString)),
            this, SLOT(itemClickedChange(QString)));


    labAlarmTimes=new QLabel(this);
    labAlarmTimes->setText("报警的时间区间集");
    labAlarmTimes->setFixedSize(rightWindowWidth,configheight);
    labAlarmTimes->setGeometry(leftWindowWidth+middleWindowWidth,windowHeight*0.4,rightWindowWidth,configheight);

    labelstar=new QLabel(this);
    labelstar->setText("开始时间");
    labelstar->setFixedSize(rightWindowWidth/2,configheight);
    labelstar->setGeometry(leftWindowWidth+middleWindowWidth,windowHeight*0.4+configheight*1,rightWindowWidth/2,configheight);

    timeEdStart=new QTimeEdit(this);
    timeEdStart->setFixedSize(rightWindowWidth/2,configheight);
    timeEdStart->setGeometry(leftWindowWidth+middleWindowWidth,windowHeight*0.4+configheight*2,rightWindowWidth/2,configheight);
    timeEdStart->setDisplayFormat("hh:mm:ss ");

    labelstop=new QLabel(this);
    labelstop->setText("结束时间");
    labelstop->setFixedSize(rightWindowWidth/2,configheight);
    labelstop->setGeometry(leftWindowWidth+middleWindowWidth+rightWindowWidth/2,windowHeight*0.4+configheight*1,rightWindowWidth/2,configheight);

    timeEdStop=new QTimeEdit(this);
    timeEdStop->setFixedSize(rightWindowWidth/2,configheight);
    timeEdStop->setGeometry(leftWindowWidth+middleWindowWidth+rightWindowWidth/2,windowHeight*0.4+configheight*2,rightWindowWidth/2,configheight);
    timeEdStop->setDisplayFormat("hh:mm:ss ");

    labelAlarmNumDuringTime=new QLabel(this);
    labelAlarmNumDuringTime->setText("设定时间内允许犯规的次数");
    labelAlarmNumDuringTime->setFixedSize(rightWindowWidth/2,configheight);
    labelAlarmNumDuringTime->setGeometry(leftWindowWidth+middleWindowWidth,windowHeight*0.4+configheight*3,rightWindowWidth/2,configheight);

    lineEditAlarmNumDuringTime=new QLineEdit(this);
    lineEditAlarmNumDuringTime->setText("0");
    lineEditAlarmNumDuringTime->setFixedSize(rightWindowWidth/2,configheight);
    lineEditAlarmNumDuringTime->setGeometry(leftWindowWidth+middleWindowWidth+rightWindowWidth/2,windowHeight*0.4+configheight*3,rightWindowWidth/2,configheight);


    addlist=new QPushButton(this);
    addlist->setFixedSize(rightWindowWidth/2,configheight);
    addlist->setText("增加");
    addlist->setGeometry(leftWindowWidth+middleWindowWidth,windowHeight*0.4+configheight*4,rightWindowWidth/2,configheight);
    addlist->setObjectName("addlist");
    addlist->setStyleSheet("#addlist{border-image: url(:/image/button2.png);}"
                               "#addlist:hover{border-image: url(:/image/button.png);}"
                               "#addlist:pressed{border-image: url(:/image/button3.png);}");

    dellist=new QPushButton(this);
    dellist->setFixedSize(rightWindowWidth/2,configheight);
    dellist->setText("删除");
    dellist->setGeometry(leftWindowWidth+middleWindowWidth+rightWindowWidth/2,windowHeight*0.4+configheight*4,rightWindowWidth/2,configheight);
    dellist->setObjectName("dellist");
    dellist->setStyleSheet("#dellist{border-image: url(:/image/button2.png);}"
                               "#dellist:hover{border-image: url(:/image/button.png);}"
                               "#dellist:pressed{border-image: url(:/image/button3.png);}");


    lvtAlarmTimes=new QListWidget(this);
    lvtAlarmTimes->setFixedSize(rightWindowWidth,100);
    lvtAlarmTimes->setGeometry(leftWindowWidth+middleWindowWidth,windowHeight*0.4+configheight*6,rightWindowWidth,100);

    buttonConfirm=new QPushButton(this);
    buttonConfirm->setFixedSize(80,40);
    buttonConfirm->setText("确认提交");
    buttonConfirm->setGeometry(leftWindowWidth+middleWindowWidth,windowHeight*0.4+configheight*7+100,200,50);
    buttonConfirm->setObjectName("buttonConfirm");
    buttonConfirm->setStyleSheet("#buttonConfirm{border-image: url(:/image/button2.png);}"
                               "#buttonConfirm:hover{border-image: url(:/image/button.png);}"
                               "#buttonConfirm:pressed{border-image: url(:/image/button3.png);}");
    connect(addlist,SIGNAL(clicked()),this,SLOT(slot_addtime()));
    connect(dellist,SIGNAL(clicked()),this,SLOT(slot_deltime()));
    connect(buttonConfirm,SIGNAL(clicked()),this,SLOT(slot_saveButton()));

}


//加载摄像头列表[AreaID],[AreaName],[AreaIP],[IPCID],[IPCName],[IPCRtspAddrMain]
void ConfigWidget::loadCameraTree()
{
    getCameraInfo();

    videoTreewidget->clear();
    IPCMap mapChannles = GLOBAL_DATA->GetChannel();

    QMap<QString,mArea>::const_iterator iterArea;
    for(iterArea = map_Area.constBegin();iterArea != map_Area.constEnd();iterArea++)
    {
        QTreeWidgetItem *itemArea = new QTreeWidgetItem
                (videoTreewidget, QStringList(iterArea.value().AreaName + "[" + iterArea.value().AreaAdress + "]"));
        itemArea->setIcon(0, QIcon(":/image/Area.png"));
        itemArea->setToolTip(0,QString(iterArea.value().AreaName + "[" + iterArea.value().AreaAdress + "]"));

        for(IPCMap::iterator itchannel = mapChannles.begin(); itchannel!=mapChannles.end(); ++itchannel)
        {
            QTreeWidgetItem *itemChannel = new QTreeWidgetItem
                    (itemArea, QStringList("通道[" + QString("%1").arg(itchannel->first) + "]"));

            vector<mIPC> &vecIPC = itchannel->second;
            for(size_t i=0; i<vecIPC.size(); i++)
            {
                QTreeWidgetItem *itemIPC = new QTreeWidgetItem(itemChannel, QStringList(QString(vecIPC[i].IPCName + "---" + vecIPC[i].IPCId)));
                itemIPC->setIcon(0, QIcon(":/image/ipc_normal.png"));
                itemIPC->setToolTip(0,QString(vecIPC[i].IPCName + "---"+vecIPC[i].IPCId));
                itemChannel->addChild(itemIPC);
            }
            itemArea->addChild(itemChannel);
        }
    }

    videoTreewidget->expandAll();
}


void ConfigWidget::getCameraInfo()
{
//    map_Machine.clear();
    map_Area.clear();
    map_AlarmType.clear();
    map_LineType.clear();
    QString retCode="";
    QString server_id=g_AnalyseServerID;

    DATA_TRANSLATE->getCameraConfig(server_id,retCode);
    if(retCode=="999")
       emit  callLogin();
    QString cameraId="";
    DATA_TRANSLATE->getCameraRule(cameraId,retCode);
    DATA_TRANSLATE->getAlarmType(retCode);
    DATA_TRANSLATE->getLineType(retCode);
    NETWORK_MANAGER->getCameraRTSPList();
}

void ConfigWidget::mousePressEvent(QMouseEvent *event)
{
    QRect rc=video_1->geometry();

    if(rc.contains(this->mapFromGlobal(QCursor::pos()))==true && m_isGetImage)//如果按下的位置
    {
        if (event->button() == Qt::LeftButton)
        {
            QPoint leftButtonPoint= event->globalPos();//获取全局位置
            leftButtonPoint=video_1->mapFromGlobal(leftButtonPoint);//相对于pictureBox的坐标原点而言的坐标
            getScaleAndoffsetXY();
            leftButtonPoint.setX(leftButtonPoint.x()-offsetX);
            leftButtonPoint.setY(leftButtonPoint.y()-offsetY);
            QString highlightNum="";
            if(isDraw)         //如果处于画图流程
            {
                pointslist[QString("%1_%2").arg(ipcid).arg(currentPointKey)].append(leftButtonPoint);
            }
            else               //选中
            {
                bool ischoose=false;

                if(isSaveLock&&pointsKey!="0")
                {
                    if(!ALARM_MANAGER->HasRule(ipcid, ruleid))
                    {
                        QMessageBox::about(NULL, "规则属性未配置！", "请您在右侧属性栏配置当前画线属性，然后点击“确认提交”保存！或者请您点击“删除选中规则”清除配置不完全的画线！");
                        return;
                    }
                }

                QMap<QString,QVector<QPoint>>::iterator iter;
                for(iter=pointslist.begin();iter!=pointslist.end();iter++)
                {
                    if(QJudgePosition::onPolygonLine(leftButtonPoint,iter.value()))
                    {
                        highlightNum=iter.key();
                        points=iter.value();
                        currentPointKey=highlightNum.split("_")[1];
                        ishighlight=true;
                        buttonCreateRule->setEnabled(false);
                        buttonStartDraw->setEnabled(false);
                        buttonRestLast->setEnabled(false);
                        buttonRedo->setEnabled(false);
                        buttonEndDraw->setEnabled(false);
                        buttonEditCurrentRule->setEnabled(true);
                        buttonClearCurrentRule->setEnabled(true);
                        emit itemClicked(highlightNum);
                        ischoose=true;
                        break;
                    }
                }
                if(!ischoose)        //如果选中了空白地带
                {
                    if(ishighlight)
                    {
                        QColor pointColor(0,0,135);
                        QColor lineColor(0,255,0);
                        QColor highlighLineColor(255,0,0);
                        showPicture(pointColor,lineColor,highlighLineColor,highlightNum);
                        points.clear();
                        currentPointKey="0";
                        buttonCreateRule->setEnabled(true);
                        buttonStartDraw->setEnabled(false);
                        buttonRestLast->setEnabled(false);
                        buttonRedo->setEnabled(false);
                        buttonEndDraw->setEnabled(false);
                        buttonEditCurrentRule->setEnabled(false);
                        buttonClearCurrentRule->setEnabled(false);
                        ishighlight=false;
                        emit itemClicked(QString(""));
                    }
                }
            }

            if(isDraw||ishighlight)
            {
                QColor pointColor(0,0,135);
                QColor lineColor(0,255,0);
                QColor highlighLineColor(255,0,0);
                showPicture(pointColor,lineColor,highlighLineColor,highlightNum);
            }
        }
    }
}


//加载单个摄像头的配置规则
void ConfigWidget::loadCameraRule()
{
    if(isSaveLock&&pointsKey!="0")
    {
        if(!ALARM_MANAGER->HasRule(ipcid, ruleid))
        {
            QMessageBox::about(NULL, "请在配置完规则后切换摄像头！", "请您在右侧属性栏配置当前画线属性，然后点击“确认提交”保存！或者请您点击“删除选中规则”清除配置不完全的画线！");
            return;
        }
    }
    //选中的是Area或分析服务器则不处理
    if (videoTreewidget->currentItem()->parent() == 0) {
        return;
    }

    emit itemClicked("");   //清除属性表
    setAttributeVisibelFalse();

    QString temp = videoTreewidget->currentIndex().data().toString();
    QStringList list=temp.split("---");
    ipcName=list[0];
    ipcid=list[1];

    pointslist.clear();

    RuleMap rules = ALARM_MANAGER->getRuleMap();
    RuleMap::iterator it = rules.find(ipcid);
    if(it != rules.end())
    {
        RuleList &listRule = it->second;
        for(RuleList::iterator itlist = listRule.begin(); itlist!=listRule.end(); ++itlist)
        {
            QVector<QPoint> tempPoints = itlist->points;

            double r1, r2;
            r1 = window_width_video / (double)PIC_WIDTH;
            r2 = window_height_video / (double)PIC_HEIGHT;
            double r = qMin(r1, r2);

            for(int i=0;i<tempPoints.size();i++)
            {
                double newX=tempPoints[i].x()*r;
                double newY=tempPoints[i].y()*r;
                tempPoints[i].setX(newX);
                tempPoints[i].setY(newY);
            }

            pointslist.insert(ipcid+"_"+itlist->rule_id, tempPoints);
        }
    }

    //start
    video_1->setVisible(false);
    labloading->setVisible(true);
    QMovie* movie = new QMovie(":/image/loading.gif");
    labloading->setAlignment(Qt::AlignCenter);
    labloading->setMovie(movie);
    movie->start();
    labloading->show();

    //QString rtsp=QString("rtsp://admin:hnmqet123@192.168.1.252:554/h264/ch1/main/av_strea5");
    QString onvf=GLOBAL_DATA->getCameraRTSP(ipcid);
    QString rtsp = onvf.split("_onviftoken_")[0];

    m_isGetImage=false;
    if(m_pFfm!=NULL && m_pFfm->init(rtsp))
    {
        int count=120;
        while(--count>0)
        {
            if(!m_pFfm->readOneFrame())
            {
                break;
            }

            cv::Mat mat;
            if(m_pFfm->getOneFrame(mat) && !mat.empty())
            {
                m_image = MatToQImage(mat);
                m_isGetImage=true;
                break;
            }

        }
    }

    //stop
    movie->stop();
    delete movie;
    movie=NULL;
    labloading->setVisible(false);
    video_1->setVisible(true);

    if(m_isGetImage)
    {
        buttonCreateRule->setEnabled(true);
        buttonStartDraw->setEnabled(false);
        buttonRestLast->setEnabled(false);
        buttonRedo->setEnabled(false);
        buttonEndDraw->setEnabled(false);
        buttonEditCurrentRule->setEnabled(false);
        buttonClearCurrentRule->setEnabled(false);

        getScaleAndoffsetXY();
        QString highlightNum="";
        QColor pointColor(0,0,135);
        QColor lineColor(0,255,0);
        QColor highlighLineColor(255,0,0);
        showPicture(pointColor,lineColor,highlighLineColor,highlightNum);
    }
    else
    {
        m_image.load(":/image/imageError.jpg");
        video_1->setMode(PictureBox::PB_MODE::AUTO_ZOOM);
        imgtemp=m_image.scaled(m_image.width()*r, m_image.height()*r);
        video_1->setImage(imgtemp);
        QMessageBox::about(NULL, "获取监控场景失败", "请重新双击摄像头尝试获取");
    }

}

void ConfigWidget::getScaleAndoffsetXY()
{
    double window_width, window_height;
    double image_width, image_height;
    double r1, r2;
    window_width = video_1->width();
    window_height = video_1->height();
    image_width = m_image.width();
    image_height = m_image.height();
    r1 = window_width / image_width;
    r2 = window_height / image_height;
    r = qMin(r1, r2);
    offsetX = (window_width - r * image_width) / 2;
    offsetY = (window_height - r * image_height) / 2;
}

void ConfigWidget::showPicture(QColor pointColor,QColor lineColor,QColor highlighLineColor,QString highlightNum)
{
    imgtemp=m_image.scaled(m_image.width()*r,m_image.height()*r);
    QMap<QString,QVector<QPoint>>::iterator iter;
    for(iter=pointslist.begin();iter!=pointslist.end();iter++)
    {
        QVector<QPoint> pointTemp=iter.value();

        if(highlightNum==iter.key())
        {
            qdrawline::drawLine(pointTemp,imgtemp,pointColor,highlighLineColor);
        }
        else
        {
            qdrawline::drawLine(pointTemp,imgtemp,pointColor,lineColor);
        }
    }

    video_1->setMode(PictureBox::PB_MODE::AUTO_ZOOM);
    video_1->setImage(imgtemp);
}

void ConfigWidget::on_videoTreewidget_doubleClicked(const QModelIndex &index)
{
    loadCameraRule();
}


void ConfigWidget::updateExpandState()
{
    QList<QtBrowserItem *> list = propertyEditor->topLevelItems();
    QListIterator<QtBrowserItem *> it(list);
    while (it.hasNext()) {
        QtBrowserItem *item = it.next();
        QtProperty *prop = item->property();
        idToExpanded[propertyToId[prop]] = propertyEditor->isExpanded(item);
    }
}


void ConfigWidget::itemClickedChange(QString pointKey)
{
    updateExpandState();

    QMap<QtProperty *, QString>::ConstIterator itProp = propertyToId.constBegin();
    while (itProp != propertyToId.constEnd()) {
        delete itProp.key();
        itProp++;
    }
    propertyToId.clear();
    idToProperty.clear();
    if (pointKey=="") {
        setAttributeVisibelFalse();
        return;
    }
    else
        setAttributeVisibelTrue();

    QtVariantProperty *property;
    rulename="";
    ruleid="0";
    lineType="";
    alarmType="";
    IPCId=ipcid;
    alarmMinute=-1;
    alertMinute=-1;
    personNum=-1;
    max_alarm_person_num = -1;
    alarm_threshold = 0;
    alart_threshold = 0;
    two_remind_time = -1;
    timelist.clear();

    //如果包含点的规则,则属性表加载此规则
    QStringList str = pointKey.split("_");
    mRule ruletemp;
    if(str.size()>=2 && ALARM_MANAGER->HasRule(str[0], str[1], ruletemp))
    {
        points=ruletemp.points;
        rulename=ruletemp.rule_name;
        ruleid=ruletemp.rule_id;
        lineType=QString("%1").arg(ruletemp.line_type);
        alarmType=QString("%1").arg(ruletemp.alarm_type);
        IPCId=ruletemp.camera_id;
        alarmMinute=ruletemp.alarm_delay;
        alertMinute=ruletemp.alert_delay;
        personNum=ruletemp.alarm_person_num;
        max_alarm_person_num = ruletemp.max_alarm_person_num;
        alarm_threshold = ruletemp.alarm_threshold;
        alart_threshold = ruletemp.alert_threshold;
        two_remind_time= ruletemp.two_remind_time;
        timelist=ruletemp.times;
    }

    QMap<int,mDictionary>::const_iterator iter;
    QStringList enumNames;
    enumNames.insert(0,"请选择");
    QStringList enumNames2;
    enumNames2.insert(0,"请选择");
    for(iter=map_LineType.constBegin();iter!=map_LineType.constEnd();iter++)
    {
        mDictionary dictionary=iter.value();
        enumNames.insert(dictionary.code.trimmed().toInt(),dictionary.name);
    }
    for(iter=map_AlarmType.constBegin();iter!=map_AlarmType.constEnd();iter++)
    {
        mDictionary dictionary=iter.value();
        enumNames2.insert(dictionary.code.trimmed().toInt(),dictionary.name);
    }

    property = variantManager->addProperty(QtVariantPropertyManager::enumTypeId(),QString("报警类型"));
    property->setAttribute(QString("enumNames"), enumNames2);
    if(!alarmType.isEmpty()) property->setValue(alarmType.toInt());
    else property->setValue(0);
    addProperty(property, QString("alarmtype"+pointKey));

    property = variantManager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("画线类型"));
    property->setAttribute(QString("enumNames"), enumNames);
    if(!lineType.isEmpty()) property->setValue(lineType.toInt());
    else property->setValue(0);
    addProperty(property, QString("linetype"+pointKey));

    property = variantManager->addProperty(QVariant::String, QString("规则名称"));
    property->setValue(rulename);
    addProperty(property, QString("rulename"+pointKey));

    property = variantManager->addProperty(QVariant::Int, QString("最小人数"));
    property->setValue(personNum);
    property->setAttribute(QString("minimum"), -1);
    property->setAttribute(QString("maximum"), 100);
    property->setAttribute(QString("singleStep"), 1);
    addProperty(property, QString("person"+pointKey));

    property = variantManager->addProperty(QVariant::Int, QString("最大人数"));
    property->setValue(max_alarm_person_num);
    property->setAttribute(QString("minimum"), -1);
    property->setAttribute(QString("maximum"), 100);
    property->setAttribute(QString("singleStep"), 1);
    addProperty(property, QString("max_alarm_person_num"+pointKey));

    property = variantManager->addProperty(QVariant::Int, QString("报警轮询(秒)"));
    property->setValue(two_remind_time);
    property->setAttribute(QString("minimum"), -1);
    property->setAttribute(QString("maximum"), 60*60);
    property->setAttribute(QString("singleStep"), 10);
    addProperty(property, QString("two_remind_time"+pointKey));

    QStringList enumNames3;
    enumNames3.insert(0,"请选择");
    enumNames3.insert(1,"仅提醒关注");
    enumNames3.insert(2,"仅报警");
    //enumNames3.insert(3,"提醒关注并报警");
    property = variantManager->addProperty(QtVariantPropertyManager::enumTypeId(),QString("报警设置"));
    property->setAttribute(QString("enumNames"), enumNames3);
    if(alarmMinute==-1&&alertMinute==-1)  property->setValue(0);
    else if(alarmMinute==-1&&alertMinute!=-1) property->setValue(1);
    else if(alarmMinute!=-1&&alertMinute==-1) property->setValue(2);
    else if(alarmMinute!=-1&&alertMinute!=-1) property->setValue(3);
    addProperty(property, QString("alarmAlert"+pointKey));

    property = variantManager->addProperty(QVariant::Int,QString("提醒延时(秒)"));
    property->setValue(alarmMinute);
    property->setAttribute(QString("minimum"), -1);
    property->setAttribute(QString("maximum"), 600);
    property->setAttribute(QString("singleStep"), 1);
    addProperty(property, QString("alterdelay"+pointKey));

    property = variantManager->addProperty(QVariant::Int,QString("提醒阈值(%)"));
    property->setValue(alart_threshold);
    property->setAttribute(QString("minimum"), 0);
    property->setAttribute(QString("maximum"), 100);
    property->setAttribute(QString("singleStep"), 10);
    addProperty(property, QString("alart_threshold"+pointKey));

    property = variantManager->addProperty(QVariant::Int,QString("报警延时(秒)"));
    property->setValue(alarmMinute);
    property->setAttribute(QString("minimum"), -1);
    property->setAttribute(QString("maximum"), 600);
    property->setAttribute(QString("singleStep"), 1);
    addProperty(property, QString("alarmdelay"+pointKey));

    property = variantManager->addProperty(QVariant::Int,QString("报警阈值(%)"));
    property->setValue(alarm_threshold);
    property->setAttribute(QString("minimum"), 0);
    property->setAttribute(QString("maximum"), 100);
    property->setAttribute(QString("singleStep"), 10);
    addProperty(property, QString("alarm_threshold"+pointKey));

    if(alarmMinute==-1&&alertMinute==-1)
        setPropertEnable(QString("alterdelay%1_%2").arg(ipcid).arg(currentPointKey),QString("alarmdelay%1_%2").arg(ipcid).arg(currentPointKey),QString("alart_threshold%1_%2").arg(ipcid).arg(currentPointKey),QString("alarm_threshold%1_%2").arg(ipcid).arg(currentPointKey),false,false,-1,-1,0,0);
    else if(alarmMinute==-1&&alertMinute!=-1)
        setPropertEnable(QString("alterdelay%1_%2").arg(ipcid).arg(currentPointKey),QString("alarmdelay%1_%2").arg(ipcid).arg(currentPointKey),QString("alart_threshold%1_%2").arg(ipcid).arg(currentPointKey),QString("alarm_threshold%1_%2").arg(ipcid).arg(currentPointKey),true,false,alertMinute,-1,alart_threshold,0);
    else if(alarmMinute!=-1&&alertMinute==-1)
        setPropertEnable(QString("alterdelay%1_%2").arg(ipcid).arg(currentPointKey),QString("alarmdelay%1_%2").arg(ipcid).arg(currentPointKey),QString("alart_threshold%1_%2").arg(ipcid).arg(currentPointKey),QString("alarm_threshold%1_%2").arg(ipcid).arg(currentPointKey),false,true,-1,alarmMinute,0,alarm_threshold);
    else if(alarmMinute!=-1&&alertMinute!=-1)
        setPropertEnable(QString("alterdelay%1_%2").arg(ipcid).arg(currentPointKey),QString("alarmdelay%1_%2").arg(ipcid).arg(currentPointKey),QString("alart_threshold%1_%2").arg(ipcid).arg(currentPointKey),QString("alarm_threshold%1_%2").arg(ipcid).arg(currentPointKey),true,true,alertMinute,alarmMinute,alart_threshold,alarm_threshold);
    //加载报警时间段
    lvtAlarmTimes->clear();
    for(int i=0;i<timelist.size();i++)
    {
        lvtAlarmTimes->addItem(timelist[i]);
    }
}

void ConfigWidget::setPropertEnable(QString keyAlert,QString keyAlarm,QString keyAlert_threshold,QString keyAlarm_threshold,bool alertEnable,bool alarmEnable,int alertMinute,int alarmMinute,float alertThresholdValue,float alarmThresholdValue)
{
    idToProperty[keyAlert]->setEnabled(alertEnable);
    idToProperty[keyAlert]->setValue(alertMinute);
    idToProperty[keyAlarm]->setEnabled(alarmEnable);
    idToProperty[keyAlarm]->setValue(alarmMinute);
    idToProperty[keyAlert_threshold]->setEnabled(alertEnable);
    idToProperty[keyAlert_threshold]->setValue(alertThresholdValue);
    idToProperty[keyAlarm_threshold]->setEnabled(alarmEnable);
    idToProperty[keyAlarm_threshold]->setValue(alarmThresholdValue);
}

void ConfigWidget::valueChanged(QtProperty *property, const QVariant &value)
{
    if (!propertyToId.contains(property))
        return;
    QString id = propertyToId[property];
    if (id == QString("alarmtype%1_%2").arg(ipcid).arg(currentPointKey))
    {
        int alarmId=idToProperty[QString("alarmtype%1_%2").arg(ipcid).arg(currentPointKey)]->value().toInt();
        QString areaName=map_Area[0].AreaName;//TODO:map_Area
        QString alarmName=QString("%1%2%3").arg(areaName).arg(ipcName).arg(map_AlarmType[alarmId].name);
        idToProperty[QString("rulename%1_%2").arg(ipcid).arg(currentPointKey)]->setValue(alarmName);
    }
    else if(id==QString("alarmAlert%1_%2").arg(ipcid).arg(currentPointKey))
    {
        int alarmAlert=idToProperty[QString("alarmAlert%1_%2").arg(ipcid).arg(currentPointKey)]->value().toInt();
        switch (alarmAlert) {
        case 0:
            setPropertEnable(QString("alterdelay%1_%2").arg(ipcid).arg(currentPointKey),QString("alarmdelay%1_%2").arg(ipcid).arg(currentPointKey),QString("alart_threshold%1_%2").arg(ipcid).arg(currentPointKey),QString("alarm_threshold%1_%2").arg(ipcid).arg(currentPointKey),false,false,-1,-1,0,0);
            break;
        case 1:
            setPropertEnable(QString("alterdelay%1_%2").arg(ipcid).arg(currentPointKey),QString("alarmdelay%1_%2").arg(ipcid).arg(currentPointKey),QString("alart_threshold%1_%2").arg(ipcid).arg(currentPointKey),QString("alarm_threshold%1_%2").arg(ipcid).arg(currentPointKey),true,false,-1,-1,0,0);
            break;
        case 2:
            setPropertEnable(QString("alterdelay%1_%2").arg(ipcid).arg(currentPointKey),QString("alarmdelay%1_%2").arg(ipcid).arg(currentPointKey),QString("alart_threshold%1_%2").arg(ipcid).arg(currentPointKey),QString("alarm_threshold%1_%2").arg(ipcid).arg(currentPointKey),false,true,-1,-1,0,0);
            break;
        case 3:
            setPropertEnable(QString("alterdelay%1_%2").arg(ipcid).arg(currentPointKey),QString("alarmdelay%1_%2").arg(ipcid).arg(currentPointKey),QString("alart_threshold%1_%2").arg(ipcid).arg(currentPointKey),QString("alarm_threshold%1_%2").arg(ipcid).arg(currentPointKey),true,true,-1,-1,0,0);
            break;
        default:
            break;
        }
    }
}

void ConfigWidget::addProperty(QtVariantProperty *property, const QString &id)
{
    propertyToId[property] = id;
    idToProperty[id] = property;
    QtBrowserItem *item = propertyEditor->addProperty(property);
    if (idToExpanded.contains(id))
        propertyEditor->setExpanded(item, idToExpanded[id]);
}

void ConfigWidget::saveRule(QString pointKey)
{
    rulename=idToProperty["rulename"+pointKey]->value().value<QString>();
    lineType=idToProperty["linetype"+pointKey]->value().value<QString>();
    alarmType=idToProperty["alarmtype"+pointKey]->value().value<QString>();

    if(idToProperty["alarmdelay"+pointKey]->isEnabled())
        alarmMinute=idToProperty["alarmdelay"+pointKey]->value().toInt();
    else alarmMinute=-1;

    if(idToProperty["alterdelay"+pointKey]->isEnabled())
        alertMinute=idToProperty["alterdelay"+pointKey]->value().toInt();
    else alertMinute=-1;

    personNum=idToProperty["person"+pointKey]->value().toInt();
    max_alarm_person_num = idToProperty["max_alarm_person_num"+pointKey]->value().toInt();
//    qDebug()<<"1111111111"<<max_alarm_person_num;
    alarm_threshold = idToProperty["alarm_threshold"+pointKey]->value().toDouble();
//    qDebug()<<"2222222222"<<alarm_threshold;
    alart_threshold = idToProperty["alart_threshold"+pointKey]->value().toDouble();
//    qDebug()<<"33333333333"<<alart_threshold;
    two_remind_time = idToProperty["two_remind_time"+pointKey]->value().toInt();
//    qDebug()<<"44444444444"<<two_remind_time;

    if(rulename.isEmpty())
    {
        QMessageBox::about(NULL, "配置不完整", "请输入规则名称");
        return;
    }
    if(lineType.toInt()<=0)
    {
        QMessageBox::about(NULL, "配置不完整", "请选择画线类型");
        return;
    }
    if(alarmType.toInt()<=0)
    {
        QMessageBox::about(NULL, "配置不完整", "请选择报警类型");
        return;
    }
    if(alarmMinute<0&&alertMinute<0)
    {
        QMessageBox::about(NULL, "配置不完整", "请输入报警时间");
        return;
    }
    if(two_remind_time<0)
    {
        QMessageBox::about(NULL, "配置不完整", "请输入二次提醒时间");
        return;
    }
    if(alarm_threshold<=0&&alarmMinute>=0)
    {
        QMessageBox::about(NULL, "配置不完整", "请输入报警阈值");
        return;
    }
    if(alart_threshold<=0&&alertMinute>=0)
    {
         QMessageBox::about(NULL, "配置不完整", "请输入提醒关注阈值");
         return;
    }
    if(alarmMinute>=0&&alertMinute>=alarmMinute)
    {
        QMessageBox::about(NULL, "配置错误", "报警时间应大于提醒关注时间");
        return;
    }
    if(personNum<0)
    {
        QMessageBox::about(NULL, "配置不完整", "请选择最少报警人数");
        return;
    }
    if(max_alarm_person_num<0&&max_alarm_person_num<personNum)
    {
        QMessageBox::about(NULL, "配置不完整", "最多报警人数<最少报警人数");
        return;
    }
    if(lvtAlarmTimes->count()==0)
    {
        QMessageBox::about(NULL, "配置不完整", "请选择监控时间段，可多选");
        return;
    }
    if(points.size()==0)
    {
        QMessageBox::about(NULL, "配置不完整", "请点击‘开始画线’绘制监控区域");
        return;
    }

    mRule ruletemp;
    //reset points based on image 1280x800
    ruletemp.points=points;


    ruletemp.rule_name=rulename;
    if(ruleid !="0")
        ruletemp.rule_id=ruleid;
    else
        ruletemp.rule_id='0';     //新规则传0,转化为空值
    ruletemp.line_type= lineType.toInt();
    ruletemp.alarm_type=alarmType.toInt();
    ruletemp.camera_id=ipcid;
    ruletemp.alarm_delay=alarmMinute;
    ruletemp.alert_delay=alertMinute;
    ruletemp.alarm_person_num=personNum;
    ruletemp.max_alarm_person_num = max_alarm_person_num;
    ruletemp.alarm_threshold = alarm_threshold;
    ruletemp.alert_threshold = alart_threshold;
    ruletemp.two_remind_time = two_remind_time;

    for(int i=0;i<lvtAlarmTimes->count();i++)
        ruletemp.times.append(lvtAlarmTimes->item(i)->text());

    isSaveLock=false;
    buttonCreateRule->setEnabled(false);
    buttonStartDraw->setEnabled(false);
    buttonRestLast->setEnabled(false);
    buttonRedo->setEnabled(false);
    buttonEndDraw->setEnabled(false);
    buttonEditCurrentRule->setEnabled(true);
    buttonClearCurrentRule->setEnabled(true);
    QString retCode="";
    DATA_TRANSLATE->insertOrUpdateRule(ruletemp,retCode);

    ALARM_MANAGER->SaveRule(ruletemp.camera_id, ruletemp);
    if(retCode=="999")
    {
       emit  callLogin();
    }
    QMessageBox::about(NULL, "温馨提示", "您的规则更改已生效！");

}

void ConfigWidget::slot_createNewRule()
{

    buttonCreateRule->setEnabled(false);
    buttonStartDraw->setEnabled(true);
    setAttributeVisibelTrue();
    int intPiontsKey=pointsKey.toInt();
    intPiontsKey++;
    pointsKey=QString("%1").arg(intPiontsKey);

    currentPointKey=pointsKey;
    emit itemClicked(QString("%1_%2").arg(ipcid).arg(currentPointKey));
}

void ConfigWidget::slot_begindraw()
{
    if(isSaveLock&&pointsKey!="0")
    {
        if(!ALARM_MANAGER->HasRule(ipcid, currentPointKey))
        {
            QMessageBox::about(NULL, "请在配置完当前规则后再配置新的规则！", "请您在右侧属性栏配置当前画线属性，然后点击“确认提交”保存！或者请您点击“删除选中规则”清除配置不完全的画线！");
            return;
        }
    }

    isDraw=true;
    points.clear();
    pointDeleted.clear();
    buttonStartDraw->setEnabled(false);
    buttonRestLast->setEnabled(true);
    buttonRedo->setEnabled(true);
    buttonEndDraw->setEnabled(true);
    buttonEditCurrentRule->setEnabled(false);
    buttonClearCurrentRule->setEnabled(false);
}

void ConfigWidget::slot_undolastline()
{
    if(pointslist[QString("%1_%2").arg(ipcid).arg(currentPointKey)].size()==0) return;
    pointDeleted.append(pointslist[QString("%1_%2").arg(ipcid).arg(currentPointKey)].last());  //保存点队列
    pointslist[QString("%1_%2").arg(ipcid).arg(currentPointKey)].removeLast();

    imgtemp=m_image.scaled(m_image.width()*r, m_image.height()*r);
    QString highlightNum=QString("%1_%2").arg(ipcid).arg(currentPointKey);
    QColor pointColor(0,0,135);
    QColor lineColor(0,255,0);
    QColor highlighLineColor(255,0,0);
    showPicture(pointColor,lineColor,highlighLineColor,highlightNum);
    video_1->setMode(PictureBox::PB_MODE::AUTO_ZOOM);
    video_1->setImage(imgtemp);
}

void ConfigWidget::slot_redolastline()
{
    if(pointDeleted.size()==0) return;
    pointslist[QString("%1_%2").arg(ipcid).arg(currentPointKey)].append(pointDeleted.last());  //保存点队列
    pointDeleted.removeLast();
;
    imgtemp=m_image.scaled(m_image.width()*r,m_image.height()*r);
    QString highlightNum=QString("%1_%2").arg(ipcid).arg(currentPointKey);
    QColor pointColor(0,0,135);
    QColor lineColor(0,255,0);
    QColor highlighLineColor(255,0,0);
    showPicture(pointColor,lineColor,highlighLineColor,highlightNum);
    video_1->setMode(PictureBox::PB_MODE::AUTO_ZOOM);
    video_1->setImage(imgtemp);
}

void ConfigWidget::slot_enddraw()
{
    bool isCancelDraw=false;
    //只画一个点的情况
    if(pointslist[QString("%1_%2").arg(ipcid).arg(currentPointKey)].size()<2)
    {
        QMessageBox::StandardButton button;
        button = QMessageBox::question(this, tr("是否结束画线"),
                                       QString(tr("您需要画至少一条完整线段，继续描点画线请按\"No\",放弃画线请按\"Yes\"")),
                                       QMessageBox::Yes | QMessageBox::No);
        if (button == QMessageBox::No) {
            return;
        }
        else if (button == QMessageBox::Yes) {
            isCancelDraw=true;
        }
    }
    //未闭合的区域
    if(pointslist[QString("%1_%2").arg(ipcid).arg(currentPointKey)].size()>2&&
            QJudgePosition::disOfTwoPoints(pointslist[QString("%1_%2").arg(ipcid).arg(currentPointKey)].last(),pointslist[QString("%1_%2").arg(ipcid).arg(currentPointKey)].first())>20)
    {
        QMessageBox::StandardButton button;
        button = QMessageBox::question(this, tr("区域未闭合"),
                                       QString(tr("是否自动闭合？按\"Yes\"自动闭合结束画图,按\"No\"放弃结束继续画图")),
                                       QMessageBox::Yes | QMessageBox::No);
        if (button == QMessageBox::No) {
            //return;
        }
        else if (button == QMessageBox::Yes) {
            QPoint p=pointslist[QString("%1_%2").arg(ipcid).arg(currentPointKey)].first();
            pointslist[QString("%1_%2").arg(ipcid).arg(currentPointKey)].append(p);
            ishighlight=true;
            isSaveLock=true;
        }
    }

    if(pointslist[QString("%1_%2").arg(ipcid).arg(currentPointKey)].size()>2&&
            QJudgePosition::disOfTwoPoints(pointslist[QString("%1_%2").arg(ipcid).arg(currentPointKey)].last(),pointslist[QString("%1_%2").arg(ipcid).arg(currentPointKey)].first())<20)
    {
        pointslist[QString("%1_%2").arg(ipcid).arg(currentPointKey)].last()=pointslist[QString("%1_%2").arg(ipcid).arg(currentPointKey)].first();
        ishighlight=true;
        isSaveLock=true;
    }


    isDraw=false;
    buttonCreateRule->setEnabled(false);
    buttonStartDraw->setEnabled(false);
    buttonRestLast->setEnabled(false);
    buttonRedo->setEnabled(false);
    buttonEndDraw->setEnabled(false);
    buttonEditCurrentRule->setEnabled(true);
    buttonClearCurrentRule->setEnabled(true);
    pointDeleted.clear();

    if(isCancelDraw)
    {
        pointslist.remove(QString("%1_%2").arg(ipcid).arg(currentPointKey));
        int intPiontsKey=pointsKey.toInt();
        intPiontsKey--;
        pointsKey=QString("%1").arg(intPiontsKey);
        ishighlight=false;
        isSaveLock=false;
        buttonCreateRule->setEnabled(true);
        buttonEditCurrentRule->setEnabled(false);
        buttonClearCurrentRule->setEnabled(false);
    }
    else
    {
        points.clear();
        points=pointslist[QString("%1_%2").arg(ipcid).arg(currentPointKey)];
        QVector<QPoint> pointstemp;
        pointstemp=points;
        double image_width, image_height;
        double r1, r2;
        image_width = PIC_WIDTH;
        image_height = PIC_HEIGHT;
        r1 = window_width_video / image_width;
        r2 = window_height_video / image_height;
        double r = qMin(r1, r2);

        for(int i=0;i<pointstemp.size();i++)
        {
            double newX=pointstemp[i].x()/r;
            double newY=pointstemp[i].y()/r;
            pointstemp[i].setX(newX);
            pointstemp[i].setY(newY);
        }
        points=pointstemp;

    }

    QString highlightNum=QString("%1_%2").arg(ipcid).arg(currentPointKey);
    QColor pointColor(0,0,135);
    QColor lineColor(0,255,0);
    QColor highlighLineColor(255,0,0);
    showPicture(pointColor,lineColor,highlighLineColor,highlightNum);
}

void ConfigWidget::slot_saveButton()
{
    saveRule(QString("%1_%2").arg(ipcid).arg(currentPointKey));
}

void ConfigWidget::slot_buttonEditCurrentRule()
{
    buttonCreateRule->setEnabled(false);
    buttonStartDraw->setEnabled(false);
    buttonRestLast->setEnabled(true);
    buttonRedo->setEnabled(true);
    buttonEndDraw->setEnabled(true);
    buttonEditCurrentRule->setEnabled(false);
    buttonClearCurrentRule->setEnabled(true);
    isDraw=true;
}

void ConfigWidget::slot_buttonClearCurrentRule()
{
    QMessageBox::StandardButton button;
    button = QMessageBox::question(this, tr("是否确定删除当前规则"),
                                   QString(tr("放弃请按\"No\",确认请按\"Yes\"")),
                                   QMessageBox::Yes | QMessageBox::No);
    if (button == QMessageBox::No) {
        return;
    }
    else if (button == QMessageBox::Yes) {
        pointslist.remove(QString("%1_%2").arg(ipcid).arg(currentPointKey));

        mRule rule;
        if(ALARM_MANAGER->HasRule(ipcid, currentPointKey, rule))
        {
            ALARM_MANAGER->RemoveRule(ipcid, currentPointKey);

            QString retCode="";
            DATA_TRANSLATE->deleteRule(rule,retCode);
            if(retCode=="999")
               emit  callLogin();
        }
    }

    buttonCreateRule->setEnabled(true);
    buttonStartDraw->setEnabled(false);
    buttonRestLast->setEnabled(false);
    buttonRedo->setEnabled(false);
    buttonEndDraw->setEnabled(false);
    buttonEditCurrentRule->setEnabled(false);
    buttonClearCurrentRule->setEnabled(false);
    QString highlightNum="";
    QColor pointColor(0,0,135);
    QColor lineColor(0,255,0);
    QColor highlighLineColor(255,0,0);
    showPicture(pointColor,lineColor,highlighLineColor,highlightNum);
    emit itemClicked(QString(""));
    isDraw=false;
    ishighlight=false;
    isSaveLock=false;
}

void ConfigWidget::slot_addtime()
{
    if(timeEdStart->time()>=timeEdStop->time())
    {
        QMessageBox::about(NULL, "温馨提示", "时间选择不正确，结束时间需大于开始时间。");
        return;
    }

    QString alarmNumDuringTime=lineEditAlarmNumDuringTime->text();
    QString time=QString("%1-%2-%3").arg(timeEdStart->time().toString("hh:mm:ss")).arg(timeEdStop->time().toString("hh:mm:ss")).arg(alarmNumDuringTime.toInt()+1);
    bool iscontains=false;
    for(int i=0;i<lvtAlarmTimes->count();i++)
    {
        QStringList timelist=lvtAlarmTimes->item(i)->text().split("-");
        QTime startTime=QTime::fromString(timelist[0],"hh:mm:ss");
        QTime stopTime=QTime::fromString(timelist[1],"hh:mm:ss");
        //时间段a->b ,c->d  ,(a-d)*(b-c)<0 则有重复
        double a=timeEdStart->time().secsTo(stopTime);
        double c=timeEdStop->time().secsTo(startTime);
        if (a*c<0)
        {

            iscontains=true;
        }
    }
    if(!iscontains)
        lvtAlarmTimes->addItem(time);
    else
        QMessageBox::about(NULL, "温馨提示", "您新增的时间段与之前时间段有重复，请更改！");
}

void ConfigWidget::slot_deltime()
{
    int row=lvtAlarmTimes->currentRow();
    lvtAlarmTimes->takeItem(row);
}

void ConfigWidget::setAttributeVisibelTrue()
{
    propertyEditor->setVisible(true);
    labAlarmTimes->setVisible(true);
    labelstar->setVisible(true);         //报警开始时间
    labelstop->setVisible(true);        //结束时间
    timeEdStart->setVisible(true);  //开始时间
    timeEdStop->setVisible(true);   //结束时间
    labelAlarmNumDuringTime->setVisible(true);
    lineEditAlarmNumDuringTime->setVisible(true);
    lvtAlarmTimes->setVisible(true);//时间条目
    addlist->setVisible(true);    //增加时间
    dellist->setVisible(true);   //删除时间
    buttonConfirm->setVisible(true);
}

void ConfigWidget::setAttributeVisibelFalse()
{
    propertyEditor->setVisible(false);
    labAlarmTimes->setVisible(false);
    labelstar->setVisible(false);         //报警开始时间
    labelstop->setVisible(false);        //结束时间
    timeEdStart->setVisible(false);  //开始时间
    timeEdStop->setVisible(false);   //结束时间
    labelAlarmNumDuringTime->setVisible(false);
    lineEditAlarmNumDuringTime->setVisible(false);
    lvtAlarmTimes->setVisible(false);//时间条目
    addlist->setVisible(false);    //增加时间
    dellist->setVisible(false);   //删除时间
    buttonConfirm->setVisible(false);
}

