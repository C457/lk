
#include <debug.h>
#include <dev/camera/sensor_if.h>
#include <platform/gpio.h>
#include <dev/camera/camera.h>
#include <daudio_ver.h>
#include <tcc_lcd.h>

extern struct tcc_cif_parameters	atv_parameters_data;
extern struct tcc_cif_parameters	lvds_parameters_data;
struct tcc_cif_parameters *		parameters_data = &atv_parameters_data;

SENSOR_FUNC_TYPE			sensor_func;

extern int sensor_if_connect_api(SENSOR_FUNC_TYPE * sensor_func);
extern int lvds_if_connect_api(SENSOR_FUNC_TYPE * sensor_func);

int sensor_if_close(void) {
	dprintf(INFO, "!@#---- %s\n", __func__);
	
	return sensor_func.close();
}

int sensor_if_open(void) {
	int		camera_type	= CAM_TYPE_DEFAULT;
	int		camera_encode	= CAM_ENC_DEFAULT;

	unsigned int loop=0;
	unsigned int mode0 = 0, mode1 = 0, mode2 = 0;
	unsigned int mode_value[3];
	struct lcd_panel *panel_info;
	
	//dprintf(INFO, "!@#---- %s\n", __func__);
	
	for(loop = 0; loop < 3; loop++) {
		mode0 = gpio_get( GPIO_PORTB|22 );
		mode1 = gpio_get( GPIO_PORTB|19 );      //M101_CAMERA_VARI2, PB[12]
		mode2 = gpio_get( GPIO_PORTB|23 );      //M100_CAMERA_VARI1, PD[12]
		camera_type = ((mode0<<2)|(mode1<<1)|mode2);
		mode_value[loop] = camera_type;

		mdelay(10);
	}

	if(daudio_lcd_type_lvds_check())
	{
		dprintf(INFO, "%s : LCD Type(LVDS)\n", __func__);
		parameters_data->viocmg_info.main_display_id = 1;
		parameters_data->viocmg_info.early_cam_display_rdma = 5;
	}
	else	//HDMI - display_id = 0, dislay_rdma = 1
		dprintf(INFO, "%s : LCD Type(HDMI)\n", __func__);

	if( mode_value[0] == mode_value[1] && mode_value[1] == mode_value[2] )
	{
		dprintf(INFO, "%s : gpio read value pass.................. \n", __func__);

		if(camera_type == 4 || camera_type == 6)
		{ //this should be seperated for adding 140 pixels black screen on the left
			parameters_data = &lvds_parameters_data;
			parameters_data->Camera_type = CAM_TYPE_LVDS;	// LVDS
			lvds_if_connect_api(&sensor_func);
		} else if (camera_type == 5)
		{
			parameters_data = &lvds_parameters_data;
			parameters_data->Camera_type = CAM_TYPE_DVRS_RVM; // DVRS_RVM
			lvds_if_connect_api(&sensor_func);
		} else if (camera_type == 7)
		{
			parameters_data = &lvds_parameters_data;
			parameters_data->Camera_type = CAM_TYPE_ADAS_PRK;	// ADAS_PRK
			parameters_data->Cam_preview_w = 1280; //Vin input size from camera/module
			lvds_if_connect_api(&sensor_func);
		} else //FOR analog rvm/svm camera type
		{
			parameters_data = &atv_parameters_data;

			switch(camera_type)
			{
				case 1:
					parameters_data->Camera_type = CAM_TYPE_SVIDEO;		// svm_svideo
					parameters_data->viocmg_info.early_cam_preview_crop_y = parameters_data->viocmg_info.early_cam_preview_crop_y + 6;
					parameters_data->viocmg_info.early_cam_preview_additional_height = parameters_data->viocmg_info.early_cam_preview_additional_height ;
					sensor_if_connect_api(&sensor_func);
					break;
				/*case 2:
					parameters_data->Camera_type = CAM_TYPE_COMPONENT;	// avm-component
					sensor_if_connect_api(&sensor_func);
					break;
				case 3:
					  parameters_data->Camera_type = CAM_TYPE_UPDATE;		// update
					  break;*/
				default:
					parameters_data->Camera_type = CAM_TYPE_CVBS;		// CVBS
					sensor_if_connect_api(&sensor_func);
					break;
			}
		}

		panel_info = tccfb_get_panel();

		// The default preview size is same as LCD size
		parameters_data->viocmg_info.early_cam_preview_width	= panel_info->xres;
		parameters_data->viocmg_info.early_cam_preview_height	= panel_info->yres;
		parameters_data->viocmg_info.early_cam_parking_line_width	= panel_info->xres;
		parameters_data->viocmg_info.early_cam_parking_line_height	= panel_info->yres;
		parameters_data->viocmg_info.early_cam_bg_width		= panel_info->xres;
		parameters_data->viocmg_info.early_cam_bg_height	= panel_info->yres;

		// Custom scenarios by vehicle type
		// For RVM camera types(analog & digital) 140 pixels of left black screen is added.
		switch(parameters_data->Camera_type)
		{
			case CAM_TYPE_CVBS:
				if( panel_info->xres >= 1280)
				{
						parameters_data->viocmg_info.early_cam_preview_x = 140; // the size of black screen on the left = X
						parameters_data->viocmg_info.early_cam_preview_width = 1280-parameters_data->viocmg_info.early_cam_preview_x;
				}
				break;

			case CAM_TYPE_SVIDEO:
				if( panel_info->xres >= 1280)
				{
					parameters_data->viocmg_info.early_cam_preview_x = 0; // (Analog) SVM does not have black screen on the left = X
					parameters_data->viocmg_info.early_cam_preview_width = 1280-parameters_data->viocmg_info.early_cam_preview_x;
				}
				break;

			case CAM_TYPE_DVRS_RVM:
					if( panel_info->xres >= 1920)
					{
						parameters_data->viocmg_info.early_cam_preview_x = 140; // the size of black screen on the left = X
						parameters_data->viocmg_info.early_cam_preview_width = 1710;  // 1920 x {(1280-X)/1280} = 1710 (X = 140) will be display FROM (140,0) coordinates in LCD
					}
				break;

			case CAM_TYPE_LVDS:

				break;

			case CAM_TYPE_ADAS_PRK:
				if( panel_info->xres >= 1280)
				{
						parameters_data->viocmg_info.early_cam_preview_x = 140; // the size of black screen on the left = X
						parameters_data->viocmg_info.early_cam_preview_width = 1280-parameters_data->viocmg_info.early_cam_preview_x;
				}
				break;

			default:
				dprintf(INFO, "%s : Not support Camera Type\n", __func__);
				break;
		}

		dprintf(INFO, "%s preview - x: %d, y: %d, w: %d, h: %d\n", __func__, \
			parameters_data->viocmg_info.early_cam_preview_x, parameters_data->viocmg_info.early_cam_preview_y, \
			parameters_data->viocmg_info.early_cam_preview_width, parameters_data->viocmg_info.early_cam_preview_height	);

	}

	dprintf(INFO, "%s : gpio read value = %d \n", __func__, camera_type);

	return sensor_func.open(camera_type, camera_encode);
}

int sensor_if_tune(int camera_type, int camera_encode) {
	dprintf(INFO, "!@#---- %s - type = 0x%x, encode = 0x%x\n", __func__, camera_type, camera_encode);
	
	return sensor_func.tune(camera_type, camera_encode);
}

