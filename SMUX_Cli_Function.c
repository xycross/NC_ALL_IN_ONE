
#include <stdlib.h>
#include <string.h>



//#include "Repeater_Comm.h"

#include "cli_n.h"


#include "smux_main.h"


struct command SMUX_commands[MAX_CMDS] = 
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

unsigned char SMUX_num_commands = ARRAY_SIZE(SMUX_commands);


