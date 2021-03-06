#include "RaspberryCapture.h"
#include "RaspberryPixelFormat.h"
#include <iostream>
#include <linux/fb.h>


namespace Raspberry{

RaspberryCapture::RaspberryCapture(const char* dev)
{
    RecvicedTriggerSignal = false;
    m_callback = NULL;
    m_mode = 0;
    memcpy(&m_camera.devicename, dev, strlen(dev));
    OpenVideo(dev);

    mThread = NULL;
    isThreadTerminate = false;
    isClassTerminate    = false;
    LoopThreadStart ();
}

RaspberryCapture::~RaspberryCapture()
{
    LoopThreadStop ();

    isClassTerminate    = true;
    CloseVideo();
}

static int xioctl(int fh, int request, void *arg)
{
    int r;
    do{
        r = ioctl(fh, request, arg);
    }while( r == -1 && ((errno == EINTR) || (errno == EAGAIN)));

    if( r == -1)
    {
        fprintf(stderr, "error: %d, %s\n", errno, strerror(errno));
    }
    return r;
}



int cam_capabilitycaps(int fd)
{

    struct v4l2_capability cap;//printf("cam_capabilitycaps\n");
    xioctl(fd, VIDIOC_QUERYCAP, &cap);
    printf("cap.driver: \"%s\"\n", cap.driver);
    printf("cap.card: \"%s\"\n", cap.card);
    printf("cap.bus_info: \"%s\"\n", cap.bus_info);
    printf("cap.version: %08X\n", cap.version);
    printf("cap.capabilities=0x%08X\n", cap.capabilities);

    if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
        printf("- VIDEO_CAPTURE\n");

    if (cap.capabilities & V4L2_CAP_VIDEO_OUTPUT)
        printf("- VIDEO_OUTPUT");

    if (cap.capabilities & V4L2_CAP_VIDEO_OVERLAY)
        printf("- VIDEO_OVERLAY");

    if (cap.capabilities & V4L2_CAP_VBI_CAPTURE)
        printf("- VBI_CAPTURE");

    if (cap.capabilities & V4L2_CAP_VBI_OUTPUT)
        printf("- VBI_OUTPUT");

    if (cap.capabilities & V4L2_CAP_RDS_CAPTURE)
        printf("- RDS_CAPTURE");

    if (cap.capabilities & V4L2_CAP_TUNER)
        printf("- TUNER");

    if (cap.capabilities & V4L2_CAP_AUDIO)
        printf("- AUDIO");

    if (cap.capabilities & V4L2_CAP_READWRITE)
        printf("- READWRITE");

    if (cap.capabilities & V4L2_CAP_ASYNCIO)
        printf("- ASYNCIO");

    if (cap.capabilities & V4L2_CAP_STREAMING)
        printf("- STREAMING\n");

    if (cap.capabilities & V4L2_CAP_TIMEPERFRAME)
        printf("- TIMEPERFRAME");

    if (!cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
        printf("Device does not support capturing.");
        return -1;

    }

    return 0;
}


void RaspberryCapture::QuerySensorSupportPictureFormat()
{
    int ret;
    struct v4l2_fmtdesc fmtdesc;
    struct v4l2_frmsizeenum frmsize;
    bzero(&fmtdesc, sizeof(fmtdesc));
    int i;
    for(i = 0;  ;i++)
    {
        printf("check.. %d\n", i);
        fmtdesc.index = i;
        fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ret = ioctl(Camfd, VIDIOC_ENUM_FMT, &fmtdesc);
        if(ret == -1)
            break;
        printf("fmtdesc.flags = %d\n",          fmtdesc.flags);
        printf("fmtdesc.description = %s\n", fmtdesc.description);
        printf("fmtdesc.pixelformat = %d\n", fmtdesc.pixelformat);

        frmsize.pixel_format = fmtdesc.pixelformat;
        frmsize.index = 0;
        while(ioctl(Camfd, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0 )
        {
            if(frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE)
            {
            }else if(frmsize.type == V4L2_FRMSIZE_TYPE_STEPWISE){
            }
            printf("line:%d %d x %d\n", __LINE__, frmsize.discrete.width, frmsize.discrete.height);
            frmsize.index++;
        }
    }
}


void RaspberryCapture::QueryFormat()
{
    bzero(&m_camera.format, sizeof(m_camera.format));
    m_camera.format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(Camfd, VIDIOC_G_FMT, &m_camera.format);
#if 0
    printf("format.fmt.pix.width:%d\n",  m_camera.format.fmt.pix.width  );
    printf("format.fmt.pix.height:%d\n",  m_camera.format.fmt.pix.height  );
    printf("format.fmt.pix.pixelformat:%d\n",  m_camera.format.fmt.pix.pixelformat  );
    printf("format.fmt.pix.field:%d\n",  m_camera.format.fmt.pix.field  );   //是否行扫描
    printf("format.fmt.pix.bytesperline:%d\n",  m_camera.format.fmt.pix.bytesperline  ); //每行byte数
    printf("format.fmt.pix.sizeimage:%d\n",  m_camera.format.fmt.pix.sizeimage  ); //总共byte数
#endif

    m_camera.format.fmt.pix.width = 1296;
    m_camera.format.fmt.pix.height = 972;
  //  format.fmt.pix.pixelformat = 1195724874;//IPU_PIX_FMT_RGB24;
    m_camera.pixelformat = IPU_PIX_FMT_JPEG;
    m_camera.pixelformat = IPU_PIX_FMT_RGB24;
    m_camera.pixelformat = IPU_PIX_FMT_BGR24;
    m_camera.pixelformat = IPU_PIX_FMT_H264 ;
    m_camera.format.fmt.pix.pixelformat = m_camera.pixelformat;
    m_camera.pixelbytes = bytes_per_pixel(m_camera.pixelformat);
    m_camera.format.fmt.pix.sizeimage = m_camera.format.fmt.pix.width * m_camera.format.fmt.pix.height * m_camera.pixelbytes;
  //  format.fmt.pix.field = V4L2_FIELD_NONE;
    m_camera.format.fmt.pix.field = V4L2_FIELD_NONE;
    m_camera.format.fmt.pix.bytesperline = m_camera.format.fmt.pix.width *m_camera.pixelbytes;
    if (ioctl(Camfd, VIDIOC_S_FMT, &m_camera.format) < 0)
    {
        printf("error[%d]:VIDIOC_S_FMT\n", __LINE__);
    }
    ioctl(Camfd, VIDIOC_G_FMT, &m_camera.format);

#if 0
    printf("format.fmt.pix.width:%d\n",  m_camera.format.fmt.pix.width  );
    printf("format.fmt.pix.height:%d\n",  m_camera.format.fmt.pix.height  );
    printf("format.fmt.pix.pixelformat:%d\n",  m_camera.format.fmt.pix.pixelformat  );
    printf("format.fmt.pix.field:%d\n",  m_camera.format.fmt.pix.field  );   //是否行扫描
    printf("format.fmt.pix.bytesperline:%d\n",  m_camera.format.fmt.pix.bytesperline  ); //每行byte数
    printf("format.fmt.pix.sizeimage:%d\n",  m_camera.format.fmt.pix.sizeimage  ); //总共byte数
#endif

}

void RaspberryCapture::QueryControl()
{
    int ret;
    struct v4l2_control ctrl;
    ctrl.id = V4L2_CID_EXPOSURE_AUTO;
    ret = ioctl(Camfd, VIDIOC_G_CTRL, &ctrl);
    if(ret != -1)    printf("V4L2_CID_EXPOSURE_AUTO:%d\n", ctrl.value);

    ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
    ret = ioctl(Camfd, VIDIOC_G_CTRL, &ctrl);
    if(ret != -1) printf("V4L2_CID_EXPOSURE_ABSOLUTE:%d\n", ctrl.value);

    ctrl.id = V4L2_CID_GAMMA;
    ret = ioctl(Camfd, VIDIOC_G_CTRL, &ctrl);
    if(ret != -1)  printf("V4L2_CID_GAMMA:%d\n", ctrl.value);

    ctrl.id = V4L2_CID_GAIN;
    ret = ioctl(Camfd, VIDIOC_G_CTRL, &ctrl);
    if(ret != -1) printf("V4L2_CID_GAIN:%d\n", ctrl.value);
}

void RaspberryCapture::Query_QueryCtrl()
{
    int ret;
    struct v4l2_queryctrl queryctl;
    queryctl.id = V4L2_CID_GAIN;
    ret = ioctl(Camfd, VIDIOC_QUERYCTRL, &queryctl);
    if(ret != -1) printf("V4L2_CID_GAIN:name(%s) , minimum(%d), maximun(%d), step(%d), defalut_value(%d) , flags(%d)\n", queryctl.name, queryctl.minimum , queryctl.maximum, queryctl.step, queryctl.default_value, queryctl.flags);

    queryctl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
    ret = ioctl(Camfd, VIDIOC_QUERYCTRL, &queryctl);
    if(ret != -1) printf("V4L2_CID_EXPOSURE_ABSOLUTE:name(%s) , minimum(%d), maximun(%d), step(%d), defalut_value(%d) , flags(%d)\n", queryctl.name, queryctl.minimum , queryctl.maximum, queryctl.step, queryctl.default_value, queryctl.flags);

    queryctl.id = V4L2_CID_EXPOSURE;
    ret = ioctl(Camfd, VIDIOC_QUERYCTRL, &queryctl);
    if(ret != -1) printf("V4L2_CID_EXPOSURE:name(%s) , minimum(%d), maximun(%d), step(%d), defalut_value(%d) , flags(%d)\n", queryctl.name, queryctl.minimum , queryctl.maximum, queryctl.step, queryctl.default_value, queryctl.flags);
}

int RaspberryCapture::OpenVideo(const char* videoDev)
{

    int ret;
    Camfd = open(videoDev, O_RDWR | O_NONBLOCK);
    if(Camfd < 0 )
    {
        printf("no find this dev:%s\n", videoDev);
        exit(-1);
    }
    m_camera.camfd = Camfd;
    int i;

    //
    struct v4l2_streamparm parm;
    bzero(&parm, sizeof(parm));
    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(Camfd, VIDIOC_G_PARM, &parm);
    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    parm.parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
    parm.parm.capture.timeperframe.numerator = 1;
    parm.parm.capture.timeperframe.denominator = 30; //设置帧数
   // parm.parm.capture.capturemode = V4L2_MODE_HIGHQUALITY;
    parm.parm.capture.capturemode = 0;
    parm.parm.capture.extendedmode = 0;
    if(ioctl(Camfd, VIDIOC_S_PARM, &parm) == -1)
    {
        printf("error[%d] :VIDIOC_S_PARM..........\n", __LINE__);
        exit(-1);
    }


    struct v4l2_input input;
    bzero(&input, sizeof(input));
    if(ioctl(Camfd, VIDIOC_G_INPUT, &input.index) == -1)
    { //查询当前设备
        printf("error[%d] :VIDIOC_G_INPUT..........\n", __LINE__);
        exit(-1);
    }else {
        if(ioctl(Camfd, VIDIOC_ENUMINPUT, &input) == -1)
        {//获取设备名
            printf("error[%d] :VIDIOC_G_INPUT..........\n", __LINE__);
            exit(-1);
        }else{
            printf("Current input:%s\n", input.name);
        }
    }

    int index = 0;
    if(ioctl(Camfd, VIDIOC_S_INPUT, &index) == -1)
    {//切换到第一个设备
        printf("error[%d] :VIDIOC_G_INPUT..........\n", __LINE__);
        exit(-1);
    }

    //设置图片格
//    QueryFormat();

    bzero(&m_camera.format, sizeof(m_camera.format));
    m_camera.format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(Camfd, VIDIOC_G_FMT, &m_camera.format);

    m_camera.format.fmt.pix.width = 1920;
    m_camera.format.fmt.pix.height = 1080;
    //  format.fmt.pix.pixelformat = 1195724874;//IPU_PIX_FMT_RGB24;
    uint32_t  pixelformat;
    pixelformat = IPU_PIX_FMT_BGR24;
    pixelformat =IPU_PIX_FMT_BGR32 ;
    pixelformat = IPU_PIX_FMT_JPEG;
    pixelformat = IPU_PIX_FMT_RGB24;
    pixelformat = IPU_PIX_FMT_H264 ;
    m_camera.format.fmt.pix.pixelformat = pixelformat;
    m_camera.format.fmt.pix.sizeimage = m_camera.format.fmt.pix.width * m_camera.format.fmt.pix.height * bytes_per_pixel(pixelformat);
    //  format.fmt.pix.field = V4L2_FIELD_NONE;
    m_camera.format.fmt.pix.field = V4L2_FIELD_NONE;
    m_camera.format.fmt.pix.bytesperline = m_camera.format.fmt.pix.width *bytes_per_pixel (pixelformat);
    if (ioctl(Camfd, VIDIOC_S_FMT, &m_camera.format) < 0)
    {
        printf("error[%d]:VIDIOC_S_FMT\n", __LINE__);
    }
    ioctl(Camfd, VIDIOC_G_FMT, &m_camera.format);

   // _setdefaultparams();

    _init_mmap ();

    struct v4l2_cropcap cropcap;
    bzero(&cropcap, sizeof(cropcap));
    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
#if 0
    if((ioctl(Camfd, VIDIOC_CROPCAP, &cropcap) == -1) && errno != EINVAL)
    {
        printf("error[%d] :VIDIOC_CROPCAP.........  \n", __LINE__);
        perror("");
    }
#endif

#if 0
    //貌似还不支持这个操作
    //从原始图片中裁剪出矩形
    struct v4l2_crop crop;
    bzero(&crop, sizeof(crop));
    crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    crop.c.width =  720;
    crop.c.height = 972;
    crop.c.top = 0;
    crop.c.left = 0;
    if( (ioctl(Camfd, VIDIOC_S_CROP, &crop) < 0)  )
    {
        printf("error[%d] :VIDIOC_S_CROP..........\n", __LINE__);
        perror("");
        exit(0);
    }
#endif
    return ret;
}

bool RaspberryCapture::HasAvailableData()
{
    for(;;)
    {
        fd_set fds;
        struct timeval tv;
        FD_ZERO(&fds);
        FD_SET(Camfd, &fds);

        tv.tv_sec = 2;
        tv.tv_usec = 0;

        int ret = select (Camfd+1, &fds, NULL, NULL, &tv);
        if(ret == -1)
        {
            printf("error[%d]:select\n", __LINE__);
            continue;
        }
        else if( ret == 0)
        {
            printf("warn[%d]:select time out\n", __LINE__);
            continue;
        }else{
            return true;
        }
    }
    return false;
}

int RaspberryCapture::AllocaFrame(uint8_t **outbuff, int *outlen)
{
    struct v4l2_buffer buffer;
    struct v4l2_format fmt;
    bzero(&buffer, sizeof(buffer));
    bzero(&fmt, sizeof(fmt));

    fmt.type   =  V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl (Camfd, VIDIOC_G_FMT, &fmt) < 0 )
    {
        printf("error:VIDIOC_G_FMT.\n");
        return -1;
    }

    buffer.type         = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buffer.memory  = V4L2_MEMORY_MMAP;
    xioctl(Camfd, VIDIOC_DQBUF, &buffer) ;

    printf("index:%d, timestamp:%d, decodetime:%d , squeue:%d\n", buffer.index, buffer.timestamp, buffer.timecode, buffer.sequence);
    BuffCurrentIndex = buffer.index;
    *outbuff = (uint8_t*)buffdata[buffer.index];
    *outlen  = fmt.fmt.pix.sizeimage;
    return 0;
}

int RaspberryCapture::ReleaseFrame()
{
    struct v4l2_buffer buffer;
    bzero(&buffer, sizeof(buffer));

    buffer.type         = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buffer.memory  = V4L2_MEMORY_MMAP;
    buffer.index       =   BuffCurrentIndex;
    if(ioctl(Camfd, VIDIOC_QBUF, &buffer))
    {
        printf("error: VIDIOC_QBUF!.");
    }
    return 0;
}

void RaspberryCapture::RegisterImageCallback(FHandler* callback)
{
    mMutex.lock ();
    if (callback == NULL && m_callback == NULL)
    {

    }else if(callback ==NULL && m_callback != NULL)
    {
        m_callback = NULL;
        m_mode = 0;
    }else if(callback != NULL && m_callback == NULL)
    {
        std::cout<<"切换到  callback mode"<<std::endl;
        m_callback =  callback;
        m_mode = 1;
    }else if(callback != NULL && m_callback  != NULL)
    {
        std::cout<<"更换  callback 函数"<<std::endl;
        m_callback =  callback;
        m_mode = 1;
    }
    mMutex.unlock ();
}


uint32_t RaspberryCapture::GrabPicture(uint8_t* imagebuff, uint32_t imagelen)
{
    if( m_mode ==1&&  m_callback != NULL )
        RegisterImageCallback (NULL);

    if (m_mode != 2)
    {
        std::cout<<"切换到 GrabPicture Mode"<<std::endl;
        m_mode = 2;
    }
    int count = -1;

    std::cout<<__LINE__<<":"<<__FUNCTION__<<std::endl;
    m_GrabPictureMutex.lock ();
    if(HasAvailableData ())
    {
        uint8_t *buff;
        int         len;
        AllocaFrame (&buff, &len );
        count = (len<=imagelen?len:imagelen);
        memcpy(imagebuff, buff,  count);
        ReleaseFrame ();
    }
    m_GrabPictureMutex.unlock ();
    return count;
}


int RaspberryCapture::GrabPicture(uint8_t** imagebuff, uint32_t* imagelen)
{
    struct v4l2_buffer buffer={0};
    int i;
    for(i = 0 ; i< 3 ;i++)
    {

        //释放缓存，放入队列
        buffer.type         = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer.memory  = V4L2_MEMORY_MMAP;
        buffer.index       =   0;
        if(ioctl(Camfd, VIDIOC_QBUF, &buffer))
        {
            printf("error: VIDIOC_QBUF!.");
            xioctl(Camfd, VIDIOC_DQBUF, &buffer) ;
            return -1;
        }

        //监听数据
        for(;;)
        {
            fd_set fds;
            struct timeval tv;
            FD_ZERO(&fds);
            FD_SET(Camfd, &fds);
            tv.tv_sec = 2;
            tv.tv_usec = 0;
            int ret = select (Camfd+1, &fds, NULL, NULL, &tv);
            if(ret > 0)
                break;
        }


        //申请缓存，取出队列
        bzero(&buffer, sizeof(buffer));
        buffer.type         = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer.memory  = V4L2_MEMORY_MMAP;
        xioctl(Camfd, VIDIOC_DQBUF, &buffer) ;

        struct v4l2_format fmt={0};
        fmt.type   =  V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl (Camfd, VIDIOC_G_FMT, &fmt) < 0 )
            printf("error:VIDIOC_G_FMT.\n");

        *imagebuff = (uint8_t*)buffdata[buffer.index];
        *imagelen = buffer.length;
        printf("sizeimage:%d , bytesused:%d, lenght:%d, timestamp:%d ", *imagelen, buffer.bytesused, buffer.length, buffer.timestamp);
        printf("field:%d ", buffer.field);
        printf("\n");
    }
    return 0;


}






void RaspberryCapture::LoopThreadPicture(RaspberryCapture* Rcapture)
{
    while(!Rcapture->isClassTerminate && !Rcapture->isThreadTerminate)
    {
        uint8_t *imagebuff;
        int          imagelen;
        if(Rcapture->m_mode == 1 &&  Rcapture->HasAvailableData () )
        {
            Rcapture->AllocaFrame (&imagebuff, &imagelen);
            //假如是外触发模式则等待读取
            if(RaspberryCapture::RecvicedTriggerSignal)
            {
                RaspberryCapture::RecvicedTriggerSignal = false;
                Rcapture->mMutex.lock ();
                if(Rcapture->m_callback  != NULL)
                {
                    TImageType info;
                    info.imagelen = imagelen;
                    info.imagebuff = imagebuff;
                    info.width  = Rcapture->m_camera.format.fmt.pix.width;
                    info.height  = Rcapture->m_camera.format.fmt.pix.height;
                    Rcapture->m_callback(&info);
                }
                Rcapture->mMutex.unlock ();
            }
            Rcapture->ReleaseFrame ();
        }else{
            usleep(1000);
        } //if (Rcapture->HasAvailableData())
    } //while
}

void RaspberryCapture::LoopThreadStart()
{
    isThreadTerminate = false;
    if( mThread == NULL && !isClassTerminate )
    {
        mThread = new std::thread(LoopThreadPicture , this);
    }
}

void RaspberryCapture::LoopThreadStop()
{
    isThreadTerminate = true;
    if(mThread != NULL)
    {
        mThread->join ();
        mThread = NULL;
    }
}



void RaspberryCapture::_init_mmap()
{
    uint32_t i;
    int ret;
    enum v4l2_buf_type buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    //分配缓冲区,mmap
    struct v4l2_requestbuffers req = {0};
    req.count =  PICTURE_BUFFNUM; //缓冲区个数, memory = V4L2_MEMORY_MMAP 有效
    req.memory = V4L2_MEMORY_MMAP;
    req.type = buf_type;
    ret = ioctl(Camfd, VIDIOC_REQBUFS, &req);
    if(ret == -1){
        printf("error :VIDIOC_REQBUFS..........\n");
        exit(-1);
    }


    if (req.count != PICTURE_BUFFNUM)
    {
        std::cout<<req.count <<":::::"<<PICTURE_BUFFNUM<<std::endl;
        printf("Error  PICTURE_BUFFNUM\n");
    }

    //查询buff状态
    struct v4l2_buffer buffers;
    for(i = 0 ; i < PICTURE_BUFFNUM ; i++)
    {
        bzero(&buffers, sizeof(buffers));
        buffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffers.memory = V4L2_MEMORY_MMAP;
        buffers.index = i;
        ret = ioctl(Camfd, VIDIOC_QUERYBUF, &buffers);
        if(ret == -1)
            printf("error :VIDIOC_QUERYBUF.\n");

        bufflength[i] = buffers.length;
        buffdata[i] = (char*)mmap(NULL, buffers.length, PROT_READ|PROT_WRITE, MAP_SHARED, Camfd, buffers.m.offset);
        if(buffdata[i] == MAP_FAILED)
            printf("mmap failed!...\n");
    }
    //设置缓冲区加入队列
    for(i = 0; i < PICTURE_BUFFNUM;i++)
    {
        bzero(&buffers, sizeof(buffers));
        buffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffers.memory = V4L2_MEMORY_MMAP;
        buffers.index = i;
        ioctl(Camfd, VIDIOC_QBUF, &buffers);
        if(ret == -1)
            printf("%d:VIDIOC_QBUF.\n",__LINE__);
    }

    //开启采集
    CaptureON();

}

void RaspberryCapture::_uninit_mmap()
{
    //关闭采集
    CaptureOFF();
    uint32_t i;
    int ret;
    enum v4l2_buf_type buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    for(i = 0 ; i < PICTURE_BUFFNUM; i++)
    {
        munmap(buffdata[i], bufflength[i]);
    }


}



void RaspberryCapture::CaptureON()
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl(Camfd, VIDIOC_STREAMON, &type);
}

void RaspberryCapture::CaptureOFF()
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl(Camfd, VIDIOC_STREAMOFF, &type);
}

void RaspberryCapture::CloseVideo()
{
    _uninit_mmap ();

    if( Camfd > 0)
        close(Camfd);
}



int RaspberryCapture::getParam(__u32 id)
{
    int ret;
    struct v4l2_control ctrl;
    bzero(&ctrl, sizeof(ctrl));

    switch(id)
    {
    case V4L2_CID_EXPOSURE_ABSOLUTE:
        ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
        std::cout<<__LINE__<<"#####:"<<V4L2_CID_EXPOSURE_ABSOLUTE<<std::endl;
        break;
    default:
        printf(".......\n");
        return 0;
        break;
    }

    ret = ioctl(Camfd, VIDIOC_G_CTRL, &ctrl);
    if(ret != -1)
        return ctrl.value;
    else
    {
        printf("error[%d]:VIDIOC_G_CTRL\n", __LINE__);
        return 0;
    }
}


void RaspberryCapture::_setdefaultparams()
{
    struct v4l2_queryctrl queryctrl = {0};
    struct v4l2_control     control    = {0};
    __u32 ids[] = {
        V4L2_CID_HFLIP,
        V4L2_CID_RED_BALANCE,
        V4L2_CID_BLUE_BALANCE,
        V4L2_CID_VFLIP,
        V4L2_CID_BRIGHTNESS,
        V4L2_CID_CONTRAST,
        V4L2_CID_SATURATION,
        V4L2_CID_HFLIP	,
        V4L2_CID_VFLIP	,
        V4L2_CID_ROTATE,
        V4L2_CID_EXPOSURE_ABSOLUTE,
        V4L2_CID_COLORFX_CBCR,
        V4L2_CID_COLORFX,
        V4L2_CID_SHARPNESS,
        V4L2_CID_SCENE_MODE,
        V4L2_CID_AUTO_EXPOSURE_BIAS,
    };

    uint32_t i = 0;
    for(i = 0 ; i < sizeof(ids) / sizeof(ids[0]); i++)
    {
        memset(&queryctrl, 0, sizeof(queryctrl));
        queryctrl.id = ids[i];
        if ( -1 != xioctl (Camfd, VIDIOC_QUERYCTRL, &queryctrl))
        {
            memset(&control, 0, sizeof(control));
            control.id = ids[i];
            control.value = queryctrl.default_value;
            xioctl(Camfd, VIDIOC_S_CTRL, &control);
            fprintf(stderr, "\t%s\t:\t%d\t\n", queryctrl.name , control.value);
            printf("name(%s) , minimum(%d), maximun(%d), step(%d), defalut_value(%d) , flags(%d)\n", queryctrl.name, queryctrl.minimum , queryctrl.maximum, queryctrl.step, queryctrl.default_value, queryctrl.flags);
        }
    }

    Query_QueryCtrl ();
    QueryControl ();

}

int RaspberryCapture::width()
{
    struct v4l2_format format;
    bzero(&format, sizeof(format));
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(Camfd, VIDIOC_G_FMT, &format);
    return format.fmt.pix.width;
}

int RaspberryCapture::height()
{
    struct v4l2_format format;
    bzero(&format, sizeof(format));
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(Camfd, VIDIOC_G_FMT, &format);
    return format.fmt.pix.height;
}

bool RaspberryCapture::setExposureTime(int timeus)
{

    int ret;
    struct v4l2_control ctrl;
    struct v4l2_queryctrl queryctl;

    // 查询当前曝光模式
    ctrl.id = V4L2_CID_EXPOSURE_AUTO;
    ret = ioctl(Camfd, VIDIOC_G_CTRL, &ctrl);
    if(ret < 0)
        return false;

    if(ctrl.value != V4L2_EXPOSURE_MANUAL)
    {
        ctrl.id = V4L2_CID_EXPOSURE_AUTO;
        ctrl.value = V4L2_EXPOSURE_MANUAL;
        ret = ioctl(Camfd, VIDIOC_S_CTRL, &ctrl);
        if(ret != -1)
            printf("曝光模式切换成manual\n");
        else
            return false;
    }

#if 0
    //查询曝光取之范围
    queryctl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
    ret = ioctl(Camfd, VIDIOC_QUERYCTRL, &queryctl);
    printf("V4L2_CID_EXPOSURE_ABSOLUTE:name(%s) , minimum(%d), maximun(%d), step(%d), defalut_value(%d) , flags(%d)\n", queryctl.name, queryctl.minimum , queryctl.maximum, queryctl.step, queryctl.default_value, queryctl.flags);
#endif

    ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
    ctrl.value = timeus;
    ret = ioctl(Camfd, VIDIOC_S_CTRL, &ctrl);



    if (ret < 0)
        return false;
    else
        return true;

}

int RaspberryCapture::ExposureTime()
{
    int ret;
    struct v4l2_control ctrl = {0};
    ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
    ret = ioctl(Camfd, VIDIOC_G_CTRL, &ctrl);
    if(ret != -1)
        return ctrl.value;
    else
        return -1;

}


int RaspberryCapture::Gain()
{
    int ret;
    struct v4l2_control ctrl;
    ctrl.id = V4L2_CID_GAIN;
    ret = ioctl(Camfd, VIDIOC_G_CTRL, &ctrl);
    return 0;
}
bool RaspberryCapture::setGain (int value)
{
    struct v4l2_control ctrl;
    ctrl.id = V4L2_CID_GAIN;
    ctrl.value = value ;
    if(ioctl(Camfd, VIDIOC_S_CTRL, &ctrl) < 0)
    {
        std::cout<<"set gain failed!" <<std::endl;
    }

    getchar();
    ctrl.id = V4L2_CID_VFLIP;
    ctrl.value = 1 ;
    if(ioctl(Camfd, VIDIOC_S_CTRL, &ctrl) < 0)
    {
        std::cout<<"set vertical filep failed!" <<std::endl;
    }

    ctrl.id = V4L2_CID_HFLIP;
    ctrl.value = 1 ;
    if(ioctl(Camfd, VIDIOC_S_CTRL, &ctrl) < 0)
    {
        std::cout<<"set vertical filep failed!" <<std::endl;
    }


    return true;
}

bool RaspberryCapture::setCtrlParamBool(__u32 id, bool enable)
{
    int ret;
    struct v4l2_control ctrl;
    ctrl.id = id;
    if(enable)
        ctrl.value = 1;
    else
        ctrl.value = 0;

    ret = ioctl(Camfd, VIDIOC_S_CTRL, &ctrl);
    if(ret >= 0)
        return true;
    else
        return false;
}

bool RaspberryCapture::setCtrlParamInt(__u32 id , int value)
{
    int ret;
    struct v4l2_control ctrl;
    ctrl.id = id;
    ctrl.value = value;
    ret = ioctl(Camfd, VIDIOC_S_CTRL, &ctrl);
    if(ret >= 0)
        return true;
    else
        return false;
}


bool RaspberryCapture::getCtrlParamBool(__u32 id)
{
    int ret;
    struct v4l2_control ctrl;
    ctrl.id = id;
    ret = ioctl(Camfd, VIDIOC_G_CTRL, &ctrl);
    if(ctrl.value == 0)
        return false;
    else
        return true;
}

int 	RaspberryCapture:: getCtrlParamInt(__u32 id)
{
    int ret;
    struct v4l2_control ctrl;
    ctrl.id = id;
    ret = ioctl(Camfd, VIDIOC_G_CTRL, &ctrl);
    if(ret < 0)
        return -1;
    else
        return ctrl.value;

}

}

