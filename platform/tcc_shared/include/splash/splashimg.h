#ifndef __SPLASHIMG_H__
#define __SPLASHIMG_H__

#include <dev/fbcon.h>

#define	IMAGE_SIZE_MAX		30
#define SPLASH_TAG		"splash"
#define SPLASH_TAG_SIZE		6

#define	SPLASH_IMAGE_WIDTH	800
#define	SPLASH_IMAGE_HEIGHT	480

#define DEFAULT_HEADER_SIZE	512
#define DEFAULT_PAGE_SIZE	512


typedef struct SPLASH_IMAGE_INFO{
	 char		ucImageName[20];
	unsigned int 		ulImageAddr;
	unsigned int		ulImageSize;
	unsigned int		ulImageWidth;
	unsigned int		ulImageHeight;
	unsigned int 		Padding;
	unsigned char		ucFmt[5];
	unsigned char		ucRev[7];
}SPLASH_IMAGE_INFO_t;

typedef struct SPLASH_IMAGE_Header_info{
	unsigned char 	ucPartition[8];
	int		                  ulNumber;	
	unsigned char 		ucRev[4];
	SPLASH_IMAGE_INFO_t	SPLASH_IMAGE[IMAGE_SIZE_MAX];
	
}SPLASH_IMAGE_Header_info_t;

typedef struct SPLASH_BUFFER {
	unsigned char *data;
	unsigned int size;
}SPLASH_BUFFER_t;

int splash_image_load(char *partition, struct fbcon_config *fb_cfg);

#endif

