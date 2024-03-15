/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 mobis_getdata_micom.h

        Copyright(c) 2019 MOBIS Corp.

        Date            Name            Version         Description
        ----------      ------          ----------  ---------------------------
        2019.07.22      kibum.lee       v1.00           Create
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#ifndef __MOBIS_GET_DATA_MICOM__
#define __MOBIS_GET_DATA_MICOM__

#if 0
typedef enum _vehicle_code
{
	SK3		=0,
	LX2,
	ONN,
	DN8,
	OS,
	DEPE		=5,
	DE20,
	SP2,
	AEPE,
	DL3,
	YGPE		=10,
	IGPE,
	HMPE,
	MQ4,
	CN7,
	JSN		=15,
	JAPE,
	KA4,
	NX4,
	LKFL,
	CKPE		=20,
	HM21,
	VEHICLE_RESERVED22,
	VEHICLE_RESERVED23,
	VEHICLE_RESERVED24,
	VEHICLE_RESERVED25		=25,

	VEHICLE_MAX			=255
} vehicle_code_t;
#endif

typedef enum _vehicle_code
{
	VEHICLE_MAX	=0,
} vehicle_code_t;


typedef enum _country_code
{
	KOR		=0,
	USA,
	CHN,
	GEN,
	MIE,
	EUR		=5,
	CAN,
	RUS,
	TUR,
	AUS,
	GUM	=10,
	IND,
	MEX,
	BRA,
	IRN,
	ZAF,
	JPN,
	COUNTRY_RESERVED17,
	COUNTRY_RESERVED18,
	COUNTRY_RESERVED19,
	COUNTRY_RESERVED20,
	COUNTRY_MAX			=255
} country_code_t;

typedef enum _engine_type
{
	COMBUSTION		=0,
	EV,
	HEV,
	PHEV,
	FCEV,
	OTHER		=5,
	ENGINE_RESERVEDCAN6,
	ENGINE_RESERVEDCAN7,
} engine_type_t;

typedef struct _data_from_micom {
	unsigned char vehicle_code;
	unsigned char country_code;
	char* vehicle_string;
	char* country_string;
	char oem_type;
	char has_modem;
	int engine_type;
} data_from_micom_t;


int get_data_from_micom(void);
int is_initalized_micom_data(void);

data_from_micom_t* get_vehicle_country_info();

#endif
