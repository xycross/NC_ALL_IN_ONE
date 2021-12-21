#ifndef	__COMMOPTICSWITCH_H__
#define	__COMMOPTICSWITCH_H__

#include "Hal.h"

#define	OPT_MASTER_STA	0xD5
#define	OPT_MASTER_ETX	0xD4
#define	OPT_SLAVE_STA	0xD2
#define	OPT_SLAVE_ETX	0xD3   

void opticMasterSwitch(uint8_t rx_data); 
void opticSlaveSwitch(uint8_t rx_data); 
void ledControl(void);
void LED_SET(LED_TYPE _type, LED_TRX _trx);



#endif

