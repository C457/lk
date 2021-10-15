#ifndef __VIOC_CPUIF_H__
#define	__VIOC_CPUIF_H__
#include <platform/reg_physical.h>

////////////////////////////////////////////////////////////////////////////////
//
//	OUTPUT CONFIGURATION
//
#define		VIOC_OUTCFG_HDMI			0
#define		VIOC_OUTCFG_SDVENC		1
#define		VIOC_OUTCFG_HDVENC		2
#define		VIOC_OUTCFG_M80			3
#define		VIOC_OUTCFG_MRGB			4

#define		VIOC_OUTCFG_DISP0			0
#define		VIOC_OUTCFG_DISP1			1
#define		VIOC_OUTCFG_DISP2			2


typedef	struct {
	unsigned				FMT			:  4;
	unsigned				reserved0	:  1;
	unsigned				XA			:  2;
	unsigned				CS			:  1;
	unsigned				SI			:  1;
	unsigned				EN			:  1;
	unsigned				reserved1	:  4;
	unsigned				IV			:  1;
	unsigned				ID			:  1;
	unsigned				reserved2	: 16;
}	VIOC_OUTCFG_CPUIF;

typedef	union {
	unsigned	long			nREG;
	VIOC_OUTCFG_CPUIF	bREG;
}	VIOC_OUTCFG_CPUIF_u;

typedef	struct {
	unsigned				HDMISEL	:  2;	// HDMI OUTPUT
	unsigned 				reserved0	:  2;
	unsigned				SDVESEL	:  2;	// SDVENC OUTPUT
	unsigned 				reserved1	:  2;
	unsigned				HDVESEL	:  2;	// HDVENC OUTPUT
	unsigned 				reserved2	:  2;
	unsigned				M80SEL		:  2;	// MIPI I80 OUTPUT
	unsigned 				reserved3	:  2;
	unsigned				MRGBSEL 	:  2;	// MIPI RGB OUTPUT
	unsigned 				reserved4	:  2;
	unsigned 				reserved5	: 12;
}	VIOC_OUTCFG_MISC;

typedef	union {
	unsigned long		nREG;
	VIOC_OUTCFG_MISC	bREG;
}	VIOC_OUTCFG_MISC_u;

typedef	struct	{
	volatile VIOC_OUTCFG_CPUIF_u		uD0CPUIF;		//0x00
	volatile VIOC_OUTCFG_CPUIF_u		uD1CPUIF;		//0x04
	volatile VIOC_OUTCFG_CPUIF_u		uD2CPUIF;		//0x08
	volatile VIOC_OUTCFG_MISC_u			uMISCCFG;		//0x10
}	VIOC_OUTCFG;

extern void VIOC_OUTCFG_SetOutConfig (unsigned  nType, unsigned nDisp);

#endif
