#include "ffmpegdecode.h"
#include <qdatetime.h>
#include <QDebug>

ffmpegDecode :: ffmpegDecode(int width,int height)
{
    pAvFrame = NULL/**pFrameRGB = NULL*/;
    m_pFormatCtx  = NULL;
    pCodecCtx   = NULL;

    m_width=width;
    m_height=height;

    m_pCvMat = new cv::Mat();
    videoindex=0;
    img_convert_ctx = NULL;
    packet = NULL;

    m_isInit=false;
    m_keyframe=false;
}

ffmpegDecode :: ~ffmpegDecode()
{
    m_pCvMat->release();
    //释放本次读取的帧内存
    av_free_packet(packet);
    avcodec_close(pCodecCtx);
    avformat_close_input(&m_pFormatCtx);

}

void ffmpegDecode::close()
{
    if(m_pFormatCtx!=NULL)
        avformat_close_input(&m_pFormatCtx);

    if(pAvFrame!=NULL)
        av_frame_free(&pAvFrame);

}

void ffmpegDecode::Register()
{
    //ffmpeg注册复用器，编码器等的函数av_register_all()。
    //该函数在所有基于ffmpeg的应用程序中几乎都是第一个被调用的。只有调用了该函数，才能使用复用器，编码器等。
    //这里注册了所有的文件格式和编解码器的库，所以它们将被自动的使用在被打开的合适格式的文件上。注意你只需要调用 av_register_all()一次，因此我们在主函数main()中来调用它。如果你喜欢，也可以只注册特定的格式和编解码器，但是通常你没有必要这样做。
    av_register_all();
    avformat_network_init();
}

bool ffmpegDecode :: init(const QString &rtsp)
{
    close();

    //阻塞设置
    if(m_pFormatCtx!=NULL)
    {
        avformat_free_context(m_pFormatCtx);
        m_pFormatCtx=NULL;
    }
    m_pFormatCtx = avformat_alloc_context();

    AVDictionary *pOptions = NULL;
    av_dict_set(&pOptions, "stimeout", "3000000", 0);
    av_dict_set(&pOptions, "buffer_size", "102400", 0);
    av_dict_set(&pOptions, "rtsp_transport", "tcp", 0);


    //pFormatCtx = avformat_alloc_context();
    //打开视频文件,通过参数filepath来获得文件名。这个函数读取文件的头部并且把信息保存到我们给的AVFormatContext结构体中。
    //最后2个参数用来指定特殊的文件格式，缓冲大小和格式参数，但如果把它们设置为空NULL或者0，libavformat将自动检测这些参数。
    if(avformat_open_input(&m_pFormatCtx,rtsp.toStdString().c_str(),NULL, &pOptions)!=0)
    {
        if(pOptions!=NULL)
        {
            av_dict_free(&pOptions);
        }
        LOG_ERROR("无法打开文件:%s.\n", rtsp.toStdString().c_str());
        return false;
    }    

    if(pOptions!=NULL)
    {
        av_dict_free(&pOptions);
    }

    //查找文件的流信息,avformat_open_input函数只是检测了文件的头部，接着要检查在文件中的流的信息
    if(avformat_find_stream_info(m_pFormatCtx,NULL)<0)

    {        
        LOG_ERROR("Couldn't find stream information.\n");
        return false;
    }    

    if(openDecode() && prepare())
        m_isInit=true;
    else
        m_isInit=false;

    return m_isInit;
}

bool ffmpegDecode :: openDecode()
{
    //遍历文件的各个流，找到第一个视频流，并记录该流的编码信息
    videoindex = -1;
    for(int i=0; i<m_pFormatCtx->nb_streams; i++)
    {
        if(m_pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoindex=i;
            break;
        }
    }
    if(videoindex==-1)
    {        
        LOG_ERROR("Didn't find a video stream.\n");
        return false;
    }

    pCodecCtx=m_pFormatCtx->streams[videoindex]->codec;
    pCodecCtx->width=m_width;
    pCodecCtx->width=m_height;

    //在库里面查找支持该格式的解码器
    AVCodec *pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL)
    {        
        LOG_ERROR("Codec not found.\n");
        return false;
    }

    //打开解码器
    if(avcodec_open2(pCodecCtx, pCodec,NULL) < 0)
    {
        LOG_ERROR("Could not open codec.\n");
        return false;
    }

    return true;
}

bool ffmpegDecode :: prepare()
{
    //分配一个帧指针，指向解码后的原始帧
    if(pAvFrame!=NULL)
    {
        delete pAvFrame;
        pAvFrame=NULL;
    }
    pAvFrame=av_frame_alloc();

    int y_size = pCodecCtx->width * pCodecCtx->height;
    //分配帧内存
    if(packet!=NULL)
    {
        delete  packet;
        packet=NULL;
    }
    packet=(AVPacket *)av_malloc(sizeof(AVPacket));
    av_new_packet(packet, y_size);

    //输出一下信息-----------------------------
    //av_dump_format(m_pFormatCtx,0,m_filepath.c_str(),0);
    //av_dump_format只是个调试函数，输出文件的音、视频流的基本信息了，帧率、分辨率、音频采样等等

    return true;
}

bool ffmpegDecode :: readOneFrame()
{
    if(av_read_frame(m_pFormatCtx, packet) < 0)
    {
        LOG_ERROR("av_read_frame() error.\n");
        m_isInit=false;
        av_buffer_unref(&(packet->buf));
        av_free_packet(packet);
        return false;
    }

    if(packet->stream_index==videoindex)
    {
        int hasPicture=0;
        int ret = avcodec_decode_video2(pCodecCtx, pAvFrame, &hasPicture, packet);
        if(ret<0)
        {
            LOG_ERROR("avcodec_decode_video2() error.\n");
            m_isInit=false;
            av_buffer_unref(&(packet->buf));
            av_free_packet(packet);
            return false;
        }

        if(hasPicture && pAvFrame->key_frame)
        {
            //根据编码信息设置渲染格式
            if(img_convert_ctx == NULL)
            {
                img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
                                                 pCodecCtx->pix_fmt, m_width ,m_height,
                                                 AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
            }

            m_mutex.lock();
            m_keyframe=true;
            get(pCodecCtx, img_convert_ctx, pAvFrame);
            m_mutex.unlock();
        }
    }

    av_buffer_unref(&(packet->buf));
    av_free_packet(packet);

    return true;
}

bool ffmpegDecode::getOneFrame(cv::Mat &mat)
{
    if(m_isInit && m_keyframe)
    {
        m_mutex.lock();
        m_keyframe=false;
        m_pCvMat->copyTo(mat);
        m_mutex.unlock();

        return true;
    }

    return false;
}

void ffmpegDecode :: get(AVCodecContext    * pCodecCtx, SwsContext * img_convert_ctx, AVFrame * pFrame)
{
    if (m_pCvMat->empty())
    {
        m_pCvMat->create(cv::Size(m_width, m_height),CV_8UC3);
    }

    AVFrame    *pFrameRGB = NULL;
    uint8_t  *out_bufferRGB = NULL;
    pFrameRGB = av_frame_alloc();

    //给pFrameRGB帧加上分配的内存;
    int size = avpicture_get_size(AV_PIX_FMT_BGR24, m_width, m_height);
    out_bufferRGB = new uint8_t[size];
    avpicture_fill((AVPicture *)pFrameRGB, out_bufferRGB, AV_PIX_FMT_BGR24, m_width, m_height);

    //YUV to RGB
    sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

    memcpy(m_pCvMat->data,out_bufferRGB,size);

    delete[] out_bufferRGB;
    av_free(pFrameRGB);
}
