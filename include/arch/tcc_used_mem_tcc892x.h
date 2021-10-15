/****************************************************************************
 *	 TCC Version 0.6
 *	 Copyright (c) telechips, Inc.
 *	 ALL RIGHTS RESERVED
 *
****************************************************************************/

#define USE_VIQE
//#define USE_EXT_CAMERA // for UVC Camera
//#define VIDEO_DUAL_DISPLAY
#define SUPPORT_CCFB
//#define HDMI_UI_DISPLAY_MEM
#define SUPPORT_ROTATION
//#define SUPPORT_MULTIVIDEO
#ifdef CONFIG_ARM_TRUSTZONE
#define SUPPORT_SECURED_INBUFER
#else
//#define SUPPORT_SECURED_INBUFER     //for WiFi Display
#endif

/* Memory Allocations */
#define PMEM_SURF_SIZE			(0)

#ifdef USE_VIQE
#if defined(FEATURE_USE_8M_CAMERA)
#define PMEM_VIQE_SIZE			(22*SZ_1M)
#else
#define PMEM_VIQE_SIZE			(21*SZ_1M)
#endif
#else
#define PMEM_VIQE_SIZE			(0)
#endif

#define UMP_RESERVED_SIZE		(37*SZ_1M)//40 for 720p

#ifdef USE_EXT_CAMERA
#define EXT_CAM_SIZE 			(21*SZ_1M)
#else
#define EXT_CAM_SIZE 			(0)
#endif

// This is for big LCD panel.
#define FB_WIDTH 		1280
#define FB_HEIGHT 		800
#define FB_HDMI_WIDTH              1280
#define FB_HDMI_HEIGHT             800
#define FB_MEM_SIZE		(ARRAY_MBYTE(FB_WIDTH * FB_HEIGHT * 4 * 4) + (ARRAY_MBYTE(FB_HDMI_WIDTH * FB_HDMI_HEIGHT * 4 * 4))) // framebuffer use defined memory area.


//for Overlay
#if defined(SUPPORT_ROTATION)
#define OVERLAY_0_MEM_SIZE			(12 * 2 * SZ_1M)
#else
#define OVERLAY_0_MEM_SIZE			(8 * 3 * SZ_1M)
#endif

#ifdef SUPPORT_CCFB
#define OVERLAY_1_MEM_SIZE			(24 * SZ_1M)
#else
#define OVERLAY_1_MEM_SIZE			(16 * SZ_1M)
#endif

#ifdef SUPPORT_ROTATION
#define OVERLAY_ROTATION_MEM_SIZE                      (4 * SZ_1M)
#else
#define OVERLAY_ROTATION_MEM_SIZE                      (0)
#endif

#define OVERLAY_MEM_TOTAL_SIZE		(OVERLAY_0_MEM_SIZE + OVERLAY_1_MEM_SIZE+OVERLAY_ROTATION_MEM_SIZE)

#ifdef SUPPORT_SECURED_INBUFER
#define SECURED_IN_MEM_SIZE		(6 * SZ_1M)
#define VIDEO_SBACKUP_MEM_SIZE		(2 * SZ_1M)
#define VIDEO_THUMB_MEM_SIZE		(6 * SZ_1M)
#else
#define SECURED_IN_MEM_SIZE		(0)
#define VIDEO_SBACKUP_MEM_SIZE		(0)
#define VIDEO_THUMB_MEM_SIZE		(0)
#endif

#if (TCC_MEM_SIZE <= 256)
/* We restrict video resolution to 720p for 256MB or DDR3 */
#define VIDEO_MEM_TOTAL_SIZE		(36*SZ_1M)
#else
#if defined(SUPPORT_SECURED_INBUFER)
#define VIDEO_MEM_TOTAL_SIZE		((130 * SZ_1M) - (OVERLAY_MEM_TOTAL_SIZE + SECURED_IN_MEM_SIZE + VIDEO_SBACKUP_MEM_SIZE))//(78*SZ_1M)
#else
#define VIDEO_MEM_TOTAL_SIZE		(68*SZ_1M)
#endif
#endif
#ifdef SUPPORT_MULTIVIDEO
#define VIDEO_MEM_EXT_SIZE        (24*SZ_1M)//42 for 720p
#else
#define VIDEO_MEM_EXT_SIZE        (0)
#endif

#define HDMI_DISPLAY_MAX_WIDTH		1920 
#define HDMI_DISPLAY_MAX_HEIGHT		1080 

#ifdef HDMI_UI_DISPLAY_MEM
// FB scale up buffer   	default: 1920 * 1080 * 2 	= 4Mbye
#define FB_SCALE_SIZE			(ARRAY_MBYTE(HDMI_DISPLAY_MAX_WIDTH * HDMI_DISPLAY_MAX_HEIGHT * 4))
// Fb rotation /format converting    : 1280 * 800 * 2  	= 2Mbyte
#define FB_G2D_SIZE				(ARRAY_MBYTE(FB_WIDTH * FB_HEIGHT * 4))
#else
#define FB_SCALE_SIZE			0
#define FB_G2D_SIZE				0
#endif//
// FB  g2d converter & scaler up total buffer size  	default : (1 * 2) + (4 * 2) = 10M
#define FB_SCALE_MEM_TOTAL_SIZE	((FB_SCALE_SIZE * 2) + (FB_G2D_SIZE * 2))

#ifdef VIDEO_DUAL_DISPLAY
#define VIDEO_DUAL_DISPLAY_SIZE (ARRAY_MBYTE(HDMI_DISPLAY_MAX_WIDTH * HDMI_DISPLAY_MAX_HEIGHT * 2 * 2))
#else
#define VIDEO_DUAL_DISPLAY_SIZE 0
#endif

#define FB_WMIXER_MEM_SIZE			(4 * SZ_1M)

#define RAM_CONSOLE_SIZE		(1*SZ_1M)

#define JPEG_ENC_CAPTURE_SIZE		(3 * SZ_1M)
#define JPEG_RAW_CAPTURE_SIZE		(3 * SZ_1M)

#define TOTAL_FIXED_MEM_SIZE		(PMEM_SURF_SIZE + PMEM_VIQE_SIZE+ UMP_RESERVED_SIZE + EXT_CAM_SIZE  + FB_MEM_SIZE + OVERLAY_MEM_TOTAL_SIZE + VIDEO_MEM_TOTAL_SIZE + FB_SCALE_MEM_TOTAL_SIZE + VIDEO_DUAL_DISPLAY_SIZE + FB_WMIXER_MEM_SIZE + RAM_CONSOLE_SIZE + JPEG_ENC_CAPTURE_SIZE + JPEG_RAW_CAPTURE_SIZE+VIDEO_MEM_EXT_SIZE+SECURED_IN_MEM_SIZE+VIDEO_SBACKUP_MEM_SIZE+VIDEO_THUMB_MEM_SIZE+FB_WMIXER_MEM_SIZE)

#if 1//(TCC_MEM_SIZE > 512)
#define PMEM_SURF_BASE			(MEM_PHYS_OFFSET + 0x8000000) // 0x2000000 -> 0x6000000
#else
#define PMEM_SURF_BASE			(MEM_PHYS_OFFSET + TOTAL_SDRAM_SIZE - TOTAL_FIXED_MEM_SIZE)
#endif

#define SECURED_IN_MEM_BASE		(PMEM_SURF_BASE + PMEM_SURF_SIZE)
#define VIDEO_SBACKUP_MEM_BASE	(SECURED_IN_MEM_BASE + SECURED_IN_MEM_SIZE)
#define OVERLAY_MEM_BASE		(VIDEO_SBACKUP_MEM_BASE + VIDEO_SBACKUP_MEM_SIZE)
#define VIDEO_MEM_BASE			(OVERLAY_MEM_BASE + OVERLAY_MEM_TOTAL_SIZE)
#define VIDEO_THUMB_MEM_BASE	(VIDEO_MEM_BASE + VIDEO_MEM_TOTAL_SIZE)
#define UMP_RESERVED_BASE		(VIDEO_THUMB_MEM_BASE + VIDEO_THUMB_MEM_SIZE)
#define FB_WMIXER_MEM_BASE		(UMP_RESERVED_BASE + UMP_RESERVED_SIZE)
#define RAM_CONSOLE_BASE		(FB_WMIXER_MEM_BASE + FB_WMIXER_MEM_SIZE)
#define FB_WMIXER_EXT_MEM_BASE	(RAM_CONSOLE_BASE + RAM_CONSOLE_SIZE)
#define PMEM_VIQE_BASE			(FB_WMIXER_EXT_MEM_BASE + FB_WMIXER_MEM_SIZE)
#define EXT_CAM_BASE			(PMEM_VIQE_BASE + PMEM_VIQE_SIZE)
#define FB_MEM_BASE				(EXT_CAM_BASE + EXT_CAM_SIZE)
#define FB_SCALE_MEM_BASE		(FB_MEM_BASE + FB_MEM_SIZE)
#define VIDEO_DUAL_DISPLAY_BASE	(FB_SCALE_MEM_BASE + FB_SCALE_MEM_TOTAL_SIZE)
#define JPEG_ENC_CAPTURE_BASE	(VIDEO_DUAL_DISPLAY_BASE + VIDEO_DUAL_DISPLAY_SIZE)
#define JPEG_RAW_CAPTURE_BASE	(JPEG_ENC_CAPTURE_BASE + JPEG_ENC_CAPTURE_SIZE)
#define VIDEO_MEM_EXT_BASE      (JPEG_RAW_CAPTURE_BASE + JPEG_RAW_CAPTURE_SIZE)

/*=============================================================================*/
#if defined(FEATURE_USE_8M_CAMERA)
#define JPEG_RAW_MEM_SIZE 				(16*SZ_1M)
#else
#define JPEG_RAW_MEM_SIZE 				(15*SZ_1M)
#endif
#define PA_JPEG_RAW_BASE_ADDR 		(PMEM_VIQE_BASE + PMEM_VIQE_SIZE - JPEG_RAW_MEM_SIZE)

#define JPEG_STREAM_MEM_SIZE 			(5*SZ_1M)
#define PA_JPEG_STREAM_BASE_ADDR 		(PA_JPEG_RAW_BASE_ADDR - JPEG_STREAM_MEM_SIZE)
#define JPEG_TOTAL_MEM_SIZE 	 		(JPEG_RAW_MEM_SIZE + JPEG_STREAM_MEM_SIZE)

#define TCC_JPEG_HEADER_SIZE 			(1*SZ_1M)
#define PA_JPEG_HEADER_BASE_ADDR 		(PA_JPEG_STREAM_BASE_ADDR - TCC_JPEG_HEADER_SIZE)

//for Camera
#define PREVIEW_MEM_SIZE 				(UMP_RESERVED_SIZE)
#define CAPTURE_MEM_SIZE 				(UMP_RESERVED_SIZE)
#define PA_PREVIEW_BASE_ADDR 			(UMP_RESERVED_BASE)

//for Video
#define VIDEO_PHY_ADDR			(VIDEO_MEM_BASE)
#define VIDEO_MEM_SIZE			(VIDEO_MEM_TOTAL_SIZE)

//for Overlay
#define OVERLAY_0_PHY_ADDR		(OVERLAY_MEM_BASE)
#define OVERLAY_1_PHY_ADDR		(OVERLAY_MEM_BASE + OVERLAY_0_MEM_SIZE)
#define OVERLAY_ROTATION_PHY_ADDR              (OVERLAY_MEM_BASE + OVERLAY_0_MEM_SIZE + OVERLAY_1_MEM_SIZE)

//	FB_SCALE_MEM_TOTAL_SIZE
#define FB_SCALE_ADDR0				FB_SCALE_MEM_BASE
#define FB_SCALE_ADDR1				(FB_SCALE_ADDR0 + FB_SCALE_SIZE)

#define FB_G2D_ADDR0			(FB_SCALE_ADDR1 + FB_SCALE_SIZE)
#define FB_G2D_ADDR1			(FB_G2D_ADDR0 + FB_G2D_SIZE)

