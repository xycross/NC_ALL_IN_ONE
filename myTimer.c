#include "cantus.h"
#include "myTimer.h"

#include "Base_Function.h"



BOOL monitor_time_Set(U32 ms)
{
	int nCh = 5;
	U32 r_tpcon = 0x80021000 + 0x20*nCh;
	U32 r_tmcon = 0x80021004 + 0x20*nCh;
	U32 r_tmcnt = 0x80021008 + 0x20*nCh;
	int intnum = nCh*4+1;
	int conval;
	
	long long int nanosecpertick = 1000000000/(GetAPBclock()/2);
	long long int usec = (long long int)ms*1000;
	long long int cnt = ((long long int)usec*1000) / nanosecpertick;
	if(cnt > 0xffffffff) // ms is too big to set, adjust PRESACLE.
		return FALSE;
	//reset
	*(volatile U32*)r_tpcon = 1<<1;
	*(volatile U32*)r_tpcon = 0;

	conval = 0;

#if 0	
#if PRESCALE==2
	conval = 0;
#elif PRESCALE==8
	conval = 1<<1;
#elif PRESCALE==32
	conval = 2<<1;
#elif PRESCALE==128
	conval = 3<<1;
#elif PRESCALE==512
	conval = 4<<1;
#elif PRESCALE==2048
	conval = 5<<1;
#elif PRESCALE==8192
	conval = 6<<1;
#elif PRESCALE==32768
	conval = 7<<1;
#else
#error "Invalid PRESCALE value"
#endif
#endif	
//	*(volatile U32*)r_tmcnt = cnt;
	*(volatile U32*)r_tmcnt = 0xFFFFFFFF;

//	debugprintf("cnt = %d\r\n", cnt);
	*(volatile U32*)r_tmcon = conval;

//	debugprintf("r_tmcon = %08X\r\n", conval);
//	debugprintf("tmr value = %08X\r\n", *((volatile unsigned int*)0x800210e8));

	EnableInterrupt(intnum,TRUE);
	return TRUE;
}

void monitor_time_run(void)
{
//	*(volatile U32*)r_tmcon = conval;

	// Timer 5 Use

//	*((volatile unsigned int*)0x800210e4) += 1;
	*((volatile unsigned int*)0x800210A4) += 1;


}



void mysettimer(int nCh, U32 cntval, U16 prescale, BOOL on_off )
{
	if(nCh > 7) return;
	
	U32 r_tpcon = 0x80021000 + 0x20*nCh;
	U32 r_tmcon = 0x80021004 + 0x20*nCh;
	U32 r_tmcnt = 0x80021008 + 0x20*nCh;
	int intnum = nCh*4+1;
	int conval;
	
	//reset
	*(volatile U32*)r_tpcon = 1<<1;
	*(volatile U32*)r_tpcon = 0;

	switch(prescale)
	{
		case 2:
			conval = 0;
			break;

		case 8:
			conval = 1<<1;
			break;

		case 32:
			conval = 2<<1;
			break;

		case 128:
			conval = 3<<1;
			break;

		case 512:
			conval = 4<<1;
			break;

		case 2048:
			conval = 5<<1;
			break;

		case 8192:
			conval = 6<<1;
			break;

		case 32768:
			conval = 7<<1;
			break;

		default:
			debugstring("Unknown Prescaler\r\n");
	}
	
	*(volatile U32*)r_tmcnt = cntval;
	if(on_off)
	{
		*(volatile U32*)r_tmcon = conval | 1;
	}
	else
	{
		*(volatile U32*)r_tmcon = conval;
	}

	switch(nCh)
	{
		case 0:
//			setinterrupt(intnum, SYSTEM_Timer_ISR);
//			setinterrupt(intnum, Scada_Timer_ISR);
			break;

		case 1:
//			setinterrupt(intnum, Key_Timer_ISR);
			break;

		case 2:
			setinterrupt(intnum, System_Timer_ISR);
			break;

		case 3:
//			setinterrupt(intnum, PAETC_Timer_ISR);
			break;

		case 4:
//			setinterrupt(intnum, Loop1_Timer_ISR);
			break;

		case 5:
//			setinterrupt(intnum, LOOP2_Timer_ISR);
			break;

		case 6:
//			setinterrupt(intnum, LOOP3_Timer_ISR);
			break;
			
		case 7:
//			setinterrupt(intnum, LOOP4_Timer_ISR);
			break;
			
	}
	
	EnableInterrupt(intnum,TRUE);

}








