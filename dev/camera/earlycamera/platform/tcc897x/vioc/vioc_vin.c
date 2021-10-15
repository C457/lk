
/****************************************************************************
  Copyright (C) 2014 Telechips, Inc.

  This software is licensed under the terms of the GNU General Public
  License version 2, as published by the Free Software Foundation, and
  may be copied, distributed, and modified under those terms.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
****************************************************************************/

#include <reg_physical.h>
#include "vioc_vin.h"

#define 	VIOC_VIN_IREQ_UPD_MASK 			0x00000001UL
#define 	VIOC_VIN_IREQ_EOF_MASK 			0x00000002UL
#define 	VIOC_VIN_IREQ_VS_MASK 			0x00000004UL
#define 	VIOC_VIN_IREQ_INVS_MASK 			0x00000008UL

/* VIN polarity Setting */
void VIOC_VIN_SetSyncPolarity(VIOC_VIN *pVIN, unsigned int hs_active_low, unsigned int vs_active_low, unsigned int field_bfield_low, unsigned int de_active_low, unsigned int gen_field_en, unsigned int pxclk_pol)
{
	//pVIN->uVIN_CTRL.bVIN_CTRL.hs_active_low		= hs_active_low; //9
	//pVIN->uVIN_CTRL.bVIN_CTRL.vs_active_low		= vs_active_low; //10
	//pVIN->uVIN_CTRL.bVIN_CTRL.field_bfield_low	= field_bfield_low; //11
	//pVIN->uVIN_CTRL.bVIN_CTRL.de_active_low		= de_active_low; //12
	//pVIN->uVIN_CTRL.bVIN_CTRL.gen_field_en		= gen_field_en;  //13
	//pVIN->uVIN_CTRL.bVIN_CTRL.pxclk_pol			= pxclk_pol;  //8

	BITCSET(pVIN->uVIN_CTRL.nREG, 0x00003F00, (hs_active_low<<9) | (vs_active_low<<10) | (field_bfield_low<<11) |
											   (de_active_low<<12) | (gen_field_en<<13) | (pxclk_pol<<8) );
}

/* VIN Configuration 1 */
void VIOC_VIN_SetCtrl(VIOC_VIN *pVIN, unsigned int conv_en, unsigned int hsde_connect_en, unsigned int vs_mask, unsigned int fmt, unsigned int data_order)
{
	//pVIN->uVIN_CTRL.bVIN_CTRL.conv_en			= conv_en;	//1
	//pVIN->uVIN_CTRL.bVIN_CTRL.hsde_connect_en	= hsde_connect_en;	//4
	//pVIN->uVIN_CTRL.bVIN_CTRL.vs_mask			= vs_mask;	//6
	//pVIN->uVIN_CTRL.bVIN_CTRL.fmt				= fmt;	//16(4)
	//pVIN->uVIN_CTRL.bVIN_CTRL.data_order		= data_order;		//20(3)

	BITCSET(pVIN->uVIN_CTRL.nREG, 0x00FF0051, (conv_en<<1) | (hsde_connect_en<<4) | (vs_mask<<6) |
											   (fmt<<16) |  (data_order<<20));
}

/* Interlace mode setting */
void VIOC_VIN_SetInterlaceMode(VIOC_VIN *pVIN, unsigned int intl_en, unsigned int intpl_en)
{
	//pVIN->uVIN_CTRL.bVIN_CTRL.intl_en	= intl_en;
	//pVIN->uVIN_CTRL.bVIN_CTRL.intpl_en	= intpl_en;

	BITCSET(pVIN->uVIN_CTRL.nREG, 0x0000000C, (intl_en<<2) | (intpl_en<<3));
}

/* VIN Capture mode Enable */
void VIOC_VIN_SetCaptureModeEnable(VIOC_VIN *pVIN, unsigned int cap_en)
{
	//pVIN->uVIN_CTRL.bVIN_CTRL.capture	= cap_en;
	BITCSET(pVIN->uVIN_CTRL.nREG, 0x80000000, cap_en);
}

/* VIN Enable/Disable */
void VIOC_VIN_SetEnable(VIOC_VIN *pVIN, unsigned int vin_en)
{
	//pVIN->uVIN_CTRL.bVIN_CTRL.enable	= vin_en;
	BITCSET(pVIN->uVIN_CTRL.nREG, 1, vin_en);
}

/* Image size setting */
void VIOC_VIN_SetImageSize(VIOC_VIN *pVIN, unsigned int width, unsigned int height)
{	/* pkjin20 : Chip Bug... To Prevent Read Operation */
	//pVIN->uVIN_SIZE.bVIN_SIZE.width	 	= width;	//(height << 16) | width;
	//pVIN->uVIN_SIZE.bVIN_SIZE.height	= height;

	BITCSET(pVIN->uVIN_SIZE.nREG, 0xFFFFFFFF, (height << 16) | width);	
}

/* Image offset setting */
void VIOC_VIN_SetImageOffset(VIOC_VIN *pVIN, unsigned int offs_width, unsigned int offs_height, unsigned int offs_height_intl)
{
	//pVIN->uVIN_OFFS.bVIN_OFFS.offs_width		= offs_width;
	//pVIN->uVIN_OFFS.bVIN_OFFS.offs_height		= offs_height;
	//pVIN->uVIN_OFFS_INTL.bVIN_OFFS.offs_height	= offs_height_intl;

	BITCSET(pVIN->uVIN_OFFS.nREG, 0xFFFFFFFF, (offs_height << 16) | (offs_width) );
	BITCSET(pVIN->uVIN_OFFS_INTL.nREG, 0xFFFF0000, (offs_height_intl << 16) );
}

void VIOC_VIN_SetImageCropSize(VIOC_VIN *pVIN, unsigned int width, unsigned int height)
{
	BITCSET(pVIN->uVIN_CROP_SIZE.nREG, 0xFFFFFFFF, ((height<<16)|(width)));
}

void VIOC_VIN_SetImageCropOffset(VIOC_VIN *pVIN, unsigned int offs_width, unsigned int offs_height)
{
	BITCSET(pVIN->uVIN_CROP_OFFS.nREG, 0xFFFFFFFF, ((offs_height<<16)|(offs_width)));
}

/* Y2R conversion mode setting */
void VIOC_VIN_SetY2RMode(VIOC_VIN *pVIN, unsigned int y2r_mode)
{
	//pVIN->uVIN_MISC1.bVIN_MISC.y2r_mode	= y2r_mode;
	BITCSET(pVIN->uVIN_MISC.nREG, 0x00000060, (y2r_mode << 5) );
}

/* Y2R conversion Enable/Disable */
void VIOC_VIN_SetY2REnable(VIOC_VIN *pVIN, unsigned int y2r_en)
{
	//pVIN->uVIN_MISC1.bVIN_MISC.y2r_en		= y2r_en;
	BITCSET(pVIN->uVIN_MISC.nREG, 0x00000010, (y2r_en << 4) );
}

/* initialize LUT, for example, LUT values are set to inverse function. */
void VIOC_VIN_SetLUT(VIOC_VIN *pVIN, unsigned int *pLUT)
{
	unsigned int *pLUT0, *pLUT1, *pLUT2, uiCount;
	
	pLUT0 = (unsigned int *)(pLUT  +     0);
	pLUT1 = (unsigned int *)(pLUT0 + 256/4);
	pLUT2 = (unsigned int *)(pLUT1 + 256/4);
	pVIN->uVIN_MISC.bVIN_MISC.lut_apbif_en = 1;	/* Access Look-Up Table Using Slave Port */

	for (uiCount = 0 ; uiCount < 256 ; uiCount = uiCount + 4)
	{	/* Initialize Look-up Table */
		*pLUT0++ = ((uiCount + 3) << 24) |
		           ((uiCount + 2) << 16) |
		           ((uiCount + 1) <<  8) |
		           ((uiCount + 0) <<  0) ;
		*pLUT1++ = ((uiCount + 3) << 24) |
		           ((uiCount + 2) << 16) |
		           ((uiCount + 1) <<  8) |
		           ((uiCount + 0) <<  0) ;
		*pLUT2++ = ((uiCount + 3) << 24) |
		           ((uiCount + 2) << 16) |
		           ((uiCount + 1) <<  8) |
		           ((uiCount + 0) <<  0) ;
	}

	pVIN->uVIN_MISC.bVIN_MISC.lut_apbif_en = 0;	/* Access Look-Up Table Using VIN Module */
}

void VIOC_VIN_SetLUT_by_table(VIOC_VIN *pVIN, unsigned int * pTable)
{
	unsigned int *pLUT0, *pLUT1, *pLUT2, uiCount;

	pLUT0 = &(pVIN->uVIN_LUT_C0);
	pLUT1 = &(pVIN->uVIN_LUT_C1);
	pLUT2 = &(pVIN->uVIN_LUT_C2);

    pVIN->uVIN_MISC.bVIN_MISC.lut_apbif_en = 1; /* Access Look-Up Table Using Slave Port */

	/* Initialize Look-up Table */
	for (uiCount = 0 ; uiCount < 256 ; uiCount = uiCount + 4)
	{	/* Initialize Look-up Table */
		*pLUT0++ = ((((pTable[uiCount + 3] & 0x00ff0000) >> 16) << 24) |
		            (((pTable[uiCount + 2] & 0x00ff0000) >> 16) << 16) |
		            (((pTable[uiCount + 1] & 0x00ff0000) >> 16) <<  8) |
		            (((pTable[uiCount + 0] & 0x00ff0000) >> 16) <<  0)) ;

		*pLUT1++ = ((((pTable[uiCount + 3] & 0x0000ff00) >> 8) << 24) |
		            (((pTable[uiCount + 2] & 0x0000ff00) >> 8) << 16) |
		            (((pTable[uiCount + 1] & 0x0000ff00) >> 8) <<  8) |
		            (((pTable[uiCount + 0] & 0x0000ff00) >> 8) <<  0)) ;

		*pLUT2++ = ((((pTable[uiCount + 3] & 0x000000ff) >> 0) << 24) |
		            (((pTable[uiCount + 2] & 0x000000ff) >> 0) << 16) |
		            (((pTable[uiCount + 1] & 0x000000ff) >> 0) <<  8) |
		            (((pTable[uiCount + 0] & 0x000000ff) >> 0) <<  0)) ;
	}

    pVIN->uVIN_MISC.bVIN_MISC.lut_apbif_en = 0; /* Access Look-Up Table Using VIN Module */
}

/* LUT Enable/Disable */
void VIOC_VIN_SetLUTEnable(VIOC_VIN *pVIN, unsigned int lut0_en, unsigned int lut1_en, unsigned int lut2_en)
{
	//pVIN->uVIN_MISC1.bVIN_MISC.lut0_en = lut0_en;	/* Color Y  (or R) */
	//pVIN->uVIN_MISC1.bVIN_MISC.lut1_en = lut1_en;	/* Color Cb (or G) */
	//pVIN->uVIN_MISC1.bVIN_MISC.lut2_en = lut2_en;	/* Color Cr (or B) */
	BITCSET(pVIN->uVIN_MISC.nREG, 0x00000007, (lut2_en << 2) | (lut1_en << 1) | (lut0_en));
}

void VIOC_VIN_SetDemuxPort(VIOC_VIN_DEMUX *pVINDEMUX, unsigned int p0, unsigned int p1, unsigned int p2, unsigned int p3)
{
	//pVINDEMUX->uVIN_DEMUX_CTRL.bVIN_DEMUX_CTRL.sel0 = p0;
	//pVINDEMUX->uVIN_DEMUX_CTRL.bVIN_DEMUX_CTRL.sel1 = p1;
	//pVINDEMUX->uVIN_DEMUX_CTRL.bVIN_DEMUX_CTRL.sel2 = p2;
	//pVINDEMUX->uVIN_DEMUX_CTRL.bVIN_DEMUX_CTRL.sel3 = p3;

	BITCSET(pVINDEMUX->uVIN_DEMUX_CTRL.nREG, 0x77770000, (p0 << 16) | (p1 << 20) | (p2 << 24) | (p3 << 28));
}

void VIOC_VIN_SetDemuxClock(VIOC_VIN_DEMUX *pVINDEMUX, unsigned int mode)
{
	//pVINDEMUX->uVIN_DEMUX_CTRL.bVIN_DEMUX_CTRL.clk_mode	= mode;

	BITCSET(pVINDEMUX->uVIN_DEMUX_CTRL.nREG, 0x00000700, (mode << 8));
}

void VIOC_VIN_SetDemuxEnable(VIOC_VIN_DEMUX *pVINDEMUX, unsigned int enable)
{
	//pVINDEMUX->uVIN_DEMUX_CTRL.bVIN_DEMUX_CTRL.enable = enable;
	BITCSET(pVINDEMUX->uVIN_DEMUX_CTRL.nREG, 1, enable);
}

/*EOF*/