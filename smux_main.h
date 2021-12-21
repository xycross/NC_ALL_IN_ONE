#pragma once



#define SCADA_COM_CH					0
#define SCADA_COM_BAUDRATE				115200

#define DEBUG_RS232_CH					3
#define DEBUG_RS232_CH_BAUDRATE			115200

#define REP_DET_COM_CH					6
#define REP_DET_COM_BAUDRATE			115200

#define SCADA_COM_TIMER					0
#define SCADA_COM_TIMER_us				1000	// 1ms
#define SCADA_COM_BYTE2BYTE_INTERVAL	1 		// ms단위


#define REP_DET_COM_TIMER				1
#define REP_DET_COM_TIMER_us			100
#define REP_DET_COM_BYTE2BYTE_INTERVAL	1		// ms단위


#define HIC_COM_DELAY_TIMER				1
#define HIC_COM_DELAY_TIMER_ms			5	
#define HIC_COM_TIMEOUT					2		// HIC_COM_DELAY_TIMER_ms * HIC_COM_TIMEOUT = 시간ms
#define HIC_COM_OFFLINE					1		// 
	
#define MUX_RUN_TIMER					3
#define MUX_RUN_TIMER_ms				1000

#define MUX_OPTIC_CH_BAUDRATE		115200
#define MUX_OPTIC_CH_4			4
#define MUX_OPTIC_CH_5			5
#define MUX_OPTIC_CH_6			6	
#define MUX_OPTIC_CH_7			7


enum switch_enum
{
	E_REP_TX,
	E_REP_WAIT,
	E_REP_DELAY_COUNT,
	E_DET_TX_READY,
	E_DET_TX,
	E_DET_WAIT,
	E_DET_DELAY_COUNT,
	E_DET_ADDR_UP,
	E_REP_ADDR_UP
	
};




int smux_main(void);

void smux_main_SystemInit();


//void main_SystemInit();
void main_VariableInit();
void main_RepeaterDataLoad();
void main_RepeaterDataSave();
void main_HICDelayCheck();
void main_WatchDog(int _ms, int _status);
void main_SMUXConfig();


void Find_Next(void);

void main_StatusLEDControl(int _count, int _delay);
void _SDATA_DATA_RECEIVE_TIMER_0_ISR_();
void _REP_DET_DATA_RECEIVE_TIMER_1_ISR_();
void _HIC_COM_DELAY_TIMER_2_ISR_();
void _MUX_RUN_TIMER_3_ISR_();

void Smux_System_Timer_ISR(void);

void smux_ledControl(void);

int Cli_Repeater_View(int argc, char *argv[]);

int Cli_Mux_Config_View(int argc, char *argv[]);





