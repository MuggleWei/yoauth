#include "command_add.h"
#include "style.h"
#include "handle.h"

void yoauth_usage_command_add()
{
	YOAUTH_OUTPUT_TITLE("Command: add");
	YOAUTH_TIP("add [OPTIONS]");
	YOAUTH_OUTPUT_COMMAND_DESC("-a, --account", "account name");
	YOAUTH_OUTPUT_COMMAND_DESC("-k, --key", "secret key in base32 format");
	YOAUTH_OUTPUT_COMMAND_DESC(
		"-u, --user",
		"specify user, without '-u', use 'default' user by default");

	YOAUTH_OUTPUT("");
	YOAUTH_TIP("e.g.");
	YOAUTH_OUTPUT_KV("yoauth -a github -k NBSWY3DPO5XXE3DE",
					 "//!< save secret key for account 'github'", 40);
}

typedef struct cmd_add_args {
	char user[64];
	char account[YOAUTH_TOTP_ACCOUNT_SIZE];
	char key[YOAUTH_TOTP_KEY_SIZE];
} cmd_add_args_t;

static bool yoauth_parse_args_command_add(int argc, char **argv,
										  cmd_add_args_t *args)
{
	static struct option long_options[] = {
		{ "user", required_argument, NULL, 'u' },
		{ "account", required_argument, NULL, 'a' },
		{ "key", required_argument, NULL, 'k' },
		{ NULL, 0, NULL, 0 }
	};

	memset(args, 0, sizeof(*args));

	while (true) {
		int c = 0;
		int option_index = 0;
		c = getopt_long(argc, argv, "u:a:k:", long_options, &option_index);
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
		case 'k': {
			int len = strlen(optarg);
			if (len >= (int)sizeof(args->key) - 1) {
				YOAUTH_ERROR("length of 'key' beyond the limit(%u)",
							 (unsigned int)sizeof(args->key) - 1);
				return false;
			}
			strncpy(args->key, optarg, sizeof(args->key) - 1);
		} break;
		}
	}

	if (args->user[0] == '\0') {
		strncpy(args->user, "default", sizeof(args->user) - 1);
	}
	if (args->account[0] == '\0') {
		YOAUTH_ERROR("run add command without option 'account'");
		return false;
	}
	if (args->key[0] == '\0') {
		YOAUTH_ERROR("run add command without option 'key'");
		return false;
	}

	return true;
}

void yoauth_run_command_add(int argc, char **argv)
{
	cmd_add_args_t args;
	if (!yoauth_parse_args_command_add(argc, argv, &args)) {
		YOAUTH_ERROR("failed parse arguments for command 'add'");
		exit(EXIT_FAILURE);
	}

	yoauth_handle_t handle;
	if (!yoauth_handle_setup(&handle, args.user)) {
		exit(EXIT_FAILURE);
	}

	if (!yoauth_handle_add(&handle, args.account, (unsigned char *)args.key,
						   strlen(args.key))) {
		YOAUTH_ERROR("failed add account '%s'", args.account);
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
