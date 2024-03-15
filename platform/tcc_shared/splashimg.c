
#include <debug.h>
#include <string.h>
#include <dev/flash.h>
#include <lib/ptable.h>
#include <dev/fbcon.h>
#include <splash/splashimg.h>
#include <tcc_lcd.h>
#include <partition_parser.h>
#include <sfl.h>
#include <platform/reg_physical.h>

#define PAGE_MASK(page_size)    ((page_size) - 1)
#define ROUND_TO_PAGE(x,y)      (((x) + (PAGE_MASK(y))) & (~(PAGE_MASK(y))))

#define PAGE_SIZE               512
#define SPLASH_HDR_SIZE         ROUND_TO_PAGE(sizeof(SPLASH_IMAGE_Header_info_t), PAGE_SIZE)

unsigned char *splash[SPLASH_HDR_SIZE];

#include <TCC_JPU_C6.h>

int jpu_c6_decoder( jpu_dec_inits_t* init_jpu, jpu_dec_outputs_t* out_jpu)
{
       unsigned int ret = 0;
       int dec_handle = 0;

       jpu_dec_init_t dec_init = {0,};
       jpu_dec_initial_info_t dec_initial_info = {0,};
       jpu_dec_input_t dec_input = {0,};
       jpu_dec_output_t dec_output = {0,};
       jpu_dec_buffer_t dec_buffer = { 0 };
       unsigned int frame_buf_addr[2] = {0,};
       unsigned int bitstream_buf_addr[2] = {0,};

       dec_init.m_iBitstreamBufSize = 0x100000;
       dec_init.m_RegBaseVirtualAddr = init_jpu->register_base_virtual_address;
       dec_init.m_BitstreamBufAddr[PHYSICAL_ADDR] = init_jpu->memory_base_physical_address;
       dec_init.m_BitstreamBufAddr[VIRTUAL_ADDR] = init_jpu->memory_base_virtual_address;
       dec_init.m_iCbCrInterleaveMode = 0;
       dec_init.m_uiDecOptFlags = 0;

       dec_init.m_Memcpy        = init_jpu->m_Memcpy;
       dec_init.m_Memset        = init_jpu->m_Memset;
       dec_init.m_Interrupt = init_jpu->m_Interrupt;

       ret = TCC_JPU_DEC( JPU_DEC_INIT, &dec_handle, &dec_init, NULL );
       if( ret != JPG_RET_SUCCESS )
       {
               dprintf(INFO, "[dec_test] JPU_DEC_INIT failed Error code is 0x%x \r\n", ret );
//             dprintf(INFO, "[dec_test] JPU_DEC_INIT failed detail errod  code is 0x%x \r\n", dec_initial_info.m_error_reason);
               goto ERR_DEC_OPEN;
       }
       //PRINTF( "[dec_test] YUV format :  %d \r\n", dec_initial_info.m_iJpg_sourceFormat );
       //PRINTF("TCC_JPU_DEC JPU_DEC_INIT OK!\r\n");

       ret = TCC_JPU_DEC( JPU_DEC_SEQ_HEADER, &dec_handle, (void*)init_jpu->input_stream_size, &dec_initial_info );
       if( ret != JPG_RET_SUCCESS )
       {
               dprintf(INFO, "[dec_test] JPU_DEC_SEQ_HEADER failed Error code is 0x%x, error reason is 0x%x \r\n", ret, dec_initial_info.m_iErrorReason );
               goto ERR_DEC_OPEN;
       }

       out_jpu->m_iYUVFormat = dec_initial_info.m_iSourceFormat;

       frame_buf_addr[PHYSICAL_ADDR] = init_jpu->memory_base_physical_address + 0x100000;
       if( frame_buf_addr[PHYSICAL_ADDR] == 0 )
       {
               dprintf(INFO, "frame_buf_addr[PA] malloc() failed \r\n");
               goto ERR_DEC_OPEN;
       }
       //PRINTF("frame_buf_addr[PA] = 0x%x\r\n", (codec_addr_t)frame_buf_addr[PA]);

       frame_buf_addr[VIRTUAL_ADDR] = init_jpu->memory_base_virtual_address+0x100000;
       if( frame_buf_addr[VIRTUAL_ADDR] == 0 )
       {
               dprintf(INFO, "frame_buf_addr[VIRTUAL_ADDR] malloc() failed \r\n");
               goto ERR_DEC_OPEN;
       }
       //PRINTF("frame_buf_addr[VIRTUAL_ADDR] = 0x%x\r\n", (codec_addr_t)frame_buf_addr[VIRTUAL_ADDR]);

       dec_buffer.m_FrameBufferStartAddr[PHYSICAL_ADDR] = frame_buf_addr[PHYSICAL_ADDR];
       dec_buffer.m_FrameBufferStartAddr[VIRTUAL_ADDR] = frame_buf_addr[VIRTUAL_ADDR];
       dec_buffer.m_iFrameBufferCount = dec_initial_info.m_iMinFrameBufferCount;
       dec_buffer.m_iJPGScaleRatio = 0;

       ret = TCC_JPU_DEC( JPU_DEC_REG_FRAME_BUFFER, &dec_handle, &dec_buffer, 0 );
       if( ret != JPG_RET_SUCCESS )
       {
               dprintf(INFO, "JPU_DEC_REG_FRAME_BUFFER failed Error code is 0x%x \r\n\n", ret);
               goto ERR_DEC_OPEN;
       }
       //PRINTF("TCC_JPU_DEC JPU_DEC_REG_FRAME_BUFFER OK!\r\n");


       dec_input.m_BitstreamDataAddr[PHYSICAL_ADDR] = init_jpu->memory_base_physical_address;
       dec_input.m_BitstreamDataAddr[VIRTUAL_ADDR] = init_jpu->memory_base_virtual_address;
       dec_input.m_iBitstreamDataSize = init_jpu->input_stream_size;

       ret = TCC_JPU_DEC( JPU_DEC_DECODE, &dec_handle, &dec_input, &dec_output );
       if(ret ==JPG_RET_FAILURE) {
               goto ERR_DEC_OPEN;
       }
       if(ret ==JPG_RET_CODEC_EXIT){
               goto ERR_DEC_OPEN;
       }

       out_jpu->m_iWidth = dec_output.m_DecOutInfo.m_iWidth;
       out_jpu->m_iHeight = dec_output.m_DecOutInfo.m_iHeight;
       out_jpu->m_pCurrOut[0][0] = dec_output.m_pCurrOut[0][0];
       out_jpu->m_pCurrOut[0][1] = dec_output.m_pCurrOut[0][1];
       out_jpu->m_pCurrOut[0][2] = dec_output.m_pCurrOut[0][2];
       out_jpu->m_pCurrOut[1][0] = dec_output.m_pCurrOut[1][0];
       out_jpu->m_pCurrOut[1][1] = dec_output.m_pCurrOut[1][1];
       out_jpu->m_pCurrOut[1][2] = dec_output.m_pCurrOut[1][2];

ERR_DEC_OPEN:
       // Now that we are done with decoding, close the open instance.
       ret = TCC_JPU_DEC( JPU_DEC_CLOSE, &dec_handle, 0, 0 );

       return ret;
}

int splash_image_decode_jpeg(unsigned int input_stream_size_,
		jpu_dec_outputs_t * jpu_out,
		unsigned int decode_addr) {

       int ret = 0;

       jpu_dec_inits_t jpu_init = {0,};
//     jpu_dec_outputs_t jpu_out = {0,};
       unsigned int register_base_physical_address = 0x75180000;
       unsigned int register_base_virtual_address = 0x75180000;
       unsigned int memory_base_physical_address = decode_addr;// 0x8cc00000;
       unsigned int memory_base_virtual_address = decode_addr;// 0x8cc00000;
       int input_stream_size = (int)input_stream_size_;        // decoding CO jpeg stream size

//     dprintf(INFO, "==== %s() in \n", __func__);

       memset(jpu_out, 0, sizeof(jpu_dec_outputs_t));

       jpu_init.input_stream_size = input_stream_size;
       jpu_init.register_base_virtual_address = register_base_virtual_address;
       jpu_init.memory_base_physical_address = memory_base_physical_address;
       jpu_init.memory_base_virtual_address = memory_base_virtual_address;
       jpu_init.m_Memset = memset;                             // (void  (*) ( void*, int, unsigned int ))sys_memset;
       jpu_init.m_Memcpy = memcpy;                             // (void* (*) ( void*, const void*, unsigned int ))sys_memcpy;
       jpu_init.m_Interrupt = NULL;                    // (int  (*) ( void ))sys_test;

#if 1
	dprintf(INFO, "==== register_base_virtual_address(0x%x) \n==== memory_base_physical_address(0x%x) \n", \
                       jpu_init.register_base_virtual_address, \
                       jpu_init.memory_base_physical_address);
#endif

       ret = jpu_c6_decoder(&jpu_init, jpu_out);
       if ( ret != 0 )
               dprintf(INFO, "jpu error : %d\n", ret);

#if 1
	dprintf(INFO, "==== width x height(%dx%d), YUV format(%d), m_pCurrOut(0x%x) \n", \
                               jpu_out->m_iWidth, jpu_out->m_iHeight, \
                               jpu_out->m_iYUVFormat, \
                               jpu_out->m_pCurrOut[0][0]);
#endif
       return ret;

}

static int get_splash_index(SPLASH_IMAGE_Header_info_t *splash_hdr, char *part){
	unsigned int idx = 0;

	for (idx = 0; idx < splash_hdr->ulNumber; idx++) {

		dprintf(SPEW, "part name : %s idx : %d\n", splash_hdr->SPLASH_IMAGE[idx].ucImageName, idx);

		if (!strcmp((splash_hdr->SPLASH_IMAGE[idx].ucImageName), part)) {
			return idx;
		}
	}

	return -1;
}

int splash_image_load(char *partition, struct fbcon_config *fb_cfg) {
        SPLASH_IMAGE_Header_info_t * splash_hdr = splash;
	int img_idx = 0;
	unsigned long long ptn = 0;
	unsigned int ptn_index;
	unsigned int img_fmt = 0;       // 0 = BMP, 1 = JPEG
	const char* ptn_name = "splash";
        int ret = 0;

	jpu_dec_outputs_t jpu_out = {0,};

	unsigned int decode_addr = JPEG_DECODE_ADDR
		+ ((fb_cfg->width * fb_cfg->height) * (fb_cfg->bpp/8));

	//dprintf(INFO, "==== %s in\n", __func__);

	ptn_index = partition_get_index(ptn_name);
	ptn = partition_get_offset(ptn_index);

	if(ptn == 0) {
		dprintf(CRITICAL, "ERROR : No splash partition found !\n");
                ret = -1;
                goto err;
	}
	else{
                if(tcc_read(ptn, splash_hdr, SPLASH_HDR_SIZE)) {
			dprintf(CRITICAL, "Splash TAG Is Mismatched\n");
                        ret = -1;
                        goto err;
		}

		if(splash_hdr->ulNumber < 0) {
		        dprintf(CRITICAL, " there is no data in splash partition \n");
                        ret = -1;
                        goto err;
		}

		img_idx = get_splash_index(splash_hdr, partition);
		dprintf(SPEW, "image idx = %d \n" ,img_idx);

		if(img_idx < 0) {
			dprintf(CRITICAL, " there is no image from [%s]\n", partition);
		}
		else{
			fb_cfg->width = splash_hdr->SPLASH_IMAGE[img_idx].ulImageWidth;
			fb_cfg->height = splash_hdr->SPLASH_IMAGE[img_idx].ulImageHeight;
			fb_cfg->stride = fb_cfg->width;

			if(!strcmp((const char *)splash_hdr->SPLASH_IMAGE[img_idx].ucFmt, "JPEG")) {
//			        dprintf(INFO, "Splash image format is JPEG \n");
			        img_fmt = 1;

			        tcc_read(ptn+(splash_hdr->SPLASH_IMAGE[img_idx].ulImageAddr), \
			                                decode_addr, splash_hdr->SPLASH_IMAGE[img_idx].ulImageSize );
			}
			else {
//			        dprintf(INFO, "Splash image format is BMP \n");
			        img_fmt = 0;

			        tcc_read(ptn+(splash_hdr->SPLASH_IMAGE[img_idx].ulImageAddr), \
			                                fb_cfg->base, splash_hdr->SPLASH_IMAGE[img_idx].ulImageSize );
			}

			if(img_fmt == 1) {
			        ret = splash_image_decode_jpeg(splash_hdr->SPLASH_IMAGE[img_idx].ulImageSize,
						&jpu_out, decode_addr);
                    if(ret != 0) {
                            ret = -1;
                            goto err;
                    }

			        splash_image_convert_format(&jpu_out, fb_cfg);
			}
		}
	}

	//dprintf(INFO, "==== %s out\n", __func__);

err:
        return ret;
}

