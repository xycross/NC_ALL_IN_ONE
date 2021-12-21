#ifndef __HAL_H__
#define __HAL_H__

#include "cantus.h"
#include "HalBoard.h"
#include "HalTimer.h"
#include "HalUart.h"


typedef enum _COMM_LED_ENUM_ {
	LED_SPOF1, 
	LED_SPOF2,	
	LED_SPOF3,		
	LED_MPOF,		
	LED_RS232,
	LED_DEBUG,
	LED_MAX,
}LED_TYPE;

typedef enum _COMM_LED_RXTX_ {
	LED_RX,
	LED_TX,
	LED_TRX_MAX,
}LED_TRX;


#endif

