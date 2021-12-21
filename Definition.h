#ifndef	__DEFINITION_H__
#define	__DEFINITION_H__


#define 	SETBIT(ADDRESS,BIT)		(ADDRESS |= (1<<BIT))
#define 	CLEARBIT(ADDRESS,BIT)		(ADDRESS &= ~(1<<BIT))
#define 	TESTBIT(ADDRESS,BIT)		(ADDRESS & (1<<BIT))
#define 	TOGGLEBIT(ADDRESS, BIT) 	(ADDRESS ^= (1<<BIT))



#define	DEF_DETECTOR_INDI_BIT_POS	11
#define	DEF_DETECTOR_S1T0_BIT_POS	10

#define	PACKET_SIZE		15


#define	MASTER_STA		0xD5			//	0xFA
#define	MASTER_ETX		0xD4			//	0xFC

#define	SLAVE_STA		0xD2			//	0xF5
#define	SLAVE_ETX		0xD3			//	0xF7




#define POS_TX_ADDRESS1			5
#define	POS_TX_ADDRESS2			6
#define	POS_TX_ADDRESS3			7
#define	POS_TX_DATA1			8
#define	POS_TX_DATA2			9
#define	POS_TX_DATA3			10
#define	POS_TX_DATA4			11
#define	POS_TX_BCC_HIGH			12
#define	POS_TX_BCC_LOW			13
#define	POS_TX_ENDER			14



#define 	UART_BASE_0		0x80021400
#define 	UART_BASE_1		0x80021420
#define 	UART_BASE_2		0x80021440
#define	UART_BASE_3		0x80021460
#define 	UART_BASE_4		0x80021800
#define 	UART_BASE_5		0x80021820
#define 	UART_BASE_6		0x80021840
#define 	UART_BASE_7		0x80021860


#endif

