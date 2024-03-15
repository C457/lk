 /****************************************************************************
 *   FileName    : at070tn93_lcd.c 
 *   Description : support for TPO AT070TN93 LCD
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

#if 0//def AT070TN93
#include <lcd.h>
#include <dev/gpio.h>
#include <tca_tco.h>

#include <platform/gpio.h>

#ifdef BACKLIGHT_PDM
extern int tcc_pwm_config(unsigned int npwm, int duty_ns, int period_ns);
#endif//

static int at070tn93_panel_init(struct lcd_panel *panel)
{
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d  \n", __func__, 0);

	tcclcd_gpio_config(pdata->display_on, GPIO_OUTPUT);
	tcclcd_gpio_config(pdata->bl_on, GPIO_OUTPUT);
	tcclcd_gpio_config(pdata->reset, GPIO_OUTPUT);
	tcclcd_gpio_config(pdata->power_on, GPIO_OUTPUT);

	tcclcd_gpio_set_value(pdata->display_on, 0);
	tcclcd_gpio_set_value(pdata->bl_on, 0);
	tcclcd_gpio_set_value(pdata->reset, 0);
	tcclcd_gpio_set_value(pdata->power_on, 0);
	
	return 0;
}

static int at070tn93_set_power(struct lcd_panel *panel, int on)
{
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d ~ \n", __func__, on);


	if (on) {
		tcclcd_gpio_set_value(pdata->reset, 1);
		lcd_delay_us(1000);

		tcclcd_gpio_set_value(pdata->power_on, 1);
		lcd_delay_us(1000);
		tcclcd_gpio_set_value(pdata->reset, 0);
		lcd_delay_us(1000);
		tcclcd_gpio_set_value(pdata->reset, 1);
		
		tcclcd_gpio_set_value(pdata->display_on, 1);
		mdelay(10);

		lcdc_initialize(pdata->lcdc_num, panel);

		LCDC_IO_Set(pdata->lcdc_num,LCD_GPIO_PORT, panel->bus_width);

		mdelay(16);
	}
	else 
	{
		tcclcd_gpio_set_value(pdata->display_on, 0);
		mdelay(10);
		tcclcd_gpio_set_value(pdata->reset, 0);
		tcclcd_gpio_set_value(pdata->power_on, 0);
		LCDC_IO_Disable(pdata->lcdc_num, panel->bus_width);
	}
	return 0;
}


static int at070tn93_set_backlight_level(struct lcd_panel *panel, int level)
{
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d\n", __func__, level);

	if (level == 0) {
		#ifdef BACKLIGHT_PDM
		tcc_pwm_config(PWM_NUM, 0, 100);
		gpio_config(pdata->bl_on, PWM_FUNC_NUM);
		#else
		tcclcd_gpio_set_value(pdata->bl_on, 0);
		#endif//
	} else {
		#ifdef BACKLIGHT_PDM
		tcc_pwm_config(PWM_NUM, 100, 100);
		gpio_config(pdata->bl_on, PWM_FUNC_NUM);
		#else
		tcclcd_gpio_set_value(pdata->bl_on, 1);
		#endif//
	}

	return 0;
}


static struct lcd_panel at070tn93_panel = {
	.name		= "AT070TN93",
	.manufacturer	= "INNOLUX",
	.id		= PANEL_ID_AT070TN93,
	.xres		= 800,
	.yres		= 480,
	.width		= 154,
	.height		= 85,
	.bpp		= 24,
	.clk_freq	= 31000000,
	.clk_div	= 2,
	.bus_width	= 24,
	.lpw		= 10,
	.lpc		= 800,
	.lswc		= 34,
	.lewc		= 130,
	.vdb		= 0,
	.vdf		= 0,
	.fpw1		= 1,
	.flc1		= 480,
	.fswc1		= 20,
	.fewc1		= 21,
	.fpw2		= 1,
	.flc2		= 480,
	.fswc2		= 20,
	.fewc2		= 21,
//�ӽ�	.sync_invert	= IV_INVERT | IH_INVERT | IP_INVERT,
	.sync_invert	= IV_INVERT | IH_INVERT,

	.init		= at070tn93_panel_init,
	.set_power	= at070tn93_set_power,
	.set_backlight_level = at070tn93_set_backlight_level,
};

struct lcd_panel *tccfb_get_panel(void)
{
	return &at070tn93_panel;
}
#endif//AT070TN93
