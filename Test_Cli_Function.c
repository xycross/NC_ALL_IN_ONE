
#include <stdlib.h>
#include <string.h>

#include "cantus.h"



#include "System_Info.h"

#include "cli_n.h"
#include "TEST_cli_function.h"
#include "CLI_Repeater_Comm.h"

struct command TEST_CLI_commands[] = 
{
	{"r", cli_Regular_Poll},
	{"f", cli_Fast_Poll},
	{"i", cli_Individual_Poll},
	{"d", cli_Det_Poll},
	{"s", cli_stop},
	{"t", cli_time},
	{"m", cli_manual},
	{"q", cli_reset}

};

char command_help_disp[50][100] = 
{
	"Peroidic Polling(Time Interval Polling)",
	"Fast Polling(Packect RX -> Next Polling)",
	"Individual Poll (Repeater One.. Polling)",
	"Detector Poll(arg : rep_no, det_no, det_value)",
	"stop Polling (command 'r', 'f')",
	"TimeOut Time Setting",
	"Repeater Command",
	"Reset",
};

uint8_t TEST_CLI_num_commands = ARRAY_SIZE(TEST_CLI_commands);


extern unsigned int REP_RX_COUNT[1000];
extern unsigned int REP_COMM_COUNT;


extern unsigned char monitor_command;
extern unsigned short start_rep_no, end_rep_no;
extern unsigned short POLLING_TIME;

#if 0
extern struct _CELL_INFO CELL[MAX_REPEATER];
extern unsigned short OUTPUT_BOARD[MAX_OUTPUT_BOARD];

extern unsigned short REPEATERS[MAX_REPEATER][REPEATER_INFO];
#endif

//extern unsigned short Repeater_Status[NO_REPEATER + 1];
//extern unsigned short Detector_Status[NO_DETECTOR + 1];

//extern unsigned short Repeater_Command[NO_REPEATER + 1];
//extern unsigned short Detector_Command[NO_DETECTOR + 1];

extern unsigned char response;

unsigned char cli_cmd;
unsigned short cli_timing;

int cli_Regular_Poll(int argc, char *argv[])
{
	
	uint16_t r_no, state;
	char buffer[10];
	
	if(argc != 3)
	{
		debugprintf("poll start_rep_no end_rep_no\r\n");
		return 1;
	}


	start_rep_no = atoi(argv[1]);

#if 0
	if(strncmp(argv[2], "0x", 2) == 0)
	{
		strcpy(buffer, argv[2]);
		state = atoi(buffer+2);
	}
	else
	{
		state = atoi(argv[2]);
	}
#endif

	end_rep_no = str_to_dec(argv[2]);

	if(start_rep_no >= 1000)
	{
		debugstring("start_rep_no < 1000 require\r\n");
		return 1;
	}


	if(end_rep_no >= 1000)
	{
		debugstring("end_rep_no < 1000 require\r\n");
		return 1;
	}

	
	if(start_rep_no > end_rep_no)
	{
		debugstring("start_rep_no > end_rep_no\r\n");
		return 1;
	}

	memset(REP_RX_COUNT, 0, sizeof(REP_RX_COUNT));
	REP_COMM_COUNT = 0;
	debugprintf("Polling From %d to %d\r\n", start_rep_no, end_rep_no);
	POLLING_Initialize();
	cli_cmd = E_POLL_REGULAR;


	return 1;

}



int cli_manual(int argc, char *argv[])
{
	
	uint16_t r_no, state, w_data;
	char buffer[10];
	
	
	if(argc != 3)
	{
		debugprintf("poll start_rep_no end_rep_no\r\n");
		return 1;
	}


	r_no = atoi(argv[1]);

	

#if 1
	if(strncmp(argv[2], "0x", 2) == 0)
	{
		strcpy(buffer, argv[2]);
		w_data = strtol(buffer+2, NULL, 16);
	}
	else
	{
		w_data = atoi(argv[2]);
	}
#endif

	Manual_Packet_Send(r_no, w_data);




	return 1;

}

int cli_Fast_Poll(int argc, char *argv[])
{
	
	uint16_t r_no, state;
	char buffer[10];
	
	if(argc != 3)
	{
		debugprintf("poll start_rep_no end_rep_no\r\n");
		return 1;
	}


	start_rep_no = atoi(argv[1]);



	end_rep_no = str_to_dec(argv[2]);

	if(start_rep_no >= 1000)
	{
		debugstring("start_rep_no < 1000 require\r\n");
		return 1;
	}


	if(end_rep_no >= 1000)
	{
		debugstring("end_rep_no < 1000 require\r\n");
		return 1;
	}

	
	if(start_rep_no > end_rep_no)
	{
		debugstring("start_rep_no > end_rep_no\r\n");
		return 1;
	}

	memset(REP_RX_COUNT, 0, sizeof(REP_RX_COUNT));
	REP_COMM_COUNT = 0;
	debugprintf("Polling From %d to %d\r\n", start_rep_no, end_rep_no);
	POLLING_Initialize();
	cli_cmd = E_POLL_FAST;


	return 1;


}


int cli_Individual_Poll(int argc, char *argv[])
{
	u16 r_no;

	if(argc != 2)
	{
		debugprintf("time arg1 required\r\n");
		return 1;
	}

	r_no = atoi(argv[1]);

	debugprintf("Packet Tx Addr = %d\r\n", r_no);
	Polling_Individual(r_no);

	return 1;

}

int cli_Det_Poll(int argc, char *argv[])
{
	u16 r_no;
	u8 i, j;

	if(argc != 4)
	{
		debugprintf("4 args required\r\n");
		return 1;
	}

	r_no = atoi(argv[1]);

	i = atoi(argv[2]);
	j = atoi(argv[3]);

	debugprintf("Packet Tx Addr = %d\r\n", r_no);
	Polling_Detector(r_no, i, j);

	return 1;

}

int cli_Det(int argc, char *argv[])
{
#if 0
	uint16_t r_no;
	uint8_t i_no, state;
	
	if(argc != 3)
	{
		UARTprintf("Det no state\r\n");
		return 1;
	}


	r_no = atoi(argv[1]);
//	i_no = atoi(argv[2]);
	state = atoi(argv[2]);

	UARTprintf("Det_Point = %d, state = %d\r\n", r_no, state);


	Manage_Detector(ENUM_DET_VALUE_UP, r_no, state);
#endif	
	return 1;
}


int cli_CELL(int argc, char *argv[])
{
	u16 r_no;

	if(argc != 2)
	{
		debugprintf("time arg1 required\r\n");
		return 1;
	}

	r_no = atoi(argv[1]);

	debugprintf("Packet Tx Addr = %d\r\n", r_no);
	Polling_Individual(r_no);



	return 1;

}


int cli_time(int argc, char *argv[])
{
	uint16_t r_no, state;
	char buffer[10];
	
	if(argc > 2)
	{
		debugprintf("time arg1 required\r\n");
		return 1;
	}
	else if(argc == 1)
	{
		debugprintf("Polling Time = %d ms\r\n", POLLING_TIME);
		return 1;
	}


	r_no = atoi(argv[1]);
	POLLING_TIME = r_no;

	debugprintf("Setting Polling Time = %d ms\r\n", POLLING_TIME);
	
#if 0	
	uint8_t i, j;

	for(i=0;i<NO_OUTPUT_BOARD;i++)
	{
		UARTprintf("Output %d : ", i+1);
		for(j=0;j<16;j++)
		{
			if(TESTBIT(OUTPUT_BOARD[i], 15-j))
			{
				UARTprintf("1 ");
			}
			else 
			{
				UARTprintf("0 ");
			}

			if(j==7)
			{
				UARTprintf("     ");
			}
		}
		UARTprintf("\r\n");
	}
#endif
	return 1;
}



int cli_View_Repeater(int argc, char *argv[])
{
	uint16_t i;

	for(i=1;i<NO_REPEATER+1;i++)
	{
//		debugprintf("REP %d : C - %04X   S - %04X\r\n", i, Repeater_Command[i], Repeater_Status[i]);
	}
	
#if 0
	uint16_t r_no;
	uint8_t i;
	
	if(argc != 2)
	{
		UARTprintf("rview rep_no\r\n");
		return 1;
	}	

	r_no = atoi(argv[1]);

	UARTprintf("Rep No : %d\r\n");

	UARTprintf("REPEATER_COMMAND            : %02X \r\n", REPEATERS[r_no][0]);
	UARTprintf("DISPLAY_REPEATER_COMMAND    : %02X \r\n", REPEATERS[r_no][1]);
	UARTprintf("VIRTUAL_REPEATER_COMMAND    : %02X \r\n", REPEATERS[r_no][2]);
	UARTprintf("REPEATER_STATUS             : %02X \r\n", REPEATERS[r_no][3]);
	UARTprintf("REPEATER_STATUS2            : %02X \r\n", REPEATERS[r_no][4]);
	UARTprintf("REAL_REPEATER_STATUS        : %02X \r\n", REPEATERS[r_no][5]);
	UARTprintf("OLD_REPEATER_STATUS         : %02X \r\n", REPEATERS[r_no][6]);
	UARTprintf("REPEATER_COMM_ERROR         : %02X \r\n", REPEATERS[r_no][7]);


#endif
	return 1;
}


int cli_stop(int argc, char *argv[])
{
	cli_cmd = 0;
	
	return 1;
}

int cli_adc_dma_timer(int argc, char *argv[])
{
	return 1;
}

int cli_adc_result_clear(int argc, char *argv[])
{

	return 1;
}



int cli_response(int argc, char *argv[])
{
	uint8_t i;
	
	if(argc != 2)
	{
		debugprintf("Argument Require\r\n");
		return 1;
	}

	i = atoi(argv[1]);

	if(i)
	{
		debugstring("Response On\r\n");
		response = 1;
	}
	else
	{
		debugstring("Response Off\r\n");
		response = 0;
	}

	return 1;
}


int cli_poll(int argc, char *argv[])
{
	uint8_t i;
	
	if(argc != 2)
	{
		debugprintf("Argument Require\r\n");
		return 1;
	}

	i = atoi(argv[1]);

	if(i)
	{
		debugstring("Monitoring On\r\n");
		monitor_command = 1;
	}
	else
	{
		debugstring("Monitoring Off\r\n");
		monitor_command = 0;
	}

	return 1;
}









int cli_all(int argc, char *argv[])
{
	uint16_t i;

	for(i=1;i<NO_REPEATER + 1;i++)
	{
//		Repeater_Command[i] = 0;
//		Repeater_Status[i] = 0;

	}

	for(i=1;i<NO_DETECTOR + 1;i++)
	{
//		Detector_Command[i] = 0;
//		Detector_Status[i] = 0;

	}
	

	return 1;

}

int cli_reset(int argc, char *argv[])
{
	
	*R_PMCTRLEN |= (1<<7);

	*R_RSTCTRL = 1;
	
	return 1;
}

