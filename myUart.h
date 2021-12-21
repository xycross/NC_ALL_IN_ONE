#ifndef __MY_UART_H__
#define __MY_UART_H__

#define	UART_CH_DEBUG		3
#define	UART_CH_SCADA		0

#define	UART_CH_LP1	4
#define	UART_CH_LP2	5
#define	UART_CH_LP3	6
#define	UART_CH_LP4	7

BOOL myUartDebugConfig(int, int, UART_DATABITS, UART_STOPBITS, UART_PARITY);

//void myUartConfig(int, int, U8);
void TEST_CLI_myUartConfig(int, int, U8 );

void GUI_myUartConfig(int, int, U8);

void Debug_Uart_ISR(void);

void myUart4ISR(void);
void TEST_CLI_DEBUG_UART_ISR(void);


#endif

