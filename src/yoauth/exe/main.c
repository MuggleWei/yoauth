#include "handle.h"

void parse_sys_args(int argc, char **argv)
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
		default: {
			YOAUTH_ERROR("unrecognize options");
			exit(EXIT_FAILURE);
		} break;
		}
	}
}

int main(int argc, char *argv[])
{
	const char *username = "default";
	if (argc == 1) {
		yoauth_message_loop(username);
	} else if (argc == 3) {
		const char *cmd = argv[1];
		const char *account = argv[2];
		if (strcmp(cmd, "add") == 0) {
		}
	} else {
		parse_sys_args(argc, argv);
	}

	return 0;
}
