#include "RaspberryDriver.h"

namespace Raspberry{

RaspberryDriver::RaspberryDriver()
{
    RCapture = new RaspberryCapture("/dev/video0");
    RIO          =  new RaspberryIO;
    ROverlay = new  RaspberryOverlay();

}

void RaspberryDriver::RegisterImageCallback(FHandler* callback)
{
    RCapture->RegisterImageCallback (callback);
}

uint32_t RaspberryDriver::GrabPicture(uint8_t* imagebuff, uint32_t imagelen)
{
    return RCapture->GrabPicture(imagebuff, imagelen);
}

int RaspberryDriver::GrabOnePicture(uint8_t** pImagebuff, uint32_t* pImagelen)
{
   return RCapture->GrabPicture (pImagebuff, pImagelen);
}


RaspberryDriver::~RaspberryDriver()
{
    delete RCapture;
    delete RIO;
    delete ROverlay;
}

bool RaspberryDriver::setGain(int value)
{
    return RCapture->setGain(value);
}

int RaspberryDriver::Gain ()
{
    return RCapture->Gain();
}

bool RaspberryDriver::setExposureTime(int timeus)
{
    return RCapture->setExposureTime (timeus);
}

int RaspberryDriver::ExposureTime()
{
    return RCapture->ExposureTime ();
}

int RaspberryDriver::width()
{
    return RCapture->width ();
}

int RaspberryDriver::height()
{
    return RCapture->height ();
}

int RaspberryDriver::maxWidth()
{

    return 0;
}

int RaspberryDriver::maxHeight()
{

    return 0;
}


bool RaspberryDriver::setHorizontalFlip(bool enable) 		//水平镜像
{
    return RCapture->setCtrlParamBool (V4L2_CID_HFLIP, enable);
}

bool RaspberryDriver::setVerticalFlip(bool enable) 			//垂直镜像
{
    return RCapture->setCtrlParamBool (V4L2_CID_VFLIP, enable);
}
bool RaspberryDriver::setSaturation(int value)   				//饱和度   minimum(-100), maximun(100), step(1), defalut_value(0)
{
    return RCapture->setCtrlParamInt (V4L2_CID_SATURATION, value);
}
bool RaspberryDriver::setContrast(int value)    					//对比度    minimum(-100), maximun(100), step(1), defalut_value(0)
{
    return RCapture->setCtrlParamInt (V4L2_CID_CONTRAST, value);
}
bool RaspberryDriver::setBrightness(int value) 					//亮度       minimum(0), maximun(100), step(1), defalut_value(50)
{
    return RCapture->setCtrlParamInt (V4L2_CID_BRIGHTNESS, value);
}
bool RaspberryDriver::setBlueBalance(int value)  				//minimum(1), maximun(7999), step(1), defalut_value(1000)
{
    return RCapture->setCtrlParamInt (V4L2_CID_BLUE_BALANCE, value);
}
bool RaspberryDriver::setRedBalance(int value)   				//minimum(1), maximun(7999), step(1), defalut_value(1000)
{
    return RCapture->setCtrlParamInt (V4L2_CID_RED_BALANCE, value);
}

bool RaspberryDriver::setRotate(int value)      //旋转角度   minimum(0), maximun(360), step(90), defalut_value(0)
{
    return RCapture->setCtrlParamInt (V4L2_CID_ROTATE, value);
}

int RaspberryDriver::Rotale ()
{
    return RCapture->getCtrlParamInt (V4L2_CID_ROTATE);
}

bool RaspberryDriver::HorizontalFlip()
{
    return RCapture->getCtrlParamBool (V4L2_CID_HFLIP);
}
bool RaspberryDriver::VerticalFlip()
{
    return RCapture->getCtrlParamBool (V4L2_CID_VFLIP);
}
int RaspberryDriver::Saturation()
{
    return RCapture->getCtrlParamInt (V4L2_CID_SATURATION);
}
int RaspberryDriver::Contrast()
{
    return RCapture->getCtrlParamInt (V4L2_CID_CONTRAST);
}
int RaspberryDriver::Brightness()
{
    return RCapture->getCtrlParamInt (V4L2_CID_BRIGHTNESS);
}
int RaspberryDriver::BlueBalance()
{
    return RCapture->getCtrlParamInt (V4L2_CID_BLUE_BALANCE);
}
int RaspberryDriver::RedBalance()
{
    return RCapture->getCtrlParamInt (V4L2_CID_RED_BALANCE);
}

void RaspberryDriver::setIO(int pin, int value)
{
    RIO->setIO(pin, value);
}

int RaspberryDriver::getIO(int pin)
{
    return 0;
}

int RaspberryDriver::setDisplayArea(int top,int left, int width, int height)
{
    return ROverlay->setDisplayArea (top, left, width, height);
}

void RaspberryDriver::setOverlay(RaspberryOverlay::TStatus  status)
{
    ROverlay->setOverlay(status);
}


}

