#ifndef	__REPEATER_COMM__
#define	__REPEATER_COMM__


#define	POS_RX_ADDRESS1		0
#define	POS_RX_ADDRESS2		1
#define	POS_RX_ADDRESS3		2
#define	POS_RX_DATA1			3
#define	POS_RX_DATA2			4
#define	POS_RX_DATA3			5
#define	POS_RX_DATA4			6
#define	POS_RX_BCC_HIGH		7
#define	POS_RX_BCC_LOW		8
#define	POS_RX_ENDER			9


#define	POS_TX_ADDRESS 		5
#define	POS_TX_ADDRESS2 	6
#define	POS_TX_ADDRESS3 	7
#define	POS_TX_DATA1 		8
#define	POS_TX_DATA2 		9
#define	POS_TX_DATA3 		10
#define	POS_TX_DATA4 		11
#define	POS_TX_BCC_HIGH		12
#define	POS_TX_BCC_LOW		13
#define POS_TX_ENDER		14





enum REP_CMD_ENUM
{
	E_NULL,
	E_POLL_REGULAR,
	E_POLL_FAST
};


enum REP_POLL_ENUM
{
	E_TX,
	E_WAIT,
	E_NEXT
};



void POLLING_Initialize(void);

u8	POLLING_Process(void);

void Polling_Individual(unsigned short);

void Polling_Detector(unsigned short, unsigned char, unsigned char);


void Main_Timer_ISR(void);

void Repeater_Initialize(void);

void Repeater_Response(unsigned short, unsigned short);

void Parsing_Command(void);
void Detail_View(void);

u8 Parsing_Repeater(unsigned char);

void Master_Packet_Send(unsigned short);

void Master_Packet_Send_Individual(unsigned short);

void Master_Packet_Send_Det(unsigned short, unsigned char, unsigned char);

void View_Comm_Count(void);


void Manual_Packet_Send(unsigned short, unsigned short);


void Parsing_Add(unsigned char *, unsigned char);

BOOL monitor_time_Set(U32);
void monitor_time_run(void);

void CLI_mysettimer(int nCh, U32 cntval, U16 prescale, BOOL on_off );



void U6_ISR(void);

void U7_ISR(void);

void R_Board_ISR();



#endif

