/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 mobis_getdata_micom.c

        Copyright(c) 2019 MOBIS Corp.

        Date            Name            Version         Description
        ----------      ------          ----------  ---------------------------
        2019.07.22      kibum.lee       v1.00           Create
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <stdlib.h>
//#include <platform/uart.h>
#include <dev/uart.h>
#include <daudio_ver.h>
#include "mobis_getdata_micom.h"

//#define MICOM_UART	2

// for test
//#define LOCAL_TEST

//read data from micom
#define MAX_READ_DATA_RETRY               3
#define MAX_READ_DATA_FROM_MICOM          8
#define UART_BUSY_WAIT_TIME_COUNT	10000	// wide(10000,  17ms)
#define FIRST_UART_BUSY_WAIT_TIME_COUNT	(5*UART_BUSY_WAIT_TIME_COUNT)
static unsigned char magic_buff[] = { 'b', 's', 'p' };
static unsigned char buff_micom[MAX_READ_DATA_FROM_MICOM] = {0, };
static int initalized_get_data = 0;
static data_from_micom_t vehicle_country_code;

#if 0
static const char* mobis_wide_pe_vehicle_code[] =
{
	"SK3",
	"LX2",
	"ON",
	"DN8",
	"OS",
	"DEPE",
	"DE20",
	"SP2",
	"AEPE",
	"DL3",
	"YGPE",
	"IGPE",
	"HMPE",
	"MQ4",
	"CN7",
	"JSN",
	"JAPE",
	"KA4",
	"NX4",
	"LKFL",
	"CKPE",
	"HM21",
	"VEHICLE_RESERVED22",
	"VEHICLE_RESERVED23",
	"VEHICLE_RESERVED24",
	"VEHICLE_RESERVED25",
};
#endif

static const char* mobis_default_vehicle_code[] =
{
	"VEHICLE_MAX",
};

static const char* mobis_country_code[] =
{
	"KOR",
	"USA",
	"CHN",
	"GEN",
	"MIE",
	"EUR",
	"CAN",
	"RUS",
	"TUR",
	"AUS",
	"GUM",
	"IND",
	"MEX",
	"BRA",
	"IRN",
	"ZAF",
	"JPN",
	"COUNTRY_RESERVED17",
	"COUNTRY_RESERVED18",
	"COUNTRY_RESERVED19",
	"COUNTRY_RESERVED20",
};

static const char* mobis_engine_type[] =
{
	"COMBUSTION",
	"EV",
	"HEV",
	"PHEV",
	"FCEV",
	"OTHER",
	"ENGINE_RESERVEDCAN6",
	"ENGINE_RESERVEDCAN7",
};

//-------------------------------------------------------------------
static void set_initalized_micom_data(int value);
static int setting_get_data_from_micom(unsigned char* buff);

//-------------------------------------------------------------------

data_from_micom_t* get_vehicle_country_info()
{
	//dprintf(INFO, "2army, func=%s %d\n", __func__, is_initalized_micom_data());

	if(is_initalized_micom_data() != 0)
		return &vehicle_country_code;
	else
		return NULL;
}

static int setting_get_data_from_micom(unsigned char* buff)
{
	int vehicle_code_size=0;
	int country_code_size=0;
	vehicle_country_code.vehicle_code = buff[3];
	vehicle_country_code.country_code = buff[4];

	country_code_size = sizeof(mobis_country_code)/sizeof(mobis_country_code[0]);
	//dprintf(INFO, "size=%d %d\n", vehicle_code_size, country_code_size);

	vehicle_country_code.vehicle_string = (char*)mobis_default_vehicle_code[0];

	if(vehicle_country_code.country_code > country_code_size)
		vehicle_country_code.country_string = "MAX_NUM";
	else
		vehicle_country_code.country_string = (char*)mobis_country_code[vehicle_country_code.country_code];

#ifdef LOCAL_TEST
	buff[5] = 0xC0;   // 1 1
	vehicle_country_code.oem_type = (buff[5] &  0x80) >> 7;
	vehicle_country_code.has_modem = (buff[5] &0x40) >> 6;
	vehicle_country_code.engine_type = (buff[5] &0x38) >> 3;
	dprintf(INFO, "vehicle_code=0x%02x, country_code=0x%02x, country=%s, oem_type=%d, modem=%d, engine=%d\n",
		vehicle_country_code.vehicle_code, vehicle_country_code.country_code,
		vehicle_country_code.country_string,
		vehicle_country_code.oem_type, vehicle_country_code.has_modem, vehicle_country_code.engine_type);

	buff[5] = 0x80;  // 1 0
	vehicle_country_code.oem_type = (buff[5] &  0x80) >> 7;
	vehicle_country_code.has_modem = (buff[5] &0x40) >> 6;
	vehicle_country_code.engine_type = (buff[5] &0x38) >> 3;
	dprintf(INFO, "vehicle_code=0x%02x, country_code=0x%02x, country=%s, oem_type=%d, modem=%d, engine=%d\n",
		vehicle_country_code.vehicle_code, vehicle_country_code.country_code,
		vehicle_country_code.country_string,
		vehicle_country_code.oem_type, vehicle_country_code.has_modem, vehicle_country_code.engine_type);

	buff[5] = 0x40;  // 0 1
	vehicle_country_code.oem_type = (buff[5] &  0x80) >> 7;
	vehicle_country_code.has_modem = (buff[5] &0x40) >> 6;
	vehicle_country_code.engine_type = (buff[5] &0x38) >> 3;
	dprintf(INFO, "vehicle_code=0x%02x, country_code=0x%02x, country=%s, oem_type=%d, modem=%d, engine=%d\n",
		vehicle_country_code.vehicle_code, vehicle_country_code.country_code,
		vehicle_country_code.country_string,
		vehicle_country_code.oem_type, vehicle_country_code.has_modem, vehicle_country_code.engine_type);

	buff[5] = 0x2F;  // 0 0
	vehicle_country_code.oem_type = (buff[5] &  0x80) >> 7;
	vehicle_country_code.has_modem = (buff[5] &0x40) >> 6;
	vehicle_country_code.engine_type = (buff[5] &0x38) >> 3;
	dprintf(INFO, "vehicle_code=0x%02x, country_code=0x%02x, country=%s, oem_type=%d, modem=%d, engine=%d\n",
		vehicle_country_code.vehicle_code, vehicle_country_code.country_code,
		vehicle_country_code.country_string,
		vehicle_country_code.oem_type, vehicle_country_code.has_modem, vehicle_country_code.engine_type);
#endif
	vehicle_country_code.oem_type = (buff[5] &  0x80) >> 7;
	vehicle_country_code.has_modem = (buff[5] &0x40) >> 6;
	vehicle_country_code.engine_type = (buff[5] &0x38) >> 3;

	set_initalized_micom_data(true);
	dprintf(INFO, "vehicle_code=0x%02x, country_code=0x%02x, country=%s, oem_type=%d, modem=%d, engine=%d\n",
		vehicle_country_code.vehicle_code, vehicle_country_code.country_code,
		vehicle_country_code.country_string,
		vehicle_country_code.oem_type, vehicle_country_code.has_modem, vehicle_country_code.engine_type);
	return 0;
}
static unsigned char calc_NMEA_checksum(unsigned char *buf, int len )
{
	unsigned char Character;
	unsigned char Checksum = 0;
	int i; // loop counter

	for (i=0; i < len; ++i)
	{
		Character = buf[i];
		switch(Character)
		{
			case '$':
			// Ignore the dollar sign
				break;
			case '*':
			// Stop processing before the asterisk
				i = len;
				continue;
			default:
				if (Checksum == 0)
					Checksum = Character;
				else
					Checksum = Checksum ^ Character;
				break;
		}
	}
	return Checksum;
}
static int verify_data_from_micom(unsigned char* buff)
{
	int i = 0;
	unsigned char val ='\0';
#ifdef LOCAL_TEST
	buff[0] = magic_buff[0];
	buff[1] = magic_buff[1];
	buff[2] = magic_buff[2];
	buff[3] = 0x80;
	buff[4] = 0x00;
	buff[5] = 0x00;
	buff[6] = 0x80;
	buff[7] = 0x61;	// result of the checksum
	for(i=0;i<MAX_READ_DATA_FROM_MICOM;i++)
		printf("%02x ", buff[i]);
	printf("\n");
#endif
	if ((buff[0] != magic_buff[0]) || (buff[1] != magic_buff[1]) || (buff[2] != magic_buff[2]))
		return -1;
	val = calc_NMEA_checksum(buff, MAX_READ_DATA_FROM_MICOM-1);
	printf("checksum=%02x\n", val);
	if (buff[7] !=val)
		return -1;

	setting_get_data_from_micom(buff);
	return 0;
}
//-------------------------------------------------------------------
int get_data_from_micom(void)
{
	int i=0;
	int j=0;

	for(i=0;i<MAX_READ_DATA_RETRY;i++)
	{
		uart_flush_rx(MICOM_UART);
		dprintf(SPEW, "func=%s, line=%d start\n", __func__, __LINE__);
		set_signal_to_micom(1);
		for(j=0;j<MAX_READ_DATA_FROM_MICOM;j++)
		{
			if(i <MAX_READ_DATA_RETRY-1)
			{
				if (j==0)
					buff_micom[j] = uart_getc_ex(MICOM_UART, FIRST_UART_BUSY_WAIT_TIME_COUNT);
				else
					buff_micom[j] = uart_getc_ex(MICOM_UART, UART_BUSY_WAIT_TIME_COUNT);
			}
			else
			{
				//final get data (so slow option)
				if (j==0)
					buff_micom[j] = uart_getc_ex(MICOM_UART, FIRST_UART_BUSY_WAIT_TIME_COUNT*40);
				else
					buff_micom[j] = uart_getc_ex(MICOM_UART, UART_BUSY_WAIT_TIME_COUNT);
			}
		}
		for(j=0;j<MAX_READ_DATA_FROM_MICOM;j++)
			dprintf(SPEW, "%02x ", buff_micom[j]);
		dprintf(SPEW, "\n");
		dprintf(CRITICAL, "func=%s, line=%d, retry=%d\n", __func__, __LINE__, i);
		set_signal_to_micom(0);

		if (verify_data_from_micom(buff_micom) == 0)
			return 0;

		udelay2(10000);		// must needed ( confirm with micom)
		udelay2(5000);
	}
	uart_flush_rx(MICOM_UART);
	return 0;
}

//-------------------------------------------------------------------
int is_initalized_micom_data(void)
{
	//dprintf(SPEW, "func=%s, line=%d\n", __func__, __LINE__);
	return initalized_get_data;
}

static void set_initalized_micom_data(int value)
{
	dprintf(SPEW, "func=%s, line=%d\n", __func__, __LINE__);
	initalized_get_data = value;
}
//-------------------------------------------------------------------
