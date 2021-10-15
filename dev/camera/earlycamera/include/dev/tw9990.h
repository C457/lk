
/****************************************************************************
  Copyright (C) 2014 Telechips, Inc.
****************************************************************************/

#ifndef __TW9990_H__
#define __TW9990_H__

#define TW9990_I2C_ADDR 			0x88   //Module I2C Slave address
#define TW9990_I2C_CH_NO			3	   // I2C Channel number

extern int sensor_connect_api(SENSOR_FUNC_TYPE * sensor_func);

#endif//__TW9990_H__
