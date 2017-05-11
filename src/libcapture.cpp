#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <string.h>
#include <malloc.h>

#include "libcapture.h"

struct v4l2_mxc_offset {
	uint32_t u_offset;
	uint32_t v_offset;
};

#define BUFFER_NUM 2

struct testbuffer
{
	unsigned char *start;
	size_t offset;
	unsigned int length;
};

struct testbuffer buffers[BUFFER_NUM];


#define ipu_fourcc(a,b,c,d)\
        (((__u32)(a)<<0)|((__u32)(b)<<8)|((__u32)(c)<<16)|((__u32)(d)<<24))

#define IPU_PIX_FMT_YUYV    ipu_fourcc('Y','U','Y','V') /*!< 16 YUV 4:2:2 */
#define IPU_PIX_FMT_UYVY    ipu_fourcc('U','Y','V','Y') /*!< 16 YUV 4:2:2 */
#define IPU_PIX_FMT_NV12    ipu_fourcc('N','V','1','2') /* 12 Y/CbCr 4:2:0 */
#define IPU_PIX_FMT_YUV420P ipu_fourcc('I','4','2','0') /*!< 12 YUV 4:2:0 */
#define IPU_PIX_FMT_YUV420P2 ipu_fourcc('Y','U','1','2') /*!< 12 YUV 4:2:0 */
#define IPU_PIX_FMT_YUV422P ipu_fourcc('4','2','2','P') /*!< 16 YUV 4:2:2 */
#define IPU_PIX_FMT_YUV444  ipu_fourcc('Y','4','4','4') /*!< 24 YUV 4:4:4 */
#define IPU_PIX_FMT_RGB565  ipu_fourcc('R','G','B','P') /*!< 16 RGB-5-6-5 */
#define IPU_PIX_FMT_BGR24   ipu_fourcc('B','G','R','3') /*!< 24 BGR-8-8-8 */
#define IPU_PIX_FMT_RGB24   ipu_fourcc('R','G','B','3') /*!< 24 RGB-8-8-8 */
#define IPU_PIX_FMT_BGR32   ipu_fourcc('B','G','R','4') /*!< 32 BGR-8-8-8-8 */
#define IPU_PIX_FMT_BGRA32  ipu_fourcc('B','G','R','A') /*!< 32 BGR-8-8-8-8 */
#define IPU_PIX_FMT_RGB32   ipu_fourcc('R','G','B','4') /*!< 32 RGB-8-8-8-8 */
#define IPU_PIX_FMT_RGBA32  ipu_fourcc('R','G','B','A') /*!< 32 RGB-8-8-8-8 */
#define IPU_PIX_FMT_ABGR32  ipu_fourcc('A','B','G','R') /*!< 32 ABGR-8-8-8-8 */


int bytes_per_pixel(int fmt)
{
	switch (fmt) {
	case IPU_PIX_FMT_YUV420P:
	case IPU_PIX_FMT_YUV422P:
	case IPU_PIX_FMT_NV12:
		return 1;
		break;
	case IPU_PIX_FMT_RGB565:
	case IPU_PIX_FMT_YUYV:
	case IPU_PIX_FMT_UYVY:
		return 2;
		break;
	case IPU_PIX_FMT_BGR24:
	case IPU_PIX_FMT_RGB24:
		return 3;
		break;
	case IPU_PIX_FMT_BGR32:
	case IPU_PIX_FMT_BGRA32:
	case IPU_PIX_FMT_RGB32:
	case IPU_PIX_FMT_RGBA32:
	case IPU_PIX_FMT_ABGR32:
		return 4;
		break;
	default:
		return 1;
		break;
	}
	return 0;
}


namespace MxcDriver{


MxcCapture::MxcCapture(int devno)
{
	fd_v4l=-1;
	m_devno=devno;
	memset(g_v4l_device,0,sizeof(g_v4l_device));
	sprintf(g_v4l_device, "/dev/video%d", m_devno);
	m_capture=0;
	m_bufindex=0;

	g_in_width = 424;
	g_in_height = 1024	;
	g_out_width = 424;
	g_out_height = 1024;
	g_top = 0;
	g_left = 0;
	g_input = 0;
	g_rotate = 0;
	g_cap_fmt = IPU_PIX_FMT_RGB24;//IPU_PIX_FMT_RGB24;//V4L2_PIX_FMT_YUV420;//IPU_PIX_FMT_RGB24;
	g_bpp = 24;
	g_camera_framerate = 30;
	g_extra_pixel = 0;
	g_capture_mode = 0;

}


void MxcCapture::pauseCapture(void)
{
	enum v4l2_buf_type type;

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (ioctl (fd_v4l, VIDIOC_STREAMOFF, &type)<0)
  {
		printf("VIDIOC_STREAMOFF error\n");
	}
}

void MxcCapture::resumeCapture(void)
{
	enum v4l2_buf_type type;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl (fd_v4l, VIDIOC_STREAMON, &type) < 0)
	{
		printf("VIDIOC_STREAMON error\n");
	}
}

int MxcCapture::setBlockCapture(bool bblock)
{
	unsigned short flags=0;
	fcntl(fd_v4l, F_GETFL, &flags);
	flags=bblock?flags&(~O_NONBLOCK):flags|O_NONBLOCK;
	return fcntl(fd_v4l, F_SETFL, flags);
}

int MxcCapture::getCapInfo(PCapInfo pCapInfo)
{
	struct v4l2_format fmt;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (fd_v4l<0) return -1;

	if (ioctl(fd_v4l, VIDIOC_G_FMT, &fmt) < 0)
	{
		printf("get format failed.\n");
		return -1;
	}
	else
	{
	/*
		printf("\t Width = %d", fmt.fmt.pix.width);
		printf("\t Height = %d", fmt.fmt.pix.height);
		printf("\t Image size = %d\n", fmt.fmt.pix.sizeimage);
		printf("\t pixelformat = %d\n", fmt.fmt.pix.pixelformat);
		printf("\t bytesperline = %d\n", fmt.fmt.pix.bytesperline);
	*/
		pCapInfo->width=fmt.fmt.pix.width;
		pCapInfo->height=fmt.fmt.pix.height;
		pCapInfo->sizeimage=fmt.fmt.pix.sizeimage;
		pCapInfo->pixelformat=fmt.fmt.pix.pixelformat;
	}
	return 0;
}

int MxcCapture::openDev(bool blockcapture)
{
	struct v4l2_format fmt;
	struct v4l2_control ctrl;
	struct v4l2_streamparm parm;
	struct v4l2_crop crop;
	struct v4l2_mxc_offset off;
	struct v4l2_frmsizeenum fsize;
	struct v4l2_fmtdesc ffmt={0};
	enum v4l2_buf_type type;

	if ((fd_v4l = open(g_v4l_device, O_RDWR | (blockcapture?0:O_NONBLOCK), 0)) < 0)
	{
		printf("Unable to open %s\n", g_v4l_device);
		return -1;
	}

	parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	parm.parm.capture.timeperframe.numerator = 1;
	//parm.parm.capture.capability= V4L2_CAP_TIMEPERFRAME;
	parm.parm.capture.timeperframe.denominator = g_camera_framerate;
	parm.parm.capture.capturemode = g_capture_mode;

	if (ioctl(fd_v4l, VIDIOC_S_PARM, &parm) < 0)
	{
		printf("VIDIOC_S_PARM failed\n");
		return -1;
	}

	if (ioctl(fd_v4l, VIDIOC_S_INPUT, &g_input) < 0)
	{
		printf("VIDIOC_S_INPUT failed\n");
		return -1;
	}

	crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd_v4l, VIDIOC_G_CROP, &crop) < 0)
	{
		printf("VIDIOC_G_CROP failed\n");
		return -1;
	}

	crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	crop.c.width = g_in_width;
	crop.c.height = g_in_height;
	crop.c.top = g_top;
	crop.c.left = g_left;
	if (ioctl(fd_v4l, VIDIOC_S_CROP, &crop) < 0)
	{
		printf("VIDIOC_S_CROP failed\n");
		return -1;
	}

	memset(&fmt, 0, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.pixelformat = g_cap_fmt;
	fmt.fmt.pix.width = g_out_width;
	fmt.fmt.pix.height = g_out_height;

	if (g_extra_pixel)
	{
		off.u_offset = (2 * g_extra_pixel + g_out_width) * (g_out_height + g_extra_pixel)
				 - g_extra_pixel + (g_extra_pixel / 2) * ((g_out_width / 2)
				 + g_extra_pixel) + g_extra_pixel / 2;
		off.v_offset = off.u_offset + (g_extra_pixel + g_out_width / 2) *
				((g_out_height / 2) + g_extra_pixel);
					fmt.fmt.pix.bytesperline = g_out_width + g_extra_pixel * 2;
		fmt.fmt.pix.priv = (uint32_t) &off;
					fmt.fmt.pix.sizeimage = (g_out_width + g_extra_pixel * 2 )
						* (g_out_height + g_extra_pixel * 2) * 3 / 2;
	}
	else
	{
		//fmt.fmt.pix.bytesperline = g_out_width;
		//fmt.fmt.pix.priv = 0;
		//fmt.fmt.pix.sizeimage = 0;
	}

	fmt.fmt.pix.sizeimage = fmt.fmt.pix.width * fmt.fmt.pix.height * bytes_per_pixel(g_cap_fmt);// bytes_per_pixel(g_cap_fmt); //g_bpp / 8;
	fmt.fmt.pix.bytesperline = g_out_width * bytes_per_pixel(g_cap_fmt);

printf("perline=%d.\n", fmt.fmt.pix.bytesperline);


	if (ioctl(fd_v4l, VIDIOC_S_FMT, &fmt) < 0)
	{
		printf("set format failed\n");
		return -1;
	}

	// Set rotation
	ctrl.id = V4L2_CID_PRIVATE_BASE + 0;
	ctrl.value = g_rotate;
	if (ioctl(fd_v4l, VIDIOC_S_CTRL, &ctrl) < 0)
	{
		printf("set ctrl failed\n");
		return -1;
	}

	struct v4l2_requestbuffers req;
	memset(&req, 0, sizeof (req));
	req.count = BUFFER_NUM;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (ioctl(fd_v4l, VIDIOC_REQBUFS, &req) < 0)
	{
		printf("v4l_capture_setup: VIDIOC_REQBUFS failed\n");
		return -1;
	}

	unsigned int i;
	struct v4l2_buffer buf;


	for (i = 0; i < BUFFER_NUM; i++)
	{
		memset(&buf, 0, sizeof (buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		if (ioctl(fd_v4l, VIDIOC_QUERYBUF, &buf) < 0)
		{
			printf("VIDIOC_QUERYBUF error\n");
			return -1;
		}

		buffers[i].length = buf.length;
		buffers[i].offset = (size_t) buf.m.offset;
		buffers[i].start = (unsigned char *)mmap (NULL, buffers[i].length,
				PROT_READ | PROT_WRITE, MAP_SHARED,
				fd_v4l, buffers[i].offset);
		memset(buffers[i].start, 0xFF, buffers[i].length);
	}

	for (i = 0; i < BUFFER_NUM; i++)
	{
		memset(&buf, 0, sizeof (buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		buf.m.offset = buffers[i].offset;
		if (g_extra_pixel)
		{
			buf.m.offset += g_extra_pixel *
			(g_out_width + 2 * g_extra_pixel) + g_extra_pixel;
		}

		if (ioctl (fd_v4l, VIDIOC_QBUF, &buf) < 0)
		{
			printf("VIDIOC_QBUF error\n");
			return -1;
		}
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl (fd_v4l, VIDIOC_STREAMON, &type) < 0)
	{
		printf("VIDIOC_STREAMON error\n");
		return -1;
	}

	return 0;
}

int MxcCapture::closeDev()
{
	enum v4l2_buf_type type;

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl (fd_v4l, VIDIOC_STREAMOFF, &type)<0)
	{
		printf("VIDIOC_STREAMOFF error.\n");
	}

	for (int i = 0; i < BUFFER_NUM; i++)
	{
		munmap(buffers[i].start, buffers[i].length);
	}

	close(fd_v4l);
	fd_v4l=-1;
	return 0;
}


int MxcCapture::getFrame(unsigned char **outbuf, int *outlen)
{
	struct v4l2_buffer buf;
	TCapInfo capinfo;

	if (fd_v4l<0)
	{
		return -1;
	}

	if (m_capture)
	{
			printf("capturing...");
			return -1;
	}

	if (getCapInfo(&capinfo)<0)
	{
		printf("getCapInfo() error.\n");
		return -1;
	}

	memset(&buf, 0, sizeof (buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	if (ioctl (fd_v4l, VIDIOC_DQBUF, &buf) < 0)
	{
		//printf("VIDIOC_DQBUF failed.\n");
		//if (!MxcIO::getKeyState((MXCKEY)((int)MXCKEY_CAM0+m_devno)))
		//	return -2;
		return -1;
	}

	//fwrite(buffers[buf.index].start, fmt.fmt.pix.sizeimage, 1, fd_y_file);

	m_bufindex=buf.index;
	//printf("buf.index=%d.\n",m_bufindex);

	m_capture=1;

	if(outbuf)
	*outbuf=buffers[buf.index].start;
	if(outlen)
	*outlen=capinfo.sizeimage;

	return 0;
}

int MxcCapture::releaseFrame(void)
{
	struct v4l2_buffer buf;

	if ((fd_v4l<0) || !m_capture)
		return -1;

	memset(&buf, 0, sizeof (buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index=m_bufindex;
	if (ioctl (fd_v4l, VIDIOC_QBUF, &buf) < 0)
	{
		//printf("VIDIOC_QBUF failed\n");
		//if (!MxcIO::getKeyState((MXCKEY)((int)MXCKEY_CAM0+m_devno)))
		//	return -2;
		return -1;
	}

	m_bufindex=0;

	m_capture=0;

	return 0;
}

}
