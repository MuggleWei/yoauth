#include "handle.h"

typedef struct sys_args {
	char username[32];
} sys_args_t;

void parse_sys_args(int argc, char **argv, sys_args_t *args)
{
	static struct option long_options[] = {
		{ "help", no_argument, NULL, 'h' },
		{ "version", no_argument, NULL, 'v' },
		{ "user", required_argument, NULL, 'u' },
		{ NULL, 0, NULL, 0 }
	};

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
			if (strlen(optarg) >= sizeof(args->username)) {
				YOAUTH_ERROR("username length beyond the limit(%u)",
							 (unsigned int)sizeof(args->username) - 1);
				exit(EXIT_FAILURE);
			}
			strncpy(args->username, optarg, sizeof(args->username) - 1);
		} break;
		}
	}

	if (args->username[0] == '\0') {
		strncpy(args->username, "default", sizeof(args->username));
	}
}

int main(int argc, char *argv[])
{
	sys_args_t args;
	memset(&args, 0, sizeof(args));
	parse_sys_args(argc, argv, &args);

	yoauth_message_loop(args.username);

	return 0;
}
