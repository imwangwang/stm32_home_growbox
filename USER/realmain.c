#include "stm32f10x.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32f4_delay.h"
#include "resource.h"
#include "stw_system_timer.h"
#include "tm_stm32f4_rtc.h"
#include <stdio.h>

extern int send_a_cmd(int cmd);
extern int get_a_cmd(int *cmd);

void TM_RTC_RequestHandler() {
    stw_system_timer_tick();
}

static void humd_timer_handler(void)
{
	const struct resource *res;
	uint32_t val;
	static uint32_t avg_count = 0;
	static uint32_t avg_val = 0;

	res = get_res_from_array(RES_SOIL_HUMDITY);

	if (res) {
		val = res->get(NULL);
		avg_val += val;
		avg_count++;
		}

	if (avg_count % 60 == 0){
		avg_val = avg_val / 60;
		if (avg_val < 2700) {
			printf("average humd:%d\n", avg_val);
			send_a_cmd(CMD_START_PUMPA);
			}
		avg_count = 0;
		avg_val = 0;
		}
	
    return;
}

void tmp_timer_handler(void)
{
	const struct resource *res;
	float val;
	static int last_routine = 0;
	static int idx = 0;
	int current_routine = 0;
	float temp[] = {
		27.0,
		22.0
		};

	res = get_res_from_array(RES_TMP);

	if (res) {
		if (res->get(&val) == 0){
			int value = 1;
			res = get_res_from_array(RES_REAR_WARM_LIGHT);
			/* get valid temp */
			if (val > temp[idx]) {
				/* cool fan */
				value = 0;
				current_routine = 1;
				if (val > 50.0) {
					/* fail safe */
					const struct resource *light_res = get_res_from_array(RES_REAR_GROW_LIGHT);
					int local_val = 0;

					if (light_res->get(NULL)) {
						light_res->set(&local_val); // shut it down 
						}
					;

					}
				}
			else {
				/* check if light is ON */
				const struct resource *light_res = get_res_from_array(RES_REAR_GROW_LIGHT);
				int local_val = 0;
				current_routine = 0;
				if (light_res->get(NULL)) {
					light_res->set(&local_val); // shut it down 
					}
				;
				}

			if (current_routine != last_routine) {
				last_routine = current_routine;
				idx++;
				idx = idx % 2;
				}
			
			res->set(&value);
			}
		}
	
	return;

}

static void light_repeating_timer (stw_tmr_t *tmr, void *parm) 
{
	send_a_cmd(CMD_LIGHT_PERIOD_TIMER);
} 

static void msoilhumd_repeating_timer (stw_tmr_t *tmr, void *parm) 
{
	send_a_cmd(CMD_HUMDITY_PERIOD_TIMER);
} 

static void tmp_repeating_timer (stw_tmr_t *tmr, void *parm) 
{
	send_a_cmd(CMD_TMP_PERIOD_TIMER);
}

static void pump_timer(stw_tmr_t *tmr, void *parm)
{
	struct resource *res = (struct resource *)parm;

	if (res->id_type == RES_PUMPA) {
		send_a_cmd(CMD_PUMPA_STOP);
		}
}

static void handle_cmd(unsigned char cmd)
{
	const struct resource *res;
	uint32_t val = 1;
	static stw_tmr_t pumpA_timer;

	printf("handle cmd %d\n", cmd);
	switch (cmd) {
		case CMD_START_PUMPA:
			stw_timer_prepare(&pumpA_timer);
			res = get_res_from_array(RES_PUMPA);
			if (res) {
				res->set((void *)&val);
				}
			stw_system_timer_start(&pumpA_timer, 
			                        5000,
			                        0,
			                        pump_timer, 
			                        (void *)res);

			break;
		case CMD_REAR_GROWLIGHT_OFF:
			val = 0;
		case CMD_REAR_GROWLIGHT_ON:
			res = get_res_from_array(RES_REAR_GROW_LIGHT);
			if (res) {
				const struct resource *res_warmlight = get_res_from_array(RES_REAR_WARM_LIGHT);
				if (!res_warmlight->get(NULL))
					res->set((void *)&val);
				}
			break;
		case CMD_LIGHT_PERIOD_TIMER:
			res = get_res_from_array(RES_LIGHT);

			if (res) {
				val = res->get(NULL);
				}

			if (val < 6000) {
				send_a_cmd(CMD_REAR_GROWLIGHT_ON);
				}
			else {
				send_a_cmd(CMD_REAR_GROWLIGHT_OFF);
				}
			break;
		case CMD_HUMDITY_PERIOD_TIMER:
			humd_timer_handler();
			break;
		case CMD_TMP_PERIOD_TIMER:
			tmp_timer_handler();
			break;
		case CMD_PUMPA_STOP:
			res = get_res_from_array(RES_PUMPA);
			val = 0;
			if (res)
				res->set(&val);
			break;
		default:
			break;
	}
}


int main(void) {
	/* Struct for RTC tim */
	TM_RTC_Time_t Time;
	uint32_t milliseconds;
	static stw_tmr_t msoilhumd_tmr; 
	static stw_tmr_t tmp_tmr;
	static stw_tmr_t light_tmr;

	/* Initialize system */
	SystemInit();
	
	/* Initialize delay */
	TM_DELAY_Init();
	
#define STW_NUMBER_BUCKETS     ( 512 ) /* useless */
#define STW_RESOLUTION         ( 1000 ) 

	
    stw_system_timer_create(STW_NUMBER_BUCKETS, 
                            STW_RESOLUTION, 
                            "Timer Wheel"); 

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
	milliseconds = 20000;
    stw_timer_prepare(&light_tmr);  
    stw_system_timer_start(&light_tmr, 
                            milliseconds,
                            milliseconds,
                            light_repeating_timer, 
                            NULL);
	

    milliseconds = 3000; // every 60 seconds 
    stw_timer_prepare(&msoilhumd_tmr);  
    stw_system_timer_start(&msoilhumd_tmr, 
                            milliseconds,
                            milliseconds,
                            msoilhumd_repeating_timer, 
                            NULL);


	milliseconds = 5000; // every 10 secs
	stw_timer_prepare(&tmp_tmr);
	stw_system_timer_start(&tmp_tmr,
                            milliseconds,
                            milliseconds,
                            tmp_repeating_timer, 
                            NULL);
		
	while (1) {
		int cmd;
		if (get_a_cmd(&cmd) != -1)
			handle_cmd(cmd);
		}

}




