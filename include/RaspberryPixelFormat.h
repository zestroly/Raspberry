#ifndef RASPBERRYPIXELFORMAT_H
#define RASPBERRYPIXELFORMAT_H

#endif // RASPBERRYPIXELFORMAT_H

#define ipu_fourcc(a,b,c,d)\
            (((__u32)(a)<<0)|((__u32)(b)<<8)|((__u32)(c)<<16)|((__u32)(d)<<24))

#define IPU_PIX_FMT_YUV420P2 	ipu_fourcc('Y','U','1','2') /*!< 12 YUV 4:2:0 */
#define IPU_PIX_FMT_YUV422P 		ipu_fourcc('4','2','2','P') /*!< 16 YUV 4:2:2 */
#define IPU_PIX_FMT_YUV444  		ipu_fourcc('Y','4','4','4') /*!< 24 YUV 4:4:4 */
#define IPU_PIX_FMT_RGB565  		ipu_fourcc('R','G','B','P') /*!< 16 RGB-5-6-5 */
#define IPU_PIX_FMT_BGR24   		ipu_fourcc('B','G','R','3') /*!< 24 BGR-8-8-8 */
#define IPU_PIX_FMT_BGR32  		 	ipu_fourcc('B','G','R','4') /*!< 32 BGR-8-8-8-8 */
#define IPU_PIX_FMT_BGRA32  		ipu_fourcc('B','G','R','A') /*!< 32 BGR-8-8-8-8 */
#define IPU_PIX_FMT_RGB32   		ipu_fourcc('R','G','B','4') /*!< 32 RGB-8-8-8-8 */
#define IPU_PIX_FMT_RGBA32  		ipu_fourcc('R','G','B','A') /*!< 32 RGB-8-8-8-8 */
#define IPU_PIX_FMT_ABGR32  		ipu_fourcc('A','B','G','R') /*!< 32 ABGR-8-8-8-8 */
//Raspberry Camera V1.3
#define IPU_PIX_FMT_YUV420P 		ipu_fourcc('I','4','2','0') /*!< 12 YUV 4:2:0 */
#define IPU_PIX_FMT_YUYV    			ipu_fourcc('Y','U','Y','V') /*!< 16 YUV 4:2:2 */
#define IPU_PIX_FMT_RGB24   		ipu_fourcc('R','G','B','3') /*!< 24 RGB-8-8-8 */
#define IPU_PIX_FMT_JPEG   		    1195724874U
#define IPU_PIX_FMT_H264	            875967048U
#define IPU_PIX_FMT_MOTION_JPEG     1196444237U
#define IPU_PIX_FMT_YVYU              1431918169
#define IPU_PIX_FMT_VYUY                  1498765654
#define IPU_PIX_FMT_PYVU               825382478
#define IPU_PIX_FMT_UYVY    			ipu_fourcc('U','Y','V','Y') /*!< 16 YUV 4:2:2 */
#define IPU_PIX_FMT_NV12    			ipu_fourcc('N','V','1','2') /*!< 12 Y/CbCr 4:2:0 */
#define IPU_PIX_FMT_BGR24   		ipu_fourcc('B','G','R','3') /*!< 24 BGR-8-8-8 */
#define IPU_PIX_FMT_YCRCB
#define IPU_PIX_FMT_BGRA32  		ipu_fourcc('B','G','R','A') /*!< 32 BGR-8-8-8-8 */


#define bytes_per_pixel(x) \
    ({ \
        int i=0; \
       switch (x) { \
            case IPU_PIX_FMT_YUV420P: \
            case IPU_PIX_FMT_YUV422P: \
            case IPU_PIX_FMT_NV12: \
                i = 1;\
                break; \
            case IPU_PIX_FMT_RGB565: \
            case IPU_PIX_FMT_YUYV: \
            case IPU_PIX_FMT_UYVY: \
                i = 2;\
                break; \
            case IPU_PIX_FMT_BGR24: \
            case IPU_PIX_FMT_RGB24: \
                i = 3;\
                break; \
            case IPU_PIX_FMT_BGR32: \
            case IPU_PIX_FMT_BGRA32:  \
            case IPU_PIX_FMT_RGB32: \
            case IPU_PIX_FMT_RGBA32: \
            case IPU_PIX_FMT_ABGR32:  \
                i = 4;\
                break; \
            default: \
                i = 1;\
                break; \
        }\
                i = i; \
    })
