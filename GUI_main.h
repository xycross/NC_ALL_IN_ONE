#pragma once

#include "cantus.h"
#include "myTimer.h"

#define MAX_BUFFER				128

#define MUX_WAN_CH				4						// D-MUX 상위 채널 
#define MUX_RS232_CH_0			0
#define MUX_OPTIC_CH_4			4
#define MUX_OPTIC_CH_5			5
#define MUX_OPTIC_CH_6			6	
#define MUX_OPTIC_CH_7			7

#define MUX_DATA_DOWN_CH_COUNT	3						// D-MUX  하위 채널 개수

#define PROTOCOL_DOWN			false
#define PROTOCOL_UP				true

#define SET						1
#define CLR						0


#define DEFAULT_RS232_CH_BAUDRATE		115200
#define DEFAULT_OPTIC_CH_BAUDRATE		115200
#define DEFAULT_BAUDRATE				115200

#define REPEATER_COMMAND_HEADER	0xD5
#define REPEATER_STATUS_HEADER	0xD2

#define REPEATER_COMMAND_ENDER	0xD4
#define REPEATER_STATUS_ENDER	0xD3

#define ROTARYSWITCH_HUNDRED	100
#define ROTARYSWITCH_TEN		10
#define ROTARYSWITCH_UNIT		1



/*
typedef struct 
{
	u32 hundreds;
	u8 tens;
	u8 units;
	u32 RotarySwitchValue;
} TS_RotarySwitch;

*/

int GUI_main(void);

//bool IsEqual(u8 *a, u8 *b, u16 size);
//int BuadRateSelector(TS_RotarySwitch _rotaySwitchValue, int _placeValue, int _defaultBaudRate);
//int RotarySwitchMapping(int keydata);
void GUI_settimer(int nCh, U32 cntval, U16 prescale, BOOL on_off );

void GUI_System_Timer_ISR(void);
void Main_Timer_ISR(void);


void GUI_Watch_Dog(int _ms, int _status);






