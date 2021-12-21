/*
 * cli.c
 *
 *  Created on: 7. 9. 2014
 *      Author: ondra
 */

 #include <stdio.h>
 #include <string.h>
#include <stdlib.h>

//#include "main.h"



#include "cli_n.h" // Public function declarations.

//#include "Definition.h"

//int (*cli_cmd_main_function)(int, char *[]);



static char cmd_line[MAX_CMD_LINE_LENGTH + 1]; // Extra space for terminating '\0'
static char history_cmd_line[5][MAX_CMD_LINE_LENGTH + 1];
static unsigned char cmd_line_len = 0;
static unsigned char cursor_pos = 0;
static char prompt_cursor_pos;

//struct command commands[MAX_CMDS] = 
extern struct command SMUX_commands[MAX_CMDS];

//static uint8_t num_commands = 2;
extern unsigned char SMUX_num_commands;


extern struct command TEST_CLI_commands[MAX_CMDS];

//static uint8_t num_commands = 2;
extern unsigned char TEST_CLI_num_commands;


unsigned int sub_tick;

//static buffer_t *rx = NULL;
//static buffer_t *tx = NULL;



//extern uint8_t DEBUG_RX_DATA, DEBUG_RX_FLAG;
//extern uint32_t sub_tick;


//extern uint8_t DEBUG_RX_POOL[DEF_DEBUG_UART_RX_POOL_SIZE];
//extern uint8_t DEBUG_RX_SAVE;

unsigned char DEBUG_RX_POOL[DEF_DEBUG_UART_RX_POOL_SIZE];
unsigned char DEBUG_RX_SAVE, DEBUG_RX_PROCESS;


unsigned char HISTORY_SAVE, HISTORY_CURRENT;

unsigned char cli_which;


#if 1
void cli_init(void)
{
	unsigned char i;
	
//	HISTORY_SAVE = MAX_HISTORY - 1;
//	HISTORY_CURRENT = MAX_HISTORY - 1;

	for(i=0;i<MAX_HISTORY;i++)
	{
		history_cmd_line[i][0] = NULL;
	}
}
#endif

void cli_loop(unsigned int sleep_ticks)
{
	static unsigned char key_escape = 0;
	static unsigned char first_key = 0;
	set_term_color(tBLUE);

//	output_prompt(0);
	while (true) 
	{
		unsigned char ch;
//		if (!os_buffer_read_ch(rx, (uint8_t *) &ch)) {
//			os_task_sleep(sleep_ticks);
//			continue;
//		}

		if(key_escape)
		{
			if(sub_tick >= 10)
			{
				key_escape = 0;
				first_key = 0;
				process_escape_seq();

			}
		}

		if(DEBUG_RX_SAVE != DEBUG_RX_PROCESS)
		{
			ch = DEBUG_RX_POOL[DEBUG_RX_PROCESS];
			if(++DEBUG_RX_PROCESS >= DEF_DEBUG_UART_RX_POOL_SIZE)
			{
				DEBUG_RX_PROCESS = 0;
			}
			
		}
		else
		{
//			continue;
			return;
		}

		if(key_escape)
		{
			if(first_key)
			{
				first_key = 0;
				key_escape = 0;
				switch(ch)
				{
					case 0x41:
						if(process_up_key())
						{
							output_prompt(1);
						}
						break;

					case 0x42:
						if(process_down_key())
						{
							output_prompt(1);
						}
						break;

					case 0x43:
						process_right_key();
						break;

					case 0x44:
						process_left_key();
						break;

					default:
						break;
				}
			}
			else
			{
				if(ch == 0x5B)
				{
					first_key = 1;
				}
			}

		}
		else
		{
			
			switch (get_char_type(ch)) 
			{
				case PRINTABLE:
					process_char_insert(ch);
					output_prompt(0);
				
					break;
				

				case DELETE:
					process_char_delete();
					output_prompt(1);
				
					break;
				case BACKSPACE:
					process_char_backspace();
					output_prompt(1);
				
					break;
				case ENTER:
					process_enter_pressed();
					break;

				case ESCAPE:
//					process_escape_seq();
					sub_tick = 0;
					key_escape = 1;
				
					break;
				default:
					break;
			}
		}


//		output_prompt();
	}
}


#if 0
bool cli_register_command(const char *name, cli_cmd_main_function cmd_func)
{
	if (num_commands >= MAX_CMDS) {
		return false;
	}

	commands[num_commands].name = name;
	commands[num_commands].cmd_func = cmd_func;

	num_commands++;

	return true;
}

bool cli_deregister_command(const char *name)
{
	unsigned char i, j;
	for (i = 0; i < num_commands; i++) 
	{
		if (strcmp(commands[i].name, name) == 0) 
		{
			for (j = i; j < num_commands - 1; j++) 
			{
				commands[j] = commands[j + 1];
			}

			num_commands--;

			return true;
		}
	}

	return false;
}

#endif


// ----------------------------------------------------------
// CLI I/O
// ----------------------------------------------------------
static enum char_type get_char_type(unsigned char ch)
{
	if ((ch >= 32 && ch <= 126) || ch == '\t')
	{
		return PRINTABLE;
	}
	else if (ch == 127 || ch == '\004')
	{
		return DELETE;
	} 
	else if (ch == 8) 
	{
		return BACKSPACE;
	}
	else if (ch == '\n' || ch == '\r') 
	{
		return ENTER;
	}
	else if (ch == 0x1B) 
	{
		return ESCAPE;
	}
	else 
	{
		debugprintf("\r\nHex : %02X\r\n", ch);
		return UNKNOWN;
	}
}

static void set_term_color(enum term_color color)
{
//	os_buffer_write_str(tx, "\e[3");
//	os_buffer_write_byte(tx, color);
//	os_buffer_write_byte(tx, 'm');
}

static unsigned char process_up_key(void)
{
	unsigned char length;


	prompt_cursor_pos = 0;

/*	
	if(history_cmd_line[HISTORY_CURRENT][0] == NULL) 
	{
		if(HISTORY_CURRENT == 0)
		{
			return 0;
		}
		else
		{
			if(++HISTORY_CURRENT >= MAX_HISTORY)
			{
				HISTORY_CURRENT = 0;
			}
		}

	
	}
*/		
//	printf("\r\n SAVE %d CURR %d\r\n", HISTORY_SAVE, HISTORY_CURRENT);

	length = strlen(history_cmd_line[HISTORY_CURRENT]);
	memcpy(cmd_line, history_cmd_line[HISTORY_CURRENT], length);
	cursor_pos = length;
	cmd_line_len = length;

	if(HISTORY_CURRENT == 0)
	{
		HISTORY_CURRENT = MAX_HISTORY - 1;
	}
	else
	{
		HISTORY_CURRENT--;
	}


	return 1;
	
}


static unsigned char process_down_key(void)
{
	unsigned char length;

	prompt_cursor_pos = 0;

/*
	if(history_cmd_line[HISTORY_CURRENT][0] == NULL)
	{
		if(HISTORY_OVERFLOW)
		{
		}
		else
		{
		}
		if(HISTORY_CURRENT == 0)
		{
			return 0;
		}
		else
		{
			if(HISTORY_CURRENT == 0)
			{
				HISTORY_CURRENT = MAX_HISTORY - 1;
			}
			else
			{
				HISTORY_CURRENT--;
			}
		}
	}
*/
//	printf("\r\n SAVE %d CURR %d\r\n", HISTORY_SAVE, HISTORY_CURRENT);	

	length = strlen(history_cmd_line[HISTORY_CURRENT]);
	memcpy(cmd_line, history_cmd_line[HISTORY_CURRENT], length);
	cursor_pos = length;
	cmd_line_len = length;

	if(++HISTORY_CURRENT >= MAX_HISTORY)
	{
		HISTORY_CURRENT = 0;
	}
	
	return 1;
}

static void process_left_key(void)
{

	if (cursor_pos == 0) {
		return;
	}
//	printf("\033[%dD", cursor_pos);
//	sprintf(test_buf, "\033[1D", cursor_pos);
//	printf(test_buf);
	debugprintf("\033[1D");
	prompt_cursor_pos--;

	cursor_pos--;

//	sprintf(test_buf, "\1B%dD", cursor_pos);
}

static void process_right_key(void)
{
	if (cursor_pos >= cmd_line_len) 
	{
		return;
	}

	debugprintf("\033[1C");
	prompt_cursor_pos++;

	cursor_pos++;
}

static void process_char_insert(char ch)
{
	unsigned char i;
	if (cmd_line_len >= MAX_CMD_LINE_LENGTH) 
	{
		return;
	}

//	for (uint8_t i = cursor_pos; i < cmd_line_len; i++)
//	{
//		cmd_line[i + 1] = cmd_line[i];
//	}
	i = cmd_line_len;

	if(cursor_pos != cmd_line_len)
	{
		while(i)
		{
			cmd_line[i] = cmd_line[i-1];
			i--;
			if(i == cursor_pos) break;
		}
	}
	cmd_line[cursor_pos] = ch;

	cursor_pos++;
	cmd_line_len++;
	
}

static void process_char_delete(void)
{
	unsigned char i;
	
	if (cursor_pos == cmd_line_len) {
		return;
	}

	for (i = cursor_pos; i < cmd_line_len - 1; i++)
	{
		cmd_line[i] = cmd_line[i + 1];
	}

	cmd_line_len--;
	if(prompt_cursor_pos < 0)
	{
		prompt_cursor_pos++;
	}
	debugprintf("\033[2K");	
}

static void process_char_backspace(void)
{
	unsigned char i;
	if (cursor_pos == 0) 
	{
		return;
	}

	for (i = cursor_pos; i < cmd_line_len; i++) 
	{
		cmd_line[i - 1] = cmd_line[i];
	}

	cursor_pos--;
	cmd_line_len--;
	
//	printf("\b");
//	printf(" ");
//	printf("\b");
}

static void process_enter_pressed(void)
{
//	os_buffer_write_str(tx, "\r\n");
//	printf("\r\n");
	debugprintf("\r\n");

	set_term_color(tYELLOW);

	execute_command();
	debugprintf("\r\n");

	set_term_color(tBLUE);

	cmd_line_len = 0;
	cursor_pos = 0;
	prompt_cursor_pos = 0;
	DEBUG_RX_PROCESS = DEBUG_RX_SAVE;
	output_prompt(0);
}

static void process_escape_seq(void)
{
	cursor_pos = 0;
	cmd_line_len = 0;
	debugprintf("\033[2K");
	debugprintf("\r>>");
}

static void output_prompt(unsigned char opt_clr)
{
//	os_buffer_write_str(tx, "\e[2K"); // clears the current line
//	os_buffer_write_str(tx, "\r -> ");

	if(opt_clr)
	{
		debugprintf("\033[2K");
	}


//	printf("\e[2K");
	debugprintf("\r>>");


//	os_buffer_write(tx, (uint8_t *) cmd_line, cmd_line_len);
	cmd_line[cmd_line_len] = '\0';	
	debugprintf("%s", cmd_line);

	if(prompt_cursor_pos != 0)
	{
		if(prompt_cursor_pos < 0)
		{
			
			debugprintf("\033[%dD", -prompt_cursor_pos);
			
		}
	}

//	int shift = cmd_line_len - cursor_pos;

#if 0
	if (shift > 0) 
	{
		char shift_buf[12];
		int buf_len = sniprintf(shift_buf, 12, "\e[%dD", shift);

		if (buf_len > 0 && buf_len < 12) 
		{
//			os_buffer_write(tx, (uint8_t *) shift_buf, (uint32_t) buf_len);
			printf("%s", shift_buf);
		}
	}
#endif	
}


static int execute_command(void)
{
	int argc = 1;
	char *argv[MAX_CMD_ARGS];
	char *saveptr;
	char buffer_cmd_line[MAX_CMD_LINE_LENGTH + 1]; 

	unsigned char i;

	cmd_line[cmd_line_len] = '\0';

	memcpy(buffer_cmd_line, cmd_line, cmd_line_len + 1);


	argv[0] = strtok_r(cmd_line, " \t", &saveptr);

	if (argv[0] != NULL)
	{
		if(cli_which == DEF_CLI_WHICH_SMUX)
		{
			for (i = 0; i < SMUX_num_commands; i++) 
			{
				if (strcmp(argv[0], SMUX_commands[i].name) == 0) 
				{

					char *token = strtok_r(NULL, " \t", &saveptr);

					while(token != NULL && argc != MAX_CMD_ARGS) {
						argv[argc++] = token;
						token = strtok_r(NULL, " \t", &saveptr);
					}

	//				return commands[i].cmd_func(rx, tx, argc, argv);
					HISTORY_CURRENT = HISTORY_SAVE;
					memcpy(history_cmd_line[HISTORY_SAVE], buffer_cmd_line, cmd_line_len + 1);
					
					if(++HISTORY_SAVE >= MAX_HISTORY)
					{
						HISTORY_SAVE = 0;
					}
					
					return SMUX_commands[i].cmd_func(argc, argv);

				}
			}

			if (strcmp(argv[0], "help") == 0) {
				help_command();
				return -1;
			}

			debugprintf("Command not found.\r\n");
			return -1;
		}
		
		else if(cli_which == DEF_CLI_WHICH_COMM_MON_CLI)
		{
			for (i = 0; i < TEST_CLI_num_commands; i++) 
			{
				if (strcmp(argv[0], TEST_CLI_commands[i].name) == 0) 
				{
	
					char *token = strtok_r(NULL, " \t", &saveptr);
	
					while(token != NULL && argc != MAX_CMD_ARGS) {
						argv[argc++] = token;
						token = strtok_r(NULL, " \t", &saveptr);
					}
	
	//				return commands[i].cmd_func(rx, tx, argc, argv);
					HISTORY_CURRENT = HISTORY_SAVE;
					memcpy(history_cmd_line[HISTORY_SAVE], buffer_cmd_line, cmd_line_len + 1);
					
					if(++HISTORY_SAVE >= MAX_HISTORY)
					{
						HISTORY_SAVE = 0;
					}
					
					return TEST_CLI_commands[i].cmd_func(argc, argv);
	
				}
			}
	
			if (strcmp(argv[0], "help") == 0) {
				help_command();
				return -1;
			}
	
			debugprintf("Command not found.\r\n");
			return -1;
		}
	}

//	os_buffer_write_str(tx, "Command not found.\r\n");
//	printf("Command not found.\r\n");


	return -1;
}


static void help_command(void)
{
	unsigned char i;
	debugprintf("The currently registered commands are:\r\n");

	if(cli_which == DEF_CLI_WHICH_SMUX)
	{
		for (i = 0; i < SMUX_num_commands; i++) 
		{
			debugprintf("\t %s", SMUX_commands[i].name);
			debugprintf("\r\n");
		}
	}
	else if(cli_which == DEF_CLI_WHICH_COMM_MON_CLI)
	{
		for (i = 0; i < TEST_CLI_num_commands; i++) 
		{
			debugprintf("\t %s", TEST_CLI_commands[i].name);
			debugprintf("\r\n");
		}
	}

	debugprintf("\r\n");
}


unsigned int str_to_dec(char *argv)
{
	if((strncmp(argv, "0x", 2) == 0) || (strncmp(argv, "0X", 2) == 0))
	{
		return strtol(argv+2, NULL, 16);
	}
	else
	{
		return strtol(argv, NULL, 10);
	}
}





