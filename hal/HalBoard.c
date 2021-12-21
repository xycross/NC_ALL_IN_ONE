#include "HalBoard.h"

#include "globalDefine.h"

void sysClockInit(void);
void Gpio_Init(void);

U16 RotarySW_Value;
U8 SW_ENUM;

extern u8 op_mode;

TS_RotarySwitchValue rotarySwitch;
extern TS_RotarySwitchValue SW;


void SystemInit(void) 
{
	sysClockInit();
	Gpio_Init();


	

	delayms(100);

	rotarySwitch = RotaySwitchDetect();
	SW = rotarySwitch;

	// 0 - 분배기	
	// 500 > RS232-to-Optic
	// other S-MUX

	if(rotarySwitch.RotarySwitchValue == 0)
	{
		op_mode = OP_DISTRIBUTION;
	}
	else if(rotarySwitch.RotarySwitchValue == 999)
	{
		op_mode = OP_OPT_BYPASS;
	}
	else if(rotarySwitch.RotarySwitchValue == 995)
	{
		op_mode = OP_OPT_TEST_GUI;
	}
	else if(rotarySwitch.RotarySwitchValue == 991)
	{
		op_mode = OP_OPT_TEST_CLI;
	}
	else if(rotarySwitch.RotarySwitchValue >= 500)
	{
		op_mode = OP_232_TO_OPTIC;
	}
	else
	{
		op_mode = OP_SMUX;
	}

	Uart_Init();
	
	InitInterrupt();		
	
	
	Timer_Init();

/*
	debugprintf("Op Mode = %d\r\n", op_mode);
		
	debugstring("D2-MUX V1.0 \r\n");
	delayms(100);
	debugprintf("AHB [%d]\r\n",GetAHBclock());
	delayms(100);
	debugprintf("APB [%d]\r\n",GetAPBclock());
	delayms(100);

	debugprintf("System Compiled at %s, %s\r\n", __DATE__, __TIME__);
*/
	statusLEDControl(5,100);

	Comm_Led_Display();
}

void Gpio_Init(void){
	// 포트 초기화
	*R_PAF5 	=	0xFFFF; 	// Port + SD
	*R_P5oDIR	=	(1<<7);
	*R_P5oHIGH	=	(1<<7);
	*R_PAF4 = 0x03C0;		// SD + UART 0, 3 USE FOR MUX & Debug	
	*R_P4oDIR	=	(1<<3);
	*R_P4oDIR	=	(1<<4);
	*R_P4oHIGH = (1<<3);
	*R_P4oHIGH = (1<<4);
	*R_PAF1 = 0x0000;		// UART 4, 5, 6, 7 USE FOR OPTICAL CABLE

	*R_PAF3 	= 0xFFFF;		// GPIO Mode
	*R_P3PUEN	= 0xF0; 		// Full Up
	*R_P3PUDIS	= 0x0F; 		// Full Up dis
	*R_P3oDIR	= 0xF0; 		// SWSEL1, 2, 3
	*R_P3iDIR	= 0x0F; 		// ASW0, 1, 2, 3

	*R_PAF0 = 0xFFFF;	// GPIO Mode
	*R_P0oDIR = 0x0F;	// P0.[0:3] Output Mode /RXD[0:3]_LED
	*R_P0oHIGH = 0x0F;	// RXD[0:3]_LED OFF

	*R_PAF2 = 0xFFFF;	// GPIO Mode
	*R_P2oDIR = 0x0F;	// P2.[0:3] Output Mode /TXD[0:3]_LED
	*R_P2oHIGH = 0x0F;	// TXD[0:3]_LED OFF
}

void sysClockInit(void){

	// PLL 설정 -- 95961600
	*R_PMCTRLEN =	PMCTRL_PLLCTRL_EN;
	*R_PMCTRLEN =	PMCTRL_CLKSEL_EN;	
	*R_PLLCTRL = PLLCTRL_ENABLE;
	*R_CKR = CKR_MCS_PLL;
}

void setWatchDog(uint32_t _ms, uint32_t _status) {
	uint32_t clock, wdtcnt;

	clock = GetAPBclock();
	
	wdtcnt = (_ms /1000) * clock;

	*R_WDCNT = wdtcnt;			// Watch Dog Reset Counter Value
	if(_status) *R_WDTCTRL = 1;				// Watch Dog Enable
}

void statusLEDControl(int _count, int _delay) {
	while(_count--)
	{
		delayms(_delay);
		*R_P5oLOW = (1<<7);
		delayms(_delay);
		*R_P5oHIGH = (1<<7);
	}
}


TS_RotarySwitchValue RotaySwitchDetect(void) {
	uint32_t i;
	uint32_t count = 100;
	TS_RotarySwitchValue RS;
	
	*R_P3oHIGH = (1<<6);
	for(i = 0 ; i < count ; i++);
	//delayms(1);
	RS.hundreds = *R_P3iLEV & 0x0f;
	*R_P3oLOW = (1<<6);

	*R_P3oHIGH = (1<<5);
	for(i = 0 ; i < count ; i++);
	//delayms(1);
	RS.tens = *R_P3iLEV & 0x0f;
	*R_P3oLOW = (1<<5);

	*R_P3oHIGH = (1<<4);
	for(i = 0 ; i < count ; i++);
	//delayms(1);
	RS.units = *R_P3iLEV & 0x0f;
	*R_P3oLOW = (1<<4);
	
	RS.RotarySwitchValue = (RS.hundreds*100) + (RS.tens*10) + RS.units;
	
	return RS;
}


U8 Get_Rotary_Switch_Process(void)
{
	switch(SW_ENUM)
	{
		case S100_SELECT:
			*R_P3oHIGH = (1<<6);
//			rotarySwitch.delay = 0;
			SW_ENUM = S100_DELAY;
			break;

		case S100_DELAY:
//			if(++rotarySwitch.delay >= 50)
			{
				SW_ENUM = S100_GET;
			}
			break;

		case S100_GET:
			rotarySwitch.hundreds = *R_P3iLEV & 0x0f;
			*R_P3oLOW = (1<<6);
			SW_ENUM = S10_SELECT;
			break;

		case S10_SELECT:
			*R_P3oHIGH = (1<<5);
//			rotarySwitch.delay = 0;
			SW_ENUM = S10_DELAY;
			break;

		case S10_DELAY:
//			if(++rotarySwitch.delay >= 50)
			{
				SW_ENUM = S10_GET;
			}
			break;

		case S10_GET:
			rotarySwitch.tens = *R_P3iLEV & 0x0f;
			*R_P3oLOW = (1<<5);
			SW_ENUM = S1_SELECT;
			break;

		case S1_SELECT:
			*R_P3oHIGH = (1<<4);
//			rotarySwitch.delay = 0;
			SW_ENUM = S1_DELAY;
			break;

		case S1_DELAY:
//			if(++rotarySwitch.delay >= 50)
			{
				SW_ENUM = S1_GET;
			}
			break;

		case S1_GET:
			rotarySwitch.units = *R_P3iLEV & 0x0f;
			*R_P3oLOW = (1<<4);

			SW_ENUM = SW_CALC;	//S100_SELECT;
			break;

		case SW_CALC:
			rotarySwitch.RotarySwitchValue = (rotarySwitch.hundreds*100) + (rotarySwitch.tens*10) + rotarySwitch.units;
			SW_ENUM = S100_SELECT;
			return 1;
	}

	return 0;

			
}


void Comm_Led_Display(void)
{
	unsigned char i;
	
	if(op_mode == OP_DISTRIBUTION)
	{
		for(i=0;i<5;i++)
		{
			*R_P0oLOW = 0x0F;

			*R_P2oLOW = 0x0F;

			delayms(500);

			*R_P0oHIGH = 0x0F;
			*R_P2oHIGH = 0x0F;

			delayms(500);
		}
	}
	else if(op_mode == OP_232_TO_OPTIC)
	{
		for(i=0;i<10;i++)
		{
			*R_P2oHIGH = (1<<2);		// OPT TX LED OFF
			*R_P0oLOW = (1<<2);			// OPT RX LED ON
			*R_P4oLOW = (1<<3);			// 232 TX LED ON
			*R_P4oHIGH = (1<<4);		// 232 RX LED OFF

			delayms(300);

			*R_P2oLOW = (1<<2);			// OPT TX LED ON
			*R_P0oHIGH = (1<<2);		// OPT RX LED OFF
			*R_P4oHIGH = (1<<3);		// 232 TX LED OFF
			*R_P4oLOW = (1<<4);			// 232 RX LED ON

			
			delayms(300);
		}

		*R_P2oHIGH = (1<<2);
		*R_P4oHIGH = (1<<4);
	}
	else if(op_mode == OP_SMUX)
	{
		for(i=0;i<5;i++)
		{
			*R_P0oLOW = (1<<2);
			*R_P2oLOW = (1<<2);
			*R_P4oLOW = (1<<3) | (1<<4);

			delayms(500);

			*R_P0oHIGH = (1<<2);
			*R_P2oHIGH = (1<<2);
			*R_P4oHIGH = (1<<3) | (1<<4);

			delayms(500);
			
		}
	}
	else if(op_mode == OP_OPT_BYPASS)
	{
		for(i=0;i<5;i++)
		{
			*R_P0oLOW = (1<<3);
			*R_P4oLOW = (1<<3);

			delayms(500);

			*R_P0oHIGH = (1<<3);
			*R_P4oHIGH = (1<<3);

			delayms(500);
			
		}
	}
}


