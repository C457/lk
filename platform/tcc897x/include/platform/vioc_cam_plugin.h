/*
 * Author:  <linux@telechips.com>
 * Created: June 10, 2008
 * Description: TCC VIOC h/w block 
 *
 * Copyright (C) 2008-2009 Telechips
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see the file COPYING, or write
 * to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __VIOC_CAM_PLUGIN_H__
#define	__VIOC_CAM_PLUGIN_H__


#include "../../vioc/vioc_wmix.h"
#include "../../vioc/vioc_rdma.h"
#include "../../vioc/vioc_wdma.h"
#include "../../vioc/vioc_scaler.h"
#include "../../vioc/vioc_config.h"
#include "../../vioc/vioc_vin.h"

#define 	ORDER_RGB 			0
#define 	ORDER_RBG 			1
#define 	ORDER_GRB 			2
#define 	ORDER_GBR 			3
#define 	ORDER_BRG 			4
#define 	ORDER_BGR 			5

#define 	FMT_YUV422_16BIT 			0
#define 	FMT_YUV422_8BIT 			1
#define 	FMT_YUVK4444_16BIT 		2
#define 	FMT_YUVK4224_24BIT 		3
#define 	FMT_RGBK4444_16BIT 		4
#define 	FMT_RGB444_24BIT 			9
#define 	FMT_SD_PROG 				12 // NOT USED

////////////////////////////////////////////////////////////////////////////////
//
//	IMAGE FORMAT
//		- VIOC_RDMA
//		- VIOC_WDMA
//
	#define VIOC_IMG_FMT_BPP1			0		// 1bit
	#define VIOC_IMG_FMT_BPP2			1		// 2bits
	#define VIOC_IMG_FMT_BPP4			2		// 4bits
	#define VIOC_IMG_FMT_BPP8			3		// 1byte
	#define VIOC_IMG_FMT_RGB332			8 		// 1byte
	#define VIOC_IMG_FMT_ARGB4444		9		// 2bytes
	#define VIOC_IMG_FMT_RGB565			10		// 2bytes
	#define VIOC_IMG_FMT_ARGB1555		11		// 2bytes
	#define VIOC_IMG_FMT_ARGB8888		12		// 4bytes
	#define VIOC_IMG_FMT_ARGB6666_4		13		// 4bytes
	#define	VIOC_IMG_FMT_RGB888			14		// 3bytes	: newly supported : 3 bytes format
	#define VIOC_IMG_FMT_ARGB6666_3		15		// 3bytes	: newly supported : 3 bytes format
	#define	VIOC_IMG_FMT_COMP			16		// 4bytes
	#define	VIOC_IMG_FMT_DECOMP			(VIOC_IMG_FMT_COMP)
	#define	VIOC_IMG_FMT_UYVY			22		// 2bytes	: LSB [Y/U/Y/V] MSB : newly supported : 2 bytes format
	#define	VIOC_IMG_FMT_VYUY			23		// 2bytes	: LSB [Y/V/Y/U] MSB : newly supported : 2 bytes format
	#define VIOC_IMG_FMT_YUV420SEP		24		// 1,1byte
	#define	VIOC_IMG_FMT_YUV422SEP		25		// 1,1byte
	#define	VIOC_IMG_FMT_YUYV			26		// 2bytes	: LSB [Y/U/Y/V] MSB : previous SEQ
	#define	VIOC_IMG_FMT_YVYU			27		// 2bytes	: LSB [Y/V/Y/U] MSB : newly supported : 2 bytes format
	#define	VIOC_IMG_FMT_YUV420IL0		28		// 1,2byte
	#define	VIOC_IMG_FMT_YUV420IL1		29		// 1,2byte
	#define	VIOC_IMG_FMT_YUV422IL0		30		// 1,2bytes
	#define	VIOC_IMG_FMT_YUV422IL1		31		// 1,2bytes

//------------------------------------------------
// PXDW FORMAT for DISP
//------------------------------------------------
	#define VIOC_PXDW_FMT_04_STN        0
	#define VIOC_PXDW_FMT_08_STN        1
	#define VIOC_PXDW_FMT_08_RGB_STRIPE 2
	#define VIOC_PXDW_FMT_16_RGB565     3
	#define VIOC_PXDW_FMT_15_RGB555     4
	#define VIOC_PXDW_FMT_18_RGB666     5
	#define VIOC_PXDW_FMT_08_UY         6
	#define VIOC_PXDW_FMT_08_VY         7
	#define VIOC_PXDW_FMT_16_YU         8
	#define VIOC_PXDW_FMT_16_YV         9
	#define VIOC_PXDW_FMT_08_RGB_DELTA0 10  // 1st : R->G->B,   2nd : G->B->R
	#define VIOC_PXDW_FMT_08_RGB_DELTA1 11  // 1st : G->B->R,   2nd : R->G->B
	#define VIOC_PXDW_FMT_24_RGB888     12
	#define VIOC_PXDW_FMT_08_RGBD       13  // R->G->B->Dummy->R->G
	#define VIOC_PXDW_FMT_16_RGB666     14
	#define VIOC_PXDW_FMT_16_RGB888     15////////////////////////////////////////////////////////////////////////////////
//
//	Plug-In/Out Related
//

#define	VIOC_SC0						(0)
#define	VIOC_SC1						(1)
#define	VIOC_SC2						(2)
#define	VIOC_SC3						(3)
#define	VIOC_SC_RDMA_00			(00)
#define	VIOC_SC_RDMA_01			(01)
#define	VIOC_SC_RDMA_02			(02)
#define	VIOC_SC_RDMA_03			(03)
#define	VIOC_SC_RDMA_04			(04)
#define	VIOC_SC_RDMA_05			(05)
#define	VIOC_SC_RDMA_06			(06)
#define	VIOC_SC_RDMA_07			(07)
#define	VIOC_SC_RDMA_08			(08)
#define	VIOC_SC_RDMA_09			(09)
#define	VIOC_SC_RDMA_10			(10)
#define	VIOC_SC_RDMA_11			(11)
#define	VIOC_SC_RDMA_12			(12)
#define	VIOC_SC_RDMA_13			(13)
#define	VIOC_SC_RDMA_14			(14)
#define	VIOC_SC_RDMA_15			(15)
#define	VIOC_SC_VIN_00 			(16)
#define	VIOC_SC_RDMA_16			(17)
#define	VIOC_SC_VIN_01 			(18)
#define	VIOC_SC_RDMA_17			(19)
#define	VIOC_SC_WDMA_00			(20)
#define	VIOC_SC_WDMA_01			(21)
#define	VIOC_SC_WDMA_02			(22)
#define	VIOC_SC_WDMA_03			(23)
#define	VIOC_SC_WDMA_04			(24)
#define	VIOC_SC_WDMA_05			(25)
#define	VIOC_SC_WDMA_06			(26)
#define	VIOC_SC_WDMA_07			(27)
#define	VIOC_SC_WDMA_08			(28)

#define	VIOC_VIQE						(4)
#if 1
#define	VIOC_VIQE_RDMA_00		(00)
#define	VIOC_VIQE_RDMA_01		(01)
#define	VIOC_VIQE_RDMA_02		(02)
#define	VIOC_VIQE_RDMA_03		(03)
#define	VIOC_VIQE_RDMA_06		(04)
#define	VIOC_VIQE_RDMA_07		(05)
#define	VIOC_VIQE_RDMA_10		(06)
#define	VIOC_VIQE_RDMA_11		(07)
#define	VIOC_VIQE_RDMA_12		(8)
#define	VIOC_VIQE_RDMA_14		(9)
#define	VIOC_VIQE_VIN_00 		(10)
#define	VIOC_VIQE_RDMA_16		(11)
#define	VIOC_VIQE_VIN_01 		(12)
#define	VIOC_VIQE_RDMA_17		(13)
#else
#define	VIOC_VIQE_RDMA_02		(00)
#define	VIOC_VIQE_RDMA_03		(01)
#define	VIOC_VIQE_RDMA_06		(02)
#define	VIOC_VIQE_RDMA_07		(03)
#define	VIOC_VIQE_RDMA_10		(04)
#define	VIOC_VIQE_RDMA_11		(05)
#define	VIOC_VIQE_RDMA_12		(06)
#define	VIOC_VIQE_VIN_00 		(07)
#define	VIOC_VIQE_VIN_01 		(08)
#endif

#define	VIOC_DEINTLS					(5)
#define	VIOC_DEINTLS_RDMA_00	(VIOC_VIQE_RDMA_00		)
#define	VIOC_DEINTLS_RDMA_01	(VIOC_VIQE_RDMA_01		)
#define	VIOC_DEINTLS_RDMA_02	(VIOC_VIQE_RDMA_02		)
#define	VIOC_DEINTLS_RDMA_03	(VIOC_VIQE_RDMA_03		)
#define	VIOC_DEINTLS_RDMA_06	(VIOC_VIQE_RDMA_06		)
#define	VIOC_DEINTLS_RDMA_07	(VIOC_VIQE_RDMA_07		)
#define	VIOC_DEINTLS_RDMA_10	(VIOC_VIQE_RDMA_10		)
#define	VIOC_DEINTLS_RDMA_11	(VIOC_VIQE_RDMA_11		)
#define	VIOC_DEINTLS_RDMA_12	(VIOC_VIQE_RDMA_12		)
#define	VIOC_DEINTLS_RDMA_14	(VIOC_VIQE_RDMA_14		)
#define	VIOC_DEINTLS_VIN_00 	(VIOC_VIQE_VIN_00 		)
#define	VIOC_DEINTLS_RDMA_16	(VIOC_VIQE_RDMA_16		)
#define	VIOC_DEINTLS_VIN_01 	(VIOC_VIQE_VIN_01 		)
#define	VIOC_DEINTLS_RDMA_17	(VIOC_VIQE_RDMA_17		)

#define	VIOC_FILT2D 					(6)
#define	VIOC_FILT2D_SC0_IN		(0)
#define	VIOC_FILT2D_SC0_OUT		(1)
#define	VIOC_FILT2D_SC1_IN		(2)
#define	VIOC_FILT2D_SC1_OUT		(3)
#define	VIOC_FILT2D_SC2_IN		(4)
#define	VIOC_FILT2D_SC2_OUT		(5)
#define	VIOC_FILT2D_SC3_IN		(6)
#define	VIOC_FILT2D_SC3_OUT		(7)

#define	VIOC_FCDEC0 					(7 )
#define	VIOC_FCDEC1 					(8 )
#define	VIOC_FCDEC2 					(9 )
#define	VIOC_FCDEC3 					(10)
#define	VIOC_FCDEC_RDMA_00		(00)
#define	VIOC_FCDEC_RDMA_01		(01)
#define	VIOC_FCDEC_RDMA_02		(02)
#define	VIOC_FCDEC_RDMA_03		(03)
#define	VIOC_FCDEC_RDMA_04		(04)
#define	VIOC_FCDEC_RDMA_05		(05)
#define	VIOC_FCDEC_RDMA_06		(06)
#define	VIOC_FCDEC_RDMA_07		(07)
#define	VIOC_FCDEC_RDMA_08		(08)
#define	VIOC_FCDEC_RDMA_09		(09)
#define	VIOC_FCDEC_RDMA_10		(10)
#define	VIOC_FCDEC_RDMA_11		(11)
#define	VIOC_FCDEC_RDMA_12		(12)
#define	VIOC_FCDEC_RDMA_13		(13)
#define	VIOC_FCDEC_RDMA_14		(14)
#define	VIOC_FCDEC_RDMA_15		(15)
#define	VIOC_FCDEC_RDMA_16		(16)
#define	VIOC_FCDEC_RDMA_17		(17)

#define	VIOC_FCENC0 					(11)
#define	VIOC_FCENC1 					(12)
#define	VIOC_FCENC_WDMA_00		(00)
#define	VIOC_FCENC_WDMA_01		(01)
#define	VIOC_FCENC_WDMA_02		(02)
#define	VIOC_FCENC_WDMA_03		(03)
#define	VIOC_FCENC_WDMA_04		(04)
#define	VIOC_FCENC_WDMA_05		(05)
#define	VIOC_FCENC_WDMA_06		(06)
#define	VIOC_FCENC_WDMA_07		(07)
#define	VIOC_FCENC_WDMA_08		(08)

#define	VIOC_FDELAY0					(13)
#define	VIOC_FDELAY1					(14)
#define	VIOC_FDELAY_SC0_OUT		(0)
#define	VIOC_FDELAY_SC1_OUT		(1)
#define	VIOC_FDELAY_SC2_OUT		(2)
#define	VIOC_FDELAY_SC3_OUT		(3)

#define	VIOC_DEBLOCK					(15)
#define	VIOC_DEBLOCK_FILT0_IN	(0)
#define	VIOC_DEBLOCK_FILT0_OUT	(1)
#define	VIOC_DEBLOCK_FILT1_IN	(2)
#define	VIOC_DEBLOCK_FILT1_OUT	(3)
#define	VIOC_DEBLOCK_FILT2_IN	(4)
#define	VIOC_DEBLOCK_FILT2_OUT	(5)
#define	VIOC_DEBLOCK_FILT3_IN	(6)
#define	VIOC_DEBLOCK_FILT3_OUT	(7)

/* VIOC DRIVER STATUS TYPE */
#define	VIOC_DEVICE_INVALID      	(-2)
#define	VIOC_DEVICE_BUSY 			(-1)
#define	VIOC_DEVICE_CONNECTED 		( 0)

/* VIOC DRIVER ERROR TYPE */
#define VIOC_DRIVER_ERR_INVALID 	(-3)
#define VIOC_DRIVER_ERR_BUSY 		(-2)
#define VIOC_DRIVER_ERR 			(-1)
#define VIOC_DRIVER_NOERR 			( 0)

/* VIOC PATH STATUS TYPE */
#define VIOC_PATH_DISCONNECTED			(0)		// 
#define VIOC_PATH_CONNECTING  			(1)		// 
#define VIOC_PATH_CONNECTED   			(2)		// 
#define VIOC_PATH_DISCONNECTING			(3)		// 

#define VIOC_WMIX0 						(0)
#define VIOC_WMIX1 						(1)
#define VIOC_WMIX2 						(2)
#define VIOC_WMIX3 						(3)
#define VIOC_WMIX4 						(4)
#define VIOC_WMIX5 						(5)
#define VIOC_WMIX6 						(6)
#define	VIOC_WMIX_RDMA_00		(00)
#define	VIOC_WMIX_RDMA_01		(01)
#define	VIOC_WMIX_RDMA_02		(02)
#define	VIOC_WMIX_RDMA_03		(03)
#define	VIOC_WMIX_RDMA_04		(04)
#define	VIOC_WMIX_RDMA_05		(05)
#define	VIOC_WMIX_RDMA_06		(06)
#define	VIOC_WMIX_RDMA_07		(07)
#define	VIOC_WMIX_RDMA_08		(08)
#define	VIOC_WMIX_RDMA_09		(09)
#define	VIOC_WMIX_RDMA_10		(10)
#define	VIOC_WMIX_RDMA_11		(11)
#define	VIOC_WMIX_RDMA_12		(12)
#define	VIOC_WMIX_RDMA_13		(13)
#define	VIOC_WMIX_RDMA_14		(14)
#define	VIOC_WMIX_RDMA_15		(15)
#define	VIOC_WMIX_VIN_00 		(16)
#define	VIOC_WMIX_RDMA_16		(17)
#define	VIOC_WMIX_VIN_01 		(18)
#define	VIOC_WMIX_RDMA_17		(19)
#define	VIOC_WMIX_WDMA_00		(20)
#define	VIOC_WMIX_WDMA_01		(21)
#define	VIOC_WMIX_WDMA_02		(22)
#define	VIOC_WMIX_WDMA_03		(23)
#define	VIOC_WMIX_WDMA_04		(24)
#define	VIOC_WMIX_WDMA_05		(25)
#define	VIOC_WMIX_WDMA_06		(26)
#define	VIOC_WMIX_WDMA_07		(27)
#define	VIOC_WMIX_WDMA_08		(28)

//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	CPUIF FORMAT
//
#define	CPUIF_OFMT_08_TYPE0   0 // LCDC : 8-8-8, SI(08bits) 8-8-8
#define	CPUIF_OFMT_24_TYPE0   4 // LCDC : 8-8-8, SI(16bits) 9-9
#define	CPUIF_OFMT_24_TYPE1   5
#define	CPUIF_OFMT_24_TYPE2   6
#define	CPUIF_OFMT_24_TYPE3   7
#define	CPUIF_OFMT_18_TYPE0   8
#define	CPUIF_OFMT_18_TYPE1   9
#define	CPUIF_OFMT_18_TYPE2   10
#define	CPUIF_OFMT_18_TYPE3   11
#define	CPUIF_OFMT_16_TYPE0   12
#define	CPUIF_OFMT_16_TYPE1   13
#define	CPUIF_OFMT_16_TYPE2   14

//
//	CPUIF FORMAT
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	WDMA SYNC SELECTION
//

#define	VIOC_WDMA_SYNC_ABSOLUTE		(0<<0)	// SYNCMD_ADDR
#define	VIOC_WDMA_SYNC_RELATIVE		(1<<0)	// SYNCMD_ADDR       
#define	VIOC_WDMA_SYNC_START_EDGE 	(0<<0)	// SYNCMD_SENS
#define	VIOC_WDMA_SYNC_START_LEVEL	(1<<0)	// SYNCMD_SENS

#define	VIOC_WDMA_SYNC_RDMA00		( 0)
#define	VIOC_WDMA_SYNC_RDMA01		( 1)
#define	VIOC_WDMA_SYNC_RDMA02		( 2)
#define	VIOC_WDMA_SYNC_RDMA03		( 3)
#define	VIOC_WDMA_SYNC_RDMA04		( 4)
#define	VIOC_WDMA_SYNC_RDMA05		( 5)
#define	VIOC_WDMA_SYNC_RDMA06		( 6)
#define	VIOC_WDMA_SYNC_RDMA07		( 7)
#define	VIOC_WDMA_SYNC_RDMA08		( 8)
#define	VIOC_WDMA_SYNC_RDMA09		( 9)
#define	VIOC_WDMA_SYNC_RDMA10		(10)
#define	VIOC_WDMA_SYNC_RDMA11		(11)
#define	VIOC_WDMA_SYNC_RDMA12		(12)
#define	VIOC_WDMA_SYNC_RDMA13		(13)
#define	VIOC_WDMA_SYNC_RDMA14		(14)
#define	VIOC_WDMA_SYNC_RDMA15		(15)
#define	VIOC_WDMA_SYNC_RDMA16		(16)
#define	VIOC_WDMA_SYNC_RDMA17		(17)
//
//	WDMA SYNC SELECTION
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	FIFO
//
#define	VIOC_FIFO_RDMA00			( 0)
#define	VIOC_FIFO_RDMA01			( 1)
#define	VIOC_FIFO_RDMA02			( 2)
#define	VIOC_FIFO_RDMA03			( 3)
#define	VIOC_FIFO_RDMA04			( 4)
#define	VIOC_FIFO_RDMA05			( 5)
#define	VIOC_FIFO_RDMA06			( 6)
#define	VIOC_FIFO_RDMA07			( 7)
#define	VIOC_FIFO_RDMA08			( 8)
#define	VIOC_FIFO_RDMA09			( 9)
#define	VIOC_FIFO_RDMA10			(10)
#define	VIOC_FIFO_RDMA11			(11)
#define	VIOC_FIFO_RDMA12			(12)
#define	VIOC_FIFO_RDMA13			(13)
#define	VIOC_FIFO_RDMA14			(14)
#define	VIOC_FIFO_RDMA15			(15)

#define	VIOC_FIFO_WDMA00			( 0)
#define	VIOC_FIFO_WDMA01			( 1)
#define	VIOC_FIFO_WDMA02			( 2)
#define	VIOC_FIFO_WDMA03			( 3)
#define	VIOC_FIFO_WDMA04			( 4)
#define	VIOC_FIFO_WDMA05			( 5)
#define	VIOC_FIFO_WDMA06			( 6)
#define	VIOC_FIFO_WDMA07			( 7)
#define	VIOC_FIFO_WDMA08			( 8)

//
//	FIFO
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	OUTPUT CONFIGURATION
//
#define		VIOC_OUTCFG_HDMI			0
#define		VIOC_OUTCFG_SDVENC			1
#define		VIOC_OUTCFG_HDVENC			2
#define		VIOC_OUTCFG_M80				3
#define		VIOC_OUTCFG_MRGB			4

#define		VIOC_OUTCFG_DISP0			0
#define		VIOC_OUTCFG_DISP1			1
#define		VIOC_OUTCFG_DISP2			2

//
//	OUTPUT CONFIGURATION
//
////////////////////////////////////////////////////////////////////////////////

#define	VIOC_LUT_RDMA_00		(00)
#define	VIOC_LUT_RDMA_01		(01)
#define	VIOC_LUT_RDMA_02		(02)
#define	VIOC_LUT_RDMA_03		(03)
#define	VIOC_LUT_RDMA_04		(04)
#define	VIOC_LUT_RDMA_05		(05)
#define	VIOC_LUT_RDMA_06		(06)
#define	VIOC_LUT_RDMA_07		(07)
#define	VIOC_LUT_RDMA_08		(08)
#define	VIOC_LUT_RDMA_09		(09)
#define	VIOC_LUT_RDMA_10		(10)
#define	VIOC_LUT_RDMA_11		(11)
#define	VIOC_LUT_RDMA_12		(12)
#define	VIOC_LUT_RDMA_13		(13)
#define	VIOC_LUT_RDMA_14		(14)
#define	VIOC_LUT_RDMA_15		(15)
#define	VIOC_LUT_VIN_00 			(16)
#define	VIOC_LUT_RDMA_16		(17)
#define	VIOC_LUT_VIN_01 			(18)
#define	VIOC_LUT_RDMA_17		(19)
#define	VIOC_LUT_WDMA_00		(20)
#define	VIOC_LUT_WDMA_01		(21)
#define	VIOC_LUT_WDMA_02		(22)
#define	VIOC_LUT_WDMA_03		(23)
#define	VIOC_LUT_WDMA_04		(24)
#define	VIOC_LUT_WDMA_05		(25)
#define	VIOC_LUT_WDMA_06		(26)
#define	VIOC_LUT_WDMA_07		(27)
#define	VIOC_LUT_WDMA_08		(28)

//
//	End of Plug-In/Out
//
////////////////////////////////////////////////////////////////////////////////

#endif



