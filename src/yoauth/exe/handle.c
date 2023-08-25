#include "handle.h"
#include <stdio.h>
#include <termios.h>
#include <signal.h>
#include "yoauth/core/core.h"

#define YOAUTH_OUTPUT_TITLE1_BLANK \
	fprintf(stdout, "                            ")
#define YOAUTH_OUTPUT_SPLIT_LINE                               \
	fprintf(stdout, ""                                         \
					"----------------------------------------" \
					"----------------------------------------\n")

static bool yoauth_root_dir(char *buf, size_t bufsize)
{
	char exepath[MUGGLE_MAX_PATH];
	if (muggle_os_process_path(exepath, sizeof(exepath)) != 0) {
		YOAUTH_ERROR("failed found yoauth process path");
		return false;
	}

	char bindir[MUGGLE_MAX_PATH];
	if (muggle_path_dirname(exepath, bindir, sizeof(bindir)) != 0) {
		YOAUTH_ERROR("failed get bin dir");
		return false;
	}

	if (muggle_path_dirname(bindir, buf, bufsize) != 0) {
		YOAUTH_ERROR("failed get root dir");
		return false;
	}

	return true;
}

static bool yoauth_data_dir(char *buf, size_t bufsize)
{
	char dirpath[MUGGLE_MAX_PATH];
	if (!yoauth_root_dir(dirpath, sizeof(dirpath))) {
		YOAUTH_ERROR("failed get yoauth root dir");
		return false;
	}

	if (muggle_path_join(dirpath, "data", buf, bufsize) != 0) {
		YOAUTH_ERROR("failed get yoauth data dir");
		return false;
	}

	return true;
}

static bool yoauth_user_dir(const char *username, char *buf, size_t bufsize)
{
	char data_dir[MUGGLE_MAX_PATH];
	if (!yoauth_data_dir(data_dir, sizeof(data_dir))) {
		YOAUTH_ERROR("failed get data dir");
		return false;
	}

	if (muggle_path_join(data_dir, username, buf, bufsize) != 0) {
		YOAUTH_ERROR("failed join user dir");
		return false;
	}

	return true;
}

static bool yoauth_user_data_path(const char *username, char *buf,
								  size_t bufsize)
{
	char user_dir[MUGGLE_MAX_PATH];
	if (!yoauth_user_dir(username, user_dir, sizeof(user_dir))) {
		YOAUTH_ERROR("failed get user dir");
		return false;
	}

	if (muggle_path_join(user_dir, "totp.data", buf, bufsize) != 0) {
		YOAUTH_ERROR("failed join user data path");
		return false;
	}

	return true;
}

static void yoauth_password(char *buf, size_t bufsize)
{
	struct termios oldt, newt;
	int i = 0;
	int c = 0;

	// saving the old settings
	tcgetattr(STDIN_FILENO, &oldt);
	memcpy(&newt, &oldt, sizeof(newt));

	newt.c_lflag &= ~(ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	while ((c = getchar()) != '\n' && c != EOF && i < (int)bufsize - 1) {
		buf[i++] = c;
	}
	buf[i] = '\0';

	// restore old settings
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

static void yoauth_cleanup()
{
	fprintf(stdout, YOAUTH_ANSI_CLEAN_SCREEN);
	fprintf(stdout, YOAUTH_ANSI_ALTER_BUF_OFF);
	fprintf(stdout, YOAUTH_ANSI_CURSOR_ON);
	fflush(stdout);
}

static void yoauth_cleanup_die(int signo)
{
	MUGGLE_UNUSED(signo);
	exit(1);
}

static void yoauth_move_to(int16_t row, int16_t col)
{
	fprintf(stdout, YOAUTH_ANSI_ESC YOAUTH_ANSI_CSI "%u;%uH", row, col);
}

void yoauth_message_loop(const char *username)
{
	char user_dir[MUGGLE_MAX_PATH];
	if (!yoauth_user_dir(username, user_dir, sizeof(user_dir))) {
		YOAUTH_ERROR("failed get user dir");
		return;
	}

	if (!muggle_path_exists(user_dir)) {
		if (!yoauth_create_user(username)) {
			YOAUTH_ERROR("failed create user");
			return;
		}
	}

	fprintf(stdout, YOAUTH_ANSI_ALTER_BUF_ON);
	atexit(yoauth_cleanup);
	signal(SIGTERM, yoauth_cleanup_die);
	signal(SIGINT, yoauth_cleanup_die);

	fprintf(stdout, YOAUTH_ANSI_CLEAN_SCREEN);
	fprintf(stdout, YOAUTH_ANSI_CURSOR_OFF);

	while (true) {
		yoauth_move_to(0, 0);

		YOAUTH_STYLE_TIP;
		fprintf(stdout, "a: ");
		YOAUTH_STYLE_NORMAL;
		fprintf(stdout, "add account & secret key;    ");
		YOAUTH_STYLE_TIP;
		fprintf(stdout, "d: ");
		YOAUTH_STYLE_NORMAL;
		fprintf(stdout, "delete account & secret key;    ");
		YOAUTH_STYLE_TIP;
		fprintf(stdout, "q: ");
		YOAUTH_STYLE_NORMAL;
		fprintf(stdout, "exit");
		YOAUTH_OUTPUT("");
		YOAUTH_OUTPUT_SPLIT_LINE;

		// TODO:
		YOAUTH_OUTPUT_COMMAND_DESC("weisz", "342623");
		YOAUTH_OUTPUT_COMMAND_DESC("weisz", "342623");
	}
}

void yoauth_show_usage()
{
	YOAUTH_STYLE_TITLE;
	YOAUTH_OUTPUT_SPLIT_LINE;
	YOAUTH_OUTPUT_TITLE1_BLANK;
	YOAUTH_OUTPUT("YoAuth version " YOAUTH_VERSION);
	YOAUTH_OUTPUT_SPLIT_LINE;
	YOAUTH_STYLE_NORMAL;

	YOAUTH_OUTPUT("");
	YOAUTH_OUTPUT("Usage:")
	YOAUTH_OUTPUT("  yoauth [--options]")

	YOAUTH_OUTPUT("");
	YOAUTH_STYLE_TITLE;
	YOAUTH_OUTPUT_SPLIT_LINE;
	YOAUTH_OUTPUT_TITLE1_BLANK;
	YOAUTH_OUTPUT("Available commands");
	YOAUTH_OUTPUT_SPLIT_LINE;
	YOAUTH_STYLE_NORMAL;

	YOAUTH_OUTPUT_COMMAND_DESC("yoauth", "login with default user");
	YOAUTH_OUTPUT_COMMAND_DESC("yoauth -u <username>", "login with <username>");
	YOAUTH_OUTPUT_COMMAND_DESC("yoauth --user <username>",
							   "login with <username>");
}

bool yoauth_create_user(const char *username)
{
	YOAUTH_OUTPUT("user['%s'] first login, plz enter password", username);

	char passwd1[32];
	fprintf(stdout, "enter password: ");
	fflush(stdout);
	yoauth_password(passwd1, sizeof(passwd1));
	YOAUTH_OUTPUT("")

	char passwd2[32];
	fprintf(stdout, "enter password again: ");
	fflush(stdout);
	yoauth_password(passwd2, sizeof(passwd2));
	YOAUTH_OUTPUT("");

	if (strcmp(passwd1, passwd2) != 0) {
		YOAUTH_ERROR("The two entered passwords don't match");
		return false;
	}

	char filepath[MUGGLE_MAX_PATH];
	if (!yoauth_user_data_path(username, filepath, sizeof(filepath))) {
		YOAUTH_ERROR("failed get user data path");
		return false;
	}

	yoauth_head_t hdr;
	memset(&hdr, 0, sizeof(hdr));
	memcpy(hdr.magic, YOAUTH_DATA_HEAD_MAGIC, 4);
	hdr.ver = YOAUTH_DATA_VERSION;
	hdr.datalen = 0;
	hdr.datatype = YOAUTH_DATA_TYPE_TOTP;
	if (!yoauth_fileutils_store_totp(filepath, passwd1, (unsigned char *)&hdr,
									 sizeof(hdr))) {
		YOAUTH_ERROR("failed create user data file");
		return false;
	}

	return true;
}
