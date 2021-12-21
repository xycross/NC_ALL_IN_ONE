
#include "HIC_Optic_COM.h"
#include "Converter.h"
#include "commOpticSwitch.h"

#include "smux_main.h"

#define Smoke_DETECTOR_SetValue_Min	7
#define Temp_DETECTOR_SetValue_Min	40

#define Smoke_DETECTOR_PreSetValue_default	8
#define Smoke_DETECTOR_RealSetValue_default	12

#define Temp_DETECTOR_PreSetValue_default	60
#define Temp_DETECTOR_RealSetValue_default	80

extern u16 m_Repeater[REPEATER_MAX][REPEATER_INFO];
extern U8 m_Detector[PLC_DETECTOR_MAX][PLC_DETECTOR_INFO];


_DET_INFO DET_INFO;

/*
#define REPEATER_COMMAND			0
#define REPEATER_STATUS				1
#define REPEATER_LOOPNUM			2
#define REPEATER_DETECTOR_COUNT		3
#define REPEATER_COM_ERR			4
#define REPEATER_COM_DELAY_COUNT	5

#define DETECTOR_COMMAND			0
#define DETECTOR_STATUS				1
#define DETECTOR_P_FIRE				2
#define DETECTOR_R_FIRE				3
#define DETECTOR_COM_ERR			4
#define DETECTOR_COM_DELAY_COUNT	5
*/


const unsigned char Code2_Confirm_Table[256] = 
{
//   	0	1	2	3	4	5	6	7	8	9	
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 0 	- 	9
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 10	- 	19
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 20	- 	29
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 30 	- 	39
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 40	- 	49
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 50	- 	59
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 60	- 	69
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 70	- 	79
	0,	0,	0,	0,	0,	1,	1,	0,	0,	1,		// 80	- 	89
	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 90 	- 	99
	0,	1,	1,	0,	0,	1,	1,	0,	0,	0,		// 100 	- 	109
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 110 	- 	119
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 120	- 	129
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 130	- 	139
	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,		// 140 	- 	149
	1,	0,	0,	1,	1,	0,	0,	0,	0,	0,		// 150 	- 	159
	0,	0,	0,	0,	0,	1,	1,	0,	0,	1,		// 160 	- 	169
	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 170 	- 	179
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 180 	- 	189
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 190 	-	199
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 200	- 	209
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 210 	- 	219
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 220 	- 	229
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 230 	- 	239
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 240	- 	249
	0,	0,	0,	0,	0,	0						// 250 	-	255
	
};


extern u8 Opt_Tx_Buffer[15];

extern u8 OPT_RX_BUFFER[100];


extern u8 Comm_TimeOut;


U8 RestoreFlag;
U8 repRestoreCnt;


void RepeaterRestoreInit(void)
{
	U16 i=0;
	for(i=0; i<REPEATER_MAX; i++)
	{
		m_Repeater[i][REPEATER_COMMAND] = 0x000F;	// Restore MSG
	}
	
	for(i=0; i<PLC_DETECTOR_MAX;i++)
	{
		CLEARBIT(m_Detector[i][DETECTOR_COMMAND], DETECTOR_CMD_LED);
		CLEARBIT(m_Detector[i][DETECTOR_COMMAND], DETECTOR_CMD_PFIRE);
		CLEARBIT(m_Detector[i][DETECTOR_COMMAND], DETECTOR_CMD_RFIRE);
		CLEARBIT(m_Detector[i][DETECTOR_COMMAND], DETECTOR_CMD_TEST);
	}

	RestoreFlag=SET;
}


void RepeaterRestoreCtrl(void) 
{
	U16 i;
	
	if(RestoreFlag == SET) 
	{
		if(++repRestoreCnt > 15)
		{
			RestoreFlag = CLR;
			
			for(i=0; i<REPEATER_MAX; i++)
			{
				m_Repeater[i][REPEATER_COMMAND] = 0x0000;	// Restore MSG
			}	
			repRestoreCnt=0;
		}
	}
}	

void Make_Tx_Packet(u16 addr, u8 det_no)
{
	u8 Detector_Index;
	u8 i, sum;
	
	
	
	if(det_no)
	{
	//	Detector_Index = HIC_FindDetectorIndex(addr, det_no);
		SETBIT(addr, 11);
		Codiz_Data12bit(&Opt_Tx_Buffer[5], addr);
		Codiz_Data8bit(&Opt_Tx_Buffer[8], DET_INFO.no);
		Codiz_Data8bit(&Opt_Tx_Buffer[10], DET_INFO.command);
//		debugprintf("TX Addr = %d, Det = %d\r\n", addr, DET_INFO.no);
	}
	else
	{
		Codiz_Data12bit(&Opt_Tx_Buffer[5], addr);
		Codiz_Data16bit(&Opt_Tx_Buffer[8], m_Repeater[addr][REPEATER_COMMAND]);
//		debugprintf("TX ADDR = %d\r\n", addr);
	}

	sum = 0;
	for(i=5;i<12;i++)
	{
		sum += Opt_Tx_Buffer[i];
	}

	Codiz_From8_To16(&Opt_Tx_Buffer[12], sum);

	for(i=0;i<15;i++)
	{
		*UART_TX_SPOF3 = Opt_Tx_Buffer[i];
	}

	LED_SET(LED_SPOF3, LED_TX);
	
}
	
u8 HIC_GetRepeaterStatus(u16 _repeaterAddress, u16 _repeaterCommand, u8* _txBuffer)
{
	u8 i;
	u8 sum;
	
	for(i = 0 ; i < 5 ; i++){_txBuffer[i] = REPEATER_COMMAND_HEADER;}	
	Codiz_Data12bit(&_txBuffer[5], _repeaterAddress);
	Codiz_Data16bit(&_txBuffer[8],_repeaterCommand);

	for( i = 5 ; i < 12 ; i++)
	{
		sum += _txBuffer[i];
	}
	Codiz_From8_To16(&_txBuffer[12], sum);
	_txBuffer[14] = REPEATER_COMMAND_ENDER;
	return 15;
	
}


u8 HIC_GetDetectorStatus(u16 _repeaterAddress, u16 _detectorAddress, u8 _detectorCommand, u8* _txBuffer)
{
	u32 i;
	u16 repeaterAddress = _repeaterAddress;
	for(i = 0 ; i < 5 ; i++){_txBuffer[i] = REPEATER_COMMAND_HEADER;}
	SETBIT(repeaterAddress, 11);
	Codiz_Data12bit(&_txBuffer[5], repeaterAddress);
	Codiz_Data8bit(&_txBuffer[8], _detectorAddress);		
	Codiz_Data8bit(&_txBuffer[10], _detectorCommand);
	u32 sum = 0;
	for( i = 5 ; i < 12 ; i++)
	{
		sum += _txBuffer[i];
	}
	Codiz_From8_To16(&_txBuffer[12], sum);
	_txBuffer[14] = REPEATER_COMMAND_ENDER;
	return 15;

}


TS_REP_DET_DATA Opt_Rx_Parsing(void)
{
	u8 i, j, sum;
	u16 rep_addr;
	TS_REP_DET_DATA RepDetData;


	sum = 0;
	for(i=0;i<7;i++)
	{
		sum += OPT_RX_BUFFER[i];
	}

	j = UnCodiz_From16_To8(&OPT_RX_BUFFER[7]);

	if(j == sum)
	{
		rep_addr = UnCodiz_12bit(&OPT_RX_BUFFER[0]);


		if(TESTBIT(rep_addr, 11))
		{
			CLEARBIT(rep_addr, 11);
			RepDetData.repeaterBoolean = false;
			RepDetData.repeaterAddress = rep_addr;
			RepDetData.detectorAddress = UnCodiz_8bit(OPT_RX_BUFFER[3], OPT_RX_BUFFER[4]);
			RepDetData.detectorData = UnCodiz_8bit(OPT_RX_BUFFER[5], OPT_RX_BUFFER[6]);
			
		}
		else
		{
			RepDetData.repeaterAddress = rep_addr;
			UnCodiz_16bit(&OPT_RX_BUFFER[3], &RepDetData.repeaterData);
		}

		RepDetData.check = true;
	}
	else
	{
		RepDetData.check = false;
	}

	return RepDetData;
	
}


TS_REP_DET_DATA HIC_Parser(u8* _pollingRxBuf, u32 length)
{
	TS_REP_DET_DATA repDetData;
	u32 i;
	u16 repeaterAddress;
	u8 sum, count = 0;
	u8 pos;

	repDetData.check = false;


#if 0	
	for(i = 0 ; i < length ; i++)
	{	
		if(_pollingRxBuf[i] == REPEATER_STATUS_HEADER)
		{
			if(!(_pollingRxBuf[i+1] == REPEATER_STATUS_HEADER))
			{
				count = i;
				break;
			}
		}
	}
#endif

	for(i=0; i < length; i++)
	{
		if(Code2_Confirm_Table[_pollingRxBuf[i]])
		{
			count++;
			if(count == 9)
			{
				break;
			}
		}
		else
		{
			count = 0;
		}
	}

	if((count == 9) && (i >= 8))
	{
		pos = i - 8;
		sum = 0;
//		debugprintf("Rx Data : ");
		for(i=pos; i < (pos + 7);i++)
		{
			sum += _pollingRxBuf[i];
//			debugprintf("%02X ", _pollingRxBuf[i]);
			
		}
//		debugprintf("\r\n");

		if(sum == UnCodiz_From16_To8(&_pollingRxBuf[pos + 7]))
		{
			LED_SET(LED_SPOF3, LED_RX);
			repeaterAddress = UnCodiz_12bit(&_pollingRxBuf[pos]);
//			debugprintf("Rep Addr = %d\r\n", repeaterAddress);

			
			if(TESTBIT(repeaterAddress,11))
			{
				repDetData.repeaterBoolean = false;
				CLEARBIT(repeaterAddress,11);
				repDetData.repeaterAddress 	= repeaterAddress;
				repDetData.detectorAddress 	= UnCodiz_8bit(_pollingRxBuf[pos + 3], _pollingRxBuf[pos + 4]);
				repDetData.detectorData		= UnCodiz_8bit(_pollingRxBuf[pos + 5], _pollingRxBuf[pos + 6]);
				repDetData.check 			= true;
			}
			else
			{
				repDetData.repeaterBoolean 	= true;
				repDetData.repeaterAddress 	= repeaterAddress;
				UnCodiz_16bit(&_pollingRxBuf[pos + 3], &(repDetData.repeaterData));
				repDetData.check 			= true;
			}				
			return repDetData;
			
		}
		
	}

	return repDetData;

	
#if 0
	
	if(_pollingRxBuf[count+10] == REPEATER_STATUS_ENDER)
	{
		u8 sum = 0;
		for(i = count + 1 ; i < count + 8 ; i++)
		{
			sum += _pollingRxBuf[i];			
		}
		if(sum == UnCodiz_From16_To8(&_pollingRxBuf[count + 8]))			//프로토콜 이상무
		{ 
			repeaterAddress = UnCodiz_12bit(&_pollingRxBuf[count + 1]);

			if(TESTBIT(repeaterAddress,11))
			{
				repDetData.repeaterBoolean = false;
				CLEARBIT(repeaterAddress,11);
				repDetData.repeaterAddress 	= repeaterAddress;
				repDetData.detectorAddress 	= UnCodiz_8bit(_pollingRxBuf[count + 4], _pollingRxBuf[count + 5]);
				repDetData.detectorData		= UnCodiz_8bit(_pollingRxBuf[count + 6], _pollingRxBuf[count + 7]);
				repDetData.check 			= true;
			}
			else
			{
				repDetData.repeaterBoolean 	= true;
				repDetData.repeaterAddress 	= repeaterAddress;
				UnCodiz_16bit(&_pollingRxBuf[count + 4], &(repDetData.repeaterData));
				repDetData.check 			= true;
			}				
			return repDetData;
		}
	}
	else 
	{
		repDetData.check = false; 
		return repDetData;
	}
	
	return repDetData;
#endif

}


/*
typedef struct 
{
	u16 repeaterAddress;
	u16 repeaterData;	
	u8 	detectorAddress;	
	u8 	detectorData;
	bool repeaterBoolean;
	bool check;

}TS_REP_DET_DATA;

#define REPEATER_COMMAND			0
#define REPEATER_STATUS				1
#define REPEATER_LOOPNUM			2
#define REPEATER_DETECTOR_COUNT		3
#define REPEATER_COM_ERR			4
#define REPEATER_COM_DELAY_COUNT	5

#define DETECTOR_COMMAND			0
#define DETECTOR_STATUS				1
#define DETECTOR_P_FIRE				2
#define DETECTOR_R_FIRE				3
#define DETECTOR_COM_ERR			4
#define DETECTOR_COM_DELAY_COUNT	5

*/
	
u16 HIC_MuxDataApply(TS_REP_DET_DATA _repDetData, u16 _muxConfig[MUX_CONFIG])
{
	
	int detector_set_valueOk=0;
	u32 detectorIndex;
	/******************************************************* 코드수정*****************************/
	/**																							**/
    /** 일자 : 2015/10/08 																		**/
    /** 내용 : 창원경상대 현장요청에 대한 수정													**/
	/**        1) 감지기 값이 255일 경우 0으로 값변경											**/
	/**																							**/
	/** 174줄 기존코드 : m_Detector[detectorIndex][DETECTOR_STATUS] = _repDetData.detectorData; 	**/
	/** 174줄 변경코드 : m_Detector[detectorIndex][DETECTOR_STATUS] = CLR; 						**/
	/**																							**/
	/*********************************************************************************************/
	
	if(_repDetData.repeaterBoolean)
	{
		m_Repeater[_repDetData.repeaterAddress][REPEATER_COM_ERR] 		 = CLR;
		m_Repeater[_repDetData.repeaterAddress][REPEATER_COM_DELAY_COUNT] = CLR;
		CLEARBIT(m_Repeater[_repDetData.repeaterAddress][REPEATER_STATUS],15);

		if(m_Repeater[_repDetData.repeaterAddress][REPEATER_STATUS] != _repDetData.repeaterData)
		{
			if(++m_Repeater[_repDetData.repeaterAddress][REPEATER_STATUS_CNT] >= _muxConfig[MUX_CONFIG_R_STATUS_CNT])
			{
				debugprintf("Opt %d : %04X -> %04X\r\n", _repDetData.repeaterAddress , m_Repeater[_repDetData.repeaterAddress][REPEATER_STATUS],
														_repDetData.repeaterData);
				
				m_Repeater[_repDetData.repeaterAddress][REPEATER_STATUS] 	 	 = _repDetData.repeaterData;
				
				return _repDetData.repeaterData;
			}
		}
		else 
		{
			m_Repeater[_repDetData.repeaterAddress][REPEATER_STATUS_CNT] = 0;
			return m_Repeater[_repDetData.repeaterAddress][REPEATER_STATUS];
		}
	}
	else
	{
		detectorIndex = HIC_FindDetectorIndex(_repDetData.repeaterAddress, _repDetData.detectorAddress);

		// 2020. 10. 09 yjm
		if(detectorIndex >= PLC_DETECTOR_MAX)
		{
			return 0;
		}

		if(_repDetData.detectorData == 255)		// PLC 중계기는 감지기가 통신이 안되거나 설치되어 있지 않은경우 255값을 전송한다
		{
			if(++m_Detector[detectorIndex][DETECTOR_COM_255_OFFLINE_COUNT] > _muxConfig[MUX_CONFIG_DETECTOR_255_OFFLINE_COUNT])
			{
				m_Detector[detectorIndex][DETECTOR_COM_255_OFFLINE_COUNT] = CLR;
				SETBIT(m_Detector[detectorIndex][DETECTOR_COMMAND],DETECTOR_CMD_OFFLINE);
			}
			m_Detector[detectorIndex][DETECTOR_STATUS]			= CLR; 						// V0.96버전 2015/10/29 -> 감지기 값 변경하지 않음
                                                                                            // v0.97버전 2016/03/02 -> 감지기 값이 255일 경우 0으로 값변경,,
			m_Detector[detectorIndex][DETECTOR_COM_ERR]			= CLR;
			m_Detector[detectorIndex][DETECTOR_COM_DELAY_COUNT] 	= CLR;
			return 0;
		}
		else
		{
			m_Detector[detectorIndex][DETECTOR_COM_ERR]			= CLR;
			m_Detector[detectorIndex][DETECTOR_COM_DELAY_COUNT] 	= CLR;
			CLEARBIT(m_Detector[detectorIndex][DETECTOR_COMMAND],DETECTOR_CMD_OFFLINE);
			
			detector_set_valueOk = 0;
				
			// Detector Fire Check
			if(!TESTBIT(m_Detector[detectorIndex][DETECTOR_COMMAND], DETECTOR_CMD_COMMSTOP))
			{					
				if(!TESTBIT(m_Detector[detectorIndex][DETECTOR_COMMAND], DETECTOR_CMD_RFIRE))
				{
					
					if(TESTBIT(m_Detector[detectorIndex][DETECTOR_COMMAND],DETECTOR_CMD_S1T0) ) // 감지기 타입 :1 = 연기
					{
						if( (m_Detector[detectorIndex][DETECTOR_R_FIRE] > Smoke_DETECTOR_SetValue_Min) && (m_Detector[detectorIndex][DETECTOR_P_FIRE] > Smoke_DETECTOR_SetValue_Min) )
						{
							detector_set_valueOk = 1;
						}
						else 
						{
							m_Detector[detectorIndex][DETECTOR_P_FIRE] = Smoke_DETECTOR_PreSetValue_default;
							m_Detector[detectorIndex][DETECTOR_R_FIRE] = Smoke_DETECTOR_RealSetValue_default;
						}

						// maximum value check
						if(_repDetData.detectorData > 22)
						{
							return m_Detector[detectorIndex][DETECTOR_STATUS];
						}
					}
					else
					{	// 감지기 타입 : 0 온도
						if( (m_Detector[detectorIndex][DETECTOR_R_FIRE] > Temp_DETECTOR_SetValue_Min) && (m_Detector[detectorIndex][DETECTOR_P_FIRE] > Temp_DETECTOR_SetValue_Min) )
						{
							detector_set_valueOk = 1;
						}
						else
						{
							m_Detector[detectorIndex][DETECTOR_P_FIRE] = Temp_DETECTOR_PreSetValue_default;
							m_Detector[detectorIndex][DETECTOR_R_FIRE] = Temp_DETECTOR_RealSetValue_default;
						}

						// maximum value check
						if(_repDetData.detectorData > 120)
						{
							return m_Detector[detectorIndex][DETECTOR_STATUS];
						}
					}
					
					if( detector_set_valueOk )	// 예비화재 및 실화재 설정값 최소 값 이상일때만 화재 비교 처리
					{
						if(_repDetData.detectorData >= m_Detector[detectorIndex][DETECTOR_R_FIRE])
						{
							if(++m_Detector[detectorIndex][DETECTOR_R_FIRE_CNT] >= _muxConfig[MUX_CONFIG_R_FIRE_CNT])
							{
								m_Detector[detectorIndex][DETECTOR_R_FIRE_CNT] = _muxConfig[MUX_CONFIG_R_FIRE_CNT];
								
								SETBIT(m_Detector[detectorIndex][DETECTOR_COMMAND], DETECTOR_CMD_RFIRE);
								SETBIT(m_Detector[detectorIndex][DETECTOR_COMMAND], DETECTOR_CMD_LED);							
							}
						}
						else 
						{
							m_Detector[detectorIndex][DETECTOR_R_FIRE_CNT] = 0;
							if(_repDetData.detectorData>=m_Detector[detectorIndex][DETECTOR_P_FIRE])
							{
								if(++m_Detector[detectorIndex][DETECTOR_P_FIRE_CNT] >= _muxConfig[MUX_CONFIG_P_FIRE_CNT])
								{
									m_Detector[detectorIndex][DETECTOR_P_FIRE_CNT] = _muxConfig[MUX_CONFIG_P_FIRE_CNT];
									SETBIT(m_Detector[detectorIndex][DETECTOR_COMMAND], DETECTOR_CMD_PFIRE);
									SETBIT(m_Detector[detectorIndex][DETECTOR_COMMAND], DETECTOR_CMD_LED);
								}	
							}
							else 
							{
								m_Detector[detectorIndex][DETECTOR_P_FIRE_CNT] = 0;
								CLEARBIT(m_Detector[detectorIndex][DETECTOR_COMMAND], DETECTOR_CMD_PFIRE);
								CLEARBIT(m_Detector[detectorIndex][DETECTOR_COMMAND], DETECTOR_CMD_LED);
							}
						}
					}
				}
			}
			
			m_Detector[detectorIndex][DETECTOR_STATUS]		= _repDetData.detectorData;							// 현재 값이 이전 값보다 작거나 같을 때만 현재 값 표시

			return m_Detector[detectorIndex][DETECTOR_STATUS];
		}		
	}
	
	return 0;
}

// return index
u32 HIC_FindDetectorIndex(u16 _repeaterAddress, u16 _detectorAddress)
{
	/*
	u32 repAddress;
	u32 sum = 0;

	for(repAddress = 0 ; repAddress < _repeaterAddress + 1 ; repAddress++)
	{
		if(TESTBIT(m_Repeater[repAddress][REPEATER_DETECTOR_COUNT],7))
		{
			sum += (m_Repeater[repAddress][REPEATER_DETECTOR_COUNT] & 0x007f);
		}		
	}
	return (sum - ((m_Repeater[_repeaterAddress][REPEATER_DETECTOR_COUNT] & 0x007f) - _detectorAddress));
	*/
	u32 index;
	if(m_Repeater[_repeaterAddress][DETECTOR_START_ADD] > FIND_REAL_DETECTOR_VALUE_ADD)
	{
		index = (m_Repeater[_repeaterAddress][DETECTOR_START_ADD] - FIND_REAL_DETECTOR_VALUE_ADD) + (_detectorAddress - 1);
	}
	else
	{
		index = CLR;
	}
	return index;
}

void Comm_Timer_Run(void)
{
	*(volatile U32*)0x80021040 = 1<<1;
	*(volatile U32*)0x80021040 = 0;

	Comm_TimeOut = 0;
}

void HIC_COMM_Delay_Timer_ISR(void)
{
	stoptimer(HIC_COM_DELAY_TIMER);
	Comm_TimeOut = 1;
}



