#include "RaspberryOverlay.h"
#include "RaspberryPixelFormat.h"
#include <linux/fb.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

RaspberryOverlay::RaspberryOverlay()
{
    VideoFb = open("/dev/video0", O_RDWR | O_NONBLOCK);
}


RaspberryOverlay::~RaspberryOverlay()
{
    setOverlay(OFF);
    close(VideoFb);

}

int RaspberryOverlay::setDisplayArea(int top,int left, int width, int height)
{
     struct v4l2_format fmt;
    bzero(&fmt, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;

    if (ioctl (VideoFb, VIDIOC_G_FMT, &fmt) < 0 )
    {
        printf("error[%d]:VIDIOC_S_FMT.\n", __LINE__);
        return -1;
    }

    fmt.fmt.win.w.top = top;
    fmt.fmt.win.w.left = left;
    fmt.fmt.win.w.height = width;
    fmt.fmt.win.w.width  =  height;

    if (ioctl (VideoFb, VIDIOC_S_FMT, &fmt) < 0 )
    {
        printf("error[%d]:VIDIOC_S_FMT.\n", __LINE__);
        return -2;
    }
}

void  RaspberryOverlay::setOverlay(TStatus  status)
{
    int on =1;
    if(status == ON)
        on = 1;
    else
        on = 0;

    if(ioctl(VideoFb, VIDIOC_OVERLAY, &on))
    {
        printf("error[%d],......... \n", __LINE__);
    }
}




