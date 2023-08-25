/******************************************************************************
 *  @file         tui.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-08-25
 *  @license      LGPL-3.0
 *  @brief        yoauth tui
 *****************************************************************************/

#ifndef YOAUTH_EXE_TUI_H_
#define YOAUTH_EXE_TUI_H_

#include "yoauth/core/macro.h"
#include "muggle/c/muggle_c.h"
#if MUGGLE_PLATFORM_WINDOWS
	// TODO:
#else
	#include <termios.h>
#endif

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

#define YOAUTH_STYLE_NORMAL \
	fprintf(stdout, YOAUTH_ANSI_ESC YOAUTH_ANSI_CSI YOAUTH_ANSI_RESET "m")

#define YOAUTH_STYLE_TIP                                                  \
	fprintf(stdout, YOAUTH_ANSI_ESC YOAUTH_ANSI_CSI YOAUTH_ANSI_ATTR_BOLD \
			";" YOAUTH_ANSI_FG_COLOR_GREEN "m")

#define YOAUTH_STYLE_ERROR                                                \
	fprintf(stdout, YOAUTH_ANSI_ESC YOAUTH_ANSI_CSI YOAUTH_ANSI_ATTR_BOLD \
			";" YOAUTH_ANSI_FG_COLOR_RED "m")

#define YOAUTH_STYLE_TITLE                                               \
	fprintf(stdout, YOAUTH_ANSI_ESC YOAUTH_ANSI_CSI YOAUTH_ANSI_ATTR_DIM \
			";" YOAUTH_ANSI_FG_COLOR_WHITE "m")

#define YOAUTH_OUTPUT(format, ...)                   \
	do {                                             \
		fprintf(stdout, format "\n", ##__VA_ARGS__); \
	} while (0);

#define YOAUTH_ERROR(format, ...)         \
	YOAUTH_STYLE_ERROR;                   \
	YOAUTH_OUTPUT(format, ##__VA_ARGS__); \
	YOAUTH_STYLE_NORMAL;

#define YOAUTH_OUTPUT_KV(k, v, width)  \
	do {                               \
		int len_k = (int)strlen(k);    \
		int num_blank = width - len_k; \
		fprintf(stdout, "  ");         \
		fprintf(stdout, "%s", k);      \
		while (--num_blank > 0) {      \
			fprintf(stdout, " ");      \
		}                              \
		fprintf(stdout, "%s", v);      \
		fprintf(stdout, "\n");         \
	} while (0)

#define YOAUTH_OUTPUT_COMMAND_DESC(k, v) YOAUTH_OUTPUT_KV(k, v, 42)

#define YOAUTH_OUTPUT_TITLE(s)            \
	do {                                  \
		int len_s = (int)strlen(s);       \
		int num_blank = (80 - len_s) / 2; \
		while (--num_blank > 0) {         \
			fprintf(stdout, " ");         \
		}                                 \
		fprintf(stdout, s "\n");          \
	} while (0)

#define YOAUTH_OUTPUT_SPLIT_LINE                               \
	fprintf(stdout, ""                                         \
					"----------------------------------------" \
					"----------------------------------------\n")

#define YOAUTH_ANSI_ALTER_BUF_ON YOAUTH_ANSI_ESC YOAUTH_ANSI_CSI "?1049h"
#define YOAUTH_ANSI_ALTER_BUF_OFF YOAUTH_ANSI_ESC YOAUTH_ANSI_CSI "?1049l"
#define YOAUTH_ANSI_CURSOR_ON YOAUTH_ANSI_ESC YOAUTH_ANSI_CSI "?25h"
#define YOAUTH_ANSI_CURSOR_OFF YOAUTH_ANSI_ESC YOAUTH_ANSI_CSI "?25l"
#define YOAUTH_ANSI_CLEAN_SCREEN YOAUTH_ANSI_ESC YOAUTH_ANSI_CSI "2J"

typedef struct yoauth_tui {
#if MUGGLE_PLATFORM_WINDOWS
	DWORD old_mode;
	DWORD cur_mode;
#else
	struct termios old_term;
	struct termios cur_term;
#endif
} yoauth_tui_t;

void yoauth_tui_setup();
#if MUGGLE_PLATFORM_WINDOWS
void yoauth_tui_cleanup(void);
#else
void yoauth_tui_cleanup();
#endif
void yoauth_tui_cleanup_die(int signo);

void yoauth_tui_move_to(int16_t row, int16_t col);

void yoauth_tui_passwd(const char *tip, char *buf, size_t bufsize);

void yoauth_tui_store(yoauth_tui_t *tui);
void yoauth_tui_recover(yoauth_tui_t *tui);

void yoauth_tui_close_echo(yoauth_tui_t *tui);

EXTERN_C_END

#endif // !YOAUTH_EXE_TUI_H_
