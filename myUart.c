#include "cantus.h"
#include "myUart.h"

#include "HalUart.h"
//#include "R_Define.h"

//#include "System_Config.h"
//#include "Scada_Comm.h"
//#include "Repeater_Comm.h"

#include "GUI_Comm.h"
#include "CLI_Repeater_Comm.h"
#include "Cli_n.h"


extern unsigned char DEBUG_RX_POOL[DEF_DEBUG_UART_RX_POOL_SIZE];
extern unsigned char DEBUG_RX_SAVE, DEBUG_RX_PROCESS;

//U8 RX7_BUF[50];

//U8 RX7_RXPOINT, RX7_INT_COUNT;

/*
BOOL myUartDebugConfig(int ch,int baud,UART_DATABITS databits,UART_STOPBITS stopbit,UART_PARITY parity)
{
	UART_RING_BUF* pubuf;
	U32 apbclock;
	volatile U32 uart_base;
	pubuf = getringbuf(ch);

	uart_base =  (U32)R_UART0_BASE;
	if(uart_base==0)
		return FALSE;

	SetUartPAF(ch);
	InitRingBuf(pubuf);
	
	apbclock = GetAPBclock();
	U16 divisor = apbclock/(baud*16) ;
	if( (apbclock%(baud*16)) > ((baud*16)/2))
		divisor++;
	U32 lcr;//line control register
	lcr = ((stopbit - 1) << 2) + (databits - 5);
	switch (parity) 
	{
		case UART_PARODD:
			lcr |= ULCR_PAR_EN;
			break;
		case UART_PAREVEN:
			lcr |= ULCR_PAR_EN | ULCR_PAR_EVEN;
			break;
		case UART_PARNONE:
		default:
			break;
	}
	
	*(volatile U32*)(uart_base + UART_FCR) = UFCR_FIFOEN|UFCR_RFTL1B;
	lcr |= ULCR_DLAB;
	*(volatile U32*)(uart_base + UART_LCR) = lcr;
	*(volatile U32*)(uart_base + UART_DIV_LO) = divisor&0xff;
	*(volatile U32*)(uart_base + UART_DIV_HI) = (divisor>>8)&0xff;
	lcr &= ~ULCR_DLAB; //divisor latch access disable, fifo control write mode.
	*(volatile U32*)(uart_base + UART_LCR) = lcr;
	*(volatile U32*)(uart_base + UART_IER) = 0;//disable rx/tx interrupts
	 Uart_rx_flush(ch);
	 Uart_tx_flush(ch);

	
	switch(ch)
	{

		case 0:
			*R_IINTMOD &= ~(1<<INTNUM_UART0);//for the first empty interrupt
			setinterrupt(INTNUM_UART0,Uart0ISR);
			EnableInterrupt(INTNUM_UART0,TRUE);
			break;
	}
	
	
	return TRUE;
}
*/

void TEST_CLI_myUartConfig(int ch, int baud, U8 FIFO_LEVEL)
{
	U32 apbclock;
	volatile U32 uart_base;
	U8 fifo_set;

	switch (ch)
	{
		case 0:
			uart_base = (U32)R_UART0_BASE;
			break;
			
		case 1:
			uart_base = (U32)R_UART1_BASE;
			break;
			
		case 2:
			uart_base = (U32)R_UART2_BASE;
			break;
			
		case 3:
			uart_base = (U32)R_UART3_BASE;
			break;
			
		case 4:
			uart_base = (U32)R_UART4_BASE;
			break;
			
		case 5:
			uart_base = (U32)R_UART5_BASE;
			break;
			
		case 6:
			uart_base = (U32)R_UART6_BASE;
			break;
			
		case 7:
			uart_base = (U32)R_UART7_BASE;
			break;

		default:
			return;
	}

	
	if(uart_base==0) return;

	apbclock = GetAPBclock();
	U16 divisor = apbclock/(baud*16) ;
	if( (apbclock%(baud*16)) > ((baud*16)/2))
		divisor++;
	U32 lcr;//line control register
	lcr = (8 - 5);

	switch(FIFO_LEVEL)
	{
		case 1:
			fifo_set = (0<<6);
			break;

		case 4:
			fifo_set = (1<<6);
			break;

		case 8:
			fifo_set = (2<<6);
			break;

		case 14:
			fifo_set = (3<<6);
			break;

		default:
			break;
	}

//	*(volatile U32*)(uart_base + UART_FCR) = UFCR_FIFOEN|UFCR_RFTL3B;
	*(volatile U32*)(uart_base + UART_FCR) = UFCR_FIFOEN|fifo_set;	
	lcr |= ULCR_DLAB;
	*(volatile U32*)(uart_base + UART_LCR) = lcr;
	*(volatile U32*)(uart_base + UART_DIV_LO) = divisor&0xff;
	*(volatile U32*)(uart_base + UART_DIV_HI) = (divisor>>8)&0xff;
	lcr &= ~ULCR_DLAB; //divisor latch access disable, fifo control write mode.
	*(volatile U32*)(uart_base + UART_LCR) = lcr;
	*(volatile U32*)(uart_base + UART_IER) = 0;//disable rx/tx interrupts
	 Uart_rx_flush(ch);
	 Uart_tx_flush(ch);
	
	switch(ch)
	{
		case 0:
			*R_IINTMOD &= ~(1<<INTNUM_UART0);//for the first empty interrupt
//			setinterrupt(INTNUM_UART0, myUart4ISR);
//			setinterrupt(INTNUM_UART0, Cli_Uart0ISR);
			setinterrupt(INTNUM_UART0, TEST_CLI_DEBUG_UART_ISR);

			EnableInterrupt(INTNUM_UART0,TRUE);
			break;

		case 1:
//			*R_IINTMOD &= ~(1<<INTNUM_UART1);
//			setinterrupt(INTNUM_UART1, MAIN_Uart_ISR);
//			EnableInterrupt(INTNUM_UART1,TRUE);
			break;

		case 2:
			*R_IINTMOD &= ~(1<<INTNUM_UART2);
//			setinterrupt(INTNUM_UART2, U2_ISR);
//			EnableInterrupt(INTNUM_UART2,TRUE);
			break;


		case 3:
			*R_IINTMOD &= ~(1<<INTNUM_UART3);
//			setinterrupt(INTNUM_UART3, Debug_Uart_ISR);
//			setinterrupt(INTNUM_UART3, U3_ISR);
//			EnableInterrupt(INTNUM_UART3,TRUE);
			break;

		case 4:
			*R_IINTMOD &= ~(1<<INTNUM_UART4_OCR2A);
//			setinterrupt(INTNUM_UART4_OCR2A, Uart4ISR());
			setinterrupt(INTNUM_UART4_OCR2A, R_Board_ISR);

			EnableInterrupt(INTNUM_UART4_OCR2A,TRUE);
			break;

		case 5:
#ifdef	LOOP2_USE			
			*R_IINTMOD &= ~(1<<INTNUM_UART5_OCR2B);
			setinterrupt(INTNUM_UART5_OCR2B, Loop2_Uart_ISR);
			EnableInterrupt(INTNUM_UART5_OCR2B,TRUE);
#endif			
			break;

		case 6:

			*R_IINTMOD &= ~(1<<INTNUM_UART6_OCR3A);
			setinterrupt(INTNUM_UART6_OCR3A, U6_ISR);
			EnableInterrupt(INTNUM_UART6_OCR3A,TRUE);

			break;

		case 7:

			*R_IINTMOD &= ~(1<<INTNUM_UART7_OCR3B);
			setinterrupt(INTNUM_UART7_OCR3B, U7_ISR);
			EnableInterrupt(INTNUM_UART7_OCR3B,TRUE);

			break;

		default:
			break;

	}
	
	
	*(U32*)(uart_base+UART_IER) |= UIER_RDAIEN;
	
}

void TEST_CLI_DEBUG_UART_ISR(void)
{

	uint32_t iir;
	uint8_t ch;
	
	iir = *(volatile uint32_t*)(RS232_UART_BASE + UART_IIR);
	iir &= 0xf;
	
	switch(iir)
	{
		case 6:
			//UART Line Status Error(Overrun,Frame,Parity)
		case 4:
		case 0xc:
			//data ready
			while((*(volatile uint32_t*)(RS232_UART_BASE + UART_LSR)) & ULSR_DRDY) {
				ch = *(volatile uint8_t*)(RS232_UART_BASE+UART_RBR);
				DEBUG_RX_POOL[DEBUG_RX_SAVE] = ch;
				if(++DEBUG_RX_SAVE >= DEF_DEBUG_UART_RX_POOL_SIZE)
				{
					DEBUG_RX_SAVE = 0;
				}
			}
			break;
		default:
			ch = *(volatile uint8_t*)(RS232_UART_BASE+UART_RBR);				
			break;
	}
}	

void GUI_myUartConfig(int ch, int baud, U8 FIFO_LEVEL)
{
	U32 apbclock;
	volatile U32 uart_base;
	U8 fifo_set;

	switch (ch)
	{
		case 0:
			uart_base = (U32)R_UART0_BASE;
			break;
			
		case 1:
			uart_base = (U32)R_UART1_BASE;
			break;
			
		case 2:
			uart_base = (U32)R_UART2_BASE;
			break;
			
		case 3:
			uart_base = (U32)R_UART3_BASE;
			break;
			
		case 4:
			uart_base = (U32)R_UART4_BASE;
			break;
			
		case 5:
			uart_base = (U32)R_UART5_BASE;
			break;
			
		case 6:
			uart_base = (U32)R_UART6_BASE;
			break;
			
		case 7:
			uart_base = (U32)R_UART7_BASE;
			break;

		default:
			return;
	}

	
	if(uart_base==0) return;

	apbclock = GetAPBclock();
	U16 divisor = apbclock/(baud*16) ;
	if( (apbclock%(baud*16)) > ((baud*16)/2))
		divisor++;
	U32 lcr;//line control register
	lcr = (8 - 5);

	switch(FIFO_LEVEL)
	{
		case 1:
			fifo_set = (0<<6);
			break;

		case 4:
			fifo_set = (1<<6);
			break;

		case 8:
			fifo_set = (2<<6);
			break;

		case 14:
			fifo_set = (3<<6);
			break;

		default:
			break;
	}

//	*(volatile U32*)(uart_base + UART_FCR) = UFCR_FIFOEN|UFCR_RFTL3B;
	*(volatile U32*)(uart_base + UART_FCR) = UFCR_FIFOEN|fifo_set;	
	lcr |= ULCR_DLAB;
	*(volatile U32*)(uart_base + UART_LCR) = lcr;
	*(volatile U32*)(uart_base + UART_DIV_LO) = divisor&0xff;
	*(volatile U32*)(uart_base + UART_DIV_HI) = (divisor>>8)&0xff;
	lcr &= ~ULCR_DLAB; //divisor latch access disable, fifo control write mode.
	*(volatile U32*)(uart_base + UART_LCR) = lcr;
	*(volatile U32*)(uart_base + UART_IER) = 0;//disable rx/tx interrupts
	 Uart_rx_flush(ch);
	 Uart_tx_flush(ch);
	
	switch(ch)
	{
		case 0:
			*R_IINTMOD &= ~(1<<INTNUM_UART0);//for the first empty interrupt
//			setinterrupt(INTNUM_UART0, myUart4ISR);
//			setinterrupt(INTNUM_UART0, Cli_Uart0ISR);
			setinterrupt(INTNUM_UART0, GUI_Uart0ISR);

			EnableInterrupt(INTNUM_UART0,TRUE);
			break;

		case 1:
//			*R_IINTMOD &= ~(1<<INTNUM_UART1);
//			setinterrupt(INTNUM_UART1, MAIN_Uart_ISR);
//			EnableInterrupt(INTNUM_UART1,TRUE);
			break;

		case 2:
			*R_IINTMOD &= ~(1<<INTNUM_UART2);
//			setinterrupt(INTNUM_UART2, U2_ISR);
//			EnableInterrupt(INTNUM_UART2,TRUE);
			break;


		case 3:
			*R_IINTMOD &= ~(1<<INTNUM_UART3);
//			setinterrupt(INTNUM_UART3, Debug_Uart_ISR);
//			setinterrupt(INTNUM_UART3, U3_ISR);
//			EnableInterrupt(INTNUM_UART3,TRUE);
			break;

		case 4:
			*R_IINTMOD &= ~(1<<INTNUM_UART4_OCR2A);
//			setinterrupt(INTNUM_UART4_OCR2A, Uart4ISR());
//			setinterrupt(INTNUM_UART4_OCR2A, Cli_Uart0ISR);

			EnableInterrupt(INTNUM_UART4_OCR2A,TRUE);
			break;

		case 5:
#ifdef	LOOP2_USE			
			*R_IINTMOD &= ~(1<<INTNUM_UART5_OCR2B);
//			setinterrupt(INTNUM_UART5_OCR2B, Loop2_Uart_ISR);
			EnableInterrupt(INTNUM_UART5_OCR2B,TRUE);
#endif			
			break;

		case 6:

			*R_IINTMOD &= ~(1<<INTNUM_UART6_OCR3A);
//			setinterrupt(INTNUM_UART6_OCR3A, U6_ISR);
			EnableInterrupt(INTNUM_UART6_OCR3A,TRUE);

			break;

		case 7:

			*R_IINTMOD &= ~(1<<INTNUM_UART7_OCR3B);
			setinterrupt(INTNUM_UART7_OCR3B, GUI_U7_ISR);
			EnableInterrupt(INTNUM_UART7_OCR3B,TRUE);

			break;

		default:
			break;

	}
	
	
	*(U32*)(uart_base+UART_IER) |= UIER_RDAIEN;
	
}

#if 0
void Debug_Uart_ISR(void)
{
	U32 iir;
	U8 rx_data;

	iir = *(volatile U32*)(RX_DEBUG_CHANNEL + UART_IIR);
	iir &= 0xf;
	switch(iir)
	{

		case 6:
			debugstring("Line Main Error\r\n");
		case 4:
		case 0xc:
			while((*(volatile U32*)(RX_DEBUG_CHANNEL + UART_LSR)) & ULSR_DRDY)//data ready
			{
				rx_data = *(volatile U8*)(RX_DEBUG_CHANNEL + UART_RBR);
				*TX_DEBUG_CHANNEL = rx_data;
			}
			break;
	}
	
	
}


void myUart4ISR(void)
{
	U32 iir;
	U8 rx_data;

	iir = *(volatile U32*)(RX_MAIN_CHANNEL + UART_IIR);
	iir &= 0xf;
	switch(iir)
	{

		case 6:
			debugstring("Line Main Error\r\n");
		case 4:
		case 0xc:
			while((*(volatile U32*)(RX_MAIN_CHANNEL + UART_LSR)) & ULSR_DRDY)//data ready
			{
				rx_data = *(volatile U8*)(RX_MAIN_CHANNEL + UART_RBR);
				debugprintf("rxd : %c\r\n", rx_data);
			}
			break;
	}
	
}
#endif

#if 0
void LP2_Uart_ISR(void)
{
	U32 iir;
	U8 temp;

	iir = *(volatile U32*)(RX_LP2_CHANNEL + UART_IIR);
	iir &= 0xf;
	switch(iir)
	{

		case 6:
			debugstring("Line LP2 Error\r\n");
		case 4:
		case 0xc:
			while((*(volatile U32*)(RX_LP2_CHANNEL + UART_LSR)) & ULSR_DRDY)//data ready
			{
				temp = *(volatile U8*)(RX_LP2_CHANNEL + UART_RBR);
				switch(L2_RAW_RX_COUNT)
				{
					case 0:
						if(temp == 0xDD)
						{
							L2_RAW_RX_DATA[L2_RAW_SAVE_POS][L2_RAW_RX_COUNT++] = temp;
						}
						break;

					default:
						L2_RAW_RX_DATA[L2_RAW_SAVE_POS][L2_RAW_RX_COUNT++] = temp;
						if(L2_RAW_RX_COUNT == RS232_PACKET_SIZE)
						{
							L2_RAW_RX_COUNT = 0;
							if(++L2_RAW_SAVE_POS >= LOOP_RX_POOL_SIZE) L2_RAW_SAVE_POS = 0;
						}
						break;
				}
			}
			break;
	}
	
	
}
#endif



