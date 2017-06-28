#ifndef RASPBERRYOVERLAY_H
#define RASPBERRYOVERLAY_H
#include <linux/videodev2.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <linux/fb.h>


class RaspberryOverlay
{
public:
    typedef enum {
        ON=1,
        OFF=2
    }TStatus;
    RaspberryOverlay();
    ~RaspberryOverlay();
    int setDisplayArea(int top,int left, int width, int height);
    void setOverlay(TStatus  status);

private:
    int VideoFb;

};

#endif // RASPBERRYOVERLAY_H
