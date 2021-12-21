#ifndef __MY_TIMER_H__
#define __MY_TIMER_H__


#define SYSTEM_TIMER 	2

#define MAIN_TIMER 		0

#define SCADA_TIMER		0

#define 		LOOP1_TIMER	4
#define 		LOOP2_TIMER	5
#define 		LOOP3_TIMER	6
#define 		LOOP4_TIMER	7



void mysettimer(int, U32, U16, BOOL);


BOOL monitor_time_Set(U32 ms);

void monitor_time_run(void);


#endif

