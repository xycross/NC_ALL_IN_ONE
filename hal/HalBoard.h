#ifndef __HALBOARD_H__
#define __HALBOARD_H__

#include "Hal.h"



typedef struct  {
	uint16_t hundreds;
	uint8_t tens;
	uint8_t units;
	uint16_t RotarySwitchValue;
	uint8_t delay;
} TS_RotarySwitchValue;



enum RS_ENUM
{
	S100_SELECT,
	S100_DELAY,
	S100_GET,
	S10_SELECT,
	S10_DELAY,
	S10_GET,
	S1_SELECT,
	S1_DELAY,
	S1_GET,
	SW_CALC
};


void SystemInit(void);
void setWatchDog(uint32_t _ms, uint32_t _status);
void statusLEDControl(int _count, int _delay);
TS_RotarySwitchValue RotaySwitchDetect(void);

U8 Get_Rotary_Switch_Process(void);

void Comm_Led_Display(void);

#endif



