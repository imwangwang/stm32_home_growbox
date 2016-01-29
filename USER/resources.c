#include "tm_stm32f4_rtc.h"
#include "tm_stm32f4_onewire.h"
#include "tm_stm32f4_ds18b20.h"
#include "tm_stm32f4_am2301.h"
#include "tm_stm32f4_usart.h"
#include "tm_stm32f4_adc.h"
#include "stw_system_timer.h"
#include <stdio.h>
#include "resource.h"

int rtc_get(void *data)
{
	TM_RTC_GetDateTime((TM_RTC_t *)data);
	return 0;
}

int rtc_set(void *data)
{
	TM_RTC_SetDateTime(data);
	return 0;
}

const struct resource rtc_res = {
	"rtc",
	RES_RTC,
	rtc_get,
	rtc_set,
	};

int tmp_get(void *data)
{
	static uint32_t init = 0;
	static uint8_t devices, i, j, alarm_count;
	static uint8_t count;
	static uint8_t device[EXPECTING_SENSORS][8];
	static uint8_t alarm_device[EXPECTING_SENSORS][8];
	static float temps[EXPECTING_SENSORS];
	/* OneWire working struct */
	static TM_OneWire_t OneWire1;

	if (init == 0) {
		init = 1;
		/* Initialize OneWire on pin PD0 */
		TM_OneWire_Init(&OneWire1, GPIOD, GPIO_Pin_0, RCC_APB2Periph_GPIOD);

		/* Checks for any device on 1-wire */
		count = 0;
		devices = TM_OneWire_First(&OneWire1);
		while (devices) {
			/* Increase counter */
			count++;
			
			/* Get full ROM value, 8 bytes, give location of first byte where to save */
			TM_OneWire_GetFullROM(&OneWire1, device[count - 1]);
			
			/* Get next device */
			devices = TM_OneWire_Next(&OneWire1);
		}

		/* If any devices on 1wire */
		if (count > 0) {
//			printf("Devices found on 1-wire: %d\n", count);
			/* Display 64bit rom code for each device */
			for (j = 0; j < count; j++) {
				for (i = 0; i < 8; i++) {
//					printf("0x%02X ", device[j][i]);
				}
//				printf("\n");
			}
		} else {
			printf("No devices on OneWire.\n");
		}
		
		/* Go through all connected devices and set resolution to 12bits */
		for (i = 0; i < count; i++) {
			/* Set resolution to 12bits */
			TM_DS18B20_SetResolution(&OneWire1, device[i], TM_DS18B20_Resolution_12bits);
		}

		/* Set high temperature alarm on device number 0, 25 degrees celcius */
		TM_DS18B20_SetAlarmHighTemperature(&OneWire1, device[0], 25);

		/* Disable alarm temperatures on device number 1 */
		TM_DS18B20_DisableAlarmTemperature(&OneWire1, device[1]);
		}
		/* Start temperature conversion on all devices on one bus */
		TM_DS18B20_StartAll(&OneWire1);
		
		/* Wait until all are done on one onewire port */
		while (!TM_DS18B20_AllDone(&OneWire1));
		
		/* Read temperature from each device separatelly */
		for (i = 0; i < count; i++) {
			/* Read temperature from ROM address and store it to temps variable */
			if (TM_DS18B20_Read(&OneWire1, device[i], &temps[i])) {
				/* Print temperature */
				printf("Temp %d: %d; \n", i, (int)temps[i]);
			} else {
				/* Reading error */
				printf("Reading error;\n");
			}
		}

		*(float *)(data) = temps[0];
		if (i == 0)
			return -1;
		
		/* Reset alarm count */
		alarm_count = 0;
		
		/* Check if any device has alarm flag set */
		while (TM_DS18B20_AlarmSearch(&OneWire1)) {
			/* Store ROM of device which has alarm flag set */
			TM_OneWire_GetFullROM(&OneWire1, alarm_device[alarm_count]);
			/* Increase count */
			alarm_count++;
		}
		
//		printf("Devices with alarm: %d\n", alarm_count);
		
		/* Any device has alarm flag set? */
		if (alarm_count > 0) {
			/* Show rom of this devices */
			for (j = 0; j < alarm_count; j++) {
//				printf("Device with alarm: ");
				for (i = 0; i < 8; i++) {
//					printf("0x%02X ", alarm_device[j][i]);
				}
//				printf("\n    ");
			}
//			printf("ALARM devices recognized!\n\r");
		}
		
		/* Print separator */
//		printf("----------\n");

		return 0;
}

const struct resource tmp_res = {
	"airtemp",
	RES_RTC,
	tmp_get,
	NULL,
	};


int airhumd_get(void *data)
{
	static uint32_t initialized = 0;
	
	if (!initialized) {
		initialized = 1;
	    TM_AM2301_Init();
		}
	
    if (TM_AM2301_Read((TM_AM2301_Data_t *)data) == TM_AM2301_OK) {
        /* Show on LCD */
        printf("Humidity: %2.1f %%\nTemperature: %2.1f C", (float)(((TM_AM2301_Data_t *)data)->Hum / 10), 
			(float)(((TM_AM2301_Data_t *)data)->Temp / 10));
		return 0;
    }

	return -1;
}

struct resource airhumd_res = {
	"airhumd",
	RES_AIR_HUMDITY,
	airhumd_get,
	NULL,
	};

int soilhumd_get(void *data)
{
	static uint32_t initialized = 0;
	int ret = 0;
	
	if (!initialized) {
		initialized = 1;
		/* Initialize ADC1 on channel 0, this is pin PA0 */
		TM_ADC_Init(ADC1, ADC_Channel_0);
		TM_ADC_Read(ADC1, ADC_Channel_0);
		while (ret < 0xFFFF) ret++;
		}
	ret = TM_ADC_Read(ADC1, ADC_Channel_0);
	printf("soilhumd get : %d\n", ret);
	return ret;
}

const struct resource soilhumd_res = {
	"soilhumd",
	RES_SOIL_HUMDITY,
	soilhumd_get,
	NULL,
	};

static uint32_t rear_growlight_initialized = 0;
int rear_growlight_get(void *data)
{
	/* just ignore parameter */
	GPIO_InitTypeDef GPIO_InitStructure;

#define GROWLIGHT_GET()	GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_5)
	
	if (!rear_growlight_initialized) {
		rear_growlight_initialized = 1;
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		}

	return GROWLIGHT_GET();

}

int rear_growlight_set(void *data)
{
	GPIO_InitTypeDef GPIO_InitStructure;

#define GROWLIGHT_SET(a)	if (a)	\
					GPIO_SetBits(GPIOA, GPIO_Pin_5);\
					else		\
					GPIO_ResetBits(GPIOA,GPIO_Pin_5)
	
	if (!rear_growlight_initialized) {
		rear_growlight_initialized = 1;
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		}
	
	printf("growlight: %s\n", (*(uint32_t *)data) ? "enable" : "disable");
	GROWLIGHT_SET(*((uint32_t *)data));
	return 0;
}

const struct resource reargrowlight_res = {
	"reargrow",
	RES_REAR_GROW_LIGHT,
	rear_growlight_get,
	rear_growlight_set,
	};


static uint32_t rear_warmlight_initialized = 0;
int rear_warmlight_get(void *data)
{
	/* just ignore parameter */
	GPIO_InitTypeDef GPIO_InitStructure;

#define WARMLIGHT_GET()	GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_6)
	
	if (!rear_warmlight_initialized) {
		rear_warmlight_initialized = 1;
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		}

	return WARMLIGHT_GET();

}

int rear_warmlight_set(void *data)
{
	GPIO_InitTypeDef GPIO_InitStructure;

#define WARMLIGHT_SET(a)	if (a)	\
					GPIO_SetBits(GPIOA, GPIO_Pin_6);\
					else		\
					GPIO_ResetBits(GPIOA,GPIO_Pin_6)
	
	if (!rear_warmlight_initialized) {
		rear_warmlight_initialized = 1;
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		}
	
	printf("WarmLight: %s\n", (*(uint32_t *)data) ? "enable" : "disable");
	WARMLIGHT_SET(*((uint32_t *)data));
	return 0;
}

const struct resource rearwarmlight_res = {
	"rearwarm",
	RES_REAR_GROW_LIGHT,
	rear_warmlight_get,
	rear_warmlight_set,
	};

/* alarm is a dumb resource FIXME: */
const struct resource alarm_res = {
	"alarm",
	RES_ALARM,
	NULL,
	NULL,
	};

const struct resource usart_res = {
	"usart",
	RES_USART,
	NULL,
	NULL,
	};

int light_get(void *data)
{
	static uint32_t initialized = 0;
	int ret;
	if (!initialized) {
		initialized = 1;
		/* Initialize ADC1 on channel 0, this is pin PA0 */
		TM_ADC_Init(ADC1, ADC_Channel_3);
		}
	ret = TM_ADC_Read(ADC1, ADC_Channel_3);
	printf("light val: %d\n", ret);
	return ret;
}

const struct resource light_res = {
	"light",
	RES_SOIL_HUMDITY,
	light_get,
	NULL,
	};

static uint32_t pumpA_initialized = 0;
int pumpA_get(void *data)
{
	/* just ignore parameter */
	GPIO_InitTypeDef GPIO_InitStructure;

#define PUMPA_GET()	GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_7)
	
	if (!pumpA_initialized) {
		pumpA_initialized = 1;
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		}

	return PUMPA_GET();

}

int pumpA_set(void *data)
{
	GPIO_InitTypeDef GPIO_InitStructure;

#define PUMPA_SET(a)	if (a)	\
					GPIO_SetBits(GPIOA, GPIO_Pin_7);\
					else		\
					GPIO_ResetBits(GPIOA,GPIO_Pin_7)
	
	if (!pumpA_initialized) {
		pumpA_initialized = 1;
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		}

	printf("pumpA: %s\n", (*(uint32_t *)data) ? "enable" : "disable");
	
	PUMPA_SET(*((uint32_t *)data));
	return 0;
}

const struct resource pumpA_res = {
	"pumpA",
	RES_PUMPA,
	pumpA_get,
	pumpA_set,
	};

static uint32_t pumpB_initialized = 0;
int pumpB_get(void *data)
{
	/* just ignore parameter */
	GPIO_InitTypeDef GPIO_InitStructure;

#define PUMPB_GET()	GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_8)
	
	if (!pumpB_initialized) {
		pumpB_initialized = 1;
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		}

	return PUMPB_GET();

}

int pumpB_set(void *data)
{
	GPIO_InitTypeDef GPIO_InitStructure;

#define PUMPB_SET(a)	if (a)	\
					GPIO_SetBits(GPIOA, GPIO_Pin_8);\
					else		\
					GPIO_ResetBits(GPIOA,GPIO_Pin_8)
	
	if (!pumpB_initialized) {
		pumpB_initialized = 1;
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		}

	PUMPB_SET(*((uint32_t *)data));
	return 0;
}

const struct resource pumpB_res = {
	"pumpB",
	RES_PUMPB,
	pumpB_get,
	pumpB_set,
	};


const struct resource *res_array[] = {
	&rtc_res,
	&tmp_res,
	&airhumd_res,
	&soilhumd_res,
	&reargrowlight_res,
	&rearwarmlight_res,
	&alarm_res,
	&usart_res,
	&light_res,
	&pumpA_res,
	&pumpB_res
	};





