
#include <stdlib.h>
#include <string.h>



//#include "Repeater_Comm.h"

#include "cli_n.h"
#include "cli_function.h"

#include "smux_main.h"


struct command commands[] = 
{
	{"rep", Cli_Repeater_View},
	{"mux", Cli_Mux_Config_View}
/*		
	{"dets", cli_Detector_set},
	{"detq", cli_Det},
	{"cell", cli_CELL},
	{"output", cli_Output},
	{"rview", cli_View_Repeater},
	{"dview", cli_View_Detector},
	{"co2", cli_co2},
	{"dong", cli_dong},
	{"dset", cli_det_set},
	{"mset", cli_mon_set},
		
	{"sreset", cli_sys_reset},
	{"reset", cli_all}
*/	

};

unsigned char num_commands = ARRAY_SIZE(commands);



#if 0
extern struct _CELL_INFO CELL[MAX_REPEATER];
extern unsigned short OUTPUT_BOARD[MAX_OUTPUT_BOARD];

extern unsigned short REPEATERS[MAX_REPEATER][REPEATER_INFO];
#endif

//extern unsigned short Repeater_Status[NO_REPEATER + 1];
//extern unsigned short Detector_Status[NO_DETECTOR + 1];

//extern unsigned short Repeater_Command[NO_REPEATER + 1];
//extern unsigned short Detector_Command[NO_DETECTOR + 1];

#if 0
int cli_repeter_set(int argc, char *argv[])
{
	
	uint16_t r_no, state;
	char buffer[10];
	
	if(argc != 3)
	{
		UARTprintf("reps r_no i_no\r\n");
		return 1;
	}


	r_no = atoi(argv[1]);

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

	state = str_to_dec(argv[2]);


	UARTprintf("r_no = %d, state = %04X\r\n", r_no, state);
//	REPEATERS[r_no][REAL_REPEATER_STATUS] = state;

	if(REP_DET_MAP[r_no][0] >= DEF_DET)
	{
		REPEATERS[r_no][DETECTOR_VALUE] = state;
	}
	else
	{
		REPEATERS[r_no][REAL_REPEATER_STATUS] = state;	
	}


	return 1;

}


int cli_Detector_set(int argc, char *argv[])
{
	
	uint16_t r_no, state;
	uint16_t prefire, realfire;
//	char buffer[10];
	
	if(argc != 4)
	{
		UARTprintf("dets r_no pre real\r\n");
		return 1;
	}


	r_no = atoi(argv[1]);


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


	prefire = atoi(argv[2]);
	realfire = atoi(argv[3]);



#if 0
	state = str_to_dec(argv[2]);


	UARTprintf("r_no = %d, state = %04X\r\n", r_no, state);
//	Detector_Status[r_no] = state;
#endif

	UARTprintf("r_no = %d, prefire = %d, realfire = %d\r\n", r_no, prefire, realfire);
	REPEATERS[r_no][0] = prefire;
	REPEATERS[r_no][1] = realfire;

	return 1;

}


int cli_InputBoard(int argc, char *argv[])
{
#if 0	
	uint16_t r_no;
	uint8_t i_no, state;
	
	if(argc != 3)
	{
		UARTprintf("In no state\r\n");
		return 1;
	}


	r_no = atoi(argv[1]);
//	i_no = atoi(argv[2]);
	state = atoi(argv[2]);

	UARTprintf("In_Point = %d, state = %d\r\n", r_no, state);

	
	Add_Event(INPUT_BOARD_SIGNAL, r_no, state, 0);
#endif
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



int cli_det_set(int argc, char *argv[])
{
	uint16_t i, j;
	
	if(argc != 3)
	{
		UARTprintf("Argument Required\r\n");
		return 1;
	}

	i = atoi(argv[1]);
	j = atoi(argv[2]);

	switch(i)
	{
		case 0:
			R_STATUS.DETECTOR_TEMP_REAL_VALUE = j;
			break;

		case 1:
			R_STATUS.DETECTOR_TEMP_PRE_VALUE = j;
			break;

		case 2:
			R_STATUS.DETECTOR_SMOKE_REAL_VALUE = j;
			break;

		case 3:
			R_STATUS.DETECTOR_SMOKE_PRE_VALUE = j;
			break;

		case 5:
			Flash_System_Setting_Save();
			break;

		default:
			UARTprintf("Arg1 Not Match\r\n");
			break;
			
	}


	UARTprintf("DET TEMP REAL = %d, PRE = %d\r\n", R_STATUS.DETECTOR_TEMP_REAL_VALUE, R_STATUS.DETECTOR_TEMP_PRE_VALUE);
	UARTprintf("DET SMOKE REAL = %d, PRE = %d\r\n", R_STATUS.DETECTOR_SMOKE_REAL_VALUE, R_STATUS.DETECTOR_SMOKE_PRE_VALUE);
	
	return 1;
}

int cli_mon_set(int argc, char *argv[])
{
	uint16_t i, j;
	
	if(argc != 3)
	{
		UARTprintf("Argument Required\r\n");
		return 1;
	}

	i = atoi(argv[1]);
	j = atoi(argv[2]);

	switch(i)
	{
		case 0:
			R_STATUS.REPEATER_FIRE_MON_COUNT = j;
			break;

		case 1:
			R_STATUS.DETECTOR_FIRE_MON_COUNT = j;
			break;

		default:
			UARTprintf("Arg1 Not Match\r\n");
			break;
			
	}

	UARTprintf("FIRE MON REP = %d, DET = %d\r\n", R_STATUS.REPEATER_FIRE_MON_COUNT, R_STATUS.DETECTOR_FIRE_MON_COUNT);
	return 1;
}





int cli_CELL(int argc, char *argv[])
{
	uint16_t i;
	
	if(argc != 2)
	{
		UARTprintf("Argument Required\r\n");
		return 1;
	}


	i = atoi(argv[1]);

	UARTprintf("Cell %d...\r\n", i);
	UARTprintf("Code :	%c	 	%c 		%c 		%c\r\n", CELL[i].Code[0], CELL[i].Code[1], CELL[i].Code[2], CELL[i].Code[3]); 
	UARTprintf("SW   :	%d	 	%d 		%d 		%d\r\n", CELL[i].SW[0], CELL[i].SW[1], CELL[i].SW[2], CELL[i].SW[3]); 
	UARTprintf("Out  :	%d	 	%d 		%d 		%d\r\n", CELL[i].Out_Count[0], CELL[i].Out_Count[1], CELL[i].Out_Count[2], CELL[i].Out_Count[3]); 
	UARTprintf("Comm :	%d\r\n", CELL[i].COMM_STATE);
	UARTprintf("Real :	%02X 	%02X	%02X	%02X\r\n", CELL[i].Real_State[0], CELL[i].Real_State[1], CELL[i].Real_State[2], CELL[i].Real_State[3]); 
	UARTprintf("Vir  :	%02X 	%02X	%02X	%02X\r\n", CELL[i].Virtual_State[0], CELL[i].Virtual_State[1], CELL[i].Virtual_State[2], CELL[i].Virtual_State[3]); 
	UARTprintf("Accu  :	%d	 	%d		%d		%d\r\n", CELL[i].Accum_Time[0], CELL[i].Accum_Time[1], CELL[i].Accum_Time[2], CELL[i].Accum_Time[3]); 	
	UARTprintf("Cir  :	%02X 	%02X	%02X	%02X\r\n", CELL[i].Circuit_Off[0], CELL[i].Circuit_Off[1], CELL[i].Circuit_Off[2], CELL[i].Circuit_Off[3]); 	
	UARTprintf("YDT  :	%d	 	%d		%d		%d\r\n", CELL[i].YDT_WHERE[0], CELL[i].YDT_WHERE[1], CELL[i].YDT_WHERE[2], CELL[i].YDT_WHERE[3]); 		
	UARTprintf("Cros :	%d	 	%d		%d		%d\r\n", CELL[i].CrossCheck[0], CELL[i].CrossCheck[1], CELL[i].CrossCheck[2], CELL[i].CrossCheck[3]);




	

#if 0	
	uint16_t i, j;
	uint8_t k, out_cnt;

	for(i=0;i<NO_REAL_REPEATER;i++)
	{
		k = 0;
		for(j=0;j<REPEATER_PER_INPUT;j++)
		{
			if((CELL[i].Real_State[j] != 0) || (CELL[i].Virtual_State[j] != 0))
			{
				if(k==0)
				{
					UARTprintf("CELL %d......\r\n", i);
					k = 1;
				}

				UARTprintf("IN : %d - Real %d, Virtual %d\r\n", j+1, CELL[i].Real_State[j], CELL[i].Virtual_State[j]);
			}

		}
		
		out_cnt = 0;
		for(j=0;j<REPEATER_PER_INPUT;j++)
		{
			if(CELL[i].Out_Count[j])
			{
				if(k==0)
				{
					UARTprintf("CELL %d......\r\n", i);
					k = 1;
				}

				UARTprintf("Out %d: %d  ", j+1, CELL[i].Out_Count[j]);
				out_cnt++;
			}

		}
		if(out_cnt)
		{
			UARTprintf("\r\n");
		}

		
	}
#endif
	return 1;

}


int cli_Output(int argc, char *argv[])
{
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

//	for(i=1;i<NO_REPEATER+1;i++)
	{
//		UARTprintf("REP %d : C - %04X   S - %04X\r\n", i, Repeater_Command[i], Repeater_Status[i]);
	}


	if(argc != 2)
	{
		UARTprintf("Argument Require... Rep No\r\n");
		return 1;
	}

	i = atoi(argv[1]);


	if(REP_DET_MAP[i][0] >= DEF_DET)
	{
		UARTprintf("Detector %d...\r\n");
		UARTprintf("P_FIRE = %d, R_FIRE = %d, VALUE = %d, STATUS = %02X, COMM_ERROR = %d\r\n",
				
					REPEATERS[i][DETECTOR_P_FIRE],
					REPEATERS[i][DETECTOR_R_FIRE],
					REPEATERS[i][DETECTOR_VALUE],
					REPEATERS[i][DETECTOR_STATUS],
					
					REPEATERS[i][DETECTOR_COMM_ERROR]);
		
	}
	else
	{

		

		UARTprintf("Repeater %d...\r\n");
		UARTprintf("Command - %04X\r\nDisplay - %04X\r\nVirtual - %04X\r\nStatus1 - %04X\r\nStatus2 - %04X\r\nReal - %04X\r\nOld_Rep - %04X\r\nComm_Error - %d\r\n",
					REPEATERS[i][REPEATER_COMMAND],
					REPEATERS[i][DISPLAY_REPEATER_COMMAND],
					REPEATERS[i][VIRTUAL_REPEATER_COMMAND],
					REPEATERS[i][REPEATER_STATUS],
					
					REPEATERS[i][REPEATER_STATUS2],
					REPEATERS[i][REAL_REPEATER_STATUS],
					REPEATERS[i][OLD_REPEATER_STATUS],
					REPEATERS[i][REPEATER_COMM_ERROR]);
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


int cli_View_Detector(int argc, char *argv[])
{
	uint16_t i;


	if(argc != 2)
	{
		UARTprintf("Argument Require... Det_No(Memory Position)\r\n");
		return 1;
	}

	i = atoi(argv[1]);

	UARTprintf("Detector %d...\r\n");
	UARTprintf("PreFire = %d, R_Fire = %d, Value = %d, Status = %04X, CommError = %d, Flash_Pos = %d\r\n", 
				REPEATERS[i][DETECTOR_P_FIRE], 
				REPEATERS[i][DETECTOR_R_FIRE], 
				REPEATERS[i][DETECTOR_VALUE], 
				REPEATERS[i][DETECTOR_STATUS],
				REPEATERS[i][DETECTOR_COMM_ERROR],
				REPEATERS[i][DETECTOR_INFO_CELL_POS]);
	


//	for(i=1;i<NO_DETECTOR+1;i++)
	{
//		UARTprintf("DET %d : C - %04X   S - %04X\r\n", i, Detector_Command[i], Detector_Status[i]);
	}
	return 1;
}

int cli_co2(int argc, char *argv[])
{
	uint8_t i;

	for(i=0;i<10;i++)
	{
		UARTprintf("CO2 %d...\r\n", i);
//		UARTprintf("cross_in = %d, manual_in = %d\r\n", CO2_LIST[i].cross_in, CO2_LIST[i].manual_in);
//		UARTprintf("manual_del = %d, panel_del = %d\r\n", CO2_LIST[i].manual_delay, CO2_LIST[i].panel_delay);
	}
	return 1;
}

int cli_dong(int argc, char *argv[])
{

//	UARTprintf("FIRE=%d SEOLBI=%d DIS=%d COMM=%d GAM_COMM = %d\r\n", NO_FIRE, NO_SEOLBI, NO_DISCONN, NO_COMM, NO_GAM_COMM);
//	UARTprintf("M.BE=%d BUZZ=%d ACCU=%d GAM=%d\r\n", NO_MAINBELL, NO_BUZZER, NO_ACCUM, NO_GAM_FIRE);
//	UARTprintf("IN.BUZ = %d, PUMP_BUZ = %d\r\n", NO_INPUT_BUZZER, NO_PUMP_BUZZER);
	UARTprintf("REP Off = %d, DET Off = %d\r\n", R_STATUS.repeaterOffline, R_STATUS.detectorOffline);
	UARTprintf("REP Fire = %d, DET Fire = %d, DET PreFire = %d\r\n", R_STATUS.fireCount, R_STATUS.detFireCount, R_STATUS.detPreFireCount);
//	if(R_STATUS.batteryPowerError | R_STATUS.mainPowerError | R_STATUS.repeaterInputOpen | R_STATUS.repeaterOffline | 
//		R_STATUS.detectorOffline | R_STATUS.facilityCount | R_STATUS.inputBoardBuzzer | R_STATUS.inputBoardOpen | 
//		R_STATUS.ManualOutput | R_STATUS.pumpPsOutput)

	return 1;
}








int cli_all(int argc, char *argv[])
{
	uint16_t i;

#if 0
	for(i=1;i<NO_REPEATER + 1;i++)
	{
		Repeater_Command[i] = 0;
		Repeater_Status[i] = 0;

	}

	for(i=1;i<NO_DETECTOR + 1;i++)
	{
		Detector_Command[i] = 0;
		Detector_Status[i] = 0;

	}
#endif	

	return 1;

}

int cli_sys_reset(int argc, char *argv[])
{

	return 1;
}
#endif


