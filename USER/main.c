/**
 *	Keil project for DS18B20 library example
 *
 *	This example first search for all devices on 1wire bus
 *	Set some parameters, described down in code
 *	Start conversion on all devices
 *	Read temperatures and display it
 *	Checks for devices with alarm flag set
 *
 *	Before you start, select your target, on the right of the "Load" button
 *
 *	@author		Tilen Majerle
 *	@email		tilen@majerle.eu
 *	@website	http://stm32f4-discovery.com
 *	@ide		Keil uVision 5
 *	@packs		STM32F4xx Keil packs version 2.2.0 or greater required
 *	@stdperiph	STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 */
/* Include core modules */

#define __TEST_DS182B0__

#include "stm32f10x.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32f4_delay.h"

#ifdef __TEST_DS182B0__
#include "tm_stm32f4_onewire.h"
#include "tm_stm32f4_ds18b20.h"
#endif

#ifdef __TEST_DHT11__
#include "tm_stm32f4_am2301.h"
#endif

#if defined (__TEST_RTC__) || defined (__TEST_ADTIMERS__)
#include "tm_stm32f4_rtc.h"
#include "timer.h"
#endif

#ifdef __TEST_USART__
#include "tm_stm32f4_usart.h"
#endif

#ifdef __TEST_ADC__
#include "tm_stm32f4_adc.h"
#endif

#ifdef __TEST_SDFS__
#include "tm_stm32f4_fatfs.h"
#endif

#ifdef __TEST_ADTIMERS__
#include "stw_system_timer.h"
#endif

//#include "tm_stm32f4_disco.h"
#include <stdio.h>

/* How many sensors we are expecting on 1wire bus? */
#define EXPECTING_SENSORS	2

#if defined (__TEST_RTC__) || defined (__TEST_ADTIMERS__)
	char buf[50];
	/* Struct for RTC tim */
	TM_RTC_Time_t Time;
	/* Struct for alarm time */
	TM_RTC_AlarmTime_t AlarmTime;
#endif

#ifdef __TEST_ADTIMERS__
typedef struct {
    uint32_t timer_a; 
    uint32_t timer_b;
} some_data_t;  

some_data_t global_data; 
static void a_repeating_timer (stw_tmr_t *tmr, void *parm) ;
#endif

int main(void) {
#ifdef __TEST_DS182B0__
	char buf[40];
	uint8_t devices, i, j, count, alarm_count;
	uint8_t device[EXPECTING_SENSORS][8];
	uint8_t alarm_device[EXPECTING_SENSORS][8];
	float temps[EXPECTING_SENSORS];
	
	/* OneWire working struct */
	TM_OneWire_t OneWire1;
	
	/* Initialize system */
	SystemInit();
	
	/* Initialize delay */
	TM_DELAY_Init();

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
		sprintf(buf, "Devices found on 1-wire: %d\n", count);
		printf("%s", buf);
		/* Display 64bit rom code for each device */
		for (j = 0; j < count; j++) {
			for (i = 0; i < 8; i++) {
				sprintf(buf, "0x%02X ", device[j][i]);
				printf("%s", buf);
			}
			printf("\n");
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
	
	while (1) {
		/* Start temperature conversion on all devices on one bus */
		TM_DS18B20_StartAll(&OneWire1);
		
		/* Wait until all are done on one onewire port */
		while (!TM_DS18B20_AllDone(&OneWire1));
		
		/* Read temperature from each device separatelly */
		for (i = 0; i < count; i++) {
			/* Read temperature from ROM address and store it to temps variable */
			if (TM_DS18B20_Read(&OneWire1, device[i], &temps[i])) {
				/* Print temperature */
				sprintf(buf, "Temp %d: %3.5f; \n", i, temps[i]);
				printf("%s", buf);
			} else {
				/* Reading error */
				printf("Reading error;\n");
			}
		}
		
		/* Reset alarm count */
		alarm_count = 0;
		
		/* Check if any device has alarm flag set */
		while (TM_DS18B20_AlarmSearch(&OneWire1)) {
			/* Store ROM of device which has alarm flag set */
			TM_OneWire_GetFullROM(&OneWire1, alarm_device[alarm_count]);
			/* Increase count */
			alarm_count++;
		}
		
		sprintf(buf, "Devices with alarm: %d\n", alarm_count);
		printf("%s", buf);
		
		/* Any device has alarm flag set? */
		if (alarm_count > 0) {
			/* Show rom of this devices */
			for (j = 0; j < alarm_count; j++) {
				printf("Device with alarm: ");
				for (i = 0; i < 8; i++) {
					sprintf(buf, "0x%02X ", alarm_device[j][i]);
					printf("%s", buf);
				}
				printf("\n    ");
			}
			printf("ALARM devices recognized!\n\r");
		}
		
		/* Print separator */
		printf("----------\n");
		
		/* Some delay */
		Delayms(1000);

		printf("I am out of delay\n");
	}

#endif /* _TEST_DS182B0_ */


#ifdef __TEST_DHT11__
{
    TM_AM2301_Data_t data;
    
    /* Initialize system */
    SystemInit();
    
    /* Initialize delay */
    TM_DELAY_Init();

	printf("DHT11\nSensor");
    /* Initialize sensor */
    TM_AM2301_Init();

    /* Reset time */
    TM_DELAY_SetTime(0);
    while (1) {
        /* Every 1 second */
        if (TM_DELAY_Time() > 1000) {
            /* Reset time */
            TM_DELAY_SetTime(0);
            
            /* Data valid */
            if (TM_AM2301_Read(&data) == TM_AM2301_OK) {
                /* Show on LCD */
                printf("Humidity: %2.1f %%\nTemperature: %2.1f C", (float)data.Hum / 10, (float)data.Temp / 10);
            }
        }
        /* Do other stuff constantly */
    }
}

#endif /* __TEST_DHT11__ */


#ifdef __TEST_RTC__
	
    /* Initialize system */
    SystemInit();
    
    /* Initialize delay */
    TM_DELAY_Init();

	InitTimerList();
	
    /* Initialize RTC with internal 32768Hz clock */
    /* It's not very accurate */
    if (!TM_RTC_Init(TM_RTC_ClockSource_External)) {
        /* RTC was first time initialized */
        /* Do your stuff here */
        /* eg. set default time */
        /* Set new time */
        Time.tm_hour = 21;
        Time.tm_min = 11;
        Time.tm_sec = 00;
        Time.tm_year = 15;
        Time.tm_mon = 3;
        Time.tm_mday = 7;
        Time.tm_wday = 1;
        /* Set new RTC time */
        TM_RTC_SetDateTime(&Time);		
    }

    /* Set wakeup interrupt every 1 second */
    TM_RTC_Interrupts(TM_RTC_Int_1s);

//	Delayms(5000);
	
//  TM_RTC_GetDateTime(&Time);
	AddTimer(TIMER_1, 5); // 5 seconds

    AlarmTime.hours = 0;
    AlarmTime.minutes = 0;
    AlarmTime.seconds = 5;
    AlarmTime.day = 0;

    /* Set RTC alarm A, time in binary format */
    TM_RTC_SetAlarm(TM_RTC_Alarm_A, &AlarmTime);            
	printf("Alarm configured\n");
	
    while (1) {
//		Delayms(1000);
		CheckTimerEvent();
	}
#endif 

#ifdef __TEST_LED__
{
	/* configure GPIO PADA */
	GPIO_InitTypeDef GPIO_InitStructure;
#define LED1(a)	if (a)	\
					GPIO_SetBits(GPIOA,GPIO_Pin_5);\
					else		\
					GPIO_ResetBits(GPIOA,GPIO_Pin_5)

#define LED2(a)	if (a)	\
					GPIO_SetBits(GPIOA,GPIO_Pin_6);\
					else		\
					GPIO_ResetBits(GPIOA,GPIO_Pin_6)


    /* Initialize system */
    SystemInit();
    
    /* Initialize delay */
    TM_DELAY_Init();

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_SetBits(GPIOA, GPIO_Pin_5);	 // turn off all led

	while (1) {
		LED1(0);
		Delayms(1000);
		LED1(1);
		Delayms(10000);
		LED1(0);
		LED2(0);
		Delayms(1000);
		LED2(1);
		Delayms(10000);
		LED2(0);
	}	

}
#endif

#ifdef __TEST_USART__
{
	uint8_t c;

    /* Initialize system */
    SystemInit();
 
    /* Initialize USART1 at 9600 baud, TX: PB6, RX: PB7 */
    TM_USART_Init(USART1, TM_USART_PinsPack_1, 9600);
    
    /* Put string to USART */
    TM_USART_Puts(USART1, "Hello world\n\r");
 
    while (1) {
        /* Get character from internal buffer */
        c = TM_USART_Getc(USART1);
        if (c) {
            /* If anything received, put it back to terminal */
            TM_USART_Putc(USART1, c);
        }
    }
}
#endif

#ifdef __TEST_ADC__
{   
    /* Initialize system */
    SystemInit();
    
    /* Initialize Delay library */
    TM_DELAY_Init();
        
    /* Initialize ADC1 on channel 0, this is pin PA0 */
    TM_ADC_Init(ADC1, ADC_Channel_0);
    
    /* Initialize ADC1 on channel 3, this is pin PA3 */
    TM_ADC_Init(ADC1, ADC_Channel_3);
    
    while (1) {
        /*                             Read ADC1 Channel0                    Read ADC1 Channel3 */
        printf("%4d: %4d\n\r", TM_ADC_Read(ADC1, ADC_Channel_0), TM_ADC_Read(ADC1, ADC_Channel_3));
        
        /* Little delay */
        Delayms(1000);
    }
}
#endif

#ifdef __TEST_SDFS__
{
    //Fatfs object
    FATFS FatFs;
    //File object
    FIL fil;
    //Free and total space
    uint32_t total, free;
    
    //Initialize system
    SystemInit();
    //Initialize delays
    TM_DELAY_Init();
 
    //Mount drive
    if (f_mount(&FatFs, "", 1) == FR_OK) {
        //Mounted OK, turn on RED LED
        printf("SDFS Mount successfully\n");
        
        //Try to open file
        if (f_open(&fil, "1stfile.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE) == FR_OK) {
            //File opened, turn off RED and turn on GREEN led
			printf("file open successfully\n");            
            //If we put more than 0 characters (everything OK)
            if (f_puts("First string in my file\n", &fil) > 0) {
                if (TM_FATFS_DriveSize(&total, &free) == FR_OK) {
                    //Data for drive size are valid
                }
                
                //Turn on both leds
                printf("file written\n");
            }
            
            //Close file, don't forget this!
            f_close(&fil);
        }
        
        //Unmount drive, don't forget this!
        f_mount(0, "", 1);
    }
    
    while (1) {
 		Delayms(1000);
    }
}
#endif
#ifdef __TEST_ADTIMERS__
    /* Initialize system */
#define STW_NUMBER_BUCKETS     ( 512 ) /* useless */
#define STW_RESOLUTION         ( 1000 ) 
	uint32_t milliseconds;
	static stw_tmr_t demo_a_tmr; 
	
    stw_system_timer_create(STW_NUMBER_BUCKETS, 
                            STW_RESOLUTION, 
                            "Timer Wheel"); 


    SystemInit();
    
    /* Initialize delay */
    TM_DELAY_Init();
	
    /* Initialize RTC with internal 32768Hz clock */
    /* It's not very accurate */
    if (!TM_RTC_Init(TM_RTC_ClockSource_External)) {
        /* RTC was first time initialized */
        /* Do your stuff here */
        /* eg. set default time */
        /* Set new time */
        Time.tm_hour = 21;
        Time.tm_min = 11;
        Time.tm_sec = 00;
        Time.tm_year = 15;
        Time.tm_mon = 3;
        Time.tm_mday = 7;
        Time.tm_wday = 1;
        /* Set new RTC time */
        TM_RTC_SetDateTime(&Time);		
    }

    /* Set wakeup interrupt every 1 second */
    TM_RTC_Interrupts(TM_RTC_Int_1s);

    milliseconds = 10000;
    stw_timer_prepare(&demo_a_tmr);  
    stw_system_timer_start(&demo_a_tmr, 
                            milliseconds,
                            milliseconds,
                            a_repeating_timer, 
                            &global_data);

//	Delayms(5000);
	
//  TM_RTC_GetDateTime(&Time);

    AlarmTime.hours = 0;
    AlarmTime.minutes = 0;
    AlarmTime.seconds = 5;
    AlarmTime.day = 0;

    /* Set RTC alarm A, time in binary format */
    TM_RTC_SetAlarm(TM_RTC_Alarm_A, &AlarmTime);            
	printf("Alarm configured\n");
	
    while (1) {
//		Delayms(1000);
		CheckTimerEvent();
	}
#endif
}


#if defined (__TEST_RTC__)
/* Custom request handler function */
/* Called on wakeup interrupt */
void Timer1Function() {
    /* Get time */
    TM_RTC_GetDateTime(&Time);
    
    /* Format time */
    sprintf(buf, "%02d.%02d.%04d %02d:%02d:%02d \n",
                Time.tm_mday,
                Time.tm_mon,
                Time.tm_year,
                Time.tm_hour,
                Time.tm_min,
                Time.tm_sec
    );

	printf("wakeup timeout: %s\n", buf);

}
 
/* Custom request handler function */
/* Called on alarm A interrupt */
void TM_RTC_AlarmAHandler(void) {
    /* Show user to USART */
	printf("Alarm A triggered\n");
    
    /* Disable Alarm so it will not trigger next week at the same time */
    //TM_RTC_DisableAlarm(TM_RTC_Alarm_A);
}

void TM_RTC_RequestHandler() {
	SystemTickEvent();
}

#endif

#ifdef __TEST_ADTIMERS__

void TM_RTC_RequestHandler() {
    stw_system_timer_tick();
}

static void a_repeating_timer (stw_tmr_t *tmr, void *parm) 
{
    some_data_t *p2gdata;

    p2gdata = (some_data_t *)parm; 

    p2gdata->timer_a++;
    printf("%s timer a count=%u \n", 
           __FUNCTION__, 
           p2gdata->timer_a);

    return;
} 

#endif



