#include "style.h"
#include "command_list.h"
#include "command_add.h"
#include "command_del.h"
#include "command_dump.h"

#define STR_USAGE                                      \
	"Commands:\n"                                      \
	"  list          list TOTP codes\n"                \
	"  add           add new account\n"                \
	"  delete        delete account\n"                 \
	"  dump          dump account and key to file\n"   \
	"  load          load account and key from file\n" \
	"  -h, --help    show usage information\n"         \
	"  -v, --version show version\n"                   \
	""

void yoauth_show_usage(const char *argv0)
{
	YOAUTH_OUTPUT_TITLE("YoAuth " YOAUTH_VERSION);
	YOAUTH_TIP("Usage: %s [COMMAND] [OPTIONS]\n", argv0);
	YOAUTH_OUTPUT(STR_USAGE);

	yoauth_usage_command_list();
	yoauth_usage_command_add();
	yoauth_usage_command_del();
	yoauth_usage_command_dump();
}

int main(int argc, char *argv[])
{
	const char *command = NULL;
	int cmd_argc = 0;
	char **cmd_argv = NULL;

	if (argc < 2) {
		command = "list";
		cmd_argc = argc;
		cmd_argv = argv;
	} else {
		command = argv[1];
		cmd_argc = argc - 1;
		cmd_argv = &argv[1];
	}

	if (strcmp(command, "list") == 0) {
		yoauth_run_command_list(cmd_argc, cmd_argv);
	} else if (strcmp(command, "add") == 0) {
		yoauth_run_command_add(cmd_argc, cmd_argv);
	} else if (strcmp(command, "delete") == 0) {
		yoauth_run_command_del(cmd_argc, cmd_argv);
	} else if (strcmp(command, "dump") == 0) {
		yoauth_run_command_dump(cmd_argc, cmd_argv);
	} else if (strcmp(command, "load") == 0) {
		YOAUTH_OUTPUT("load");
	} else {
		if (strcmp(command, "-h") == 0 || strcmp(command, "--help") == 0) {
			yoauth_show_usage(argv[0]);
		} else if (strcmp(command, "-v") == 0 ||
				   strcmp(command, "--version") == 0) {
			YOAUTH_OUTPUT("yoauth %s", YOAUTH_VERSION);
		} else {
			YOAUTH_ERROR("invalid command: %s", command);
			YOAUTH_ERROR("use -h or --help to get Usage Information");
		}
	}

	return 0;
}
