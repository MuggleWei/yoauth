#include "command_list.h"
#include "style.h"
#include "handle.h"

void yoauth_usage_command_list()
{
	YOAUTH_OUTPUT_TITLE("Command: list");
	YOAUTH_OUTPUT("list [OPTIONS]");
	YOAUTH_OUTPUT_COMMAND_DESC("-f, --filter", "filter account");
	YOAUTH_OUTPUT_COMMAND_DESC(
		"-u, --user",
		"specify user, without '-u', use 'default' user by default");

	YOAUTH_OUTPUT("");
	YOAUTH_OUTPUT("e.g.");
	YOAUTH_OUTPUT_KV("yoauth -f google", "get 'google' TOTP code", 32);
	YOAUTH_OUTPUT_KV("yoauth list -f google", "get 'google' TOTP code", 32);
	YOAUTH_OUTPUT_KV("yoauth list -u foo -f google",
					 "get 'google' TOTP code for user 'foo'", 32);

	YOAUTH_OUTPUT("");
}

typedef struct cmd_list_args {
	char user[64];
	char filter[64];
} cmd_list_args_t;

static bool yoauth_parse_args_command_list(int argc, char **argv,
										   cmd_list_args_t *args)
{
	static struct option long_options[] = {
		{ "user", required_argument, NULL, 'u' },
		{ "filter", required_argument, NULL, 'f' },
		{ NULL, 0, NULL, 0 }
	};

	memset(args, 0, sizeof(*args));

	while (true) {
		int c = 0;
		int option_index = 0;
		c = getopt_long(argc, argv, "u:f:", long_options, &option_index);
		if (c == -1) {
			break;
		}

		switch (c) {
		case 'u': {
			int len = strlen(optarg);
			if (len >= (int)sizeof(args->user) - 1) {
				YOAUTH_ERROR("length of 'user' beyond the limit(%u)",
							 (unsigned int)sizeof(args->user) - 1);
				return false;
			}
			strncpy(args->user, optarg, sizeof(args->user) - 1);
		} break;
		case 'f': {
			int len = strlen(optarg);
			if (len >= (int)sizeof(args->filter) - 1) {
				YOAUTH_ERROR("length of 'filter' beyond the limit(%u)",
							 (unsigned int)sizeof(args->filter) - 1);
				return false;
			}
			strncpy(args->filter, optarg, sizeof(args->filter) - 1);
		} break;
		}
	}

	if (args->user[0] == '\0') {
		strncpy(args->user, "default", sizeof(args->user) - 1);
	}

	return true;
}

void yoauth_run_command_list(int argc, char **argv)
{
	cmd_list_args_t args;
	if (!yoauth_parse_args_command_list(argc, argv, &args)) {
		YOAUTH_ERROR("failed parse arguments for command 'list'");
		return;
	}

	yoauth_handle_t handle;
	if (!yoauth_handle_setup(&handle, args.user)) {
		return;
	}

	YOAUTH_OUTPUT_TITLE("TOTP codes");
	yoauth_handle_list(&handle, args.filter);

	yoauth_handle_destroy(&handle);
}
