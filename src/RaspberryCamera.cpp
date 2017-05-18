#include "RaspberryCamera.h"

namespace Raspberry{

RaspberryCamera::RaspberryCamera()
{
    RCapture = new RaspberryCapture("/dev/video0");
    RIO          =  new RaspberryIO;

}

void RaspberryCamera::RegisterImageCallback(FHandler* callback)
{
    RCapture->RegisterImageCallback (callback);
}

uint32_t RaspberryCamera::GrabPicture(uint8_t* imagebuff, uint32_t imagelen)
{
    return RCapture->GrabPicture(imagebuff, imagelen);
}

RaspberryCamera::~RaspberryCamera()
{
    delete RCapture;
    delete RIO;
}

bool RaspberryCamera::setGain(int value)
{
    return RCapture->setGain(value);
}

int RaspberryCamera::Gain ()
{
    return RCapture->Gain();
}

bool RaspberryCamera::setExposureTime(int timeus)
{
    return RCapture->setExposureTime (timeus);
}

int RaspberryCamera::ExposureTime()
{
    return RCapture->ExposureTime ();
}

int RaspberryCamera::width()
{
    return RCapture->width ();
}

int RaspberryCamera::height()
{
    return RCapture->height ();
}

int RaspberryCamera::maxWidth()
{

    return 0;
}

int RaspberryCamera::maxHeight()
{

    return 0;
}


bool RaspberryCamera::setHorizontalFlip(bool enable) 		//水平镜像
{
    return RCapture->setCtrlParamBool (V4L2_CID_HFLIP, enable);
}

bool RaspberryCamera::setVerticalFlip(bool enable) 			//垂直镜像
{
    return RCapture->setCtrlParamBool (V4L2_CID_VFLIP, enable);
}
bool RaspberryCamera::setSaturation(int value)   				//饱和度   minimum(-100), maximun(100), step(1), defalut_value(0)
{
    return RCapture->setCtrlParamInt (V4L2_CID_SATURATION, value);
}
bool RaspberryCamera::setContrast(int value)    					//对比度    minimum(-100), maximun(100), step(1), defalut_value(0)
{
    return RCapture->setCtrlParamInt (V4L2_CID_CONTRAST, value);
}
bool RaspberryCamera::setBrightness(int value) 					//亮度       minimum(0), maximun(100), step(1), defalut_value(50)
{
    return RCapture->setCtrlParamInt (V4L2_CID_BRIGHTNESS, value);
}
bool RaspberryCamera::setBlueBalance(int value)  				//minimum(1), maximun(7999), step(1), defalut_value(1000)
{
    return RCapture->setCtrlParamInt (V4L2_CID_BLUE_BALANCE, value);
}
bool RaspberryCamera::setRedBalance(int value)   				//minimum(1), maximun(7999), step(1), defalut_value(1000)
{
    return RCapture->setCtrlParamInt (V4L2_CID_RED_BALANCE, value);
}

bool RaspberryCamera::setRotate(int value)      //旋转角度   minimum(0), maximun(360), step(90), defalut_value(0)
{
    return RCapture->setCtrlParamInt (V4L2_CID_ROTATE, value);
}

int RaspberryCamera::Rotale ()
{
    return RCapture->getCtrlParamInt (V4L2_CID_ROTATE);
}

bool RaspberryCamera::HorizontalFlip()
{
    return RCapture->getCtrlParamBool (V4L2_CID_HFLIP);
}
bool RaspberryCamera::VerticalFlip()
{
    return RCapture->getCtrlParamBool (V4L2_CID_VFLIP);
}
int RaspberryCamera::Saturation()
{
    return RCapture->getCtrlParamInt (V4L2_CID_SATURATION);
}
int RaspberryCamera::Contrast()
{
    return RCapture->getCtrlParamInt (V4L2_CID_CONTRAST);
}
int RaspberryCamera::Brightness()
{
    return RCapture->getCtrlParamInt (V4L2_CID_BRIGHTNESS);
}
int RaspberryCamera::BlueBalance()
{
    return RCapture->getCtrlParamInt (V4L2_CID_BLUE_BALANCE);
}
int RaspberryCamera::RedBalance()
{
    return RCapture->getCtrlParamInt (V4L2_CID_RED_BALANCE);
}



}

