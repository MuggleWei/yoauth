#include "muggle/c/muggle_c.h"
#include "path_manager.h"
#include "style.h"
#include "command_list.h"

#define STR_USAGE                                      \
	"Usage: %s [COMMAND] [OPTIONS]\n"                  \
	"\n"                                               \
	"Commands:\n"                                      \
	"  list          list TOTP codes\n"                \
	"  add           add new account\n"                \
	"  remove        delete account\n"                 \
	"  dumps         dumps account and key to file\n"  \
	"  load          load account and key from file\n" \
	"  -h, --help    show usage information\n"         \
	"  -v, --version show version\n"                   \
	""

void yoauth_show_usage(const char *argv0)
{
	YOAUTH_OUTPUT_TITLE("YoAuth " YOAUTH_VERSION);
	YOAUTH_OUTPUT(STR_USAGE, argv0);

	yoauth_usage_command_list();
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
		YOAUTH_OUTPUT("add");
	} else if (strcmp(command, "remove") == 0) {
		YOAUTH_OUTPUT("remove");
	} else if (strcmp(command, "dumps") == 0) {
		YOAUTH_OUTPUT("dumps");
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
