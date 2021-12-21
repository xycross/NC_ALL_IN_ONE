#pragma once

#include "cantus.h"
#include "globalDefine.h"


#define REPEATER_COMMAND_HEADER	0xD5
#define REPEATER_COMMAND_ENDER	0xD4

#define REPEATER_STATUS_HEADER	0xD2
#define REPEATER_STATUS_ENDER	0xD3


typedef struct 
{
	u16 repeaterAddress;
	u16 repeaterData;	
	u8 	detectorAddress;	
	u8 	detectorData;
	bool repeaterBoolean;
	bool check;

}TS_REP_DET_DATA;

typedef struct
{
	u16 index;
	u8 no;
	u8 command;
}_DET_INFO;


void Make_Tx_Packet(u16 addr, u8 det_no);


u8 				HIC_GetRepeaterStatus(u16 _repeaterAddress, u16 _repeaterCommand, u8* _txBuffer);
u8 				HIC_GetDetectorStatus(u16 _repeaterAddress, u16 _detectorAddress, u8 _detectorCommand, u8* _txBuffer);
u32 			HIC_FindDetectorIndex(u16 _repeaterAddress, u16 _detectorAddress);


TS_REP_DET_DATA Opt_Rx_Parsing(void);


TS_REP_DET_DATA HIC_Parser(u8* _pollingRxBuf, u32 length);
u16 			HIC_MuxDataApply(TS_REP_DET_DATA _repDetData, u16 _muxConfig[MUX_CONFIG]);
void RepeaterRestoreCtrl(void);
void RepeaterRestoreInit(void);

void Comm_Timer_Run(void);

void HIC_COMM_Delay_Timer_ISR(void);


