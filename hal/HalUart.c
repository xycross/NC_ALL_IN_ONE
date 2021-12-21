#include "HalUart.h"
#include "Cli_n.h"
#include "commOpticSwitch.h"
#include "Converter.h"
//#include "main.h"
#include "globalDefine.h"

#include "cimon-PLC-RS232_422_485.h"

//extern unsigned char DEBUG_RX_POOL[DEF_DEBUG_UART_RX_POOL_SIZE];
extern unsigned char DEBUG_RX_SAVE, DEBUG_RX_PROCESS;
extern u8 op_mode;


extern int debugch;

extern const unsigned char Code_Confirm_Table[256];

extern const unsigned char Code2_Confirm_Table[256];

extern u8 OPT_RAW_RX_BUFFER[200], OPT_RAW_RX_COUNT;
extern u8 OPT_RX_BUFFER[100], OPT_RX_COUNT, OPT_RX_FLAG;

extern u8 OPT_RX_STATE;

extern unsigned char DEBUG_RX_POOL[DEF_DEBUG_UART_RX_POOL_SIZE];

BOOL myUartConfig(int ch,int baud,UART_DATABITS databits,UART_STOPBITS stopbit,UART_PARITY parity);
BOOL myUart_Distribution_Config(int ch,int baud,UART_DATABITS databits,UART_STOPBITS stopbit,UART_PARITY parity);

//void _RS232_UART_ISR_(void);
void _DEBUG_UART_ISR_(void);
void _SPOF1_UART_ISR_(void);
void _SPOF2_UART_ISR_(void);
void _SPOF3_UART_ISR_(void);
void _MPOF_UART_ISR_(void);

void Uart_Init(void)
{
	debugch = 3;
	*R_PAF1 = 0x0000;

	myUartConfig(DEBUG_UART_CH, DEBUG_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);


	if(op_mode == OP_DISTRIBUTION)
	{
		myUart_Distribution_Config(SPOF1_UART_CH, SPOF1_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);
		myUart_Distribution_Config(SPOF2_UART_CH, SPOF2_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);	
		myUart_Distribution_Config(SPOF3_UART_CH, SPOF3_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);
		myUart_Distribution_Config(MPOF_UART_CH, MPOF_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);
	}
	else if(op_mode == OP_232_TO_OPTIC)
	{
		myUart_OPTO232_Config(RS232_UART_CH, RS232_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);
		myUart_OPTO232_Config(SPOF3_UART_CH, SPOF1_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);
	}
	else if(op_mode == OP_OPT_BYPASS)
	{
		myUart_OPTBYPASS_Config(RS232_UART_CH, RS232_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);
		myUart_OPTBYPASS_Config(MPOF_UART_CH, RS232_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);
	}
	else		// S-MUX Mode
	{
//		UartConfig(SCADA_COM_CH, SCADA_COM_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);
//		UartConfig(MUX_OPTIC_CH_4, MUX_OPTIC_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);
//		myUartConfig(RS232_UART_CH, RS232_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);
//		myUartConfig(DEBUG_UART_CH, DEBUG_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);	
//		myUartConfig(SPOF1_UART_CH, SPOF1_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);
//		myUartConfig(SPOF2_UART_CH, SPOF2_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);	
//		myUartConfig(SPOF3_UART_CH, SPOF3_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);
//		myUartConfig(MPOF_UART_CH, MPOF_CH_BAUDRATE, DATABITS_8, STOPBITS_1, UART_PARNONE);
	}
}



BOOL myUart_OPTO232_Config(int ch,int baud,UART_DATABITS databits,UART_STOPBITS stopbit,UART_PARITY parity) 
{
	U32 apbclock;
	volatile U32 uart_base;

	switch (ch) {

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
			return 0;
	}

	if(uart_base==0)
		return FALSE;

//	SetUartPAF(ch);
	
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
			*R_IINTMOD &= ~(1<<INTNUM_UART0);
			setinterrupt(INTNUM_UART0,RS232_TO_OPTIC_UART_ISR);
			EnableInterrupt(INTNUM_UART0,TRUE);
			break;
			
		case 1:
			break;
			
		case 2:
			break;
			
		case 3:
			*R_IINTMOD &= ~(1<<INTNUM_UART3);
			setinterrupt(INTNUM_UART3,_DEBUG_UART_ISR_);
			EnableInterrupt(INTNUM_UART3,TRUE);
			break;
			
		case 4:
			*R_IINTMOD &= ~(1<<INTNUM_UART4_OCR2A);
//			setinterrupt(INTNUM_UART4_OCR2A, SPOF1_TO_RS232_UART_ISR);
//			EnableInterrupt(INTNUM_UART4_OCR2A,TRUE);
			break;
			
		case 5:
			*R_IINTMOD &= ~(1<<INTNUM_UART5_OCR2B);
			setinterrupt(INTNUM_UART5_OCR2B, SPOF2_DISTRIBUTION_UART_ISR);
			EnableInterrupt(INTNUM_UART5_OCR2B,TRUE);
			break;
			
		case 6:
			*R_IINTMOD &= ~(1<<INTNUM_UART6_OCR3A);
			setinterrupt(INTNUM_UART6_OCR3A, SPOF3_TO_RS232_UART_ISR);
			EnableInterrupt(INTNUM_UART6_OCR3A,TRUE);
			break;
					
		case 7:
			*R_IINTMOD &= ~(1<<INTNUM_UART7_OCR3B);
			setinterrupt(INTNUM_UART7_OCR3B, MPOF_DISTRIBUTION_UART_ISR);
			EnableInterrupt(INTNUM_UART7_OCR3B,TRUE);
			break;
		default:
			break;
	}

	// UART RX Intterupt Enable
	*(U32*)(uart_base+UART_IER) |= UIER_RDAIEN;
	return TRUE;
}



void RS232_TO_OPTIC_UART_ISR(void)
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
			while((*(volatile uint32_t*)(RS232_UART_BASE + UART_LSR)) & ULSR_DRDY) 
			{
				ch = *(volatile uint8_t*)(RS232_UART_BASE+UART_RBR);
//				*(volatile uint8_t*)(RS232_UART_BASE+UART_RBR) = ch;
				
//				DEBUG_RX_POOL[DEBUG_RX_SAVE] = ch;
//				if(++DEBUG_RX_SAVE >= DEF_DEBUG_UART_RX_POOL_SIZE)
//				{
//					DEBUG_RX_SAVE = 0;
//				}
//				if((ch == OPT_MASTER_STA) || (ch == OPT_MASTER_ETX) || (Code_Confirm_Table[ch]))
				if(((ch >= OPT_SLAVE_STA) && (ch <= OPT_MASTER_STA)) || (Code_Confirm_Table[ch]))
				{
					*UART_TX_SPOF3 = ch;
					LED_SET(LED_RS232, LED_RX);
					LED_SET(LED_SPOF3, LED_TX);
				}
			}
			break;
			
		default:
			ch = *(volatile uint8_t*)(RS232_UART_BASE+UART_RBR);				
			break;
	}
	
}


void SPOF3_TO_RS232_UART_ISR(void)
{
	uint32_t iir;
	uint8_t ch;
	
	iir = *(volatile uint32_t*)(SPOF3_UART_BASE + UART_IIR);
	iir &= 0xf;
	
	switch(iir)
	{
		case 6:
			//UART Line Status Error(Overrun,Frame,Parity)
		case 4:
		case 0xc:
			//data ready
			while((*(volatile uint32_t*)(SPOF3_UART_BASE + UART_LSR)) & ULSR_DRDY) 
			{
				ch = *(volatile uint8_t*)(SPOF3_UART_BASE+UART_RBR);
//				if((ch == OPT_SLAVE_STA) || (ch == OPT_SLAVE_ETX) || (Code_Confirm_Table[ch]))
				if(((ch >= OPT_SLAVE_STA) && (ch <= OPT_MASTER_STA)) || (Code_Confirm_Table[ch]))
				{
					*UART_TX_RS232 = ch;
					LED_SET(LED_SPOF3, LED_RX);
					LED_SET(LED_RS232, LED_TX);				
				}
		
//				opticSlaveSwitch(ch);		
			}
			break;
		default:
			break;
	}
	
}






BOOL myUart_OPTBYPASS_Config(int ch,int baud,UART_DATABITS databits,UART_STOPBITS stopbit,UART_PARITY parity) 
{
	U32 apbclock;
	volatile U32 uart_base;

	switch (ch) {

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
			return 0;
	}

	if(uart_base==0)
		return FALSE;

//	SetUartPAF(ch);
	
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
			*R_IINTMOD &= ~(1<<INTNUM_UART0);
			setinterrupt(INTNUM_UART0,RS232_BYPASS_ISR);
			EnableInterrupt(INTNUM_UART0,TRUE);
			break;
			
		case 1:
			break;
			
		case 2:
			break;
			
		case 3:
			*R_IINTMOD &= ~(1<<INTNUM_UART3);
			setinterrupt(INTNUM_UART3,_DEBUG_UART_ISR_);
			EnableInterrupt(INTNUM_UART3,TRUE);
			break;
			
		case 4:
			*R_IINTMOD &= ~(1<<INTNUM_UART4_OCR2A);
			setinterrupt(INTNUM_UART4_OCR2A, SPOF1_DISTRIBUTION_UART_ISR);
			EnableInterrupt(INTNUM_UART4_OCR2A,TRUE);
			break;
			
		case 5:
			*R_IINTMOD &= ~(1<<INTNUM_UART5_OCR2B);
			setinterrupt(INTNUM_UART5_OCR2B, SPOF2_DISTRIBUTION_UART_ISR);
			EnableInterrupt(INTNUM_UART5_OCR2B,TRUE);
			break;
			
		case 6:
			*R_IINTMOD &= ~(1<<INTNUM_UART6_OCR3A);
			setinterrupt(INTNUM_UART6_OCR3A, SPOF3_DISTRIBUTION_UART_ISR);
			EnableInterrupt(INTNUM_UART6_OCR3A,TRUE);
			break;
					
		case 7:
			*R_IINTMOD &= ~(1<<INTNUM_UART7_OCR3B);
			setinterrupt(INTNUM_UART7_OCR3B, MPOF_BYPASS_ISR);
			EnableInterrupt(INTNUM_UART7_OCR3B,TRUE);
			break;
		default:
			break;
	}

	// UART RX Intterupt Enable
	*(U32*)(uart_base+UART_IER) |= UIER_RDAIEN;
	return TRUE;
}

void RS232_BYPASS_ISR(void)
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
				*UART_TX_MPOF = ch;
//				*(volatile uint8_t*)(RS232_UART_BASE+UART_RBR) = ch;
				
//				DEBUG_RX_POOL[DEBUG_RX_SAVE] = ch;
//				if(++DEBUG_RX_SAVE >= DEF_DEBUG_UART_RX_POOL_SIZE)
//				{
//					DEBUG_RX_SAVE = 0;
//				}
				LED_SET(LED_RS232, LED_RX);
				LED_SET(LED_MPOF, LED_TX);
			}
			break;
		default:
			ch = *(volatile uint8_t*)(RS232_UART_BASE+UART_RBR);				
			break;
	}
}



void MPOF_BYPASS_ISR(void)
{	
	uint32_t iir;
	uint8_t ch;
	
	iir = *(volatile U32*)(MPOF_UART_BASE + UART_IIR);
	iir &= 0xf;
	
	switch(iir) 
	{
		case 6:
			//UART Line Status Error(Overrun,Frame,Parity)
		case 4:
		case 0xc:
			//data ready
			while((*(volatile uint32_t*)(MPOF_UART_BASE + UART_LSR)) & ULSR_DRDY) 
			{
				ch = *(volatile uint8_t*)(MPOF_UART_BASE+UART_RBR);
//				if((ch == OPT_MASTER_STA) || (ch == OPT_MASTER_ETX) || (Code_Confirm_Table[ch])) 
				{
					*UART_TX_RS232 = ch;
					
					LED_SET(LED_MPOF, LED_RX);
					LED_SET(LED_RS232, LED_TX);
//					LED_SET(LED_SPOF1, LED_TX);
//					LED_SET(LED_SPOF2, LED_TX);
//					LED_SET(LED_SPOF3, LED_TX); 	
				}				
//				opticMasterSwitch(ch);
			}
			break;
		default:	
			break;
	}
}

BOOL myUart_Distribution_Config(int ch,int baud,UART_DATABITS databits,UART_STOPBITS stopbit,UART_PARITY parity) 
{
	U32 apbclock;
	volatile U32 uart_base;

	switch (ch) {

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
			return 0;
	}

	if(uart_base==0)
		return FALSE;

//	SetUartPAF(ch);
	
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
			*R_IINTMOD &= ~(1<<INTNUM_UART0);
			setinterrupt(INTNUM_UART0,_RS232_UART_ISR_);
			EnableInterrupt(INTNUM_UART0,TRUE);
			break;
			
		case 1:
			break;
			
		case 2:
			break;
			
		case 3:
			*R_IINTMOD &= ~(1<<INTNUM_UART3);
			setinterrupt(INTNUM_UART3,_DEBUG_UART_ISR_);
			EnableInterrupt(INTNUM_UART3,TRUE);
			break;
			
		case 4:
			*R_IINTMOD &= ~(1<<INTNUM_UART4_OCR2A);
			setinterrupt(INTNUM_UART4_OCR2A, SPOF1_DISTRIBUTION_UART_ISR);
			EnableInterrupt(INTNUM_UART4_OCR2A,TRUE);
			break;
			
		case 5:
			*R_IINTMOD &= ~(1<<INTNUM_UART5_OCR2B);
			setinterrupt(INTNUM_UART5_OCR2B, SPOF2_DISTRIBUTION_UART_ISR);
			EnableInterrupt(INTNUM_UART5_OCR2B,TRUE);
			break;
			
		case 6:
			*R_IINTMOD &= ~(1<<INTNUM_UART6_OCR3A);
			setinterrupt(INTNUM_UART6_OCR3A, SPOF3_DISTRIBUTION_UART_ISR);
			EnableInterrupt(INTNUM_UART6_OCR3A,TRUE);
			break;
					
		case 7:
			*R_IINTMOD &= ~(1<<INTNUM_UART7_OCR3B);
			setinterrupt(INTNUM_UART7_OCR3B, MPOF_DISTRIBUTION_UART_ISR);
			EnableInterrupt(INTNUM_UART7_OCR3B,TRUE);
			break;
		default:
			break;
	}

	// UART RX Intterupt Enable
	*(U32*)(uart_base+UART_IER) |= UIER_RDAIEN;
	return TRUE;
}



BOOL myUartConfig(int ch,int baud,UART_DATABITS databits,UART_STOPBITS stopbit,UART_PARITY parity)
{
	U32 apbclock;
	volatile U32 uart_base;

	switch (ch) {

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
			return 0;
	}

	if(uart_base==0)
		return FALSE;

//	SetUartPAF(ch);
	
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
			*R_IINTMOD &= ~(1<<INTNUM_UART0);
//			setinterrupt(INTNUM_UART0,_RS232_UART_ISR_);
			setinterrupt(INTNUM_UART0, SMUX_RS232_UART_ISR_);

			EnableInterrupt(INTNUM_UART0,TRUE);
			break;
			
		case 1:
			break;
			
		case 2:
			break;
			
		case 3:
			*R_IINTMOD &= ~(1<<INTNUM_UART3);
			setinterrupt(INTNUM_UART3,_DEBUG_UART_ISR_);
			EnableInterrupt(INTNUM_UART3,TRUE);
			break;
			
		case 4:
			*R_IINTMOD &= ~(1<<INTNUM_UART4_OCR2A);
			setinterrupt(INTNUM_UART4_OCR2A,_SPOF1_UART_ISR_);
			EnableInterrupt(INTNUM_UART4_OCR2A,TRUE);
			break;
			
		case 5:
			*R_IINTMOD &= ~(1<<INTNUM_UART5_OCR2B);
			setinterrupt(INTNUM_UART5_OCR2B,_SPOF2_UART_ISR_);
			EnableInterrupt(INTNUM_UART5_OCR2B,TRUE);
			break;
			
		case 6:
			*R_IINTMOD &= ~(1<<INTNUM_UART6_OCR3A);
			setinterrupt(INTNUM_UART6_OCR3A,_SPOF3_UART_ISR_);
			EnableInterrupt(INTNUM_UART6_OCR3A,TRUE);
			break;
					
		case 7:
			*R_IINTMOD &= ~(1<<INTNUM_UART7_OCR3B);
			setinterrupt(INTNUM_UART7_OCR3B,_MPOF_UART_ISR_);
			EnableInterrupt(INTNUM_UART7_OCR3B,TRUE);
			break;
		default:
			break;
	}

	// UART RX Intterupt Enable
	*(U32*)(uart_base+UART_IER) |= UIER_RDAIEN;
	return TRUE;
}



BOOL SMux_UartConfig(int ch,int baud,UART_DATABITS databits,UART_STOPBITS stopbit,UART_PARITY parity) 
{
	U32 apbclock;
	volatile U32 uart_base;

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
			return 0;
	}

	if(uart_base==0)
		return FALSE;

//	SetUartPAF(ch);
	
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
			*R_IINTMOD &= ~(1<<INTNUM_UART0);
			setinterrupt(INTNUM_UART0,_RS232_UART_ISR_);
			EnableInterrupt(INTNUM_UART0,TRUE);
			break;
			
		case 1:
			break;
			
		case 2:
			break;
			
		case 3:
			*R_IINTMOD &= ~(1<<INTNUM_UART3);
			setinterrupt(INTNUM_UART3,_DEBUG_UART_ISR_);
			EnableInterrupt(INTNUM_UART3,TRUE);
			break;
			
		case 4:
			*R_IINTMOD &= ~(1<<INTNUM_UART4_OCR2A);
			setinterrupt(INTNUM_UART4_OCR2A,SMUX_OPT_UART_ISR_);
			EnableInterrupt(INTNUM_UART4_OCR2A,TRUE);
			break;
			
		case 5:
			*R_IINTMOD &= ~(1<<INTNUM_UART5_OCR2B);
			setinterrupt(INTNUM_UART5_OCR2B,_SPOF2_UART_ISR_);
			EnableInterrupt(INTNUM_UART5_OCR2B,TRUE);
			break;
			
		case 6:
			*R_IINTMOD &= ~(1<<INTNUM_UART6_OCR3A);
			setinterrupt(INTNUM_UART6_OCR3A,SMUX_OPT_UART_ISR_);
			EnableInterrupt(INTNUM_UART6_OCR3A,TRUE);
			break;
					
		case 7:
			*R_IINTMOD &= ~(1<<INTNUM_UART7_OCR3B);
			setinterrupt(INTNUM_UART7_OCR3B,SMUX_OPT_UART_ISR_);
			EnableInterrupt(INTNUM_UART7_OCR3B,TRUE);
			break;
		default:
			break;
	}

	// UART RX Intterupt Enable
	*(U32*)(uart_base+UART_IER) |= UIER_RDAIEN;
	return TRUE;
}






#if 1
void _RS232_UART_ISR_(void){
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
//				*(volatile uint8_t*)(RS232_UART_BASE+UART_RBR) = ch;
				
//				DEBUG_RX_POOL[DEBUG_RX_SAVE] = ch;
//				if(++DEBUG_RX_SAVE >= DEF_DEBUG_UART_RX_POOL_SIZE)
//				{
//					DEBUG_RX_SAVE = 0;
//				}
			}
			break;
		default:
			ch = *(volatile uint8_t*)(RS232_UART_BASE+UART_RBR);				
			break;
	}
}
#endif






void _DEBUG_UART_ISR_(void){
	uint32_t iir;
	uint8_t ch;
	
	iir = *(volatile uint32_t*)(DEBUG_UART_BASE + UART_IIR);
	iir &= 0xf;
	
	switch(iir)
	{
		case 6:
			//UART Line Status Error(Overrun,Frame,Parity)
		case 4:
		case 0xc:
			//data ready
			while((*(volatile uint32_t*)(DEBUG_UART_BASE + UART_LSR)) & ULSR_DRDY) {
				ch = *(volatile uint8_t*)(DEBUG_UART_BASE+UART_RBR);
				DEBUG_RX_POOL[DEBUG_RX_SAVE] = ch;
				if(++DEBUG_RX_SAVE >= DEF_DEBUG_UART_RX_POOL_SIZE)
				{
					DEBUG_RX_SAVE = 0;
				}
			}
			break;
		default:
			ch = *(volatile uint8_t*)(DEBUG_UART_BASE+UART_RBR);				
			break;
	}
}


void MPOF_DISTRIBUTION_UART_ISR(void)
{	
	uint32_t iir;
	uint8_t ch;
	
	iir = *(volatile U32*)(MPOF_UART_BASE + UART_IIR);
	iir &= 0xf;
	
	switch(iir) 
	{
		case 6:
			//UART Line Status Error(Overrun,Frame,Parity)
		case 4:
		case 0xc:
			//data ready
			while((*(volatile uint32_t*)(MPOF_UART_BASE + UART_LSR)) & ULSR_DRDY) 
			{
				ch = *(volatile uint8_t*)(MPOF_UART_BASE+UART_RBR);
				if((ch == OPT_MASTER_STA) || (ch == OPT_MASTER_ETX) || (Code_Confirm_Table[ch])) 
				{
					*UART_TX_SPOF1 = ch;
					*UART_TX_SPOF2 = ch;
					*UART_TX_SPOF3 = ch;
					
					LED_SET(LED_MPOF, LED_RX);
					LED_SET(LED_SPOF1, LED_TX);
					LED_SET(LED_SPOF2, LED_TX);
					LED_SET(LED_SPOF3, LED_TX); 	
				}				
//				opticMasterSwitch(ch);
			}
			break;
		default:	
			break;
	}
}



void SPOF1_DISTRIBUTION_UART_ISR(void)
{
	uint32_t iir;
	uint8_t ch;
	
	iir = *(volatile uint32_t*)(SPOF1_UART_BASE + UART_IIR);
	iir &= 0xf;
	
	switch(iir)
	{
		case 6:
			//UART Line Status Error(Overrun,Frame,Parity)
		case 4:
		case 0xc:
			//data ready
			while((*(volatile uint32_t*)(SPOF1_UART_BASE + UART_LSR)) & ULSR_DRDY) 
			{
				ch = *(volatile uint8_t*)(SPOF1_UART_BASE+UART_RBR);
				if((ch == OPT_SLAVE_STA) || (ch == OPT_SLAVE_ETX) || (Code_Confirm_Table[ch])) 
				{
					*UART_TX_MPOF = ch;

					LED_SET(LED_SPOF1, LED_RX);
					LED_SET(LED_MPOF, LED_TX);				
					
				}
		
//				opticSlaveSwitch(ch);		
			}
			break;
		default:
			break;
	}
	
}


void SPOF2_DISTRIBUTION_UART_ISR(void)
{
	uint32_t iir;
	uint8_t ch;
	
	iir = *(volatile uint32_t*)(SPOF2_UART_BASE + UART_IIR);
	iir &= 0xf;
	
	switch(iir)
	{
		case 6:
			//UART Line Status Error(Overrun,Frame,Parity)
		case 4:
		case 0xc:
			//data ready
			while((*(volatile uint32_t*)(SPOF2_UART_BASE + UART_LSR)) & ULSR_DRDY) 
			{
				ch = *(volatile uint8_t*)(SPOF2_UART_BASE+UART_RBR);
				if((ch == OPT_SLAVE_STA) || (ch == OPT_SLAVE_ETX) || (Code_Confirm_Table[ch])) 
				{
					*UART_TX_MPOF = ch;
					LED_SET(LED_SPOF2, LED_RX);
					LED_SET(LED_MPOF, LED_TX);				
					
				}
		
//				opticSlaveSwitch(ch);		
			}
			break;
		default:
			break;
	}
	
}



void SPOF3_DISTRIBUTION_UART_ISR(void)
{
	uint32_t iir;
	uint8_t ch;
	
	iir = *(volatile uint32_t*)(SPOF3_UART_BASE + UART_IIR);
	iir &= 0xf;
	
	switch(iir)
	{
		case 6:
			//UART Line Status Error(Overrun,Frame,Parity)
		case 4:
		case 0xc:
			//data ready
			while((*(volatile uint32_t*)(SPOF3_UART_BASE + UART_LSR)) & ULSR_DRDY) 
			{
				ch = *(volatile uint8_t*)(SPOF3_UART_BASE+UART_RBR);
				if((ch == OPT_SLAVE_STA) || (ch == OPT_SLAVE_ETX) || (Code_Confirm_Table[ch])) 
				{
					*UART_TX_MPOF = ch;
					LED_SET(LED_SPOF3, LED_RX);
					LED_SET(LED_MPOF, LED_TX);				
					
				}
		
//				opticSlaveSwitch(ch);		
			}
			break;
		default:
			break;
	}
	
}


void SMUX_OPT_UART_ISR_(void)
{
	uint32_t iir;
	uint8_t ch;
	
	iir = *(volatile uint32_t*)(SPOF3_UART_BASE + UART_IIR);
	iir &= 0xf;
	
	switch(iir)
	{
		case 6:
			//UART Line Status Error(Overrun,Frame,Parity)
		case 4:
		case 0xc:
			//data ready
			while((*(volatile uint32_t*)(SPOF3_UART_BASE + UART_LSR)) & ULSR_DRDY) {
				ch = *(volatile uint8_t*)(SPOF3_UART_BASE+UART_RBR);

				OPT_RAW_RX_BUFFER[OPT_RAW_RX_COUNT++] = ch;

				switch(OPT_RX_STATE)
				{
					case 0:
						if(ch == OPT_SLAVE_STA)
						{
							OPT_RX_STATE = 1;
							OPT_RX_COUNT = 0;
						}
						break;

					default:
						if(ch == OPT_SLAVE_STA)
						{
							OPT_RX_COUNT = 0;
						}
						else if(ch == OPT_SLAVE_ETX)
						{
							OPT_RX_STATE = 0;
							if(OPT_RX_COUNT >= 9)
							{
								OPT_RX_FLAG = 1;
							}
						}
						else
						{
							if(Code2_Confirm_Table[ch])
							{
								OPT_RX_BUFFER[OPT_RX_COUNT++] = ch;
							}
							else
							{
								OPT_RX_STATE = 0;
								if(OPT_RX_COUNT >= 9)
								{
									OPT_RX_FLAG = 1;
								}
							}
						}
							
						break;
				}

			}
			break;
		default:
			break;
	}
}


void _SPOF1_UART_ISR_(void){
	uint32_t iir;
	uint8_t ch;
	
	iir = *(volatile uint32_t*)(SPOF1_UART_BASE + UART_IIR);
	iir &= 0xf;
	
	switch(iir)
	{
		case 6:
			//UART Line Status Error(Overrun,Frame,Parity)
		case 4:
		case 0xc:
			//data ready
			while((*(volatile uint32_t*)(SPOF1_UART_BASE + UART_LSR)) & ULSR_DRDY) {
				ch = *(volatile uint8_t*)(SPOF1_UART_BASE+UART_RBR);
				opticSlaveSwitch(ch);		
				LED_SET(LED_SPOF1, LED_RX);
				LED_SET(LED_MPOF, LED_TX);				
			}
			break;
		default:
			break;
	}
}

void _SPOF2_UART_ISR_(void){
	uint32_t iir;
	uint8_t ch;
	iir = *(volatile uint32_t*)(SPOF2_UART_BASE + UART_IIR);
	iir &= 0xf;
	
	switch(iir)
	{
		case 6:
			//UART Line Status Error(Overrun,Frame,Parity)
		case 4:
		case 0xc:
			//data ready
			while((*(volatile uint32_t*)(SPOF2_UART_BASE + UART_LSR)) & ULSR_DRDY) {
				ch = *(volatile uint8_t*)(SPOF2_UART_BASE+UART_RBR);
				opticSlaveSwitch(ch);	
				LED_SET(LED_SPOF2, LED_RX);
				LED_SET(LED_MPOF, LED_TX);				
			}
			break;
		default:
			break;
	}
}

void _SPOF3_UART_ISR_(void){
	uint32_t iir;
	uint8_t ch;

	iir = *(volatile uint32_t*)(SPOF3_UART_BASE + UART_IIR);
	iir &= 0xf;
	
	switch(iir) {
		case 6:
			//UART Line Status Error(Overrun,Frame,Parity)
		case 4:
		case 0xc:
			//data ready
			while((*(volatile uint32_t*)(SPOF3_UART_BASE + UART_LSR)) & ULSR_DRDY) {
				ch = *(volatile uint8_t*)(SPOF3_UART_BASE+UART_RBR);
				opticSlaveSwitch(ch);
				LED_SET(LED_SPOF3, LED_RX);
				LED_SET(LED_MPOF, LED_TX);
			}
			break;
		default:				
			break;
	}
}

void _MPOF_UART_ISR_(void){	
	uint32_t iir;
	uint8_t ch;
	
	iir = *(volatile U32*)(MPOF_UART_BASE + UART_IIR);
	iir &= 0xf;
	
	switch(iir) {
		case 6:
			//UART Line Status Error(Overrun,Frame,Parity)
		case 4:
		case 0xc:
			//data ready
			while((*(volatile uint32_t*)(MPOF_UART_BASE + UART_LSR)) & ULSR_DRDY) {
				ch = *(volatile uint8_t*)(MPOF_UART_BASE+UART_RBR);
				opticMasterSwitch(ch);
				LED_SET(LED_MPOF, LED_RX);
				LED_SET(LED_SPOF1, LED_TX);
				LED_SET(LED_SPOF2, LED_TX);
				LED_SET(LED_SPOF3, LED_TX);		
			}
			break;
		default:	
			break;
	}
}



