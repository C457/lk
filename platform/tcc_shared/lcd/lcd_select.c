/****************************************************************************
 *   FileName    : lcd_FLD0800.c
 *   Description : support for FLD0800 LVDS Panel
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips Inc.
 *   All rights reserved 
 
This source code contains confidential information of Telechips.
Any unauthorized use without a written permission of Telechips including not limited to re-distribution in source or binary form is strictly prohibited.
This source code is provided "AS IS" and nothing contained in this source code shall constitute any express or implied warranty of any kind, including without limitation, any warranty of merchantability, fitness for a particular purpose or non-infringement of any patent, copyright or other third party intellectual property right. No warranty is made, express or implied, regarding the information's accuracy, completeness, or performance. 
In no event shall Telechips be liable for any claim, damages or other liability arising from, out of or in connection with this source code or the use in the source code. 
This source code is provided subject to the terms of a Mutual Non-Disclosure Agreement between Telechips and Company.
*
****************************************************************************/
#include <debug.h>
#include <stdlib.h>
#include <string.h>
#include <tcc_lcd.h>


#include <debug.h>
#include <stdlib.h>
#include <string.h>
#include <reg.h>
#include <platform/iomap.h>
#include <dev/fbcon.h>

#include <lcd.h>
#include <dev/gpio.h>
#include <platform/gpio.h>
#include <daudio_ver.h>

#include <tnftl/IO_TCCXXX.h>

//#include <platform/reg_physical.h>
#include <platform/structures_display.h>


#define     LVDS_VCO_45MHz        45000000
#define     LVDS_VCO_60MHz        60000000
extern void tca_ckc_setpmupwroff( unsigned int periname , unsigned int isenable);
#ifdef BACKLIGHT_PDM
extern int tcc_pwm_config(unsigned int npwm, int duty_ns, int period_ns);
#endif//

unsigned int lvds_stbyb;

static int fld0800_panel_init(struct lcd_panel *panel)
{
	printf("%s : %d\n", __func__, 0);

	return 0;
}

static int fld0800_set_power(struct lcd_panel *panel, int on)
{
	unsigned int P, M, S, VSEL, TC;

	PDDICONFIG		pDDICfg 	= (DDICONFIG *)HwDDI_CONFIG_BASE;
	struct lcd_platform_data *pdata = &(panel->dev);
	printf("%s : %d \n", __func__, on);

	if (on) {
		lcdc_initialize(pdata->lcdc_num, panel);
//		LCDC_IO_Set(pdata->lcdc_num, panel->bus_width);
		
		//LVDS 6bit setting for internal dithering option !!!
		//tcc_lcdc_dithering_setting(pdata->lcdc_num);

		// LVDS reset	
	   	pDDICfg->LVDS_CTRL.bREG.RST = 1;
		pDDICfg->LVDS_CTRL.bREG.RST = 0;		



		BITCSET(pDDICfg->LVDS_TXO_SEL0.nREG, 0xFFFFFFFF, 0x13121110);
		BITCSET(pDDICfg->LVDS_TXO_SEL1.nREG, 0xFFFFFFFF, 0x09081514);
		BITCSET(pDDICfg->LVDS_TXO_SEL2.nREG, 0xFFFFFFFF, 0x0D0C0B0A);
		BITCSET(pDDICfg->LVDS_TXO_SEL3.nREG, 0xFFFFFFFF, 0x03020100);
		BITCSET(pDDICfg->LVDS_TXO_SEL4.nREG, 0xFFFFFFFF, 0x1A190504);
		BITCSET(pDDICfg->LVDS_TXO_SEL5.nREG, 0xFFFFFFFF, 0x0E171618);
		BITCSET(pDDICfg->LVDS_TXO_SEL6.nREG, 0xFFFFFFFF, 0x1F07060F);
		BITCSET(pDDICfg->LVDS_TXO_SEL7.nREG, 0xFFFFFFFF, 0x1F1E1F1E);
		BITCSET(pDDICfg->LVDS_TXO_SEL8.nREG, 0xFFFFFFFF, 0x001E1F1E);

		//LVDS P,M,S,VSEL select

		if( panel->clk_freq >= LVDS_VCO_45MHz && panel->clk_freq < LVDS_VCO_60MHz)
		{
			#if defined (VIOC_TCC8930)
				   M = 7; P = 7; S = 0; VSEL = 0; TC = 4;
			#elif defined (VIOC_TCC8960) || defined(VIOC_TCC8970)
//				   M = 10; P = 10; S = 1; VSEL = 0; TC = 4;
				   M = 10; P = 10; S = 2; VSEL = 0; TC = 4;
			#endif//
		}
		else
		{
			#if defined (VIOC_TCC8930)
					M = 10; P = 10; S = 0; VSEL = 0; TC = 4;		
			#elif defined (VIOC_TCC8960) || defined(VIOC_TCC8970)
//					M = 14; P = 14; S = 1; VSEL = 1; TC = 4;		
					M = 10; P = 10; S = 1; VSEL = 0; TC = 4;		
			#endif//
		}
           
		printf("%s : M  = %d, P = %d, S = %d, VSEL = %d, TC = %d \n", __func__, M, P, S, VSEL, TC);
		BITCSET(pDDICfg->LVDS_CTRL.nREG, 0x00FFFFF0, (VSEL<<4)|(S<<5)|(M<<8)|(P<<15)|(TC<<21)); //LCDC1

		#if defined (VIOC_TCC8960) || defined(VIOC_TCC8970)
		pDDICfg->LVDS_MISC1.bREG.LC = 0;
		pDDICfg->LVDS_MISC1.bREG.CC = 0;
		pDDICfg->LVDS_MISC1.bREG.CMS = 0;
		pDDICfg->LVDS_MISC1.bREG.VOC = 1;
		#endif//
		// LVDS Select LCDC 1
		if(pdata->lcdc_num ==1)
			BITCSET(pDDICfg->LVDS_CTRL.nREG, 0x3 << 30 , 0x1 << 30);
		else
			BITCSET(pDDICfg->LVDS_CTRL.nREG, 0x3 << 30 , 0x0 << 30);


		pDDICfg->LVDS_CTRL.bREG.RST = 1;        //  reset

	    	// LVDS enable
	  	pDDICfg->LVDS_CTRL.bREG.EN = 1;   // enable


	}
	else 	{
		tcclcd_gpio_set_value(pdata->power_on, 0);
	}
	lcd_delay_us(1600000);
	
	return 0;
}

static int fld0800_set_backlight_level(struct lcd_panel *panel, int level)
{
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d\n", __func__, level);

	return 0;
}

static int hdmi1280x720_panel_init(struct lcd_panel *panel)
{
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d\n", __func__, 0);
	tcclcd_gpio_config(pdata->display_on, 1);
	tcclcd_gpio_config(pdata->bl_on, 1);
	tcclcd_gpio_config(pdata->reset, 1);
	tcclcd_gpio_config(pdata->power_on, 1);

	return 0;
}
static int hdmi1280x720_set_power(struct lcd_panel *panel, int on)
{
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d \n", __func__, on);
	if (on) {
		tcclcd_gpio_set_value(pdata->power_on, 1);
		tcclcd_gpio_set_value(pdata->reset, 1);

		mdelay(10);
		lcdc_initialize(1, panel);
		
		LCDC_IO_Set(1, 1, panel->bus_width);

		mdelay(16);
		
		tcclcd_gpio_set_value(pdata->display_on, 1);

	}
	else 
	{

		tcclcd_gpio_set_value(pdata->display_on, 0);
		mdelay(10);
		tcclcd_gpio_set_value(pdata->reset, 0);
		tcclcd_gpio_set_value(pdata->power_on, 0);
		LCDC_IO_Disable(0, panel->bus_width);
	}

	return 0;
}

static int hdmi1280x720_set_backlight_level(struct lcd_panel *panel, int level)
{
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d\n", __func__, level);
	
	if (level == 0) {
		tcclcd_gpio_set_value(pdata->bl_on, 0);
	} else {
		tcclcd_gpio_set_value(pdata->bl_on, 1);
	}

	return 0;
}

static struct lcd_panel Truly_lvds1280x720_8 = {
	.name		= "LVDS1280x720_8_0",
	.manufacturer	= "Truly",
	.id		= PANEL_ID_FLD0800,
	.xres		= 1280,
	.yres		= 720,
	.width		= 60,
	.height		= 72,
	.bpp		= 24,

	.clk_freq	= 63700000,
	.clk_div	= 2,
	.bus_width	= 24,
	
	.lpw		= 2,
	.lpc		= 1280,
	.lswc		= 12,
	.lewc		= 44,

	.vdb		= 0,
	.vdf		= 0,

	.fpw1		= 2,
	.flc1		= 720,
	.fswc1		= 1,
	.fewc1		= 67,
	
	.fpw2		= 2,
	.flc2		= 720,
	.fswc2		= 1,
	.fewc2		= 67,

	.sync_invert	= IV_INVERT | IH_INVERT,
	.init		= fld0800_panel_init,
	.set_power	= fld0800_set_power,
	.set_backlight_level = fld0800_set_backlight_level,
};

static struct lcd_panel BOE_hdmi1280x720_8 = {
        .name           = "HDMI1280x720_8_0",
        .manufacturer   = "BOE",
        .id             = PANEL_ID_HDMI,
        .xres           = 1280,
        .yres           = 720,
        .width          = 60,
        .height         = 72,
        .bpp            = 24,

        .clk_freq       = 64000000,
        .clk_div        = 2,
        .bus_width      = 24,

        .lpw            = 32,
        .lpc            = 1280,
        .lswc           = 80,
        .lewc           = 48,

        .vdb            = 0,
        .vdf            = 0,

        .fpw1           = 5,
        .flc1           = 720,
        .fswc1          = 13,
        .fewc1          = 3,

        .fpw2           = 5,
        .flc2           = 720,
        .fswc2          = 13,
        .fewc2          = 3,

        .sync_invert    = IV_INVERT | IH_INVERT,
        .init           = hdmi1280x720_panel_init,
        .set_power      = hdmi1280x720_set_power,
        .set_backlight_level = hdmi1280x720_set_backlight_level,
};

static struct lcd_panel BOE_lvds1280x720_8 = {
        .name           = "LVDS1280x720_8_0",
        .manufacturer   = "BOE",
        .id             = PANEL_ID_FLD0800,
        .xres           = 1280,
        .yres           = 720,
        .width          = 176,
        .height         = 99,
        .bpp            = 24,

        .clk_freq       = 64200000,
        .clk_div        = 2,
        .bus_width      = 24,

        .lpw            = 31,
        .lpc            = 1279,
        .lswc           = 79,
        .lewc           = 47,

        .vdb            = 0,
        .vdf            = 0,

        .fpw1           = 4,
        .flc1           = 719,
        .fswc1          = 12,
        .fewc1          = 4,

        .fpw2           = 4,
        .flc2           = 719,
        .fswc2          = 12,
        .fewc2          = 4,

        .sync_invert    = IV_INVERT | IH_INVERT,
	.init           = fld0800_panel_init,
        .set_power      = fld0800_set_power,
        .set_backlight_level = fld0800_set_backlight_level,
};

static struct lcd_panel LGD_hdmi1920x720_10_25 = {
	.name		= "HDMI1920x720_10_25",
	.manufacturer	= "LGDisplay",
	.id		= PANEL_ID_HDMI,
	.xres		= 1920,
	.yres		= 720,
	.width		= 177,
	.height		= 100,
	.bpp		= 32,
	.clk_freq 	= 89600000,
	.clk_div	= 2,
	.bus_width	= 24,
	.lpw		= 40,
	.lpc		= 1920,
	.lswc		= 44,
	.lewc		= 44,
	.vdb		= 0,
	.vdf		= 0,
	.fpw1		= 2,
	.flc1		= 720,
	.fswc1		= 3,
	.fewc1		= 4,
	.fpw2		= 2,
	.flc2		= 720,
	.fswc2		= 3,
	.fewc2		= 4,
	.sync_invert	= IV_INVERT | IH_INVERT,
	.init		= hdmi1280x720_panel_init,
	.set_power	= hdmi1280x720_set_power,
	.set_backlight_level = hdmi1280x720_set_backlight_level,
};

static struct lcd_panel LGD_hdmi1920x720_12_3 = {
        .name           = "HDMI1920x720_12_3",
        .manufacturer   = "LGDisplay",
        .id             = PANEL_ID_HDMI,
        .xres           = 1920,
        .yres           = 720,
        .width          = 177,
        .height         = 100,
        .bpp            = 32,
        .clk_freq       = 98500000,
        .clk_div        = 2,
        .bus_width      = 24,
        .lpw            = 32,
        .lpc            = 1920,
        .lswc           = 144,
        .lewc           = 48,
        .vdb            = 0,
        .vdf            = 0,
        .fpw1           = 5,
        .flc1           = 720,
        .fswc1          = 26,
        .fewc1          = 15,
        .fpw2           = 5,
        .flc2           = 720,
        .fswc2          = 26,
        .fewc2          = 15,
        .sync_invert    = IV_INVERT | IH_INVERT,
        .init           = hdmi1280x720_panel_init,
        .set_power      = hdmi1280x720_set_power,
        .set_backlight_level = hdmi1280x720_set_backlight_level,
};


struct lcd_panel *tccfb_get_panel(void)
{
	unsigned char lcd_ver = get_daudio_lcd_ver();

	if(gpio_get(TCC_GPB(24))) // OE
		switch(lcd_ver)
		{
			case DAUDIOKK_LCD_OI_10_25_1920_720_INCELL_Si_LG: //0
			case DAUDIOKK_LCD_OI_10_25_1920_720_INCELL_Si_2_LG: //1
			case DAUDIOKK_LCD_OD_10_25_1920_720_INCELL_Si_LG: //3
			case DAUDIOKK_LCD_OD_10_25_1920_720_INCELL_LTPS_LG: //5
			case DAUDIOKK_LCD_OI_10_25_1920_720_INCELL_LTPS_LG: //7
				return &LGD_hdmi1920x720_10_25;
				break;

			case DAUDIOKK_LCD_OD_12_30_1920_720_INCELL_Si_LG: //4
                                return &LGD_hdmi1920x720_12_3;
                                break;

			case DAUDIOKK_LCD_OD_08_00_1280_720_OGS_Si_BOE: //6 HDMI SERDES
				return &BOE_hdmi1280x720_8;
				break;
			case DAUDIOKK_LCD_OI_DISCONNECTED: //10
				return &LGD_hdmi1920x720_10_25;
				break;
			case DAUDIOKK_LCD_OI_08_00_1280_720_OGS_Si_BOE: //8
				return &BOE_lvds1280x720_8;
                                break;
			default:
				dprintf(INFO,"ADC value is wrong number : %d\n",lcd_ver);
				return &LGD_hdmi1920x720_10_25;
				break;
		}
	else			// PIO
		switch(lcd_ver)
                {
                        case DAUDIOKK_LCD_PI_10_25_1920_720_PIO_AUO: //5
                                return &LGD_hdmi1920x720_10_25;
                                break;
                        case DAUDIOKK_LCD_PI_08_00_800_400_PIO_TRULY: //6
                                return &Truly_lvds1280x720_8;
                                break;
                        case DAUDIOKK_LCD_OI_DISCONNECTED: //10 Temporary use DAUDIOKK_LCD_PI_08_00_800_400_PIO_TRULY
                                return &Truly_lvds1280x720_8;
                                break;
                        default:
                                dprintf(INFO,"ADC value is wrong number : %d\n",lcd_ver);
				return &LGD_hdmi1920x720_10_25;
                                break;
                }

}

