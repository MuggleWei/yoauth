/******************************************************************************
 *  @file         handle.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-08-24
 *  @license      LGPL-3.0
 *  @brief        yoauth handle
 *****************************************************************************/

#ifndef YOAUTH_EXE_HANDLE_H_
#define YOAUTH_EXE_HANDLE_H_

#include "yoauth/core/macro.h"
#include "muggle/c/muggle_c.h"

EXTERN_C_BEGIN

#define YOAUTH_ANSI_ESC "\x1B"
#define YOAUTH_ANSI_CSI "["

#define YOAUTH_ANSI_RESET "0"
#define YOAUTH_ANSI_FG_COLOR_BLACK "30"
#define YOAUTH_ANSI_FG_COLOR_RED "31"
#define YOAUTH_ANSI_FG_COLOR_GREEN "32"
#define YOAUTH_ANSI_FG_COLOR_YELLOW "33"
#define YOAUTH_ANSI_FG_COLOR_BLUE "34"
#define YOAUTH_ANSI_FG_COLOR_MAGENTA "35"
#define YOAUTH_ANSI_FG_COLOR_CYAN "36"
#define YOAUTH_ANSI_FG_COLOR_WHITE "37"
#define YOAUTH_ANSI_ATTR_NORMAL "0"
#define YOAUTH_ANSI_ATTR_BOLD "1"
#define YOAUTH_ANSI_ATTR_DIM "2"

#define YOAUTH_STYLE_TIP                                                  \
	fprintf(stdout, YOAUTH_ANSI_ESC YOAUTH_ANSI_CSI YOAUTH_ANSI_ATTR_BOLD \
			";" YOAUTH_ANSI_FG_COLOR_GREEN "m")

#define YOAUTH_STYLE_TITLE                                               \
	fprintf(stdout, YOAUTH_ANSI_ESC YOAUTH_ANSI_CSI YOAUTH_ANSI_ATTR_DIM \
			";" YOAUTH_ANSI_FG_COLOR_WHITE "m")

#define YOAUTH_STYLE_ERROR                                                \
	fprintf(stdout, YOAUTH_ANSI_ESC YOAUTH_ANSI_CSI YOAUTH_ANSI_ATTR_BOLD \
			";" YOAUTH_ANSI_FG_COLOR_RED "m")

#define YOAUTH_STYLE_NORMAL \
	fprintf(stdout, YOAUTH_ANSI_ESC YOAUTH_ANSI_CSI YOAUTH_ANSI_RESET "m")

#define YOAUTH_OUTPUT(format, ...)                   \
	do {                                             \
		fprintf(stdout, format "\n", ##__VA_ARGS__); \
	} while (0);

#define YOAUTH_ERROR(format, ...)       \
	YOAUTH_STYLE_ERROR;                 \
	YOAUTH_OUTPUT(format, ##__VA_ARGS__); \
	YOAUTH_STYLE_NORMAL;

#define YOAUTH_OUTPUT_COMMAND_DESC(command, desc) \
	do {                                          \
		int len_command = (int)strlen(command);   \
		int num_blank = 42 - len_command;         \
		fprintf(stdout, "  ");                    \
		fprintf(stdout, command);                 \
		while (--num_blank > 0) {                 \
			fprintf(stdout, " ");                 \
		}                                         \
		fprintf(stdout, desc "\n");               \
	} while (0);

typedef struct yoauth_handle {
	char *user;
	const char *data_dir;
	uint32_t nargs;
	char *args[32];
} yoauth_handle_t;

/**
 * @brief show welcome information
 *
 * @return  boolean value
 */
bool yoauth_show_welcome_info();

/**
 * @brief message loop
 */
void yoauth_message_loop();

/**
 * @brief show Usage information
 */
void yoauth_show_usage();

/**
 * @brief parse user input into handle.args
 *
 * @param handle  yoauth handle pointer
 * @param input   user input
 */
void yoauth_parse_input(yoauth_handle_t *handle, char *input);

EXTERN_C_END

#endif // !YOAUTH_EXE_HANDLE_H_
