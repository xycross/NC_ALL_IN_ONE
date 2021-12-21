
#include "cantus.h"
#include "norFlash_Control.h"


#include "globalDefine.h"
#include "smux_main.h"
#include "HalBoard.h"
#include "commOpticSwitch.h"
#include "HIC_Optic_COM.h"

#include "HalUart.h"

#include "cimon-PLC-RS232_422_485.h"


#include "Cli_n.h"

TS_SCADA_DATA scadaData;




	
u16 m_Repeater[REPEATER_MAX][REPEATER_INFO];			// 중계기(word형)
														// 	[0] command
														//	[1] status 	
														//	[2] 중계기의 루프넘버
														//	[3] PLC 중계기당 감지기 갯수
														//	[4] COM Err count
														//	[5] COM delay count {b0000 0000 1000 0000 - 7번째 비트가 set되야 카운트 시작}
														//  [6] 감지기 첫번째 어드레스
														//  [7] status cnt

	
	
U8 	m_Detector[PLC_DETECTOR_MAX][PLC_DETECTOR_INFO];	// 감지기(char형)
														// 	[0] command
														//	[1] status/value 	
														//	[2] 예비화재값
														//	[3] 실화재값
														//	[4] COM Err count
														//	[5] COM delay count {b1000 0000 - 7번째 비트가 set되야 카운트 시작}
														//  [6] 감지기 값이 255일때 Offline 카운트
														//	[7]	에비화재 카운트
														//  [8] 실화재 카운트 
	
U16 m_MuxConfig[MUX_CONFIG];							// S-MUX Config,,
														// [0] 중계기/감지기 Polling 간격 설정(ms)
														// [1] 중계기/감지기 응답지연 설정 (전체 폴링을 기준,, 2라고 하면 2번의 폴링시간동안 응답하지 않음)
														// [2] 감지기 오프라인(255) 카운트 리미트
														// [3] 중계기/감지기 offline 설정 (응답지연이 offline 판정 카운트를 넘어섰을때 Offline 판정한다)
														// [4].0 S-MUX 재부팅
														// [4].1 전체 시스템 회로시험 ON/OFF(각 S-MUX 계통만 됨)
														// [4].2 전체 시스템 출력 ON/OFF(각 S-MUX 계통만 됨)
														// [4].3 전체 감지기 LED ON/OFF(각 S-MUX 계통만 됨)
														// [4].4 플래쉬 메모리 삭제
														// [4].5
														// [4].6
														// [4].7

U8 FLASH_BUFFER[PLC_DETECTOR_MAX * 3 + 2 * 3];





//scada data receive variable
//u8  m_Rxbuffer[MAXBUFFER];
u32	m_ScadaByteToByteTimeCount 	= 0;
u32	m_ScadaReceiveCount 		= 0;
u32	m_ScadaDataReceive 			= 0;
u32 m_ScadaWriteTimerCount		= 0;
bool m_ScadaWriteTimerFlag		= false;
bool m_ScadaReceiveFlag = false;

//HIC data receive variable
u32	m_REP_DET_ByteToByteTimeCount 	= 0;
u32	m_REP_DET_ReceiveCount 			= 0;
u32	m_REP_DET_DataReceive 			= 0;

//어드레스 
TS_RotarySwitchValue m_Address;


BOOL m_RunLEDState	;
BOOL m_SMUXConfigFlag;
u16  m_SMUXConfigCompare;

u16 Rotary_SMUX_WHERE;


extern uint8_t ledSleep[LED_MAX][LED_TRX_MAX];
extern uint8_t ledFlag[LED_MAX][LED_TRX_MAX];

extern TS_RotarySwitchValue rotarySwitch;

extern _DET_INFO DET_INFO;

extern u8 SCADA_RXD;

u8 Comm_TimeOut;

u8 OPT_RX_BUFFER[100], OPT_RX_COUNT, OPT_RX_FLAG, OPT_RX_STATE;
u8 OPT_RAW_RX_BUFFER[200], OPT_RAW_RX_COUNT;

u8 Opt_Tx_Buffer[15];

u8 DEBUG_MODE;


//U8 Det_Info_Buffer[PLC_DETECTOR_MAX * 3];
U8 Write_Disable;
U16 REP_POLL_ADDR;


U16 Cur_Find_Next, Finded_Next_Poll_No;
U8 Find_Next_Flag;

int smux_main(void)
{	
//	main_SystemInit();
	int i = 0;
	int j = 0;
	int detectorCount;
	int detectorIndex;
	int follingCount = 0;

	TS_REP_DET_DATA rep;

	u8 step_switch = E_REP_TX;

	


//	*R_WDTCTRL = 0;

	smux_main_SystemInit();
//	main_VariableInit();


	
	// 중계기 데이터 로드
	
//	main_RepeaterDataLoad();
	main_WatchDog(0xffffffff,SET);

/*
	while(1)
	{
			main_WatchDog(0xffffffff, CLR);	
	}

*/
	
	while(1)
	{	

		switch(step_switch)
		{
			case E_REP_TX:
//				debugprintf("E_REP_TX %d\r\n", REP_POLL_ADDR);
//				if((m_Address.RotarySwitchValue == m_Repeater[REP_POLL_ADDR][REPEATER_LOOPNUM]) && (m_Address.RotarySwitchValue != 0))
				if(Rotary_SMUX_WHERE == m_Repeater[REP_POLL_ADDR][REPEATER_LOOPNUM])
				{
					if(!TESTBIT(m_Repeater[REP_POLL_ADDR][REPEATER_COMMAND], 15))
					{
						OPT_RX_STATE = 0;
						OPT_RX_FLAG = 0;
						OPT_RAW_RX_COUNT = 0;

						Comm_TimeOut = 0;
//						debugprintf("TX : %d\r\n", REP_POLL_ADDR);
						Find_Next_Flag = 0;
						Cur_Find_Next = REP_POLL_ADDR;
						Make_Tx_Packet(REP_POLL_ADDR, 0);
						settimer(HIC_COM_DELAY_TIMER, m_MuxConfig[MUX_CONFIG_POLLING_DELAY]);
						
						step_switch = E_REP_WAIT;
						
					}
				}
				else
				{
//					step_switch = E_REP_ADDR_UP;
					
					step_switch = E_DET_TX_READY;
				}
				
				break;

			case E_REP_WAIT:
				if(OPT_RX_FLAG)
				{
					OPT_RX_FLAG = 0;
//					debugstring("RXD\r\n");

					rep = Opt_Rx_Parsing();

					if(rep.check)
					{
						stoptimer(HIC_COM_DELAY_TIMER);
						LED_SET(LED_SPOF3, LED_RX);
						HIC_MuxDataApply(rep, m_MuxConfig);
						step_switch = E_DET_TX_READY;
					}
					else
					{
						
						if(DEBUG_MODE)
						{
							debugstring("OPT : ");
							for(i=0;i<OPT_RX_COUNT;i++)
							{
								debugprintf("%02X ", OPT_RX_BUFFER[i]);
							}
							debugstring("\r\n");
						}
					}

					
				}
				else if(Comm_TimeOut)
				{
					Comm_TimeOut = 0;
					if(DEBUG_MODE)
					{
						if(OPT_RAW_RX_COUNT)
						{
							debugstring("OPT RAW : ");
							for(i=0;i<OPT_RAW_RX_COUNT;i++)
							{
								debugprintf("%02X ", OPT_RAW_RX_BUFFER[i]);
							}
							debugstring("\r\n");
						}
					}
					
//					SETBIT(m_Repeater[REP_POLL_ADDR][REPEATER_COM_DELAY_COUNT], 7);
//					if(++m_Repeater[REP_POLL_ADDR][REPEATER_COM_DELAY_COUNT] > (m_MuxConfig[MUX_CONFIG_RESPONSE_DELAY] + 127))
					if(++m_Repeater[REP_POLL_ADDR][REPEATER_COM_DELAY_COUNT] > (m_MuxConfig[MUX_CONFIG_RESPONSE_DELAY]))
					{
						m_Repeater[REP_POLL_ADDR][REPEATER_COM_DELAY_COUNT] = CLR;				
						if(++m_Repeater[REP_POLL_ADDR][REPEATER_COM_ERR] >= m_MuxConfig[MUX_CONFIG_OFFLINE_COUNT])
						{
						
							m_Repeater[REP_POLL_ADDR][REPEATER_COM_ERR] = CLR;
							SETBIT(m_Repeater[REP_POLL_ADDR][REPEATER_STATUS],15);
						}
					}

					step_switch = E_DET_TX_READY;
				}
				break;

			case E_DET_TX_READY:
				if(TESTBIT(m_Repeater[REP_POLL_ADDR][REPEATER_DETECTOR_COUNT],7))
				{
					detectorCount = m_Repeater[REP_POLL_ADDR][REPEATER_DETECTOR_COUNT] & 0x7f;
					j = 1;
					step_switch = E_DET_TX;
				}
				else
				{
					step_switch = E_REP_ADDR_UP;
				}
				break;

			case E_DET_TX:
				DET_INFO.index = HIC_FindDetectorIndex(REP_POLL_ADDR, j);
				DET_INFO.no = j;
				DET_INFO.command = m_Detector[detectorIndex][DETECTOR_COMMAND];

//				if(!(TESTBIT(m_Detector[detectorIndex][DETECTOR_COMMAND],7)))
				if(!(TESTBIT(DET_INFO.command, 7)))
				{
						OPT_RX_STATE = 0;
						OPT_RX_FLAG = 0;
						OPT_RAW_RX_COUNT = 0;

						Comm_TimeOut = 0;
//						debugprintf("TX : %d\r\n", REP_POLL_ADDR);
						Find_Next_Flag = 0;
						Cur_Find_Next = REP_POLL_ADDR;
						Make_Tx_Packet(REP_POLL_ADDR, 1);
						settimer(HIC_COM_DELAY_TIMER, m_MuxConfig[MUX_CONFIG_POLLING_DELAY]);

						step_switch = E_DET_WAIT;
											
				}
				else
				{
//					step_switch = E_REP_ADDR_UP;
					
					step_switch = E_DET_ADDR_UP;
				}
				
				break;


			case E_DET_WAIT:

				if(OPT_RX_FLAG)
				{
					OPT_RX_FLAG = 0;
//					debugstring("RXD\r\n");

					rep = Opt_Rx_Parsing();

					if(rep.check)
					{
						stoptimer(HIC_COM_DELAY_TIMER);
						LED_SET(LED_SPOF3, LED_RX);
						HIC_MuxDataApply(rep, m_MuxConfig);
						step_switch = E_DET_ADDR_UP;
					}
					else
					{
						
						if(DEBUG_MODE)
						{
							debugstring("OPT : ");
							for(i=0;i<OPT_RX_COUNT;i++)
							{
								debugprintf("%02X ", OPT_RX_BUFFER[i]);
							}
							debugstring("\r\n");
						}
					}

					
				}
				else if(Comm_TimeOut)
				{
					Comm_TimeOut = 0;
					if(DEBUG_MODE)
					{
						if(OPT_RAW_RX_COUNT)
						{
							debugstring("OPT RAW : ");
							for(i=0;i<OPT_RAW_RX_COUNT;i++)
							{
								debugprintf("%02X ", OPT_RAW_RX_BUFFER[i]);
							}
							debugstring("\r\n");
						}
					}

				
//					if(++m_Detector[detectorIndex][DETECTOR_COM_DELAY_COUNT] > m_MuxConfig[MUX_CONFIG_RESPONSE_DELAY]+127)	// 1000 0000 flag bit +
					if(++m_Detector[DET_INFO.index][DETECTOR_COM_DELAY_COUNT] > m_MuxConfig[MUX_CONFIG_RESPONSE_DELAY])	// 1000 0000 flag bit +
					{
						m_Detector[DET_INFO.index][DETECTOR_COM_DELAY_COUNT] = CLR;
						if(++m_Detector[DET_INFO.index][DETECTOR_COM_ERR] >= m_MuxConfig[MUX_CONFIG_OFFLINE_COUNT])
						{
							m_Detector[DET_INFO.index][DETECTOR_COM_ERR] = CLR;
							SETBIT(m_Detector[DET_INFO.index][DETECTOR_COMMAND],6);
						}							
					}

					step_switch = E_DET_ADDR_UP;
				}

				break;


			case E_DET_ADDR_UP:
				if(++j > detectorCount)
				{
					step_switch = E_REP_ADDR_UP;
				}
				else
				{
					step_switch = E_DET_TX;
				}
				break;
				

			case E_REP_ADDR_UP:
				if(Find_Next_Flag)
				{
//					debugstring("F ");
					REP_POLL_ADDR = Finded_Next_Poll_No;
				}
				else
				{
					if(++REP_POLL_ADDR >= REPEATER_MAX)
					{
						REP_POLL_ADDR = 0;
					}
				}
				//cli_loop(1);
				step_switch = E_REP_TX;
				break;
		}

		cli_loop(1);

		Find_Next();


		Scada_Processing();


/*
		if(SCADA_RXD)
		{
			SCADA_RXD = 0;
			Scada_Parser(SCADA_RX_DATA);

			if(scadaData.check)
			{
				Scada_MuxResponse
			}
			Scada_MuxResponse(u8 * _txBuffer,u16 _muxConfig [ MUX_CONFIG ])
		}
*/
/*
		if(m_ScadaReceiveFlag)
		{
			m_ScadaReceiveFlag = false;
			UartPutData(SCADA_COM_CH, m_Rxbuffer, Scada_MuxResponse(m_Rxbuffer, m_MuxConfig));
			LED_SET(LED_RS232, LED_TX);
		}
*/

/*		
		main_HICDelayCheck();
		follingCount = CLR;
		for(i = 0 ; i < REPEATER_MAX ; i++)
		{
			if(m_ScadaReceiveFlag)
			{
				m_ScadaReceiveFlag = false;
				UartPutData(SCADA_COM_CH, m_Rxbuffer, Scada_MuxResponse(m_Rxbuffer, m_MuxConfig));
				LED_SET(LED_RS232, LED_TX);
			}
			if(m_Address.RotarySwitchValue == m_Repeater[i][REPEATER_LOOPNUM] && m_Address.RotarySwitchValue != 0 )
			{
				if(!(TESTBIT(m_Repeater[i][REPEATER_COMMAND], 15)))
				{
					if(UartPutData(REP_DET_COM_CH, txBuffer, HIC_GetRepeaterStatus(i, m_Repeater[i][REPEATER_COMMAND], txBuffer)))
					{
						LED_SET(LED_SPOF3, LED_TX);
						SETBIT(m_Repeater[i][REPEATER_COM_DELAY_COUNT], 7);		// 타이머 카운트 flag	
						follingCount++;
					}
					delayms(m_MuxConfig[MUX_CONFIG_POLLING_DELAY]);
					if(m_Repeater[i][REPEATER_DETECTOR_COUNT] < 0x00ff) // 7번째 비트는 plc중계기 판단,, 하위 비트 7개는 감지기 수량 
					{
						if(TESTBIT(m_Repeater[i][REPEATER_DETECTOR_COUNT],7))
						{
							detectorCount = (m_Repeater[i][REPEATER_DETECTOR_COUNT] & 0x007f);
							for(j = 1 ; j < detectorCount + 1; j++)			// 감지기는 0번지 쓰지 않음
							{
								detectorIndex = HIC_FindDetectorIndex(i, j);
								if(!(TESTBIT(m_Detector[detectorIndex][DETECTOR_COMMAND],7)))
								{						
									if(UartPutData(REP_DET_COM_CH, txBuffer, HIC_GetDetectorStatus(i, j, m_Detector[detectorIndex][DETECTOR_COMMAND], txBuffer)))
									{
										LED_SET(LED_SPOF3, LED_TX);
										SETBIT(m_Detector[detectorIndex][DETECTOR_COM_DELAY_COUNT], 7);		// 타이머 카운트 flag
										follingCount++;
									}
									if(m_ScadaReceiveFlag)
									{
										m_ScadaReceiveFlag = false;
										UartPutData(SCADA_COM_CH, m_Rxbuffer, Scada_MuxResponse(m_Rxbuffer, m_MuxConfig));
										LED_SET(LED_RS232, LED_TX);
									}
									delayms(m_MuxConfig[MUX_CONFIG_POLLING_DELAY]);
								}							
							}
						}											
					}
				}
			}
		}
*/
		main_WatchDog(0xffffffff, CLR);

//		main_WatchDog((m_MuxConfig[MUX_CONFIG_POLLING_DELAY] * follingCount) + 10000, CLR);
		main_SMUXConfig();
		
	}
	
	return 0;
}


void smux_main_SystemInit()
{

#if 0	
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
	
	//*R_P3RED = 0x0f;			// rising Edge set
	//*R_P3FED = 0x0f;			// falling Edge set
	
#endif	
	// 변수 초기화
	main_VariableInit();
	
	// 중계기 데이터 로드
	main_RepeaterDataLoad();
	
	//인터럽트 초기화
	InitInterrupt();


	myUartConfig(DEBUG_UART_CH, DEBUG_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);

	myUartConfig(SCADA_COM_CH, DEBUG_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);

	
	// Uart 초기화
//	UartConfig(SCADA_COM_CH, SCADA_COM_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);
//	UartConfig(DEBUG_RS232_CH, DEBUG_RS232_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);
//	UartConfig(MUX_OPTIC_CH_4, MUX_OPTIC_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);
	SMux_UartConfig(SPOF3_UART_CH, MUX_OPTIC_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);

//	UartConfig(MUX_OPTIC_CH_5, MUX_OPTIC_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);
//	UartConfig(MUX_OPTIC_CH_6, MUX_OPTIC_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);
//	UartConfig(MUX_OPTIC_CH_7, MUX_OPTIC_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);

	debugstring("End Uart\r\n");
	
	// 타이머 초기화
//	settimer_us(SCADA_COM_TIMER,SCADA_COM_TIMER_us);		// Timer0 : 1ms ,, SCADA COM
//	setinterrupt(INTNUM_TIMER0,_SDATA_DATA_RECEIVE_TIMER_0_ISR_);	
	
//	settimer_us(REP_DET_COM_TIMER,REP_DET_COM_TIMER_us);	// Timer1 : 100us ,, HIC COM
//	setinterrupt(INTNUM_TIMER1,_REP_DET_DATA_RECEIVE_TIMER_1_ISR_);	

//	settimer(HIC_COM_DELAY_TIMER,HIC_COM_DELAY_TIMER_ms);		// Timer2 : 1ms ,, HIC COM delay
//	setinterrupt(INTNUM_TIMER2,_HIC_COM_DELAY_TIMER_2_ISR_);	

	settimer(HIC_COM_DELAY_TIMER, m_MuxConfig[MUX_CONFIG_POLLING_DELAY]);
	setinterrupt(INTNUM_TIMER1, HIC_COMM_Delay_Timer_ISR);

	stoptimer(HIC_COM_DELAY_TIMER);

	
	settimer(SYSTEM_TIMER, SYSTEM_TIMER_ms);	// System Timer 10ms	4
	setinterrupt(INTNUM_TIMER2, Smux_System_Timer_ISR);		

	settimer(MUX_RUN_TIMER,MUX_RUN_TIMER_ms);		//
	setinterrupt(INTNUM_TIMER3,_MUX_RUN_TIMER_3_ISR_);	

	debugstring("End Timer\r\n");
	// 어드레스 GET
    m_Address = RotaySwitchDetect();
	if(m_Address.RotarySwitchValue == 999)
	{
		debugstring("999 Erase\r\n");
//		Flash_Erase(6);				// 어드레스가 999이면 플래쉬 메모리 섹터 6을 지운다
//		main_RepeaterDataLoad();	// 중계기 데이터 로드
	}



	Rotary_SMUX_WHERE = m_Address.RotarySwitchValue;
	if(m_Address.RotarySwitchValue >= 100)
	{
		DEBUG_MODE = 1;
		debugstring("Debug Mode\r\n");
	//	m_Address.RotarySwitchValue -= 100;
		Rotary_SMUX_WHERE -= 100; 
	}

	
	main_StatusLEDControl(5,100);
	debugstring("CANTUS S-MUX V0.96\r\n");
	delayms(100);
	debugprintf("m_Address [%d]\r\n", m_Address.RotarySwitchValue);
	if(DEBUG_MODE)
	{
		debugstring("DEBUG Mode Enable\r\n");
	}
	delayms(1000);
	
	
	
	
}


/*

void _SDATA_DATA_RECEIVE_TIMER_0_ISR_(void)
{
	stoptimer(SCADA_COM_TIMER);
	u32 length;
	length = UartGetDataSize(SCADA_COM_CH);
	if(m_ScadaReceiveCount != length) 
	{
		m_ScadaByteToByteTimeCount 	= CLR;					
		m_ScadaReceiveCount			= length; 
	}	
	if(m_ScadaByteToByteTimeCount > SCADA_COM_BYTE2BYTE_INTERVAL)  // 5ms
	{
		m_ScadaReceiveCount			= CLR;
		m_ScadaByteToByteTimeCount 	= CLR;			
		UartGetData(SCADA_COM_CH, m_Rxbuffer, length);
		Scada_Parser(m_Rxbuffer);
		if(scadaData.check)
		{
			//UartPutData(SCADA_COM_CH, m_Rxbuffer, Scada_MuxResponse(ts_scadadata, m_Rxbuffer, m_Repeater, m_Detector, m_MuxConfig));
			m_ScadaReceiveFlag = true;
			LED_SET(LED_RS232, LED_RX);
			if(scadaData.startAddress >= 5001 && scadaData.startAddress <= 9004)
			{
				if(scadaData.command == 'w' || scadaData.command == 'W')
				{
					//main_RepeaterDataSave();
					m_ScadaWriteTimerCount 	= CLR;
					m_ScadaWriteTimerFlag	= true;
				}				
			}			
		}
	}
	if(m_ScadaReceiveCount) 
	{
		m_ScadaByteToByteTimeCount++;
	}	
	settimer_us(SCADA_COM_TIMER,SCADA_COM_TIMER_us);		//1m

}
*/



void Find_Next(void)
{
	if(!Find_Next_Flag)
	{
		if(Cur_Find_Next > 999)
		{
			Cur_Find_Next = 0;
		}
		else
		{
			Cur_Find_Next++;
		}
		if(Rotary_SMUX_WHERE == m_Repeater[Cur_Find_Next][REPEATER_LOOPNUM])
		{
			Find_Next_Flag = 1;
			Finded_Next_Poll_No = Cur_Find_Next;
		}
	}
}

void _REP_DET_DATA_RECEIVE_TIMER_1_ISR_()
{
	stoptimer(REP_DET_COM_TIMER);
//	u32 i;
	u32 length;
	u32 data;
	u8 buffer[MAXBUFFER];
	length = UartGetDataSize(REP_DET_COM_CH);

	if(m_REP_DET_ReceiveCount != length) 
	{
		m_REP_DET_ByteToByteTimeCount 	= CLR;					
		m_REP_DET_ReceiveCount			= length; 
	}	
	if(m_REP_DET_ReceiveCount) 
	{
		m_REP_DET_ByteToByteTimeCount++;
	}	
	if(m_REP_DET_ByteToByteTimeCount > REP_DET_COM_BYTE2BYTE_INTERVAL)  // 5ms
	{
		m_REP_DET_ReceiveCount			= CLR;
		m_REP_DET_ByteToByteTimeCount 	= CLR;

		UartGetData(REP_DET_COM_CH, buffer, length);
		TS_REP_DET_DATA rep_det_data = HIC_Parser(buffer, length);
		if(rep_det_data.check)
		{
			data = HIC_MuxDataApply( rep_det_data, m_MuxConfig);		
//debugprintf("repeater[%d]  data [%x]",rep_det_data.repeaterAddress, data);
		}
/*
else
{
	//debugprintf("err\r\n");
	for(i = 0 ; i < length ; i++)
	{
		debugprintf("%x ", buffer[i]);
		
	}
}
*/
	}
	settimer_us(REP_DET_COM_TIMER,REP_DET_COM_TIMER_us);		//100us

}
	
void _HIC_COM_DELAY_TIMER_2_ISR_()
{	
	stoptimer(HIC_COM_DELAY_TIMER);	
	u32 i, j;
	u32 detectorCount;
	u32 detectorIndex;
	for(i = 1 ; i < REPEATER_MAX ; i++)
	{
		if(m_Address.RotarySwitchValue == m_Repeater[i][REPEATER_LOOPNUM])
		{
			if(TESTBIT(m_Repeater[i][REPEATER_COM_DELAY_COUNT],7))
			{
//debugprintf("OFF?? R[%d]",i);
				if(++m_Repeater[i][REPEATER_COM_DELAY_COUNT] > HIC_COM_TIMEOUT+127)	// 1000 0000 flag bit +
				{
					m_Repeater[i][REPEATER_COM_DELAY_COUNT] = CLR;				
					if(++m_Repeater[i][REPEATER_COM_ERR] >= HIC_COM_OFFLINE)
					{
						m_Repeater[i][REPEATER_COM_ERR] = CLR;
						m_Repeater[i][REPEATER_STATUS] = CLR;
						SETBIT(m_Repeater[i][REPEATER_STATUS],15);
//debugprintf("OFFLINE R[%d]",i);
					}
				}
			}
			if(m_Repeater[i][REPEATER_DETECTOR_COUNT] < 0xffff)//if(TESTBIT(m_Repeater[i][REPEATER_DETECTOR_COUNT],7)) // && m_Repeater[i][REPEATER_DETECTOR_COUNT] != 0xff
			{
				if(TESTBIT(m_Repeater[i][REPEATER_DETECTOR_COUNT],7))
				{
					detectorCount = (m_Repeater[i][REPEATER_DETECTOR_COUNT] & 0x007f);
					for(j = 1 ; j < detectorCount + 1; j++)			// 감지기는 0번지 쓰지 않음
					{
						detectorIndex = HIC_FindDetectorIndex(i, j);
						if(TESTBIT(m_Detector[detectorIndex][DETECTOR_COM_DELAY_COUNT],7))
						{
							m_Detector[detectorIndex][DETECTOR_COM_DELAY_COUNT]++;
//debugprintf("OFFLINE?? R[%d] D[%d] Count[%d]",i,j, m_Detector[detectorIndex][DETECTOR_COM_DELAY_COUNT]);
//delayms(100);
							if(m_Detector[detectorIndex][DETECTOR_COM_DELAY_COUNT] > HIC_COM_TIMEOUT+127)	// 1000 0000 flag bit +
							{
//debugprintf("OFFLINE?? R[%d] D[%d]",i,j);
//delayms(100);
								m_Detector[detectorIndex][DETECTOR_COM_DELAY_COUNT] = CLR;
								if(++m_Detector[detectorIndex][DETECTOR_COM_ERR] >= HIC_COM_OFFLINE)
								{
									m_Detector[detectorIndex][DETECTOR_COM_ERR] = CLR;
									m_Detector[detectorIndex][DETECTOR_STATUS] = CLR;
									SETBIT(m_Detector[detectorIndex][DETECTOR_COMMAND],6);
//debugprintf("OFFLINE Detector R[%d] D[%d]",i,j);
//delayms(100);
								}							
							}
						}											
					}
				}				
			}
		}
	}	
	
	settimer(HIC_COM_DELAY_TIMER,HIC_COM_DELAY_TIMER_ms);
}

void _MUX_RUN_TIMER_3_ISR_()
{
	stoptimer(MUX_RUN_TIMER);
	
	TS_RotarySwitchValue addressValue;
	if(m_ScadaWriteTimerFlag)
	{
		m_ScadaWriteTimerCount++;
		if(m_ScadaWriteTimerCount > 4)
		{
			m_ScadaWriteTimerFlag	= false;
			m_ScadaWriteTimerCount 	= CLR;			
			main_RepeaterDataSave();			
		}
	}

#if 1	
	addressValue = RotaySwitchDetect();
	if(m_Address.RotarySwitchValue != addressValue.RotarySwitchValue)
	{
		main_StatusLEDControl(5,100);
		*R_PMCTRLEN = PMCTRL_RSTCTRL_EN;
		*R_RSTCTRL = RSTCTRL_RESET;
//		m_Address = addressValue;
		
	}
#endif

	if(m_RunLEDState)
	{
		m_RunLEDState = ~m_RunLEDState;
		*R_P5oLOW = (1<<7);
	}
	else
	{
		m_RunLEDState = ~m_RunLEDState;
		*R_P5oHIGH = (1<<7);
	} 
	
	RepeaterRestoreCtrl();
		

	settimer(MUX_RUN_TIMER,MUX_RUN_TIMER_ms);		//1m
		
	
}

void main_RepeaterDataLoad(void)
{
	u32 i, j;
	u32 detectorStartAdd;
	u32 sectorAddress;		// sector 6
//	u8 readBufferRepDetInfo[REPEATER_MAX * 4];
	u8 readBufferSMUXConfig[MUX_CONFIG * 2];
	//for(i = 0 ; i < REPEATER_MAX * 4 ; i++){readBuffer[i] = 0;}

	sectorAddress = (64*1024)*6;											// 262,144
	Flash_Read(FLASH_BUFFER, REPEATER_MAX * 4, sectorAddress);
	
	sectorAddress = (((64*1024)*6) + (REPEATER_MAX * 4));
	Flash_Read(readBufferSMUXConfig, MUX_CONFIG * 2, sectorAddress);

	debugstring("Data Load...\r\n");

	// 0-999 : Repeater Loop No
	// 1000-1999 : Detector Count
	// 2000-3999 : Detector Start Address... 2byte
	
	for(i = 0 ; i < REPEATER_MAX ; i++)
	{
		if(FLASH_BUFFER[i] > SMUX_LOOP_MAX)
		{
			m_Repeater[i][REPEATER_LOOPNUM] = CLR;
		}
		else
		{
			m_Repeater[i][REPEATER_LOOPNUM] = (u16)(FLASH_BUFFER[i]);
		}
		
		if((FLASH_BUFFER[i + REPEATER_MAX] & 0x7f) > PLC_REP_PER_DET_MAX)	
		{
			m_Repeater[i][REPEATER_DETECTOR_COUNT] = CLR;
		}
		else
		{
			m_Repeater[i][REPEATER_DETECTOR_COUNT] = (u16)(FLASH_BUFFER[i + REPEATER_MAX]);
		}
		
		detectorStartAdd  = 	(FLASH_BUFFER[((i*2) + (REPEATER_MAX * 2))]) << 8;
		detectorStartAdd |= 	FLASH_BUFFER[((i*2) + ((REPEATER_MAX * 2) + 1))];
		if(detectorStartAdd >= DETECTOR_VALUE_ADD_MIN_D20001 && detectorStartAdd <= DETECTOR_VALUE_ADD_MAX_D25000)
		{
			m_Repeater[i][DETECTOR_START_ADD] = detectorStartAdd;
		}
		else
		{
			m_Repeater[i][DETECTOR_START_ADD] = CLR;
		}		
	}

	for(i = 0 ; i < MUX_CONFIG ; i++)
	{
		m_MuxConfig[i]  = readBufferSMUXConfig[(i*2)] << 8;
		m_MuxConfig[i] |= readBufferSMUXConfig[(i*2)+1];	
	}
	
	
	if(m_MuxConfig[MUX_CONFIG_OFFLINE_COUNT] <= 0 || m_MuxConfig[MUX_CONFIG_OFFLINE_COUNT] >= 0xffff)
	{
		m_MuxConfig[MUX_CONFIG_OFFLINE_COUNT] = DEFAULT_OFFLINE_COUNT;
	}	
	if(m_MuxConfig[MUX_CONFIG_RESPONSE_DELAY] <= 0 || m_MuxConfig[MUX_CONFIG_RESPONSE_DELAY] >= 0xffff)
	{
		m_MuxConfig[MUX_CONFIG_RESPONSE_DELAY] = DEFAULT_RESPONSE_DELAY;
	}
	if(m_MuxConfig[MUX_CONFIG_POLLING_DELAY] <= 0 || m_MuxConfig[MUX_CONFIG_POLLING_DELAY] >= 0xffff)
	{
		m_MuxConfig[MUX_CONFIG_POLLING_DELAY] = DEFAULT_POLLING_DELAY_MS;
	}
	if(m_MuxConfig[MUX_CONFIG_DETECTOR_255_OFFLINE_COUNT] <= 0 || m_MuxConfig[MUX_CONFIG_DETECTOR_255_OFFLINE_COUNT] >= 0xffff)
	{
		m_MuxConfig[MUX_CONFIG_DETECTOR_255_OFFLINE_COUNT] = DEFAULT_DETECTOR_255_OFFLINE_COUNT;
	}
	if(m_MuxConfig[MUX_CONFIG_R_FIRE_CNT] <= 0 || m_MuxConfig[MUX_CONFIG_R_FIRE_CNT] >= 0xffff)
	{
		m_MuxConfig[MUX_CONFIG_R_FIRE_CNT] = DEFAULT_R_FIRE_CNT;
	}
	if(m_MuxConfig[MUX_CONFIG_P_FIRE_CNT] <= 0 || m_MuxConfig[MUX_CONFIG_P_FIRE_CNT] >= 0xffff)
	{
		m_MuxConfig[MUX_CONFIG_P_FIRE_CNT] = DEFAULT_P_FIRE_CNT;
	}	
	if(m_MuxConfig[MUX_CONFIG_R_STATUS_CNT] <= 0 || m_MuxConfig[MUX_CONFIG_R_STATUS_CNT] >= 0xffff)
	{
		m_MuxConfig[MUX_CONFIG_R_STATUS_CNT] = DEFAULT_R_STATUS_CNT;
	}
	m_MuxConfig[MUX_CONFIG_ALL_TEST_REP_DET] = CLR;


	
	sectorAddress = ((64*1024)*6) + 8000;
	debugprintf("Read Sector Address = %d\r\n", sectorAddress);
	Flash_Read(FLASH_BUFFER, 9000, sectorAddress);
	j = 0;
	for(i=0;i<PLC_DETECTOR_MAX;i++)
	{
		if(FLASH_BUFFER[j] != 0xFF)
		{
			m_Detector[i][DETECTOR_COMMAND] = FLASH_BUFFER[j++] & 0x08;
		}
		else
		{
			j++;
		}


		if(FLASH_BUFFER[j] != 0xFF)
		{
			m_Detector[i][DETECTOR_P_FIRE] = FLASH_BUFFER[j++];
		}
		else
		{
			j++;
		}

		if(FLASH_BUFFER[j] != 0xFF)
		{
			m_Detector[i][DETECTOR_R_FIRE] = FLASH_BUFFER[j++];
		}
		else
		{
			j++;
		}
	}

	debugstring("End Load PLC_DETECTOR\r\n");
	
}

void main_RepeaterDataSave()
{	
	//stoptimer(SCADA_COM_TIMER);
	//stoptimer(REP_DET_COM_TIMER);
	//stoptimer(HIC_COM_DELAY_TIMER);	
	//stoptimer(MUX_RUN_TIMER);
	u32 i, j;
	u32 bufferCount;
	u32 sectorAddress;		// sector 6
//	u8 writeBufferRepDetInfo[(REPEATER_MAX * 4)];
	u8 writeBufferSMUXConfig[MUX_CONFIG * 2];
	//for(i = 0 ; i < (REPEATER_MAX * 4) ; i++){writeBuffer[i] = CLR;}
	//for(i = 0 ; i < MUX_CONFIG ; i++){writeBufferSMUXConfig[i] = CLR;}


	debugstring("Data Save\r\n");
	
	for(i = 0 ; i < REPEATER_MAX ; i++)
	{
		FLASH_BUFFER[i] 										= (u8)(m_Repeater[i][REPEATER_LOOPNUM]);
		FLASH_BUFFER[i + REPEATER_MAX] 						= (u8)(m_Repeater[i][REPEATER_DETECTOR_COUNT]);
		if(m_Repeater[i][DETECTOR_START_ADD] > FIND_REAL_DETECTOR_VALUE_ADD)
		{
			FLASH_BUFFER[((i*2) + (REPEATER_MAX * 2))] 			= (u8)(m_Repeater[i][DETECTOR_START_ADD] >> 8);		
			FLASH_BUFFER[((i*2) + ((REPEATER_MAX * 2) + 1))] 		= (u8)(m_Repeater[i][DETECTOR_START_ADD] & 0x00ff);
		}
		else
		{
			FLASH_BUFFER[((i*2) + (REPEATER_MAX * 2))] 			= CLR;
			FLASH_BUFFER[((i*2) + ((REPEATER_MAX * 2) + 1))] 		= CLR;			
		}

	}
	for(i = 0 ; i < MUX_CONFIG ; i++)
	{
		writeBufferSMUXConfig[(i*2)] 	 = (u8)(m_MuxConfig[i] >> 8);
		writeBufferSMUXConfig[(i*2) + 1] = (u8)(m_MuxConfig[i] & 0x00ff);
	}
	main_StatusLEDControl(5,100);
	Flash_Erase(6);
	bufferCount = REPEATER_MAX * 4;
	sectorAddress = (64*1024)*6;		
	Flash_Write (FLASH_BUFFER, bufferCount,  sectorAddress);
	
	bufferCount = MUX_CONFIG * 2;
	sectorAddress = (((64*1024)*6) + (REPEATER_MAX * 4));
	Flash_Write (writeBufferSMUXConfig, bufferCount,  sectorAddress);


	j = 0;
	for(i=0;i<PLC_DETECTOR_MAX;i++)
	{
		FLASH_BUFFER[j++] = m_Detector[i][DETECTOR_COMMAND] & 0x08;
		FLASH_BUFFER[j++] = m_Detector[i][DETECTOR_P_FIRE];
		FLASH_BUFFER[j++] = m_Detector[i][DETECTOR_R_FIRE];
	}


	sectorAddress = ((64*1024)*6) + 8000;
	Flash_Write(FLASH_BUFFER, j, sectorAddress);	
	//settimer_us(SCADA_COM_TIMER,SCADA_COM_TIMER_us);
	//settimer_us(REP_DET_COM_TIMER,REP_DET_COM_TIMER_us);
	//settimer(HIC_COM_DELAY_TIMER,HIC_COM_DELAY_TIMER_ms);
	//settimer(MUX_RUN_TIMER,MUX_RUN_TIMER_ms);	
}

void main_VariableInit()
{
	int i, j;
	for(i = 0 ; i < REPEATER_MAX ; i++)
	{
		for(j = 0 ; j < REPEATER_INFO ; j++)
		{
			m_Repeater[i][j] = CLR;
		}

	}
	for(i = 0 ; i < PLC_DETECTOR_MAX ; i++)
	{
		for(j = 0 ; j < PLC_DETECTOR_INFO ; j++)
		{
			m_Detector[i][j] = CLR;
		}
	}
	for(i = 0 ; i < MUX_CONFIG ; i++)
	{
		m_MuxConfig[i] = CLR;
	}

	Opt_Tx_Buffer[0] = REPEATER_COMMAND_HEADER;
	Opt_Tx_Buffer[1] = REPEATER_COMMAND_HEADER;
	Opt_Tx_Buffer[2] = REPEATER_COMMAND_HEADER;
	Opt_Tx_Buffer[3] = REPEATER_COMMAND_HEADER;
	Opt_Tx_Buffer[4] = REPEATER_COMMAND_HEADER;

	Opt_Tx_Buffer[14] = REPEATER_COMMAND_ENDER;
	
	

}

void main_WatchDog(int _ms, int _status)
{
	U32 clock, wdtcnt;

	clock = GetAPBclock();
	
	wdtcnt = (_ms /1000) * clock;


	*R_WDCNT = wdtcnt;			// Watch Dog Reset Counter Value
	if(_status) *R_WDTCTRL = 1;				// Watch Dog Enable
}

void main_HICDelayCheck()
{
	u32 i, j;
	u32 detectorCount;
	u32 detectorIndex;

	// Repeater
	// Offline Decision
	// RESPONSE_DELAY -> OFFLINE_COUNT -> SetBit 15;;
	
	for(i = 1 ; i < REPEATER_MAX ; i++)
	{
		if(m_Address.RotarySwitchValue == m_Repeater[i][REPEATER_LOOPNUM] && m_Address.RotarySwitchValue != 0 )
		{
			if(TESTBIT(m_Repeater[i][REPEATER_COM_DELAY_COUNT],7))
			{
//debugprintf("OFF?? R[%d]",i);
				if(++m_Repeater[i][REPEATER_COM_DELAY_COUNT] > m_MuxConfig[MUX_CONFIG_RESPONSE_DELAY]+127)	// 1000 0000 flag bit +
				{
					m_Repeater[i][REPEATER_COM_DELAY_COUNT] = CLR;				
					if(++m_Repeater[i][REPEATER_COM_ERR] >= m_MuxConfig[MUX_CONFIG_OFFLINE_COUNT])
					{

						m_Repeater[i][REPEATER_COM_ERR] = CLR;
						SETBIT(m_Repeater[i][REPEATER_STATUS],15);
					}
				}
			}

			// Detector....

			
			if(m_Repeater[i][REPEATER_DETECTOR_COUNT] < 0x00ff)// 7번째 비트는 plc중계기 판단,, 하위 비트 7개는 감지기 수량 
			{
				if(TESTBIT(m_Repeater[i][REPEATER_DETECTOR_COUNT],7))
				{
					detectorCount = (m_Repeater[i][REPEATER_DETECTOR_COUNT] & 0x007f);
					for(j = 1 ; j < detectorCount + 1; j++)			// 감지기는 0번지 쓰지 않음
					{
						detectorIndex = HIC_FindDetectorIndex(i, j);
						if(TESTBIT(m_Detector[detectorIndex][DETECTOR_COM_DELAY_COUNT],7))
						{
							if(++m_Detector[detectorIndex][DETECTOR_COM_DELAY_COUNT] > m_MuxConfig[MUX_CONFIG_RESPONSE_DELAY]+127)	// 1000 0000 flag bit +
							{
								m_Detector[detectorIndex][DETECTOR_COM_DELAY_COUNT] = CLR;
								if(++m_Detector[detectorIndex][DETECTOR_COM_ERR] >= m_MuxConfig[MUX_CONFIG_OFFLINE_COUNT])
								{
									m_Detector[detectorIndex][DETECTOR_COM_ERR] = CLR;
									SETBIT(m_Detector[detectorIndex][DETECTOR_COMMAND],6);
								}							
							}
						}											
					}
				}				
			}
		}
	}	
}

void main_SMUXConfig()
{
	u16 smuxConfig = CLR;
	smuxConfig = m_SMUXConfigCompare ^ m_MuxConfig[MUX_CONFIG_ALL_TEST_REP_DET];
	if(smuxConfig != 0)
	{
		int i, j;
		u32 detectorCount;
		u32 detectorIndex;
		m_SMUXConfigCompare = m_MuxConfig[MUX_CONFIG_ALL_TEST_REP_DET];	
		if(TESTBIT(smuxConfig, MUX_CONFIG_RESET_BIT))
		{
			if(TESTBIT(m_MuxConfig[MUX_CONFIG_ALL_TEST_REP_DET], MUX_CONFIG_RESET_BIT))
			{
				main_StatusLEDControl(5,100);
				*R_PMCTRLEN = PMCTRL_RSTCTRL_EN;
				*R_RSTCTRL = RSTCTRL_RESET;
			}
		}
		
		if(TESTBIT(smuxConfig, MUX_CONFIG_ALL_CIRCUIT_TEST_BIT))
		{
			for(i = 0 ; i < REPEATER_MAX ; i++)
			{
				if(m_Repeater[i][REPEATER_LOOPNUM] == m_Address.RotarySwitchValue && m_Address.RotarySwitchValue != 0)
				{
					if(TESTBIT(m_MuxConfig[MUX_CONFIG_ALL_TEST_REP_DET], MUX_CONFIG_ALL_CIRCUIT_TEST_BIT))
					{m_Repeater[i][REPEATER_COMMAND] |= 0x00f0;}
					else
					{m_Repeater[i][REPEATER_COMMAND] &= 0xff0f;}
				}
				
			}
		}
		
		if(TESTBIT(smuxConfig, MUX_CONFIG_ALL_OUT_BIT))
		{
/*
			for(i = 0 ; i < REPEATER_MAX ; i++)
			{
				if(m_Repeater[i][REPEATER_LOOPNUM] == m_Address.RotarySwitchValue && m_Address.RotarySwitchValue != 0)
				{
					if(TESTBIT(m_MuxConfig[MUX_CONFIG_ALL_TEST_REP_DET], MUX_CONFIG_ALL_OUT_BIT))
					{m_Repeater[i][REPEATER_COMMAND] |= 0x0f00;}
					else
					{m_Repeater[i][REPEATER_COMMAND] &= 0xf0ff;}
				
				}
				
			}
*/	
		}
		
		if(TESTBIT(smuxConfig, MUX_CONFIG_ALL_DETECTOR_LED_BIT))
		{
			for(i = 0 ; i < REPEATER_MAX ; i++)
			{
				if(m_Repeater[i][REPEATER_LOOPNUM] == m_Address.RotarySwitchValue && m_Address.RotarySwitchValue != 0)
				{
					if(m_Repeater[i][REPEATER_DETECTOR_COUNT] < 0x00ff) // 7번째 비트는 plc중계기 판단,, 하위 비트 7개는 감지기 수량 
					{				
						detectorCount = (m_Repeater[i][REPEATER_DETECTOR_COUNT] & 0x007f);
						for(j = 1 ; j < detectorCount + 1; j++)			// 감지기는 0번지 쓰지 않음
						{
							detectorIndex = HIC_FindDetectorIndex(i, j);
							if(TESTBIT(m_MuxConfig[MUX_CONFIG_ALL_TEST_REP_DET], MUX_CONFIG_ALL_DETECTOR_LED_BIT))
							{
								SETBIT(m_Detector[detectorIndex][DETECTOR_COMMAND],DETECTOR_CMD_LED);
							}
							else
							{
								CLEARBIT(m_Detector[detectorIndex][DETECTOR_COMMAND],DETECTOR_CMD_LED);
							}
							
						}
					}
				}				
			}
		}	
			
		if(TESTBIT(smuxConfig, MUX_CONFIG_FLASH_DEL_BIT))
		{
			if(TESTBIT(m_MuxConfig[MUX_CONFIG_ALL_TEST_REP_DET], MUX_CONFIG_FLASH_DEL_BIT))
			{
				main_StatusLEDControl(5,100);
				debugstring("Flash Erased\r\n");
				Flash_Erase(6);				// 
				main_RepeaterDataLoad();	// 중계기 데이터 로드
			}
		}

		if(TESTBIT(smuxConfig, MUX_CONFIG_REPEATER_RESTORE_BIT))
		{
			if(TESTBIT(m_MuxConfig[MUX_CONFIG_ALL_TEST_REP_DET], MUX_CONFIG_REPEATER_RESTORE_BIT))
			{
				CLEARBIT(m_MuxConfig[MUX_CONFIG_ALL_TEST_REP_DET], MUX_CONFIG_REPEATER_RESTORE_BIT);
				CLEARBIT(m_SMUXConfigCompare, MUX_CONFIG_REPEATER_RESTORE_BIT);
				main_StatusLEDControl(5,100);
				RepeaterRestoreInit();
			}
		}
		
	}		
}

void main_StatusLEDControl(int _count, int _delay)
{
	while(_count--)
	{
		delayms(_delay);
		*R_P5oLOW = (1<<7);
		delayms(_delay);
		*R_P5oHIGH = (1<<7);
	}
}


void smux_ledControl(void)
{
	uint8_t i;

#if 0	
	if(++m_RunLEDCount >= 10) {
		m_RunLEDCount= 0;
		if(m_RunLEDState_p) {
			m_RunLEDState_p = ~m_RunLEDState_p;
			*R_P5oLOW = (1<<7);
		}else {
			m_RunLEDState_p = ~m_RunLEDState_p;
			*R_P5oHIGH = (1<<7);
		}				
	}
#endif


#if 0
	for(i=0; i<4; i++)
	{
		if(ledSleep[i][LED_RX]) 
		{
			--ledSleep[i][LED_RX];
			*R_P0oHIGH = (1<<i); 			
		}
		else if(ledFlag[i][LED_RX]) 
		{
			*R_P0oLOW = (1<<i);
			--ledFlag[i][LED_RX];

			if(!ledFlag[i][LED_RX]) 
			{
				*R_P0oHIGH = (1<<i); 
			}
			ledSleep[i][LED_RX]=1;			
		}
	}

	for(i=0; i<4; i++){
		if(ledSleep[i][LED_TX]) {
			--ledSleep[i][LED_TX];
			*R_P2oHIGH = (1<<i);			
		}else if(ledFlag[i][LED_TX]) {
			*R_P2oLOW = (1<<i);
			--ledFlag[i][LED_TX];
			if(!ledFlag[i][LED_TX]) {
				*R_P2oHIGH = (1<<i);
			}
			ledSleep[i][LED_TX]=1;
		}
	}
#endif	

	if(ledSleep[2][LED_RX])
	{
		--ledSleep[2][LED_RX];
		*R_P0oHIGH = (1<<2);
	}
	else if(ledFlag[2][LED_RX])
	{
		*R_P0oLOW = (1<<2);
		--ledFlag[2][LED_RX];
		if(!ledFlag[2][LED_RX])
		{
			*R_P0oHIGH = (1<<2);	
		}
		ledSleep[2][LED_RX] = 1;
	}

	
	if(ledSleep[4][LED_RX])
	{
		--ledSleep[4][LED_RX];
		*R_P4oHIGH = (1<<4);
	}
	else if(ledFlag[4][LED_RX])
	{
		*R_P4oLOW = (1<<4);
		--ledFlag[4][LED_RX];
		if(!ledFlag[4][LED_RX])
		{
			*R_P4oHIGH = (1<<4);	
		}
		ledSleep[4][LED_RX] = 1;
	}


#if 1
	if(ledSleep[2][LED_TX])
	{
		--ledSleep[2][LED_TX];
		*R_P2oHIGH = (1<<2);
	}
	else if(ledFlag[2][LED_TX])
	{
		*R_P2oLOW = (1<<2);
		--ledFlag[2][LED_TX];
		if(!ledFlag[2][LED_TX])
		{
			*R_P2oHIGH = (1<<2);	
		}
		ledSleep[2][LED_TX] = 1;
	}
#endif


	if(ledSleep[4][LED_TX])
	{
		--ledSleep[4][LED_TX];
		*R_P4oHIGH = (1<<3);
	}
	else if(ledFlag[4][LED_TX])
	{
		*R_P4oLOW = (1<<3);
		--ledFlag[4][LED_TX];
		if(!ledFlag[4][LED_TX])
		{
			*R_P4oHIGH = (1<<3);	
		}
		ledSleep[4][LED_TX] = 1;
	}
/*
	if(Get_Rotary_Switch_Process())
	{
		if(m_Address.RotarySwitchValue != rotarySwitch.RotarySwitchValue)
		{
			debugprintf("New Address = %d\r\n", rotarySwitch.RotarySwitchValue);
			*R_PMCTRLEN = PMCTRL_RSTCTRL_EN;
			*R_RSTCTRL = RSTCTRL_RESET;
		}
	}
*/	
}


int Cli_Repeater_View(int argc, char *argv[])
{
	u16 Rep_View;
	
	if(argc != 2)
	{
		debugstring("Repeater No.. Request\r\n");
		return 0;
	}

	Rep_View = atoi(argv[1]);

	debugprintf("Rep %d ...\r\n", Rep_View);

	debugprintf("0 : Command  - %04X\r\n", m_Repeater[Rep_View][REPEATER_COMMAND]);
	debugprintf("1 : Status   - %04X\r\n", m_Repeater[Rep_View][REPEATER_STATUS]);
	debugprintf("2 : LoopNo   - %04X\r\n", m_Repeater[Rep_View][REPEATER_LOOPNUM]);
	debugprintf("3 : Det.Cnt  - %04X\r\n", m_Repeater[Rep_View][REPEATER_DETECTOR_COUNT]);
	debugprintf("4 : Comm.Er  - %04X\r\n", m_Repeater[Rep_View][REPEATER_COM_ERR]);
	debugprintf("5 : Comm.De  - %04X\r\n", m_Repeater[Rep_View][REPEATER_COM_DELAY_COUNT]);
	debugprintf("6 : Det.Add  - %04X\r\n", m_Repeater[Rep_View][DETECTOR_START_ADD]);
	debugprintf("7 : Stat.Cnt - %04X\r\n", m_Repeater[Rep_View][REPEATER_STATUS_CNT]);

	return 1;
	
}


int Cli_Mux_Config_View(int argc, char *argv[])
{

	debugstring("MUX Config...\r\n");

	debugprintf("0 : MUX_CONFIG_POLLING_DELAY = %d\r\n", m_MuxConfig[MUX_CONFIG_POLLING_DELAY]);
	debugprintf("1 : MUX_CONFIG_RESPONSE_DELAY = %d\r\n", m_MuxConfig[MUX_CONFIG_RESPONSE_DELAY]);
	debugprintf("2 : MUX_CONFIG_OFFLINE_COUNT = %d\r\n", m_MuxConfig[MUX_CONFIG_OFFLINE_COUNT]);
	debugprintf("3 : MUX_CONFIG_DETECTOR_255_OFFLINE_COUNT = %d\r\n", m_MuxConfig[MUX_CONFIG_DETECTOR_255_OFFLINE_COUNT]);
	debugprintf("4 : MUX_CONFIG_ALL_TEST_REP_DET(HEXA) = %X\r\n", m_MuxConfig[MUX_CONFIG_ALL_TEST_REP_DET]);

	debugprintf("5 : MUX_CONFIG_R_FIRE_CNT = %d\r\n", m_MuxConfig[MUX_CONFIG_R_FIRE_CNT]);
	debugprintf("6 : MUX_CONFIG_P_FIRE_CNT = %d\r\n", m_MuxConfig[MUX_CONFIG_P_FIRE_CNT]);
	debugprintf("7 : MUX_CONFIG_R_STATUS_CNT = %d\r\n", m_MuxConfig[MUX_CONFIG_R_STATUS_CNT]);
	

	return 1;	
}






