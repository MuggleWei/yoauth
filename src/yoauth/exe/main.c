#include "handle.h"

typedef struct sys_args {
	char user[16];
} sys_args_t;

void parse_sys_args(int argc, char **argv, sys_args_t *args)
{
	static struct option long_options[] = {
		{ "help", no_argument, NULL, 'h' },
		{ "version", no_argument, NULL, 'v' },
		{ "user", required_argument, NULL, 'u' },
		{ NULL, 0, NULL, 0 }
	};

	memset(args, 0, sizeof(*args));

	while (true) {
		int c = 0;
		int option_index = 0;
		c = getopt_long(argc, argv, "hvu:", long_options, &option_index);
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
		default: {
			YOAUTH_ERROR("unrecognize options");
			exit(EXIT_FAILURE);
		} break;
		}
	}

	if (args->user[0] == '\0') {
		strncpy(args->user, "default", sizeof(args->user) - 1);
	}
}

int main(int argc, char *argv[])
{
	sys_args_t args;
	parse_sys_args(argc, argv, &args);

	yoauth_message_loop(args.user);

	return 0;
}
