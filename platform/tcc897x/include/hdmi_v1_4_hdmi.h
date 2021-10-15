
//-------------------------------------------------------------------
// Copyright (c) Telechips, Inc.
// All right reserved.
//
//-------------------------------------------------------------------
/**
 * @file hdmi.h
 * @brief Definition for hdmi/video
 *
 */

#ifndef	_HDMI_H_
#define	_HDMI_H_

#include <lcd.h>
#if defined(TCC_HDMI_USE)

#ifndef TELECHIPS
	#define TELECHIPS
#endif


#ifndef __HDMI_VIDEO_VIDEOFORMAT__
#define __HDMI_VIDEO_VIDEOFORMAT__
/**
 * @enum VideoFormat
 * Video format
 */
enum VideoFormat
{
    /** 640x480p\@60Hz */
    v640x480p_60Hz = 0,
    /** 720x480p\@60Hz */
    v720x480p_60Hz,
    /** 1280x700p\@60Hz */
    v1280x720p_60Hz,
    /** 1920x1080i\@60Hz */
    v1920x1080i_60Hz,
    /** 720x480i\@60Hz */
    v720x480i_60Hz,
    /** 720x240p\@60Hz */
    v720x240p_60Hz,
    /** 2880x480i\@60Hz */
    v2880x480i_60Hz,
    /** 2880x240p\@60Hz */
    v2880x240p_60Hz,
    /** 1440x480p\@60Hz */
    v1440x480p_60Hz,
    /** 1920x1080p\@60Hz */
    v1920x1080p_60Hz,
    /** 720x576p\@60Hz */
    v720x576p_50Hz,  //10
    /** 1280x720p\@50Hz */
    v1280x720p_50Hz,
    /** 1920x1080i\@50Hz (V total = 1125) */
    v1920x1080i_50Hz,
    /** 720x576i\@50Hz */
    v720x576i_50Hz,
    /** 720x288p\@50Hz */
    v720x288p_50Hz,
    /** 2880x576i\@50Hz */
    v2880x576i_50Hz,
    /** 2880x288p\@50Hz */
    v2880x288p_50Hz,
    /** 1440x576p\@50Hz */
    v1440x576p_50Hz,
    /** 1920x1080p\@50Hz */
    v1920x1080p_50Hz,
    /** 1920x1080p\@24Hz */
    v1920x1080p_24Hz,
    /** 1920x1080p\@25Hz */
    v1920x1080p_25Hz,  //20
    /** 1920x1080p\@30Hz */
    v1920x1080p_30Hz,
    /** 2880x480p\@60Hz */
    v2880x480p_60Hz,
    /** 2880x576p\@60Hz */
    v2880x576p_50Hz,
    /** 1920x1080i\@50Hz (V total = 1250) */
    v1920x1080i_50Hz_1250,
    /** 1920x1080i\@100Hz */
    v1920x1080i_100Hz,
    /** 1280x720p\@100Hz */
    v1280x720p_100Hz,
    /** 720x576p\@100Hz */
    v720x576p_100Hz,
    /** 720x576i\@100Hz */
    v720x576i_100Hz,
    /** 1920x1080i\@120Hz */
    v1920x1080i_120Hz,
    /** 1280x720p\@120Hz */
    v1280x720p_120Hz, //30
    /** 720x480p\@120Hz */
    v720x480p_120Hz,
    /** 720x480i\@120Hz */
    v720x480i_120Hz,
    /** 720x576p\@200Hz */
    v720x576p_200Hz,
    /** 720x576i\@200Hz */
    v720x576i_200Hz,
    /** 720x480p\@240Hz */
    v720x480p_240Hz,
    /** 720x480i\@240Hz */
    v720x480i_240Hz,

    /** 1920x720@60Hz */
    v1920x720p_60Hz,    //37
#if defined(TELECHIPS)
	max_video_formats,
#endif
};
#endif /* __HDMI_VIDEO_VIDEOFORMAT__ */
#ifndef __HDMI_VIDEO_COLORSPACE__
#define __HDMI_VIDEO_COLORSPACE__
/**
 * @enum ColorSpace
 * Color space of video stream.
 */
enum ColorSpace
{
    /** RGB color space */
    HDMI_CS_RGB,
    /** YCbCr 4:4:4 color space */
    HDMI_CS_YCBCR444,
    /** YCbCr 4:2:2 color space */
    HDMI_CS_YCBCR422
};
#endif /* __HDMI_VIDEO_COLORSPACE__ */

#ifndef __HDMI_VIDEO_COLORDEPTH__
#define __HDMI_VIDEO_COLORDEPTH__
/**
 * @enum ColorDepth
 * Color depth per pixel of video stream
 */
enum ColorDepth
{
    /** 36 bit color depth per pixel */
    HDMI_CD_36,
    /** 30 bit color depth per pixel */
    HDMI_CD_30,
    /** 24 bit color depth per pixel */
    HDMI_CD_24
};
#endif /* __HDMI_VIDEO_COLORDEPTH__ */

#ifndef __HDMI_VIDEO_HDMIMODE__
#define __HDMI_VIDEO_HDMIMODE__
/**
 * @enum HDMIMode
 * System mode
 */
enum HDMIMode
{
    /** DVI mode */
    DVI = 0,
    /** HDMI mode */
    HDMI
};
#endif /* __HDMI_VIDEO_HDMIMODE__ */

#ifndef __HDMI_VIDEO_PIXELLIMIT__
#define __HDMI_VIDEO_PIXELLIMIT__
/**
 * @enum PixelLimit
 * Pixel limitation of video stream
 */
enum PixelLimit
{
    /** Full range */
    HDMI_FULL_RANGE,
    /** Limit range for RGB color space */
    HDMI_RGB_LIMIT_RANGE,
    /** Limit range for YCbCr color space */
    HDMI_YCBCR_LIMIT_RANGE
};
#endif /* __HDMI_VIDEO_PIXELLIMIT__ */

#ifndef __HDMI_VIDEO_COLORIMETRY__
#define __HDMI_VIDEO_COLORIMETRY__
/**
 * @enum HDMIColorimetry
 * Colorimetry of video stream
 */
enum HDMIColorimetry
{
    /** Colorimetry is not defined */
    HDMI_COLORIMETRY_NO_DATA,
    /** ITU601 colorimetry */
    HDMI_COLORIMETRY_ITU601,
    /** ITU709 colorimetry */
    HDMI_COLORIMETRY_ITU709,
    /** Extended ITU601 colorimetry */
    HDMI_COLORIMETRY_EXTENDED_xvYCC601,
    /** Extended ITU709 colorimetry */
    HDMI_COLORIMETRY_EXTENDED_xvYCC709
};
#endif /* __HDMI_VIDEO_COLORIMETRY__ */

#ifndef __HDMI_VIDEO_PIXELASPECTRATIO__
#define __HDMI_VIDEO_PIXELASPECTRATIO__
/**
 * @enum PixelAspectRatio
 * Pixel aspect ratio of video stream
 */
enum PixelAspectRatio
{
    /** 4:3 pixel ratio */
    HDMI_PIXEL_RATIO_4_3,
    /** 16:9 pixel ratio */
    HDMI_PIXEL_RATIO_16_9
};
#endif /* __HDMI_VIDEO_PIXELASPECTRATIO__ */


#ifndef __HDMI_AUDIO_AUDIOSAMPLEPACKETTYPE__
#define __HDMI_AUDIO_AUDIOSAMPLEPACKETTYPE__
/**
 * @enum HDMIASPType
 * Type of HDMI audio sample packet
 */
enum HDMIASPType
{
    /** Audio Sample Packet Type */
    HDMI_ASP,
    /** One Bit Audio Packet Type */
    HDMI_DSD,
    /** High Bit Rate Packet Type */
    HDMI_HBR,
    /** DST Packet Type */
    HDMI_DST
};
#endif /* __HDMI_AUDID_AUDIOSAMPLEPACKETTYPE__ */

#ifndef __HDMI_PHY_PIXELFREQUENCY__
#define __HDMI_PHY_PIXELFREQUENCY__

/**
 * @enum PHYFreq
 * PHY Frequency
 */
enum PHYFreq
{
	/** Not supported */
	PHY_FREQ_NOT_SUPPORTED = -1,
	/** 25.2 MHz pixel frequency */
	PHY_FREQ_25_200 = 0,
	/** 25.175 MHz pixel frequency */
	PHY_FREQ_25_175,
	/** 27 MHz pixel frequency */
	PHY_FREQ_27,
	/** 27.027 MHz pixel frequency */
	PHY_FREQ_27_027,
	/** 54 MHz pixel frequency */
	PHY_FREQ_54,
	/** 54.054 MHz pixel frequency */
	PHY_FREQ_54_054,
	/** 74.25 MHz pixel frequency */
	PHY_FREQ_74_250,
	/** 74.176 MHz pixel frequency */
	PHY_FREQ_74_176,
	/** 148.5 MHz pixel frequency */
	PHY_FREQ_148_500,
	/** 148.352 MHz pixel frequency */
	PHY_FREQ_148_352,
	/** 108.108 MHz pixel frequency */
	PHY_FREQ_108_108,
	/** 72 MHz pixel frequency */
	PHY_FREQ_72,
	/** 25 MHz pixel frequency */
	PHY_FREQ_25,
	/** 65 MHz pixel frequency */
	PHY_FREQ_65,
	/** 108 MHz pixel frequency */
	PHY_FREQ_108,
	/** 162 MHz pixel frequency */
	PHY_FREQ_162,
	/** 59.4 MHz pixel frequency */
	PHY_FREQ_59_400,
	/** 97.5 MHz pixel frequency */
	PHY_FREQ_97_340,		//17

};

#endif /* __HDMI_PHY_PIXELFREQUENCY__ */

#ifndef __HDMI_DEVICE_VIDEO_PARAMS__
#define __HDMI_DEVICE_VIDEO_PARAMS__
/**
 * @struct device_video_params
 * Video timing paramters to set HDMI H/W @n
 * For more information, refer to HDMI register map doc.
 */
struct device_video_params
{
	/** [H Total] */
	unsigned int HTotal;

	/** [H Blank] */
	unsigned int HBlank;

	/** [V Total] */
	unsigned int VTotal;

	/** [V Blank] : in interlaced mode, this is VBlank - 0.5
	*               but, VIC ID = 39, this is VBlank
	*/
	unsigned int VBlank;

	/** [HFront] */
	unsigned int HFront;

	/** [HSync] */
	unsigned int HSync;

	/** H Sync polarity */
	unsigned int HPol;

	/** [VFront] */
	unsigned int VFront;

	/** [VSync] */
	unsigned int VSync;

	/** V Sync polarity */
	unsigned int VPol;

	/** CEA VIC */
	unsigned int AVI_VIC;

	/** CEA VIC for 16:9 pixel ratio */
	unsigned int AVI_VIC_16_9;

	/** 0 - progresive, 1 - interlaced */
	unsigned int interlaced;

	/** Pixel repetition if double, set 1 */
	unsigned int repetition;
};

#endif /* __HDMI_DEVICE_VIDEO_PARAMS__ */

#if defined(TELECHIPS)
#ifndef __HDMI_DEVICE_LCDC_TIMING_PARAMS__
#define __HDMI_DEVICE_LCDC_TIMING_PARAMS__
/**
 * @struct device_lcdc_timing_params
 * Video timing paramters to set LCDC H/W @n
 * For more information, refer to LCDC register map doc.
 */
typedef struct _lcdc_timimg_parms_t {

	unsigned int id;
	unsigned int iv;
	unsigned int ih;
	unsigned int ip;
	unsigned int dp;
	unsigned int ni;
	unsigned int tv;
	unsigned int tft;
	unsigned int stn;

	//LHTIME1
	unsigned int lpw;
	unsigned int lpc;
	//LHTIME2
	unsigned int lswc;
	unsigned int lewc;
	//LVTIME1
	unsigned int vdb;
	unsigned int vdf;
	unsigned int fpw;
	unsigned int flc;
	//LVTIME2
	unsigned int fswc;
	unsigned int fewc;
	//LVTIME3
	unsigned int fpw2;
	unsigned int flc2;
	//LVTIME4
	unsigned int fswc2;
	unsigned int fewc2;

}lcdc_timimg_parms_t;


#if (0)
#define DEFAULT_HDMI_LCDC_TIMING	{ 0,  1,  1,  0,  0,  1,  0,  1,  0,  61 ,    639 ,   59 ,     37 ,    0,  0,  5,  479 ,   29,  8,   5,   479 ,    29,  8, /*640x480p @ 60Hz       25.200MHz   */ }
#else
#define DEFAULT_HDMI_LCDC_TIMING	{ 0,  0,  0,  0,  0,  1,  0,  1,  0,  43 ,    1919,   147,     87 ,    0,  0,  4,  1079,   35,  3,   4,   1079,    35,  3, /*1920x1080p @ 60Hz     148.5MHz    */ }
#endif

static const lcdc_timimg_parms_t LCDCTimimgParams[] = 
{
#if 0                                              /*  ID  IV  IH  IP  DP  NI  TV TFT STN  LPW       LPC   LSWC     LEWC   VDB VDF FPW    FLC  FSWC FEWC FPW2   FLC2  FSWC2 FEWC2 */
  /* v1024x768p_60Hz        PIXEL_FREQ_65    ,  */    { 0,  1,  1,  0,  0,  1,  0,  1,  0,  135,    1023,   159,     23 ,    0,  0,  5,  767 ,   28,  2,   5,   767 ,    28,  2, /*1024x768p @ 60Hz       65.000MHz   */ },
#else
  /* v640x480p_60Hz         PIXEL_FREQ_25_200,  */    { 0,  1,  1,  0,  0,  1,  0,  1,  0,  61 ,    639 ,   59 ,     37 ,    0,  0,  5,  479 ,   29,  8,   5,   479 ,    29,  8, /*640x480p @ 60Hz       25.200MHz   */ },
#endif
  /* v720x480p_60Hz         PIXEL_FREQ_27_027,  */    { 0,  1,  1,  0,  0,  1,  0,  1,  0,  61 ,    719 ,   59 ,     15 ,    0,  0,  5,  479 ,   29,  8,   5,   479 ,    29,  8, /*720x480p @ 60Hz       27.027MHz   */ },
  /* v1280x720p_60Hz        PIXEL_FREQ_74_250,  */    { 0,  0,  0,  0,  0,  1,  0,  1,  0,  39 ,    1279,   219,     109,    0,  0,  4,  719 ,   19,  4,   4,   719 ,    19,  4, /*1280x720p @ 60Hz      74.25MHz    */ },
  /* v1920x1080i_60Hz       PIXEL_FREQ_74_250,  */    { 0,  0,  0,  0,  0,  0,  1,  0,  0,  43 ,    1919,   147,     87 ,    0,  0,  9,  1079,   29,  3,   9,   1079,    31,  3, /*1920x1080i @ 60Hz     74.25MHz    */ },
  /* v720x480i_60Hz         PIXEL_FREQ_27_027,  */    { 0,  1,  1,  0,  1,  0,  1,  0,  0,  123,    1439,   113,     37 ,    0,  0,  5,  479 ,   29,  7,   5,   479 ,    31,  7, /*720x480i @ 60Hz       27.027MHz   */ },
  /* v720x240p_60Hz         PIXEL_FREQ_27_027,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v2880x480i_60Hz        PIXEL_FREQ_54_054,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v2880x240p_60Hz        PIXEL_FREQ_54_054,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1440x480p_60Hz        PIXEL_FREQ_54_054,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1920x1080p_60Hz       PIXEL_FREQ_148_500, */    { 0,  0,  0,  0,  0,  1,  0,  1,  0,  43 ,    1919,   147,     87 ,    0,  0,  4,  1079,   35,  3,   4,   1079,    35,  3, /*1920x1080p @ 60Hz     148.5MHz    */ },
  /* v720x576p_50Hz         PIXEL_FREQ_27,      */    { 0,  1,  1,  0,  0,  1,  0,  1,  0,  63 ,    719 ,   67 ,     11 ,    0,  0,  4,  575 ,   38,  4,   4,   575 ,    38,  4, /*720x576p @ 50Hz       27MHz       */ },
  /* v1280x720p_50Hz        PIXEL_FREQ_74_250,  */    { 0,  0,  0,  0,  0,  1,  0,  1,  0,  39 ,    1279,   219,     439,    0,  0,  4,  719 ,   19,  4,   4,   719 ,    19,  4, /*1280x720p @ 50Hz      74.25MHz    */ },
  /* v1920x1080i_50Hz       PIXEL_FREQ_74_250,  */    { 0,  0,  0,  0,  0,  0,  1,  0,  0,  43 ,    1919,   174,     500,    0,  0,  9,  1079,   29,  3,   9,   1079,    31,  3, /*1920x1080i @ 50Hz     74.25MHz    */ },
  /* v720x576i_50Hz         PIXEL_FREQ_27,      */    { 0,  1,  1,  0,  1,  0,  1,  0,  0,  125,    1439,   137,     23 ,    0,  0,  5,  575 ,   37,  3,   5,   575 ,    39,  3, /*720x576i @ 50Hz       27MHz       */ },
  /* v720x288p_50Hz         PIXEL_FREQ_27,      */    DEFAULT_HDMI_LCDC_TIMING,
  /* v2880x576i_50Hz        PIXEL_FREQ_54,      */    DEFAULT_HDMI_LCDC_TIMING,
  /* v2880x288p_50Hz        PIXEL_FREQ_54,      */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1440x576p_50Hz        PIXEL_FREQ_54,      */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1920x1080p_50Hz       PIXEL_FREQ_148_500, */    { 0,  0,  0,  0,  0,  1,  0,  1,  0,  43 ,    1919,   174,     500,    0,  0,  4,  1079,   35,  3,   4,   1079,    35,  3, /*1920x1080p @ 50Hz     148.5MHz    */ },
  /* v1920x1080p_24Hz       PIXEL_FREQ_74_250,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1920x1080p_25Hz       PIXEL_FREQ_74_250,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1920x1080p_30Hz       PIXEL_FREQ_74_250,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v2880x480p_60Hz        PIXEL_FREQ_108_108, */    DEFAULT_HDMI_LCDC_TIMING,
  /* v2880x576p_50Hz        PIXEL_FREQ_108,     */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1920x1080i_50Hz(1250) PIXEL_FREQ_72,      */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1920x1080i_100Hz      PIXEL_FREQ_148_500, */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1280x720p_100Hz       PIXEL_FREQ_148_500, */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x576p_100Hz        PIXEL_FREQ_54,      */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x576i_100Hz        PIXEL_FREQ_54,      */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1920x1080i_120Hz      PIXEL_FREQ_148_500, */    DEFAULT_HDMI_LCDC_TIMING,
  /* v1280x720p_120Hz       PIXEL_FREQ_148_500, */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x480p_120Hz        PIXEL_FREQ_54_054,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x480i_120Hz        PIXEL_FREQ_54_054,  */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x576p_200Hz        PIXEL_FREQ_108,     */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x576i_200Hz        PIXEL_FREQ_108,     */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x480p_240Hz        PIXEL_FREQ_108_108, */    DEFAULT_HDMI_LCDC_TIMING,
  /* v720x480i_240Hz        PIXEL_FREQ_108_108, */    DEFAULT_HDMI_LCDC_TIMING,


//mobis first
  /* v1920x720p_60Hz       PIXEL_FREQ_97_340, */  //     { 0,  0,  0,  0,  0,  1,  0,  1,  0,   19 ,    1919,     79,     59 ,   0,  0,  0,     719,   56,  1,   0,   719,     56,  1,   /*1920x720p @ 60Hz     97.34MHz    */ },
#ifdef WIDE_LCD_TIANMA 
  /* TIANMA 10.25 */
  /* v1920x720p_60Hz       PIXEL_FREQ_97_340, */       { 0,  0,  0,  0,  0,  1,  0,  1,  0,    19 ,   1919,     23,     19 ,   0,  0,   3,     719,   17,  17,    3,   719,     17, 17,    /*1920x720p @ 60Hz     97.34MHz    */ },
#elif defined(HDMI_1920_720_12_3)
	/* v1920x720p_60Hz       PIXEL_FREQ_97_340, */       { 0,  0,  0,  0,  0,  1,  0,  1,  0,    19 ,   1919,     69,     69 ,   0,  0,   3,     719,   17,  17,    3,   719,     17, 17,    /*1920x720p @ 60Hz     97.34MHz    */ },
#endif


};

static const struct hdmi_video_mode_list_t {
	enum VideoFormat 		vfmt_val;
	enum PixelAspectRatio	ratio;
	unsigned int			width;
	unsigned int			height;
	unsigned char			vfmt_str[32];
} gRefHdmiVideoModeList[16] = 
{
/* [ 0] */ { v1920x1080p_60Hz, HDMI_PIXEL_RATIO_16_9,  1920,   1080,   "1920x1080P 60Hz [16:9]"  },
/* [ 1] */ { v1920x1080p_50Hz, HDMI_PIXEL_RATIO_16_9,  1920,   1080,   "1920x1080P 50Hz [16:9]"  },
/* [ 2] */ { v1920x1080i_60Hz, HDMI_PIXEL_RATIO_16_9,  1920,   1080,   "1920x1080I 60Hz [16:9]"  },
/* [ 3] */ { v1920x1080i_50Hz, HDMI_PIXEL_RATIO_16_9,  1920,   1080,   "1920x1080I 50Hz [16:9]"  },
/* [ 4] */ { v1280x720p_60Hz , HDMI_PIXEL_RATIO_16_9,  1280,    720,   " 1280x720P 60Hz [16:9]"  },
/* [ 5] */ { v1280x720p_50Hz , HDMI_PIXEL_RATIO_16_9,  1280,    720,   " 1280x720P 50Hz [16:9]"  },
/* [ 6] */ { v720x480p_60Hz  , HDMI_PIXEL_RATIO_16_9,   720,    480,   "  720x480P 60Hz [16:9]"  },
/* [ 7] */ { v720x480p_60Hz  , HDMI_PIXEL_RATIO_4_3 ,   720,    480,   "  720x480P 60Hz [ 4:3]"  },
/* [ 8] */ { v720x480i_60Hz  , HDMI_PIXEL_RATIO_16_9,   720,    480,   "  720x480I 60Hz [16:9]"  },
/* [ 9] */ { v720x480i_60Hz  , HDMI_PIXEL_RATIO_4_3 ,   720,    480,   "  720x480I 60Hz [ 4:3]"  },
/* [10] */ { v720x576p_50Hz  , HDMI_PIXEL_RATIO_16_9,   720,    576,   "  720x576P 50Hz [16:9]"  },
/* [11] */ { v720x576p_50Hz  , HDMI_PIXEL_RATIO_4_3 ,   720,    576,   "  720x576P 50Hz [ 4:3]"  },
/* [12] */ { v720x576i_50Hz  , HDMI_PIXEL_RATIO_16_9,   720,    576,   "  720x576I 50Hz [16:9]"  },
/* [13] */ { v720x576i_50Hz  , HDMI_PIXEL_RATIO_4_3 ,   720,    576,   "  720x576I 50Hz [ 4:3]"  },
/* [14] */ { v640x480p_60Hz  , HDMI_PIXEL_RATIO_4_3 ,   640,    480,   "  640x480P 60Hz [ 4:3]"  },
/* [15] */ { v1920x720p_60Hz  , HDMI_PIXEL_RATIO_4_3 ,  1920,  720,   " 1920x720 60Hz [ 8:3]"  }

};
#endif /* __HDMI_DEVICE_VIDEO_PARAMS__ */
#endif /* TELECHIPS */


#ifndef __HDMI_VIDEO_PIXELFREQUENCY__
#define __HDMI_VIDEO_PIXELFREQUENCY__
/**
 * @enum PixelFreq
 * Pixel Frequency
 */
#if defined(DEFAULT_DISPLAY_LCD)
enum PixelFreq {
    /** 25.2 MHz pixel frequency */
    PIXEL_FREQ_25_200 = 2520,
    /** 25.175 MHz pixel frequency */
    PIXEL_FREQ_25_175 = 2517,
    /** 27 MHz pixel frequency */
    PIXEL_FREQ_27 = 2700,
    /** 27.027 MHz pixel frequency */
    PIXEL_FREQ_27_027 = 2702,
    /** 54 MHz pixel frequency */
    PIXEL_FREQ_54 = 5400,
    /** 54.054 MHz pixel frequency */
    PIXEL_FREQ_54_054 = 5405,
    /** 74.25 MHz pixel frequency */
    PIXEL_FREQ_74_250 = 7425,
    /** 74.176 MHz pixel frequency */
    PIXEL_FREQ_74_176 = 7417,
    /** 148.5 MHz pixel frequency */
    PIXEL_FREQ_148_500 = 14850,
    /** 148.352 MHz pixel frequency */
    PIXEL_FREQ_148_352 = 14835,
    /** 108.108 MHz pixel frequency */
    PIXEL_FREQ_108_108 = 10810,
    /** 72 MHz pixel frequency */
    PIXEL_FREQ_72 = 7200,
    /** 25 MHz pixel frequency */
    PIXEL_FREQ_25 = 2500,
    /** 65 MHz pixel frequency */
    PIXEL_FREQ_65 = 6500,
    /** 108 MHz pixel frequency */
    PIXEL_FREQ_108 = 10800,
    /** 162 MHz pixel frequency */
    PIXEL_FREQ_162 = 16200,
};
#endif//
#endif /* __HDMI_VIDEO_PIXELFREQUENCY__ */

#ifndef __HDMI_VIDEO_SOURCE__
#define __HDMI_VIDEO_SOURCE__
/**
 * @enum HDMIVideoSource
 * Type of video source.
 */
enum HDMIVideoSource
{
	/** Internal Video Source */
	HDMI_SOURCE_INTERNAL,
	/** External Video Source */
	HDMI_SOURCE_EXTERNAL,
};
#endif /* __HDMI_VIDEO_SOURCE__ */

#ifndef __HDMI_3D_VIDEO_STRUCTURE__
#define __HDMI_3D_VIDEO_STRUCTURE__
/**
 * @enum HDMI3DVideoStructure
 * Type of 3D Video Structure
 */
enum HDMI3DVideoStructure
{
	/** 2D Video Format  */
	HDMI_2D_VIDEO_FORMAT = -1,
	/** 3D Frame Packing Structure */
	HDMI_3D_FP_FORMAT = 0,
	/** 3D Field Alternative Structure  */
	HDMI_3D_FA_FORMAT,
	/** 3D Line Alternative Structure */
	HDMI_3D_LA_FORMAT,
	/** Side-by-Side(Full)Structure */
	HDMI_3D_SSF_FORMAT,
	/** 3D L+Depth Structure */
	HDMI_3D_LD_FORMAT,
	/** 3D L+Depth+Graphics Structure */
	HDMI_3D_LDGFX_FORMAT,
	/** 3D Top-and-Bottom Structure */
	HDMI_3D_TB_FORMAT,
	/** HDMI VIC Structure (ex. 4Kx2K) */
	HDMI_VIC_FORMAT,
	/** Side-by-Side(Half)Structure */
	HDMI_3D_SSH_FORMAT,
};
#endif /* __HDMI_3D_VIDEO_STRUCTURE__ */

#ifndef __HDMI_VIDEO_PARAMETERS__
#define __HDMI_VIDEO_PARAMETERS__

//! Structure for HDMI video
struct HDMIVideoParameter {
    /** Video interface */
    enum HDMIMode mode;
    /** Video format */
    enum VideoFormat resolution;
    /** Color space */
    enum ColorSpace colorSpace;
    /** Color depth */
    enum ColorDepth colorDepth;
    /** Colorimetry */
    enum HDMIColorimetry colorimetry;
    /** Pixel aspect ratio */
    enum PixelAspectRatio pixelAspectRatio;
	/** Video Source */
	enum HDMIVideoSource videoSrc;
	/** 3D Video Structure */
	enum HDMI3DVideoStructure hdmi_3d_format;
};
#endif /* __HDMI_VIDEO_PARAMETERS__ */

#endif//defined(TCC_HDMI_USE)

#endif /* _VIDEO_H_ */
