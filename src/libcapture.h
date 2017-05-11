#ifndef LIB_CAPTURE_H
#define LIB_CAPTURE_H


namespace MxcDriver{

typedef struct {
  int width;
  int height;
  int sizeimage;
  int pixelformat;
} TCapInfo, *PCapInfo;


class MxcCapture
{
public:
	MxcCapture(int devno);
	int openDev(bool blockcapture=true);
	int closeDev();

	/*
	 * return:
	 *		0	success
	 *		-1 unknown error
	 *		-2 camera offline
	 */
	int getFrame(unsigned char **outbuf, int *outlen);
	int releaseFrame(void);
	void pauseCapture(void);
	void resumeCapture(void);
	int setBlockCapture(bool bblock);

private:
	int m_devno;
	int fd_v4l;
	int m_capture;
	int m_bufindex;
	int g_in_width ;
	int g_in_height;
	int g_out_width;
	int g_out_height;
	int g_top ;
	int g_left;
	int g_input;
	int g_capture_count;
	int g_rotate ;
	int g_cap_fmt;
	int g_bpp;
	int g_camera_framerate ;
	int g_extra_pixel ;
	int g_capture_mode ;
	char g_v4l_device[128];


	int v4l_capture_setup(void);
	int start_capturing(void);
	int stop_capturing(void);
	int getCapInfo(PCapInfo pCapInfo);
};

}
#endif
