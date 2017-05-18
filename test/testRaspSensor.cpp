#include <stdio.h>
#include <iostream>
#include "RaspberryCamera.h"
#include "rgbtobmp.h"

using namespace Raspberry;

void callback(TImageType* info)
{
    std::cout<<" 图片"<<info->width<<"x"<<info->height<<" size:"<<info->imagelen <<std::endl;

    FILE *pfile = fopen("./cam.jpg", "wb+");
    fwrite(info->imagebuff, info->imagelen,1,pfile);
    fclose(pfile);

//    savebmp((unsigned char*)info->imagebuff, (char*)"./camer.bmp", info->width,  info->height );
}



int main()
{
//    RaspberryCapture* Rcapture = new RaspberryCapture("/dev/video0");
    RaspberryCamera *Camera =  new RaspberryCamera();

    std::cout<<"width:"<<Camera->width ()<<std::endl;
    std::cout<<"height:"<<Camera->height ()<<std::endl;

    Camera->RegisterImageCallback (callback);
    Camera->setExposureTime (1000);
    int value = 10;
    std::cout<<".......\n"<<std::endl;
    getchar();
    Camera->setRotate (90);
    std::cout<<Camera->Rotale ()<<std::endl;
    getchar();
    Camera->setRotate (180);
    std::cout<<Camera->Rotale ()<<std::endl;
    getchar();
    Camera->setRotate (270);
    std::cout<<Camera->Rotale ()<<std::endl;
    getchar();


    int i = 0;
    while(i++ < 10)
    {
        uint8_t *buff = new uint8_t[1024*768 *3];
        uint32_t size = Camera->GrabPicture (buff, 1024*768);
        printf("%d, %02x, %02x\n", size , buff[0], buff[size/2]);
    }

    Camera->RegisterImageCallback (NULL);
    sleep(1);


    getchar();
    delete Camera;
    getchar();
    return 0;
}
