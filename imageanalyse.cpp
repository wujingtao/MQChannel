#include "imageanalyse.h"
#include "NetWorkManager.h"
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include "QDomElement"
#include <QProcess>
#include "imagedata.h"
using namespace std;
using namespace cv;

#include "AlarmManager.h"


imageAnalyse::imageAnalyse(QString cameraID, const QString &savePath)
{
    m_cameraID=cameraID;
    m_savepath=savePath;
    m_alarmRecords.clear();

    QDateTime nowtime = QDateTime::currentDateTime();
    m_delTime=nowtime;
    m_collectTime=nowtime;
}

void imageAnalyse::analyse(cv::Mat &inputimage, const tagDetectAll &detectAll)
{
    if(inputimage.empty())
    {
        LOG_WARN("inputimage empty.\n");
        return;
    }

     //QVector<QRect> points = points_Score.nodelist;
     //QVector<float> scores = points_Score.scores;

     //定时保存图像
     QStringList nodelist;

     //1.收集圖像
     QDateTime nowtime =  QDateTime::currentDateTime();
     vector<uchar> vecbuf;
     if(m_collectTime.secsTo(nowtime)>=COLLECT_TIME_SECOND)
     {
         m_collectTime=nowtime;
         cv::imencode(".jpg", inputimage, vecbuf);

         collectImage(vecbuf, inputimage.cols, inputimage.rows, nowtime, detectAll);
     }
     //2.畫圖像
     xmlToMat(inputimage, detectAll);

     //3.收集已處理
     QString filefullpath = m_savepath +"/dealed/"+nowtime.toString("yyyyMMddhhmmsszzz")+".jpg";
     dealedImage(inputimage, filefullpath, nowtime);

    //*******第三步，分析图片***********************//
    ALARM_MANAGER->Alarm(filefullpath, m_cameraID, detectAll);
}

void imageAnalyse::collectImage(const vector<uchar> &vecbuf, int width,int height,const QDateTime &nowtime, const tagDetectAll &detectAll)
{
    QString collect = m_savepath+"/collect";
    QDir dir(collect);
    if(!dir.exists())
    {
        dir.mkdir(collect);
    }

    QString collectPath = collect+"/"+nowtime.toString("yyyyMMdd");

    //删除3天前当天文件夹
    QString timeBefore=nowtime.addDays(-4).toString("yyyyMMdd");
    QString collectPathToDel= m_savepath+"/collect/"+timeBefore;
    QDir dirtoDelete(collectPathToDel);
    if(dirtoDelete.exists()) dirtoDelete.removeRecursively();

    //存储路径
    QDir dircollect(collectPath);
    if(!dircollect.exists())
    {
        dircollect.mkdir(collectPath);
    }   

    QString strTime=nowtime.toString("yyyyMMddhhmmss");
    QString picName=strTime+".jpg";
    QString xmlName=collectPath +"/"+strTime+".xml";
    QString imageName=collectPath+"/"+picName;

    QString autoWriteXML=QString("%1$$%2$$%3$$%4").arg(picName).arg(width).arg(height).arg(xmlName);
    writexml(autoWriteXML, detectAll);

    //保存到文件
    ofstream ofs(imageName.toStdString().c_str(), ofstream::binary);
    if(ofs.is_open())
    {
        ofs.write((char*)&vecbuf[0], vecbuf.size());
    }
    ofs.close();
}

////循环删除图片，避免磁盘空间被占满
void imageAnalyse::deleteImagesCircle(QString path, const QDateTime &nowTime)
{
    if(m_delTime.secsTo(nowTime) < COLLECT_DEL_SECOND) return;

    QDir dirtodeal(path);
    dirtodeal.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dirtodeal.setSorting(QDir::Time | QDir::Reversed);

    QFileInfoList list = dirtodeal.entryInfoList();
    if(list.size()<500) return;
    for(int i=0;i<list.size();i++)
    {
        QDateTime createdtime = list[i].created();       //文件创建时间
        if(createdtime.secsTo(nowTime)>COLLECT_KEEP_SECOND)
        {
            QFile file(list[i].absoluteFilePath());
            file.remove();
        }
    }
}

//定时保存图片供后续训练
void imageAnalyse::writexml(QString autoWriteXML, const tagDetectAll &detectAll)
{
    QStringList listname_w_h=autoWriteXML.split("$$");
    QString fileName=listname_w_h[0];
    QString widtha=listname_w_h[1];
    QString heighta=listname_w_h[2];
    QString filepath=listname_w_h[3];
    int loc=filepath.lastIndexOf("/");
    QString xmlpath=filepath.left(loc);
    QDir dir(xmlpath);
    if(!dir.exists()) dir.mkdir(xmlpath);
    QFile file(filepath);
    if (!file.exists()) {
        file.open( QIODevice::WriteOnly );
        file.close();
    }

    QDomDocument domDocument;
    //  QDomElement tree      = domDocument.documentElement();

    //在firstitem上添加一个节点
    QDomElement annotationItem = domDocument.createElement("annotation");

    QDomElement folderItem = domDocument.createElement("folder");
    folderItem.appendChild(domDocument.createTextNode("Unknown"));

    QDomElement filename = domDocument.createElement("filename");
    filename.appendChild(domDocument.createTextNode(fileName));

    QDomElement source = domDocument.createElement("source");
    QDomElement database=domDocument.createElement("database");
    database.appendChild(domDocument.createTextNode("Unknown"));
    source.appendChild(database);
    QDomElement sizea = domDocument.createElement("size");
    QDomElement width = domDocument.createElement("width");
    width.appendChild(domDocument.createTextNode(widtha));
    QDomElement height = domDocument.createElement("height");
    height.appendChild(domDocument.createTextNode(heighta));
    QDomElement depth = domDocument.createElement("depth");
    depth.appendChild(domDocument.createTextNode("3"));
    sizea.appendChild(width);
    sizea.appendChild(height);
    sizea.appendChild(depth);

    QDomElement segmented = domDocument.createElement("source");
    segmented.appendChild(domDocument.createTextNode("0"));

    annotationItem.appendChild(folderItem);
    annotationItem.appendChild(filename);
    annotationItem.appendChild(source);
    annotationItem.appendChild(sizea);
    annotationItem.appendChild(segmented);

    for(int i=0; i<detectAll.heads.size(); i++)
    {
        QRect rect=detectAll.heads[i].rect;
        QDomElement element = xmlElement(domDocument, "head", rect.x(), rect.y(), rect.x()+rect.width(), rect.y()+rect.height());
        annotationItem.appendChild(element);
    }

    for(int i=0; i<detectAll.bodys.size(); i++)
    {
        QRect rect=detectAll.bodys[i].rect;
        QDomElement element = xmlElement(domDocument, "prisoner", rect.x(), rect.y(), rect.x()+rect.width(), rect.y()+rect.height());
        annotationItem.appendChild(element);
    }

    for(int i=0; i<detectAll.polices.size(); i++)
    {
        QRect rect=detectAll.polices[i].rect;
        QDomElement element = xmlElement(domDocument, "police", rect.x(), rect.y(), rect.x()+rect.width(), rect.y()+rect.height());
        annotationItem.appendChild(element);
    }

    //添加元素节点到父节点
    domDocument.appendChild(annotationItem);
    file.close();

    QFile fileModify(filepath);
    if (!fileModify.open(QFile::WriteOnly | QFile::Text)){
        return ;
    }

    QTextStream out(&fileModify);
    domDocument.save(out,3);
    fileModify.close();
    return ;
}

QDomElement imageAnalyse::xmlElement(QDomDocument &domDocument, const QString &element, int minx, int miny, int maxx, int maxy)
{
    QDomElement objecta = domDocument.createElement("object");
    QDomElement name = domDocument.createElement("name");
    name.appendChild(domDocument.createTextNode(element));

    QDomElement pose = domDocument.createElement("pose");
    pose.appendChild(domDocument.createTextNode("Unspecified"));
    QDomElement truncated = domDocument.createElement("truncated");
    truncated.appendChild(domDocument.createTextNode("0"));
    QDomElement difficult = domDocument.createElement("difficult");
    difficult.appendChild(domDocument.createTextNode("0"));

    QDomElement bndbox = domDocument.createElement("bndbox");

    QDomElement xmin = domDocument.createElement("minx");
    xmin.appendChild(domDocument.createTextNode(QString("%1").arg(minx)));
    QDomElement ymin = domDocument.createElement("ymin");
    ymin.appendChild(domDocument.createTextNode(QString("%1").arg(miny)));
    QDomElement xmax = domDocument.createElement("xmax");
    xmax.appendChild(domDocument.createTextNode(QString("%1").arg(maxx)));
    QDomElement ymax = domDocument.createElement("ymax");
    ymax.appendChild(domDocument.createTextNode(QString("%1").arg(maxy)));
    bndbox.appendChild(xmin);
    bndbox.appendChild(ymin);
    bndbox.appendChild(xmax);
    bndbox.appendChild(ymax);

    objecta.appendChild(name);
    objecta.appendChild(pose);
    objecta.appendChild(truncated);
    objecta.appendChild(difficult);
    objecta.appendChild(bndbox);

    return objecta;
}

bool imageAnalyse::saveimage(cv::Mat &image, string filename)
{
    if (image.empty()) return false;
    const char *filenamechar = filename.c_str();
    FILE *fpw = fopen(filenamechar, "wb");//如果没有则创建，如果存在则从头开始写
    if (fpw == NULL)
    {
        fclose(fpw);
        return false;
    }
    int channl = image.channels();//第一个字节  通道
    int rows = image.rows;     //四个字节存 行数
    int cols = image.cols;   //四个字节存 列数

    fwrite(&channl, sizeof(char), 1, fpw);
    fwrite(&rows, sizeof(char), 4, fpw);
    fwrite(&cols, sizeof(char), 4, fpw);
    char* dp = (char*)image.data;
    if (channl == 3)
    {
        for (int i = 0; i < rows*cols; i++)
        {
            fwrite(&dp[i * 3], sizeof(char), 1, fpw);
            fwrite(&dp[i * 3 + 1], sizeof(char), 1, fpw);
            fwrite(&dp[i * 3 + 2], sizeof(char), 1, fpw);
        }
    }
    else if (channl == 1)
    {
        for (int i = 0; i < rows*cols; i++)
        {
            fwrite(&dp[i], sizeof(char), 1, fpw);
        }
    }
    fclose(fpw);
    return true;
}

void imageAnalyse::xmlToMat(cv::Mat &mat, const tagDetectAll &detectAll)
{
    //1.head
    for(int i=0; i<detectAll.heads.size(); i++)
    {
        drawBox("head", detectAll.heads[i], mat);
    }
    //2.
    for(int i=0; i<detectAll.bodys.size(); i++)
    {
        drawBox("prisoner", detectAll.bodys[i], mat);
    }
    //3.
    for(int i=0; i<detectAll.polices.size(); i++)
    {
        drawBox("police", detectAll.polices[i], mat);
    }
}

void imageAnalyse::drawBox(const QString &txtFlag, const tagCaffeObject &caffeObj, cv::Mat &mat)
{
    QRect rect=caffeObj.rect;
    float score=caffeObj.score;

    //图上画框
    Point pt1 = Point(rect.x() + rect.width()/2, rect.y() + rect.height()/2);
    circle(mat, pt1, 1, Scalar(CV_RGB(0,0,255)),2,2);//描第一个点

    if(DRAW_BOX)
    {
        cv::rectangle(mat, cv::Point(rect.x(),rect.y()),
                      cv::Point(rect.x() + rect.width(), rect.y() + rect.height()),
                      cv::Scalar(0,0,255),2);

        QString txt_score = QString::number(score, 'f', 3);
        QString txt=txtFlag + txt_score;
        putText(mat, txt.toStdString(), Point(rect.x(), rect.y() - 5),CV_FONT_HERSHEY_SIMPLEX,0.36,Scalar(0,0,255));
    }
}

void imageAnalyse::dealedImage(const cv::Mat &mat, const QString &filefullpath, const QDateTime &nowtime)
{
    QString dealedpath = m_savepath+"/dealed";
    QDir dealeddir(dealedpath);
    if(!dealeddir.exists())
    {
        dealeddir.mkdir(dealedpath);
    }
    deleteImagesCircle(dealedpath, nowtime);

        //cv::imwrite(filefullpath.toStdString(), inputimage);
        vector<uchar> vecbuf;
        cv::imencode(".jpg", mat, vecbuf);

        IMAGEDATA_INSTANCE->PushCaffeImage((char*)&vecbuf[0], vecbuf.size(), m_cameraID);

        //保存到文件
        ofstream ofs(filefullpath.toStdString().c_str(), ofstream::binary);
        if(ofs.is_open())
        {
            ofs.write((char*)&vecbuf[0], vecbuf.size());
        }
        ofs.close();
}
