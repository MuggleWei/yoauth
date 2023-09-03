#include "command_dump.h"
#include "style.h"
#include "handle.h"

void yoauth_usage_command_dump()
{
	YOAUTH_OUTPUT_TITLE("Command: dump");
	YOAUTH_TIP("dump [OPTIONS]");
	YOAUTH_OUTPUT_COMMAND_DESC("-o, --output", "output file path");
	YOAUTH_OUTPUT_COMMAND_DESC(
		"-u, --user",
		"specify user, without '-u', use 'default' user by default");

	YOAUTH_OUTPUT("");
	YOAUTH_TIP("e.g.");
	YOAUTH_OUTPUT_KV("yoauth dump -o ./dump/foo.txt",
					 "//!< dump account and key into ./dump/foo.txt", 40);
}

typedef struct cmd_dump_args {
	char user[64];
	char output[MUGGLE_MAX_PATH];
} cmd_dump_args_t;

static bool yoauth_parse_args_command_dump(int argc, char **argv,
										  cmd_dump_args_t *args)
{
	static struct option long_options[] = {
		{ "user", required_argument, NULL, 'u' },
		{ "output", required_argument, NULL, 'o' },
		{ "help", required_argument, NULL, 'h' },
		{ NULL, 0, NULL, 0 }
	};

	memset(args, 0, sizeof(*args));

	while (true) {
		int c = 0;
		int option_index = 0;
		c = getopt_long(argc, argv, "u:o:h", long_options, &option_index);
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
		case 'o': {
			int len = strlen(optarg);
			if (len >= (int)sizeof(args->output) - 1) {
				YOAUTH_ERROR("length of 'output' beyond the limit(%u)",
							 (unsigned int)sizeof(args->output) - 1);
				return false;
			}
			strncpy(args->output, optarg, sizeof(args->output) - 1);
		} break;
		case 'h': {
			yoauth_usage_command_dump();
			exit(EXIT_SUCCESS);
		} break;
		}
	}

	if (args->user[0] == '\0') {
		strncpy(args->user, "default", sizeof(args->user) - 1);
	}
	if (args->output[0] == '\0') {
		YOAUTH_ERROR("run dump command without option 'output'");
		return false;
	}

	return true;
}

void yoauth_run_command_dump(int argc, char **argv)
{
	cmd_dump_args_t args;
	if (!yoauth_parse_args_command_dump(argc, argv, &args)) {
		YOAUTH_ERROR("failed parse arguments for command 'dump'");
		exit(EXIT_FAILURE);
	}

	yoauth_handle_t handle;
	if (!yoauth_handle_setup(&handle, args.user)) {
		exit(EXIT_FAILURE);
	}

	if (!yoauth_handle_dump(&handle, args.output)) {
		YOAUTH_ERROR("failed dump to '%s'", args.output);
		yoauth_handle_destroy(&handle);
		exit(EXIT_FAILURE);
	}

	yoauth_handle_destroy(&handle);
}
