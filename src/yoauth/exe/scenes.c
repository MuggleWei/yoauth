#include "scenes.h"
#include "path_manager.h"
#include "tui.h"
#include "handle.h"

void yoauth_loop(sys_args_t *args)
{
	yoauth_handle_t handle;
	memset(&handle, 0, sizeof(handle));

	char user_dir[MUGGLE_MAX_PATH];
	if (!yoauth_path_user_dir(args->user, user_dir, sizeof(user_dir))) {
		YOAUTH_ERROR("failed get user dir");
		return;
	}

	if (!muggle_path_exists(user_dir)) {
		// create new user
		if (!yoauth_create_user(args)) {
			YOAUTH_ERROR("failed create user");
		} else {
			YOAUTH_OUTPUT("success create user['%s'], exit; plz relogin",
						  args->user);
		}
		return;
	} else {
		int cnt = 0;
		while (true) {
			yoauth_tui_passwd("enter password: ", args->password,
							  sizeof(args->password));
			// check password
			if (yoauth_handle_init(&handle, args->user, args->password)) {
				YOAUTH_OUTPUT("");
				break;
			}
			if (++cnt == 3) {
				YOAUTH_ERROR("already try 3 times, exit");
				return;
			}
		}
	}

	if (args->add_account[0] != '\0') {
		if (!yoauth_handle_add(&handle, args->add_account,
							   (unsigned char *)args->secret, args->keylen)) {
			YOAUTH_ERROR("failed add account");
		} else {
			yoauth_handle_dumps(&handle);
		}
	} else if (args->del_account[0] != '\0') {
		if (!yoauth_handle_del(&handle, args->del_account)) {
			YOAUTH_ERROR("failed delete account");
		} else {
			yoauth_handle_dumps(&handle);
		}
	} else {
		yoauth_tui_setup();

		yoauth_tui_t tui;
		yoauth_tui_store(&tui);
		yoauth_tui_close_echo(&tui);

		yoauth_scenes_main(&handle, &tui);

		yoauth_tui_cleanup();

		yoauth_handle_destroy(&handle);
	}
}

bool yoauth_create_user(sys_args_t *args)
{
	YOAUTH_OUTPUT("user['%s'] first login, plz enter password", args->user);

	char passwd1[sizeof(args->password)];
	yoauth_tui_passwd("enter password: ", passwd1, sizeof(passwd1));
	YOAUTH_OUTPUT("")

	char passwd2[sizeof(args->password)];
	yoauth_tui_passwd("enter password again: ", passwd2, sizeof(passwd2));
	YOAUTH_OUTPUT("");

	if (strcmp(passwd1, passwd2) != 0) {
		YOAUTH_ERROR("The two entered passwords don't match");
		return false;
	}

	memcpy(args->password, passwd1, sizeof(args->password));

	char filepath[MUGGLE_MAX_PATH];
	if (!yoauth_path_user_file(args->user, filepath, sizeof(filepath))) {
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
	MUGGLE_UNUSED(tui);

	yoauth_tui_move_to(0, 0);

	// page head
	YOAUTH_STYLE_TIP;
	fprintf(stdout, "press enter: ");
	YOAUTH_STYLE_NORMAL;
	fprintf(stdout, "exit");
	YOAUTH_OUTPUT("");
	YOAUTH_OUTPUT_SPLIT_LINE;

	time_t ts = time(NULL);
	size_t cnt = muggle_array_list_size(handle->datas);
	for (size_t i = 0; i < cnt; i++) {
		muggle_array_list_node_t *node =
			muggle_array_list_index(handle->datas, i);
		yoauth_totp_data_t *data = (yoauth_totp_data_t *)node->data;
		int32_t code = yoauth_totp_at(data, ts);
		char v[16];
		snprintf(v, sizeof(v), "%06d", code);
		YOAUTH_OUTPUT_KV(data->account, v, sizeof(data->account) + 2);
	}

	getchar();
}
