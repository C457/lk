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

#ifndef _PLATFORM_TCC_IOMAP_H_
#define _PLATFORM_TCC_IOMAP_H_

#include <platform/reg_physical.h>

/*
 * Part 2. SMU & CKC
 */

#define TCC_CKC_BASE		0x74000000
#define TCC_VIC_BASE		0x74100000
#define TCC_GPIO_BASE		0x74200000
#define TCC_TIMER_BASE		0x74300000
#define TCC_PMU_BASE		0x74400000
#define TCC_SMU_I2C_CH0_BASE	0x74500000


/* IO Bus */

#define TCC_UART0_BASE		HwUART0_BASE
#define TCC_UART1_BASE		HwUART1_BASE
#define TCC_UART2_BASE		HwUART2_BASE
#define TCC_UART3_BASE		HwUART3_BASE
#define TCC_UART4_BASE		HwUART4_BASE
#define TCC_UART5_BASE		HwUART5_BASE
#define TCC_UART6_BASE		HwUART6_BASE
#define TCC_UART7_BASE		HwUART7_BASE

#define TCC_I2C_CH0_BASE	HwI2C_MASTER0_BASE

#define TCC_USB20OTG_BASE	HwUSB20OTG_BASE
#define TCC_USBPHYCFG_BASE	HwUSBPHYCFG_BASE

#define TCC_TSADC_BASE		HwTSADC_BASE

#define TCC_IOBUSCFG_BASE	HwIOBUSCFG_BASE


/*
 * CPU Bus
 */
#define TCC_CPU_BASE		0x77000000
#define TCC_GIC_DIST_BASE	0x77101000
#define TCC_GIC_CPU_BASE	0x77102000
#define TCC_CCICFG_BASE		0x738B0000

//refer : u-boot/include/configs/tcc8971-lcn.h
#if WITH_DEBUG_LOG_BUF
#define PHYS_DDR_BASE				0x80000000	/* DDR Base Address */
#define ABOOT_FORCE_UART_ADDR			PHYS_DDR_BASE + 0x3600000      /* 31k */
#define ABOOT_FORCE_UART_COUNT_ADDR		PHYS_DDR_BASE + 0x3607C00     /* 1k */
#endif // WITH_DEBUG_LOG_BUF

#endif
