#pragma once

#include "cantus.h"
#include "globalDefine.h"

#define ENQ		0x05	// Master Frame Header,, 	스카다 헤더
#define EOT		0x04	// Master Frame Tail,, 		스카다 엔더

#define STX		0x02	// Slave Header
#define ETX		0x03	// Slave Tail

#define	SCADA_MASTER_ENQ						0x05
#define	SCADA_MASTER_EOT						0x04
#define	SCADA_SLAVE_STX						0x02
#define	SCADA_SLAVE_ETX						0x03


#define	DEF_SCADA_RX_LENGTH					50



#define SCADA_PROTOCOL_DATA_MAX		256


// SCADA PROTOCOL ////////////////////
#define SCADA_READ_WORD			'R' //
#define SCADA_WRITE_WORD		'W' //
#define SCADA_READ_BIT			'r' //
#define SCADA_WRITE_BIT			'w' //
#define SCADA_ERROR_RESPONSE	'E' //
#define ENQ_INDEX			0  		//
#define STATION_H_INDEX		1  		//
#define STATION_L_INDEX		2  		//
#define CMD_INDEX			3  		//
#define LENGTH_H_INDEX		4  		//
#define LENGTH_L_INDEX		5  		//
#define DATATYPE_INDEX		6  		//
									//
#define w_r_STARTADD_INDEX	8 	 	//
#define W_R_STARTADD_INDEX	9  		//
#define w_r_STARTBIT_INDEX	13 		//
#define DATA_LENGTH_H_INDEX	14 		//
#define DATA_LENGTH_L_INDEX	15 		//
#define DATA_START_INDEX	16		//
//////////////////////////////////////



typedef struct 
{
	u8 	station;
	u8 	command;	
	u8 	dataLength;
	U8	dataType;
	u32	startAddress;
	u8	startBitNumber;	
	u16 wordData[SCADA_PROTOCOL_DATA_MAX];
	u8  bitData[SCADA_PROTOCOL_DATA_MAX];
	BOOL  check; 

}TS_SCADA_DATA;


void Scada_Processing(void);



void Scada_Parser(u8* _scadaRxBuf);
u8 Scada_MuxResponse(u16 _muxConfig[MUX_CONFIG]);

//u8 Scada_MuxResponse(u8* _txBuffer, u16 _muxConfig[MUX_CONFIG]);
u32 Scada_MuxDataApplyA(u16 _muxConfig[MUX_CONFIG], u16* _muxDataBuffer);
u32 Scada_MuxDataApplyB(u16* _muxDataBuffer);

void SMUX_RS232_UART_ISR_(void);



