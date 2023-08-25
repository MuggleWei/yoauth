#include "tui.h"
#include <signal.h>

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
void yoauth_tui_cleanup()
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
	while ((c = getchar()) != '\n' && c != EOF && i < (int)bufsize - 1) {
		buf[i++] = c;
	}
	buf[i] = '\0';

	yoauth_tui_recover(&tui);
}

void yoauth_tui_store(yoauth_tui_t *tui)
{
	tcgetattr(STDIN_FILENO, &tui->old_term);
	memcpy(&tui->cur_term, &tui->old_term, sizeof(tui->cur_term));
}
void yoauth_tui_recover(yoauth_tui_t *tui)
{
	tcsetattr(STDIN_FILENO, TCSANOW, &tui->old_term);
}

void yoauth_tui_close_echo(yoauth_tui_t *tui)
{
	tui->cur_term.c_lflag &= ~(ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &tui->cur_term);
}
