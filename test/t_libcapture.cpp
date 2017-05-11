#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

#include "libcapture.h"

using namespace MxcDriver;

int savefile(char *filename, unsigned char *pbuf, int len)
{
	int fd;
	if ((fd= open(filename, O_RDWR | O_CREAT | O_TRUNC, 0x0666)) < 0)
	{
		printf("Unable to create y frame recording file\n");
		return -1;
	}
	write(fd, pbuf,len);

	close(fd);
return 0;
}

int main(int argc, char **argv)
{
	char outfile[120]={0};
	unsigned char *buf=NULL;
	int ln=0;
	int devno=0;

	MxcCapture Capture=MxcCapture(devno);

	Capture.openDev(false);

	struct timeval tv1,tv2;
	struct timezone tz;
	gettimeofday (&tv1 , &tz);



	for (;;)
	{
		ln=0;
		int ret=Capture.getFrame(&buf, &ln);
		if(!ln)
		{
			if(ret==-2)
				printf("offline\n");
			continue;
		}

		printf("capture len=%d.\n", ln);

		Capture.releaseFrame();
	}

	Capture.closeDev();

return 0;

}
