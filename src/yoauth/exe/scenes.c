#include "scenes.h"
#include "path_manager.h"
#include "tui.h"
#include "handle.h"

void yoauth_loop(const char *username)
{
	char user_dir[MUGGLE_MAX_PATH];
	if (!yoauth_path_user_dir(username, user_dir, sizeof(user_dir))) {
		YOAUTH_ERROR("failed get user dir");
		return;
	}

	if (!muggle_path_exists(user_dir)) {
		// create new user
		if (!yoauth_create_user(username)) {
			YOAUTH_ERROR("failed create user");
		} else {
			YOAUTH_OUTPUT("success create user['%s'], exit; plz relogin",
						  username);
		}
		return;
	}

	// check password
	yoauth_handle_t handle;
	memset(&handle, 0, sizeof(handle));

	int cnt = 0;
	char password[32];
	while (true) {
		yoauth_tui_passwd("enter password: ", password, sizeof(password));
		if (yoauth_handle_init(&handle, username, password)) {
			break;
		}
		if (++cnt == 3) {
			YOAUTH_OUTPUT("already try 3 times, exit");
			return;
		}
	}

	yoauth_tui_setup();

	yoauth_tui_t tui;
	yoauth_tui_store(&tui);
	yoauth_tui_close_echo(&tui);

	while (true) {
		yoauth_scenes_main(&handle, &tui);
	}

	yoauth_tui_cleanup();

	yoauth_handle_destroy(&handle);
}

bool yoauth_create_user(const char *username)
{
	YOAUTH_OUTPUT("user['%s'] first login, plz enter password", username);

	char passwd1[32];
	yoauth_tui_passwd("enter password: ", passwd1, sizeof(passwd1));
	YOAUTH_OUTPUT("")

	char passwd2[32];
	yoauth_tui_passwd("enter password again: ", passwd2, sizeof(passwd2));
	YOAUTH_OUTPUT("");

	if (strcmp(passwd1, passwd2) != 0) {
		YOAUTH_ERROR("The two entered passwords don't match");
		return false;
	}

	char filepath[MUGGLE_MAX_PATH];
	if (!yoauth_path_user_file(username, filepath, sizeof(filepath))) {
		YOAUTH_ERROR("failed get user data path");
		return false;
	}

	yoauth_head_t hdr;
	memset(&hdr, 0, sizeof(hdr));
	memcpy(hdr.magic, YOAUTH_DATA_HEAD_MAGIC, 4);
	hdr.ver = YOAUTH_DATA_VERSION;
	hdr.data_type = YOAUTH_DATA_TYPE_TOTP;
	hdr.data_size = sizeof(yoauth_totp_data_t);
	hdr.cnt = 0;
	if (!yoauth_fileutils_store_totp(filepath, passwd1, (unsigned char *)&hdr,
									 sizeof(hdr))) {
		YOAUTH_ERROR("failed create user data file");
		return false;
	}

	return true;
}

void yoauth_scenes_main(yoauth_handle_t *handle, yoauth_tui_t *tui)
{
	yoauth_tui_move_to(0, 0);

	// page head
	YOAUTH_STYLE_TIP;
	fprintf(stdout, "a: ");
	YOAUTH_STYLE_NORMAL;
	fprintf(stdout, "add account & secret key;    ");
	YOAUTH_STYLE_TIP;
	fprintf(stdout, "d: ");
	YOAUTH_STYLE_NORMAL;
	fprintf(stdout, "delete account & secret key;    ");
	YOAUTH_STYLE_TIP;
	fprintf(stdout, "q: ");
	YOAUTH_STYLE_NORMAL;
	fprintf(stdout, "exit");
	YOAUTH_OUTPUT("");
	YOAUTH_OUTPUT_SPLIT_LINE;

	// TODO: testxxx
	getchar();
}
