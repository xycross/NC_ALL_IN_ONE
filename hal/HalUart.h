#ifndef __HALUART_H__
#define	__HALUART_H__

#include "Hal.h"


#define UART_BASE_0		0x80021400
#define UART_BASE_1		0x80021420
#define UART_BASE_2		0x80021440
#define	UART_BASE_3		0x80021460
#define UART_BASE_4		0x80021800
#define UART_BASE_5		0x80021820
#define UART_BASE_6		0x80021840
#define UART_BASE_7		0x80021860


#define UART_TX_RS232	R_UART0_BASE
#define UART_TX_DEBUG	R_UART3_BASE
#define UART_TX_SPOF1	R_UART4_BASE
#define UART_TX_SPOF2	R_UART5_BASE
#define UART_TX_SPOF3	R_UART6_BASE
#define UART_TX_MPOF	R_UART7_BASE






#define RS232_UART_BASE		UART_BASE_0
#define DEBUG_UART_BASE		UART_BASE_3
#define SPOF1_UART_BASE		UART_BASE_4
#define SPOF2_UART_BASE		UART_BASE_5
#define SPOF3_UART_BASE		UART_BASE_6
#define MPOF_UART_BASE		UART_BASE_7



#define RS232_UART_CH		0
#define DEBUG_UART_CH		3
#define SPOF1_UART_CH		4
#define SPOF2_UART_CH		5
#define SPOF3_UART_CH		6
#define MPOF_UART_CH		7

#define RS232_CH_BAUDRATE	115200
#define DEBUG_CH_BAUDRATE	115200
#define SPOF1_CH_BAUDRATE	115200
#define SPOF2_CH_BAUDRATE	115200
#define SPOF3_CH_BAUDRATE	115200
#define MPOF_CH_BAUDRATE	115200

void Uart_Init(void);

BOOL myUartConfig(int ch,int baud,UART_DATABITS databits,UART_STOPBITS stopbit,UART_PARITY parity);


BOOL myUart_OPTO232_Config(int ch,int baud,UART_DATABITS databits,UART_STOPBITS stopbit,UART_PARITY parity);

void RS232_TO_OPTIC_UART_ISR(void);
void SPOF3_TO_RS232_UART_ISR(void);

BOOL SMux_UartConfig(int ch,int baud,UART_DATABITS databits,UART_STOPBITS stopbit,UART_PARITY parity);



BOOL myUart_OPTBYPASS_Config(int ch,int baud,UART_DATABITS databits,UART_STOPBITS stopbit,UART_PARITY parity);

void RS232_BYPASS_ISR(void);
void MPOF_BYPASS_ISR(void);


void SMUX_OPT_UART_ISR_(void);

void MPOF_DISTRIBUTION_UART_ISR(void);

void SPOF1_DISTRIBUTION_UART_ISR(void);
void SPOF2_DISTRIBUTION_UART_ISR(void);
void SPOF3_DISTRIBUTION_UART_ISR(void);


void _RS232_UART_ISR_(void);



#endif


