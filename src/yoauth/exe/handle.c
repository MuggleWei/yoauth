#include "handle.h"
#include <stdio.h>
#include "yoauth/exe/command_user.h"

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

static bool yoauth_data_dir(char *buf, size_t bufsize) {
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

bool yoauth_show_welcome_info()
{
	char dirpath[MUGGLE_MAX_PATH];
	if (!yoauth_root_dir(dirpath, sizeof(dirpath))) {
		return false;
	}

	YOAUTH_OUTPUT("YoAuth root dir: %s", dirpath);
	YOAUTH_OUTPUT("YoAuth version: " YOAUTH_VERSION);
	YOAUTH_OUTPUT("enter \"h\" or \"help\" to get help");

	return true;
}

void yoauth_message_loop()
{
	static const char *null_user = "<no user login>";
	char data_dir[MUGGLE_MAX_PATH];
	if (!yoauth_data_dir(data_dir, sizeof(data_dir))) {
		YOAUTH_ERROR("failed get data dir");
		return;
	}

	yoauth_handle_t handle;
	memset(&handle, 0, sizeof(handle));
	handle.user = (char*)malloc(64);
	strncpy(handle.user, null_user, strlen(null_user));
	handle.user[strlen(null_user)] = '\0';
	handle.data_dir = data_dir;

	// message loop
	char buf[1024];
	do {
		YOAUTH_STYLE_TIP;
		fprintf(stdout, "[yoauth-%s]# ", handle.user);
		YOAUTH_STYLE_NORMAL;

		fgets(buf, sizeof(buf), stdin);

		yoauth_parse_input(&handle, buf);

		if (handle.nargs == 0) {
			continue;
		}

		const char *command = handle.args[0];
		if (strcmp(command, "user") == 0) {
			yoauth_cmd_user(&handle);
		} else if (strcmp(command, "totp") == 0) {
			// TODO:
		} else if (strcmp(command, "quit") == 0 || strcmp(command, "q") == 0) {
			break;
		} else if (strcmp(command, "help") == 0 || strcmp(command, "h") == 0) {
			yoauth_show_usage();
		} else {
			YOAUTH_ERROR("unrecognized command: %s", command);
		}
	} while (true);

	// clean up
	if (handle.user) {
		free(handle.user);
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
	YOAUTH_OUTPUT("  <command> [--options]")

	YOAUTH_OUTPUT("");
	YOAUTH_OUTPUT("Commands:");
	YOAUTH_OUTPUT("  user        operations on users");
	YOAUTH_OUTPUT("  totp        TOTP show/add/delete");

	YOAUTH_OUTPUT("");
	YOAUTH_STYLE_TITLE;
	YOAUTH_OUTPUT_SPLIT_LINE;
	YOAUTH_OUTPUT_TITLE1_BLANK;
	YOAUTH_OUTPUT("Available commands");
	YOAUTH_OUTPUT_SPLIT_LINE;
	YOAUTH_STYLE_NORMAL;

	yoauth_cmd_user_show_usage();

	YOAUTH_OUTPUT("TOTP Commands:");
	YOAUTH_OUTPUT_COMMAND_DESC("totp list",
							   "show all TOTP code of current user");
	YOAUTH_OUTPUT_COMMAND_DESC("totp add <account> <secret key>",
							   "add new { <account>: <secret key> }");
	YOAUTH_OUTPUT_COMMAND_DESC("totp del <account>", "delete TOTP account");
	YOAUTH_OUTPUT_COMMAND_DESC("totp show <account>",
							   "show TOTP code of account");
}

void yoauth_parse_input(yoauth_handle_t *handle, char *input)
{
	handle->nargs = 0;

	char *p = input;
	char *q = input;
	while (*p != '\0') {
		if (*p == '\t' || *p == ' ' || *p == '\n') {
			if (p == q) {
				p++;
				q++;
			} else {
				*p = '\0';
				handle->args[handle->nargs++] = q;
				if (handle->nargs >=
					sizeof(handle->args) / sizeof(handle->args[0])) {
					break;
				}
				p++;
				q = p;
			}
		} else {
			p++;
		}
	}

	if (p != q &&
		handle->nargs < sizeof(handle->args) / sizeof(handle->args[0])) {
		handle->args[handle->nargs++] = q;
	}
}
