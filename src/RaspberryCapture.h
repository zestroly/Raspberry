#ifndef  __RASPBERRYCAPTURE_H_
#define __RASPBERRYCAPTURE_H_

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

#include <thread>
#include <mutex>


#define PICTURE_BUFFNUM 2


namespace Raspberry{

typedef struct _ImageType{
    uint32_t width;
    uint32_t height;
    uint32_t imagelen;
    uint8_t imagebuff;
}TImageType;

typedef void FHandler(TImageType* info);

typedef struct _TCameraInfo{
    int camfd;
    char devicename[64];
    int width;
    int height;
    int image_size;
    int frame_number;
}TCameraInfo;

class RaspberryCapture{
public:
    RaspberryCapture(const char* dev);
    ~RaspberryCapture();
    //查询支持格式
    void QuerySensorSupportPictureFormat();
    //查询picture参数
    void QueryFormat();
    //查询sensor 参数
    void QueryControl();
    //查询是否支持sensor的参数，最大值，最小值，步进，默认值
    void Query_QueryCtrl();

    //查询是否有可用数据
    bool HasAvailableData();
    //
    static void LoopThreadPicture(RaspberryCapture* Rcapture);
    void LoopThreadStart();
    void LoopThreadStop();

    void CaptureON();
    void CaptureOFF();
    int AllocaFrame(uint8_t **outbuff, int *outlen);
    int ReleaseFrame();
    int ioctlVideo(int fd, int request, void* data);

private:
    //打开相机
    int OpenVideo(const char* videoDev);
    //关闭相机
    void CloseVideo();

    int      Camfd;
    char* buffdata[PICTURE_BUFFNUM];
    int      bufflength[PICTURE_BUFFNUM];
    int      BuffCurrentIndex;   //当前访问的缓冲区号
    char* DeviceName;

    std::thread  *mThread;
    std::mutex   mMutex;
    bool isThreadTerminate;   //线程s是否中断
    bool isClassTerminate;   //类是否收到结束


};


}

#endif //__RASPBERRYCAPTURE_H_
