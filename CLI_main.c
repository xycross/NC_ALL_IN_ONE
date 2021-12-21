#include "cli_main.h"


#include "typedef.h"

#include "System_Info.h"

#include "Definition.h"
#include "cli_n.h"
#include "Converter.h"
#include "CLI_Repeater_Comm.h"
//#include "Detector_Comm.h"
//#include "Differ_Process.h"

#include "myUart.h"




extern const unsigned char Code_Confirm_Table[256];

 
 
 

extern u8  m_RunLEDCount;


extern bool m_RunLEDState;


unsigned char opt_rx_state, opt2_rx_state;

unsigned char opt_rx_data[20], opt2_rx_data[10];
//unsigned char opt_rx_count;
unsigned char opt_rx_check, opt2_rx_check;

unsigned char monitor_command;


extern unsigned char Parsing_Save, Parsing_Process;


extern unsigned char cli_cmd;
unsigned char m_rx_data;

unsigned char response = 0;

extern u8 cli_which;

extern int debugch;

int CLI_main(void)
{	
	//지역 변수 선언
	int i, j;
	TS_RotarySwitch rotarySwitchValue;


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
	CLI_mysettimer(SYSTEM_TIMER, 149940, 32, TRUE);	// System Timer 100ms	4
	setinterrupt(INTNUM_TIMER0, Main_Timer_ISR);
//	settimer(0, 500);
//	stoptimer(0);
	
	cli_which = DEF_CLI_WHICH_COMM_MON_CLI;
//	rotarySwitchValue = RotaySwitchDetect();
	
	debugch = 0;
	TEST_CLI_myUartConfig(0, 115200, 8);
//	myUartConfig(4, 115200, 8);
	TEST_CLI_myUartConfig(6, 115200, 1);
	TEST_CLI_myUartConfig(7, 115200, 1);
	
//	UartConfig(MUX_RS232_CH_0, DEFAULT_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);
//	UartConfig(MUX_OPTIC_CH_4, DEFAULT_BAUDRATE, DATABITS_8,STOPBITS_1, UART_PARNONE);
//	UartConfig(MUX_OPTIC_CH_5, DEFAULT_BAUDRATE, DATABITS_8,STOPBITS_1, UART_PARNONE);
//	UartConfig(MUX_OPTIC_CH_6, DEFAULT_BAUDRATE, DATABITS_8,STOPBITS_1, UART_PARNONE);
//	UartConfig(MUX_OPTIC_CH_7, DEFAULT_BAUDRATE, DATABITS_8,STOPBITS_1, UART_PARNONE);
	
	debugstring("Packet Monitoring V1.0 \r\n");
	delayms(100);

	dcache_invalidate_way();
	
	Repeater_Initialize();

//	Repeater_Initialize();
//	Detector_Initialize();


	debugprintf("System Started...Compiled %s %s\r\n", __DATE__, __TIME__);

	debugprintf("System Clock = %ld\r\n", GetAHBclock());
	debugprintf("APB_Clock = %ld\r\n", GetAPBclock());
	// 왓츠독 설정
	Watch_Dog(2000,SET);



	while(1)
	{
		cli_loop(1);

		if(old_cli_cmd >= E_POLL_REGULAR)
		{
			if(POLLING_Process())
			{
			}
			else
			{
				View_Comm_Count();
				old_cli_cmd = 0;
 			}
		}
		else
		{
			old_cli_cmd = cli_cmd;
//			rotarySwitchValue = RotaySwitchDetect();
//			if(rotarySwitchValue.RotarySwitchValue != 991)
//			{
//				*R_PMCTRLEN = PMCTRL_RSTCTRL_EN;
//				*R_RSTCTRL = RSTCTRL_RESET;	
//			}
 		}


	
		Watch_Dog(2000,CLR);
	}

	
	return 0;
}



int BuadRateSelector(TS_RotarySwitch _rotaySwitchValue, int _placeValue, int _defaultBaudRate)
{
	switch(_placeValue)
	{
		case 100 :
		{
			switch(_rotaySwitchValue.hundreds)
			{	
				case 0 : {return _defaultBaudRate;}
				case 1 : {return 9600;}
				case 2 : {return 19200;}
				case 3 : {return 28800;}
				case 4 : {return 38400;}
				case 5 : {return 57600;}
				case 6 : {return 115200;}
				case 7 : {return 230400;}
				case 8 : {return 460800;}
				case 9 : {return 921600;}
			}
			break;
		}
		case 10 :
		{
			switch(_rotaySwitchValue.tens)
			{
				case 0 : {return _defaultBaudRate;}
				case 1 : {return 9600;}
				case 2 : {return 19200;}
				case 3 : {return 28800;}
				case 4 : {return 38400;}
				case 5 : {return 57600;}
				case 6 : {return 115200;}
				case 7 : {return 230400;}
				case 8 : {return 460800;}
				case 9 : {return 921600;}
			}
			break;
		}
		case 1 :
		{
			switch(_rotaySwitchValue.units)
			{
				case 0 : {return _defaultBaudRate;}
				case 1 : {return 9600;}
				case 2 : {return 19200;}
				case 3 : {return 28800;}
				case 4 : {return 38400;}
				case 5 : {return 57600;}
				case 6 : {return 115200;}
				case 7 : {return 230400;}
				case 8 : {return 460800;}
				case 9 : {return 921600;}
			}
			break;
		}		
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

#endif


void CLI_System_Timer_ISR(void)
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

void Watch_Dog(int _ms, int _status)
{
	U32 clock, wdtcnt;

	clock = GetAPBclock();
	
	wdtcnt = (_ms /1000) * clock;


	*R_WDCNT = wdtcnt;			// Watch Dog Reset Counter Value
	if(_status) *R_WDTCTRL = 1;				// Watch Dog Enable
}
	
	
