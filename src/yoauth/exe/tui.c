#include "tui.h"
#include <signal.h>
#include <conio.h>

void yoauth_tui_setup()
{
	fprintf(stdout, YOAUTH_ANSI_ALTER_BUF_ON);
	fflush(stdout);

	atexit(yoauth_tui_cleanup);
	signal(SIGTERM, yoauth_tui_cleanup_die);
	signal(SIGINT, yoauth_tui_cleanup_die);

	fprintf(stdout, YOAUTH_ANSI_CLEAN_SCREEN);
	fprintf(stdout, YOAUTH_ANSI_CURSOR_OFF);
}
#if MUGGLE_PLATFORM_WINDOWS
void yoauth_tui_cleanup(void)
#else
void yoauth_tui_cleanup()
#endif
{
	fprintf(stdout, YOAUTH_ANSI_CLEAN_SCREEN);
	fprintf(stdout, YOAUTH_ANSI_ALTER_BUF_OFF);
	fprintf(stdout, YOAUTH_ANSI_CURSOR_ON);
	fflush(stdout);
}
void yoauth_tui_cleanup_die(int signo)
{
	MUGGLE_UNUSED(signo);
	exit(1);
}

void yoauth_tui_move_to(int16_t row, int16_t col)
{
	fprintf(stdout, YOAUTH_ANSI_ESC YOAUTH_ANSI_CSI "%u;%uH", row, col);
}

static int yoauth_win_getch() {
#if MUGGLE_PLATFORM_WINDOWS
	HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
	if (h == NULL) {
		YOAUTH_ERROR("console not found");
		return 0;
	}
	TCHAR c = 0;
	DWORD cc = 0;
	ReadConsole( h, &c, 1, &cc, NULL );
	return (int)c;
#else
	return getchar();
#endif
}

void yoauth_tui_passwd(const char *tip, char *buf, size_t bufsize)
{
	fprintf(stdout, "%s", tip);
	fflush(stdout);

	yoauth_tui_t tui;
	memset(&tui, 0, sizeof(tui));
	yoauth_tui_store(&tui);

	yoauth_tui_close_echo(&tui);

	int i = 0;
	int c = 0;
#if MUGGLE_PLATFORM_WINDOWS
	char newline = '\r';
#else
	char newline = '\n';
#endif
	while ((c = yoauth_win_getch()) != newline && c != EOF && i < (int)bufsize - 1) {
		// YOAUTH_OUTPUT("%c(%d)", (char)c, c);
		if (c == 8 && i != 0) {
			i--;
			continue;
		}
		buf[i++] = c;
	}
	buf[i] = '\0';

	yoauth_tui_recover(&tui);
}

void yoauth_tui_store(yoauth_tui_t *tui)
{
#if MUGGLE_PLATFORM_WINDOWS
	HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
	if (h == NULL) {
		YOAUTH_ERROR("console not found");
		return;
	}
	GetConsoleMode(h, &tui->old_mode);
	tui->cur_mode = tui->old_mode;
#else
	tcgetattr(STDIN_FILENO, &tui->old_term);
	memcpy(&tui->cur_term, &tui->old_term, sizeof(tui->cur_term));
#endif
}
void yoauth_tui_recover(yoauth_tui_t *tui)
{
#if MUGGLE_PLATFORM_WINDOWS
	HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
	if (h == NULL) {
		YOAUTH_ERROR("console not found");
		return;
	}
	SetConsoleMode(h, tui->old_mode);
#else
	tcsetattr(STDIN_FILENO, TCSANOW, &tui->old_term);
#endif
}

void yoauth_tui_close_echo(yoauth_tui_t *tui)
{
#if MUGGLE_PLATFORM_WINDOWS
	HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
	if (h == NULL) {
		YOAUTH_ERROR("console not found");
		return;
	}
	tui->cur_mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
	SetConsoleMode( h, tui->cur_mode);
#else
	tui->cur_term.c_lflag &= ~(ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &tui->cur_term);
#endif
}
