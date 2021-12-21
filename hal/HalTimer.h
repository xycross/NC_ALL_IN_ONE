#ifndef __HALTIMER_H__
#define __HALTIMER_H__

#include "Hal.h"


#define SYSTEM_TIMER 	2
#define SYSTEM_TIMER_ms	50


void Timer_Init(void);
void mysettimer(int, U32, U16, BOOL);

#endif


