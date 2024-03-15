/****************************************************************************
 *
 * Copyright (C) 2014 Telechips Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions
 * andlimitations under the License.
 ****************************************************************************/

#include <dev/gpio.h>
#include <platform/gpio.h>
#include <plat/cpu.h>
#include <daudio_ver.h>

/* Set all ports to output low except some ports in init_tlb[] */
#define GPIO_INIT_PORTCFG_USE

struct gpio_cfg {
	unsigned port;
	unsigned function;
	unsigned direction;
	unsigned pull;
};

/* Please keep the order: GPA, GPB, ,,, GPG, GPHDMI, GPSD3, GPSD0 */
struct gpio_cfg init_tbl_ws5[] = {
	{ TCC_GPA(0)  , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPA(1)  , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPA(2)  , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPA(3)  , GPIO_FN0 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },
	{ TCC_GPA(4)  , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPA(5)  , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPA(6)  , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPA(7)  , GPIO_FN3 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//SD3_CLK
	{ TCC_GPA(8)  , GPIO_FN3 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//SD3_CMD
	{ TCC_GPA(9)  , GPIO_FN3 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//SD3_DATA[00]
	{ TCC_GPA(10) , GPIO_FN3 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//SD3_DATA[01]
	{ TCC_GPA(11) , GPIO_FN3 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//SD3_DATA[02]
	{ TCC_GPA(12) , GPIO_FN3 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//SD3_DATA[03]
	{ TCC_GPA(13) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },	//SD_CARD_WP
	{ TCC_GPA(14) , GPIO_FN0 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//SD_PWR_EN
	{ TCC_GPA(15) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },	//SD_CARD_CD
	{ TCC_GPA(16) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPA(17) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPA(18) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPA(19) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPA(20) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPA(21) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPA(22) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPA(23) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPA(24) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPA(25) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPA(26) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPA(27) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPA(28) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPA(29) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPA(30) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPA(31) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },

	{ TCC_GPB(0)  , GPIO_FN0 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE }, //ANT_SENSE_SEL
	{ TCC_GPB(1)  , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE }, //ANT_POWER_ERR
	{ TCC_GPB(2)  , GPIO_FN0 , GPIO_OUTPUT | GPIO_HIGH		, GPIO_PULLDISABLE },	//DMB_DAB_ANT_PWRON
	{ TCC_GPB(3)  , GPIO_FN0 , GPIO_OUTPUT | GPIO_LOW 		, GPIO_PULLDISABLE },	//J_SATURN_BOOTSTR
	{ TCC_GPB(4)  , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },	//M96_MICOM_PD11
	{ TCC_GPB(5)  , GPIO_FN0 , GPIO_OUTPUT | GPIO_HIGH		, GPIO_PULLDISABLE },	//GPS_ANT_PWR_ON
	{ TCC_GPB(6)  , GPIO_FN0 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//SIRIUS_RESET
	{ TCC_GPB(7)  , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },	//RDS_DATA
	{ TCC_GPB(8)  , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },	//M42_MICOM_PF[8]
	{ TCC_GPB(9)  , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE }, //9277_LOCK_DET
	{ TCC_GPB(10) , GPIO_FN0 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//XM_SHDN
	{ TCC_GPB(11) , GPIO_FN0 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//M156_CPU_9V_DET (micom<->cpu UART)
	{ TCC_GPB(12) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },	//WL_HOST_WAKE_UP
	{ TCC_GPB(13) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },	//CPU_VARIANT_MON(INTEGRATED, DEPARTED).
	{ TCC_GPB(14) , GPIO_FN0 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//WL_REG_ON
	{ TCC_GPB(15) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE }, 	//M105_CPU_4.5V_DET
	{ TCC_GPB(16) , GPIO_FN0 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//DRM_RESET
	{ TCC_GPB(17) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },	//CAM_DESER_LOCK
	{ TCC_GPB(18) , GPIO_FN0 , GPIO_OUTPUT    		, GPIO_PULLDISABLE },	//LVDS_SER_PDN
	{ TCC_GPB(19) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },	//M101_CAMERA VARI_2
	{ TCC_GPB(20) , GPIO_FN0 , GPIO_OUTPUT | GPIO_HIGH		, GPIO_PULLDISABLE }, 	//BT_RESET
	{ TCC_GPB(21) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE }, //SERDES_LINE_FAULT
	{ TCC_GPB(22) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },	//M109_MICOM_PI09
	{ TCC_GPB(23) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },	//M100_CAMERA_VARI_1
	{ TCC_GPB(24) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },	//CPU_VARIANT_OE/PIO
	{ TCC_GPB(25) , GPIO_FN9 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//SPI_SIRIUS_MISO
	{ TCC_GPB(26) , GPIO_FN9 , GPIO_INPUT			, GPIO_PULLDISABLE },	//SPI_SIRIUS_CS
	{ TCC_GPB(27) , GPIO_FN9 , GPIO_INPUT			, GPIO_PULLDISABLE },	//SPI_SIRIUS_MOSI
	{ TCC_GPB(28) , GPIO_FN9 , GPIO_INPUT			, GPIO_PULLDISABLE },	//SPI_SIRIUS_SCLK
	{ TCC_GPB(29) , GPIO_FN0 , GPIO_OUTPUT			, GPIO_PULLDISABLE }, //96751_PWDNB
	{ TCC_GPB(30) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPB(31) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },

	{ TCC_GPC(0)  , GPIO_FN1 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//MAC1_RFE_CLK
	{ TCC_GPC(1)  , GPIO_FN1 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//MAC1_MDC
	{ TCC_GPC(2)  , GPIO_FN1 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//MAC1_MDIO
	{ TCC_GPC(3)  , GPIO_FN1 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//MAC1_RXD[0]
	{ TCC_GPC(4)  , GPIO_FN1 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//MAC1_RXD[1]
	{ TCC_GPC(5)  , GPIO_FN1 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//MAC1_TXD[0]
	{ TCC_GPC(6)  , GPIO_FN1 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//MAC1_TXD[1]
	{ TCC_GPC(7)  , GPIO_FN1 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//MAC1_TXEN
	{ TCC_GPC(8)  , GPIO_FN1 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE }, //MAC1_CRS_DV
	{ TCC_GPC(9)  , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },	//MAC1_INT
	{ TCC_GPC(10) , GPIO_FN0 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//MAC1_RST_B
	{ TCC_GPC(11) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },       //N.C.
	{ TCC_GPC(12) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },       //N.C.
	{ TCC_GPC(13) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },	//CDMA_BOOT_OK_OUT
	{ TCC_GPC(14) , GPIO_FN1 , GPIO_OUTPUT | GPIO_LOW	, GPIO_PULLDISABLE },	//MAC1_RX_ER
	{ TCC_GPC(15) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },      //M144_SATURN_DMB/HD_SPI_INT
	{ TCC_GPC(16) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },      //N.C.
	{ TCC_GPC(17) , GPIO_FN0 , GPIO_INPUT	     	, GPIO_PULLDISABLE },	   //N.C.
	{ TCC_GPC(18) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },      //N.C.
	{ TCC_GPC(19) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },       //N.C.
	{ TCC_GPC(20) , GPIO_FN0 , GPIO_OUTPUT | GPIO_HIGH		, GPIO_PULLUP      },	//GPS_RESET_N
	{ TCC_GPC(21) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE }, //M136_SATURN_SPI_INT
	{ TCC_GPC(22) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE }, //M18_MICOM_PE[0]
	{ TCC_GPC(23) , GPIO_FN5 , GPIO_INPUT			, GPIO_PULLDISABLE },	//HSPI_MISO_SATURN
	{ TCC_GPC(24) , GPIO_FN5 , GPIO_INPUT			, GPIO_PULLDISABLE },	//HSPI_CS_SATURN
	{ TCC_GPC(25) , GPIO_FN5 , GPIO_INPUT			, GPIO_PULLDISABLE },	//HSPI_MOSI_SATURN
	{ TCC_GPC(26) , GPIO_FN5 , GPIO_INPUT			, GPIO_PULLDISABLE },	//HSPI_SCL_SATURN
	{ TCC_GPC(27) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },      //N.C.
	{ TCC_GPC(28) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },      //N.C.
	{ TCC_GPC(29) , GPIO_FN0 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//M23_CPU_BOOT_OK
	{ TCC_GPC(30) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPC(31) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },

	{ TCC_GPD(0)  , GPIO_FN4 , GPIO_INPUT			, GPIO_PULLDISABLE },	//CMMB/LVDS camera input
	{ TCC_GPD(1)  , GPIO_FN4 , GPIO_INPUT			, GPIO_PULLDISABLE },	//CMMB/LVDS camera input
	{ TCC_GPD(2)  , GPIO_FN4 , GPIO_INPUT			, GPIO_PULLDISABLE },	//CMMB/LVDS camera input
	{ TCC_GPD(3)  , GPIO_FN4 , GPIO_INPUT			, GPIO_PULLDISABLE },	//CMMB/LVDS camera input
	{ TCC_GPD(4)  , GPIO_FN4 , GPIO_INPUT			, GPIO_PULLDISABLE },	//CMMB/LVDS camera input
	{ TCC_GPD(5)  , GPIO_FN4 , GPIO_INPUT			, GPIO_PULLDISABLE },	//CMMB/LVDS camera input
	{ TCC_GPD(6)  , GPIO_FN4 , GPIO_INPUT			, GPIO_PULLDISABLE },	//CMMB/LVDS camera input
	{ TCC_GPD(7)  , GPIO_FN4 , GPIO_INPUT			, GPIO_PULLDISABLE },	//CMMB/LVDS camera input
	{ TCC_GPD(8)  , GPIO_FN4 , GPIO_INPUT			, GPIO_PULLDISABLE },	//CMMB/LVDS camera input
	{ TCC_GPD(9)  , GPIO_FN4 , GPIO_INPUT			, GPIO_PULLDISABLE },	//LVDS camera input
	{ TCC_GPD(10) , GPIO_FN4 , GPIO_INPUT			, GPIO_PULLDISABLE },	//LVDS camera input
	{ TCC_GPD(11) , GPIO_FN6 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },
	{ TCC_GPD(12) , GPIO_FN6 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },
	{ TCC_GPD(13) , GPIO_FN6 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPD(14) , GPIO_FN6 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },
	{ TCC_GPD(15) , GPIO_FN2 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//WiFi(SD3.0, SDR104)
	{ TCC_GPD(16) , GPIO_FN2 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//WiFi(SD3.0, SDR104)
	{ TCC_GPD(17) , GPIO_FN2 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//WiFi(SD3.0, SDR104)
	{ TCC_GPD(18) , GPIO_FN2 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//WiFi(SD3.0, SDR104)
	{ TCC_GPD(19) , GPIO_FN2 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//WiFi(SD3.0, SDR104)
	{ TCC_GPD(20) , GPIO_FN2 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//WiFi(SD3.0, SDR104)
	{ TCC_GPD(21) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPD(22) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPD(23) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPD(24) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPD(25) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPD(26) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPD(27) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPD(28) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPD(29) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPD(30) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPD(31) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },

	{ TCC_GPE(0)  , GPIO_FN6 , GPIO_INPUT			, GPIO_PULLDISABLE },	//SPI_SCL_TRAFFIC
	{ TCC_GPE(1)  , GPIO_FN6 , GPIO_INPUT			, GPIO_PULLDISABLE },  //SPI_MOSI_TRAFFIC
	{ TCC_GPE(2)  , GPIO_FN6 , GPIO_INPUT			, GPIO_PULLDISABLE },  //SPI_CS_TRAFFIC
	{ TCC_GPE(3)  , GPIO_FN6 , GPIO_INPUT			, GPIO_PULLDISABLE },  //SPI_MOSI_TRAFFIC
	{ TCC_GPE(4)  , GPIO_FN7 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//M45_UART1_RX_CPU
	{ TCC_GPE(5)  , GPIO_FN7 , GPIO_INPUT			, GPIO_PULLDISABLE },	//M44_UART1_TX_CPU
	{ TCC_GPE(6)  , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },	//XM_F_ACT
	{ TCC_GPE(7)  , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPE(8)  , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },  //M111_REVERSE_CPU
	{ TCC_GPE(9)  , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPE(10) , GPIO_FN0 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE }, //CAS_RESET
	{ TCC_GPE(11) , GPIO_FN7 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//EX_CDP_UART_TXD
	{ TCC_GPE(12) , GPIO_FN7 , GPIO_INPUT			, GPIO_PULLDISABLE },	//EX_CDP_UART_RXD
	{ TCC_GPE(13) , GPIO_FN8 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//PMIC_I2C_DATA
	{ TCC_GPE(14) , GPIO_FN8 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//PMIC_I2C_CLK
#if defined(INCLUDE_FM1688)
	{ TCC_GPE(15) , GPIO_FN0 , GPIO_OUTPUT | GPIO_LOW			, GPIO_PULLDISABLE }, //FM1688_RESET
#else
	{ TCC_GPE(15) , GPIO_FN0 , GPIO_INPUT            , GPIO_PULLDISABLE },
#endif
	{ TCC_GPE(16) , GPIO_FN6 , GPIO_INPUT			, GPIO_PULLDISABLE },	//TSTX1_CLK
	{ TCC_GPE(17) , GPIO_FN6 , GPIO_INPUT			, GPIO_PULLDISABLE },	//TSTX1_SYNC
	{ TCC_GPE(18) , GPIO_FN6 , GPIO_INPUT			, GPIO_PULLDISABLE },	//TSTX1_VALID
	{ TCC_GPE(19) , GPIO_FN6 , GPIO_INPUT			, GPIO_PULLDISABLE },	//TSTX1_DATA
	{ TCC_GPE(20) , GPIO_FN7 , GPIO_INPUT			, GPIO_PULLDISABLE },  //CAS_DATA_TX
	{ TCC_GPE(21) , GPIO_FN7 , GPIO_INPUT			, GPIO_PULLDISABLE },  //CAS_DATA_RX
	{ TCC_GPE(22) , GPIO_FN3 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPE(23) , GPIO_FN3 , GPIO_OUTPUT | GPIO_LOW			, GPIO_PULLDISABLE },	//BT_PCM_CLK
	{ TCC_GPE(24) , GPIO_FN3 , GPIO_OUTPUT | GPIO_LOW			, GPIO_PULLDISABLE },	//BT_PCM_SYNC
#if defined(INCLUDE_FM1688)
	{ TCC_GPE(25) , GPIO_FN3 , GPIO_OUTPUT | GPIO_LOW	    , GPIO_PULLDISABLE },	//FM1688_PCM_DOUT
	{ TCC_GPE(26) , GPIO_FN3 , GPIO_INPUT			, GPIO_PULLDISABLE },		//FM1688_PCM_DIN
#else
	{ TCC_GPE(25) , GPIO_FN0 , GPIO_INPUT            , GPIO_PULLDISABLE },  //N.C.
	{ TCC_GPE(26) , GPIO_FN0 , GPIO_INPUT            , GPIO_PULLDISABLE },  //N.C.
#endif
	{ TCC_GPE(27) , GPIO_FN3 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//BT_PCM_DOUT
	{ TCC_GPE(28) , GPIO_FN3 , GPIO_INPUT			, GPIO_PULLDISABLE },	//BT_PCM_DIN
	{ TCC_GPE(29) , GPIO_FN3 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//MODEM_PCM_DOUT
	{ TCC_GPE(30) , GPIO_FN3 , GPIO_INPUT			, GPIO_PULLDISABLE },	//MODEM_PCM_DIN
#if defined(INCLUDE_FM1688)
	{ TCC_GPE(31) , GPIO_FN0 , GPIO_OUTPUT | GPIO_LOW			, GPIO_PULLDISABLE }, //MIC_SW, 
#else
	{ TCC_GPE(31) , GPIO_FN0 , GPIO_INPUT           , GPIO_PULLDISABLE },
#endif
	{ TCC_GPF(0)  , GPIO_FN1 , GPIO_INPUT			, GPIO_PULLDISABLE },	//Rear camera(9921)
	{ TCC_GPF(1)  , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },	//Rear camera(9921)
	{ TCC_GPF(2)  , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },	//Rear camera(9921)
	{ TCC_GPF(3)  , GPIO_FN1 , GPIO_INPUT			, GPIO_PULLDISABLE },	//Rear camera(9921)
	{ TCC_GPF(4)  , GPIO_FN1 , GPIO_INPUT			, GPIO_PULLDISABLE },	//Rear camera(9921)
	{ TCC_GPF(5)  , GPIO_FN1 , GPIO_INPUT			, GPIO_PULLDISABLE },	//Rear camera(9921)
	{ TCC_GPF(6)  , GPIO_FN1 , GPIO_INPUT			, GPIO_PULLDISABLE },	//Rear camera(9921)
	{ TCC_GPF(7)  , GPIO_FN1 , GPIO_INPUT			, GPIO_PULLDISABLE },	//Rear camera(9921)
	{ TCC_GPF(8)  , GPIO_FN1 , GPIO_INPUT			, GPIO_PULLDISABLE },	//Rear camera(9921)
	{ TCC_GPF(9)  , GPIO_FN1 , GPIO_INPUT			, GPIO_PULLDISABLE },	//Rear camera(9921)
	{ TCC_GPF(10) , GPIO_FN1 , GPIO_INPUT			, GPIO_PULLDISABLE },	//Rear camera(9921)
	{ TCC_GPF(11) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPF(12) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
#if defined(INCLUDE_XM)
	{ TCC_GPF(13) , GPIO_FN0 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//CPU_SIRIUS_UART_TX
	{ TCC_GPF(14) , GPIO_FN0 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//CPU_SIRIUS_UART_RX
#else
	{ TCC_GPF(13) , GPIO_FN9 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//CPU_SIRIUS_UART_TX
	{ TCC_GPF(14) , GPIO_FN9 , GPIO_INPUT			, GPIO_PULLDISABLE },	//CPU_SIRIUS_UART_RX
#endif
	{ TCC_GPF(15) , GPIO_FN10 , GPIO_OUTPUT | GPIO_LOW	, GPIO_PULLDISABLE }, //I2C_TOUCH_SDA
	{ TCC_GPF(16) , GPIO_FN10 , GPIO_OUTPUT | GPIO_LOW	, GPIO_PULLDISABLE }, //I2C_TOUCH_SCL
	{ TCC_GPF(17) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },   //N.C.
	{ TCC_GPF(18) , GPIO_FN0 , GPIO_OUTPUT | GPIO_LOW	, GPIO_PULLDISABLE },	//TOUCH_POR
	{ TCC_GPF(19) , GPIO_FN10 , GPIO_OUTPUT | GPIO_LOW | GPIO_CD(3) , GPIO_PULLDISABLE }, //IPOD_I2C_SDA
	{ TCC_GPF(20) , GPIO_FN10 , GPIO_OUTPUT | GPIO_LOW | GPIO_CD(3) , GPIO_PULLDISABLE },	//IPOD_I2C_SCL
	{ TCC_GPF(21) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },	//I2C_TOUCH_INT
	{ TCC_GPF(22) , GPIO_FN9 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//Debug serial
	{ TCC_GPF(23) , GPIO_FN9 , GPIO_INPUT			, GPIO_PULLDISABLE },	//Debug serial
	{ TCC_GPF(24) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },	//SD BOOT MODE
	{ TCC_GPF(25) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },	//SD BOOT MODE
	{ TCC_GPF(26) , GPIO_FN9 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//UART4_TX_GPS
	{ TCC_GPF(27) , GPIO_FN9 , GPIO_INPUT			, GPIO_PULLDISABLE },	//UART4_RX_GPS
	{ TCC_GPF(28) , GPIO_FN10, GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//9990_I2C3_SDA
	{ TCC_GPF(29) , GPIO_FN10, GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//9990_I2C3_SCL
	{ TCC_GPF(30) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE }, //N.C.
	{ TCC_GPF(31) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE }, //N.C.

	{ TCC_GPG(0)  , GPIO_FN0 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//CPU_SATURN_RESET
	{ TCC_GPG(1)  , GPIO_FN1 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//mm sound out
	{ TCC_GPG(2)  , GPIO_FN1 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//mm sound out
	{ TCC_GPG(3)  , GPIO_FN1 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//mm sound out
	{ TCC_GPG(4)  , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },	//TOUCH_JIG_DET
	{ TCC_GPG(5)  , GPIO_FN0 , GPIO_OUTPUT | GPIO_LOW      	, GPIO_PULLDISABLE },	//CPU_MICOM_RESET
	{ TCC_GPG(6)  , GPIO_FN4 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE }, //D117_SABRE_I2C_SDA
	{ TCC_GPG(7)  , GPIO_FN4 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE }, //D118_SABRE_I2C_SCL
	{ TCC_GPG(8)  , GPIO_FN3 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//BT_UART_TX
	{ TCC_GPG(9)  , GPIO_FN3 , GPIO_INPUT			, GPIO_PULLDISABLE },	//BT_UART_RX
	{ TCC_GPG(10) , GPIO_FN3 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//BT_UART_RTS
	{ TCC_GPG(11) , GPIO_FN3 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//BT_UART_CTS
	{ TCC_GPG(12) , GPIO_FN9 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//MIC/SPK_CLK
	{ TCC_GPG(13) , GPIO_FN9 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//MIC/SPK_SYNC
	{ TCC_GPG(14) , GPIO_FN9 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//SPK_I2S_DOUT
	{ TCC_GPG(15) , GPIO_FN9 , GPIO_INPUT		, GPIO_PULLDISABLE },	//MIC_I2S_DIN
	{ TCC_GPG(16) , GPIO_FN0 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//9990_RESET
	{ TCC_GPG(17) , GPIO_FN1 , GPIO_INPUT			, GPIO_PULLDISABLE },	//MSPDIF_RX
	{ TCC_GPG(18) , GPIO_FN1 , GPIO_OUTPUT | GPIO_LOW		, GPIO_PULLDISABLE },	//MSPDIF_TX
	{ TCC_GPG(19) , GPIO_FN0 , GPIO_OUTPUT			, GPIO_PULLDISABLE },
	{ TCC_GPG(20) , GPIO_FN0 , GPIO_OUTPUT			, GPIO_PULLDISABLE },
	{ TCC_GPG(21) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPG(22) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPG(23) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPG(24) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPG(25) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPG(26) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPG(27) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPG(28) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPG(29) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPG(30) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },
	{ TCC_GPG(31) , GPIO_FN0 , GPIO_INPUT			, GPIO_PULLDISABLE },

	{ TCC_GPSD0(0), GPIO_FN1, GPIO_OUTPUT			, GPIO_PULLDISABLE },
	{ TCC_GPSD0(1), GPIO_FN1, GPIO_OUTPUT			, GPIO_PULLDISABLE },
	{ TCC_GPSD0(2), GPIO_FN1, GPIO_OUTPUT			, GPIO_PULLDISABLE },
	{ TCC_GPSD0(3), GPIO_FN1, GPIO_OUTPUT			, GPIO_PULLDISABLE },
	{ TCC_GPSD0(4), GPIO_FN1, GPIO_OUTPUT			, GPIO_PULLDISABLE },
	{ TCC_GPSD0(5), GPIO_FN1, GPIO_OUTPUT			, GPIO_PULLDISABLE },
	{ TCC_GPSD0(6), GPIO_FN1, GPIO_OUTPUT			, GPIO_PULLDISABLE },
	{ TCC_GPSD0(7), GPIO_FN1, GPIO_OUTPUT			, GPIO_PULLDISABLE },
	{ TCC_GPSD0(8), GPIO_FN1, GPIO_OUTPUT			, GPIO_PULLDISABLE },
	{ TCC_GPSD0(9), GPIO_FN1, GPIO_OUTPUT			, GPIO_PULLDISABLE },
	{ TCC_GPSD0(10), GPIO_FN0, GPIO_OUTPUT			, GPIO_PULLDISABLE },

	{ TCC_GPHDMI(0), GPIO_FN2, GPIO_OUTPUT | GPIO_HIGH	, GPIO_PULLDISABLE }, //GSCK(21)
	{ TCC_GPHDMI(1), GPIO_FN2, GPIO_OUTPUT | GPIO_HIGH	, GPIO_PULLDISABLE }, //GSCMD(21)
	{ TCC_GPHDMI(2), GPIO_FN2, GPIO_INPUT 			, GPIO_PULLDISABLE },  //GSDI(21)
	{ TCC_GPHDMI(3), GPIO_FN2, GPIO_OUTPUT | GPIO_HIGH	, GPIO_PULLDISABLE }, //GSDO(21)
};

#ifdef GPIO_INIT_PORTCFG_USE
static int tlb_idx = 0;
static void gpio_set_init_port(unsigned GPIO, int max_size)
{
	int i;
	for (i = 0 ; i < max_size ; i++) {
		if (((GPIO + i) == init_tbl_ws5[tlb_idx].port) && (tlb_idx < (sizeof(init_tbl_ws5) / sizeof(init_tbl_ws5[0])))) {
			gpio_config(init_tbl_ws5[tlb_idx].port,
						init_tbl_ws5[tlb_idx].function | init_tbl_ws5[tlb_idx].direction | init_tbl_ws5[tlb_idx].pull);
			tlb_idx++;
		}
		else {
			/* set default status */
			gpio_config(GPIO + i, GPIO_FN(0) | GPIO_OUTPUT | GPIO_LOW | GPIO_PULLDISABLE);
		}
	}
}
#endif

void gpio_init_early(void)
{
	int i;
	for (i = 0 ; i < (sizeof(init_tbl_ws5) / sizeof(init_tbl_ws5[0])) ; i++)
		gpio_config(init_tbl_ws5[i].port, init_tbl_ws5[i].function | init_tbl_ws5[i].direction | init_tbl_ws5[i].pull);

}
