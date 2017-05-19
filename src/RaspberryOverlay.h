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
    RaspberryOverlay();
};

#endif // RASPBERRYOVERLAY_H
