#include "handle.h"

void parse_sys_args(int argc, char **argv)
{
	char str_usage[1024];
	snprintf(str_usage, sizeof(str_usage),
			 "Usage: %s <command>\n"
			 "\t-h, --help     output help information\n"
			 "\t-v, --version  show version\n"
			 "",
			 argv[0]);

	static struct option long_options[] = { { "help", no_argument, NULL, 'h' },
											{ "version", no_argument, NULL,
											  'v' },
											{ NULL, 0, NULL, 0 } };

	while (true) {
		int c = 0;
		int option_index = 0;
		c = getopt_long(argc, argv, "hv", long_options, &option_index);
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
		}
	}
}

int main(int argc, char *argv[])
{
	parse_sys_args(argc, argv);

	yoauth_show_welcome_info();
	yoauth_message_loop();

	return 0;
}
