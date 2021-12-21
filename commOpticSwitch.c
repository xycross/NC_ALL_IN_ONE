#include "commOpticSwitch.h"
#include "Converter.h"

extern const unsigned char Code_Confirm_Table[256];

u8  m_RunLEDCount;
bool m_RunLEDState_p;

uint8_t ledSleep[LED_MAX][LED_TRX_MAX];
uint8_t ledFlag[LED_MAX][LED_TRX_MAX];


void opticMasterSwitch(uint8_t rx_data) {
	
	if(rx_data == OPT_MASTER_STA || rx_data == OPT_MASTER_ETX || Code_Confirm_Table[rx_data]) 
	{
		*UART_TX_SPOF1 = rx_data;				
		*UART_TX_SPOF2 = rx_data;				
		*UART_TX_SPOF3 = rx_data;		
	}
	else
	{
		debugprintf("%02X\r\n", rx_data);
	}
}

void opticSlaveSwitch(uint8_t rx_data) 
{

	if(rx_data == OPT_SLAVE_STA || rx_data == OPT_SLAVE_ETX || Code_Confirm_Table[rx_data]) 
	{
		*UART_TX_MPOF = rx_data;	
		
	}
	else
	{
		debugprintf("%02X\r\n", rx_data);
	}
}


void ledControl(void){
	uint8_t i;
	
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
	for(i=0; i<4; i++){
		if(ledSleep[i][LED_RX]) {
			--ledSleep[i][LED_RX];
			*R_P0oHIGH = (1<<i); 			
		}else if(ledFlag[i][LED_RX]) {
			*R_P0oLOW = (1<<i);
			--ledFlag[i][LED_RX];

			if(!ledFlag[i][LED_RX]) {
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

	
}


void LED_SET(LED_TYPE _type, LED_TRX _trx) {

	ledFlag[_type][_trx] = 2;
}

