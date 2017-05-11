#include "RaspberryCapture.h"
#include <iostream>
#include <linux/fb.h>

#define ipu_fourcc(a,b,c,d)\
            (((__u32)(a)<<0)|((__u32)(b)<<8)|((__u32)(c)<<16)|((__u32)(d)<<24))

#define IPU_PIX_FMT_YUYV    			ipu_fourcc('Y','U','Y','V') /*!< 16 YUV 4:2:2 */
#define IPU_PIX_FMT_UYVY    			ipu_fourcc('U','Y','V','Y') /*!< 16 YUV 4:2:2 */
#define IPU_PIX_FMT_NV12    			ipu_fourcc('N','V','1','2') /*!< 12 Y/CbCr 4:2:0 */
#define IPU_PIX_FMT_YUV420P 		ipu_fourcc('I','4','2','0') /*!< 12 YUV 4:2:0 */
#define IPU_PIX_FMT_YUV420P2 	ipu_fourcc('Y','U','1','2') /*!< 12 YUV 4:2:0 */
#define IPU_PIX_FMT_YUV422P 		ipu_fourcc('4','2','2','P') /*!< 16 YUV 4:2:2 */
#define IPU_PIX_FMT_YUV444  		ipu_fourcc('Y','4','4','4') /*!< 24 YUV 4:4:4 */
#define IPU_PIX_FMT_RGB565  		ipu_fourcc('R','G','B','P') /*!< 16 RGB-5-6-5 */
#define IPU_PIX_FMT_BGR24   		ipu_fourcc('B','G','R','3') /*!< 24 BGR-8-8-8 */
#define IPU_PIX_FMT_RGB24   		ipu_fourcc('R','G','B','3') /*!< 24 RGB-8-8-8 */
#define IPU_PIX_FMT_BGR32  		 	ipu_fourcc('B','G','R','4') /*!< 32 BGR-8-8-8-8 */
#define IPU_PIX_FMT_BGRA32  		ipu_fourcc('B','G','R','A') /*!< 32 BGR-8-8-8-8 */
#define IPU_PIX_FMT_RGB32   		ipu_fourcc('R','G','B','4') /*!< 32 RGB-8-8-8-8 */
#define IPU_PIX_FMT_RGBA32  		ipu_fourcc('R','G','B','A') /*!< 32 RGB-8-8-8-8 */
#define IPU_PIX_FMT_ABGR32  		ipu_fourcc('A','B','G','R') /*!< 32 ABGR-8-8-8-8 */

namespace Raspberry{

int bytes_per_pixel(int pixelformat)
{
    switch (pixelformat) {
    case IPU_PIX_FMT_YUV420P:
    case IPU_PIX_FMT_YUV422P:
    case IPU_PIX_FMT_NV12:
        return 1;
        break;
    case IPU_PIX_FMT_RGB565:
    case IPU_PIX_FMT_YUYV:
    case IPU_PIX_FMT_UYVY:
        return 2;
        break;
    case IPU_PIX_FMT_BGR24:
    case IPU_PIX_FMT_RGB24:
        return 3;
        break;
    case IPU_PIX_FMT_BGR32:
    case IPU_PIX_FMT_BGRA32:
    case IPU_PIX_FMT_RGB32:
    case IPU_PIX_FMT_RGBA32:
    case IPU_PIX_FMT_ABGR32:
        return 4;
        break;
    default:
        return 1;
        break;
    }
    return 0;
}

RaspberryCapture::RaspberryCapture(const char* dev)
{
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

static void xioctl(int fh, int request, void *arg)
{
    int r;
    do{
        r = ioctl(fh, request, arg);
    }while( r == -1 && ((errno == EINTR) || (errno == EAGAIN)));

    if( r == -1)
    {
        fprintf(stderr, "error: %d, %s\n", errno, strerror(errno));
    }
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
 //   cam_capabilitycaps(Camfd);

    int ret;
    struct v4l2_fmtdesc fmtdesc;
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
    }
}


void RaspberryCapture::QueryFormat()
{
 //   QuerySensorSupportPictureFormat();

    struct v4l2_format format;
    bzero(&format, sizeof(format));
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    // ioctl(camfd, VIDIOC_S_FMT, &format);
    ioctl(Camfd, VIDIOC_G_FMT, &format);
#if 0
    printf("format.fmt.pix.width:%d\n",  format.fmt.pix.width  );
    printf("format.fmt.pix.height:%d\n",  format.fmt.pix.height  );
    printf("format.fmt.pix.pixelformat:%d\n",  format.fmt.pix.pixelformat  );
    printf("format.fmt.pix.field:%d\n",  format.fmt.pix.field  );   //是否行扫描
    printf("format.fmt.pix.bytesperline:%d\n",  format.fmt.pix.bytesperline  ); //每行byte数
    printf("format.fmt.pix.sizeimage:%d\n",  format.fmt.pix.sizeimage  ); //总共byte数
#endif

    format.fmt.pix.width = 800;
    format.fmt.pix.height = 600;
  //  format.fmt.pix.pixelformat = 1195724874;//IPU_PIX_FMT_RGB24;
    uint32_t pixelformat = IPU_PIX_FMT_RGB24;
    format.fmt.pix.pixelformat = pixelformat;
    format.fmt.pix.sizeimage = format.fmt.pix.width * format.fmt.pix.height * bytes_per_pixel(pixelformat);
  //  format.fmt.pix.field = V4L2_FIELD_NONE;
    format.fmt.pix.field = V4L2_FIELD_NONE;
    format.fmt.pix.bytesperline = format.fmt.pix.width *bytes_per_pixel (pixelformat);
    if (ioctl(Camfd, VIDIOC_S_FMT, &format) < 0)
    {
        printf("error[%d]:VIDIOC_S_FMT\n", __LINE__);
    }
    ioctl(Camfd, VIDIOC_G_FMT, &format);
    printf("format.fmt.pix.width:%d\n",  format.fmt.pix.width  );
    printf("format.fmt.pix.height:%d\n",  format.fmt.pix.height  );
    printf("format.fmt.pix.pixelformat:%d\n",  format.fmt.pix.pixelformat  );
    printf("format.fmt.pix.field:%d\n",  format.fmt.pix.field  );   //是否行扫描
    printf("format.fmt.pix.bytesperline:%d\n",  format.fmt.pix.bytesperline  ); //每行byte数
    printf("format.fmt.pix.sizeimage:%d\n",  format.fmt.pix.sizeimage  ); //总共byte数

}

void RaspberryCapture::QueryControl()
{
    int ret;
    struct v4l2_control ctrl;
    bzero(&ctrl, sizeof(ctrl));
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
    int i;

    //
    struct v4l2_streamparm parm;
    bzero(&parm, sizeof(parm));
    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    parm.parm.capture.timeperframe.numerator = 1;
    parm.parm.capture.timeperframe.denominator = 15; //设置帧数
   // parm.parm.capture.timeperframe.denominator = 30; //设置帧数
    parm.parm.capture.capturemode = 0;
    if(ioctl(Camfd, VIDIOC_S_PARM, &parm) == -1)
    {
        printf("error[%d] :VIDIOC_S_PARM..........\n", __LINE__);
        exit(-1);
    }

    struct v4l2_input input;
    bzero(&input, sizeof(input));
    if(ioctl(Camfd, VIDIOC_G_INPUT, &input.index) == -1)
    { //查询当前设j备
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

    struct v4l2_cropcap cropcap;
    bzero(&cropcap, sizeof(cropcap));
    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

#if 0
     if(ioctl(Camfd, VIDIOC_CROPCAP, &cropcap) == -1)
    {
        printf("error[%d] :VIDIOC_CROPCAP.........\n", __LINE__);
        exit(-1);
    }
#endif



#if 0
     //貌似还不支持这个操作
    //从原始图片中裁剪出矩形
    struct v4l2_crop crop;
    bzero(&crop, sizeof(crop));
    crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    crop.c.width =  640;
    crop.c.height = 480;
    crop.c.top = 0;
    crop.c.left = 0;
    if(ioctl(Camfd, VIDIOC_G_CROP, &crop) == -1)
    {
        printf("error[%d] :VIDIOC_S_CROP..........\n", __LINE__);
        exit(-1);
    }
#endif
    //设置图片格

    QueryFormat();




    //分配缓冲区,mmap
    struct v4l2_requestbuffers requestbuffers;
    bzero(&requestbuffers, sizeof(requestbuffers));
    requestbuffers.count =  PICTURE_BUFFNUM; //缓冲区个数, memory = V4L2_MEMORY_MMAP 有效
    requestbuffers.memory = V4L2_MEMORY_MMAP;
    requestbuffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(Camfd, VIDIOC_REQBUFS, &requestbuffers);
    if(ret == -1){
        printf("error :VIDIOC_REQBUFS..........\n");
        exit(-1);
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

        printf("buffers>>index :%d\t", buffers.index );
        printf("type :%d\t", buffers.type );
        printf("bytesused :%d\t", buffers.bytesused );
        printf("flags :%d\t", buffers.flags );
        printf("field :%d\t", buffers.field );
        printf("timestamp :%u\t", buffers.timestamp );
        printf("timecode :%u\t", buffers.timecode );
        printf("sequence :%d\t", buffers.sequence );
        printf("m.offset :%d\t", buffers.m.offset );
        printf("length :%d\t\n", buffers.length );

        bufflength[i] = buffers.length;
        buffdata[i] = (char*)mmap(NULL, buffers.length, PROT_READ|PROT_WRITE, MAP_SHARED, Camfd, buffers.m.offset);
        if(buffdata[i] == MAP_FAILED)
            printf("mmap failed!...\n");
    }

    //设置缓冲区已经准备好
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


#if 0


    struct v4l2_format fmt;
    bzero(&fmt, sizeof(fmt));

    fmt.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
    fmt.fmt.win.w.top = 0;
    fmt.fmt.win.w.left = 0;
    fmt.fmt.win.w.height = 400;
    fmt.fmt.win.w.width  = 600;

    if (ioctl (Camfd, VIDIOC_S_FMT, &fmt) < 0 )
    {
        printf("error[%d]:VIDIOC_S_FMT.\n", __LINE__);
        return -1;
    }

    struct fb_fix_screeninfo fb0_fix, fb_fg_fix;
    struct fb_var_screeninfo vinfo;
    struct v4l2_framebuffer fb;
    int on =1;

    int fdfb0 = open("/dev/fb0", O_RDWR);
    if( fdfb0 <  0)
    {
        printf("error[%d], open /dev/fb0 error!\n", __LINE__);
    }


    int fbnumber = 0;
    if(ioctl(Camfd, VIDIOC_S_INPUT, &fbnumber) < 0)
    {
        printf("error[%d]:......\n",__LINE__);
    }

    if(ioctl(Camfd, VIDIOC_S_OUTPUT, &fbnumber) < 0)
    {
        printf("error[%d]:......\n",__LINE__);
    }

    if(ioctl(Camfd, VIDIOC_G_FBUF, &fb) < 0)
    {
        printf("error[%d]:......\n",__LINE__);
    }

    if(ioctl(fdfb0, FBIOGET_VSCREENINFO, &vinfo) < 0);
    {
        printf("error[%d],.......... \n", __LINE__);
    }

    char *fb_addr;
    if( (fb_addr = (char*)mmap(0, 1024*768*3, PROT_READ|PROT_WRITE, MAP_SHARED, fdfb0, 0) ) == NULL)
    {
        printf("error[%d],....mmap...... \n", __LINE__);
    }

    if(ioctl(fdfb0, FBIOGET_FSCREENINFO, &fb0_fix) < 0);
    {
        printf("error[%d],.......... \n", __LINE__);
    }


    if(ioctl(Camfd, VIDIOC_OVERLAY, &on))
    {
        printf("error[%d],......... \n", __LINE__);
    }
#endif


    //开启采集
    CaptureON();



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

  //  printf("index:%d, timestamp:%d\n", buffer.index, buffer.timestamp);
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


void RaspberryCapture::LoopThreadPicture(RaspberryCapture* Rcapture)
{
    while(!Rcapture->isClassTerminate && !Rcapture->isThreadTerminate)
    {
        uint8_t *imagebuff;
        int          imagelen;
        if( Rcapture->HasAvailableData () )
        {
            Rcapture->AllocaFrame (&imagebuff, &imagelen);
            //假如是外触发模式则等待读取

            //

            Rcapture->ReleaseFrame ();
        }else{
            usleep(1000);
        }

    }
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
    int i;

    CaptureOFF ();
    for(i = 0; i < PICTURE_BUFFNUM; i++)
        munmap(buffdata[i], bufflength[i]);

    if( Camfd > 0)
        close(Camfd);
}



}

