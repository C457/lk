/*
 * Copyright (c) 2008 Travis Geiselbrecht
 * Copyright (c) 2010 Telechips, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <debug.h>
#include <reg.h>
#include <dev/uart.h>
#include <platform/iomap.h>
#include <dev/gpio.h>
#include <plat/cpu.h>
#include <platform/gpio.h>
#include <platform/reg_physical.h>
#include <platform/tcc_ckc.h>

#ifndef BITCSET
#define BITCSET(X, CMASK, SMASK)	( (X) = ((((unsigned int)(X)) & ~((unsigned int)(CMASK))) | ((unsigned int)(SMASK))) )
#endif

#define DEBUG_UART	0
#define UART_CLK	48000000

struct uart_stat {
	addr_t base;
	int ch;
};

static struct uart_stat uart[] = {
	{ TCC_UART0_BASE, 0 },
	{ TCC_UART1_BASE, 1 },
	{ TCC_UART2_BASE, 2 },
	{ TCC_UART3_BASE, 3 },
	{ TCC_UART4_BASE, 4 },
	{ TCC_UART5_BASE, 5 },
	{ TCC_UART6_BASE, 6 },
	{ TCC_UART7_BASE, 7 },
};

#define uart_reg_write(p, a, v)	writel(v, uart[p].base + (a))
#define uart_reg_read(p, a)		readl(uart[p].base + (a))

#define UART_RBR	0x00		/* receiver buffer register */
#define UART_THR	0x00		/* transmitter holding register */
#define UART_DLL	0x00		/* divisor latch (LSB) */
#define UART_IER	0x04		/* interrupt enable register */
#define UART_DLM	0x04		/* divisor latch (MSB) */
#define UART_IIR	0x08		/* interrupt ident. register */
#define UART_FCR	0x08		/* FIFO control register */
#define UART_LCR	0x0C		/* line control register */
#define UART_MCR	0x10		/* MODEM control register */
#define UART_LSR	0x14		/* line status register */
#define UART_MSR	0x18		/* MODEM status register */
#define UART_SCR	0x1C		/* scratch register */
#define UART_AFT	0x20		/* AFC trigger level register */
#define UART_SIER	0x50		/* interrupt enable register */

#define LCR_WLS_MSK	0x03		/* character length select mask */
#define LCR_WLS_5	0x00		/* 5 bit character length */
#define LCR_WLS_6	0x01		/* 6 bit character length */
#define LCR_WLS_7	0x02		/* 7 bit character length */
#define LCR_WLS_8	0x03		/* 8 bit character length */
#define LCR_STB		0x04		/* Number of stop Bits, off = 1, on = 1.5 or 2) */
#define LCR_PEN		0x08		/* Parity eneble */
#define LCR_EPS		0x10		/* Even Parity Select */
#define LCR_STKP	0x20		/* Stick Parity */
#define LCR_SBRK	0x40		/* Set Break */
#define LCR_BKSE	0x80		/* Bank select enable */

#define FCR_FIFO_EN	0x01		/* FIFO enable */
#define FCR_RXFR	0x02		/* receiver FIFO reset */
#define FCR_TXFR	0x04		/* transmitter FIFO reset */

#define MCR_RTS		0x02

#define LSR_DR		0x01
#define LSR_THRE	0x20		/* transmitter holding register empty */
#define LSR_TEMT	0x40		/* transmitter empty */

struct uart_port_map_type {
	unsigned int	id;
	unsigned int	tx_port;
	unsigned int	rx_port;
	unsigned int	cts_port;
	unsigned int	rts_port;
	unsigned int	fn_sel;
};

#define NC_PORT	0xFFFFFFFF

static struct uart_port_map_type uart_port_map[] = {
	/*  txd          rxd           cts         rts        fn_sel */
	{0 , TCC_GPA(7),	TCC_GPA(8), TCC_GPA(10), TCC_GPA(9), GPIO_FN(7) },	// UT_TXD(0)
	{1 , TCC_GPA(13),TCC_GPA(14), TCC_GPA(16), TCC_GPA(15), GPIO_FN(7) },	// UT_TXD(1)
	{2 , TCC_GPB(0) , TCC_GPB(1) , TCC_GPB(3) , TCC_GPB(2) , GPIO_FN(10) },	// UT_TXD(2)
	{3 , TCC_GPB(11), TCC_GPB(12), TCC_GPB(14), TCC_GPB(13), GPIO_FN(10) }, // UT_TXD(3)
	{4 , TCC_GPB(19), TCC_GPB(20), TCC_GPB(22), TCC_GPB(21), GPIO_FN(10) },// UT_TXD(4)
	{5 , TCC_GPB(25), TCC_GPB(26), TCC_GPB(28), TCC_GPB(27), GPIO_FN(10) }, // UT_TXD(5)	
	{6 , TCC_GPC(0),   TCC_GPC(1),   TCC_GPC(3),   TCC_GPC(2),  GPIO_FN(6)},	// UT_TXD(6)
	{7 , TCC_GPC(10), TCC_GPC(11), TCC_GPC(13), TCC_GPC(12), GPIO_FN(6)},	// UT_TXD(7)
	{8 , TCC_GPC(23), TCC_GPC(24), TCC_GPC(26), TCC_GPC(25), GPIO_FN(6)},	// UT_TXD(8)
	{9 , TCC_GPD(4),   TCC_GPD(5),  TCC_GPD(7),   TCC_GPD(6),  GPIO_FN(7) },	// UT_TXD(9)
	{10, TCC_GPD(11), TCC_GPD(12), TCC_GPD(14), TCC_GPD(13), GPIO_FN(7) }, // UT_TXD(10)
	{11, TCC_GPD(17), TCC_GPD(18), TCC_GPD(20), TCC_GPD(19), GPIO_FN(7) }, // UT_TXD(11)
	{12, TCC_GPE(4),   TCC_GPE(5),   NC_PORT,   NC_PORT,   GPIO_FN(7) }, // UT_TXD(12)
	{13, TCC_GPE(11), TCC_GPE(12), NC_PORT, NC_PORT, GPIO_FN(7) },	// UT_TXD(13)
	{14, TCC_GPE(20), TCC_GPE(21), TCC_GPE(23), TCC_GPE(22), GPIO_FN(7) },	// UT_TXD(14)
	{15, TCC_GPE(27), TCC_GPE(28), TCC_GPE(30), TCC_GPE(29), GPIO_FN(7) },	// UT_TXD(15)
	{16, TCC_GPF(0),   TCC_GPF(1),   TCC_GPF(3),  TCC_GPF(2),   GPIO_FN(9) },	// UT_TXD(16)
	{17, TCC_GPF(2),   TCC_GPF(3),   TCC_GPF(1),   TCC_GPF(0),  GPIO_FN(10) }, // UT_TXD(17)	
	{18, TCC_GPF(4),   TCC_GPF(5),   TCC_GPF(7),   TCC_GPF(6),  GPIO_FN(9) },	// UT_TXD(18)	
	{19, TCC_GPF(6),   TCC_GPF(7),   TCC_GPF(9),   TCC_GPF(8),  GPIO_FN(10) }, // UT_TXD(19)
	{20, TCC_GPF(13), TCC_GPF(14), NC_PORT, NC_PORT, GPIO_FN(9) },	// UT_TXD(20)
	{21, TCC_GPF(17), TCC_GPF(18), TCC_GPF(20), TCC_GPF(19), GPIO_FN(9) },	// UT_TXD(21)	
	{22, TCC_GPF(22), TCC_GPF(23), NC_PORT, NC_PORT, GPIO_FN(9) },	// UT_TXD(22)
	{23, TCC_GPF(26), TCC_GPF(27), NC_PORT, NC_PORT, GPIO_FN(9) },	// UT_TXD(23)	
	{24, TCC_GPG(0),   TCC_GPG(1),   TCC_GPG(3),  TCC_GPG(2),  GPIO_FN(3) },// UT_TXD(24)
	{25, TCC_GPG(4),   TCC_GPG(5),   TCC_GPG(7),  TCC_GPG(6),   GPIO_FN(3) },// UT_TXD(25)
	{26, TCC_GPG(8),   TCC_GPG(9),   TCC_GPG(11), TCC_GPG(10), GPIO_FN(3) },// UT_TXD(26)
	{27, TCC_GPG(15), TCC_GPG(14), TCC_GPG(12), TCC_GPG(13), GPIO_FN(3) },// UT_TXD(27)
	{28, TCC_GPG(19), TCC_GPG(18), TCC_GPG(16), TCC_GPG(17), GPIO_FN(3) },// UT_TXD(28)	
	{29, TCC_GPHDMI(0), TCC_GPHDMI(1), TCC_GPHDMI(2), TCC_GPHDMI(3), GPIO_FN(3) },// UT_TXD(29)
};

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)	(sizeof(a) / sizeof((a)[0]))
#endif
#define NUM_UART_PORT	ARRAY_SIZE(uart_port_map)

static void uart_set_port_mux(unsigned int ch, unsigned int port)
{
	unsigned int idx;
	PUARTPORTCFG pUARTPORTCFG = (PUARTPORTCFG)HwUART_PORTCFG_BASE;

	for (idx=0 ; idx<NUM_UART_PORT ; idx++) {
		if (uart_port_map[idx].id == port)
			break;
	}

	if (idx >= NUM_UART_PORT)
		return;

	if (ch < 4)
		BITCSET(pUARTPORTCFG->PCFG0.nREG, 0xFF<<(ch*8), port<<(ch*8));
	else if (ch < 8)
		BITCSET(pUARTPORTCFG->PCFG1.nREG, 0xFF<<((ch-4)*8), port<<((ch-4)*8));

	if (ch == DEBUG_UART || ch == MICOM_UART) {
		gpio_config(uart_port_map[idx].tx_port, uart_port_map[idx].fn_sel);	// TX
		gpio_config(uart_port_map[idx].rx_port, uart_port_map[idx].fn_sel);	// RX
	}
}

static void uart_set_gpio(void)
{
	PUARTPORTCFG pUARTPORTCFG = (PUARTPORTCFG)HwUART_PORTCFG_BASE;

	pUARTPORTCFG->PCFG0.nREG = 0xFFFFFFFF;
	pUARTPORTCFG->PCFG1.nREG = 0xFFFFFFFF;

	uart_set_port_mux(0, 22); //debug
	uart_set_port_mux(1, 26); // bt
	uart_set_port_mux(2, 12); // micom
	uart_set_port_mux(3, 23); // gps
	uart_set_port_mux(4, 20); // sirus
	uart_set_port_mux(6, 13); // cd_exp
}

void uart_init_port(int port, uint baud)
{
	uint16_t baud_divisor = (UART_CLK / 16 / baud);

	uart_reg_write(port, UART_LCR, LCR_EPS | LCR_STB | LCR_WLS_8);	/* 8 data, 1 stop, no parity */
	uart_reg_write(port, UART_IER, 0);
	uart_reg_write(port, UART_LCR, LCR_BKSE | LCR_EPS | LCR_STB | LCR_WLS_8);	/* 8 data, 1 stop, no parity */
	uart_reg_write(port, UART_DLL, baud_divisor & 0xff);
	uart_reg_write(port, UART_DLM, (baud_divisor >> 8) & 0xff);
	uart_reg_write(port, UART_FCR, FCR_FIFO_EN | FCR_RXFR | FCR_TXFR | Hw4 | Hw5);
	uart_reg_write(port, UART_LCR, LCR_EPS | LCR_STB | LCR_WLS_8);	/* 8 data, 1 stop, no parity */
}

static int _uart_putc(int port, char c )
{
	/* wait for the last char to get out */
	while (!(uart_reg_read(port, UART_LSR) & LSR_THRE))
		;
	uart_reg_write(port, UART_THR, c);
	return 0;
}

int uart_putc(int port, char c)
{
	if (c == '\n') {
		_uart_putc(0, '\r');
	}
	_uart_putc(0, c);
	return 0;
}

int uart_getc(int port, bool wait)  /* returns -1 if no data available */
{
	if (wait) {
		/* wait for data to show up in the rx fifo */
		while (!(uart_reg_read(port, UART_LSR) & LSR_DR))
			;
	} else {
		if (!(uart_reg_read(port, UART_LSR) & LSR_DR))
			return -1;
	}
	return uart_reg_read(port, UART_RBR);
}

#ifdef MOBIS_GET_DATA_FROM_MICOM
int uart_getc_ex(int port, int time)
{
	int i = 0;

	for (i=time;i>=0;i--)
	{
		if (!(uart_reg_read(port, UART_LSR) & LSR_DR))
			continue;
		else
			break;
	}
	if (i<= 0)
		return -1;
	else
		return uart_reg_read(port, UART_RBR);
}
#endif

void uart_flush_tx(int port)
{
	/* wait for the last char to get out */
	while (!(uart_reg_read(port, UART_LSR) & LSR_TEMT))
		;
}

void uart_flush_rx(int port)
{
	/* empty the rx fifo */
	while (uart_reg_read(port, UART_LSR) & LSR_DR) {
		volatile char c = uart_reg_read(port, UART_RBR);
		(void)c;
	}
}

void uart_init_early(void)
{
	tcc_set_peri(PERI_UART0, ENABLE, 48000000);
	uart_set_gpio();
	uart_init_port(DEBUG_UART, 115200);

#ifdef MOBIS_GET_DATA_FROM_MICOM
	tcc_set_peri(PERI_UART2, ENABLE, 48000000);
	uart_init_port(MICOM_UART, 115200);
#endif
}

void uart_init(void)
{
	uart_flush_rx(DEBUG_UART);

#ifdef MOBIS_GET_DATA_FROM_MICOM
	uart_flush_rx(MICOM_UART);
#endif
}

