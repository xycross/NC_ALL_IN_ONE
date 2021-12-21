#include "CLI_main.h"
#include "../include/cantus/timer.h"

#include "Definition.h"
#include "System_Info.h"

#include "Converter.h"
//#include "Differ_Process.h"
#include "CLI_Repeater_Comm.h"

//unsigned short Repeater_Command[NO_REPEATER + 1];
//unsigned short Repeater_Status[NO_REPEATER + 1];

unsigned char Rep_Tx_Buffer[15];

extern unsigned char Code_Confirm_Table[256];

extern unsigned char opt_rx_data[10], opt_rx_check, opt2_rx_data[10], opt2_rx_check;
unsigned char opt_rx_count, opt2_rx_count;

extern unsigned char opt_rx_state, opt2_rx_state;

unsigned short D_M_SAVE, D_M_PROCESS;

unsigned char Parsing_Pool[10][20];
unsigned char Parsing_Save, Parsing_Process;


unsigned int REP_RX_COUNT[1000];
unsigned int REP_COMM_COUNT;

extern unsigned char cli_cmd;
extern unsigned short cli_timing;

unsigned char POLL_ENUM;
unsigned short POLL_NO;
extern unsigned short POLLING_TIME;

unsigned short start_rep_no, end_rep_no;
unsigned char MASTER_TIMER_TIMEOUT;
unsigned char U7_RX_BUFFER[200], U7_RX_COUNT;
unsigned char RAW_MODE;

unsigned char RX_OK;

void Repeater_Initialize(void)
{

	Rep_Tx_Buffer[0] = MASTER_STA;
	Rep_Tx_Buffer[1] = MASTER_STA;
	Rep_Tx_Buffer[2] = MASTER_STA;
	Rep_Tx_Buffer[3] = MASTER_STA;
	Rep_Tx_Buffer[4] = MASTER_STA;

	Rep_Tx_Buffer[14] = MASTER_ETX;

	POLLING_TIME = 10;
}

void POLLING_Initialize(void)
{
	unsigned short i;

	for(i=0;i<1000;i++)
	{
		REP_RX_COUNT[i] = 0;
	}
	REP_COMM_COUNT = 0;
	POLL_ENUM = E_TX;
	POLL_NO = start_rep_no;
}

void Polling_Individual(unsigned short r_no)
{
	u16 k;
	u8 i;
	POLL_NO = r_no;
	memset(U7_RX_BUFFER, 0, sizeof(U7_RX_BUFFER));
	U7_RX_COUNT = 0;
	opt2_rx_state = 0;
	Master_Packet_Send_Individual(r_no);
	MASTER_TIMER_TIMEOUT = 0;
//			debugprintf("Poll Time = %d\r\n", POLLING_TIME);
	settimer(0,POLLING_TIME);
	Parsing_Save = 0;
	Parsing_Process = 0;
	

	while(1)
	{
		if(Parsing_Save != Parsing_Process)
		{
			if(Parsing_Repeater(1))
			{
				stoptimer(0);

				delayms(100);
				if(U7_RX_COUNT)
				{
					debugstring("RX Data : ");
					for(i=0;i<U7_RX_COUNT;i++)
					{
						debugprintf("%02X ", U7_RX_BUFFER[i]);
					}
					debugstring("\r\n");
					Detail_View();
				}
	//			if(++Parsing_Process >= 10)
	//			{
	//				Parsing_Process = 0;
	//			}
				
				
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
			if(U7_RX_COUNT)
			{
				debugprintf("%d RX : ", POLL_NO);
				for(k=0;k<U7_RX_COUNT;k++)
				{
					debugprintf("%02X ", U7_RX_BUFFER[k]);
				}
				debugstring("\r\n");
				debugprintf("Opt Rx Count : %d\r\n", opt2_rx_count);
				Detail_View();
			}
			break;
		}


	}
}



void Polling_Detector(unsigned short r_no, unsigned char det_no, unsigned char det_Value)
{
	u16 k;
	u8 i;
	POLL_NO = r_no;
	memset(U7_RX_BUFFER, 0, sizeof(U7_RX_BUFFER));
	U7_RX_COUNT = 0;
	opt2_rx_state = 0;
	Master_Packet_Send_Det(r_no, det_no, det_Value);
	MASTER_TIMER_TIMEOUT = 0;
//			debugprintf("Poll Time = %d\r\n", POLLING_TIME);
	settimer(0,POLLING_TIME);
	Parsing_Save = 0;
	Parsing_Process = 0;
	

	while(1)
	{
		if(Parsing_Save != Parsing_Process)
		{
			if(Parsing_Repeater(1))
			{
				stoptimer(0);

				delayms(100);
				if(U7_RX_COUNT)
				{
					debugstring("RX Data : ");
					for(i=0;i<U7_RX_COUNT;i++)
					{
						debugprintf("%02X ", U7_RX_BUFFER[i]);
					}
					debugstring("\r\n");
					Detail_View();
				}
	//			if(++Parsing_Process >= 10)
	//			{
	//				Parsing_Process = 0;
	//			}
				
				
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
			if(U7_RX_COUNT)
			{
				debugprintf("%d RX : ", POLL_NO);
				for(k=0;k<U7_RX_COUNT;k++)
				{
					debugprintf("%02X ", U7_RX_BUFFER[k]);
				}
				debugstring("\r\n");
				debugprintf("Opt Rx Count : %d\r\n", opt2_rx_count);
				Detail_View();
			}
			break;
		}


	}
}


u8 POLLING_Process(void)
{
	unsigned short k;
	
	switch(POLL_ENUM)
	{
		case E_TX:
//			debugprintf("E_TX\r\n");
			memset(U7_RX_BUFFER, 0, sizeof(U7_RX_BUFFER));
			U7_RX_COUNT = 0;
			opt2_rx_state = 0;
			RX_OK = 0;
			Master_Packet_Send(POLL_NO);
			MASTER_TIMER_TIMEOUT = 0;
//			debugprintf("Poll Time = %d\r\n", POLLING_TIME);
			settimer(0,POLLING_TIME);
			
			POLL_ENUM = E_WAIT;
			break;

		case E_WAIT:
			if(Parsing_Save != Parsing_Process)
			{
				if(Parsing_Repeater(0))
				{
					RX_OK = 1;
	//				stoptimer(0);
	//				POLL_ENUM = E_NEXT;
				}
				
				if(++Parsing_Process >= 10)
				{
					Parsing_Process = 0;
				}
				
				if(cli_cmd == E_POLL_FAST)
				{
					stoptimer(0);
					POLL_ENUM = E_NEXT;
				}
			}
			else if(MASTER_TIMER_TIMEOUT)
			{
				if(!RX_OK)
				{
					debugprintf("TimeOut %d RX : ", POLL_NO);
					if(U7_RX_COUNT)
					{
						debugprintf("%d RX : ", POLL_NO);
						for(k=0;k<U7_RX_COUNT;k++)
						{
							debugprintf("%02X ", U7_RX_BUFFER[k]);
						}
						debugstring("\r\n");
					}
					else
					{
						debugprintf("\r\n");
					}
					
				}
				POLL_ENUM = E_NEXT;
			}
			break;

		case E_NEXT:
			delayms(1);
//			debugprintf("E_NEXT\r\n");
			if(++POLL_NO > end_rep_no)
			{
				REP_COMM_COUNT++;
				POLL_NO = start_rep_no;
//				debugprintf("Next Poll No = %d\r\n", POLL_NO);

				
				if(cli_cmd < E_POLL_REGULAR)
				{
					return 0;
				}
				
			}
			


			POLL_ENUM = E_TX;
			break;
	}

	return 1;
}

void Detail_View(void)
{
	u8 i;

	debugstring("UnCode : ");
	for(i=0;i<U7_RX_COUNT;i++)
	{
		if(Code_Confirm_Table[U7_RX_BUFFER[i]])
		{
			debugprintf("%X  ", UnCodiz_4bit(U7_RX_BUFFER[i]));
		}
		else
		{
			debugprintf("%02X ", U7_RX_BUFFER[i]);
		}
	}
	debugstring("\r\n");
}

void Master_Packet_Send(unsigned short addr)
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

	for(i=0;i<PACKET_SIZE;i++)
	{
		*(volatile u8*)(UART_BASE_7 + UART_THR) = Rep_Tx_Buffer[i];
	}

	
}

void Master_Packet_Send_Individual(unsigned short addr)
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

	debugstring("\r\nTX Data : ");
	for(i=0;i<PACKET_SIZE;i++)
	{
		debugprintf("%02X ", Rep_Tx_Buffer[i]);
	}
	debugstring("\r\n");
	

	monitor_time_Set(10);
	monitor_time_run();

	for(i=0;i<PACKET_SIZE;i++)
	{
		*(volatile u8*)(UART_BASE_7 + UART_THR) = Rep_Tx_Buffer[i];
	}

	
}


void Master_Packet_Send_Det(unsigned short addr, unsigned char det_n, unsigned char det_value)
{
	unsigned char i, j;

	SETBIT(addr, 11);
	Codiz_Data12bit(&Rep_Tx_Buffer[POS_TX_ADDRESS], addr);
	Codiz_Data8bit(&Rep_Tx_Buffer[POS_TX_DATA1], det_n);
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

void Manual_Packet_Send(unsigned short addr, unsigned short w_data)
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

void View_Comm_Count(void)
{
	unsigned short i, j, per;

	debugprintf("Communication Report ---- Rep From %d To %d\r\n", start_rep_no, end_rep_no);
	debugprintf("........................ Polling Time : %d\r\n", POLLING_TIME);
	debugprintf("Total Tx : %ld\r\n", REP_COMM_COUNT);

	for(i=start_rep_no;i<(end_rep_no + 1);i++)
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

void Main_Timer_ISR(void)
{
	
	stoptimer(0);
//	debugstring("MAin Timer...\r\n");
	MASTER_TIMER_TIMEOUT = 1;
}

void Parsing_Command(void)
{
	u16 addr, cmd_data;

	addr = UnCodiz_12bit(opt_rx_data);
	UnCodiz_16bit(&opt_rx_data[3], &cmd_data);
	debugprintf("RAW Addr = %03X, Data = %04X  ", addr, cmd_data);

	if(TESTBIT(addr, 11))
	{
		CLEARBIT(addr, 11);
		debugprintf("DET - Rep %d, ", addr);

		

		debugprintf("%d - %02X\r\n", (cmd_data >> 8), (cmd_data & 0x00FF));
	}
	else
	{
		debugprintf("REP - %d : ", addr);

		UnCodiz_16bit(&opt_rx_data[3], &cmd_data);

		debugprintf("%04X\r\n", cmd_data);
		
	}
}

void Parsing_Add(unsigned char *pos, unsigned char length)
{
//	debugprintf("Parsing Add pos = %d, length = %d\r\n", Parsing_Save, length);
	memcpy(Parsing_Pool[Parsing_Save], pos, length);
	if(++Parsing_Save >= 10)
	{
		Parsing_Save = 0;
	}
}


BOOL monitor_time_Set(U32 ms)
{
	int nCh = 5;
	U32 r_tpcon = 0x80021000 + 0x20*nCh;
	U32 r_tmcon = 0x80021004 + 0x20*nCh;
	U32 r_tmcnt = 0x80021008 + 0x20*nCh;
	int intnum = nCh*4+1;
	int conval;
	
	long long int nanosecpertick = 1000000000/(GetAPBclock()/2);
	long long int usec = (long long int)ms*1000;
	long long int cnt = ((long long int)usec*1000) / nanosecpertick;
	if(cnt > 0xffffffff) // ms is too big to set, adjust PRESACLE.
		return FALSE;
	//reset
	*(volatile U32*)r_tpcon = 1<<1;
	*(volatile U32*)r_tpcon = 0;

	conval = 0;

#if 0	
#if PRESCALE==2
	conval = 0;
#elif PRESCALE==8
	conval = 1<<1;
#elif PRESCALE==32
	conval = 2<<1;
#elif PRESCALE==128
	conval = 3<<1;
#elif PRESCALE==512
	conval = 4<<1;
#elif PRESCALE==2048
	conval = 5<<1;
#elif PRESCALE==8192
	conval = 6<<1;
#elif PRESCALE==32768
	conval = 7<<1;
#else
#error "Invalid PRESCALE value"
#endif
#endif	
//	*(volatile U32*)r_tmcnt = cnt;
	*(volatile U32*)r_tmcnt = 0xFFFFFFFF;

//	debugprintf("cnt = %d\r\n", cnt);
	*(volatile U32*)r_tmcon = conval;

//	debugprintf("r_tmcon = %08X\r\n", conval);
//	debugprintf("tmr value = %08X\r\n", *((volatile unsigned int*)0x800210e8));

	EnableInterrupt(intnum,TRUE);
	return TRUE;
}

void monitor_time_run(void)
{
//	*(volatile U32*)r_tmcon = conval;

	// Timer 5 Use

//	*((volatile unsigned int*)0x800210e4) += 1;
	*((volatile unsigned int*)0x800210A4) += 1;


}

void CLI_mysettimer(int nCh, U32 cntval, U16 prescale, BOOL on_off )
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
			setinterrupt(intnum, CLI_System_Timer_ISR);
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




u8 Parsing_Repeater(unsigned char indi)
{
	unsigned char i, j, m, n;
	unsigned short k, s_16;

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

		if(indi)
		{
		}
//		CLEARBIT(k, 11);
		if((k & 0x7FF) != POLL_NO)
		{
			debugprintf("Address Diff %d -> %d\r\n", POLL_NO, k);
			return 0;
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

void U6_ISR(void)
{
	U32 iir;
	unsigned char ch;
	iir = *(volatile U32*)(UART_BASE_6 + UART_IIR);
	iir &= 0xf;

	switch(iir)
	{
	//#ifdef CONFIG_UART_RX_INTERRUPT

		case 6:
			debugstring("UART Line Status Error(Overrun,Frame,Parity)\r\n");
		case 4:
		case 0xc:
			while((*(volatile U32*)(UART_BASE_6 + UART_LSR)) & ULSR_DRDY)//data ready
			{
				ch = *(volatile U8*)(UART_BASE_6 + UART_RBR);
//				*(volatile u8*)(UART_BASE_7 + UART_THR) = ch;

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

//							opt_rx_check = 1;
							Parsing_Add(opt_rx_data, opt_rx_count);
						}
						else									// Address & Data
						{
							if(!Code_Confirm_Table[ch]) // Code Receive Error	
							{
								opt_rx_state = 0;			// Restart
								if(opt_rx_count >= 9)		//POS_RX_ENDER) 
								{
//									opt_rx_check = 1;
									Parsing_Add(opt_rx_data, opt_rx_count);
								}
							}
							else								// Code Receive Success
							{
		//								OPT1_RX_RAW_DATA[OPT1_RX_RAW_SAVE][OPT1_RX_RAW_COUNT++] = rx_data;	// RX_Data Save in Buffer
								opt_rx_data[opt_rx_count++] = ch;
		//						if(OPT1_RX_RAW_COUNT >= REAL_RX_DATA_SIZE)

						//		if(opt_rx_count >= (9 + 1)) 			// if didn't read Ender
								if(opt_rx_count >= (9 + 1))			// No Ender Check
								{
									opt_rx_state  = 0;														// Restart
		//									if(++OPT1_RX_RAW_SAVE >= RAW_BUFFER_SIZE) OPT1_RX_RAW_SAVE = 0; // Next Save Buffer Select (Total 5) 
//									opt_rx_check = 1;
									Parsing_Add(opt_rx_data, opt_rx_count);
								}
							}
						}
						break;

				}
#endif
			}
			break;
				

	}
	
}

void U7_ISR(void)
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
				switch(opt2_rx_state)
				{
					case 0:
						if(ch == SLAVE_STA)				//Header (Operation Packet) 
						{
							opt2_rx_state++;
							opt2_rx_count = 0;
						}
						break;

					default:
						if(ch == SLAVE_STA)				//Header (Operation Packet) 
						{
							opt2_rx_count = 0;
						}
						else if(ch == SLAVE_ETX)		// Ender (Operation Packet) 
						{
							opt2_rx_state = 0;

	//						opt2_rx_check = 1;
							Parsing_Add(opt2_rx_data, opt2_rx_count);
						}
						else									// Address & Data
						{
							if(!Code_Confirm_Table[ch]) // Code Receive Error	
							{
								opt2_rx_state = 0;			// Restart
								if(opt2_rx_count >= 9)		//POS_RX_ENDER) 
								{
//									opt2_rx_check = 1;
									opt2_rx_state = 0;
									Parsing_Add(opt2_rx_data, opt2_rx_count);

								}
							}
							else								// Code Receive Success
							{
		//								OPT1_RX_RAW_DATA[OPT1_RX_RAW_SAVE][OPT1_RX_RAW_COUNT++] = rx_data;	// RX_Data Save in Buffer
								opt2_rx_data[opt2_rx_count++] = ch;
		//						if(OPT1_RX_RAW_COUNT >= REAL_RX_DATA_SIZE)

//								if(opt2_rx_count >= (9 + 1)) 			// if didn't read Ender
								if(opt2_rx_count >= (9))
								{
									opt2_rx_state  = 0;														// Restart
		//									if(++OPT1_RX_RAW_SAVE >= RAW_BUFFER_SIZE) OPT1_RX_RAW_SAVE = 0; // Next Save Buffer Select (Total 5) 
//									opt2_rx_check = 1;
									Parsing_Add(opt2_rx_data, opt2_rx_count);
								}
							}
						}
						break;

				}
#endif				

			}
			break;
				

	}	
}


void R_Board_ISR()
{
//	getuartfifo((U32)R_UART4_BASE,&ubuf4);

	U32 iir;
	unsigned char ch;
	iir = *(volatile U32*)(UART_BASE_4 + UART_IIR);
	iir &= 0xf;
	
	switch(iir)
	{
//#ifdef CONFIG_UART_RX_INTERRUPT
#if 1
		case 6:
			debugstring("UART Line Status Error(Overrun,Frame,Parity)\r\n");
		case 4:
		case 0xc:
			while((*(volatile U32*)(UART_BASE_4 + UART_LSR)) & ULSR_DRDY)//data ready
			{
				ch = *(volatile U8*)(UART_BASE_4 + UART_RBR);
//				*(volatile U8*)(UART_BASE_0 + UART_THR) = ch;
//				*(volatile U8*)(UART_BASE_0 + UART_THR) = ch;

				switch(opt_rx_state)
				{
					case 0:
						if(ch == MASTER_STA)				//Header (Operation Packet) 
						{
							opt_rx_state++;
							opt_rx_count = 0;
						}
						break;
	
					default:
						if(ch == MASTER_STA)				//Header (Operation Packet) 
						{
							opt_rx_count = 0;
						}
						else if(ch == MASTER_ETX)		// Ender (Operation Packet) 
						{
							opt_rx_state = 0;

							opt_rx_check = 1;
	//						TOGGLEBIT(GPIO_A_OUT, LED_PLC_RX_POS);
						}
						else									// Address & Data
						{
							if(!Code_Confirm_Table[ch])	// Code Receive Error	
							{
								opt_rx_state = 0;			// Restart
								if(opt_rx_count >= 9) 		//POS_RX_ENDER)	
								{
									opt_rx_check = 1;
								}
							}
							else								// Code Receive Success
							{
//								OPT1_RX_RAW_DATA[OPT1_RX_RAW_SAVE][OPT1_RX_RAW_COUNT++] = rx_data;	// RX_Data Save in Buffer
								opt_rx_data[opt_rx_count++] = ch;
		//						if(OPT1_RX_RAW_COUNT >= REAL_RX_DATA_SIZE)

								if(opt_rx_count >= (9 + 1)) 			// if didn't read Ender
								{
									opt_rx_state  = 0;														// Restart
//									if(++OPT1_RX_RAW_SAVE >= RAW_BUFFER_SIZE) OPT1_RX_RAW_SAVE = 0; // Next Save Buffer Select (Total 5) 
									opt_rx_check = 1;
								}
							}
						}
						break;
	
					}

				}
				break;
				

		}

#endif
}

	
	






