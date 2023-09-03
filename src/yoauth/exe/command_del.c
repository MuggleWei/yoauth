#include "command_del.h"
#include "style.h"
#include "handle.h"

void yoauth_usage_command_del()
{
	YOAUTH_OUTPUT_TITLE("Command: delete");
	YOAUTH_TIP("delete [OPTIONS]");
	YOAUTH_OUTPUT_COMMAND_DESC("-a, --account", "account name");
	YOAUTH_OUTPUT_COMMAND_DESC(
		"-u, --user",
		"specify user, without '-u', use 'default' user by default");

	YOAUTH_OUTPUT("");
	YOAUTH_TIP("e.g.");
	YOAUTH_OUTPUT_KV("yoauth delete -d github",
					 "//!< delte account 'github'", 40);
}

typedef struct cmd_del_args {
	char user[64];
	char account[YOAUTH_TOTP_ACCOUNT_SIZE];
} cmd_del_args_t;

static bool yoauth_parse_args_command_del(int argc, char **argv,
										  cmd_del_args_t *args)
{
	static struct option long_options[] = {
		{ "user", required_argument, NULL, 'u' },
		{ "account", required_argument, NULL, 'a' },
		{ NULL, 0, NULL, 0 }
	};

	memset(args, 0, sizeof(*args));

	while (true) {
		int c = 0;
		int option_index = 0;
		c = getopt_long(argc, argv, "u:a:", long_options, &option_index);
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
		case 'a': {
			int len = strlen(optarg);
			if (len >= (int)sizeof(args->account) - 1) {
				YOAUTH_ERROR("length of 'account' beyond the limit(%u)",
							 (unsigned int)sizeof(args->account) - 1);
				return false;
			}
			strncpy(args->account, optarg, sizeof(args->account) - 1);
		} break;
		}
	}

	if (args->user[0] == '\0') {
		strncpy(args->user, "default", sizeof(args->user) - 1);
	}
	if (args->account[0] == '\0') {
		YOAUTH_ERROR("run del command without option 'account'");
		return false;
	}

	return true;
}

void yoauth_run_command_del(int argc, char **argv)
{
	cmd_del_args_t args;
	if (!yoauth_parse_args_command_del(argc, argv, &args)) {
		YOAUTH_ERROR("failed parse arguments for command 'del'");
		exit(EXIT_FAILURE);
	}

	yoauth_handle_t handle;
	if (!yoauth_handle_setup(&handle, args.user)) {
		exit(EXIT_FAILURE);
	}

	if (!yoauth_handle_del(&handle, args.account)) {
		YOAUTH_ERROR("failed delete account '%s'", args.account);
		yoauth_handle_destroy(&handle);
		exit(EXIT_FAILURE);
	}
	if (!yoauth_handle_store(&handle)) {
		YOAUTH_ERROR("failed store account '%s'", args.account);
		yoauth_handle_destroy(&handle);
		exit(EXIT_FAILURE);
	}

	yoauth_handle_destroy(&handle);
}
