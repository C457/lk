/* Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <debug.h>
#include <smem.h>
//#include <msm_panel.h>
//#include <pm8x41.h>
//#include <pm8x41_wled.h>
#include <board.h>
//#include <mdp5.h>
#include <platform/gpio.h>
#include <target/display.h>
#include <dev/fbcon.h>
#include "../../app/aboot/recovery.h"
#include <daudio_settings.h>
#include <splash/splashimg.h>
#include <splash/bootlogoinfo.h>
#include <daudio_ver.h>

#ifdef MOBIS_GET_DATA_FROM_MICOM
#include <mobis_getdata_micom.h>
#endif
//static struct msm_fb_panel_data panel;
static uint8_t display_enable;

static struct fbcon_config *fb_config;

static void display_bootlogo(struct fbcon_config *fb_cfg)
{
	dprintf(INFO , "display_bootlogo \n");

#if 0 //mk files from opio_wide, std folder
	int snapshot_header = 0;
	int ret = -1;

	dprintf(INFO , "display_bootlogo \n");

	VARIANT_CODE_INFO_t variant_code_info;

	snapshot_header = snapshot_boot_header();
	dprintf(INFO , "snapshot_boot_header %d \n", snapshot_header);

	ret = read_variant_setting(&variant_code_info);
	if (ret == 0) {
		printf("[DEBUG] variant code -------------------------  \r\n");
		printf("[DEBUG] id : %x %x %x %x  \r\n", variant_code_info.id[0], variant_code_info.id[1], variant_code_info.id[2], variant_code_info.id[3]);
		printf("[DEBUG] version : %d  \r\n", variant_code_info.version);
		printf("[DEBUG] size : %d  \r\n", variant_code_info.size);
		printf("[DEBUG] variant code [0] : %d  \r\n", variant_code_info.variant_code[0]);
		printf("[DEBUG] variant code [1] : %d  \r\n", variant_code_info.variant_code[1]);
		printf("[DEBUG] --------------------------------------  \r\n");
	}
	if (snapshot_header == -1) {
		struct recovery_message Msg;
		if(get_recovery_message(&Msg) == 0) {
			if(0 == strcmp("boot-recovery", Msg.command)) {
				dprintf(INFO , "Not valid snapshot header, But recovery update \n");
				if ( (ret == 0)
						&& (variant_code_info.version == VARIANT_STRUCT_VERSION)
						&& (strncmp(variant_code_info.id, VARIANT_STRUCT_ID, 4) == 0)) {
					if (variant_code_info.variant_code[0] == 1) {
						splash_image_load("bluelink", fb_cfg);
					}
					else {
						splash_image_load("bootlogo", fb_cfg);
					}
				}
				else {
					splash_image_load("bluelink", fb_cfg);
				}
			}
			else {
				dprintf(INFO , "Not valid snapshot header \n");
				splash_image_load("quickboot", fb_cfg);
			}
		}
		else {
			dprintf(INFO , "Not valid snapshot header \n");
			splash_image_load("quickboot", fb_cfg);
		}
	}
	else {
		dprintf(INFO , "Valid snapshot header \n");

		if (ret == 0) {
			if ((variant_code_info.version == VARIANT_STRUCT_VERSION)
					&& (strncmp(variant_code_info.id, VARIANT_STRUCT_ID, 4) == 0)) {
				if (variant_code_info.variant_code[0] == 1) {
					splash_image_load("bluelink", fb_cfg);
				}
				else {
					splash_image_load("bootlogo", fb_cfg);
				}
			}
			else {
				// not valid variant code data
				splash_image_load("bootlogo", fb_cfg);
			}
		}
		else {
			dprintf(INFO, "read_variant_setting error (%d) \r\n", ret);
			// not read emmc data
			splash_image_load("bootlogo", fb_cfg);
		}
	}
#else // display booting logo after receiving data
	dprintf(INFO , "[bootinglogo]one binary booting logo applied\n");

	uint8_t modem = 0, oem = 0, type = 0, local = 0, vehicle_code = 0, i = 0, lcd_ver = 0;

	data_from_micom_t  *p_vehicle_country_code = NULL;
	p_vehicle_country_code = get_vehicle_country_info();

	vehicle_code = p_vehicle_country_code->vehicle_code;
	local = p_vehicle_country_code->country_code;
	modem = p_vehicle_country_code->has_modem;
	type = p_vehicle_country_code->engine_type;
	//oem = p_vehicle_country_code->oem_type;

	lcd_ver = get_daudio_lcd_ver();
	if(local != 0xff) { //Get a data from micom
		dprintf(INFO , "[bootinglogo]modem=0x%02x, type=0x%02x, local=0x%02x, vehicle_code=0x%02x\n", modem, type, local, vehicle_code);

		while(vehicle_code != bootlogo_info[i][0]) {
			if(bootlogo_info[i][0] == 0xFF) { //end of the list
				dprintf(INFO , "\x1b[41m[bootinglogo]Check bootlogo_info list. Matching is failed!\x1b[0m\n");
				break;
			}
			i++;
		}
		oem = bootlogo_info[i][1]; //oem type 0:Hyundai, 1:Kia, 2:Genesis, 3:New Kia
		oem |= bootlogo_info[i][2] << 4; //lcd type 0:1920x720, 1:1280x720

		switch(oem) {
			case 0x0: //Hyundai
				splash_image_load("hyundai_1920x720", fb_cfg);
				break;

			case 0x1: //Kia
				splash_image_load("kia_1920x720", fb_cfg);
				break;

			case 0x2: //Genesis
				splash_image_load("genesis_1920x720", fb_cfg);
				break;

			case 0x3: //New Kia
				splash_image_load("newkia_1920x720", fb_cfg);
				break;

			case 0x4: //New Hyundai
				splash_image_load("newhyundai_1920x720", fb_cfg);
				break;

			case 0x5: //hyundai ECO
				splash_image_load("hyundaieco_1920x720", fb_cfg);
				break;

			case 0x10: //Hyundai 1280x720(8inch LCD)
				splash_image_load("hyundai_1280x720", fb_cfg);
				break;

			case 0x11: //Kia 1280x720(8inch LCD)
				splash_image_load("kia_1280x720", fb_cfg);
				break;

			case 0x13: //New Kia 1280x720(8inch LCD)
				splash_image_load("newkia_1280x720", fb_cfg);
				break;

			case 0x14: //New Hyundai 1280x720(8inch LCD)
				splash_image_load("newhyundai_1280x720", fb_cfg);
				break;

			default:
				if(lcd_ver == DAUDIOKK_LCD_OD_08_00_1280_720_OGS_Si_BOE || lcd_ver == DAUDIOKK_LCD_OI_08_00_1280_720_OGS_Si_BOE)
					splash_image_load("error_1280x720", fb_cfg);
				else
					splash_image_load("error_1920x720", fb_cfg);

				dprintf(INFO , "\x1b[41m[bootinglogo]Vehicle code matching fail. Check device/mobis/daudio/product/bootlogoinfo.dat\x1b[0m\n", oem, i);
				break;
		}
	}
	else { //vehicle_code = 0. Failed to get a data from micom
		if(lcd_ver == DAUDIOKK_LCD_OD_08_00_1280_720_OGS_Si_BOE || lcd_ver == DAUDIOKK_LCD_OI_08_00_1280_720_OGS_Si_BOE)
			splash_image_load("error_1280x720", fb_cfg);
		else
			splash_image_load("error_1920x720", fb_cfg);

		dprintf(INFO , "\x1b[41m[bootinglogo]Maybe micom version err. It should be over V301. oem=%d, i=%d\x1b[0m\n", oem, i);
	}
#endif
}

void display_init(void)
{
	uint32_t hw_id = 0 ;//board_hardware_id();
//	uint32_t soc_ver = board_soc_version();

	unsigned char lcd_ver = get_daudio_lcd_ver();

	dprintf(INFO, "display_init(),target_id=%d.\n", hw_id);

	if(gpio_get(TCC_GPB(24))) // OE
		switch(lcd_ver) {
			case DAUDIOKK_LCD_OI_10_25_1920_720_INCELL_Si_LG : //0
			case DAUDIOKK_LCD_OI_10_25_1920_720_INCELL_Si_2_LG : //1
			case DAUDIOKK_LCD_OD_10_25_1920_720_INCELL_Si_LG: //3
			case DAUDIOKK_LCD_OD_12_30_1920_720_INCELL_Si_LG: //4
			case DAUDIOKK_LCD_OD_10_25_1920_720_INCELL_LTPS_LG: //5
			case DAUDIOKK_LCD_OD_08_00_1280_720_OGS_Si_BOE: //6 HDMI SERDES
			case DAUDIOKK_LCD_OI_10_25_1920_720_INCELL_LTPS_LG : //7
			case DAUDIOKK_LCD_OI_DISCONNECTED: //10
				fb_config = lcdc_init_hdmi();
				break;
			case DAUDIOKK_LCD_OI_08_00_1280_720_OGS_Si_BOE: //8
				fb_config = lcdc_init();
				break;
			default:
				dprintf(INFO, "ADC value is wrong\n");
				fb_config = lcdc_init_hdmi();
				break;
		}
	else                    // PIO
		switch(lcd_ver) {
			case DAUDIOKK_LCD_PI_10_25_1920_720_PIO_AUO: //5
				fb_config = lcdc_init_hdmi();
				break;
			case DAUDIOKK_LCD_PI_08_00_800_400_PIO_TRULY: //6
				fb_config = lcdc_init();
				break;
			case DAUDIOKK_LCD_PI_DISCONNECTED: //10
				fb_config = lcdc_init();
				dprintf(INFO, "There is no monitor\n");
				break;
			default:
				dprintf(INFO, "ADC value is wrong\n");
				fb_config = lcdc_init();
				break;
		}

	ASSERT(fb_config);
	fbcon_setup(fb_config);

#if 1//!defined(DISPLAY_SPLASH_SCREEN_DIRECT)
	dprintf(INFO, "Splash Display int: start\n");
	//splash_image_load("bootlogo", fb_config);
#if 1//defined(DEFAULT_DISPLAY_LCD)
	if(!check_fwdn_mode() && lcd_ver != DAUDIOKK_LCD_PI_DISCONNECTED)
		display_bootlogo(fb_config);
#endif
	display_splash_logo(fb_config);
	dprintf(INFO, "Splash Display int: end\n");
#endif

	display_enable = 1;
}

void display_shutdown(void)
{
	//if (display_enable)
	//	msm_display_off();
}
