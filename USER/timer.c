#include "timer.h"

#define NULL 0
static tTimer mpTimerList[16];
static uint8_t intialized = 0;

/* Global variable for reference */
static unsigned short gTimer = 0;

static int FindTimer(unsigned short TimerId)
{
	int i = 0;

	while (i < 16) {
		if (mpTimerList[i].TimerId == TimerId)
			return i;
		}

	return -1;
}

void InitTimerList(void)
{
	int i = 0;

	if (intialized != 0) {
		return;
		}
	
	intialized = 1;
	
	while (i < 16) {
		mpTimerList[i].Period = 0;
		mpTimerList[i].TimerId = 1 << i;
		i++;
		}
}

int AddTimer(unsigned short TimerId, unsigned short Period)
{
	int i;
	bool ReturnValue = FALSE;
	
    /* Look for the timer ¨C if already exists */
	i = FindTimer(TimerId);

	if(i != -1)
    {
        /* Set the timer interval */
    	mpTimerList[i].Period = Period;
    	ReturnValue = TRUE;
    }

	return ReturnValue;
}

static void SystemTickEvent(void)
{
	int i = 0;

	while(i < 16)
    {
    	if(mpTimerList[i].Period != 0)
        {
            /* Decrement the timer period */
        	mpTimerList[i].Period--;

            /* Check if the timer has reached zero */
        	if(mpTimerList[i].Period == 0)
            {
                /* Set the corresponding bit when the timer reaches zero */
            	gTimer = gTimer | mpTimerList[i].TimerId;
            }
        }
        /* Move to the next timer in the list */
		i++;
    }
}


void CheckTimerEvent(void)
{
	unsigned short nTimer;

    /* Read the global variable gTimer and reset the value */
	__disable_irq();
	nTimer = gTimer; 
	gTimer = 0;
	__enable_irq();

    /* Check for TimerId */
	if( nTimer != 0)
    {
    	if(nTimer & TIMER_1)
        {
            /* Call Timer 1 routine */
        	Timer1Function();
        }
    	if(nTimer & TIMER_2)
        {
            /* Call Timer 2 routine */
        	Timer2Function();
        }
    }
}
