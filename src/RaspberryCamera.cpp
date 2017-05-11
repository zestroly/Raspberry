#include "RaspberryCamera.h"

namespace Raspberry{

RaspberryCamera::RaspberryCamera()
{
    RCapture = new RaspberryCapture("/dev/video0");
    RIO          =  new RaspberryIO;

}

void RaspberryCamera::RegisterImageCallback(FHandler* callback)
{

}

uint32_t RaspberryCamera::GrabPicture(uint8_t* imagebuff, uint32_t imagelen)
{

}

RaspberryCamera::~RaspberryCamera()
{
    delete RCapture;
    delete RIO;
}


}

