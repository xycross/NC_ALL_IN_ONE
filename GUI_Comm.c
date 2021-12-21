#include <string.h>

#include "cantus.h"

#include "Definition.h"
#include "GUI_Comm.h"
#include "Converter.h"

#include "CLI_Repeater_Comm.h"

const unsigned short CRC_CCITT_TABLE[256] =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

const char *BOARD_MESSAGE = "  Comm Mon V2.0 2021/12/25";



extern unsigned char Code_Confirm_Table[256];

unsigned short POLLING_TIME = 10;

unsigned char GUI_RX_BUFFER[50];

unsigned char GUI_RX_STATE, GUI_RX_DATA_LENGTH, GUI_RX_COUNT, GUI_RX_SEQ;

unsigned char GUI_TX_BUFFER[50];
unsigned char GUI_TX_STATE, GUI_TX_RETRY_COUNT, GUI_TX_P_LENGTH, GUI_TX_OPCODE, GUI_TX_ACK_REQUIRE;

unsigned char P_Received;
unsigned char GUI_COMM_ING, GUI_COMM_ACK_TIMEOUT, GUI_COMM_RETRY, GUI_COMM_ACK_RECEIVED;

struct _TX_PACKET GUI_TX_POOL[DEF_GUI_TX_POOL_SIZE];

unsigned short GUI_TX_SAVE, GUI_TX_PROCESS;


unsigned short GUI_REP_START_NO, GUI_REP_END_NO, GUI_REP_POLL_NO;
unsigned char GUI_POLLING_ENABLE, GUI_POLL_ACTION, GUI_POLL_CMD;
unsigned char GUI_INDI_ACTION, GUI_INDI_TYPE, GUI_INDI_DET_NO, GUI_INDI_DET_COMMAND;

unsigned short GUI_INDI_REP_NO, GUI_INDI_COMMAND;

unsigned char GUI_POLL_ENUM;


unsigned char Message_Buffer[50], M_Buffer_Count;

// Rep Comm Variables
extern unsigned char Rep_Tx_Buffer[15];

extern unsigned char U7_RX_BUFFER[200], U7_RX_COUNT;
unsigned char T_U7_RX_BUFFER[200], T_U7_RX_COUNT;

extern unsigned char opt_rx_data[100], opt_rx_count, opt_rx_state, RX_OK;

extern unsigned char MASTER_TIMER_TIMEOUT;

extern unsigned char Parsing_Pool[10][20];
extern unsigned char Parsing_Save, Parsing_Process;


extern unsigned int REP_RX_COUNT[1000];
extern unsigned int REP_COMM_COUNT;

unsigned short Rep_Status[10], Rep_Command[10];

unsigned char rep_test;
void GUI_Procedure(void)
{
	if(P_Received)
	{
//		debugstring("Packet RXD\r\n");
		P_Received = 0;
		GUI_RX_Process();
	}

	GUI_TX_Process();

	if(GUI_POLL_ACTION)
	{
		GUI_POLLING_PROCEDURE();
	}

	if(GUI_INDI_ACTION)
	{
		GUI_INDI_ACTION = 0;
		GUI_INDI_PROCEDURE();
	}
}

void GUI_POLLING_PROCEDURE(void)
{
		unsigned short k;
	
	switch(GUI_POLL_ENUM)
	{
		case GUI_REP_E_TX:
//			debugprintf("E_TX\r\n");
			memset(U7_RX_BUFFER, 0, sizeof(U7_RX_BUFFER));
			U7_RX_COUNT = 0;
			opt_rx_state = 0;
			RX_OK = 0;
			GUI_Master_Packet_Send(GUI_REP_POLL_NO);
			MASTER_TIMER_TIMEOUT = 0;
//			debugprintf("Poll Time = %d\r\n", POLLING_TIME);
			settimer(0,POLLING_TIME);
			
			GUI_POLL_ENUM = GUI_REP_E_WAIT;
			break;

		case GUI_REP_E_WAIT:
			if(Parsing_Save != Parsing_Process)
			{
				if(GUI_Parsing_Repeater(0))
				{
					RX_OK = 1;
	//				stoptimer(0);
	//				POLL_ENUM = E_NEXT;
				}
				
				if(++Parsing_Process >= 10)
				{
					Parsing_Process = 0;
				}
				
				if(GUI_POLL_CMD == E_GUI_POLL_FAST)
				{
					stoptimer(0);
					GUI_POLL_ENUM = GUI_REP_E_NEXT;
				}
			}
			else if(MASTER_TIMER_TIMEOUT)
			{
				if(!RX_OK)
				{
					//debugprintf("TimeOut %d RX : ", GUI_REP_POLL_NO);
					if(U7_RX_COUNT)
					{
						for(k=0;k<U7_RX_COUNT;k++)
						{
							debugprintf("%02X ", U7_RX_BUFFER[k]);
							
						}

						if(U7_RX_COUNT < 48)
						{
							Message_Buffer[0] = GUI_REP_POLL_NO >> 8;
							Message_Buffer[1] = GUI_REP_POLL_NO & 0xFF;
							memcpy(&Message_Buffer[2], U7_RX_BUFFER, U7_RX_COUNT);
							GUI_TX_MAKE(OP_S_POLL_MESSAGE, Message_Buffer, U7_RX_COUNT + 2, 0);
						}
						debugstring("\r\n");
					}
					else
					{
					//	debugprintf("\r\n");
					}
					
				}
				GUI_POLL_ENUM = GUI_REP_E_NEXT;
			}
			break;

		case GUI_REP_E_NEXT:
//			delayms(1);
//			debugprintf("E_NEXT\r\n");
			if(++GUI_REP_POLL_NO > GUI_REP_END_NO)
			{
				REP_COMM_COUNT++;
				GUI_REP_POLL_NO = GUI_REP_START_NO;
//				debugprintf("Next Poll No = %d\r\n", POLL_NO);

				
				if(GUI_POLL_CMD < E_GUI_POLL_REGULAR)
				{
					GUI_POLL_ACTION = 0;
					GUI_Polling_Report();
					return;
				}
				
			}
			


			GUI_POLL_ENUM = GUI_REP_E_TX;
			break;
	}

	
}


void GUI_INDI_PROCEDURE(void)
{
	unsigned char k;
	
	switch(GUI_INDI_TYPE)
	{
		case E_INDI_REP_ONLY:
			memset(U7_RX_BUFFER, 0, sizeof(U7_RX_BUFFER));
			U7_RX_COUNT = 0;
			opt_rx_state = 0;
			GUI_Master_Packet_Send_Individual(GUI_INDI_REP_NO);
			MASTER_TIMER_TIMEOUT = 0;
			settimer(0, POLLING_TIME);
			Parsing_Save = 0;
			Parsing_Process = 0;
			break;

		case E_INDI_DET_ONLY:
			memset(U7_RX_BUFFER, 0, sizeof(U7_RX_BUFFER));
			U7_RX_COUNT = 0;
			opt_rx_state = 0;
			GUI_Master_Packet_Send_Det(GUI_INDI_REP_NO, GUI_INDI_DET_NO, GUI_INDI_DET_COMMAND);
			MASTER_TIMER_TIMEOUT = 0;
			settimer(0, POLLING_TIME);
			Parsing_Save = 0;
			Parsing_Process = 0;
			
			break;

		case E_INDI_CMD:
			memset(U7_RX_BUFFER, 0, sizeof(U7_RX_BUFFER));
			U7_RX_COUNT = 0;
			opt_rx_state = 0;
			GUI_Manual_Packet_Send(GUI_INDI_REP_NO, GUI_INDI_COMMAND);
			
			MASTER_TIMER_TIMEOUT = 0;
			settimer(0, POLLING_TIME);
			Parsing_Save = 0;
			Parsing_Process = 0;

			
			break;
	}


	while(1)
	{
		if(Parsing_Save != Parsing_Process)
		{
//			if(Parsing_Repeater(1))
			if(GUI_Parsing_Indi())
			{
				if(++Parsing_Process >= 10)
				{
					Parsing_Process = 0;
				}
				
				break;
			}

			if(++Parsing_Process >= 10)
			{
				Parsing_Process = 0;
			}
		
			
			
		}
		else if(MASTER_TIMER_TIMEOUT)
		{
			*R_TM7CON = 0;
			debugstring("Time Out\r\n");
			Message_Buffer[0] = 0;

			if(U7_RX_COUNT)
			{
				memcpy(&Message_Buffer[1],U7_RX_BUFFER, U7_RX_COUNT);
			}
			GUI_TX_MAKE(OP_S_INDI_RESULT, Message_Buffer, U7_RX_COUNT + 1, 1);

#if 1				
//				debugprintf("%d RX : ", POLL_NO);
				debugstring("RX : ");
				for(k=0;k<U7_RX_COUNT;k++)
				{
					debugprintf("%02X ", U7_RX_BUFFER[k]);
				}
				debugstring("\r\n");
//				debugprintf("Opt Rx Count : %d\r\n", opt2_rx_count);
//				Detail_View();
#endif				

			break;
		}


	}	
}

u8 GUI_Parsing_Indi(void)
{
	
	unsigned char i, j, m, n;
	unsigned short k, s_16;
	unsigned char buffer[5];
	unsigned char *p;

	unsigned int tmr_value;
	float timing;
	
	unsigned char *ptr = Parsing_Pool[Parsing_Process];


	j = 0;
	for(i = POS_RX_ADDRESS1; i < POS_RX_BCC_HIGH; i++)
	{

		j += *(ptr + i);
	}

	m = UnCodiz_From16_To8(ptr + POS_RX_BCC_HIGH);

//	debugprintf("rx bcc = %02X, calc bcc = %02X\r\n", m, j);
	if(m == j)
	{
		tmr_value = *((volatile unsigned int*)0x800210e8);
		*R_TM7CON = 0;
		memcpy(T_U7_RX_BUFFER, U7_RX_BUFFER, U7_RX_COUNT);
		T_U7_RX_COUNT = U7_RX_COUNT;

		s_16 =  UnCodiz_12bit(ptr);
		CLEARBIT(s_16, 11);
		if(s_16 != GUI_INDI_REP_NO)
		{
			
			Message_Buffer[0] = 0;
			
			if(U7_RX_COUNT)
			{
				memcpy(&Message_Buffer[1],T_U7_RX_BUFFER, T_U7_RX_COUNT);
			}
			GUI_TX_MAKE(OP_S_INDI_RESULT, Message_Buffer, T_U7_RX_COUNT + 1, 1);

			return 0;

		}


		buffer[0] = 1;
		GUI_TX_MAKE(OP_S_INDI_RESULT, buffer, 1, 1);
		if(GUI_INDI_TYPE == E_INDI_CMD)
		{
			return 1;
		}
		timing = tmr_value * 1000.0 / 23990400;
		p = &timing;

		buffer[0] = *p;
		buffer[1] = *(p+1);
		buffer[2] = *(p+2);
		buffer[3] = *(p+3);

		GUI_TX_MAKE(OP_S_INDI_TIME, buffer, 4, 1);

		GUI_TX_MAKE(OP_S_INDI_PACKET, T_U7_RX_BUFFER, T_U7_RX_COUNT, 1);

		
		
		return 1;
		
	}
	else
	{
		Message_Buffer[0] = 0;

		if(U7_RX_COUNT)
		{
			memcpy(&Message_Buffer[1],U7_RX_BUFFER, U7_RX_COUNT);
		}
		GUI_TX_MAKE(OP_S_INDI_RESULT, Message_Buffer, U7_RX_COUNT + 1, 1);
//		debugprintf("BCC Error\r\n");
		return 0;
	}
}

u8 GUI_Parsing_Repeater(unsigned char indi)
{
	unsigned char i, j, m, n;
	unsigned short k, s_16, q;
	unsigned char buffer[6];

	unsigned int tmr_value;
	double timing;
	
	unsigned char *ptr = Parsing_Pool[Parsing_Process];

#if 0
	debugstring("Parsing Repeater : ");
	for(i=0;i<15;i++)
	{
		debugprintf("%02X ", *(ptr + i));
	}
	debugprintf("\r\n");
#endif		

	j = 0;
	for(i = POS_RX_ADDRESS1; i < POS_RX_BCC_HIGH; i++)
	{

		j += *(ptr + i);
	}

	m = UnCodiz_From16_To8(ptr + POS_RX_BCC_HIGH);

//	debugprintf("rx bcc = %02X, calc bcc = %02X\r\n", m, j);
	if(m == j)
	{
		if(indi)
		{
			tmr_value = *((volatile unsigned int*)0x800210e8);
			*R_TM7CON = 0;
			debugprintf("TMR CNT = %ld\r\n", tmr_value);
			timing = tmr_value * 1000.0 / 23990400;
			debugprintf("Rx Time = %lf ms\r\n", timing);
		}
		
		k = UnCodiz_12bit(ptr);


		if((k & 0x7FF) != GUI_REP_POLL_NO)
		{
			debugprintf("Address Diff %d -> %d\r\n", GUI_REP_POLL_NO, k);
			return 0;
		}

		UnCodiz_16bit((ptr+POS_RX_DATA1), &s_16);

		if(rep_test && (k < 5))
		{
		
			if(Rep_Status[k] != s_16)
			{
				buffer[0] = k >> 8;
				buffer[1] = k & 0xFF;
				buffer[2] = s_16 >> 8;
				buffer[3] = s_16 & 0xFF;
				GUI_TX_MAKE(OP_S_REP_RESULT, buffer, 4, 1);
				q = (s_16 & 0x000F) << 8;
				Rep_Command[k] = q;
				Rep_Status[k] = s_16;
				
			}
		}
		

		if(indi)
		{
			debugprintf("RX OK  : %d\r\n", k);
			
			if(TESTBIT(k, 11))
			{
				debugprintf("Rep Addr : %d, Det No : %d, Det Value = %d\r\n", (k & 0x7FF), 
							UnCodiz_8bit(*(ptr + POS_RX_DATA1), *(ptr + POS_RX_DATA2)), 
							UnCodiz_8bit(*(ptr + POS_RX_DATA3), *(ptr + POS_RX_DATA4)));
			}
			else
			{
				UnCodiz_16bit((ptr+POS_RX_DATA1), &s_16);
				debugprintf("Rep Addr : %d, Rep Value = %04X\r\n", (k & 0x7FF), s_16);
			}
		}

		REP_RX_COUNT[k]++;

/*
		if(TESTBIT(k, 11))
		{
		}
		else
		{
			m = UnCodiz_4bit(*(ptr + POS_RX_DATA4));
			if(Repeater_Status[k] != m)
			{
				debugprintf("%d : %02X -> %02X\r\n", k, Repeater_Status[k], m);
				Repeater_Status[k] = m;
				REP_RX_COUNT[k]++;
			}
		}
*/		
//		debugprintf("Addr = %d\r\n", k);
		return 1;
		
	}
	else
	{
		debugprintf("BCC Error\r\n");
		return 0;
	}

	
}


void GUI_TX_Process(void)
{
	unsigned char i;
	unsigned short m16;
	
	switch(GUI_TX_STATE)
	{
		case E_TX_MAKE_PACKET:
			if(GUI_TX_SAVE != GUI_TX_PROCESS)
			{
				GUI_TX_BUFFER[P_POS_HEADER] = SLAVE_HEADER;
				GUI_TX_BUFFER[P_POS_SEQ_NO] = GUI_RX_SEQ;

				i = GUI_TX_POOL[GUI_TX_PROCESS].length;
				
				GUI_TX_BUFFER[P_POS_DATA_LENGTH] = 3 + i;
				if(i)
				{
					memcpy(&GUI_TX_BUFFER[P_POS_DATA_START], GUI_TX_POOL[GUI_TX_PROCESS].message, i);
				}
				GUI_TX_OPCODE = GUI_TX_BUFFER[P_POS_OPCODE] = GUI_TX_POOL[GUI_TX_PROCESS].opcode;


				m16 = Calculate_CRC_CCITT(GUI_TX_BUFFER, 4 + i);

				GUI_TX_BUFFER[P_POS_DATA_START + i] = m16 >> 8;
				GUI_TX_BUFFER[P_POS_DATA_START + i + 1] = m16 & 0xFF;
				GUI_TX_P_LENGTH = 6 + i;

//				if(++GUI_TX_PROCESS >= DEF_GUI_TX_POOL_SIZE)
//				{
//					GUI_TX_PROCESS = 0;
//				}

				GUI_TX_STATE = E_TX_SEND;

				if(GUI_TX_POOL[GUI_TX_PROCESS].ack_require)
				{
					GUI_COMM_ACK_RECEIVED = 0;
					GUI_COMM_RETRY = 0;
					GUI_COMM_ACK_TIMEOUT = 0;
				}
				
			}
			break;

		case E_TX_SEND:
			for(i=0;i<GUI_TX_P_LENGTH;i++)
			{

				while(!((*(volatile U32*)(UART_BASE_0 + UART_LSR))  & ULSR_TEMP));
				
				*(volatile u8*)(UART_BASE_0 + UART_THR)	= GUI_TX_BUFFER[i];
			}
//			debugstring("PACKET TX\r\n");

			if(GUI_TX_POOL[GUI_TX_PROCESS].ack_require)
			{
				settimer(6, DEF_GUI_COMM_TIMEOUT_MS);
				GUI_COMM_ACK_RECEIVED = 0;
				GUI_COMM_ACK_TIMEOUT = 0;

				GUI_TX_STATE = E_TX_WAIT_ACK;
			}
			else
			{
				GUI_TX_STATE = E_TX_NEXT;
			}
			break;


		case E_TX_WAIT_ACK:
			if(GUI_COMM_ACK_RECEIVED)
			{
//				debugstring("ACK Rxd\r\n");
//				GUI_COMM_ACK_RECEIVED = 0;
				GUI_TX_STATE = E_TX_NEXT;
			}
			else if(GUI_COMM_ACK_TIMEOUT)
			{
				if(++GUI_TX_RETRY_COUNT >= DEF_GUI_COMM_RETRY)
				{
					debugstring("COMM 3Retry Error\r\n");
					GUI_TX_STATE = E_TX_NEXT;
				}
				else
				{
					GUI_TX_STATE = E_TX_SEND;
				}
			}
			
			break;

		case E_TX_NEXT:
			
			if(++GUI_TX_PROCESS >= DEF_GUI_TX_POOL_SIZE)
			{
				GUI_TX_PROCESS = 0;
			}
			
			GUI_TX_STATE = E_TX_MAKE_PACKET;
			break;

		
	}
}


void GUI_RX_Process(void)
{
	unsigned short calc_crc, rx_crc;
	unsigned char i;
	unsigned char buffer[40];

#if 0
	debugstring("RX P : ");
	for(i=0;i<GUI_RX_DATA_LENGTH + 3; i++)
	{
		debugprintf("%02X ", GUI_RX_BUFFER[i]);
	}
	debugstring("\r\n");
#endif
	calc_crc = Calculate_CRC_CCITT(GUI_RX_BUFFER, GUI_RX_DATA_LENGTH + 1);
	rx_crc = GUI_RX_BUFFER[GUI_RX_DATA_LENGTH + 1];
	rx_crc = rx_crc << 8;
	rx_crc += GUI_RX_BUFFER[GUI_RX_DATA_LENGTH + 2];

	if(calc_crc == rx_crc)
	{
		GUI_RX_SEQ = GUI_RX_BUFFER[P_POS_SEQ_NO];
//		debugstring("CRC OK\r\n");
		switch(GUI_RX_BUFFER[P_POS_OPCODE])
		{
			case OP_M_BOARD_CHECK:
				rx_crc = strlen(BOARD_MESSAGE);
				memcpy(buffer, BOARD_MESSAGE, rx_crc);
				buffer[0] = POLLING_TIME >> 8;
				buffer[1] = POLLING_TIME & 0xFF;
				GUI_TX_MAKE(OP_S_BOARD_CHECK, buffer, rx_crc, 0);
				break;

			case OP_M_POLL_TIME:
				rx_crc = GUI_RX_BUFFER[P_POS_DATA_START];
				rx_crc = rx_crc << 8;
				rx_crc += GUI_RX_BUFFER[P_POS_DATA_START + 1];
				POLLING_TIME = rx_crc;
				buffer[0] = POLLING_TIME >> 8;
				buffer[1] = POLLING_TIME & 0xFF;

				GUI_TX_MAKE(OP_S_POLL_TIME, buffer, 2, 0);
				
				break;


			case OP_M_REP_TEST:
				rep_test = 1;
				GUI_TX_MAKE(OP_S_PERIODIC_POLL, NULL, 0, 0);
				rx_crc = GUI_RX_BUFFER[P_POS_DATA_START];
				rx_crc = rx_crc << 8;
				rx_crc += GUI_RX_BUFFER[P_POS_DATA_START + 1];
				GUI_REP_START_NO = rx_crc;
				GUI_REP_POLL_NO = GUI_REP_START_NO;

				rx_crc = GUI_RX_BUFFER[P_POS_DATA_START + 2];
				rx_crc = rx_crc << 8;
				rx_crc += GUI_RX_BUFFER[P_POS_DATA_START + 3];
				GUI_REP_END_NO = rx_crc;
				
	//				debugprintf("Period Poll From %d to %d\r\n", GUI_REP_START_NO, GUI_REP_END_NO);
				GUI_Poll_Initialize();
				GUI_POLL_CMD = E_GUI_POLL_REGULAR;
				
				break;	
				
			case OP_M_PERIODIC_POLL:
				rep_test = 0;
			
				GUI_TX_MAKE(OP_S_PERIODIC_POLL, NULL, 0, 0);
				rx_crc = GUI_RX_BUFFER[P_POS_DATA_START];
				rx_crc = rx_crc << 8;
				rx_crc += GUI_RX_BUFFER[P_POS_DATA_START + 1];
				GUI_REP_START_NO = rx_crc;
				GUI_REP_POLL_NO = GUI_REP_START_NO;

				rx_crc = GUI_RX_BUFFER[P_POS_DATA_START + 2];
				rx_crc = rx_crc << 8;
				rx_crc += GUI_RX_BUFFER[P_POS_DATA_START + 3];
				GUI_REP_END_NO = rx_crc;
				
//				debugprintf("Period Poll From %d to %d\r\n", GUI_REP_START_NO, GUI_REP_END_NO);
				GUI_Poll_Initialize();
				GUI_POLL_CMD = E_GUI_POLL_REGULAR;
				break;

			case OP_M_FAST_POLL:
				rep_test = 0;
				GUI_TX_MAKE(OP_S_FAST_POLL, NULL, 0, 0);
				rx_crc = GUI_RX_BUFFER[P_POS_DATA_START];
				rx_crc = rx_crc << 8;
				rx_crc += GUI_RX_BUFFER[P_POS_DATA_START + 1];
				GUI_REP_START_NO = rx_crc;
				GUI_REP_POLL_NO = GUI_REP_START_NO;

				rx_crc = GUI_RX_BUFFER[P_POS_DATA_START + 2];
				rx_crc = rx_crc << 8;
				rx_crc += GUI_RX_BUFFER[P_POS_DATA_START + 3];
				GUI_REP_END_NO = rx_crc;
				
//				debugprintf("Period Poll From %d to %d\r\n", GUI_REP_START_NO, GUI_REP_END_NO);
				GUI_Poll_Initialize();
				GUI_POLL_CMD = E_GUI_POLL_FAST;
				break;

			case OP_M_REP_STOP:
			case OP_M_POLL_STOP:
				GUI_POLL_CMD = E_GUI_NULL;
				break;

			case OP_M_INDI_POLL:
				rx_crc = GUI_RX_BUFFER[P_POS_DATA_START];
				rx_crc = rx_crc << 8;
				rx_crc += GUI_RX_BUFFER[P_POS_DATA_START + 1];
				GUI_INDI_ACTION = 1;
				GUI_INDI_TYPE = E_INDI_REP_ONLY;
				GUI_INDI_REP_NO = rx_crc;
				
				break;

			case OP_M_INDI_DET:
				rx_crc = GUI_RX_BUFFER[P_POS_DATA_START];
				rx_crc = rx_crc << 8;
				rx_crc += GUI_RX_BUFFER[P_POS_DATA_START + 1];
				GUI_INDI_ACTION = 1;
				GUI_INDI_TYPE = E_INDI_DET_ONLY;
				GUI_INDI_REP_NO = rx_crc;
				GUI_INDI_DET_NO = GUI_RX_BUFFER[P_POS_DATA_START + 2];
				GUI_INDI_DET_COMMAND = GUI_RX_BUFFER[P_POS_DATA_START + 3];
				break;

			case OP_M_CMD_REP:
			case OP_M_CMD_DET:
				rx_crc = GUI_RX_BUFFER[P_POS_DATA_START];
				rx_crc = rx_crc << 8;
				rx_crc += GUI_RX_BUFFER[P_POS_DATA_START + 1];
				GUI_INDI_REP_NO = rx_crc;

				rx_crc = GUI_RX_BUFFER[P_POS_DATA_START + 2];
				rx_crc = rx_crc << 8;
				rx_crc += GUI_RX_BUFFER[P_POS_DATA_START + 3];
				GUI_INDI_COMMAND = rx_crc;

				
				GUI_INDI_ACTION = 1;
				GUI_INDI_TYPE = E_INDI_CMD;
				break;

			case OP_ACK:
				GUI_COMM_ACK_RECEIVED = 1;
				break;
		}
		
	}

	
	
}


void GUI_Poll_Initialize(void)
{
	unsigned short i;

	for(i=GUI_REP_START_NO;i < (GUI_REP_END_NO+1); i++)
	{
		REP_RX_COUNT[i] = 0;
	}

	GUI_POLL_ENUM = GUI_REP_E_TX;

	REP_COMM_COUNT = 0;

	GUI_POLL_ACTION = 1;
	
}

void GUI_TX_MAKE(unsigned char opcode, unsigned char *buffer, unsigned char length, unsigned char ack_req)
{
//	debugprintf("Make opcode = %02X, length = %d\r\n", opcode, length);
	GUI_TX_POOL[GUI_TX_SAVE].opcode = opcode;
	GUI_TX_POOL[GUI_TX_SAVE].length = length;
	GUI_TX_POOL[GUI_TX_SAVE].ack_require = ack_req;
	if(length)
	{
		memcpy(GUI_TX_POOL[GUI_TX_SAVE].message, buffer, length);
	}

	if(++GUI_TX_SAVE >= DEF_GUI_TX_POOL_SIZE)
	{
		GUI_TX_SAVE = 0;
	}	
	
}



unsigned short Calculate_CRC_CCITT(const unsigned char* buffer, int size)
{
    unsigned short tmp;
    unsigned short crc = 0xffff;
	unsigned short i;

    for (i=0; i < size ; i++)
    {
        tmp = (crc >> 8) ^ buffer[i];
        crc = (crc << 8) ^ CRC_CCITT_TABLE[tmp];
    }

    return crc;
}


void Rep_Parsing_Add(unsigned char *pos, unsigned char length)
{
//	debugprintf("Parsing Add pos = %d, length = %d\r\n", Parsing_Save, length);
	memcpy(Parsing_Pool[Parsing_Save], pos, length);
	if(++Parsing_Save >= 10)
	{
		Parsing_Save = 0;
	}
}



void GUI_Master_Packet_Send(unsigned short addr)
{
	unsigned char i, j;
	
	Codiz_Data12bit(&Rep_Tx_Buffer[POS_TX_ADDRESS], addr);

	if(rep_test)
	{
		Codiz_Data16bit(&Rep_Tx_Buffer[POS_TX_DATA1], Rep_Command[addr]);
	}
	else
	{
		Codiz_Data16bit(&Rep_Tx_Buffer[POS_TX_DATA1], 0);
	}

	j = 0;
	
	for(i=POS_TX_ADDRESS;i<POS_TX_BCC_HIGH;i++)
	{
		j += Rep_Tx_Buffer[i];
	}

	Codiz_From8_To16(&Rep_Tx_Buffer[POS_TX_BCC_HIGH], j);

	for(i=0;i<PACKET_SIZE;i++)
	{
		*(volatile u8*)(UART_BASE_7 + UART_THR) = Rep_Tx_Buffer[i];
	}

	
}

void GUI_Master_Packet_Send_Individual(unsigned short addr)
{
	unsigned char i, j;
	
	Codiz_Data12bit(&Rep_Tx_Buffer[POS_TX_ADDRESS], addr);
	Codiz_Data16bit(&Rep_Tx_Buffer[POS_TX_DATA1], 0);

	j = 0;
	
	for(i=POS_TX_ADDRESS;i<POS_TX_BCC_HIGH;i++)
	{
		j += Rep_Tx_Buffer[i];
	}

	Codiz_From8_To16(&Rep_Tx_Buffer[POS_TX_BCC_HIGH], j);

#if 0
	debugstring("\r\nTX Data : ");
	for(i=0;i<PACKET_SIZE;i++)
	{
		debugprintf("%02X ", Rep_Tx_Buffer[i]);
	}
	debugstring("\r\n");
#endif	

	monitor_time_Set(10);
	monitor_time_run();

	for(i=0;i<PACKET_SIZE;i++)
	{
		*(volatile u8*)(UART_BASE_7 + UART_THR) = Rep_Tx_Buffer[i];
	}

	
}


void GUI_Master_Packet_Send_Det(unsigned short addr, unsigned char det_n, unsigned char det_value)
{
	unsigned char i, j;

	SETBIT(addr, 11);
	Codiz_Data12bit(&Rep_Tx_Buffer[POS_TX_ADDRESS], addr);
	Codiz_Data8bit(&Rep_Tx_Buffer[POS_TX_DATA1], det_n);
	if(det_value)
	{
		det_value = 4;
	}
	Codiz_Data8bit(&Rep_Tx_Buffer[POS_TX_DATA3], det_value);

	j = 0;
	
	for(i=POS_TX_ADDRESS;i<POS_TX_BCC_HIGH;i++)
	{
		j += Rep_Tx_Buffer[i];
	}

	Codiz_From8_To16(&Rep_Tx_Buffer[POS_TX_BCC_HIGH], j);

	monitor_time_Set(10);
	monitor_time_run();

	for(i=0;i<PACKET_SIZE;i++)
	{
		*(volatile u8*)(UART_BASE_7 + UART_THR) = Rep_Tx_Buffer[i];
	}

	
}

void GUI_Manual_Packet_Send(unsigned short addr, unsigned short w_data)
{
	unsigned char i, j;


	Codiz_Data12bit(&Rep_Tx_Buffer[POS_TX_ADDRESS], addr);
	Codiz_Data16bit(&Rep_Tx_Buffer[POS_TX_DATA1], w_data);

	j = 0;
	
	for(i=POS_TX_ADDRESS;i<POS_TX_BCC_HIGH;i++)
	{
		j += Rep_Tx_Buffer[i];
	}

	Codiz_From8_To16(&Rep_Tx_Buffer[POS_TX_BCC_HIGH], j);


	U7_RX_COUNT = 0;

	for(i=0;i<PACKET_SIZE;i++)
	{
		*(volatile u8*)(UART_BASE_7 + UART_THR) = Rep_Tx_Buffer[i];
	}
	
}

void GUI_Polling_Report(void)
{
	unsigned short i, j;
	unsigned char buffer[10];

	buffer[0] = REP_COMM_COUNT >> 24;
	buffer[1] = REP_COMM_COUNT >> 16;
	buffer[2] = REP_COMM_COUNT >> 8;
	buffer[3] = REP_COMM_COUNT & 0xFF;
	
	GUI_TX_MAKE(OP_S_POLL_RESULT_HEADR, buffer, 4, 1);

	for(i=GUI_REP_START_NO; i < (GUI_REP_END_NO + 1); i++)
	{
		buffer[0] = i >> 8;
		buffer[1] = i & 0xFF;
		buffer[2] = REP_RX_COUNT[i] >> 24;
		buffer[3] = REP_RX_COUNT[i] >> 16;
		buffer[4] = REP_RX_COUNT[i] >> 8;
		buffer[5] = REP_RX_COUNT[i] & 0xFF;
		GUI_TX_MAKE(OP_S_POLL_RESULT_BODY, buffer, 6, 1);
	}

	GUI_TX_MAKE(OP_S_POLL_RESULT_ENDER, NULL, 0, 1);
}

#if 0
void View_Comm_Count(void)
{
	unsigned short i, j, per;

//	debugprintf("Communication Report ---- Rep From %d To %d\r\n", start_rep_no, end_rep_no);
	debugprintf("........................ Polling Time : %d\r\n", POLLING_TIME);
	debugprintf("Total Tx : %ld\r\n", REP_COMM_COUNT);

//	for(i=start_rep_no;i<(end_rep_no + 1);i++)
	{
#if 1		
		per = (REP_RX_COUNT[i] * 100 / REP_COMM_COUNT);
		debugprintf("%03d - ", i);
		for(j=0;j<(per *4 /10);j++)
		{
			debugprintf("@");
		}
		debugprintf("  (%d - %d %%)\r\n", REP_RX_COUNT[i], per); 
#endif		
//		debugprintf("%d : %ld - Success : %d\r\n", i, REP_RX_COUNT[i], (REP_RX_COUNT[i] * 100 / REP_COMM_COUNT));//((float)REP_RX_COUNT[i] / REP_COMM_COUNT * 100));
	}
}
#endif

void GUI_Repeater_Initialize(void)
{
	unsigned char i;

	Rep_Tx_Buffer[0] = MASTER_STA;
	Rep_Tx_Buffer[1] = MASTER_STA;
	Rep_Tx_Buffer[2] = MASTER_STA;
	Rep_Tx_Buffer[3] = MASTER_STA;
	Rep_Tx_Buffer[4] = MASTER_STA;

	Rep_Tx_Buffer[14] = MASTER_ETX;

	POLLING_TIME = 10;
}

#if 0
void Main_Timer_ISR(void)
{
	
	stoptimer(0);
//	debugstring("MAin Timer...\r\n");
	MASTER_TIMER_TIMEOUT = 1;
}
#endif

void GUI_Comm_Timer_ISR()
{
	stoptimer(6);
	
}

void GUI_Uart0ISR()
{
//	getuartfifo((U32)R_UART4_BASE,&ubuf4);

		U32 iir;
		U8 ch;
		iir = *(volatile U32*)(UART_BASE_0 + UART_IIR);
		iir &= 0xf;
		
		switch(iir)
		{
//#ifdef CONFIG_UART_RX_INTERRUPT
#if 1
		case 6:
			debugstring("UART Line Status Error(Overrun,Frame,Parity)\r\n");
		case 4:
		case 0xc:
			while((*(volatile U32*)(UART_BASE_0 + UART_LSR)) & ULSR_DRDY)//data ready
			{
				ch = *(volatile U8*)(UART_BASE_0 + UART_RBR);
//				*(volatile U8*)(R_UART4_BASE + UART_THR) = ch;
//				*(volatile U8*)(UART_BASE_0 + UART_THR) = ch;

				switch(GUI_RX_STATE)
				{
					case 0: // Header Check
						if(ch == MASTER_HEADER)
						{
							GUI_RX_STATE = 1;
							GUI_RX_BUFFER[0] = ch;
						}
						break;
		
					case 1: 	// SEQ NO
				//		GUI_RX_SEQ = ch;
						GUI_RX_BUFFER[1] = ch;
						GUI_RX_STATE = 2;
						break;
		
					case 2: 	// Data Length;
		//				if(rx_data >= 0x80)
		//				{
		//					P_RX_STATE = 0;
		//					break;
		//				}
						GUI_RX_DATA_LENGTH = ch;
						GUI_RX_BUFFER[2] = ch;
						GUI_RX_COUNT = 0;
						GUI_RX_STATE = 3;
//							printf("R Len = %d\r\n", P_RX_DATA_LENGTH);
						break;
		
					case 3: 	// OpCode + DataLength + CRC
						GUI_RX_BUFFER[3 + GUI_RX_COUNT] = ch;
						if(++GUI_RX_COUNT >= (GUI_RX_DATA_LENGTH))
						{
							P_Received = 1;
							GUI_RX_STATE = 0;
		
						}
						break;
				}
			

//				DEBUG_RX_POOL[DEBUG_RX_SAVE] = ch;
//				if(++DEBUG_RX_SAVE >= DEF_DEBUG_UART_RX_POOL_SIZE)
//				{
//					DEBUG_RX_SAVE = 0;
//				}
			}
			break;
#endif
		}

	
	
}


void GUI_U7_ISR(void)
{
	U32 iir;
	unsigned char ch;
	iir = *(volatile U32*)(UART_BASE_7 + UART_IIR);
	iir &= 0xf;

	switch(iir)
	{
	//#ifdef CONFIG_UART_RX_INTERRUPT

		case 6:
			debugstring("UART Line Status Error(Overrun,Frame,Parity)\r\n");
		case 4:
		case 0xc:
			while((*(volatile U32*)(UART_BASE_7 + UART_LSR)) & ULSR_DRDY)//data ready
			{
				ch = *(volatile U8*)(UART_BASE_7 + UART_RBR);
//				*(volatile u8*)(UART_BASE_6 + UART_THR) = ch;
				U7_RX_BUFFER[U7_RX_COUNT++] = ch;

#if 1
				switch(opt_rx_state)
				{
					case 0:
						if(ch == SLAVE_STA)				//Header (Operation Packet) 
						{
							opt_rx_state++;
							opt_rx_count = 0;
						}
						break;

					default:
						if(ch == SLAVE_STA)				//Header (Operation Packet) 
						{
							opt_rx_count = 0;
						}
						else if(ch == SLAVE_ETX)		// Ender (Operation Packet) 
						{
							opt_rx_state = 0;
							Rep_Parsing_Add(opt_rx_data, opt_rx_count);
						}
						else									// Address & Data
						{
							if(!Code_Confirm_Table[ch]) // Code Receive Error	
							{
								opt_rx_state = 0;			// Restart
								if(opt_rx_count >= 9)		//POS_RX_ENDER) 
								{
									opt_rx_state = 0;
									Rep_Parsing_Add(opt_rx_data, opt_rx_count);

								}
							}
							else								// Code Receive Success
							{
								opt_rx_data[opt_rx_count++] = ch;

							//	if(opt_rx_count >= (9 + 1))
							//	{
							//		opt_rx_state  = 0;														// Restart
							//		Rep_Parsing_Add(opt_rx_data, opt_rx_count);
							//	}
							}
						}
						break;

				}
#endif				

			}
			break;
				

	}	
}



