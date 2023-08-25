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

#define YOAUTH_ERROR(format, ...)         \
	YOAUTH_STYLE_ERROR;                   \
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

#define YOAUTH_ANSI_ALTER_BUF_ON YOAUTH_ANSI_ESC YOAUTH_ANSI_CSI "?1049h"
#define YOAUTH_ANSI_ALTER_BUF_OFF YOAUTH_ANSI_ESC YOAUTH_ANSI_CSI "?1049l"
#define YOAUTH_ANSI_CURSOR_ON YOAUTH_ANSI_ESC YOAUTH_ANSI_CSI "?25h"
#define YOAUTH_ANSI_CURSOR_OFF YOAUTH_ANSI_ESC YOAUTH_ANSI_CSI "?25l"
#define YOAUTH_ANSI_CLEAN_SCREEN YOAUTH_ANSI_ESC YOAUTH_ANSI_CSI "2J"

typedef struct yoauth_handle {
	const char *user;
	const char *user_dir;
	uint32_t nargs;
	char *args[32];
} yoauth_handle_t;

/**
 * @brief message loop
 *
 * @param username  user name
 */
void yoauth_message_loop(const char *username);

/**
 * @brief show Usage information
 */
void yoauth_show_usage();

/**
 * @brief create new user
 *
 * @param username  user name
 *
 * @return boolean value
 */
bool yoauth_create_user(const char *username);

EXTERN_C_END

#endif // !YOAUTH_EXE_HANDLE_H_
