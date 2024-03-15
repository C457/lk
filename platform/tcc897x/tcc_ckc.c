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

#include <platform/reg_physical.h>
#include <platform/tcc_ckc.h>
#include <clock.h>
#include <plat/cpu.h>
#include <reg.h>
#include <debug.h>
#include <string.h>
#include <div64.h>

#define CPU_CLOCK_CHANGE_BY_AUTO_MODE

/**********************************
 *  Pre Defines
 **********************************/
#define ckc_writel	writel
#define ckc_readl	readl

#define CPU0_SRC_CH	PLL_3
#define MEM_SRC_CH	PLL_4

#define MAX_TCC_PLL	5
#define MAX_CLK_SRC	(MAX_TCC_PLL*2 + 2)	// XIN, XTIN

static void __iomem	*ckc_base = NULL;
static void __iomem	*pmu_base = NULL;
static void __iomem	*membus_cfg_base = NULL;
static void __iomem	*ddibus_cfg_base = NULL;
static void __iomem	*gpubus_cfg_base = NULL;
static void __iomem	*iobus_cfg_base = NULL;
static void __iomem	*vpubus_cfg_base = NULL;
static void __iomem	*hsiobus_cfg_base = NULL;

static unsigned int	stClockSource[MAX_CLK_SRC];
static bool		audio_pll_used = false;
static u32		audio_pll_ch = 0;

#ifdef HDMIA_SRC_SHOULD_BE_SAME_WITH_AUDIO
static unsigned int	stHDMIASrc = 0xFFFFFFFF;
#endif

static unsigned long tcc_ckc_peri_get_rate(int id);
static inline int tcc_find_audio_pclk(tPCLKCTRL *PCLKCTRL);
static inline void tcc_ckc_reset_clock_source(int id);
extern int change_mem_clock(unsigned int freq);

/**********************************
 *  CKC Register Control
 **********************************/
#define CKC_CLKCTRL	0x000
#define CKC_PLL		0x040
#define CKC_CLKDIVC	0x060
#define CKC_SWRESET	0x07C
#define CKC_PCLKCTRL	0x080

/* PLL */
#define PLL_MAX_RATE		(2500*1000*1000UL)
#define PLL_MIN_RATE		(40*1000*1000UL)
#define PLL_VCO_MAX		(2500*1000*1000UL)
#define PLL_VCO_MIN		(1250*1000*1000UL)
#define PLL_P_MAX		12	// 63	FREF = FIN/p  (2MHz ~ 12MHz)
#define PLL_P_MIN		2	// 1	FREF = FIN/p  (2MHz ~ 12MHz)
#define PLL_P_SHIFT		0
#define PLL_P_MASK		0x3F
#define PLL_M_MAX		1023
#define PLL_M_MIN		64
#define PLL_M_SHIFT		6
#define PLL_M_MASK		0x3FF
#define PLL_S_MAX		5
#define PLL_S_MIN		0
#define PLL_S_SHIFT		16
#define PLL_S_MASK		0x7
#define PLL_SRC_SHIFT		19
#define PLL_SRC_MASK		0x3
#define PLL_BYPASS_SHIFT	21
#define PLL_LOCKST_SHIFT	23
#define PLL_CHGPUMP_SHIFT	24
#define PLL_CHGPUMP_MASK	0x3
#define PLL_LOCKEN_SHIFT	26
#define PLL_RSEL_SHIFT		27
#define PLL_RSEL_MASK		0xF
#define PLL_EN_SHIFT		31
#define tcc_pll_write(reg,en,p,m,s,src) { \
	if (en) { \
		volatile unsigned int i; \
		*(volatile unsigned *)reg = (0 \
			|(1<<PLL_LOCKEN_SHIFT)|(2<<PLL_CHGPUMP_SHIFT) \
			|((src&PLL_SRC_MASK)<<PLL_SRC_SHIFT)|((s&PLL_S_MASK)<<PLL_S_SHIFT) \
			|((m&PLL_M_MASK)<<PLL_M_SHIFT)|((p&PLL_P_MASK)<<PLL_P_SHIFT)); \
		/* need to delay at least 1us. */ \
		if (reg == ckc_base+CKC_PLL+0xC) \
			; /* no need to delay. cpu clock is XIN(24MHz). */ \
		else \
			for (i=100 ; i ; i--);  /* if cpu clokc is 1HGz then loop 100. */ \
		*(volatile unsigned *)reg |= (en&1)<<PLL_EN_SHIFT; \
		while(((*(volatile unsigned *)reg)&(1<<PLL_LOCKST_SHIFT))==0); \
	} else \
		*(volatile unsigned *)reg &= ~(1<<PLL_EN_SHIFT); \
}
#if defined(CPU_CLOCK_CHANGE_BY_AUTO_MODE)
#define tcc_pll_write_cb(reg,p,m,s,src) { \
	*(volatile unsigned *)reg = ((1<<PLL_EN_SHIFT) \
		|(1<<PLL_LOCKEN_SHIFT)|(2<<PLL_CHGPUMP_SHIFT) \
		|((src&PLL_SRC_MASK)<<PLL_SRC_SHIFT)|((s&PLL_S_MASK)<<PLL_S_SHIFT) \
		|((m&PLL_M_MASK)<<PLL_M_SHIFT)|((p&PLL_P_MASK)<<PLL_P_SHIFT)); \
	while(((*(volatile unsigned *)reg)&(1<<PLL_LOCKST_SHIFT))==0); \
}
#endif
enum{ /* PLL Clock Source */
	PLLSRC_XIN=0,
	PLLSRC_HDMIXI,
	PLLSRC_EXTCLK0,
	PLLSRC_EXTCLK1,
	PLLSRC_MAX
};

#if !defined(CPU_CLOCK_CHANGE_BY_AUTO_MODE)
/* CLKCTRL - CPU/MEM */
#define CPU_SEL_MIN		0
#define CPU_SEL_MAX		2
#define CPU_SEL_SHIFT		0
#define CPU_SEL_MASK		0x7
#define CPU_MODE_SHIFT		3
#define CPU_CNT_MIN		1
#define CPU_CNT_MAX		255
#define CPU_CNT_SHIFT		8
#define CPU_CNT_MASK		0xFF
#define CPU_CFGRQ_SHIFT		29
#define CPU_SYNRQ_SHIFT		30
#define CPU_CHGRQ_SHIFT		31
#define tcc_cpu_write(reg,cnt,sel) { \
	*(volatile unsigned *)reg = ((*(volatile unsigned *)reg)&(~((CPU_SEL_MASK<<CPU_SEL_SHIFT)|(1<<CPU_MODE_SHIFT)))) \
			|((sel&CPU_SEL_MASK)<<CPU_SEL_SHIFT); \
	while((*(volatile unsigned *)reg) & (1<<CPU_CHGRQ_SHIFT)); \
	*(volatile unsigned *)reg = ((*(volatile unsigned *)reg)&(~(CPU_CNT_MASK<<CPU_CNT_SHIFT))) \
			|((cnt&CPU_CNT_MASK)<<CPU_CNT_SHIFT); \
	while((*(volatile unsigned *)reg) & (1<<CPU_CFGRQ_SHIFT)); \
}
enum { /* CLKCTRL SEL */
	CPU_SEL_XIN=0,
	CPU_SEL_PLL,	/* cpu:pll3, mem:pll4 direct output */
	CPU_SEL_PLL0,
};
#endif

/* CLKCTRL - Other BUS */
#define CLKCTRL_SEL_MIN		0
#define CLKCTRL_SEL_MAX		15
#define CLKCTRL_SEL_SHIFT	0
#define CLKCTRL_SEL_MASK	0xF
#define CLKCTRL_CONFIG_MIN	1
#define CLKCTRL_CONFIG_MAX	15
#define CLKCTRL_CONFIG_SHIFT	4
#define CLKCTRL_CPU_MASK	0xFFFF
#define CLKCTRL_CONFIG_MASK	0xF
#define CLKCTRL_EN_SHIFT	21
#define CLKCTRL_CFGRQ_SHIFT	29
#define CLKCTRL_SYNRQ_SHIFT	30
#define CLKCTRL_CHGRQ_SHIFT	31
#define tcc_clkctrl_cpu_write(reg,en,config,sel) { \
	*(volatile unsigned *)reg = ((*(volatile unsigned *)reg)&(~(CLKCTRL_SEL_MASK<<CLKCTRL_SEL_SHIFT))) \
			|((sel&CLKCTRL_SEL_MASK)<<CLKCTRL_SEL_SHIFT); \
	while((*(volatile unsigned *)reg) & (1<<CLKCTRL_CHGRQ_SHIFT)); \
	*(volatile unsigned *)reg = ((*(volatile unsigned *)reg)&(~(CLKCTRL_CPU_MASK<<CLKCTRL_CONFIG_SHIFT))) \
			|((config&CLKCTRL_CPU_MASK)<<CLKCTRL_CONFIG_SHIFT); \
	while((*(volatile unsigned *)reg) & (1<<CLKCTRL_CFGRQ_SHIFT)); \
	*(volatile unsigned *)reg = ((*(volatile unsigned *)reg)&(~(1<<CLKCTRL_EN_SHIFT))) \
			|((en&1)<<CLKCTRL_EN_SHIFT); \
	while((*(volatile unsigned *)reg) & (1<<CLKCTRL_CFGRQ_SHIFT)); \
}
#define tcc_clkctrl_write(reg,en,config,sel) { \
	*(volatile unsigned *)reg = ((*(volatile unsigned *)reg)&(~(CLKCTRL_SEL_MASK<<CLKCTRL_SEL_SHIFT))) \
			|((sel&CLKCTRL_SEL_MASK)<<CLKCTRL_SEL_SHIFT); \
	while((*(volatile unsigned *)reg) & (1<<CLKCTRL_CHGRQ_SHIFT)); \
	*(volatile unsigned *)reg = ((*(volatile unsigned *)reg)&(~(CLKCTRL_CONFIG_MASK<<CLKCTRL_CONFIG_SHIFT))) \
			|((config&CLKCTRL_CONFIG_MASK)<<CLKCTRL_CONFIG_SHIFT); \
	while((*(volatile unsigned *)reg) & (1<<CLKCTRL_CFGRQ_SHIFT)); \
	*(volatile unsigned *)reg = ((*(volatile unsigned *)reg)&(~(1<<CLKCTRL_EN_SHIFT))) \
			|((en&1)<<CLKCTRL_EN_SHIFT); \
	while((*(volatile unsigned *)reg) & (1<<CLKCTRL_CFGRQ_SHIFT)); \
}
enum { /* CLKCTRL SEL */
	CLKCTRL_SEL_PLL0=0,
	CLKCTRL_SEL_PLL1,
	CLKCTRL_SEL_PLL2,
	CLKCTRL_SEL_XIN = 4,
	CLKCTRL_SEL_PLL0DIV,
	CLKCTRL_SEL_PLL1DIV,
	CLKCTRL_SEL_XTIN,
	CLKCTRL_SEL_EXTIN0,
	CLKCTRL_SEL_EXTIN1,
	CLKCTRL_SEL_PLL2DIV,
	CLKCTRL_SEL_EXTIN2 = 12,
	CLKCTRL_SEL_EXTIN3,
	CLKCTRL_SEL_XINDIV,
	CLKCTRL_SEL_XTINDIV,
};

/* PeriPheral Clocks */
#define PCLKCTRL_MAX_FCKS	(1600*1000*1000)
#define PCLKCTRL_DIV_MIN	0
#define PCLKCTRL_DIV_DCO_MIN	1
#define PCLKCTRL_DIV_SHIFT	0
#define PCLKCTRL_DIV_XXX_MAX	0xFFF
#define PCLKCTRL_DIV_XXX_MASK	PCLKCTRL_DIV_XXX_MAX
#define PCLKCTRL_DIV_YYY_MAX	0xFFFFFF
#define PCLKCTRL_DIV_YYY_MASK	PCLKCTRL_DIV_YYY_MAX
#define PCLKCTRL_SEL_MIN	0
#define PCLKCTRL_SEL_MAX	28
#define PCLKCTRL_SEL_SHIFT	24
#define PCLKCTRL_SEL_MASK	0x1F
#define PCLKCTRL_EN_SHIFT	29
#define PCLKCTRL_MD_SHIFT	31
#define tcc_pclkctrl_write(reg,md,en,sel,div,type) { \
	volatile unsigned int div_mask; \
	*(volatile unsigned *)reg = ((*(volatile unsigned *)reg)&(~(1<<PCLKCTRL_EN_SHIFT))); \
	if (type == PCLKCTRL_TYPE_YYY) \
		div_mask = PCLKCTRL_DIV_YYY_MASK; \
	else \
		div_mask = PCLKCTRL_DIV_XXX_MASK; \
	*(volatile unsigned *)reg = ((*(volatile unsigned *)reg) & ~(div_mask<<PCLKCTRL_DIV_SHIFT)) \
				  | ((div&div_mask)<<PCLKCTRL_DIV_SHIFT); \
	*(volatile unsigned *)reg = ((*(volatile unsigned *)reg) & ~(PCLKCTRL_SEL_MASK<<PCLKCTRL_SEL_SHIFT)) \
				  | ((sel&PCLKCTRL_SEL_MASK)<<PCLKCTRL_SEL_SHIFT); \
	*(volatile unsigned *)reg = ((*(volatile unsigned *)reg) & ~(1<<PCLKCTRL_MD_SHIFT)) \
				  | ((md&1)<<PCLKCTRL_MD_SHIFT); \
	*(volatile unsigned *)reg = ((*(volatile unsigned *)reg) & ~(1<<PCLKCTRL_EN_SHIFT)) \
				  | ((en&1)<<PCLKCTRL_EN_SHIFT); \
}
typedef enum { /* PCLK Type */
	PCLKCTRL_TYPE_XXX=0,
	PCLKCTRL_TYPE_YYY,
	PCLKCTRL_TYPE_MAX
} tPCLKTYPE;
enum { /* PCLK Mode Selection */
	PCLKCTRL_MODE_DCO=0,
	PCLKCTRL_MODE_DIVIDER,
	PCLKCTRL_MODE_MAX
};
enum{ /* Peri. Clock Source */
	PCLKCTRL_SEL_PLL0=0,
	PCLKCTRL_SEL_PLL1,
	PCLKCTRL_SEL_PLL2,
	PCLKCTRL_SEL_XIN=4,
	PCLKCTRL_SEL_PLL0DIV,
	PCLKCTRL_SEL_PLL1DIV,
	PCLKCTRL_SEL_PLL2DIV,
	PCLKCTRL_SEL_XTIN=9,
	PCLKCTRL_SEL_HDMITMDS=11,
	PCLKCTRL_SEL_HDMIPCLK,
	PCLKCTRL_SEL_HDMIXIN,	// 24Mhz
	PCLKCTRL_SEL_XINDIV=16,
	PCLKCTRL_SEL_XTINDIV,
	PCLKCTRL_SEL_EXTCLK0=25,
	PCLKCTRL_SEL_EXTCLK1,
	PCLKCTRL_SEL_EXTCLK2,
	PCLKCTRL_SEL_EXTCLK3,
};

/* io bus configuration */
#define IOBUS_PWDN0	0x10
#define IOBUS_PWDN1	0x14
#define IOBUS_PWDN2	0x68
#define IOBUS_RESET0	0x18
#define IOBUS_RESET1	0x1C
#define IOBUS_RESET2	0x6C

/* display bus configuration */
#define DDIBUS_PWDN	0x00
#define DDIBUS_RESET	0x04

/* graphic bus configuration */
#define GPUBUS_PWDN	0x00
#define GPUBUS_RESET	0x04

/* video bus configuraion */
#define VPUBUS_PWDN	0x00
#define VPUBUS_RESET	0x04

/* hsio bus configuration */
#define HSIOBUS_PWDN	0x00
#define HSIOBUS_RESET	0x04


inline int tcc_find_pms(tPMS *PLL, unsigned int srcfreq)
{
	u64	u64_pll, u64_src, fvco, srch_p, srch_m, u64_tmp;
	unsigned int srch_pll, err, srch_err;
	int	srch_s;

	if (PLL->fpll ==0) {
		PLL->en = 0;
		return 0;
	}

	u64_pll = (u64)PLL->fpll;
	u64_src = (u64)srcfreq;

	err = 0xFFFFFFFF;
	srch_err = 0xFFFFFFFF;
	for (srch_s=PLL_S_MAX,fvco=(u64_pll<<PLL_S_MAX) ; srch_s >= PLL_S_MIN ; fvco=(u64_pll<<(--srch_s))) {
		if (fvco >= PLL_VCO_MIN && fvco <= PLL_VCO_MAX) {
			for (srch_p=PLL_P_MIN ; srch_p<=PLL_P_MAX ; srch_p++) {
				srch_m = fvco*srch_p;
				do_div(srch_m, srcfreq);
		                if (srch_m < PLL_M_MIN || srch_m > PLL_M_MAX)
		                        continue;
				u64_tmp = srch_m*u64_src;
				do_div(u64_tmp, srch_p);
		                srch_pll = (unsigned int)(u64_tmp>>srch_s);
				if (srch_pll < PLL_MIN_RATE || srch_pll > PLL_MAX_RATE)
					continue;
		                srch_err = (srch_pll > u64_pll) ? srch_pll - u64_pll : u64_pll - srch_pll;
		                if (srch_err < err) {
		                        err = srch_err;
		                        PLL->p = (unsigned int)srch_p;
		                        PLL->m = (unsigned int)srch_m;
					PLL->s = (unsigned int)srch_s;
		                }
			}
		}
	}
	if (err == 0xFFFFFFFF)
		return -1;

	u64_tmp = u64_src*(unsigned long long)PLL->m;
	do_div(u64_tmp, PLL->p);
	PLL->fpll = (unsigned int)(u64_tmp>>PLL->s);
	PLL->en = 1;
	return 0;
}

static int tcc_ckc_pll_set_rate(int id, unsigned long rate)
{
	void __iomem	*reg = ckc_base+CKC_PLL+id*4;
	unsigned int	srcfreq = 0;
	unsigned int	src = PLLSRC_XIN;
	tPMS		nPLL;

	if (id >= MAX_TCC_PLL)
		return -1;

	if (rate < PLL_MIN_RATE || rate > PLL_MAX_RATE)
		return -1;

	memset(&nPLL, 0x0, sizeof(tPMS));

	switch(src) {
	case PLLSRC_XIN:
		srcfreq = XIN_CLK_RATE;
		break;
	case PLLSRC_HDMIXI:
		srcfreq = HDMI_CLK_RATE;
		break;
	case PLLSRC_EXTCLK0:
		srcfreq = EXT0_CLK_RATE;
		break;
	case PLLSRC_EXTCLK1:
		srcfreq = EXT1_CLK_RATE;
		break;
	default:
		goto tca_ckc_setpll_failed;
	}
	if (srcfreq==0)
		goto tca_ckc_setpll_failed;

	nPLL.fpll = rate;
	if (tcc_find_pms(&nPLL, srcfreq))
		goto tca_ckc_setpll_failed;
#if defined(CPU_CLOCK_CHANGE_BY_AUTO_MODE)
	if (id == CPU0_SRC_CH) {
		tcc_pll_write_cb(reg, nPLL.p, nPLL.m, nPLL.s, src);
	}
	else 
#endif
	{
		tcc_pll_write(reg, nPLL.en, nPLL.p, nPLL.m, nPLL.s, src);
	}
	tcc_ckc_reset_clock_source(id);
	return 0;

tca_ckc_setpll_failed:
	tcc_pll_write(reg, 0, PLL_P_MIN, (PLL_P_MIN*PLL_VCO_MIN+XIN_CLK_RATE)/XIN_CLK_RATE, PLL_S_MIN, src);
	tcc_ckc_reset_clock_source(id);
	return -1;
}

static unsigned long tcc_ckc_pll_get_rate(int id)
{
	void __iomem	*reg = ckc_base+CKC_PLL+id*4;
	unsigned	reg_values = ckc_readl(reg);
	tPMS		nPLLCFG;
	unsigned int	src_freq;
	u64		u64_tmp;

	if (id >= MAX_TCC_PLL)
		return 0;

	nPLLCFG.p = (reg_values>>PLL_P_SHIFT)&(PLL_P_MASK);
	nPLLCFG.m = (reg_values>>PLL_M_SHIFT)&(PLL_M_MASK);
	nPLLCFG.s = (reg_values>>PLL_S_SHIFT)&(PLL_S_MASK);
	nPLLCFG.en = (reg_values>>PLL_EN_SHIFT)&(1);
	nPLLCFG.src = (reg_values>>PLL_SRC_SHIFT)&(PLL_SRC_MASK);

	if (nPLLCFG.en == 0)
		return 0;

	switch (nPLLCFG.src) {
	case PLLSRC_XIN:
		src_freq = XIN_CLK_RATE;
		break;
	case PLLSRC_HDMIXI:
		src_freq = HDMI_CLK_RATE;
		break;
	case PLLSRC_EXTCLK0:
		src_freq = EXT0_CLK_RATE;
		break;
	case PLLSRC_EXTCLK1:
		src_freq = EXT1_CLK_RATE;
		break;
	default:
		return 0;
	}

	u64_tmp = (u64)src_freq*(u64)nPLLCFG.m;
	do_div(u64_tmp, nPLLCFG.p);
	return (unsigned int)((u64_tmp)>>nPLLCFG.s);
}

static int tcc_ckc_pll_div_set(int id, unsigned int div)
{
	void __iomem	*reg;
	unsigned	reg_values;
	unsigned int	offset=0;

	if (id >= MAX_TCC_PLL)
		return 0;

	switch(id) {
	case PLL_0:
	case PLL_1:
	case PLL_2:
		reg = (void __iomem *)(ckc_base+CKC_CLKDIVC);
		offset = (3-id)*8;
		break;
	default:
		return -1;
	}

	reg_values = ckc_readl(reg) & ~(0xFF<<offset);
	ckc_writel(reg_values, reg);
	if (div)
		reg_values |= (0x80|(div&0x3F))<<offset;
	else
		reg_values |= 1<<offset;
	ckc_writel(reg_values, reg);

	return 0;
}

static unsigned long tcc_ckc_plldiv_get_rate(int id)
{
	void __iomem	*reg;
	unsigned	reg_values;
	unsigned int	offset=0, fpll=0, pdiv=0;

	if (id >= MAX_TCC_PLL)
		return 0;

	switch(id) {
	case PLL_0:
	case PLL_1:
	case PLL_2:
		reg = ckc_base+CKC_CLKDIVC;
		offset = (3-id)*8;
		break;
	default:
		return 0;
	}

	reg_values = ckc_readl(reg);
	if (((reg_values >> offset)&0x80) == 0)	/* check plldivc enable bit */
		return 0;
	pdiv = (reg_values >> offset)&0x3F;
	if (!pdiv)	/* should not be zero */
		return 0;
	fpll = tcc_ckc_pll_get_rate(id);
	return (unsigned int)fpll/(pdiv+1);
}

static inline int tcc_find_clkctrl(tCLKCTRL *CLKCTRL)
{
	unsigned int i, div[MAX_CLK_SRC], err[MAX_CLK_SRC], searchsrc, clk_rate;
	searchsrc = 0xFFFFFFFF;

	if (CLKCTRL->freq <= (XIN_CLK_RATE/2)) {
		CLKCTRL->sel = CLKCTRL_SEL_XIN;
		CLKCTRL->freq = XIN_CLK_RATE/2;
		CLKCTRL->config = 1;
	}
	else {
		for (i=0 ; i<MAX_CLK_SRC ; i++) {
			if (stClockSource[i] == 0)
				continue;
			div[i] = (stClockSource[i]+CLKCTRL->freq-1)/CLKCTRL->freq;
			if (div[i] > (CLKCTRL_CONFIG_MAX+1))
				div[i] = CLKCTRL_CONFIG_MAX+1;
			else if (div[i] < (CLKCTRL_CONFIG_MIN+1))
				div[i] = CLKCTRL_CONFIG_MIN+1;
			clk_rate = stClockSource[i]/div[i];
			if (CLKCTRL->freq < clk_rate)
				continue;
			err[i] = CLKCTRL->freq - clk_rate;
			if (searchsrc == 0xFFFFFFFF)
				searchsrc = i;
			else {
				/* find similar clock */
				if (err[i] < err[searchsrc])
					searchsrc = i;
				/* find even division vlaue */
				else if(err[i] == err[searchsrc]) {
					if (div[i]%2 == 0)
						searchsrc = i;
				}
			}
			if (err[searchsrc] == 0)
				break;
		}
		if (searchsrc == 0xFFFFFFFF)
			return -1;
		switch(searchsrc) {
		case PLL_0:	CLKCTRL->sel = CLKCTRL_SEL_PLL0; break;
		case PLL_1:	CLKCTRL->sel = CLKCTRL_SEL_PLL1; break;
		case PLL_2:	CLKCTRL->sel = CLKCTRL_SEL_PLL2; break;
		case PLL_DIV_0:	CLKCTRL->sel = CLKCTRL_SEL_PLL0DIV; break;
		case PLL_DIV_1:	CLKCTRL->sel = CLKCTRL_SEL_PLL1DIV; break;
		case PLL_DIV_2:	CLKCTRL->sel = CLKCTRL_SEL_PLL2DIV; break;
		case PLL_XIN:	CLKCTRL->sel = CLKCTRL_SEL_XIN; break;
		default: return -1;
		}
		if (div[searchsrc] > (CLKCTRL_CONFIG_MAX+1))
			div[searchsrc] = CLKCTRL_CONFIG_MAX+1;
		else if (div[searchsrc] <= CLKCTRL_CONFIG_MIN)
			div[searchsrc] = CLKCTRL_CONFIG_MIN+1;
		CLKCTRL->freq = stClockSource[searchsrc]/div[searchsrc];
		CLKCTRL->config = div[searchsrc] - 1;
	}
	return 0;
}

static inline int tcc_find_clkctrl_for_cpu1(tCLKCTRL *CLKCTRL)
{
	unsigned int i, div[MAX_CLK_SRC], err[MAX_CLK_SRC], searchsrc, clk_rate, unit;
	searchsrc = 0xFFFFFFFF;

	for (i=0 ; i<MAX_CLK_SRC ; i++) {
		if (stClockSource[i] == 0)
			continue;
		unit = stClockSource[i]/(CLKCTRL_CONFIG_MAX+1);
		div[i] = CLKCTRL->freq/unit;
		if (div[i] > (CLKCTRL_CONFIG_MAX+1))
			div[i] = CLKCTRL_CONFIG_MAX+1;
		clk_rate = div[i]*unit;
		if (CLKCTRL->freq < clk_rate)
			continue;
		err[i] = CLKCTRL->freq - clk_rate;
		if (searchsrc == 0xFFFFFFFF)
			searchsrc = i;
		else if (err[i] < err[searchsrc])
			searchsrc = i;

		if (err[searchsrc] == 0)
			break;
	}
	if (searchsrc == 0xFFFFFFFF)
		return -1;
	switch(searchsrc) {
	case PLL_0:	CLKCTRL->sel = CLKCTRL_SEL_PLL0; break;
	case PLL_1:	CLKCTRL->sel = CLKCTRL_SEL_PLL1; break;
	case PLL_2:	CLKCTRL->sel = CLKCTRL_SEL_PLL2; break;
	case PLL_DIV_0:	CLKCTRL->sel = CLKCTRL_SEL_PLL0DIV; break;
	case PLL_DIV_1:	CLKCTRL->sel = CLKCTRL_SEL_PLL1DIV; break;
	case PLL_DIV_2:	CLKCTRL->sel = CLKCTRL_SEL_PLL2DIV; break;
	case PLL_XIN:	CLKCTRL->sel = CLKCTRL_SEL_XIN; break;
	default: return -1;
	}
	if (div[searchsrc] > (CLKCTRL_CONFIG_MAX+1))
		div[searchsrc] = CLKCTRL_CONFIG_MAX+1;
	else if (div[searchsrc] <= CLKCTRL_CONFIG_MIN)
		div[searchsrc] = CLKCTRL_CONFIG_MIN;
	CLKCTRL->freq = div[searchsrc]*(stClockSource[searchsrc]/(CLKCTRL_CONFIG_MAX+1));
	CLKCTRL->config = 0;
	for (i=0 ; i<div[searchsrc] ; i++)
		CLKCTRL->config |= (1<<i);

	return 0;
}

static int tcc_ckc_clkctrl_enable(int id)
{
	void __iomem	*reg = ckc_base+CKC_CLKCTRL+id*4;

	if (id == FBUS_CPU0 || id == FBUS_MEM)
		return 0;

	ckc_writel(ckc_readl(reg) | (1<<CLKCTRL_EN_SHIFT), reg);
	while (ckc_readl(reg) & (1<<CLKCTRL_CFGRQ_SHIFT));

	return 0;
}

static int tcc_ckc_clkctrl_disable(int id)
{
	void __iomem	*reg = ckc_base+CKC_CLKCTRL+id*4;

	if (id == FBUS_CPU0 || id == FBUS_MEM)
		return 0;

	ckc_writel(ckc_readl(reg) & ~(1<<CLKCTRL_EN_SHIFT), reg);
	return 0;
}

static int tcc_ckc_is_clkctrl_enabled(int id)
{
	void __iomem	*reg = ckc_base+CKC_CLKCTRL+id*4;

	if (id == FBUS_CPU0 || id == FBUS_MEM)
		return 1;

	return (ckc_readl(reg) & (1<<CLKCTRL_EN_SHIFT)) ? 1 : 0;
}

static int tcc_ckc_clkctrl_set_rate(int id, unsigned long rate)
{
	void __iomem	*reg = ckc_base+CKC_CLKCTRL+id*4;
	tCLKCTRL	nCLKCTRL;

	nCLKCTRL.en = (ckc_readl(reg) & (1<<CLKCTRL_EN_SHIFT)) ? 1 : 0;
	switch (id) {
	case FBUS_CPU0:
#if defined(CPU_CLOCK_CHANGE_BY_AUTO_MODE)
		tcc_ckc_pll_set_rate(CPU0_SRC_CH, rate);
#else
		tcc_cpu_write(reg, CPU_CNT_MASK, CPU_SEL_XIN);
		tcc_ckc_pll_set_rate(CPU0_SRC_CH, rate);
		tcc_cpu_write(reg, CPU_CNT_MASK, CPU_SEL_PLL);
#endif
		return 0;
	case FBUS_CPU1:
		nCLKCTRL.freq = rate;
		if (tcc_find_clkctrl_for_cpu1(&nCLKCTRL))
			return -1;
		nCLKCTRL.en = ckc_readl(reg) & (1<<CLKCTRL_EN_SHIFT) ? 1 : 0;
		tcc_clkctrl_cpu_write(reg, nCLKCTRL.en, nCLKCTRL.config, nCLKCTRL.sel);
		return 0;
	case FBUS_MEM:
		//dprintf(INFO, "memory clock cannot change !!\n");
		//while (1);
		change_mem_clock(rate); // for test
		return 0;
	}

	nCLKCTRL.freq = rate;
	if (tcc_find_clkctrl(&nCLKCTRL))
		return -1;

	tcc_clkctrl_write(reg, nCLKCTRL.en, nCLKCTRL.config, nCLKCTRL.sel);
	return 0;
}

static unsigned long tcc_ckc_clkctrl_get_rate(int id)
{
	void __iomem	*reg = ckc_base+CKC_CLKCTRL+id*4;
	unsigned	reg_values = ckc_readl(reg);
	tCLKCTRL	nCLKCTRL;
	unsigned int	src_freq = 0;

	if(id == FBUS_CPU0)
		return tcc_ckc_pll_get_rate(CPU0_SRC_CH);
	else if(id == FBUS_MEM)
		return tcc_ckc_pll_get_rate(MEM_SRC_CH);

	nCLKCTRL.sel = (reg_values & (CLKCTRL_SEL_MASK<<CLKCTRL_SEL_SHIFT))>>CLKCTRL_SEL_SHIFT;
	switch (nCLKCTRL.sel) {
	case CLKCTRL_SEL_PLL0:
		src_freq =	tcc_ckc_pll_get_rate(PLL_0);
		break;
	case CLKCTRL_SEL_PLL1:
		src_freq =	tcc_ckc_pll_get_rate(PLL_1);
		break;
	case CLKCTRL_SEL_PLL2:
		src_freq =	tcc_ckc_pll_get_rate(PLL_2);
		break;
	case CLKCTRL_SEL_XIN:
		src_freq =	XIN_CLK_RATE;
		break;
	case CLKCTRL_SEL_XTIN:
		src_freq =	XTIN_CLK_RATE;
		break;
	case CLKCTRL_SEL_PLL0DIV:
		src_freq =	tcc_ckc_plldiv_get_rate(PLL_0);
		break;
	case CLKCTRL_SEL_PLL1DIV:
		src_freq =	tcc_ckc_plldiv_get_rate(PLL_1);
		break;
	case CLKCTRL_SEL_PLL2DIV:
		src_freq =	tcc_ckc_plldiv_get_rate(PLL_2);
		break;
/*
	case CLKCTRL_SEL_XINDIV:
		src_freq =	XIN_CLK_RATE/2;
		break;
	case CLKCTRL_SEL_XTINDIV:
		src_freq =	XTIN_CLK_RATE/2;
		break;
*/
	default: return 0;
	}

	if (id == FBUS_CPU1) {
		int i, lcnt=0;
		u64 u64_tmp;
		nCLKCTRL.config = (reg_values & (CLKCTRL_CPU_MASK<<CLKCTRL_CONFIG_SHIFT))>>CLKCTRL_CONFIG_SHIFT;
		for(i = 0; i < (CLKCTRL_CONFIG_MAX+1); i++) {
			if((nCLKCTRL.config & 0x1))
				lcnt++;
			nCLKCTRL.config = nCLKCTRL.config>>1;
		}
		u64_tmp = (u64)src_freq * (u64)lcnt;
		do_div(u64_tmp, (CLKCTRL_CONFIG_MAX+1));
		nCLKCTRL.freq = (unsigned int)u64_tmp;
	}
	else {
		nCLKCTRL.config = (reg_values & (CLKCTRL_CONFIG_MASK<<CLKCTRL_CONFIG_SHIFT))>>CLKCTRL_CONFIG_SHIFT;
			nCLKCTRL.freq = src_freq / (nCLKCTRL.config+1);
	}

	return (unsigned long)nCLKCTRL.freq;
}

static inline unsigned int tcc_pclk_support_below_freq(unsigned int periname)
{
	if ((periname == PERI_SDMMC0) || (periname == PERI_SDMMC1)
	 || (periname == PERI_SDMMC2) || (periname == PERI_SDMMC3)) {
	 	/* calc. freq. must be below(same or under) value */
	 	return 1;
	}

	return 0;
}

static inline unsigned int tcc_ckc_pclk_divider(tPCLKCTRL *PCLKCTRL, unsigned int *div,
	const unsigned int src_CLK, unsigned int div_min, unsigned int div_max)
{
	unsigned int	clk_rate1, clk_rate2, err1, err2;

	if (src_CLK <= PCLKCTRL->freq)
		*div = 1;
	else
		*div = src_CLK/PCLKCTRL->freq;

#ifdef HDMIA_SRC_SHOULD_BE_SAME_WITH_AUDIO
	switch (PCLKCTRL->periname) {
	case PERI_ADAI3:
	case PERI_ADAI0:
	case PERI_ADAI1:
	case PERI_ADAI2:
		if (*div < 2) /* dai should be (hdmi_audio*2) */
			*div = 2;
		break;
	}
#endif

	if (*div > div_max)
		*div = div_max;

	clk_rate1 = src_CLK/(*div);
	clk_rate2 = src_CLK/((*div < div_max) ? (*div+1) : *div);
	if (tcc_pclk_support_below_freq(PCLKCTRL->periname)) {
		err1 = (clk_rate1 > PCLKCTRL->freq)?0xFFFFFFFF:(PCLKCTRL->freq - clk_rate1);
		err2 = (clk_rate2 > PCLKCTRL->freq)?0xFFFFFFFF:(PCLKCTRL->freq - clk_rate2);
	}
	else {
		err1 = (clk_rate1 > PCLKCTRL->freq)?(clk_rate1 - PCLKCTRL->freq):(PCLKCTRL->freq - clk_rate1);
		err2 = (clk_rate2 > PCLKCTRL->freq)?(clk_rate2 - PCLKCTRL->freq):(PCLKCTRL->freq - clk_rate2);
	}

	if (err1 > err2)
		*div += 1;

	return (err1 < err2) ? err1 : err2;
}

static inline unsigned int tcc_ckc_pclk_dco(	tPCLKCTRL *PCLKCTRL, unsigned int *div,
	const unsigned int src_CLK, unsigned int div_min, unsigned int div_max)
{
	unsigned int	clk_rate1, clk_rate2, err1, err2;
	u64 u64_tmp;

	if (src_CLK < PCLKCTRL->freq)
		return 0xFFFFFFFF;

	u64_tmp = (unsigned long long)(PCLKCTRL->freq)*(unsigned long long)div_max;
	do_div(u64_tmp,src_CLK);
	*div = (unsigned int)u64_tmp;

	if (*div > (div_max+1)/2)
		return 0xFFFFFFFF;

#ifdef HDMIA_SRC_SHOULD_BE_SAME_WITH_AUDIO
	switch (PCLKCTRL->periname) {
	case PERI_ADAI3:
	case PERI_ADAI0:
	case PERI_ADAI1:
	case PERI_ADAI2:
		if (*div > (div_max+1)/4) /* dai should be (hdmi_audio*2) */
			return 0xFFFFFFFF;
		break;
	}
#endif

	u64_tmp = (unsigned long long)src_CLK*(unsigned long long)(*div);
	do_div(u64_tmp,(div_max+1));
	clk_rate1 = (unsigned int)u64_tmp;
	u64_tmp = (unsigned long long)src_CLK*(unsigned long long)(*div+1);
	do_div(u64_tmp,(div_max+1));
	clk_rate2 = (unsigned int)u64_tmp;

	if (tcc_pclk_support_below_freq(PCLKCTRL->periname)) {
		err1 = (clk_rate1 > PCLKCTRL->freq) ? 0xFFFFFFFF : PCLKCTRL->freq - clk_rate1;
		err2 = (clk_rate2 > PCLKCTRL->freq) ? 0xFFFFFFFF : PCLKCTRL->freq - clk_rate2;
	}
	else {
		err1 = (clk_rate1 > PCLKCTRL->freq) ? clk_rate1 - PCLKCTRL->freq : PCLKCTRL->freq - clk_rate1;
		err2 = (clk_rate2 > PCLKCTRL->freq) ? clk_rate2 - PCLKCTRL->freq : PCLKCTRL->freq - clk_rate2;
	}
	if (err1 > err2)
		*div += 1;

	return (err1 < err2) ? err1 : err2;
}

static inline int tcc_find_pclk(tPCLKCTRL *PCLKCTRL, tPCLKTYPE type)
{
	int i;
	unsigned int div_max, searchsrc, err_dco, err_div, md;
	unsigned int div[MAX_CLK_SRC], err[MAX_CLK_SRC];
	unsigned int div_dco = PCLKCTRL_DIV_DCO_MIN;
	unsigned int div_div = PCLKCTRL_DIV_MIN;

	switch (type) {
	case PCLKCTRL_TYPE_XXX:
		PCLKCTRL->md = PCLKCTRL_MODE_DIVIDER;
		div_max = PCLKCTRL_DIV_XXX_MAX;
		break;
	case PCLKCTRL_TYPE_YYY:
		PCLKCTRL->md = PCLKCTRL_MODE_DCO;
		div_max = PCLKCTRL_DIV_YYY_MAX;
		if (((PCLKCTRL->periname == PERI_ADAI0)
#ifdef HDMIA_SRC_SHOULD_BE_SAME_WITH_AUDIO
			|| ((PCLKCTRL->periname == PERI_HDMIA) && (stHDMIASrc == audio_pll_ch))
#endif
		     ) && audio_pll_used) {
			if (tcc_find_audio_pclk(PCLKCTRL) == 0)
				return 0;
		}
		break;
	default:
		return -1;
	}

	searchsrc = 0xFFFFFFFF;
	for (i=(MAX_CLK_SRC-1) ; i>=0 ; i--) {
#ifdef HDMIA_SRC_SHOULD_BE_SAME_WITH_AUDIO
		if ((stHDMIASrc < MAX_CLK_SRC) && (PCLKCTRL->periname == PERI_HDMIA))
			i = stHDMIASrc;
		else
#endif
		{
			if (stClockSource[i] == 0)
				continue;
			if ((stClockSource[i] >= PCLKCTRL_MAX_FCKS) && (type == PCLKCTRL_TYPE_XXX))
				continue;
		}

		/* dco mode */
		if (type == PCLKCTRL_TYPE_XXX)
			err_dco = 0xFFFFFFFF;
		else
			err_dco = tcc_ckc_pclk_dco(PCLKCTRL, &div_dco, stClockSource[i], PCLKCTRL_DIV_DCO_MIN, div_max);

		/* divider mode */
		err_div = tcc_ckc_pclk_divider(PCLKCTRL, &div_div, stClockSource[i], PCLKCTRL_DIV_MIN+1, div_max+1);

		/* common */
		if (err_dco < err_div) {
			err[i] = err_dco;
			div[i] = div_dco;
			md = PCLKCTRL_MODE_DCO;
		}
		else {
			err[i] = err_div;
			div[i] = div_div;
			md = PCLKCTRL_MODE_DIVIDER;
		}

		if (searchsrc == 0xFFFFFFFF) {
			searchsrc = i;
			PCLKCTRL->md = md;
		}
		else {
			/* find similar clock */
			if (err[i] < err[searchsrc]) {
				searchsrc = i;
				PCLKCTRL->md = md;
			}
		}

#ifdef HDMIA_SRC_SHOULD_BE_SAME_WITH_AUDIO
		if ((stHDMIASrc < MAX_CLK_SRC) && (PCLKCTRL->periname == PERI_HDMIA))
			break;
#endif

		if (err[searchsrc] == 0)
			break;
	}

	switch(searchsrc) {
	case PLL_0:	PCLKCTRL->sel = PCLKCTRL_SEL_PLL0; break;
	case PLL_1:	PCLKCTRL->sel = PCLKCTRL_SEL_PLL1; break;
	case PLL_2:	PCLKCTRL->sel = PCLKCTRL_SEL_PLL2; break;
	case PLL_XIN:	PCLKCTRL->sel = PCLKCTRL_SEL_XIN; break;
	case PLL_DIV_0:	PCLKCTRL->sel = PCLKCTRL_SEL_PLL0DIV; break;
	case PLL_DIV_1:	PCLKCTRL->sel = PCLKCTRL_SEL_PLL1DIV; break;
	case PLL_DIV_2:	PCLKCTRL->sel = PCLKCTRL_SEL_PLL2DIV; break;
	case PLL_XTIN:	PCLKCTRL->sel = PCLKCTRL_SEL_XTIN; break;
	default: return -1;
	}

	if (PCLKCTRL->md == PCLKCTRL_MODE_DCO) {
		u64 u64_tmp;
		PCLKCTRL->div = div[searchsrc];
		if (PCLKCTRL->div > div_max/2)
			u64_tmp = (unsigned long long)stClockSource[searchsrc]*(unsigned long long)(div_max-PCLKCTRL->div);
		else
			u64_tmp = (unsigned long long)stClockSource[searchsrc]*(unsigned long long)PCLKCTRL->div;
		do_div(u64_tmp,div_max);
		PCLKCTRL->freq = (unsigned int)u64_tmp;

		if ((PCLKCTRL->div < PCLKCTRL_DIV_DCO_MIN) || (PCLKCTRL->div > (div_max-1)))
			return -1;
	}
	else { /* Divider mode */
		PCLKCTRL->div = div[searchsrc];
		if (PCLKCTRL->div >= (PCLKCTRL_DIV_MIN+1) && PCLKCTRL->div <= (div_max+1))
			PCLKCTRL->div -= 1;
		else
			return -1;
		PCLKCTRL->freq = stClockSource[searchsrc]/(PCLKCTRL->div+1);
	}
	return 0;
}

static inline tPCLKTYPE tcc_check_pclk_type(unsigned int periname)
{
	switch (periname) {
		case PERI_LCD0:
		case PERI_LCD1:
		case PERI_HDMI:
		case PERI_HDMIA:
		case PERI_ADAI3:
		case PERI_ADAM3:
		case PERI_SPDIF3:
		case PERI_ADAI0:
		case PERI_ADAM0:
		case PERI_SPDIF0:
		case PERI_ADC:
		case PERI_CMBUS_TSIF0:
		case PERI_CMBUS_TSIF1:
		case PERI_CMBUS_TSIF2:
		case PERI_CMBUS_TSIF3:
		case PERI_ADAI1:
		case PERI_ADAM1:
		case PERI_SPDIF1:
		case PERI_ADAI2:
		case PERI_ADAM2:
		case PERI_SPDIF2:
			return PCLKCTRL_TYPE_YYY;
	}
	return PCLKCTRL_TYPE_XXX;
}

static int tcc_ckc_peri_enable(int id)
{
	void __iomem	*reg = ckc_base+CKC_PCLKCTRL+id*4;
	ckc_writel(ckc_readl(reg) | 1<<PCLKCTRL_EN_SHIFT, reg);

	return 0;
}

static int tcc_ckc_peri_disable(int id)
{
	void __iomem	*reg = ckc_base+CKC_PCLKCTRL+id*4;
	ckc_writel(ckc_readl(reg) & ~(1<<PCLKCTRL_EN_SHIFT), reg);

	return 0;
}

static int tcc_ckc_is_peri_enabled(int id)
{
	void __iomem	*reg = ckc_base+CKC_PCLKCTRL+id*4;
	return (ckc_readl(reg) & (1<<PCLKCTRL_EN_SHIFT)) ? 1 : 0;
}

static int tcc_ckc_peri_set_rate(int id, unsigned long rate)
{
	void __iomem	*reg = ckc_base+CKC_PCLKCTRL+id*4;
	tPCLKCTRL	nPCLKCTRL;
	tPCLKTYPE	type = tcc_check_pclk_type(id);

	nPCLKCTRL.freq = rate;
	nPCLKCTRL.periname = id;
	nPCLKCTRL.div = 0;
	nPCLKCTRL.md = PCLKCTRL_MODE_DCO;
	nPCLKCTRL.sel = PCLKCTRL_SEL_XIN;

	/* if input rate are 27(dummy value for set HDMIPCLK) then set the lcdc pclk source to HDMIPCLK */
	if (((nPCLKCTRL.periname == PERI_LCD0) || (nPCLKCTRL.periname == PERI_LCD1))
			&& (nPCLKCTRL.freq == HDMI_PCLK_RATE)) {
		nPCLKCTRL.sel = PCLKCTRL_SEL_HDMIPCLK;
		nPCLKCTRL.div = 0;
		nPCLKCTRL.md = PCLKCTRL_MODE_DIVIDER;
		nPCLKCTRL.freq = HDMI_PCLK_RATE;
	}
	else {
		if (tcc_find_pclk(&nPCLKCTRL, type))
			goto tca_ckc_setperi_failed;
	}

	nPCLKCTRL.en = (ckc_readl(reg) & (1<<PCLKCTRL_EN_SHIFT)) ? 1 : 0;

	tcc_pclkctrl_write(reg, nPCLKCTRL.md, nPCLKCTRL.en, nPCLKCTRL.sel, nPCLKCTRL.div, type);

	return 0;

tca_ckc_setperi_failed:
	tcc_pclkctrl_write(reg, PCLKCTRL_MODE_DIVIDER, CKC_DISABLE, PCLKCTRL_SEL_XIN, 1, type);
	return -1;
}

static unsigned long tcc_ckc_peri_get_rate(int id)
{
	void __iomem	*reg = ckc_base+CKC_PCLKCTRL+id*4;
	unsigned	reg_values = ckc_readl(reg);
	tPCLKCTRL	nPCLKCTRL;
	tPCLKTYPE	type = tcc_check_pclk_type(id);
	unsigned int	src_freq = 0, div_mask;

	nPCLKCTRL.md = reg_values & (1<<PCLKCTRL_MD_SHIFT) ? PCLKCTRL_MODE_DIVIDER : PCLKCTRL_MODE_DCO;
	nPCLKCTRL.sel = (reg_values&(PCLKCTRL_SEL_MASK<<PCLKCTRL_SEL_SHIFT))>>PCLKCTRL_SEL_SHIFT;
	switch(nPCLKCTRL.sel) {
	case PCLKCTRL_SEL_PLL0:
		src_freq = tcc_ckc_pll_get_rate(PLL_0);
		break;
	case PCLKCTRL_SEL_PLL1:
		src_freq = tcc_ckc_pll_get_rate(PLL_1);
		break;
	case PCLKCTRL_SEL_PLL2:
		src_freq = tcc_ckc_pll_get_rate(PLL_2);
		break;
	case PCLKCTRL_SEL_XIN:
		src_freq = XIN_CLK_RATE;
		break;
	case PCLKCTRL_SEL_PLL0DIV:
		src_freq = tcc_ckc_plldiv_get_rate(PLL_0);
		break;
	case PCLKCTRL_SEL_PLL1DIV:
		src_freq = tcc_ckc_plldiv_get_rate(PLL_1);
		break;
	case PCLKCTRL_SEL_PLL2DIV:
		src_freq = tcc_ckc_plldiv_get_rate(PLL_2);
		break;
	case PCLKCTRL_SEL_XTIN:
		src_freq = XTIN_CLK_RATE;
		break;
	case PCLKCTRL_SEL_HDMITMDS:
	case PCLKCTRL_SEL_HDMIPCLK:
	case PCLKCTRL_SEL_HDMIXIN:
	case PCLKCTRL_SEL_XINDIV:
	case PCLKCTRL_SEL_XTINDIV:
	case PCLKCTRL_SEL_EXTCLK0:
	case PCLKCTRL_SEL_EXTCLK1:
	case PCLKCTRL_SEL_EXTCLK2:
	case PCLKCTRL_SEL_EXTCLK3:
	default :
		return 0;
	}

	switch (type) {
	case PCLKCTRL_TYPE_XXX:
		div_mask = PCLKCTRL_DIV_XXX_MASK;
		nPCLKCTRL.md = PCLKCTRL_MODE_DIVIDER;
		break;
	case PCLKCTRL_TYPE_YYY:
		div_mask = PCLKCTRL_DIV_YYY_MASK;
		break;
	default:
		return 0;
	}
	nPCLKCTRL.freq = 0;
	nPCLKCTRL.div = (reg_values&(div_mask<<PCLKCTRL_DIV_SHIFT))>>PCLKCTRL_DIV_SHIFT;
	if (nPCLKCTRL.md == PCLKCTRL_MODE_DIVIDER)
		nPCLKCTRL.freq = src_freq/(nPCLKCTRL.div+1);
	else {
		u64 u64_tmp;
		if (nPCLKCTRL.div > (div_mask+1)/2)
			u64_tmp = (unsigned long long)src_freq*(unsigned long long)((div_mask+1)-nPCLKCTRL.div);
		else
			u64_tmp = (unsigned long long)src_freq*(unsigned long long)nPCLKCTRL.div;
		do_div(u64_tmp ,div_mask+1);
		nPCLKCTRL.freq = (unsigned int)u64_tmp;
	}
	return (unsigned long)nPCLKCTRL.freq;
}

static inline int tcc_ckc_set_audio_pll(unsigned long rate)
{
	void __iomem	*reg = ckc_base+CKC_PLL+audio_pll_ch*4;
	tPMS		nPLL, dco_PLL, div_PLL;
	unsigned int	pll, real_pll, cmp_value, int_tmp, real_pclk, dco_err, div_err;
	u64		u64_tmp;

	memset(&dco_PLL, 0x0, sizeof(tPMS));
	memset(&div_PLL, 0x0, sizeof(tPMS));
	dco_err = 0xFFFFFFFF;
	div_err = 0xFFFFFFFF;

	/* divider mode */
#ifdef HDMIA_SRC_SHOULD_BE_SAME_WITH_AUDIO
	for (int_tmp = (PCLKCTRL_DIV_YYY_MAX+1) ; int_tmp > 1 ; int_tmp--)
#else
	for (int_tmp = (PCLKCTRL_DIV_YYY_MAX+1) ; int_tmp ; int_tmp--)
#endif
	{
		u64_tmp = (u64)int_tmp * (u64)rate;
		if ((u64_tmp > PLL_MAX_RATE) || (u64_tmp < PLL_MIN_RATE))
			continue;
		if (u64_tmp > PCLKCTRL_MAX_FCKS)
			continue;
		nPLL.fpll = (unsigned int)u64_tmp;
		if (tcc_find_pms(&nPLL, XIN_CLK_RATE))
			continue;
		real_pll = nPLL.fpll;
		real_pclk = real_pll/int_tmp;

		if (rate > real_pclk)
			cmp_value = rate - real_pclk;
		else
			cmp_value = real_pclk - rate;

		if (div_err > cmp_value) {
			memcpy(&div_PLL, &nPLL, sizeof(tPMS));
			div_err = cmp_value;
		}

		if (!div_err)
			goto tcc_set_audio_pll_found;
	}

	/* dco mode */
#ifdef HDMIA_SRC_SHOULD_BE_SAME_WITH_AUDIO
	for (int_tmp = (PCLKCTRL_DIV_YYY_MAX+1)/4 ; int_tmp ; int_tmp--)
#else
	for (int_tmp = (PCLKCTRL_DIV_YYY_MAX+1)/2 ; int_tmp ; int_tmp--)
#endif
	{
		u64_tmp = (u64)(PCLKCTRL_DIV_YYY_MAX+1)*(u64)rate;
		do_div(u64_tmp, int_tmp);
		pll = (unsigned int)u64_tmp;
		if (pll > PLL_MAX_RATE)
			break;
		if (pll < PLL_MIN_RATE)
			continue;
		nPLL.fpll = pll;
		if (tcc_find_pms(&nPLL, XIN_CLK_RATE))
			continue;
		real_pll = nPLL.fpll;
		u64_tmp = (u64)real_pll*(u64)int_tmp;
		do_div(u64_tmp, PCLKCTRL_DIV_YYY_MAX+1);
		real_pclk = (unsigned int)u64_tmp;

		if (rate > real_pclk)
			cmp_value = rate - real_pclk;
		else
			cmp_value = real_pclk - rate;

		if (dco_err > cmp_value) {
			memcpy(&dco_PLL, &nPLL, sizeof(tPMS));
			dco_err = cmp_value;
		}

		if (!dco_err)
			break;
	}

	if ((dco_err == 0xFFFFFFFF) && (div_err == 0xFFFFFFFF))
		goto tcc_set_audio_pll_finish;

tcc_set_audio_pll_found:
	(dco_err < div_err) ? memcpy(&nPLL, &dco_PLL, sizeof(tPMS)) : memcpy(&nPLL, &div_PLL, sizeof(tPMS));

	//printk("\n@@@@ %s:, pclk:%d, pll:%d, p:%2d, m:%3d, s:%d @@@@\n\n", __func__, rate, nPLL.fpll, nPLL.p, nPLL.m, nPLL.s);
	tcc_pll_write(reg, nPLL.en, nPLL.p, nPLL.m, nPLL.s, PLLSRC_XIN);
	return nPLL.fpll;

tcc_set_audio_pll_finish:
	tcc_pll_write(reg, 0, PLL_P_MIN, (PLL_P_MIN*PLL_VCO_MIN+XIN_CLK_RATE)/XIN_CLK_RATE, PLL_S_MIN, PLLSRC_XIN);
	return 0;
}

static inline int tcc_find_audio_pclk(tPCLKCTRL *PCLKCTRL)
{
	unsigned int audio_pll, div_max;
	unsigned int err_dco = 0xFFFFFFFF;
	unsigned int err_div = 0xFFFFFFFF;
	unsigned int div_div = PCLKCTRL_DIV_MIN;
	unsigned int div_dco = PCLKCTRL_DIV_DCO_MIN;

#ifdef HDMIA_SRC_SHOULD_BE_SAME_WITH_AUDIO
	if (PCLKCTRL->periname == PERI_HDMIA) {
		// hdmi audio clock must not change the pll rate. (hdmi audio = dai*2 or spdif*2)
		if (stHDMIASrc == audio_pll_ch)
			audio_pll = tcc_ckc_pll_get_rate(audio_pll_ch);
		else
			return -1;
	}
	else
#endif
	{
		void __iomem *reg = ckc_base+CKC_PCLKCTRL+(PCLKCTRL->periname)*4;
		PCLKCTRL->en = (ckc_readl(reg) & (1<<PCLKCTRL_EN_SHIFT)) ? 1 : 0;
		tcc_pclkctrl_write(reg, PCLKCTRL_MODE_DIVIDER , PCLKCTRL->en, PCLKCTRL_SEL_XIN, 0, PCLKCTRL_TYPE_YYY);
		audio_pll = tcc_ckc_set_audio_pll(PCLKCTRL->freq);
	}
	div_max = PCLKCTRL_DIV_YYY_MAX;

	/* dco mode */
	err_dco = tcc_ckc_pclk_dco(PCLKCTRL, &div_dco, audio_pll, PCLKCTRL_DIV_DCO_MIN, div_max);

	/* divider mode */
	err_div = tcc_ckc_pclk_divider(PCLKCTRL, &div_div, audio_pll, PCLKCTRL_DIV_MIN+1, div_max+1);

	/* select mode/sel */
	PCLKCTRL->sel = (unsigned int)audio_pll_ch;
	if (err_dco < err_div) {
		u64 u64_tmp;
		PCLKCTRL->md = PCLKCTRL_MODE_DCO;
		PCLKCTRL->div = div_dco;
		if (PCLKCTRL->div > (div_max+1)/2)
			u64_tmp = (unsigned long long)audio_pll*(unsigned long long)((div_max+1)-PCLKCTRL->div);
		else
			u64_tmp = (unsigned long long)audio_pll*(unsigned long long)PCLKCTRL->div;
		do_div(u64_tmp,(div_max+1));
		PCLKCTRL->freq = (unsigned int)u64_tmp;
	}
	else {
		PCLKCTRL->md = PCLKCTRL_MODE_DIVIDER;
		PCLKCTRL->div = div_div ? div_div-1 : div_div;
		PCLKCTRL->freq = (audio_pll/(PCLKCTRL->div+1));
	}

	return 0;
}

static inline void tcc_ckc_reset_clock_source(int id)
{
	if (id >= MAX_CLK_SRC)
		return;

	/* check special pll channels */
	switch (id) {
	case CPU0_SRC_CH:
	case MEM_SRC_CH:
		stClockSource[id] = 0;
		stClockSource[MAX_TCC_PLL+id] = 0;
		return;		
	}

 	if (audio_pll_used && ((int)audio_pll_ch == id)) {
		stClockSource[id] = 0;
		stClockSource[MAX_TCC_PLL+id] = 0;
		return;
	}

	if (id < MAX_TCC_PLL) {
		stClockSource[id] = tcc_ckc_pll_get_rate(id);
		stClockSource[MAX_TCC_PLL+id] = tcc_ckc_plldiv_get_rate(id);
	}
}

static int tcc_ckc_iobus_pwdn(int id, bool pwdn)
{
	void __iomem *ckc_reg = ckc_base+CKC_CLKCTRL+(FBUS_IO*0x4);
	void __iomem *reg;

	if ((ckc_readl(ckc_reg) & (1<<CLKCTRL_EN_SHIFT)) == 0)
		return -2;

	if      (id < 32*1)
		reg = iobus_cfg_base+IOBUS_PWDN0;
	else if (id < 32*2)
		reg = iobus_cfg_base+IOBUS_PWDN1;
	else if (id < 32*3)
		reg = iobus_cfg_base+IOBUS_PWDN2;
	else
		return -1;

	id %= 32;

	if (pwdn)
		ckc_writel(ckc_readl(reg) & ~(1<<id), reg);
	else
		ckc_writel(ckc_readl(reg) | (1<<id), reg);

	return 0;
}

static int tcc_ckc_is_iobus_pwdn(int id)
{
	void __iomem *ckc_reg = ckc_base+CKC_CLKCTRL+(FBUS_IO*0x4);
	void __iomem *reg;

	if ((ckc_readl(ckc_reg) & (1<<CLKCTRL_EN_SHIFT)) == 0)
		return -2;

	if      (id < 32*1)
		reg = iobus_cfg_base+IOBUS_PWDN0;
	else if (id < 32*2)
		reg = iobus_cfg_base+IOBUS_PWDN1;
	else if (id < 32*3)
		reg = iobus_cfg_base+IOBUS_PWDN2;
	else
		return -1;

	id %= 32;

	return (ckc_readl(reg) & (1<<id)) ? 0 : 1;
}

static int tcc_ckc_iobus_swreset(int id, bool reset)
{
	void __iomem *ckc_reg = ckc_base+CKC_CLKCTRL+(FBUS_IO*0x4);
	void __iomem *reg;

	if ((ckc_readl(ckc_reg) & (1<<CLKCTRL_EN_SHIFT)) == 0)
		return -2;

	if      (id < 32*1)
		reg = iobus_cfg_base+IOBUS_RESET0;
	else if (id < 32*2)
		reg = iobus_cfg_base+IOBUS_RESET1;
	else if (id < 32*3)
		reg = iobus_cfg_base+IOBUS_RESET2;
	else
		return -1;

	id %= 32;

	if (reset)
		ckc_writel(ckc_readl(reg) & ~(1<<id), reg);
	else
		ckc_writel(ckc_readl(reg) | (1<<id), reg);

	return 0;
}

static int tcc_ckc_ddibus_pwdn(int id, bool pwdn)
{
	void __iomem *ckc_reg = ckc_base+CKC_CLKCTRL+(FBUS_DDI*0x4);
	void __iomem *reg = ddibus_cfg_base+DDIBUS_PWDN;

	if (id >= DDIBUS_MAX)
		return -1;

	if ((ckc_readl(ckc_reg) & (1<<CLKCTRL_EN_SHIFT)) == 0)
		return -2;

	if (pwdn)
		ckc_writel(ckc_readl(reg) & ~(1<<id), reg);
	else
		ckc_writel(ckc_readl(reg) | (1<<id), reg);

	return 0;
}

static int tcc_ckc_is_ddibus_pwdn(int id)
{
	void __iomem *ckc_reg = ckc_base+CKC_CLKCTRL+(FBUS_DDI*0x4);
	void __iomem *reg = ddibus_cfg_base+DDIBUS_PWDN;

	if (id >= DDIBUS_MAX)
		return -1;

	if ((ckc_readl(ckc_reg) & (1<<CLKCTRL_EN_SHIFT)) == 0)
		return -2;

	return (ckc_readl(reg) & (0x1 << id)) ? 0 : 1;
}

static int tcc_ckc_ddibus_swreset(int id, bool reset)
{
	void __iomem *ckc_reg = ckc_base+CKC_CLKCTRL+(FBUS_DDI*0x4);
	void __iomem *reg = ddibus_cfg_base+DDIBUS_RESET;

	if (id >= DDIBUS_MAX)
		return -1;

	if ((ckc_readl(ckc_reg) & (1<<CLKCTRL_EN_SHIFT)) == 0)
		return -2;

	if (reset)
		ckc_writel(ckc_readl(reg) & ~(1<<id), reg);
	else
		ckc_writel(ckc_readl(reg) | (1<<id), reg);

	return 0;
}

static int tcc_ckc_gpubus_pwdn(int id, bool pwdn)
{
	void __iomem *ckc_reg = ckc_base+CKC_CLKCTRL+(FBUS_GPU*0x4);
	void __iomem *reg = gpubus_cfg_base+GPUBUS_PWDN;

	if (id >= GPUBUS_MAX)
		return -1;

	if ((ckc_readl(ckc_reg) & (1<<CLKCTRL_EN_SHIFT)) == 0)
		return -2;

	if (pwdn)
		ckc_writel(ckc_readl(reg) & ~(1<<id), reg);
	else
		ckc_writel(ckc_readl(reg) | (1<<id), reg);

	return 0;
}

static int tcc_ckc_is_gpubus_pwdn(int id)
{
	void __iomem *ckc_reg = ckc_base+CKC_CLKCTRL+(FBUS_GPU*0x4);
	void __iomem *reg = gpubus_cfg_base+GPUBUS_PWDN;

	if (id >= GPUBUS_MAX)
		return -1;

	if ((ckc_readl(ckc_reg) & (1<<CLKCTRL_EN_SHIFT)) == 0)
		return -2;

	return (ckc_readl(reg) & (0x1 << id)) ? 0 : 1;
}

static int tcc_ckc_gpubus_swreset(int id, bool reset)
{
	void __iomem *ckc_reg = ckc_base+CKC_CLKCTRL+(FBUS_GPU*0x4);
	void __iomem *reg = gpubus_cfg_base+GPUBUS_RESET;

	if (id >= GPUBUS_MAX)
		return -1;

	if ((ckc_readl(ckc_reg) & (1<<CLKCTRL_EN_SHIFT)) == 0)
		return -2;

	if (reset)
		ckc_writel(ckc_readl(reg) & ~(1<<id), reg);
	else
		ckc_writel(ckc_readl(reg) | (1<<id), reg);

	return 0;
}

static int tcc_ckc_vpubus_pwdn(int id, bool pwdn)
{
	void __iomem *ckc_reg = ckc_base+CKC_CLKCTRL+(FBUS_VBUS*0x4);
	void __iomem *reg = vpubus_cfg_base+VPUBUS_PWDN;

	if (id >= VIDEOBUS_MAX)
		return -1;

	if ((ckc_readl(ckc_reg) & (1<<CLKCTRL_EN_SHIFT)) == 0)
		return -2;

	if (pwdn)
		ckc_writel(ckc_readl(reg) & ~(1<<id), reg);
	else
		ckc_writel(ckc_readl(reg) | (1<<id|1<<3), reg);	/* 3: Video Bus Matrix should be set */

	return 0;
}

static int tcc_ckc_is_vpubus_pwdn(int id)
{
	void __iomem *ckc_reg = ckc_base+CKC_CLKCTRL+(FBUS_VBUS*0x4);
	void __iomem *reg = vpubus_cfg_base+VPUBUS_PWDN;

	if (id >= VIDEOBUS_MAX)
		return -1;

	if ((ckc_readl(ckc_reg) & (1<<CLKCTRL_EN_SHIFT)) == 0)
		return -2;

	return (ckc_readl(reg) & (0x1 << id)) ? 0 : 1;
}

static int tcc_ckc_vpubus_swreset(int id, bool reset)
{
	void __iomem *ckc_reg = ckc_base+CKC_CLKCTRL+(FBUS_VBUS*0x4);
	void __iomem *reg = vpubus_cfg_base+VPUBUS_RESET;

	if (id >= VIDEOBUS_MAX)
		return -1;

	if ((ckc_readl(ckc_reg) & (1<<CLKCTRL_EN_SHIFT)) == 0)
		return -2;

	if (reset)
		ckc_writel(ckc_readl(reg) & ~(1<<id), reg);
	else
		ckc_writel(ckc_readl(reg) | (1<<id|1<<3), reg);	/* 3: Video Bus Matrix should be set */

	return 0;
}

static int tcc_ckc_hsiobus_pwdn(int id, bool pwdn)
{
	void __iomem *ckc_reg = ckc_base+CKC_CLKCTRL+(FBUS_HSIO*0x4);
	void __iomem *reg = hsiobus_cfg_base+HSIOBUS_PWDN;

	if (id >= HSIOBUS_MAX)
		return -1;

	if ((ckc_readl(ckc_reg) & (1<<CLKCTRL_EN_SHIFT)) == 0)
		return -2;

	if (pwdn)
		ckc_writel(ckc_readl(reg) & ~(1<<id), reg);
	else
		ckc_writel(ckc_readl(reg) | (1<<id), reg);

	return 0;
}

static int tcc_ckc_is_hsiobus_pwdn(int id)
{
	void __iomem *ckc_reg = ckc_base+CKC_CLKCTRL+(FBUS_HSIO*0x4);
	void __iomem *reg = hsiobus_cfg_base+HSIOBUS_PWDN;

	if (id >= HSIOBUS_MAX)
		return -1;

	if ((ckc_readl(ckc_reg) & (1<<CLKCTRL_EN_SHIFT)) == 0)
		return -2;

	return (ckc_readl(reg) & (0x1 << id)) ? 0 : 1;
}

static int tcc_ckc_hsiobus_swreset(int id, bool reset)
{
	void __iomem *ckc_reg = ckc_base+CKC_CLKCTRL+(FBUS_HSIO*0x4);
	void __iomem *reg = hsiobus_cfg_base+HSIOBUS_RESET;

	if (id >= HSIOBUS_MAX)
		return -1;

	if ((ckc_readl(ckc_reg) & (1<<CLKCTRL_EN_SHIFT)) == 0)
		return -2;

	if (reset)
		ckc_writel(ckc_readl(reg) & ~(1<<id), reg);
	else
		ckc_writel(ckc_readl(reg) | (1<<id), reg);

	return 0;
}


/**********************************
 *  PMU PWDN/SWRESET Control
 **********************************/
#define PMU_PWRSTS0		0x000
#define PMU_PWRSTS1		0x004
#define PMU_SYSRST		0x010
#define PMU_ISOL		0x09C
#define PMU_PWRDN_CMBUS		0x07C
#define PMU_PWRDN_VBUS		0x088
#define PMU_PWRDN_GBUS		0x08C
#define PMU_PWRDN_DBUS		0x090
#define PMU_PWRDN_HSBUS		0x094
#define PMU_PWRDN_CODA		0x0A0
#define PMU_PWRDN_HEVC		0x0A4
#define PMU_PWRDN_SHADER0	0x0A8
#define PMU_PWRDN_SHADER1	0x0AC
#define PMU_PWRUP_CMBUS		0x0E8
#define PMU_PWRUP_VBUS		0x0F4
#define PMU_PWRUP_GBUS		0x0F8
#define PMU_PWRUP_DBUS		0x0FC
#define PMU_PWRUP_HSBUS		0x100
#define PMU_PWRUP_CODA		0x104
#define PMU_PWRUP_HEVC		0x108
#define PMU_PWRUP_SHADER0	0x10C
#define PMU_PWRUP_SHADER1	0x110

#define MBUS_HCLK		0x000
#define MBUS_SWRESET		0x004
#define MBUS_STS		0x014
#define MBUS_ARQOS0		0x040

static int tcc_ckc_ip_pwdn(int id, bool pwdn)
{
	void __iomem *reg = pmu_base+PMU_ISOL;
	unsigned value;

	switch(id) {
	case PMU_ISOL_OTP:
	case PMU_ISOL_RTC:
	case PMU_ISOL_PLL:
	case PMU_ISOL_ECID:
	case PMU_ISOL_HDMI:
	case PMU_ISOL_VDAC_3CH:
	case PMU_ISOL_VDAC_1CH:
	case PMU_ISOL_TSADC:
	case PMU_ISOL_USB2P:
	case PMU_ISOL_USBOTG:
	case PMU_ISOL_LVDS:
		value = ckc_readl(reg);
		if (pwdn)
			value |= (1<<id);
		else
			value &= ~(1<<id);
		ckc_writel(value, reg);
		break;
	default:
		return -1;
	}

	return 0;
}

static int tcc_ckc_is_ip_pwdn(int id)
{
	void __iomem *reg = pmu_base+PMU_ISOL;

	switch(id) {
	case PMU_ISOL_OTP:
	case PMU_ISOL_RTC:
	case PMU_ISOL_PLL:
	case PMU_ISOL_ECID:
	case PMU_ISOL_HDMI:
	case PMU_ISOL_VDAC_3CH:
	case PMU_ISOL_VDAC_1CH:
	case PMU_ISOL_TSADC:
	case PMU_ISOL_USB2P:
	case PMU_ISOL_USBOTG:
	case PMU_ISOL_LVDS:
		return (ckc_readl(reg) & (1<<id)) ? 1 : 0;
	}

	return -1;
}

static int tcc_ckc_pmu_pwdn(int id, bool pwdn)
{
	void __iomem *ckcrst_reg = ckc_base + CKC_SWRESET;
	void __iomem *pwrsts_reg = NULL, *pwrsts1_reg = NULL, *pwrsts2_reg = NULL;
	void __iomem *pwrupdn_reg = NULL, *pwrupdn1_reg = NULL, *pwrupdn2_reg = NULL;
	void __iomem *memhclk_reg = membus_cfg_base + MBUS_HCLK;
	void __iomem *memrst_reg = membus_cfg_base + MBUS_SWRESET;
	void __iomem *memsts_reg = membus_cfg_base + MBUS_STS;
	unsigned ckcrst_mask = 0, pwrsts_mask=0, pwrsts1_mask=0, pwrsts2_mask=0;
	unsigned memhclk_mask=0, memrst_mask=0, memsts_mask=0;

	switch(id) {
	case FBUS_CPU0:
	case FBUS_CPU1:
	case FBUS_MEM:
		return -1;
	case FBUS_DDI:
		ckcrst_mask = 1<<10|1<<3;
		pwrsts_reg = pmu_base + PMU_PWRSTS0;
		if (pwdn) {
			pwrsts_mask = 1<<3;
			pwrupdn_reg = pmu_base + PMU_PWRDN_DBUS;
		}
		else {
			pwrsts_mask = 1<<2;
			pwrupdn_reg = pmu_base + PMU_PWRUP_DBUS;
		}
		memhclk_mask = 1<<27|1<<26;
		memrst_mask = 1<<16|1<<9;
		memsts_mask = 1<<25|1<<23|1<<19|1<<9|1<<7|1<<3;
		break;
	case FBUS_GPU:
		ckcrst_mask = 1<<4;
		pwrsts_reg = pmu_base + PMU_PWRSTS0;
		pwrsts1_reg = pmu_base + PMU_PWRSTS1;
		pwrsts2_reg = pmu_base + PMU_PWRSTS1;
		if (pwdn) {
			pwrsts_mask = 1<<5;
			pwrupdn_reg = pmu_base + PMU_PWRDN_GBUS;
			pwrsts1_mask = 1<<3;
			pwrupdn1_reg = pmu_base + PMU_PWRDN_SHADER0;
			pwrsts2_mask = 1<<5;
			pwrupdn2_reg = pmu_base + PMU_PWRDN_SHADER1;
		}
		else {
			pwrsts_mask = 1<<4;
			pwrupdn_reg = pmu_base + PMU_PWRUP_GBUS;
			pwrsts1_mask = 1<<2;
			pwrupdn1_reg = pmu_base + PMU_PWRUP_SHADER0;
			pwrsts2_mask = 1<<4;
			pwrupdn2_reg = pmu_base + PMU_PWRUP_SHADER1;
		}
		memhclk_mask = 1<<24;
		memrst_mask = 1<<10;
		memsts_mask = (1<<22)|(1<<6);
		break;
	case FBUS_IO:
		return -1;
	case FBUS_VBUS:
		ckcrst_mask = 1<<6;
		pwrsts_reg = pmu_base + PMU_PWRSTS1;
		if (pwdn) {
			pwrsts_mask = 1<<11;
			pwrupdn_reg = pmu_base + PMU_PWRDN_VBUS;
		}
		else {
			pwrsts_mask = 1<<10;
			pwrupdn_reg = pmu_base + PMU_PWRUP_VBUS;
		}
		memhclk_mask = 1<<25;
		memrst_mask = 1<<11;
		memsts_mask = (1<<24)|(1<<21)|(1<<8)|(1<<5);
		break;
	case FBUS_CODA:
		ckcrst_mask = 1<<7;
		pwrsts_reg = pmu_base + PMU_PWRSTS1;
		if (pwdn) {
			pwrsts_mask = 1<<9;
			pwrupdn_reg = pmu_base + PMU_PWRDN_CODA;
		}
		else {
			pwrsts_mask = 1<<8;
			pwrupdn_reg = pmu_base + PMU_PWRUP_CODA;
		}
		memhclk_reg = NULL; /* not controlled */
		memsts_reg = NULL;
		break;
	case FBUS_HSIO:
		ckcrst_mask = 1<<8;
		pwrsts_reg = pmu_base + PMU_PWRSTS0;
		if (pwdn) {
			pwrsts_mask = 1<<1;
			pwrupdn_reg = pmu_base + PMU_PWRDN_HSBUS;
		}
		else {
			pwrsts_mask = 1<<0;
			pwrupdn_reg = pmu_base + PMU_PWRUP_HSBUS;
		}
		memhclk_mask = 1<<29;
		memrst_mask = 1<<12;
		memsts_mask = (1<<20)|(1<<4);
		break;
	case FBUS_SMU:
	case FBUS_G2D:
		return -1;
	case FBUS_CMBUS:
		#if defined(TSBM_ENABLE) || defined(CONFIG_ARM_TRUSTZONE) || defined(TZOW_INCLUDE) || defined(TCC_NSK_ENABLE)
		return -1;
		#endif
		ckcrst_mask = 1<<11;
		pwrsts_reg = pmu_base + PMU_PWRSTS1;
/* Patch for unintended value write on the OTPROM
		if (pwdn) {
			pwrsts_mask = 1<<1;
			pwrupdn_reg = pmu_base + PMU_PWRDN_CMBUS;
		}
		else {
			pwrsts_mask = 1<<0;
			pwrupdn_reg = pmu_base + PMU_PWRUP_CMBUS;
		}
*/
		memhclk_mask = 1<<28;
		memrst_mask = 1<<3;
		memsts_reg = NULL;
		break;
	case FBUS_HEVC_VCE:
//	case FBUS_HEVC_VCPU:
//	case FBUS_HEVC_BPU:
		ckcrst_mask = 1<<14|1<<13|1<<12;
		pwrsts_reg = pmu_base + PMU_PWRSTS1;
		if (pwdn) {
			pwrsts_mask = 1<<7;
			pwrupdn_reg = pmu_base + PMU_PWRDN_HEVC;
		}
		else {
			pwrsts_mask = 1<<6;
			pwrupdn_reg = pmu_base + PMU_PWRUP_HEVC;
		}
		memhclk_reg = NULL; /* not controlled */
		break;
	default:
		return -1;
	}

	/* check pmu_pwrsts0 main_state. 0x0 is idle */
	while(ckc_readl(pmu_base+PMU_PWRSTS0)&(0x3F<<25));

	if (pwdn) {
		if (memhclk_mask)
			ckc_writel(ckc_readl(memhclk_reg) & ~(memhclk_mask), memhclk_reg);
		if (memsts_mask)
			while (ckc_readl(memsts_reg)&memsts_mask);
		if (memrst_mask)
			ckc_writel(ckc_readl(memrst_reg) & ~(memrst_mask), memrst_reg);
		if (ckcrst_mask)
			ckc_writel(ckc_readl(ckcrst_reg) & ~(ckcrst_mask), ckcrst_reg);
		if (pwrupdn2_reg) {
			ckc_writel(1, pwrupdn2_reg);
			while((ckc_readl(pwrsts2_reg)&pwrsts2_mask) == 0);
		}
		if (pwrupdn1_reg) {
			ckc_writel(1, pwrupdn1_reg);
			while((ckc_readl(pwrsts1_reg)&pwrsts1_mask) == 0);
		}
		if (pwrupdn_reg) {
			ckc_writel(1, pwrupdn_reg);
			while((ckc_readl(pwrsts_reg)&pwrsts_mask) == 0);
		}
	}
	else {
		if (pwrupdn_reg) {
			ckc_writel(1, pwrupdn_reg);
			while((ckc_readl(pwrsts_reg)&pwrsts_mask) == 0);
		}
		if (pwrupdn1_reg) {
			ckc_writel(1, pwrupdn1_reg);
			while((ckc_readl(pwrsts1_reg)&pwrsts1_mask) == 0);
		}
		if (pwrupdn2_reg) {
			ckc_writel(1, pwrupdn2_reg);
			while((ckc_readl(pwrsts2_reg)&pwrsts2_mask) == 0);
		}
		if (ckcrst_mask)
			ckc_writel(ckc_readl(ckcrst_reg)|(ckcrst_mask), ckcrst_reg);
		if (memrst_mask)
			ckc_writel(ckc_readl(memrst_reg)|(memrst_mask), memrst_reg);
		if (memhclk_mask)
			ckc_writel(ckc_readl(memhclk_reg)|memhclk_mask, memhclk_reg);
	}

	return 0;
}

static int tcc_ckc_is_pmu_pwdn(int id)
{
	void __iomem *pwrsts_reg = NULL;
	unsigned pwrsts_mask = 0;

	switch (id) {
	case FBUS_CPU0:
		pwrsts_reg = pmu_base + PMU_PWRSTS1;
		pwrsts_mask = 1<<20;
		break;
	case FBUS_CPU1:
		pwrsts_reg = pmu_base + PMU_PWRSTS1;
		pwrsts_mask = 1<<12;
		break;
	case FBUS_MEM:
		pwrsts_reg = pmu_base + PMU_PWRSTS0;
		pwrsts_mask = (1<<8)|(1<<6);
		break;
	case FBUS_DDI:
		pwrsts_reg = pmu_base + PMU_PWRSTS0;
		pwrsts_mask = 1<<2;
		break;
	case FBUS_GPU:
		pwrsts_reg = pmu_base + PMU_PWRSTS0;
		pwrsts_mask = 1<<4;
		break;
	case FBUS_IO:
		break;
	case FBUS_VBUS:
		pwrsts_reg = pmu_base + PMU_PWRSTS1;
		pwrsts_mask = 1<<10;
		break;
	case FBUS_CODA:
		pwrsts_reg = pmu_base + PMU_PWRSTS1;
		pwrsts_mask = 1<<8;
		break;
	case FBUS_HSIO:
		pwrsts_reg = pmu_base + PMU_PWRSTS0;
		pwrsts_mask = 1<<0;
		break;
	case FBUS_SMU:
	case FBUS_G2D:
		return -1;
	case FBUS_CMBUS:
		pwrsts_reg = pmu_base + PMU_PWRSTS1;
		pwrsts_mask = 1<<0;
		break;
	case FBUS_HEVC_VCE:
	case FBUS_HEVC_VCPU:
	case FBUS_HEVC_BPU:
		pwrsts_reg = pmu_base + PMU_PWRSTS1;
		pwrsts_mask = 1<<6;
		break;
	default:
		return -1;
	}

	return (ckc_readl(pwrsts_reg)&pwrsts_mask) ? 0 : 1;
}


/**********************************
 *  MISC. Functions
 **********************************/

int tcc_ckc_set_hdmi_audio_src(unsigned int src_id)
{
#ifdef HDMIA_SRC_SHOULD_BE_SAME_WITH_AUDIO
	volatile unsigned *pPCLKCTRL = (volatile unsigned *)(ckc_base+CKC_PCLKCTRL)+src_id;
	unsigned int sel;

	if (src_id != PERI_ADAI0 && src_id != PERI_SPDIF0 &&
	    src_id != PERI_ADAI1 && src_id != PERI_SPDIF1 &&
	    src_id != PERI_ADAI2 && src_id != PERI_SPDIF2 &&
	    src_id != PERI_ADAI3 && src_id != PERI_SPDIF3) {
		stHDMIASrc = 0xFFFFFFFF;
		return -1;
	}

	sel = (*(volatile unsigned *)pPCLKCTRL&(PCLKCTRL_SEL_MASK<<PCLKCTRL_SEL_SHIFT))>>PCLKCTRL_SEL_SHIFT;
	switch (sel) {
		case PCLKCTRL_SEL_PLL0:
		case PCLKCTRL_SEL_PLL1:
		case PCLKCTRL_SEL_PLL2:
			stHDMIASrc = sel;
			break;
		case PCLKCTRL_SEL_XIN:
		case PCLKCTRL_SEL_XTIN:
			stHDMIASrc = sel + MAX_TCC_PLL;
			break;
		case PCLKCTRL_SEL_PLL0DIV:
		case PCLKCTRL_SEL_PLL1DIV:
		case PCLKCTRL_SEL_PLL2DIV:
			stHDMIASrc = sel;
			break;
		default:
			stHDMIASrc = 0xFFFFFFFF;
	}

	return 0;
#else
	return -1;
#endif
}

/*
 * This functions called by nand driver.
 * Return value unit is 100Hz.
 */
unsigned long tca_ckc_get_nand_iobus_clk(void)
{
	return tcc_ckc_clkctrl_get_rate(FBUS_IO)/100;
}


static struct tcc_ckc_ops tcc897x_ops = {
	.ckc_pmu_pwdn			= &tcc_ckc_pmu_pwdn,
	.ckc_is_pmu_pwdn		= &tcc_ckc_is_pmu_pwdn,
	.ckc_swreset			= NULL,
	.ckc_ip_pwdn			= &tcc_ckc_ip_pwdn,
	.ckc_is_ip_pwdn			= &tcc_ckc_is_ip_pwdn,
	.ckc_pll_set_rate		= &tcc_ckc_pll_set_rate,
	.ckc_pll_get_rate		= &tcc_ckc_pll_get_rate,
	.ckc_plldiv_set			= &tcc_ckc_pll_div_set,
	.ckc_clkctrl_enable		= &tcc_ckc_clkctrl_enable,
	.ckc_clkctrl_disable		= &tcc_ckc_clkctrl_disable,
	.ckc_clkctrl_set_rate		= &tcc_ckc_clkctrl_set_rate,
	.ckc_clkctrl_get_rate		= &tcc_ckc_clkctrl_get_rate,
	.ckc_is_clkctrl_enabled		= &tcc_ckc_is_clkctrl_enabled,
	.ckc_peri_enable		= &tcc_ckc_peri_enable,
	.ckc_peri_disable		= &tcc_ckc_peri_disable,
	.ckc_peri_set_rate		= &tcc_ckc_peri_set_rate,
	.ckc_peri_get_rate		= &tcc_ckc_peri_get_rate,
	.ckc_is_peri_enabled		= &tcc_ckc_is_peri_enabled,
	.ckc_ddibus_pwdn		= &tcc_ckc_ddibus_pwdn,
	.ckc_is_ddibus_pwdn		= &tcc_ckc_is_ddibus_pwdn,
	.ckc_ddibus_swreset		= &tcc_ckc_ddibus_swreset,
	.ckc_gpubus_pwdn		= &tcc_ckc_gpubus_pwdn,
	.ckc_is_gpubus_pwdn		= &tcc_ckc_is_gpubus_pwdn,
	.ckc_gpubus_swreset		= &tcc_ckc_gpubus_swreset,
	.ckc_iobus_pwdn			= &tcc_ckc_iobus_pwdn,
	.ckc_is_iobus_pwdn		= &tcc_ckc_is_iobus_pwdn,
	.ckc_iobus_swreset		= &tcc_ckc_iobus_swreset,
	.ckc_vpubus_pwdn		= &tcc_ckc_vpubus_pwdn,
	.ckc_is_vpubus_pwdn		= &tcc_ckc_is_vpubus_pwdn,
	.ckc_vpubus_swreset		= &tcc_ckc_vpubus_swreset,
	.ckc_hsiobus_pwdn		= &tcc_ckc_hsiobus_pwdn,
	.ckc_is_hsiobus_pwdn		= &tcc_ckc_is_hsiobus_pwdn,
	.ckc_hsiobus_swreset		= &tcc_ckc_hsiobus_swreset,
	.ckc_g2dbus_pwdn		= NULL,
	.ckc_is_g2dbus_pwdn		= NULL,
	.ckc_g2dbus_swreset		= NULL,
	.ckc_cmbus_pwdn			= NULL,
	.ckc_is_cmbus_pwdn		= NULL,
	.ckc_cmbus_swreset		= NULL,
};


static int initialized = 0;
void tcc_ckc_init(void)
{
	int i;

	if (initialized)
		return;
	initialized = 1;

	ckc_base = (void __iomem *)HwCKC_BASE;
	pmu_base = (void __iomem *)HwPMU_BASE;
	membus_cfg_base = (void __iomem *)HwMBUSCFG_BASE;
	ddibus_cfg_base = (void __iomem *)HwDDI_CONFIG_BASE;
	gpubus_cfg_base = (void __iomem *)HwGRPBUSCONFIG_BASE;
	iobus_cfg_base = (void __iomem *)HwIOBUSCFG_BASE;
	vpubus_cfg_base = (void __iomem *)HwVIDEOBUSCONFIG_BASE;
	hsiobus_cfg_base = (void __iomem *)HwHSIOBUSCFG_BASE;

	/* PMU_IPISOL */
	ckc_writel(0x0, pmu_base+PMU_ISOL);

	/* Disable PLL Divider */
	ckc_writel(0x01010101, ckc_base+CKC_CLKDIVC);		/* PLL0,PLL1,PLL2 */
	ckc_writel(0x01010101, ckc_base+CKC_CLKDIVC+0x4);	/* XIN,XTIN */

	for (i=0 ; i<MAX_TCC_PLL*2 ; i++)
		stClockSource[i] = 0;

	/* read memory pll rate for use clock source */
	tcc_ckc_reset_clock_source(MEM_SRC_CH);

	stClockSource[MAX_TCC_PLL*2] = XIN_CLK_RATE;	// XIN
	stClockSource[MAX_TCC_PLL*2+1] = XTIN_CLK_RATE;	// XTIN

	/* disable all peri clocks*/
	for (i=0 ; i<PERI_MAX ; i++)
		tcc_pclkctrl_write((ckc_base + CKC_PCLKCTRL + (i*0x4)), 0, 0, PCLKCTRL_SEL_XIN, 0, 0);

	tcc_ckc_set_ops(&tcc897x_ops);	

	/* set arm arch timer */
	tcc_set_peri(PERI_CMBUS_CNT, ENABLE, XIN_CLK_RATE);

	/* set clkctrl */
	for (i=0 ; i<FBUS_MAX ; i++) {
		if (i == FBUS_CPU0 || i == FBUS_MEM)
			continue;
		else if (i == FBUS_IO || i == FBUS_SMU)
			tcc_set_clkctrl(i, ENABLE, XIN_CLK_RATE/2);
		else
			tcc_set_clkctrl(i, DISABLE, XIN_CLK_RATE/2);
	}

	ckc_writel(0x0ff00000, membus_cfg_base+MBUS_ARQOS0);		/* qos setting */
}

