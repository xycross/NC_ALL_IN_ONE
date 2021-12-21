#include "cimon-PLC-RS232_422_485.h"
#include "HalUart.h"
#include "Converter.h"
#include "commOpticSwitch.h"

extern TS_SCADA_DATA scadaData;
extern u16 m_Repeater[REPEATER_MAX][REPEATER_INFO];
extern U8 	m_Detector[PLC_DETECTOR_MAX][PLC_DETECTOR_INFO];

extern U8 Write_Disable;

extern U16 m_MuxConfig[MUX_CONFIG];

extern u32 m_ScadaWriteTimerCount;
extern bool m_ScadaWriteTimerFlag;


u8 SCADA_RX_DATA[SCADA_PROTOCOL_DATA_MAX];
u8 SCADA_TX_DATA[SCADA_PROTOCOL_DATA_MAX];

u8 SCADA_RXD, SCADA_RX_STATE, SCADA_RX_POS;

////////////////////// SCADA PROTOCOL //////////////////////
////				#define SCADA_READ_WORD			'R' ////
////				#define SCADA_WRITE_WORD		'W' ////
////				#define SCADA_READ_BIT			'r' ////
////				#define SCADA_WRITE_BIT			'w' ////
////				#define SCADA_ERROR_RESPONSE	'E' ////
////				#define ENQ_INDEX			0  		////
////				#define STATION_H_INDEX		1  		////
////				#define STATION_L_INDEX		2  		////
////				#define CMD_INDEX			3  		////
////				#define LENGTH_H_INDEX		4  		////
////				#define LENGTH_L_INDEX		5  		////
////				#define DATATYPE_INDEX		6  		////
////													////
////				#define w_r_STARTADD_INDEX	8 	 	////
////				#define W_R_STARTADD_INDEX	9  		////
////				#define w_r_STARTBIT_INDEX	13 		////
////				#define DATA_LENGTH_H_INDEX	14 		////
////				#define DATA_LENGTH_L_INDEX	15 		////
////				#define DATA_START_INDEX	16		////
////////////////////////////////////////////////////////////


void Scada_Processing(void)
{
	u8 length, i;
	
	if(SCADA_RXD)
	{
		SCADA_RXD = 0;
		LED_SET(LED_RS232, LED_RX);
		
		Scada_Parser(SCADA_RX_DATA);
		if(scadaData.check)
		{
			if((scadaData.startAddress >= 5001) && (scadaData.startAddress <= 9004))
			{
				if((scadaData.command == 'w') || (scadaData.command == 'W'))
				{
					m_ScadaWriteTimerCount = CLR;
					m_ScadaWriteTimerFlag = true;
				}
			}
			length = Scada_MuxResponse(m_MuxConfig);
			for(i=0;i<length;i++)
			{
				while(!((*(volatile U32*)(RS232_UART_BASE + UART_LSR))  & ULSR_TEMP));
				*UART_TX_RS232 = SCADA_TX_DATA[i];	
			}
			LED_SET(LED_RS232, LED_TX);
		}
	}
}

void Scada_Parser(u8* _scadaRxBuf)
{

	int i;
	int scadaProtocolDataLength		=	0 ;
	int scadaBCC					= 	0 ;
	int calculScadaBCC 				= 	0 ;

	if(_scadaRxBuf[ ENQ_INDEX ] == ENQ)
	{
		scadaProtocolDataLength = 	Conv_2ASC_1Byte(_scadaRxBuf[ LENGTH_H_INDEX ], 
													_scadaRxBuf[ LENGTH_L_INDEX ]);  								// Scada Data의 길이
		scadaBCC				=	Conv_2ASC_1Byte(_scadaRxBuf[ LENGTH_L_INDEX + scadaProtocolDataLength + 1 ],
													_scadaRxBuf[ LENGTH_L_INDEX + scadaProtocolDataLength + 2 ]);	// BCC 
											
		for(i = 0 ; i < CMD_INDEX + scadaProtocolDataLength ; i++)
		{
			calculScadaBCC += _scadaRxBuf[CMD_INDEX + i];
		}		
		calculScadaBCC %= 256;	
		
		
		if(calculScadaBCC == scadaBCC 
		   && _scadaRxBuf[ LENGTH_L_INDEX + scadaProtocolDataLength + 3 ]  ==  EOT)			// 프로토콜 이상무
		{
				scadaData.station		= 	Conv_2ASC_1Byte(_scadaRxBuf[ STATION_H_INDEX ], 
															_scadaRxBuf[ STATION_L_INDEX ]);
				scadaData.command		=	_scadaRxBuf[ CMD_INDEX ];
				scadaData.dataType		=	_scadaRxBuf[ DATATYPE_INDEX ];
				scadaData.dataLength	=	Conv_2ASC_1Byte(_scadaRxBuf[ DATA_LENGTH_H_INDEX ], 
															_scadaRxBuf[ DATA_LENGTH_L_INDEX ]);
				if(scadaData.command == SCADA_WRITE_WORD || scadaData.command == SCADA_READ_WORD)
				{
					scadaData.startAddress	= 	Conv_5ASC_2Byte(&_scadaRxBuf[ W_R_STARTADD_INDEX ]);
					if(scadaData.command == SCADA_WRITE_WORD)
					{
						for(i = 0 ; i < scadaData.dataLength; i++)
						{
							scadaData.wordData[i]	= Conv_4ASC_2Hex(&_scadaRxBuf[ DATA_START_INDEX + (i * 4) ]);
						}
					}				
				}
				else if(scadaData.command == SCADA_WRITE_BIT || scadaData.command == SCADA_READ_BIT )
				{
					scadaData.startAddress		= 	Conv_5ASC_2Byte(&_scadaRxBuf[ w_r_STARTADD_INDEX ]);
					scadaData.startBitNumber	=	Conv_1ASC_1Byte(_scadaRxBuf[ w_r_STARTBIT_INDEX ]);
					if(scadaData.command == SCADA_WRITE_BIT)
					{
						for(i = 0 ; i < scadaData.dataLength; i++)
						{
							scadaData.bitData[i]	=	Conv_1ASC_1Byte(_scadaRxBuf[ DATA_START_INDEX + i]);
						}
					}
				}
					
						
				scadaData.check			= true; 
		}
		else
		{
			scadaData.check = false;
		}
		
	}
	else
	{
		scadaData.check = false;
	}
}


///////////////////TS_SCADA_DATA//////////////////
// typedef struct 								//
// {											//
// 		u8 	station;							//
// 		u8 	command;							//
//		u8 	dataLength;							//
//		U8	dataType;							//
//		u32	startAddress;						//
//		u8	startBitNumber;						//
//		u16 wordData[SCADA_PROTOCOL_DATA_MAX];	//
//		u8  bitData[SCADA_PROTOCOL_DATA_MAX];	//
//		BOOL  check; 							//
//												//
// }TS_SCADA_DATA;								//
//////////////////////////////////////////////////

u8 Scada_MuxResponse(u16 _muxConfig[MUX_CONFIG])
{
	u16 muxDataBuffer[MAXBUFFER];
	u32 i, j;
	u16 station;
	u32 bcc;
	u16 ASCLength;
	u8 length;
	u32 data;
	u32 sum;
	
	Scada_MuxDataApplyA(_muxConfig, muxDataBuffer);
	station = Conv_1Byte_2ASC(scadaData.station);
	
	SCADA_TX_DATA[0] = STX;
	SCADA_TX_DATA[1] = station >> 8;	
	SCADA_TX_DATA[2] = (u8)(station & 0x00ff);
	SCADA_TX_DATA[3] = scadaData.command;
	
	switch(scadaData.command)
	{
		case SCADA_WRITE_WORD :
		{	
			SCADA_TX_DATA[4] = '0';
			SCADA_TX_DATA[5] = '0';
			
			bcc	=	Conv_1Byte_2ASC(((SCADA_TX_DATA[3]+SCADA_TX_DATA[4]+SCADA_TX_DATA[5])%256));
			SCADA_TX_DATA[6] = bcc >> 8;
			SCADA_TX_DATA[7] = (u8)(bcc & 0x00ff);
			SCADA_TX_DATA[8] = ETX;
			
			return 9;
		}
		case SCADA_WRITE_BIT :
		{
			SCADA_TX_DATA[4] = '0';
			SCADA_TX_DATA[5] = '0';
			
			bcc	=	Conv_1Byte_2ASC(((SCADA_TX_DATA[3]+SCADA_TX_DATA[4]+SCADA_TX_DATA[5])%256));
			SCADA_TX_DATA[6] = bcc >> 8;
			SCADA_TX_DATA[7] = (u8)(bcc & 0x00ff);
			SCADA_TX_DATA[8] = ETX;
			
			return 9;
		}
		case SCADA_READ_WORD :
		{
			ASCLength =	Conv_1Byte_2ASC(scadaData.dataLength * 4);
			length = scadaData.dataLength * 4;
			SCADA_TX_DATA[4] = ASCLength >> 8;
			SCADA_TX_DATA[5] = (u8)(ASCLength & 0x00ff);
			data = 0;
			for( i = 0 ; i < scadaData.dataLength; i++)
			{
				data = Conv_1Word_4ASC(muxDataBuffer[i]);
				j = 6+(i*4);
				SCADA_TX_DATA[j+0] = (u8)(data >> 24);
				SCADA_TX_DATA[j+1] = (u8)(data >> 16);
				SCADA_TX_DATA[j+2] = (u8)(data >> 8);
				SCADA_TX_DATA[j+3] = (u8)(data);
			}

			sum = 0;
			for( i = 0 ; i < length + 3 ; i++)
			{
				sum += SCADA_TX_DATA[3 + i];
			}			
			bcc	=	Conv_1Byte_2ASC((sum%256));
			SCADA_TX_DATA[length + 6] = bcc >> 8;
			SCADA_TX_DATA[length + 7] = (u8)(bcc & 0x00ff);
			SCADA_TX_DATA[length + 8] = ETX;

			return length + 9;
		}
		case SCADA_READ_BIT :
		{
			break;
		}
	}
	
	return 0;
}
	
#if 0	
u8 Scada_MuxResponse(u8* _txBuffer, u16 _muxConfig[MUX_CONFIG])
{
	u16 muxDataBuffer[MAXBUFFER];
	u32 i, j;
	u16 station;
	u32 bcc;
	u16 ASCLength;
	u8 length;
	u32 data;
	u32 sum;
	
	Scada_MuxDataApplyA(_muxConfig, muxDataBuffer);
	station = Conv_1Byte_2ASC(scadaData.station);
	
	_txBuffer[0] = STX;
	_txBuffer[1] = station >> 8;	
	_txBuffer[2] = (u8)(station & 0x00ff);
	_txBuffer[3] = scadaData.command;
	
	switch(scadaData.command)
	{
		case SCADA_WRITE_WORD :
		{	
			_txBuffer[4] = '0';
			_txBuffer[5] = '0';
			
			bcc	=	Conv_1Byte_2ASC(((_txBuffer[3]+_txBuffer[4]+_txBuffer[5])%256));
			_txBuffer[6] = bcc >> 8;
			_txBuffer[7] = (u8)(bcc & 0x00ff);
			_txBuffer[8] = ETX;
			
			return 9;
		}
		case SCADA_WRITE_BIT :
		{
			_txBuffer[4] = '0';
			_txBuffer[5] = '0';
			
			bcc	=	Conv_1Byte_2ASC(((_txBuffer[3]+_txBuffer[4]+_txBuffer[5])%256));
			_txBuffer[6] = bcc >> 8;
			_txBuffer[7] = (u8)(bcc & 0x00ff);
			_txBuffer[8] = ETX;
			
			return 9;
		}
		case SCADA_READ_WORD :
		{
			ASCLength =	Conv_1Byte_2ASC(scadaData.dataLength * 4);
			length = scadaData.dataLength * 4;
			_txBuffer[4] = ASCLength >> 8;
			_txBuffer[5] = (u8)(ASCLength & 0x00ff);
			data = 0;
			for( i = 0 ; i < scadaData.dataLength; i++)
			{
				data = Conv_1Word_4ASC(muxDataBuffer[i]);
				j = 6+(i*4);
				_txBuffer[j+0] = (u8)(data >> 24);
				_txBuffer[j+1] = (u8)(data >> 16);
				_txBuffer[j+2] = (u8)(data >> 8);
				_txBuffer[j+3] = (u8)(data);
			}

			sum = 0;
			for( i = 0 ; i < length + 3 ; i++)
			{
				sum += _txBuffer[3 + i];
			}			
			bcc	=	Conv_1Byte_2ASC((sum%256));
			_txBuffer[length + 6] = bcc >> 8;
			_txBuffer[length + 7] = (u8)(bcc & 0x00ff);
			_txBuffer[length + 8] = ETX;

			return length + 9;
		}
		case SCADA_READ_BIT :
		{
			break;
		}
	}
	
	return 0;
}
#endif

/////////////REPEATER DEFINE//////////////
//	#define REPEATER_COMMAND		0   //
//	#define REPEATER_STATUS			1	//
//	#define REPEATER_LOOPNUM		2	//
//	#define REPEATER_DETECTOR_COUNT	3	//
//	#define REPEATER_COM_ERR		4	//
//////////////////////////////////////////
	
/////////////DETECTOR DEFINE//////////////
//#define DETECTOR_COMMAND			0	//
//#define DETECTOR_STATUS			1	//
//#define DETECTOR_P_FIRE			2	//
//#define DETECTOR_R_FIRE			3	//
//#define DETECTOR_COM_ERR			4	//
//////////////////////////////////////////







u32 Scada_MuxDataApplyA(u16 _muxConfig[MUX_CONFIG], u16* _muxDataBuffer)
{
	u32 i;	
	u32 address 	= scadaData.startAddress;
	u32 bitAddress 	= scadaData.startBitNumber;
	
	switch(scadaData.command)
	{
		case SCADA_WRITE_WORD :
		{
			for(i = 0 ; i < scadaData.dataLength ; i++ )
			{
				debugprintf("SCADA WRITE WORD : %d - (%04X, %d)\r\n", address, scadaData.wordData[i], scadaData.wordData[i]);
				//중계기 입력 데이터 ,, D00001~D01000 
				//중계기 인덱스 :: 커맨드[0], 상태[1], 중계기의 루프넘버 [2], PLC 중계기당 감지기 갯수 [3], 통신ERR COUNT[4]
				
				if(address >= REPEATER_INDATA_ADD_MIN_D00001 && address <= REPEATER_INDATA_ADD_MAX_D01000)
				{
					m_Repeater[ address - FIND_REAL_REPEATER_INDATA_ADD ][REPEATER_STATUS]		= scadaData.wordData[i];
				}		
				//중계기 출력 데이터 ,, D01001~D02000
				else if(address >= REPEATER_OUTDATA_ADD_MIN_D01001 && address <= REPEATER_OUTDATA_ADD_MAX_D02000)
				{
					m_Repeater[ address - FIND_REAL_REPEATER_OUTDATA_ADD ][REPEATER_COMMAND]	= scadaData.wordData[i];
				}
				//감지기 예비화재 설정값 ,, D10001~D15000
				//PLC 감지기 인덱스 :: 커맨드[0], 상태(값)[1], 예비화재값[2] , 실화재 값[3], 통신ERR COUNT[4]
				else if(address >= DETECTOR_P_FIRE_ADD_MIN_D10001 && address <= DETECTOR_P_FIRE_ADD_MAX_D15000)		
				{
					if(!Write_Disable)
					{
						m_Detector[ address - FIND_REAL_DETECTOR_P_FIRE_ADD ][DETECTOR_P_FIRE] 	= (u8)(scadaData.wordData[i] & 0x00ff);				
					}
				}
				//감지기 실화재 설정값 ,, D15001~D20000
				else if(address >= DETECTOR_R_FIRE_ADD_MIN_D15001 && address <= DETECTOR_R_FIRE_ADD_MAX_D20000)	
				{
					if(!Write_Disable)
					{
						m_Detector[ address - FIND_REAL_DETECTOR_R_FIRE_ADD ][DETECTOR_R_FIRE] 	= (u8)(scadaData.wordData[i] & 0x00ff);
					}
				}		
				//감지기 값 ,, D20001~D25000 
				else if(address >= DETECTOR_VALUE_ADD_MIN_D20001 && address <= DETECTOR_VALUE_ADD_MAX_D25000)
				{
					m_Detector[ address - FIND_REAL_DETECTOR_VALUE_ADD ][DETECTOR_STATUS] 		= (u8)(scadaData.wordData[i] & 0x00ff);
				}		
				//감지기 상태/제어 값 ,, D25001~D3000
				else if(address >= DETECTOR_COMMAND_ADD_MIN_D25001 && address <= DETECTOR_COMMAND_ADD_MAX_D30000)
				{
					m_Detector[ address - FIND_REAL_DETECTOR_COMMAND_ADD ][DETECTOR_COMMAND] 	= (u8)(scadaData.wordData[i] & 0x00ff);
				}
				// 중계기의 루프넘버 D5001~D6000
				else if(address >= REPEATER_LOOP_ADD_MIN_D5001 && address <= REPEATER_LOOP_ADD_MAX_D6000)
				{					
					m_Repeater[ address - FIND_REAL_REPEATER_LOOP_ADD ][REPEATER_LOOPNUM] 	= scadaData.wordData[i];
				}
				// PLC 중계기당 감지기 갯수	D6001~D7000				
				else if((address >= REP_PER_DEC_COUNT_ADD_MIN_D6001) && (address <= REP_PER_DEC_COUNT_ADD_MAX_D7000))
				{					
					m_Repeater[ address - FIND_REAL_REP_PER_DEC_COUNT_ADD ][REPEATER_DETECTOR_COUNT]= scadaData.wordData[i];
					debugprintf("Rep %d DetCnt = %d\r\n", address - FIND_REAL_REP_PER_DEC_COUNT_ADD, scadaData.wordData[i]);
				}
				// 감지기의 첫번째 어드레스 D7001~D8000
				else if(address >= DETECTOR_FIRST_ADD_MIN && address <= DETECTOR_FIRST_ADD_MAX)
				{					
					m_Repeater[ address - FIND_REAL_DETECTOR_FIRST_ADD ][DETECTOR_START_ADD]= scadaData.wordData[i];
				}
				
				// S-MUX CONFIG
				// 폴링 간격 D9001
				else if(address == SMUX_CONFIG_POLLING_DELAY_ADD)
				{
					if(scadaData.wordData[i] <= 0) {_muxConfig[MUX_CONFIG_POLLING_DELAY] = DEFAULT_POLLING_DELAY_MS;}
					else{_muxConfig[MUX_CONFIG_POLLING_DELAY] = scadaData.wordData[i];}

				}
				// 응답지연 D9002
				else if(address == SMUX_CONFIG_RESPONSE_DELAY_ADD)
				{
					if(scadaData.wordData[i] <= 0) {_muxConfig[MUX_CONFIG_RESPONSE_DELAY] = DEFAULT_RESPONSE_DELAY;}
					else{_muxConfig[MUX_CONFIG_RESPONSE_DELAY] = scadaData.wordData[i];}
					
				}
				// 오프라인 카운트 D9003
				else if(address == SMUX_CONFIG_OFFLINE_COUNT_ADD)
				{
					if(scadaData.wordData[i] <= 0) {_muxConfig[MUX_CONFIG_OFFLINE_COUNT] = DEFAULT_OFFLINE_COUNT;}
					else{_muxConfig[MUX_CONFIG_OFFLINE_COUNT] = scadaData.wordData[i];}
				}
				// 감지기 255 오프라인 카운트 D9004
				else if(address == SMUX_CONFIG_DETECTOR_255_OFFLINE_COUNT_ADD)
				{
					if(scadaData.wordData[i] <= 0) {_muxConfig[MUX_CONFIG_DETECTOR_255_OFFLINE_COUNT] = DEFAULT_DETECTOR_255_OFFLINE_COUNT;}
					else{_muxConfig[MUX_CONFIG_DETECTOR_255_OFFLINE_COUNT] = scadaData.wordData[i];}
				}
				// 올 테스트 D9005
				else if(address == SMUX_CONFIG_ALL_TEST_REP_DET_ADD)
				{
					_muxConfig[MUX_CONFIG_ALL_TEST_REP_DET] = scadaData.wordData[i];
				}
				// 실화재 카운트 D9006
				else if(address == SMUX_CONFIG_R_FIRE_CNT_ADD) 
				{
					_muxConfig[MUX_CONFIG_R_FIRE_CNT] = scadaData.wordData[i];
				}
				// 실화재 카운트 D9007
				else if(address == SMUX_CONFIG_P_FIRE_CNT_ADD) 
				{
					_muxConfig[MUX_CONFIG_P_FIRE_CNT] = scadaData.wordData[i];
				}
				// 실화재 카운트 D9008
				else if(address == SMUX_CONFIG_R_STATUS_CNT_ADD) 
				{
					_muxConfig[MUX_CONFIG_R_STATUS_CNT] = scadaData.wordData[i];
				}

				address++;
			}			
			return scadaData.dataLength;	
		}
		case SCADA_WRITE_BIT :
		{
			for(i = 0 ; i < scadaData.dataLength ; i++ )
			{

				debugprintf("SCADA WRITE Bit : %d\r\n", address);
				//중계기 입력 데이터 ,, D00001~D01000 
				//중계기 인덱스 :: 커맨드[0], 상태[1], 중계기의 루프넘버 [2], PLC 중계기당 감지기 갯수 [3], 통신ERR COUNT[4]
				if(address >= REPEATER_INDATA_ADD_MIN_D00001 && address <= REPEATER_INDATA_ADD_MAX_D01000)
				{
					if(scadaData.bitData[i])
					{SETBIT(m_Repeater[ address - FIND_REAL_REPEATER_INDATA_ADD ][REPEATER_STATUS], (bitAddress + i));}
					else
					{CLEARBIT(m_Repeater[ address - FIND_REAL_REPEATER_INDATA_ADD ][REPEATER_STATUS], (bitAddress + i));}
				}		
				//중계기 출력 데이터 ,, D01001~D02000
				else if(address >= REPEATER_OUTDATA_ADD_MIN_D01001 && address <= REPEATER_OUTDATA_ADD_MAX_D02000)
				{
					if(scadaData.bitData[i])
					{SETBIT(m_Repeater[ address - FIND_REAL_REPEATER_OUTDATA_ADD ][REPEATER_COMMAND], (bitAddress + i));}
					else
					{CLEARBIT(m_Repeater[ address - FIND_REAL_REPEATER_OUTDATA_ADD ][REPEATER_COMMAND], (bitAddress + i));}
				}				
				//감지기 예비화재 설정값 ,, D10001~D15000 (// edit - 2020.05.20 : Write Bit 시 예비설정값 및 실화재 설정값 적용 할 필요 없음)
				//PLC 감지기 인덱스 :: 커맨드[0], 상태(값)[1], 예비화재값[2] , 실화재 값[3], 통신ERR COUNT[4]
				//if(address >= DETECTOR_P_FIRE_ADD_MIN_D10001 && address <= DETECTOR_P_FIRE_ADD_MAX_D15000)		
				//{
				//	if(scadaData.bitData[i])
				//	{SETBIT(m_Detector[ address - FIND_REAL_DETECTOR_P_FIRE_ADD ][DETECTOR_P_FIRE], (bitAddress + i));}
				//	else
				//	{CLEARBIT(m_Detector[ address - FIND_REAL_DETECTOR_P_FIRE_ADD ][DETECTOR_P_FIRE], (bitAddress + i));}					
				//}
				//감지기 실화재 설정값 ,, D15001~D20000
				//if(address >= DETECTOR_R_FIRE_ADD_MIN_D15001 && address <= DETECTOR_R_FIRE_ADD_MAX_D20000)	
				//{
				//	if(scadaData.bitData[i])
				//	{SETBIT(m_Detector[ address - FIND_REAL_DETECTOR_R_FIRE_ADD ][DETECTOR_R_FIRE], (bitAddress + i));}
				//	else
				//	{CLEARBIT(m_Detector[ address - FIND_REAL_DETECTOR_R_FIRE_ADD ][DETECTOR_R_FIRE], (bitAddress + i));}
				//}		
				//감지기 값 ,, D20001~D25000 
				//if(address >= DETECTOR_VALUE_ADD_MIN_D20001 && address <= DETECTOR_VALUE_ADD_MAX_D25000)
				//{
				//	if(scadaData.bitData[i])
				//	{SETBIT(m_Detector[ address - FIND_REAL_DETECTOR_VALUE_ADD ][DETECTOR_STATUS], (bitAddress + i));}
				//	else
				//	{CLEARBIT(m_Detector[ address - FIND_REAL_DETECTOR_VALUE_ADD ][DETECTOR_STATUS], (bitAddress + i));}
				//}		
				//감지기 상태/제어 값 ,, D25001~D3000
				else if(address >= DETECTOR_COMMAND_ADD_MIN_D25001 && address <= DETECTOR_COMMAND_ADD_MAX_D30000)
				{
					if(scadaData.bitData[i])
					{SETBIT(m_Detector[ address - FIND_REAL_DETECTOR_COMMAND_ADD ][DETECTOR_COMMAND], (bitAddress + i));}
					else
					{CLEARBIT(m_Detector[ address - FIND_REAL_DETECTOR_COMMAND_ADD ][DETECTOR_COMMAND], (bitAddress + i));}
					
					if((bitAddress + i) == DETECTOR_CMD_TEST){
						if(TESTBIT(m_Detector[ address - FIND_REAL_DETECTOR_COMMAND_ADD ][DETECTOR_COMMAND], DETECTOR_CMD_TEST)){
							SETBIT(m_Detector[ address - FIND_REAL_DETECTOR_COMMAND_ADD ][DETECTOR_COMMAND], DETECTOR_CMD_RFIRE);
							SETBIT(m_Detector[ address - FIND_REAL_DETECTOR_COMMAND_ADD ][DETECTOR_COMMAND], DETECTOR_CMD_LED);						
						}
					}else if((bitAddress + i) == DETECTOR_CMD_RESTORE){
						if(TESTBIT(m_Detector[ address - FIND_REAL_DETECTOR_COMMAND_ADD ][DETECTOR_COMMAND], DETECTOR_CMD_RESTORE)){
							CLEARBIT(m_Detector[ address - FIND_REAL_DETECTOR_COMMAND_ADD ][DETECTOR_COMMAND], DETECTOR_CMD_RFIRE);
							CLEARBIT(m_Detector[ address - FIND_REAL_DETECTOR_COMMAND_ADD ][DETECTOR_COMMAND], DETECTOR_CMD_PFIRE);
							CLEARBIT(m_Detector[ address - FIND_REAL_DETECTOR_COMMAND_ADD ][DETECTOR_COMMAND], DETECTOR_CMD_TEST);
							CLEARBIT(m_Detector[ address - FIND_REAL_DETECTOR_COMMAND_ADD ][DETECTOR_COMMAND], DETECTOR_CMD_LED);	
							CLEARBIT(m_Detector[ address - FIND_REAL_DETECTOR_COMMAND_ADD ][DETECTOR_COMMAND], DETECTOR_CMD_RESTORE);	
						}
					}else if((bitAddress + i) == DETECTOR_CMD_COMMSTOP){
						// Restore
						CLEARBIT(m_Detector[ address - FIND_REAL_DETECTOR_COMMAND_ADD ][DETECTOR_COMMAND], DETECTOR_CMD_RFIRE);
						CLEARBIT(m_Detector[ address - FIND_REAL_DETECTOR_COMMAND_ADD ][DETECTOR_COMMAND], DETECTOR_CMD_PFIRE);
						CLEARBIT(m_Detector[ address - FIND_REAL_DETECTOR_COMMAND_ADD ][DETECTOR_COMMAND], DETECTOR_CMD_TEST);
						CLEARBIT(m_Detector[ address - FIND_REAL_DETECTOR_COMMAND_ADD ][DETECTOR_COMMAND], DETECTOR_CMD_LED);	
						CLEARBIT(m_Detector[ address - FIND_REAL_DETECTOR_COMMAND_ADD ][DETECTOR_COMMAND], DETECTOR_CMD_RESTORE);						
					}
				}
				// 중계기의 루프넘버 D5001~D6000
				//if(address >= REPEATER_LOOP_ADD_MIN_D5001 && address <= REPEATER_LOOP_ADD_MAX_D6000)
				//{	
				//	if(scadaData.bitData[i])
				//	{SETBIT(m_Repeater[ address - FIND_REAL_REPEATER_LOOP_ADD ][REPEATER_LOOPNUM], (bitAddress + i));}
				//	else
				//	{CLEARBIT(m_Repeater[ address - FIND_REAL_REPEATER_LOOP_ADD ][REPEATER_LOOPNUM], (bitAddress + i));}
				//}
				 //PLC 중계기당 감지기 갯수	D6001~D7000		
				else if(address >= REP_PER_DEC_COUNT_ADD_MIN_D6001 && address <= REP_PER_DEC_COUNT_ADD_MAX_D7000)
				{						
					if(scadaData.bitData[i])
					{SETBIT(m_Repeater[ address - FIND_REAL_REP_PER_DEC_COUNT_ADD ][REPEATER_DETECTOR_COUNT], (bitAddress + i));}
					else
					{CLEARBIT(m_Repeater[ address - FIND_REAL_REP_PER_DEC_COUNT_ADD ][REPEATER_DETECTOR_COUNT], (bitAddress + i));}
				}
				// 감지기의 첫번째 어드레스 D7001~D8000
				//if(address >= DETECTOR_FIRST_ADD_MIN && address <= DETECTOR_FIRST_ADD_MAX)
				//{						
				//	if(scadaData.bitData[i])
				//	{SETBIT(m_Repeater[ address - FIND_REAL_DETECTOR_FIRST_ADD ][DETECTOR_START_ADD], (bitAddress + i));}
				//	else
				//	{CLEARBIT(m_Repeater[ address - FIND_REAL_DETECTOR_FIRST_ADD ][DETECTOR_START_ADD], (bitAddress + i));}
				//}
				// S-MUX CONFIG
				// TIMEOUT ,, D9001
				//if(address == SMUX_CONFIG_POLLING_DELAY_ADD)
				//{
				//	if(scadaData.bitData[i])
				//	{SETBIT(_muxConfig[MUX_CONFIG_POLLING_DELAY], (bitAddress + i));}
				//	else
				//	{CLEARBIT(_muxConfig[MUX_CONFIG_POLLING_DELAY], (bitAddress + i));}
				//}
				// POLLING_DELAY ,, D9002
				//if(address == SMUX_CONFIG_RESPONSE_DELAY_ADD)
				//{
				//	if(scadaData.bitData[i])
				//	{SETBIT(_muxConfig[MUX_CONFIG_RESPONSE_DELAY], (bitAddress + i));}
				//	else
				//	{CLEARBIT(_muxConfig[MUX_CONFIG_RESPONSE_DELAY], (bitAddress + i));}
				//}
				// ALL_TEST_REP_DET ,, D9003
				//if(address == SMUX_CONFIG_OFFLINE_COUNT_ADD)
				//{
				//	if(scadaData.bitData[i])
				//	{SETBIT(_muxConfig[MUX_CONFIG_OFFLINE_COUNT], (bitAddress + i));}
				//	else
				//	{CLEARBIT(_muxConfig[MUX_CONFIG_OFFLINE_COUNT], (bitAddress + i));}
				//}
				// 감지기 255 오프라인 카운트 D9004
				//if(address == SMUX_CONFIG_DETECTOR_255_OFFLINE_COUNT_ADD)
				//{
				//	if(scadaData.bitData[i])
				//	{SETBIT(_muxConfig[MUX_CONFIG_DETECTOR_255_OFFLINE_COUNT], (bitAddress + i));}
				//	else
				//	{CLEARBIT(_muxConfig[MUX_CONFIG_DETECTOR_255_OFFLINE_COUNT], (bitAddress + i));}
				//}
				
				// 올 테스트 D9005
				else if(address == SMUX_CONFIG_ALL_TEST_REP_DET_ADD)
				{
					if(scadaData.bitData[i])
					{SETBIT(_muxConfig[MUX_CONFIG_ALL_TEST_REP_DET], (bitAddress + i));}
					else
					{CLEARBIT(_muxConfig[MUX_CONFIG_ALL_TEST_REP_DET], (bitAddress + i));}
				}
				
			}
			return scadaData.dataLength;
		}
		case SCADA_READ_WORD :
		{	
			for(i = 0 ; i < scadaData.dataLength ; i++ )
			{			
				//중계기 입력 데이터 ,, D00001~D01000 
				//중계기 인덱스 :: 커맨드[0], 상태[1], 중계기의 루프넘버 [2], PLC 중계기당 감지기 갯수 [3], 통신ERR COUNT[4]
				if(address >= REPEATER_INDATA_ADD_MIN_D00001 && address <= REPEATER_INDATA_ADD_MAX_D01000)
				{
					_muxDataBuffer[i] = m_Repeater[ address - FIND_REAL_REPEATER_INDATA_ADD ][REPEATER_STATUS];
				}		
				//중계기 출력 데이터 ,, D01001~D02000
				else if(address >= REPEATER_OUTDATA_ADD_MIN_D01001 && address <= REPEATER_OUTDATA_ADD_MAX_D02000)
				{
					_muxDataBuffer[i] = m_Repeater[ address - FIND_REAL_REPEATER_OUTDATA_ADD ][REPEATER_COMMAND];
				}				
				//감지기 예비화재 설정값 ,, D10001~D15000
				//PLC 감지기 인덱스 :: 커맨드[0], 상태(값)[1], 예비화재값[2] , 실화재 값[3], 통신ERR COUNT[4]
				else if(address >= DETECTOR_P_FIRE_ADD_MIN_D10001 && address <= DETECTOR_P_FIRE_ADD_MAX_D15000)		
				{
					_muxDataBuffer[i] = m_Detector[ address - FIND_REAL_DETECTOR_P_FIRE_ADD ][DETECTOR_P_FIRE];					
				}
				//감지기 실화재 설정값 ,, D15001~D20000
				else if(address >= DETECTOR_R_FIRE_ADD_MIN_D15001 && address <= DETECTOR_R_FIRE_ADD_MAX_D20000)	
				{
					_muxDataBuffer[i] = m_Detector[ address - FIND_REAL_DETECTOR_R_FIRE_ADD ][DETECTOR_R_FIRE];					
				}		
				//감지기 값 ,, D20001~D25000 
				else if(address >= DETECTOR_VALUE_ADD_MIN_D20001 && address <= DETECTOR_VALUE_ADD_MAX_D25000)
				{
					_muxDataBuffer[i] = m_Detector[ address - FIND_REAL_DETECTOR_VALUE_ADD ][DETECTOR_STATUS];					
				}		
				//감지기 상태/제어 값 ,, D25001~D3000
				else if(address >= DETECTOR_COMMAND_ADD_MIN_D25001 && address <= DETECTOR_COMMAND_ADD_MAX_D30000)
				{
					_muxDataBuffer[i] = m_Detector[ address - FIND_REAL_DETECTOR_COMMAND_ADD ][DETECTOR_COMMAND];					
				}
				// 중계기의 루프넘버 D5001~D6000
				else if(address >= REPEATER_LOOP_ADD_MIN_D5001 && address <= REPEATER_LOOP_ADD_MAX_D6000)
				{					
					_muxDataBuffer[i] = m_Repeater[ address - FIND_REAL_REPEATER_LOOP_ADD ][REPEATER_LOOPNUM];
				}
				// PLC 중계기당 감지기 갯수	D6001~D7000			
				else if(address >= REP_PER_DEC_COUNT_ADD_MIN_D6001 && address <= REP_PER_DEC_COUNT_ADD_MAX_D7000)
				{					
					_muxDataBuffer[i] = m_Repeater[ address - FIND_REAL_REP_PER_DEC_COUNT_ADD ][REPEATER_DETECTOR_COUNT];
				}
				// 감지기의 첫번째 어드레스 D7001~D8000
				else if(address >= DETECTOR_FIRST_ADD_MIN && address <= DETECTOR_FIRST_ADD_MAX)
				{					
					_muxDataBuffer[i] = m_Repeater[ address - FIND_REAL_DETECTOR_FIRST_ADD ][DETECTOR_START_ADD];
				}
				// S-MUX CONFIG
				// 폴링 간격 D9001
				else if(address == SMUX_CONFIG_POLLING_DELAY_ADD)
				{
					_muxDataBuffer[i] = _muxConfig[MUX_CONFIG_POLLING_DELAY];
				}
				// 오프라인 카운트 D9002
				else if(address == SMUX_CONFIG_RESPONSE_DELAY_ADD)
				{
					_muxDataBuffer[i] = _muxConfig[MUX_CONFIG_RESPONSE_DELAY];
				}
				// 응답지연 D9003
				else if(address == SMUX_CONFIG_OFFLINE_COUNT_ADD)
				{
					_muxDataBuffer[i] = _muxConfig[MUX_CONFIG_OFFLINE_COUNT];
				}
				// 감지기 255 오프라인 카운트 D9004
				else if(address == SMUX_CONFIG_DETECTOR_255_OFFLINE_COUNT_ADD)
				{
					_muxDataBuffer[i] = _muxConfig[MUX_CONFIG_DETECTOR_255_OFFLINE_COUNT];
				}
				// 올 테스트 D9005
				else if(address == SMUX_CONFIG_ALL_TEST_REP_DET_ADD)
				{
					_muxDataBuffer[i] = _muxConfig[MUX_CONFIG_ALL_TEST_REP_DET];
				}
				// 감지기 실화재 카운트 D9006
				else if(address == SMUX_CONFIG_R_FIRE_CNT_ADD)
				{
					_muxDataBuffer[i] = _muxConfig[MUX_CONFIG_R_FIRE_CNT];
				}
				// 감지기 에비화재 카운트 D9007
				else if(address == SMUX_CONFIG_P_FIRE_CNT_ADD)
				{
					_muxDataBuffer[i] = _muxConfig[MUX_CONFIG_P_FIRE_CNT];
				}

				// 감지기 에비화재 카운트 D9008
				else if(address == SMUX_CONFIG_R_STATUS_CNT_ADD)
				{
					_muxDataBuffer[i] = _muxConfig[MUX_CONFIG_R_STATUS_CNT];
				}

				
				address++;
			}			
			return scadaData.dataLength;
		}
		case SCADA_READ_BIT :
		{
			return 0;
		}
		default :
		{
			return 0;
		}
				
	}
	return 0;
}



void SMUX_RS232_UART_ISR_(void)
{
	uint32_t iir;
	uint8_t ch;
	
	iir = *(volatile uint32_t*)(RS232_UART_BASE + UART_IIR);
	iir &= 0xf;
	
	switch(iir) {
		case 6:
			//UART Line Status Error(Overrun,Frame,Parity)
		case 4:
		case 0xc:
			//data ready
			while((*(volatile uint32_t*)(RS232_UART_BASE + UART_LSR)) & ULSR_DRDY) 
			{
				ch = *(volatile uint8_t*)(RS232_UART_BASE+UART_RBR);

				switch(SCADA_RX_STATE)
				{
					case 0:
						if(ch == SCADA_MASTER_ENQ)
						{
							SCADA_RX_STATE++;
							SCADA_RX_POS = 0;
							SCADA_RX_DATA[SCADA_RX_POS++] = ch;
						}
						break;
			
					default:
						if(ch == SCADA_MASTER_EOT)
						{
							SCADA_RXD = 1;
							SCADA_RX_STATE = 0;
							SCADA_RX_DATA[SCADA_RX_POS++] = ch;
						}else
						{
							SCADA_RX_DATA[SCADA_RX_POS++] = ch;
							if(SCADA_RX_POS >= (DEF_SCADA_RX_LENGTH -3))
							{
								SCADA_RX_STATE = 0;
							}
						}
						break;
				}

			}
			break;
		default:
			ch = *(volatile uint8_t*)(RS232_UART_BASE+UART_RBR);				
			break;
	}
}




/*

u32 Scada_MuxDataApplyB(TS_SCADA_DATA scadaData,  u16 m_Repeater[REPEATER_MAX][REPEATER_INFO], u8 m_Detector[PLC_DETECTOR_MAX][PLC_DETECTOR_INFO], u16* _muxDataBuffer)
{
	u32 i;	
	u32 realAddress;
	u32 address 	= scadaData.startAddress;
	u32 bitAddress 	= scadaData.startBitNumber;
	for(i = 0 ; i < scadaData.dataLength ; i++ )
	{
		//중계기 입력 데이터 ,, D00001~D01000 
		//중계기 인덱스 :: 커맨드[0], 상태[1], 중계기의 루프넘버 [2], PLC 중계기당 감지기 갯수 [3], 통신ERR COUNT[4]
		
		if(address >= REPEATER_INDATA_ADD_MIN_D00001 && address <= REPEATER_INDATA_ADD_MAX_D01000)
		{
			realAddress = address - FIND_REAL_REPEATER_INDATA_ADD;
			switch(scadaData.command)
			{
				case SCADA_WRITE_WORD :	{m_Repeater[realAddress][REPEATER_STATUS]	= scadaData.wordData[i]; break;}
				case SCADA_WRITE_BIT :	
				{
					if(scadaData.bitData[i]){SETBIT(m_Repeater[realAddress][REPEATER_STATUS], (bitAddress + i));}
					else{CLEARBIT(m_Repeater[realAddress][REPEATER_STATUS], (bitAddress + i));}
					break;
				}
				case SCADA_READ_WORD :	{_muxDataBuffer[i] = m_Repeater[realAddress][REPEATER_STATUS]; break;}
				case SCADA_READ_BIT :{return 0;}
				default :{ return 0;}
			}
			
		}		
		//중계기 출력 데이터 ,, D01001~D02000
		if(address >= REPEATER_OUTDATA_ADD_MIN_D01001 && address <= REPEATER_OUTDATA_ADD_MAX_D02000)
		{
			realAddress = address - FIND_REAL_REPEATER_OUTDATA_ADD;
			switch(scadaData.command)
			{
				case SCADA_WRITE_WORD :	{m_Repeater[realAddress][REPEATER_COMMAND]	= scadaData.wordData[i]; break;}
				case SCADA_WRITE_BIT :	
				{
					if(scadaData.bitData[i]){SETBIT(m_Repeater[realAddress][REPEATER_COMMAND], (bitAddress + i));}
					else{CLEARBIT(m_Repeater[realAddress][REPEATER_COMMAND], (bitAddress + i));}
					break;
				}
				case SCADA_READ_WORD :	{_muxDataBuffer[i] = m_Repeater[realAddress][REPEATER_COMMAND]; break;}
				case SCADA_READ_BIT :{return 0;}
				default :{ return 0;}
			}
			
		}
		//감지기 예비화재 설정값 ,, D10001~D15000
		//PLC 감지기 인덱스 :: 커맨드[0], 상태(값)[1], 예비화재값[2] , 실화재 값[3], 통신ERR COUNT[4]
		if(address >= DETECTOR_P_FIRE_ADD_MIN_D10001 && address <= DETECTOR_P_FIRE_ADD_MAX_D15000)		
		{
			realAddress = address - FIND_REAL_DETECTOR_P_FIRE_ADD;
			switch(scadaData.command)
			{
				case SCADA_WRITE_WORD :	{m_Detector[realAddress][DETECTOR_P_FIRE] = (u8)(scadaData.wordData[i] & 0x00ff); break;}
				case SCADA_WRITE_BIT :	
				{
					if(scadaData.bitData[i]){SETBIT(m_Detector[realAddress][DETECTOR_P_FIRE], (bitAddress + i));}
					else{CLEARBIT(m_Detector[realAddress][DETECTOR_P_FIRE], (bitAddress + i));}
					break;
				}
				case SCADA_READ_WORD :	{_muxDataBuffer[i] = m_Detector[realAddress][DETECTOR_P_FIRE]; break;}
				case SCADA_READ_BIT :{return 0;}
				default :{ return 0;}
			}
			
		}
		//감지기 실화재 설정값 ,, D15001~D20000
		if(address >= DETECTOR_R_FIRE_ADD_MIN_D15001 && address <= DETECTOR_R_FIRE_ADD_MAX_D20000)	
		{
			realAddress = address - FIND_REAL_DETECTOR_R_FIRE_ADD;
			switch(scadaData.command)
			{
				case SCADA_WRITE_WORD :	{m_Detector[realAddress][DETECTOR_R_FIRE] = (u8)(scadaData.wordData[i] & 0x00ff); break;}
				case SCADA_WRITE_BIT :	
				{
					if(scadaData.bitData[i]){SETBIT(m_Detector[realAddress][DETECTOR_R_FIRE], (bitAddress + i));}
					else{CLEARBIT(m_Detector[realAddress][DETECTOR_R_FIRE], (bitAddress + i));}
					break;
				}
				case SCADA_READ_WORD :	{_muxDataBuffer[i] = m_Detector[realAddress][DETECTOR_R_FIRE]; break;}
				case SCADA_READ_BIT :{return 0;}
				default :{ return 0;}
			}
		}		
		//감지기 값 ,, D20001~D25000 
		if(address >= DETECTOR_VALUE_ADD_MIN_D20001 && address <= DETECTOR_VALUE_ADD_MAX_D25000)
		{
			realAddress = address - FIND_REAL_DETECTOR_VALUE_ADD;
			switch(scadaData.command)
			{
				case SCADA_WRITE_WORD :	{m_Detector[realAddress][DETECTOR_STATUS] = (u8)(scadaData.wordData[i] & 0x00ff); break;}
				case SCADA_WRITE_BIT :	
				{
					if(scadaData.bitData[i]){SETBIT(m_Detector[realAddress][DETECTOR_STATUS], (bitAddress + i));}
					else{CLEARBIT(m_Detector[realAddress][DETECTOR_STATUS], (bitAddress + i));}
					break;
				}
				case SCADA_READ_WORD :	{_muxDataBuffer[i] = m_Detector[realAddress][DETECTOR_STATUS]; break;}
				case SCADA_READ_BIT :{return 0;}
				default :{ return 0;}
			}
		}		
		//감지기 상태/제어 값 ,, D25001~D3000
		if(address >= DETECTOR_COMMAND_ADD_MIN_D25001 && address <= DETECTOR_COMMAND_ADD_MAX_D26000)
		{
			realAddress = address - FIND_REAL_DETECTOR_COMMAND_ADD;
			switch(scadaData.command)
			{
				case SCADA_WRITE_WORD :	{m_Detector[realAddress][DETECTOR_COMMAND] = (u8)(scadaData.wordData[i] & 0x00ff); break;}
				case SCADA_WRITE_BIT :	
				{
					if(scadaData.bitData[i]){SETBIT(m_Detector[realAddress][DETECTOR_COMMAND], (bitAddress + i));}
					else{CLEARBIT(m_Detector[realAddress][DETECTOR_COMMAND], (bitAddress + i));}
					break;
				}
				case SCADA_READ_WORD :	{_muxDataBuffer[i] = m_Detector[realAddress][DETECTOR_COMMAND]; break;}
				case SCADA_READ_BIT :{return 0;}
				default :{ return 0;}
			}
		}
		// 중계기의 루프넘버
		if(address >= REPEATER_LOOP_ADD_MIN_D30001 && address <= REPEATER_LOOP_ADD_MIN_D31000)
		{
			realAddress = address - FIND_REAL_REPEATER_LOOP_ADD;
			switch(scadaData.command)
			{
				case SCADA_WRITE_WORD :	{m_Repeater[realAddress][REPEATER_LOOPNUM]	= scadaData.wordData[i]; break;}
				case SCADA_WRITE_BIT :	
				{
					if(scadaData.bitData[i]){SETBIT(m_Repeater[realAddress][REPEATER_LOOPNUM], (bitAddress + i));}
					else{CLEARBIT(m_Repeater[realAddress][REPEATER_LOOPNUM], (bitAddress + i));}
					break;
				}
				case SCADA_READ_WORD :	{_muxDataBuffer[i] = m_Repeater[realAddress][REPEATER_LOOPNUM]; break;}
				case SCADA_READ_BIT :{return 0;}
				default :{ return 0;}
			}
		}
		// PLC 중계기당 감지기 갯수				
		if(address >= REP_PER_DEC_COUNT_ADD_MIN_D31001 && address <= REP_PER_DEC_COUNT_ADD_MIN_D32000)
		{					
			realAddress = address - FIND_REAL_REP_PER_DEC_COUNT_ADD;
			switch(scadaData.command)
			{
				case SCADA_WRITE_WORD :	{m_Repeater[realAddress][REPEATER_DETECTOR_COUNT]	= scadaData.wordData[i]; break;}
				case SCADA_WRITE_BIT :	
				{
					if(scadaData.bitData[i]){SETBIT(m_Repeater[realAddress][REPEATER_DETECTOR_COUNT], (bitAddress + i));}
					else{CLEARBIT(m_Repeater[realAddress][REPEATER_DETECTOR_COUNT], (bitAddress + i));}
					break;
				}
				case SCADA_READ_WORD :	{_muxDataBuffer[i] = m_Repeater[realAddress][REPEATER_DETECTOR_COUNT]; break;}
				case SCADA_READ_BIT :{return 0;}
				default :{ return 0;}
			}
		}
		address++;
	}
	return scadaData.dataLength;
}
*/
