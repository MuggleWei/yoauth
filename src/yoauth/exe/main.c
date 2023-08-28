#include "muggle/c/muggle_c.h"
#include "tui.h"
#include "path_manager.h"
#include "scenes.h"

void yoauth_show_usage()
{
	YOAUTH_STYLE_TITLE;
	YOAUTH_OUTPUT_SPLIT_LINE;
	YOAUTH_OUTPUT_TITLE("YoAuth version " YOAUTH_VERSION);
	YOAUTH_OUTPUT_SPLIT_LINE;
	YOAUTH_STYLE_NORMAL;

	YOAUTH_OUTPUT("");
	YOAUTH_OUTPUT("Usage:")
	YOAUTH_OUTPUT("  yoauth [--options]")

	YOAUTH_OUTPUT("");
	YOAUTH_STYLE_TITLE;
	YOAUTH_OUTPUT_SPLIT_LINE;
	YOAUTH_OUTPUT_TITLE("Available commands");
	YOAUTH_OUTPUT_SPLIT_LINE;
	YOAUTH_STYLE_NORMAL;

	YOAUTH_OUTPUT_COMMAND_DESC("yoauth", "");
	YOAUTH_OUTPUT_COMMAND_DESC("  -u, --user", "specifiy user");
	YOAUTH_OUTPUT_COMMAND_DESC("  -a, --add", "add new account");
	YOAUTH_OUTPUT_COMMAND_DESC("  -d, --delete", "delete account");
	YOAUTH_OUTPUT_COMMAND_DESC("  -s, --secret", "secret key for new account");
	YOAUTH_OUTPUT_COMMAND_DESC("", "  - 16 bytes for text");
	YOAUTH_OUTPUT_COMMAND_DESC("", "  - other for base32 format");

	YOAUTH_OUTPUT("");
	YOAUTH_STYLE_TITLE;
	YOAUTH_OUTPUT_SPLIT_LINE;
	YOAUTH_OUTPUT_TITLE("For example");
	YOAUTH_OUTPUT_SPLIT_LINE;
	YOAUTH_STYLE_NORMAL;

	YOAUTH_OUTPUT("yoauth");
	YOAUTH_OUTPUT("  list account and TOTP codes for default user");
	YOAUTH_OUTPUT("");

	YOAUTH_OUTPUT("yoauth -u mugglewei");
	YOAUTH_OUTPUT("  list account and TOTP codes for user 'mugglewei'");
	YOAUTH_OUTPUT("");

	YOAUTH_OUTPUT("yoauth -a gitlab -s 06Y8nuqqNdwo8oHd");
	YOAUTH_OUTPUT("  add account and TOTP secret key");
	YOAUTH_OUTPUT("");

	YOAUTH_OUTPUT("yoauth -a gitlab -s GA3FSODOOVYXCTTEO5XTQ32IMQ");
	YOAUTH_OUTPUT("  add account and TOTP secret key with base32 format");
	YOAUTH_OUTPUT("");

	YOAUTH_OUTPUT("yoauth -d gitlab");
	YOAUTH_OUTPUT("  delete account");
	YOAUTH_OUTPUT("");
}

void parse_sys_args(int argc, char **argv, sys_args_t *args)
{
	static struct option long_options[] = {
		{ "help", no_argument, NULL, 'h' },
		{ "version", no_argument, NULL, 'v' },
		{ "user", required_argument, NULL, 'u' },
		{ "add", required_argument, NULL, 'a' },
		{ "delete", required_argument, NULL, 'd' },
		{ "secret", required_argument, NULL, 's' },
		{ NULL, 0, NULL, 0 }
	};

	memset(args, 0, sizeof(*args));

	while (true) {
		int c = 0;
		int option_index = 0;
		c = getopt_long(argc, argv, "hvu:a:s:", long_options, &option_index);
		if (c == -1) {
			break;
		}

		switch (c) {
		case 'h': {
			yoauth_show_usage();
			exit(EXIT_SUCCESS);
		} break;
		case 'v': {
			YOAUTH_OUTPUT("YoAuth version: " YOAUTH_VERSION);
			exit(EXIT_SUCCESS);
		} break;
		case 'u': {
			int len = strlen(optarg);
			if (len >= (int)sizeof(args->user) - 1) {
				YOAUTH_ERROR("length of user beyond the limit(15)");
				exit(EXIT_FAILURE);
			}
			strncpy(args->user, optarg, len);
			args->user[len] = '\0';
		} break;
		case 'a': {
			int len = strlen(optarg);
			if (len >= (int)sizeof(args->add_account) - 1) {
				YOAUTH_ERROR("length of add_account beyond the limit(15)");
				exit(EXIT_FAILURE);
			}
			strncpy(args->add_account, optarg, len);
		} break;
		case 'd': {
			int len = strlen(optarg);
			if (len >= (int)sizeof(args->del_account) - 1) {
				YOAUTH_ERROR("length of del_account beyond the limit(15)");
				exit(EXIT_FAILURE);
			}
			strncpy(args->del_account, optarg, len);
		} break;
		case 's': {
			int len = strlen(optarg);
			if (len == 16) {
				memcpy(args->secret, optarg, 16);
			} else {
				int cnt = base32_decode((unsigned char *)optarg,
										(unsigned char *)args->secret,
										sizeof(args->secret));
				if (cnt != sizeof(args->secret)) {
					YOAUTH_ERROR("failed base32 decode");
					exit(EXIT_FAILURE);
				}
			}
		} break;
		default: {
			YOAUTH_ERROR("unrecognize options");
			exit(EXIT_FAILURE);
		} break;
		}
	}

	if (args->user[0] == '\0') {
		strncpy(args->user, "default", sizeof(args->user) - 1);
	}

	if (args->add_account[0] != '\0' && args->secret[0] == '\0') {
		YOAUTH_ERROR("failed add account, secret is null");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[])
{
	muggle_log_complicated_init(-1, LOG_LEVEL_DEBUG, NULL);

	sys_args_t args;
	parse_sys_args(argc, argv, &args);

	yoauth_loop(&args);

	return 0;
}
