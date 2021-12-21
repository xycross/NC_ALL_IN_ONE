#include "HalTimer.h"
#include "commOpticSwitch.h"
#include "globalDefine.h"

#include "smux_main.h"

extern u8 op_mode;

void mysettimer(int nCh, U32 cntval, U16 prescale, BOOL on_off );
void System_Timer_ISR(void);


void Timer_Init(void) 
{

#if 0
	if(op_mode == OP_SMUX)
	{
	// 타이머 초기화
		settimer_us(SCADA_COM_TIMER,SCADA_COM_TIMER_us);		// Timer0 : 1ms ,, SCADA COM
		setinterrupt(INTNUM_TIMER0,_SDATA_DATA_RECEIVE_TIMER_0_ISR_);	
		
		settimer_us(REP_DET_COM_TIMER,REP_DET_COM_TIMER_us);	// Timer1 : 100us ,, HIC COM
		setinterrupt(INTNUM_TIMER1,_REP_DET_DATA_RECEIVE_TIMER_1_ISR_);	

		//settimer(HIC_COM_DELAY_TIMER,HIC_COM_DELAY_TIMER_ms);		// Timer2 : 1ms ,, HIC COM delay
		//setinterrupt(INTNUM_TIMER2,_HIC_COM_DELAY_TIMER_2_ISR_);	

		settimer(MUX_RUN_TIMER,MUX_RUN_TIMER_ms);		//
		setinterrupt(INTNUM_TIMER3,_MUX_RUN_TIMER_3_ISR_);			
	}
	else
	{
		settimer(SYSTEM_TIMER, SYSTEM_TIMER_ms);	// System Timer 10ms	4
		setinterrupt(INTNUM_TIMER2, System_Timer_ISR);		
	}
#endif

	if(op_mode != OP_SMUX)
	{
		settimer(SYSTEM_TIMER, SYSTEM_TIMER_ms);	// System Timer 10ms	4
		setinterrupt(INTNUM_TIMER2, System_Timer_ISR);		
	}
}

void System_Timer_ISR(void) 
{
	stoptimer(SYSTEM_TIMER);

	ledControl();
	
	settimer(SYSTEM_TIMER, SYSTEM_TIMER_ms);	
}


void Smux_System_Timer_ISR(void)
{
	stoptimer(SYSTEM_TIMER);

	smux_ledControl();
	
	settimer(SYSTEM_TIMER, SYSTEM_TIMER_ms);		
}

