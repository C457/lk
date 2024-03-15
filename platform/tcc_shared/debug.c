/*
 * Copyright (c) 2013 Telechips, Inc.
 * All rights reserved.
 * Copyright (c) 2009, The Linux Foundation. All rights reserved.
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
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
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

#include <stdlib.h>
#include <debug.h>
#include <printf.h>
#include <arch/arm/dcc.h>
#include <dev/fbcon.h>
#include <dev/uart.h>
#include <platform/gpio.h>

#if WITH_DEBUG_LOG_BUF
#include <platform/iomap.h>

#define UART_LOG_BUF_SIZE (31*1024) /* align on 31k */
bool freed = false;

#define LK_LOG_COOKIE    0x474f4c52 /* "RLOG" in ASCII */


struct lk_log {
	struct lk_log_header {
		unsigned cookie;
		unsigned max_size;
		unsigned size_written;
		unsigned idx;
	} header;
	char data[UART_LOG_BUF_SIZE];
};

static struct lk_log log = {
	.header = {
		.cookie = LK_LOG_COOKIE,
		.max_size = sizeof(log.data),
		.size_written = 0,
		.idx = 0,
	},
	.data = {0}
};

static void log_putc(char c)
{
	log.data[log.header.idx++] = c;
	log.header.size_written++;
	if (unlikely(log.header.idx >= log.header.max_size-1))
		log.header.idx = 0;
}

void send_uart_to_kernel() {
	//char cnt_buf[6] = { 0, };
	//snprintf((char *)cnt_buf, sizeof(cnt_buf), "%d", log.header.idx);
	//memcpy((void*)ABOOT_FORCE_UART_ADDR, (char*)log.data, sizeof(log.data));
	//memcpy((void*)ABOOT_FORCE_UART_COUNT_ADDR, cnt_buf, sizeof(cnt_buf));
	memcpy((void*)ABOOT_FORCE_UART_ADDR, (void*)&log, sizeof(log));
	freed = true;
}

#endif /* WITH_DEBUG_LOG_BUF */

void _dputc(char c)
{
#if WITH_DEBUG_LOG_BUF
	log_putc(c);
#endif

	if(gpio_get( GPIO_PORTC|22) || mem_test_mode)
	{
#if WITH_DEBUG_DCC
		if (c == '\n') {
			while (dcc_putc('\r') < 0) ;
		}
		while (dcc_putc(c) < 0) ;
#endif
#if WITH_DEBUG_UART
		uart_putc(0, c);
#endif
#if WITH_DEBUG_FBCON && WITH_DEV_FBCON
		fbcon_putc(c);
#endif
#if WITH_DEBUG_JTAG
		jtag_dputc(c);
#endif
	}
}

void _fbputc(char c)
{
#if WITH_DEBUG_UART
	uart_putc(0, c);
#endif
#if WITH_DEV_FBCON
	fbcon_putc(c);
#endif
}

int dgetc(char *c, bool wait)
{
	int n;
#if WITH_DEBUG_DCC
	n = dcc_getc();
#elif WITH_DEBUG_UART
	n = uart_getc(0, 0);
#else
	n = -1;
#endif
	if (n < 0) {
		return -1;
	}
	else {
		*c = n;
		return 0;
	}
}

void platform_halt(void)
{
	dprintf(INFO, "HALT: spinning forever...\n");
	for (;;) ;
}
