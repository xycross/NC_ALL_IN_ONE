#include "main.h"
#include "smux_main.h"

//#include "cli_n.h"

TS_RotarySwitchValue SW, OLD_SW;
extern TS_RotarySwitchValue rotarySwitch;

u8 op_mode;

int main(void) {	
	SystemInit();
//	SW = RotaySwitchDetect();

//	debugprintf("SW = %d\r\n", SW.val);
//	OLD_SW = SW;
//	setWatchDog(2000,SET);	


	if(op_mode == OP_SMUX)
	{
		smux_main();
	}
	else
	{
		main_WatchDog(0xffffffff,SET);
		while(1) 
		{		

	//		debugprintf("Hello\r\n");
	//		delayms(100);
	
			if(Get_Rotary_Switch_Process())
			{
				if(SW.RotarySwitchValue != rotarySwitch.RotarySwitchValue)
				{
					debugprintf("New Address = %d\r\n", rotarySwitch.RotarySwitchValue);
					*R_PMCTRLEN = PMCTRL_RSTCTRL_EN;
					*R_RSTCTRL = RSTCTRL_RESET;	
				}
			}
			main_WatchDog(10000, CLR);

		}
	}
	return 0;
}
