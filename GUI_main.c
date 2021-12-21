#include "main.h"


#include "typedef.h"

#include "System_Info.h"

#include "Definition.h"
#include "cli_n.h"
#include "cli_main.h"
#include "Converter.h"
//#include "Repeater_Comm.h"
//#include "Detector_Comm.h"
//#include "Differ_Process.h"

#include "GUI_Comm.h"

#include "myUart.h"
#include "CLI_Repeater_Comm.h"

#include "GUI_main.h"

extern const unsigned char Code_Confirm_Table[256];


extern u8  m_RunLEDCount;


extern bool m_RunLEDState;


//unsigned char opt_rx_state, opt2_rx_state;

//unsigned char opt_rx_data[20], opt2_rx_data[10];
//unsigned char opt_rx_count;
//unsigned char opt_rx_check, opt2_rx_check;

//unsigned char monitor_command;


extern unsigned char Parsing_Save, Parsing_Process;


extern unsigned char cli_cmd;

//unsigned char response = 0;

unsigned char *ptr;


int GUI_main(void)
{	
	//지역 변수 선언
	int i, j;
	TS_RotarySwitch rotarySwitchValue;
	unsigned short addr, daddr, p_data;

	unsigned char old_cli_cmd = 0;
	
	//전역 변수 초기화
	m_RunLEDCount		= 0 ;
	m_RunLEDState 		= false;
		
	
	InitInterrupt();
	
	// PLL 설정 -- 95961600
	*R_PMCTRLEN = 	PMCTRL_PLLCTRL_EN;
	*R_PMCTRLEN	=	PMCTRL_CLKSEL_EN;	
	*R_PLLCTRL	=	PLLCTRL_ENABLE;
	*R_CKR		=	CKR_MCS_PLL;
		
	// 포트 초기화
	*R_PAF5 	= 	0xFFFF;		// Port + SD
	*R_P5oDIR	= 	(1<<7);
	*R_P5oHIGH	=	(1<<7);
	*R_PAF4 = 0x0000;		// SD + UART 0, 3 USE FOR MUX & Debug	
	*R_PAF1 = 0x0000;		// UART 4, 5, 6, 7 USE FOR OPTICAL CABLE

	*R_PAF3		= 0xFFFF;		// Port Mode
	*R_P3PUEN	= 0xF0;			// Full Up
	*R_P3PUDIS	= 0x0F;			// Full Up dis
	*R_P3oDIR	= 0xF0;			// SWSEL1, 2, 3
	*R_P3iDIR	= 0x0F;			// ASW0, 1, 2, 3
	
	
	
	// 타이머, Uart Config
	GUI_settimer(SYSTEM_TIMER, 149940, 32, TRUE);	// System Timer 100ms	4
	setinterrupt(INTNUM_TIMER0, Main_Timer_ISR);

	setinterrupt(INTNUM_TIMER6, GUI_Comm_Timer_ISR);
//	settimer(0, 500);
//	stoptimer(0);
	

//	rotarySwitchValue. = RotaySwitchDetect();


	ptr = malloc(sizeof(struct _TX_PACKET) * 1000);
	
	
	GUI_myUartConfig(0, 115200, 8);
	//myUartConfig(4, 115200, 8);
	//myUartConfig(6, 115200, 1);
	GUI_myUartConfig(7, 115200, 1);
	
//	UartConfig(MUX_RS232_CH_0, DEFAULT_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);
//	UartConfig(MUX_OPTIC_CH_4, DEFAULT_BAUDRATE, DATABITS_8,STOPBITS_1, UART_PARNONE);
//	UartConfig(MUX_OPTIC_CH_5, DEFAULT_BAUDRATE, DATABITS_8,STOPBITS_1, UART_PARNONE);
//	UartConfig(MUX_OPTIC_CH_6, DEFAULT_BAUDRATE, DATABITS_8,STOPBITS_1, UART_PARNONE);
//	UartConfig(MUX_OPTIC_CH_7, DEFAULT_BAUDRATE, DATABITS_8,STOPBITS_1, UART_PARNONE);
	
	debugstring("Packet Monitoring V1.0 \r\n");
	delayms(100);

	dcache_invalidate_way();
	
	GUI_Repeater_Initialize();

//	Repeater_Initialize();
//	Detector_Initialize();


	debugprintf("System Started...Compiled %s %s\r\n", __DATE__, __TIME__);

	debugprintf("System Clock = %ld\r\n", GetAHBclock());
	debugprintf("APB_Clock = %ld\r\n", GetAPBclock());
	// 왓츠독 설정
	GUI_Watch_Dog(2000,SET);

	while(1)
	{
		cli_loop(1);
		GUI_Procedure();
		GUI_Watch_Dog(2000, CLR);
	}



	
	return 0;
}


#if 0
TS_RotarySwitch RotaySwitchDetect()
{
	/*//////////////////////////////////////
	// 	RotaySW 0 = b0000 - 0
	// 	RotaySW 1 = b1000 - 8
	// 	RotaySW 2 = b0001 - 1
	// 	RotaySW 3 = b1001 - 9
	// 	RotaySW 4 = b0010 - 2
	// 	RotaySW 5 = b1010 - 10
	// 	RotaySW 6 = b0011 - 3
	// 	RotaySW 7 = b1011 - 11
	// 	RotaySW 8 = b0100 - 4
	// 	RotaySW 9 = b1100 - 12
	/////////////////////////////////////*/
		
	TS_RotarySwitch rotarySwitch ;
	
	*R_P3oHIGH	=	(1<<6);
	delayms(1);
	rotarySwitch.hundreds	=	RotarySwitchMapping(*R_P3iLEV & 0x0f);
	*R_P3oLOW	=	(1<<6);

	*R_P3oHIGH	=	(1<<5);
	delayms(1);
	rotarySwitch.tens		=	RotarySwitchMapping(*R_P3iLEV & 0x0f);
	*R_P3oLOW	=	(1<<5);

	*R_P3oHIGH	=	(1<<4);
	delayms(1);
	rotarySwitch.units		=	RotarySwitchMapping(*R_P3iLEV & 0x0f);
	*R_P3oLOW	=	(1<<4);
	
	rotarySwitch.RotarySwitchValue = (rotarySwitch.hundreds*100) + (rotarySwitch.tens*10) + rotarySwitch.units;
	
	return rotarySwitch;
	
}
#endif



void GUI_settimer(int nCh, U32 cntval, U16 prescale, BOOL on_off )
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
			setinterrupt(intnum, GUI_System_Timer_ISR);
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


int RotarySwitchMapping(int keydata)
{
	switch(keydata)
	{
		case 0	:{return 0;}
		case 8	:{return 1;}
		case 1	:{return 2;}
		case 9	:{return 3;}
		case 2	:{return 4;}
		case 10 :{return 5;}
		case 3	:{return 6;}
		case 11 :{return 7;}
		case 4	:{return 8;}
		case 12	:{return 9;}
		default	:{return 0;}
	}
}

void GUI_System_Timer_ISR(void)
{
	if(++m_RunLEDCount >= 2)
	{
		m_RunLEDCount= 0;
 
		if(m_RunLEDState)
		{
			m_RunLEDState = false;
			*R_P5oLOW = (1<<7);
		}
		else
		{
			m_RunLEDState = true;
			*R_P5oHIGH = (1<<7);
		} 				
	}

}



/*

int RotaySwitchDetect()
{
	///////////////////////////////////////
	// 	RotaySW 0 = b0000 - 0
	// 	RotaySW 1 = b1000 - 8
	// 	RotaySW 2 = b0001 - 1
	// 	RotaySW 3 = b1001 - 9
	// 	RotaySW 4 = b0010 - 2
	// 	RotaySW 5 = b1010 - 10
	// 	RotaySW 6 = b0011 - 3
	// 	RotaySW 7 = b1011 - 11
	// 	RotaySW 8 = b0100 - 4
	// 	RotaySW 9 = b1100 - 12
	//////////////////////////////////////
		
	int il, sib, bak;
	*R_P3oHIGH	=	(1<<6);
	delayms(1);
	bak			=	RotarySwitchMapping(*R_P3iLEV & 0x0f);
	*R_P3oLOW	=	(1<<6);

	*R_P3oHIGH	=	(1<<5);
	delayms(1);
	sib			=	RotarySwitchMapping(*R_P3iLEV & 0x0f);
	*R_P3oLOW	=	(1<<5);

	*R_P3oHIGH	=	(1<<4);
	delayms(1);
	il			=	RotarySwitchMapping(*R_P3iLEV & 0x0f);
	*R_P3oLOW	=	(1<<4);
	
	return ((bak*100)+(sib*10)+il);			
}
*/

void GUI_Watch_Dog(int _ms, int _status)
{
	U32 clock, wdtcnt;

	clock = GetAPBclock();
	
	wdtcnt = (_ms /1000) * clock;


	*R_WDCNT = wdtcnt;			// Watch Dog Reset Counter Value
	if(_status) *R_WDTCTRL = 1;				// Watch Dog Enable
}
	
	
