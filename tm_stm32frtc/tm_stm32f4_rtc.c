/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#include "tm_stm32f4_rtc.h"

/* Internal functions */
void TM_RTC_Config(TM_RTC_ClockSource_t source);

uint32_t TM_RTC_Status = RTC_STATUS_ZERO;

NVIC_InitTypeDef NVIC_InitStruct;
EXTI_InitTypeDef EXTI_InitStruct;

uint8_t TM_RTC_Months[2][12] = {
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},	/* Not leap year */
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}	/* Leap year */
};

#define FEBRUARY		2
#define	STARTOFTIME		1970
#define SECDAY			86400L
#define SECYR			(SECDAY * 365)
#define	leapyear(year)		((year) % 4 == 0)
#define	days_in_year(a) 	(leapyear(a) ? 366 : 365)
#define	days_in_month(a) 	(month_days[(a) - 1])

static int month_days[12] = {	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

uint32_t TM_RTC_Init(TM_RTC_ClockSource_t source) {
	uint32_t status;
	uint8_t stat = 1;
	TM_RTC_t datatime;
	
	/* Enable PWR peripheral clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);
	
	/* Get RTC status */
	status = BKP_ReadBackupRegister(RTC_STATUS_REG);
	
	if (status == RTC_STATUS_TIME_OK) {
		TM_RTC_Status = RTC_STATUS_TIME_OK;
		
		/* Start internal clock if we choose internal clock */
		if (source == TM_RTC_ClockSource_Internal) {
			TM_RTC_Config(TM_RTC_ClockSource_Internal);
		}
		
		/* Wait for RTC APB registers synchronisation (needed after start-up from Reset) */
		RTC_WaitForSynchro();
		
		/* Clear interrupt flags */
		RTC_ClearITPendingBit(RTC_IT_ALR);
		EXTI_ClearITPendingBit(EXTI_Line17);
		
		/* Get date and time */
		TM_RTC_GetDateTime(&datatime);
	} else if (status == RTC_STATUS_INIT_OK) {
		TM_RTC_Status = RTC_STATUS_INIT_OK;
		
		/* Start internal clock if we choose internal clock */
		if (source == TM_RTC_ClockSource_Internal) {
			TM_RTC_Config(TM_RTC_ClockSource_Internal);
		}
		
		/* Wait for RTC APB registers synchronisation (needed after start-up from Reset) */
		RTC_WaitForSynchro();
		
		/* Clear interrupt flags */
		RTC_ClearITPendingBit(RTC_IT_ALR);
		EXTI_ClearITPendingBit(EXTI_Line17);
		
		/* Get date and time */
		TM_RTC_GetDateTime(&datatime);
	} else {
		TM_RTC_Status = RTC_STATUS_ZERO;
		/* Return status = 0 -> RTC Never initialized before */
		stat = RTC_STATUS_ZERO;
		/* Config RTC */
		TM_RTC_Config(source);
		
		/* Set date and time */
		datatime.tm_mday = 1;
		datatime.tm_wday= 1;
		datatime.tm_mon= 1;
		datatime.tm_year = 0;
		datatime.tm_hour = 0;
		datatime.tm_min = 0;
		datatime.tm_sec = 0;
		
		/* Set date and time */
		TM_RTC_SetDateTime(&datatime);
		
		/* Initialized OK */
		TM_RTC_Status = RTC_STATUS_INIT_OK;
	}
	/* If first time initialized */
	if (stat == RTC_STATUS_ZERO) {
		return 0;
	}
	return TM_RTC_Status;
}

/*
 * This only works for the Gregorian calendar - i.e. after 1752 (in the UK)
 */
void GregorianDay(TM_RTC_t *tm)
{
	int leapsToDate;
	int lastYear;
	int day;
	int MonthOffset[] = { 0,31,59,90,120,151,181,212,243,273,304,334 };

	lastYear=tm->tm_year-1;

	leapsToDate = lastYear/4 - lastYear/100 + lastYear/400;      

	if((tm->tm_year%4==0) &&
	   ((tm->tm_year%100!=0) || (tm->tm_year%400==0)) &&
	   (tm->tm_mon>2)) {
		/*
		 * We are past Feb. 29 in a leap year
		 */
		day=1;
	} else {
		day=0;
	}

	day += lastYear*365 + leapsToDate + MonthOffset[tm->tm_mon-1] + tm->tm_mday;

	tm->tm_wday=day%7;
}

/* Converts Gregorian date to seconds since 1970-01-01 00:00:00.
 * Assumes input in normal date format, i.e. 1980-12-31 23:59:59
 * => year=1980, mon=12, day=31, hour=23, min=59, sec=59.
 *
 * [For the Julian calendar (which was used in Russia before 1917,
 * Britain & colonies before 1752, anywhere else before 1582,
 * and is still in use by some communities) leave out the
 * -year/100+year/400 terms, and add 10.]
 *
 * This algorithm was first published by Gauss (I think).
 *
 * WARNING: this function will overflow on 2106-02-07 06:28:16 on
 * machines were long is 32-bit! (However, as time_t is signed, we
 * will already get problems at other places on 2038-01-19 03:14:08)
 */
u32 mktimev(TM_RTC_t *tm)
{
	if (0 >= (int) (tm->tm_mon -= 2)) {	/* 1..12 -> 11,12,1..10 */
		tm->tm_mon += 12;		/* Puts Feb last since it has leap day */
		tm->tm_year -= 1;
	}

	return (((
		(u32) (tm->tm_year/4 - tm->tm_year/100 + tm->tm_year/400 + 367*tm->tm_mon/12 + tm->tm_mday) +
			tm->tm_year*365 - 719499
	    )*24 + tm->tm_hour /* now have hours */
	  )*60 + tm->tm_min /* now have minutes */
	)*60 + tm->tm_sec; /* finally seconds */
}

void to_tm(u32 tim, TM_RTC_t *tm)
{
	register u32    i;
	register long   hms, day;

	day = tim / SECDAY;
	hms = tim % SECDAY;

	/* Hours, minutes, seconds are easy */
	tm->tm_hour = hms / 3600;
	tm->tm_min = (hms % 3600) / 60;
	tm->tm_sec = (hms % 3600) % 60;

	/* Number of years in days */
	for (i = STARTOFTIME; day >= days_in_year(i); i++) {
		day -= days_in_year(i);
	}
	tm->tm_year = i;

	/* Number of months in days left */
	if (leapyear(tm->tm_year)) {
		days_in_month(FEBRUARY) = 29;
	}
	for (i = 1; day >= days_in_month(i); i++) {
		day -= days_in_month(i);
	}
	days_in_month(FEBRUARY) = 28;
	tm->tm_mon = i;

	/* Days are what is left over (+1) from all that. *//*计算当前日期*/
	tm->tm_mday = day + 1;

	/*
	 * Determine the day of week
	 */
	GregorianDay(tm);
	
}


TM_RTC_Result_t TM_RTC_SetDateTime(TM_RTC_t* data) {

	RTC_WaitForLastTask();
	GregorianDay(data);	
	RTC_SetCounter(mktimev(data));
	RTC_WaitForLastTask();
	
	if (TM_RTC_Status != RTC_STATUS_ZERO) {
		/* Write backup registers */
		BKP_WriteBackupRegister(RTC_STATUS_REG, RTC_STATUS_TIME_OK);
	}
	
	/* Return OK */
	return TM_RTC_Result_Ok;
}

TM_RTC_Result_t TM_RTC_SetDateTimeString(char* str) {
	TM_RTC_t tmp;
	uint8_t i = 0;
	
	/* Get date */
	tmp.tm_mday = 0;
	while (TM_RTC_CHARISNUM(*(str + i))) {
		tmp.tm_mday = tmp.tm_mday * 10 + TM_RTC_CHAR2NUM(*(str + i));
		i++;
	}
	i++;
	
	/* Get month */
	tmp.tm_mon = 0;
	while (TM_RTC_CHARISNUM(*(str + i))) {
		tmp.tm_mon = tmp.tm_mon * 10 + TM_RTC_CHAR2NUM(*(str + i));
		i++;
	}
	i++;
	
	/* Get year */
	tmp.tm_year = 0;
	while (TM_RTC_CHARISNUM(*(str + i))) {
		tmp.tm_year = tmp.tm_year * 10 + TM_RTC_CHAR2NUM(*(str + i));
		i++;
	}
	i++;
		
	/* Get hours */
	tmp.tm_hour = 0;
	while (TM_RTC_CHARISNUM(*(str + i))) {
		tmp.tm_hour = tmp.tm_hour * 10 + TM_RTC_CHAR2NUM(*(str + i));
		i++;
	}
	i++;
	
	/* Get minutes */
	tmp.tm_min = 0;
	while (TM_RTC_CHARISNUM(*(str + i))) {
		tmp.tm_min = tmp.tm_min * 10 + TM_RTC_CHAR2NUM(*(str + i));
		i++;
	}
	i++;
	
	/* Get seconds */
	tmp.tm_sec = 0;
	while (TM_RTC_CHARISNUM(*(str + i))) {
		tmp.tm_sec = tmp.tm_sec * 10 + TM_RTC_CHAR2NUM(*(str + i));
		i++;
	}
	i++;
	
	/* Return status from set date time function */
	return TM_RTC_SetDateTime(&tmp);
}

void TM_RTC_GetDateTime(TM_RTC_t* data) {
	uint32_t unix;

	unix = RTC_GetCounter();
	to_tm(unix, data);
}

uint8_t TM_RTC_GetDaysInMonth(uint8_t month, uint8_t year) {
	/* Check input data */
	if (
		month == 0 ||
		month > 12
	) {
		/* Error */
		return 0;
	}
	
	/* Return days in month */
	return TM_RTC_Months[TM_RTC_LEAP_YEAR(2000 + year) ? 1 : 0][month - 1];
}

uint16_t TM_RTC_GetDaysInYear(uint8_t year) {
	/* Return days in year */
	return TM_RTC_DAYS_IN_YEAR(2000 + year);
}

void TM_RTC_Config(TM_RTC_ClockSource_t source) {
	if (source == TM_RTC_ClockSource_Internal) {
		/* Enable the LSI OSC */
		RCC_LSICmd(ENABLE);

		/* Wait till LSI is ready */
		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

		/* Select the RTC Clock Source */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	} else if (source == TM_RTC_ClockSource_External) {
		/* Enable the LSE OSC */
		RCC_LSEConfig(RCC_LSE_ON);

		/* Wait till LSE is ready */ 
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

		/* Select the RTC Clock Source */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	}
	
	/* Enable the RTC Clock */
	RCC_RTCCLKCmd(ENABLE);
	
	/* Wait for register synchronization */
	RTC_WaitForSynchro();

	/* Write status */
	BKP_WriteBackupRegister(RTC_STATUS_REG, RTC_STATUS_INIT_OK);
}

void TM_RTC_Interrupts(TM_RTC_Int_t int_value) {
	uint32_t int_val;
	
	/* Clear pending bit */
	EXTI_ClearITPendingBit(EXTI_Line17);

	/* NVIC init for RTC */
	NVIC_InitStruct.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = RTC_PRIORITY;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = RTC_WAKEUP_SUBPRIORITY;
	
	/* RTC connected to EXTI_Line22 */
	EXTI_InitStruct.EXTI_Line = EXTI_Line17;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	
	if (int_value == TM_RTC_Int_Disable) {
		/* Disable RTC interrupt flag */
		RTC_ITConfig(RTC_IT_SEC, DISABLE);
		
		/* Disable NVIC */
		NVIC_InitStruct.NVIC_IRQChannelCmd = DISABLE;
		NVIC_Init(&NVIC_InitStruct); 
		/* Disable EXT1 interrupt */
		EXTI_InitStruct.EXTI_LineCmd = DISABLE;
		EXTI_Init(&EXTI_InitStruct);
	} else {
		/* Enable NVIC */
		NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStruct); 
		/* Enable EXT1 interrupt */
		EXTI_InitStruct.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStruct);


		if (int_value == TM_RTC_Int_30s) {
			int_val = 0xFFFFF;		/* 30 seconds */
		} else if (int_value == TM_RTC_Int_15s) {
			int_val = 0x7FFFF;		/* 15 seconds */
		} else if (int_value == TM_RTC_Int_10s) {
			int_val = 0x3FFFF;		/* 10 seconds */
		} else if (int_value == TM_RTC_Int_5s) {
			int_val = 0x1FFFF;		/* 5 seconds */
		} else if (int_value == TM_RTC_Int_2s) {
			int_val = 0xFFFF;		/* 2 seconds */
		} else if (int_value == TM_RTC_Int_1s) {
			int_val = 0x7FFF;		/* 1 second */
		} else if (int_value == TM_RTC_Int_500ms) {
			int_val = 0x7FFF >> 1;		/* 500 ms */
		} else if (int_value == TM_RTC_Int_250ms) {
			int_val = 0x7FFF >> 2;		/* 250 ms */
		} else if (int_value == TM_RTC_Int_125ms) {
			int_val = 0x7FFF >> 3;		/* 125 ms */
		}		
		/* Wait for RTC APB registers synchronisation */
		RTC_WaitForSynchro();
		/* Set RTC wakeup counter */
		RTC_SetPrescaler(int_val);
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
		/* Enable wakeup interrupt */
	 	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	 	RTC_WaitForLastTask();		
	}
}

void TM_RTC_SetAlarm(TM_RTC_Alarm_t Alarm, TM_RTC_AlarmTime_t* DataTime) {
	uint32_t unix;
	
	/* Disable alarm first */
	TM_RTC_DisableAlarm(Alarm);
	
	unix = ((DataTime->day * 24 + 
			DataTime->hours) * 60 + DataTime->minutes) * 60
			+ DataTime->seconds;
	
	switch (Alarm) {
		case TM_RTC_Alarm_A:		
		case TM_RTC_Alarm_B:
			/* Configure the RTC Alarm A */
			RTC_SetAlarm(RTC_GetCounter() + unix);
    		RTC_WaitForLastTask();					
			/* Enable Alarm A interrupt */
			RTC_ITConfig(RTC_IT_ALR, ENABLE);
		
			/* Clear Alarm A pending bit */
			RTC_ClearFlag(RTC_IT_ALR);
			break;
		default:
			break;
	}
}

void TM_RTC_DisableAlarm(TM_RTC_Alarm_t Alarm) {
	/* we only support one alarm on stm32F103 board */
	switch (Alarm) {
		case TM_RTC_Alarm_A:
		case TM_RTC_Alarm_B:		
		    /* Wait till RTC Second event occurs */
		    RTC_ClearFlag(RTC_FLAG_SEC);
    		while(RTC_GetFlagStatus(RTC_FLAG_SEC) == RESET);
			/* Disable Alarm B interrupt */
			RTC_ITConfig(RTC_IT_ALR, DISABLE);
		
			/* Clear Alarm B pending bit */
			RTC_ClearFlag(RTC_IT_ALR);
			break;
		default:
			break;
	}
	
	/* Clear RTC Alarm pending bit */
	EXTI_ClearITPendingBit(EXTI_Line17);
	
	/* Configure EXTI 17 as interrupt */
	EXTI_InitStruct.EXTI_Line = EXTI_Line17;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	
	/* Initialite Alarm EXTI interrupt */
	EXTI_Init(&EXTI_InitStruct);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	/* Configure the RTC Alarm Interrupt */
	NVIC_InitStruct.NVIC_IRQChannel = RTCAlarm_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = RTC_PRIORITY;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = RTC_ALARM_SUBPRIORITY;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	
	/* Initialize RTC Alarm Interrupt */
	NVIC_Init(&NVIC_InitStruct);
}

void RTC_IRQHandler(void) {
	if (RTC_GetITStatus(RTC_FLAG_SEC) != RESET) {
		/* Clear interrupt flags */
		RTC_ClearITPendingBit(RTC_FLAG_SEC);
		EXTI_ClearITPendingBit(EXTI_Line17);
		
		/* Call user function */
		TM_RTC_RequestHandler();
	}
}

void RTCAlarm_IRQHandler(void) {
	if (RTC_GetITStatus(RTC_IT_ALR) != RESET) {
		/* Clear RTC Alarm A interrupt flag */
		RTC_ClearITPendingBit(RTC_IT_ALR);
		
		/* Call user function for Alarm A */
		TM_RTC_AlarmAHandler();
	}
		
	/* Clear EXTI line 17 bit */
	EXTI_ClearITPendingBit(EXTI_Line17);
}
