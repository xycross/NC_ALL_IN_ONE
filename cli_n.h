/**
 * @file
 *
 * simple command line interface for MourOS.
 *
 */

#ifndef MOUROS_CLI_H_
#define MOUROS_CLI_H_

#include <stdbool.h>
//#include <stdint.h>

//#include <mouros/buffer.h>


#define	ARRAY_SIZE(x)	( sizeof((x))/sizeof((x)[0]) )


#define MAX_CMD_LINE_LENGTH 50
#define MAX_CMDS 30
#define MAX_CMD_ARGS 10
#define MAX_ESC_SEQ_LENGTH 5
#define	MAX_HISTORY 5



#define DEF_DEBUG_UART_RX_POOL_SIZE		10


enum term_color {
	tBLACK = '0',
	tRED = '1',
	tGREEN = '2',
	tYELLOW = '3',
	tBLUE = '4',
	tMAGENTA = '5',
	tCYAN = '6',
	tWHITE = '7'
};


typedef int (*cli_cmd_main_function)(int, char *[]);

enum char_type {
	PRINTABLE,
	DELETE,
	BACKSPACE,
	ENTER,
	ESCAPE,
	UNKNOWN
};

struct command {
	const char *name;
	cli_cmd_main_function cmd_func;
};




//typedef int (*cli_cmd_main_function)(buffer_t *rx_buf, buffer_t *tx_buf, int argc, char *argv[]);




//void cli_init(buffer_t *rx_buf, buffer_t *tx_buf);

void cli_init(void);

void cli_loop(unsigned int sleep_ticks);

bool cli_register_command(const char *name, cli_cmd_main_function cmd_func);
bool cli_deregister_command(const char *name);
enum char_type get_char_type(unsigned char ch);
void set_term_color(enum term_color color);

unsigned char process_up_key(void);
unsigned char process_down_key(void);
void process_left_key(void);
void process_right_key(void);

void process_char_insert(char ch);
void process_char_delete(void);
void process_char_backspace(void);
void process_enter_pressed(void);
void process_escape_seq(void);

void output_prompt(unsigned char);

int execute_command(void);

void help_command(void);

unsigned int str_to_dec(char *);




//void Cli_Uart0ISR();



#endif /* MOUROS_CLI_H_ */
