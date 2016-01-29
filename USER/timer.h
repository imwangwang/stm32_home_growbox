#ifndef __TIMER_H__
#define __TIMER_H__

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_rtc.h"


/* Timer record for each timer */
typedef struct Timer
{
	unsigned short TimerId;
	unsigned short Period;
}tTimer;

#define TIMER_1	            (unsigned short)0x0001
#define TIMER_2	            (unsigned short)0x0002
#define TIMER_3	            (unsigned short)0x0004
#define TIMER_4				(unsigned short)0x0008
#define TIMER_5				(unsigned short)0x0010
#define TIMER_6				(unsigned short)0x0020
#define TIMER_7				(unsigned short)0x0040
#define TIMER_8				(unsigned short)0x0080
#define TIMER_9				(unsigned short)0x0100
#define TIMER_10			(unsigned short)0x0200
#define TIMER_11			(unsigned short)0x0400
#define TIMER_12				(unsigned short)0x0800
#define TIMER_13				(unsigned short)0x1000
#define TIMER_14				(unsigned short)0x2000
#define TIMER_15				(unsigned short)0x4000
#define TIMER_16	            (unsigned short)0x8000

extern __weak void Timer1Function(void);
extern __weak void Timer2Function(void);
extern __weak void Timer3Function(void);
extern __weak void Timer4Function(void);
extern __weak void Timer5Function(void);
extern __weak void Timer6Function(void);
extern __weak void Timer7Function(void);
extern __weak void Timer8Function(void);
extern __weak void Timer9Function(void);
extern __weak void Timer10Function(void);
extern __weak void Timer11Function(void);
extern __weak void Timer12Function(void);
extern __weak void Timer13Function(void);
extern __weak void Timer14Function(void);
extern __weak void Timer15Function(void);
extern __weak void Timer6Function(void);
extern int AddTimer(unsigned short TimerId, unsigned short Period);
extern void CheckTimerEvent(void);
extern void InitTimerList(void);
#endif
