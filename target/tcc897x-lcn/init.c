/* Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <debug.h>
#include <platform/iomap.h>
#include <platform/gpio.h>
#include <reg.h>
#include <target.h>
#include <platform.h>
#include <mmc.h>
#include <spmi.h>
#include <board.h>
#include <smem.h>
#include <baseband.h>
#include <dev/keys.h>
#include <partition_parser.h>
#include <scm.h>
#include <platform/tcc_ckc.h>
#include <stdlib.h>
#include <target/guid_partition.h>
#include <sdmmc/emmc.h>
#include <i2c.h>
#include <plat/cpu.h>
#include <dev/camera/camera.h>
#include <daudio_settings.h>
#include <daudio_ver.h>
#include <dev/gpio.h>

#ifdef MOBIS_GET_DATA_FROM_MICOM
#include <mobis_getdata_micom.h>
#endif

#define LINUX_MACHTYPE  5014

extern  bool target_use_signed_kernel(void);
extern unsigned boot_into_chrome;
extern unsigned boot_into_active_partition;
static void set_sdc_power_ctrl();

static unsigned int target_id;
static uint32_t pmic_ver;

extern unsigned boot_into_qb_prebuilt;

extern int check_fwdn_mode(void);
extern void fwdn_start(void);
extern void edi_init(void);
extern char swsusp_header_sig[QB_SIG_SIZE];
extern unsigned int swsusp_header_addr;
#ifdef FACTORYRST_CMDLINE
extern unsigned int get_factory_rst_flag(void);
#endif
#define RECOVERY_MODE   0x77665502

static void control_boot_ok_pin(void);

unsigned char lcd_ver = 0;
int serdes_connect;

void target_early_init(void)
{
	i2c_init(I2C_CH_MASTER0, 22);
	i2c_init(I2C_CH_MASTER1, 21);
	i2c_init(I2C_CH_MASTER2, 18);
	i2c_init(I2C_CH_MASTER3, 23);
	i2c_init(I2C_CH_SMU, 0);
}

void target_init(void)
{
	uint32_t base_addr;
	uint8_t slot;

	lcd_ver = get_daudio_lcd_ver();

	dprintf(INFO, "target_init()\n");
	dprintf(INFO, "## build date : %s, time : %s ##\n", __DATE__, __TIME__);

	dprintf(INFO, "LCD ADC : %d GPIO_B24 : %d (OE:1/PIO:0) GPIO_B13 : %d (Int:1/De:0)\n",
			get_daudio_lcd_ver(), gpio_get(TCC_GPB(24)), gpio_get(TCC_GPB(13)));

	if (!check_fwdn_mode()) {
		hdmi_serdes_pre_init();
	}

	if (target_is_emmc_boot()) {
		emmc_boot_main();       // emmc boot
	}

 #ifdef MOBIS_GET_DATA_FROM_MICOM
	get_data_from_micom();
#endif

	dprintf(INFO, "farmer_Display Init: Start\n");
	display_init();
	dprintf(INFO, "farmer_Display Init: Done\n");

	if( (0 == check_fwdn_mode()) && (RECOVERY_MODE != check_reboot_mode()) ) {
		tcc_cm4_ctrl_ioctl(IOCTL_CM4_CTRL_ON, 0);
	}

#if defined (CONFIG_TCC_PARKING_GUIDE_LINE)
	parking_guide_clear();
	load_parking_guide();
#endif

	if (!check_fwdn_mode()) {
		hdmi_serdes_post_init();
	}

	if (!check_fwdn_mode()) {
		control_boot_ok_pin();
	}

	return;
}

void hdmi_serdes_pre_init()
{
	int i;

	int serdes_line_fault = 0;

	int lock_check_retry = 3;

	if(gpio_get(TCC_GPB(24))) // OE
		if(gpio_get(TCC_GPB(13)))
			switch(lcd_ver)
			{
				case DAUDIOKK_LCD_OI_10_25_1920_720_INCELL_Si_LG : //0
				case DAUDIOKK_LCD_OI_10_25_1920_720_INCELL_Si_2_LG : //1
				case DAUDIOKK_LCD_OI_10_25_1920_720_INCELL_LTPS_LG : //7
					hdmi_set_reg_1();
					break;
				case DAUDIOKK_LCD_OI_08_00_1280_720_OGS_Si_BOE: //8
					break;
				default :
					dprintf(INFO,"ADC value is wrong %d\n", lcd_ver);
					break;
			}
		else
		{
			gpio_config(TCC_GPB(29),1);
			gpio_set(TCC_GPB(29),1);
			dprintf(INFO,"TCC_GPB(29) : %d HIGH\n", gpio_get(TCC_GPB(29)));

			udelay2(10000);

			serdes_line_fault = serdes_line_fault_check();

			if(!serdes_line_fault)
			{
				dprintf(INFO, "LCD LINE FAULT NOT OK\n");
				serdes_connect = 0;
				return;
			}

			for(i = 0;i <= lock_check_retry; i++)
			{
				udelay2(10000);
				serdes_connect = serdes_connect_check();
				if(serdes_connect)
					break;
			}

			if(serdes_connect) {
				dprintf(INFO, "LCD CONNECTED\n");
			}
			else {
				if(serdes_line_fault) {
					dprintf(INFO, "LCD LINE FAULT OK\n");
                                        ser_reg_3gbps();
					for(i=0;i<20;i++)
						udelay2(10000);
                                        serdes_connect = serdes_connect_check();

                                        if(serdes_connect) {
                                                dprintf(INFO, "LCD CONNECTED\n");
                                        }
                                        else {
                                                dprintf(INFO, "LCD DISCONNECTED\n");
                                        }
                                }
                                else {
                                        dprintf(INFO, "LCD DISCONNECTED\n");
                                }
			}
			switch(lcd_ver)
		        {
				case DAUDIOKK_LCD_OD_10_25_1920_720_INCELL_Si_LG: //3
			        	seperated_serdes_reg_10_25(serdes_connect);
			                break;
			        case DAUDIOKK_LCD_OD_12_30_1920_720_INCELL_Si_LG: //4
			        	seperated_serdes_reg_12_3(serdes_connect);
					serdes_reg_3gbps();
			                break;
				case DAUDIOKK_LCD_OD_10_25_1920_720_INCELL_LTPS_LG: //5
					dprintf(INFO,"TCC_GPB(8) : %d\n",gpio_get(TCC_GPB(8)));
					if(factory_connect_for_lcd())
					{
						seperated_serdes_reg_12_3(serdes_connect);
						break;
					}
	        	                seperated_serdes_reg_10_25_ltps(serdes_connect);
					for(i=0;i<3;i++)
						udelay2(10000);
					serdes_reg_3gbps();
	        	                break;
				case DAUDIOKK_LCD_OD_08_00_1280_720_OGS_Si_BOE: //6 HDMI SERDES
			        	seperated_serdes_reg_8_0();
			                break;
				default :
					dprintf(INFO,"ADC value is wrong %d\n", lcd_ver);
					break;
			}
                }
	else // PIO
                switch(lcd_ver)
                {
			case DAUDIOKK_LCD_PI_10_25_1920_720_PIO_AUO : //5
				hdmi_set_reg_1();
	                        break;
			case DAUDIOKK_LCD_PI_08_00_800_400_PIO_TRULY :
				break;
			default:
				dprintf(INFO,"ADC value is wrong %d\n", lcd_ver);
	                        break;
                }
}

void hdmi_serdes_post_init()
{
	int i=0;
	if(gpio_get(TCC_GPB(24))) // OE
		if(gpio_get(TCC_GPB(13)))
			switch(lcd_ver)
			{
				case DAUDIOKK_LCD_OI_10_25_1920_720_INCELL_Si_LG : //0
        	                case DAUDIOKK_LCD_OI_10_25_1920_720_INCELL_Si_2_LG : //1
	                        case DAUDIOKK_LCD_OI_10_25_1920_720_INCELL_LTPS_LG : //7
					for(i=0;i<6;i++)
						udelay2(10000);
					hdmi_set_reg_2_OE();
					break;
				case DAUDIOKK_LCD_OI_08_00_1280_720_OGS_Si_BOE: //8
					break;
				default :
					dprintf(INFO, "ADC value is wrong %d\n", lcd_ver);
					break;
			}
		else {
			switch(lcd_ver)
			{
				case DAUDIOKK_LCD_OD_12_30_1920_720_INCELL_Si_LG: //4
					serdes_reg_12_3_touch(serdes_connect);
					break;
				case DAUDIOKK_LCD_OD_10_25_1920_720_INCELL_LTPS_LG: //5
					serdes_reg_10_25_ltps_touch(serdes_connect);
					break;
				case DAUDIOKK_LCD_OI_DISCONNECTED: //10
					dprintf(INFO,"There is no monitor\n");
					break;
				default:
					dprintf(INFO,"ADC value is wrong\n");
					break;
			}
		}
	else 			// PIO
		switch(lcd_ver)
                {
			case DAUDIOKK_LCD_PI_10_25_1920_720_PIO_AUO: //5
				for(i=0;i<6;i++)
					udelay2(10000);
                                hdmi_set_reg_2_PIO();
				break;
                        case DAUDIOKK_LCD_PI_08_00_800_400_PIO_TRULY: //6
                                break;
			case DAUDIOKK_LCD_PI_DISCONNECTED: //10
                                dprintf(INFO,"There is no monitor\n");
                                break;
                        default:
                                dprintf(INFO,"ADC value is wrong\n");
                                break;
                }
}

void target_control_vbus(unsigned on)
{
	gpio_set(TCC_GPC(29), on);
}

static void control_boot_ok_pin(void)
{
	int i =0;
	gpio_set(TCC_GPC(29), 1);
	for(i=0;i<10;i++)
	{
		dprintf(INFO, "TCC_GPC(29) HIGH\n");
		if(gpio_get(TCC_GPC(29)))
		{
			break;
		}
		else
		{
			dprintf(INFO, "TCC_GPC(29) current low, count=%d\n", i);
			udelay2(10);
			gpio_set(TCC_GPC(29), 1);
		}
	}
	dprintf(INFO, "TCC_GPC(29) boot_ok_pin HIGH\n");
}

unsigned board_machtype(void)
{
	target_id = LINUX_MACHTYPE;
	return target_id;
}

void reboot_device(unsigned reboot_reason)
{
	reboot(reboot_reason);
}

static int board_get_serialno(char *serialno)
{
	int n, i;
	char temp[32];
#if _EMMC_BOOT
	if (target_is_emmc_boot())
		n = get_emmc_serial(temp);
	else
#endif
		n = NAND_GetSerialNumber(temp, 32);

	for (i = 0; i < 4; i++)	// 4 = custon field(2) + product number(2)
		*serialno++ = temp[i];
	for (i = 16; i < 32; i++)	// 16 = time(12) + serial count(4)
		*serialno++ = temp[i];
	*serialno = '\0';
	return strlen(serialno);
}

static int board_get_wifimac(char *wifimac)
{
	int n, i;
	char temp[32];
#if _EMMC_BOOT
	if (target_is_emmc_boot())
		n = get_emmc_serial(temp);
	else
#endif
		n = NAND_GetSerialNumber(temp, 32);

	if (temp[1] == '1') {
		for (i = 0; i < 12; i++) {
			*wifimac++ = temp[4 + i];
			if (i == 11) break;
			if (!((i + 1) % 2)) *wifimac++ = ':';
		}
	}
	else if(temp[1] == '2') {
		for (i = 0; i < 12; i++) {
			*wifimac++ = temp[16 + i];
			if (i == 11) break;
			if (!((i + 1) % 2)) *wifimac++ = ':';
		}
	}
	*wifimac = '\0';
	return strlen(wifimac);
}

static int board_get_btaddr(char *btaddr)
{
	int n, i;
	char temp[32];
#if _EMMC_BOOT
	if (target_is_emmc_boot())
		n = get_emmc_serial(temp);
	else
#endif
		n = NAND_GetSerialNumber(temp, 32);

	for (i = 4; i < 16; i++)	// 12 = bluetooth bd address field(12)
		*btaddr++ = temp[i];
	*btaddr = '\0';
	return strlen(btaddr);
}

void target_cmdline_kpanic(char *cmdline)
{
	char s[128];
	unsigned long kpanic_base = 0;
	unsigned long kpanic_size = 0;

	char s2[64];
	unsigned int reboot_reason;

	if (get_partition_info("kpanic", &kpanic_base, &kpanic_size) ) {
		printf("%s : get kpanic info failed...\n", __func__);
		//return -1;
	}

	printf("%s : kpanic_base(%d), kpanic_size(%d)\n", __func__, kpanic_base, kpanic_size);

	sprintf(s, " tcc_kpanic_base=%d tcc_kpanic_size=%d", kpanic_base, kpanic_size);
	strcat(cmdline, s);

	reboot_reason = get_restart_reason();

	printf("%s : reboot_reason(0x%x)\n", __func__, reboot_reason);

	sprintf(s2, " androidboot.reboot.reason=0x%x", reboot_reason);
	strcat(cmdline, s2);
}
void target_cmdline_bootmode(char *cmdline)
{
	extern unsigned boot_into_factory;
	char s[32] = "";
	char s2[32] = "";

#if (0)
	if (target_is_battery_charging())
		strcpy(s, "charger");
	else
#endif
		if (target_is_emmc_boot())
			strcpy(s, "emmc");
		else
			strcpy(s, "nand");

	strcat(cmdline, " androidboot.mode=");
	strcat(cmdline, s);

	if (boot_into_factory ) {
		strcat(s2, "tcc_factory");
	}
	strcat(cmdline, " androidboot.dignostic=");
	strcat(cmdline, s2);
}

void target_cmdline_loglevel(char *cmdline)
{
	char s[32];
	int s2;

	if(!gpio_get( GPIO_PORTC | 22))
		s2 = 0;
	else
		s2 = 7;

	sprintf(s, " loglevel=%d", s2);

	strcat(cmdline, s);
}

void target_cmdline_serialno(char *cmdline)
{
	char s[128];
	char s2[64];

	board_get_serialno(s2);


#if _EMMC_BOOT
	if(boot_into_chrome)
#if defined(PIO_WIDE_64GB_PARTITION)
		sprintf(s, " root=/dev/mmcblk0p2 rw rootfstype=ext4 rootwait noinitrd", s2);
#elif defined(PIO_WIDE_128GB_PARTITION)
		sprintf(s, " root=/dev/mmcblk0p2 rw rootfstype=ext4 rootwait noinitrd", s2);
#elif defined(PIO_WIDE_4GB_PARTITION)
		sprintf(s, " root=/dev/mmcblk0p2 rw rootfstype=ext4 rootwait noinitrd", s2);
#elif defined(PIO_WIDE_8GB_PARTITION)
		sprintf(s, " root=/dev/mmcblk0p2 rw rootfstype=ext4 rootwait noinitrd", s2);
#elif defined(PIO_WIDE_8GB_PARTITION2)
		sprintf(s, " root=/dev/mmcblk0p2 rw rootfstype=ext4 rootwait noinitrd", s2);
#else
		sprintf(s, " root=/dev/mmcblk0p3 rw rootfstype=ext2 rootwait noinitrd", s2);

#endif //PIO_WIDE_64GB_PARTITION
	else
		sprintf(s, " androidboot.serialno=%s", s2);
#else
	sprintf(s, " androidboot.serialno=%s", s2);
#endif

	strcat(cmdline, s);
}

void target_cmdline_wifimac(char *cmdline)
{
	char s[64];
	char s2[32];

	board_get_wifimac(s2);

	sprintf(s, " androidboot.wifimac=%s", s2);
	strcat(cmdline, s);
}

void target_cmdline_btaddr(char *cmdline)
{
	char s[64];
	char s2[32];

	board_get_btaddr(s2);

	sprintf(s, " androidboot.btaddr=%s", s2);
	strcat(cmdline, s);
}

void target_cmdline_memtype(char *cmdline)
{
	char s[64];
	char memtype[2] = {""};
#if defined(DRAM_DDR2)
	char temp = '1';
#elif defined(DRAM_DDR3)
	char temp = '2';
#elif ! defined(TCC88XX)
	char temp = '1';
#else
	char temp = '0';
#endif
	memtype[0] = temp;
	memtype[1] = '\0';

	sprintf(s, " androidboot.memtype=%s", memtype);
	strcat(cmdline, s);
}

void target_cmdline_vmalloc(char *cmdline)
{
	char s[64];
	sprintf(s, " androidboot.hardware=tcc897x vmalloc=%s", "480M");
	strcat(cmdline, s);
}

void target_cmdline_poll_msec(char *cmdline)
{
	char tmp_str[48];
	memset(&tmp_str, 0x00, sizeof(char) * 48);
	sprintf(tmp_str, " block.events_dfl_poll_msecs=2000");

	strcat(cmdline, tmp_str);
}

void target_cmdline_daudio_add(char* cmdline, const char* cmd, unsigned int value)
{
	char s[128];

	sprintf(s, " %s=%d", cmd, value);
	strcat(cmdline, s);
}

void target_cmdline_daudio_lk_version(char *cmdline)
{
	dprintf(SPEW, "%s LK version: %d\n",
			__func__, (unsigned int)BSP_LK_VERSION);

	target_cmdline_daudio_add(cmdline, "daudio_lk_ver", (unsigned int)BSP_LK_VERSION);
}

void target_cmdline_daudio_board_version(char *cmdline)
{
	unsigned char hw_ver = get_daudio_hw_ver();
	unsigned char main_ver = get_daudio_main_ver();
	unsigned char bt_ver = get_daudio_bt_ver();
	unsigned char lcd_ver = get_daudio_lcd_ver();
	unsigned int version = bt_ver << 24 | lcd_ver << 16 | main_ver << 8 | hw_ver;
	dprintf(SPEW, "%s H/W version: %d\n", __func__, version);

	target_cmdline_daudio_add(cmdline, "daudio_hw_ver", version);
}

void target_cmdline_daudio_board_adc(char* cmdline)
{
	char s[128];

	unsigned short hw_adc	= get_daudio_hw_adc();
	unsigned short main_adc	= get_daudio_main_adc();
	unsigned short bt_adc	= get_daudio_bt_adc();
	unsigned short lcd_adc	= get_daudio_lcd_adc();

	dprintf(SPEW, "%s lcd adc: %d\n", __func__, lcd_adc);

	target_cmdline_daudio_add(cmdline, "daudio_adc_hw", hw_adc);
	target_cmdline_daudio_add(cmdline, "daudio_adc_main", main_adc);
	target_cmdline_daudio_add(cmdline, "daudio_adc_bt", bt_adc);
	target_cmdline_daudio_add(cmdline, "daudio_adc_lcd", lcd_adc);
}

void target_cmdline_daudio_em_setting_info(char* cmdline)
{
	unsigned long long id;
	unsigned int mode;

	struct em_setting_info info;
	const int size = sizeof(struct em_setting_info) / sizeof(unsigned);
	int ret = read_em_setting(&info);

	if (info.id != EM_SETTING_ID) {
		info.id = 0;
		info.mode = 0;
	}
	else if (info.mode > 1) {
		info.mode = 0;
	}
	dprintf(SPEW, "%s Engineer mode id: 0x%llx, mode: 0x%x, size: %d, ret: %d\n",
			__func__, info.id, info.mode, size, ret);


	id = info.id;
	mode = info.mode;

	target_cmdline_daudio_add(cmdline, "daudio_info_id", id);
	target_cmdline_daudio_add(cmdline, "daudio_info_mode", mode);

}

void target_cmdline_daudio_quickboot_info(char* cmdline)
{
	char s[128];
	struct quickboot_info info;
	const int size = sizeof(struct quickboot_info) / sizeof(unsigned);

	memset(info.sig, 0, QB_SIG_SIZE);
	memcpy(info.sig, swsusp_header_sig, QB_SIG_SIZE);
	info.addr = swsusp_header_addr;

	sprintf(s, " daudio_qb_sig=%s", info.sig);
	strcat(cmdline, s);

	target_cmdline_daudio_add(cmdline, "daudio_qb_addr", info.addr);

}

void target_cmdline_daudio_active_partition(char* cmdline)
{
	char s[64];

	if(boot_into_active_partition == 0) {
		sprintf(s, " active_partition=%s", "_boot");
	}
	else if(boot_into_active_partition == 1) {
		sprintf(s, " active_partition=%s", "_mirror");
	}

	strcat(cmdline, s);
}

#ifdef MOBIS_GET_DATA_FROM_MICOM
void target_cmdline_mobis_vehicle_code(char* cmdline)
{
	char s[64] = { 0, };

	data_from_micom_t* p_vehicle_country_code = NULL;

	p_vehicle_country_code = get_vehicle_country_info();
	if(p_vehicle_country_code==NULL)
		sprintf(s, " vehicle_info=0xff");
	else
		sprintf(s, " vehicle_info=0x%02x", p_vehicle_country_code->vehicle_code);

	strcat(cmdline, s);
	//dprintf(INFO, "cmdline=%s\n", cmdline);
}
void target_cmdline_mobis_country_code(char* cmdline)
{
	char s[64] = { 0, };

	data_from_micom_t* p_vehicle_country_code = NULL;

	p_vehicle_country_code = get_vehicle_country_info();
	if(p_vehicle_country_code == NULL)
		sprintf(s, " country_info=0xff");
	else
		sprintf(s, " country_info=0x%02x", p_vehicle_country_code->country_code);

	strcat(cmdline, s);
	dprintf(INFO, "cmdline=%s\n", cmdline);
}
#endif

#ifdef FACTORYRST_CMDLINE
void target_cmdline_factory_reset_flag(char* cmdline)
{
	unsigned int factory_rst_flag = get_factory_rst_flag();

	target_cmdline_daudio_add(cmdline, "factory_rst_flag", factory_rst_flag);
}
#endif
