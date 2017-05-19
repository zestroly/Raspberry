#ifndef  RASPBERRYDRIVER_H
#define RASPBERRYDRIVER_H

#include "RaspberryCapture.h"
#include "RaspberryIO.h"



namespace Raspberry {


class RaspberryDriver
{
public:
    RaspberryDriver();
    ~RaspberryDriver();

    void RegisterImageCallback(FHandler* callback);
    uint32_t GrabPicture(uint8_t* imagebuff, uint32_t imagelen);
    int GrabOnePicture(uint8_t** pImagebuff, uint32_t* pImagelen);

    bool setExposureTime(int timeus);         //曝光时间
    bool setHorizontalFlip(bool enable); 		//水平镜像
    bool setVerticalFlip(bool enable); 			//垂直镜像
    bool setSaturation(int value);   				//饱和度   minimum(-100), maximun(100), step(1), defalut_value(0)
    bool setContrast(int value);    					//对比度    minimum(-100), maximun(100), step(1), defalut_value(0)
    bool setBrightness(int value); 					//亮度       minimum(0), maximun(100), step(1), defalut_value(50)
    bool setBlueBalance(int value);  				//minimum(1), maximun(7999), step(1), defalut_value(1000)
    bool setRedBalance(int value);   				//minimum(1), maximun(7999), step(1), defalut_value(1000)
    bool setRotate(int value);      //旋转角度   minimum(0), maximun(360), step(90), defalut_value(0)
    bool setSharpness(int value);   //锐利度

    int ExposureTime();
    bool HorizontalFlip();
    bool VerticalFlip();
    int Saturation();
    int Contrast();
    int Brightness();
    int BlueBalance();
    int RedBalance();
    int Rotale();

    int width();
    int height();

    void setIO(int pin, int value);
    int getIO(int pin);


private:
    int maxWidth();
    int maxHeight();
    bool setGain(int value);
    int Gain();
    RaspberryCapture *RCapture;
    RaspberryIO           *RIO;
};

}
#endif // RASPBERRYDRIVER_H
