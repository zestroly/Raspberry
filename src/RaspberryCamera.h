#ifndef  RASPBERRYCAMERA_H
#define RASPBERRYCAMERA_H

#include "RaspberryCapture.h"
#include "RaspberryIO.h"



namespace Raspberry {


class RaspberryCamera
{
public:
    RaspberryCamera();
    ~RaspberryCamera();

    void RegisterImageCallback(FHandler* callback);
    uint32_t GrabPicture(uint8_t* imagebuff, uint32_t imagelen);

private:
    RaspberryCapture *RCapture;
    RaspberryIO           *RIO;
};

}
#endif // RASPBERRYCAMERA_H
