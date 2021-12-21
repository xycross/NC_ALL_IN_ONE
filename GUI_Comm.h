#ifndef	__GUI_COMM_H__
#define	__GUI_COMM_H__


#define	DEF_GUI_RX_POOL_SIZE	10


#define	MASTER_HEADER		0xAC
#define SLAVE_HEADER        0xC8



#define P_POS_HEADER        0
#define P_POS_SEQ_NO        1
#define P_POS_DATA_LENGTH   2
#define P_POS_OPCODE        3

#define P_POS_DATA_START    4


#define DEF_SEND_LIST_SIZE  20


#define	OP_ACK				0x06
#define	OP_NAK				0x15

#define OP_M_BOARD_CHECK    0x20
#define OP_S_BOARD_CHECK    0x22

#define OP_M_POLL_TIME      0x24
#define OP_S_POLL_TIME      0x26


#define	OP_M_PERIODIC_POLL	0x30
#define	OP_S_PERIODIC_POLL	0x32

#define OP_M_FAST_POLL      0x34
#define OP_S_FAST_POLL      0x36

#define	OP_S_POLL_MESSAGE	0x37


#define	OP_M_POLL_STOP		0x38

#define	OP_S_POLL_RESULT_HEADR	0x3A
#define	OP_S_POLL_RESULT_BODY	0x3B
#define	OP_S_POLL_RESULT_ENDER	0x3C


#define OP_M_INDI_POLL      0x40
#define OP_S_INDI_RESULT    0x42
#define OP_S_INDI_TIME      0x43
#define OP_S_INDI_PACKET     0x44


#define	OP_M_INDI_DET		0x50

#define OP_M_CMD_REP        0x60
#define OP_M_CMD_DET        0x61

#define OP_S_CMD_RESULT     0x62


#define OP_M_REP_TEST       0x70
#define OP_S_REP_RESULT     0x72

#define OP_M_REP_STOP       0x78

#define	DEF_GUI_TX_POOL_SIZE	100 //1000

#define	DEF_GUI_COMM_TIMEOUT_MS	500

#define	DEF_GUI_COMM_RETRY		3


#define	DEF_GUI_POLL_TYPE_PERIODIC	1
#define	DEF_GUI_POLL_TYPE_FAST		2



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



struct _TX_PACKET
{
	unsigned char opcode;
	unsigned char length;
	unsigned char ack_require;
	unsigned char message[50];
};


enum _GUI_TX_ENUM
{
	E_TX_MAKE_PACKET,
	E_TX_SEND,
	E_TX_WAIT_ACK,
	E_TX_NEXT
};

enum GUI_REP_POLL_ENUM
{
	GUI_REP_E_TX,
	GUI_REP_E_WAIT,
	GUI_REP_E_NEXT
};


enum GUI_REP_CMD_ENUM
{
	E_GUI_NULL,
	E_GUI_POLL_REGULAR,
	E_GUI_POLL_FAST
};


enum GUI_INDI_CMD_ENUM
{
	E_INDI_REP_ONLY,
	E_INDI_DET_ONLY,
	E_INDI_CMD
};

void GUI_Procedure(void);

void GUI_RX_Process(void);

void GUI_POLLING_PROCEDURE(void);
void GUI_INDI_PROCEDURE(void);


u8 GUI_Parsing_Repeater(unsigned char);

u8 GUI_Parsing_Indi(void);

void GUI_Poll_Initialize(void);

void GUI_TX_Process(void);

void GUI_TX_MAKE(unsigned char, unsigned char *, unsigned char, unsigned char);

void GUI_Polling_Report(void);

void GUI_Comm_Timer_ISR();

unsigned short Calculate_CRC_CCITT(const unsigned char*, int);

void Rep_Parsing_Add(unsigned char *, unsigned char);

void GUI_Master_Packet_Send(unsigned short);

void GUI_Master_Packet_Send_Individual(unsigned short);

void GUI_Master_Packet_Send_Det(unsigned short, unsigned char, unsigned char);



void GUI_Manual_Packet_Send(unsigned short, unsigned short);

void GUI_Repeater_Initialize(void);

void Main_Timer_ISR(void);



void GUI_Uart0ISR();

void GUI_U7_ISR(void);


#endif

