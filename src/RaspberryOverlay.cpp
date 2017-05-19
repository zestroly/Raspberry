#include "RaspberryOverlay.h"
#include "RaspberryPixelFormat.h"
#include <linux/fb.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

RaspberryOverlay::RaspberryOverlay()
{
    int FramebuffFb = open("/dev/fb0", O_RDWR);

    int VideoFb = open("/dev/video0", O_RDWR | O_NONBLOCK);

    int on =1;
    //获取framebuffer  地址，大小图片格式
   // VIDIOC_G_FBUF;
  //  VIDIOC_S_FBUF;
 //   V4L2_BUF_TYPE_VIDEO_OVERLAY;

    on = 0;
    if(ioctl(VideoFb, VIDIOC_OVERLAY, &on))
    {
        printf("error[%d],......... \n", __LINE__);
    }

    struct v4l2_format fmt;
    bzero(&fmt, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;


    if (ioctl (VideoFb, VIDIOC_G_FMT, &fmt) < 0 )
    {
        printf("error[%d]:VIDIOC_S_FMT.\n", __LINE__);
        exit(0);
    }

    std::cout<<"top:"<<fmt.fmt.win.w.top <<std::endl;
    std::cout<<"left:"<<fmt.fmt.win.w.left <<std::endl;
    std::cout<<"height:"<<fmt.fmt.win.w.height <<std::endl;
    std::cout<<"width:"<<fmt.fmt.win.w.width <<std::endl;

    struct v4l2_framebuffer fbuf;
    if(ioctl(VideoFb, VIDIOC_G_FBUF, &fbuf) < 0)
        perror("VIDIOC_G_FBUF");

    struct fb_fix_screeninfo si;
    if(0 == ioctl(FramebuffFb, FBIOGET_FSCREENINFO, &si))
    {
        printf("%u,   %u\n", si.smem_start, (unsigned long)fbuf.base);
    }


    fmt.fmt.win.w.top = 0;
    fmt.fmt.win.w.left = 0;
    fmt.fmt.win.w.height = 800;
    fmt.fmt.win.w.width  =  600;

    if (ioctl (VideoFb, VIDIOC_S_FMT, &fmt) < 0 )
    {
        printf("error[%d]:VIDIOC_S_FMT.\n", __LINE__);
        exit(0);
    }


    on = 1;
    if(ioctl(VideoFb, VIDIOC_OVERLAY, &on))
    {
        printf("error[%d],......... \n", __LINE__);
    }

#if 0
    fmt.fmt.win.w.top = 0;
    fmt.fmt.win.w.left = 0;
    fmt.fmt.win.w.height = 600;
    fmt.fmt.win.w.width  = 800;

    if (ioctl (Camfd, VIDIOC_S_FMT, &fmt) < 0 )
    {
        printf("error[%d]:VIDIOC_S_FMT.\n", __LINE__);
        exit(0);
    }

    struct fb_fix_screeninfo fb0_fix, fb_fg_fix;
    struct fb_var_screeninfo vinfo;
    struct v4l2_framebuffer fb;

    int fdfb0 = open("/dev/fb0", O_RDWR);
    if( fdfb0 <  0)
    {
        printf("error[%d], open /dev/fb0 error!\n", __LINE__);
        exit(0);
    }

    int fbnumber = 0;
    if(ioctl(Camfd, VIDIOC_S_INPUT, &fbnumber) < 0)
    {V4L2_CID_EXPOSURE_ABSOLUTE
                printf("error[%d]:......\n",__LINE__);
        exit(0);
    }

    if(ioctl(Camfd, VIDIOC_S_OUTPUT &fbnumber) < 0)
    {
        printf("error[%d]:......\n",__LINE__);
    }

    if(ioctl(Camfd, VIDIOC_G_FBUF, &fb) < 0)
    {
        printf("error[%d]:......\n",__LINE__);
        exit(0);
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



}

