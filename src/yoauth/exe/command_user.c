#include "command_user.h"

bool yoauth_cmd_user(yoauth_handle_t *handle)
{
	if (handle->nargs < 2) {
		YOAUTH_STYLE_ERROR;
		YOAUTH_OUTPUT("User Command with incorrect arguments");
		yoauth_cmd_user_show_usage();
		YOAUTH_STYLE_NORMAL;
		return false;
	}

	bool ret = false;
	if (strcmp(handle->args[1], "login") == 0) {
		ret = yoauth_cmd_user_login(handle);
	} else if (strcmp(handle->args[1], "create") == 0) {
	} else {
		YOAUTH_STYLE_ERROR;
		YOAUTH_OUTPUT("User Command with incorrect sub-command");
		yoauth_cmd_user_show_usage();
		YOAUTH_STYLE_NORMAL;
	}

	return ret;
}

void yoauth_cmd_user_show_usage()
{
	YOAUTH_OUTPUT("User Commands:");
	YOAUTH_OUTPUT_COMMAND_DESC("user login <username> <password>",
							   "user login");
	YOAUTH_OUTPUT_COMMAND_DESC("user create <username> <password>",
							   "add new user");
	YOAUTH_OUTPUT("");
}

bool yoauth_cmd_user_login(yoauth_handle_t *handle)
{
	if (handle->nargs != 4) {
		YOAUTH_STYLE_ERROR;
		YOAUTH_OUTPUT("User Login Command with incorrect argument number");
		yoauth_cmd_user_show_usage();
		YOAUTH_STYLE_NORMAL;
		return false;
	}

	const char *user = handle->args[2];
	const char *passwd = handle->args[3];

	char user_dir[MUGGLE_MAX_PATH];
	muggle_path_join(handle->data_dir, user, user_dir, sizeof(user_dir));
	if (!muggle_path_exists(user_dir)) {
		YOAUTH_ERROR("user['%s'] not exists", user);
		return false;
	}

	char totp_filepath[MUGGLE_MAX_PATH];
	muggle_path_join(user_dir, "totp.data", totp_filepath, sizeof(totp_filepath));
	if (!muggle_path_exists(totp_filepath)) {
		YOAUTH_ERROR("user['%s'] totp file not exist", user);
		return false;
	}


	return true;
}
