/******************************************************************************
 *  @file         style.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-09-03
 *  @license      LGPL-3.0
 *  @brief        yoauth style
 *****************************************************************************/

#ifndef YOAUTH_EXE_STYLE_H_
#define YOAUTH_EXE_STYLE_H_

#include "yoauth/core/macro.h"
#include "muggle/c/muggle_c.h"

EXTERN_C_BEGIN

#define YOAUTH_OUTPUT_SPLIT_LINE                               \
	fprintf(stdout, ""                                         \
					"----------------------------------------" \
					"----------------------------------------\n")

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

#define YOAUTH_OUTPUT_COMMAND_DESC(k, v) YOAUTH_OUTPUT_KV(k, v, 20)

#define YOAUTH_OUTPUT(format, ...)                   \
	do {                                             \
		fprintf(stdout, format "\n", ##__VA_ARGS__); \
	} while (0);

#if MUGGLE_PLATFORM_WINDOWS
	#define YOAUTH_WINDOWS_LOAD_STYLE                                 \
		const HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE); \
		CONSOLE_SCREEN_BUFFER_INFO sb_info;                           \
		GetConsoleScreenBufferInfo(stdout_handle, &sb_info);          \
		const WORD old_sb_attrs = sb_info.wAttributes;

	#define YOAUTH_WINDOWS_RESET_STYLE \
		SetConsoleTextAttribute(stdout_handle, old_sb_attrs);

	#define YOAUTH_STYLE_ERROR                 \
		SetConsoleTextAttribute(stdout_handle, \
								FOREGROUND_RED | FOREGROUND_INTENSITY);

	#define YOAUTH_STYLE_TIP                   \
		SetConsoleTextAttribute(stdout_handle, \
								FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	#define YOAUTH_STYLE_TITLE \
		SetConsoleTextAttribute(stdout_handle, FOREGROUND_INTENSITY);

	#define YOAUTH_ERROR(format, ...)             \
		do {                                      \
			YOAUTH_WINDOWS_LOAD_STYLE;            \
			YOAUTH_STYLE_ERROR;                   \
			YOAUTH_OUTPUT(format, ##__VA_ARGS__); \
			YOAUTH_WINDOWS_RESET_STYLE;           \
		} while (0)

	#define YOAUTH_TIP(format, ...)               \
		do {                                      \
			YOAUTH_WINDOWS_LOAD_STYLE;            \
			YOAUTH_STYLE_TIP;                     \
			YOAUTH_OUTPUT(format, ##__VA_ARGS__); \
			YOAUTH_WINDOWS_RESET_STYLE;           \
		} while (0)

	#define YOAUTH_OUTPUT_TITLE(s)            \
		do {                                  \
			YOAUTH_WINDOWS_LOAD_STYLE;        \
			YOAUTH_STYLE_TITLE;               \
			YOAUTH_OUTPUT_SPLIT_LINE;         \
			int len_s = (int)strlen(s);       \
			int num_blank = (80 - len_s) / 2; \
			while (--num_blank > 0) {         \
				fprintf(stdout, " ");         \
			}                                 \
			fprintf(stdout, s "\n");          \
			YOAUTH_OUTPUT_SPLIT_LINE;         \
			YOAUTH_WINDOWS_RESET_STYLE;       \
		} while (0)
#else
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

	#define YOAUTH_ERROR(format, ...)         \
		YOAUTH_STYLE_ERROR;                   \
		YOAUTH_OUTPUT(format, ##__VA_ARGS__); \
		YOAUTH_STYLE_NORMAL;

	#define YOAUTH_TIP(format, ...)           \
		YOAUTH_STYLE_TIP;                     \
		YOAUTH_OUTPUT(format, ##__VA_ARGS__); \
		YOAUTH_STYLE_NORMAL;

	#define YOAUTH_OUTPUT_TITLE(s)            \
		do {                                  \
			YOAUTH_STYLE_TITLE;               \
			YOAUTH_OUTPUT_SPLIT_LINE;         \
			int len_s = (int)strlen(s);       \
			int num_blank = (80 - len_s) / 2; \
			while (--num_blank > 0) {         \
				fprintf(stdout, " ");         \
			}                                 \
			fprintf(stdout, "%s\n", s);       \
			YOAUTH_OUTPUT_SPLIT_LINE;         \
			YOAUTH_STYLE_NORMAL;              \
		} while (0)
#endif

EXTERN_C_END

#endif // !YOAUTH_EXE_STYLE_H_
