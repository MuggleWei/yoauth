#include "command_load.h"
#include "style.h"
#include "handle.h"

void yoauth_usage_command_load()
{
	YOAUTH_OUTPUT_TITLE("Command: load");
	YOAUTH_TIP("load [OPTIONS]");
	YOAUTH_OUTPUT_COMMAND_DESC("-i, --input", "input file path");
	YOAUTH_OUTPUT_COMMAND_DESC(
		"-u, --user",
		"specify user, without '-u', use 'default' user by default");

	YOAUTH_OUTPUT("");
	YOAUTH_TIP("e.g.");
	YOAUTH_OUTPUT_KV("yoauth load -i ./load/foo.txt",
					 "//!< load account and key from ./load/foo.txt", 40);
}

typedef struct cmd_load_args {
	char user[64];
	char input[MUGGLE_MAX_PATH];
} cmd_load_args_t;

static bool yoauth_parse_args_command_load(int argc, char **argv,
										  cmd_load_args_t *args)
{
	static struct option long_options[] = {
		{ "user", required_argument, NULL, 'u' },
		{ "input", required_argument, NULL, 'i' },
		{ NULL, 0, NULL, 0 }
	};

	memset(args, 0, sizeof(*args));

	while (true) {
		int c = 0;
		int option_index = 0;
		c = getopt_long(argc, argv, "u:i:", long_options, &option_index);
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
		case 'i': {
			int len = strlen(optarg);
			if (len >= (int)sizeof(args->input) - 1) {
				YOAUTH_ERROR("length of 'input' beyond the limit(%u)",
							 (unsigned int)sizeof(args->input) - 1);
				return false;
			}
			strncpy(args->input, optarg, sizeof(args->input) - 1);
		} break;
		}
	}

	if (args->user[0] == '\0') {
		strncpy(args->user, "default", sizeof(args->user) - 1);
	}
	if (args->input[0] == '\0') {
		YOAUTH_ERROR("run load command without option 'input'");
		return false;
	}

	return true;
}

void yoauth_run_command_load(int argc, char **argv)
{
	cmd_load_args_t args;
	if (!yoauth_parse_args_command_load(argc, argv, &args)) {
		YOAUTH_ERROR("failed parse arguments for command 'load'");
		exit(EXIT_FAILURE);
	}

	yoauth_handle_t handle;
	if (!yoauth_handle_setup(&handle, args.user)) {
		exit(EXIT_FAILURE);
	}

	if (!yoauth_handle_load_csv(&handle, args.input)) {
		YOAUTH_ERROR("failed load from '%s'", args.input);
		yoauth_handle_destroy(&handle);
		exit(EXIT_FAILURE);
	}
	if (!yoauth_handle_store(&handle)) {
		YOAUTH_ERROR("failed store account '%s'", handle.user);
		yoauth_handle_destroy(&handle);
		exit(EXIT_FAILURE);
	}

	yoauth_handle_destroy(&handle);
}
