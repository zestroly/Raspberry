#include "RaspberryOverlay.h"

//using namespace Raspberry;
int main()
{

    RaspberryOverlay Overlay;
    Overlay.setOverlay (RaspberryOverlay::ON);

    getchar();

    Overlay.setDisplayArea (100, 100, 900,700);
    getchar();


    Overlay.setOverlay (RaspberryOverlay::OFF);


}



