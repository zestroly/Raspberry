#include <stdio.h>
#include <iostream>
#include "RaspberryCamera.h"

using namespace Raspberry;

void callback(TImageType* info)
{

}



int main()
{
//    RaspberryCapture* Rcapture = new RaspberryCapture("/dev/video0");
    RaspberryCamera *Camera =  new RaspberryCamera();
    Camera->RegisterImageCallback (callback);
    getchar();
    Camera->RegisterImageCallback (NULL);

    delete Camera;
    getchar();
    return 0;
}
