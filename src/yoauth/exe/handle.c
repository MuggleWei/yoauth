#include "handle.h"
#include "yoauth/exe/tui.h"

static bool yoauth_root_dir(char *buf, size_t bufsize)
{
	char exepath[MUGGLE_MAX_PATH];
	if (muggle_os_process_path(exepath, sizeof(exepath)) != 0) {
		YOAUTH_ERROR("failed found yoauth process path");
		return false;
	}

	char bindir[MUGGLE_MAX_PATH];
	if (muggle_path_dirname(exepath, bindir, sizeof(bindir)) != 0) {
		YOAUTH_ERROR("failed get bin dir");
		return false;
	}

	if (muggle_path_dirname(bindir, buf, bufsize) != 0) {
		YOAUTH_ERROR("failed get root dir");
		return false;
	}

	return true;
}

static bool yoauth_data_dir(char *buf, size_t bufsize)
{
	char dirpath[MUGGLE_MAX_PATH];
	if (!yoauth_root_dir(dirpath, sizeof(dirpath))) {
		YOAUTH_ERROR("failed get yoauth root dir");
		return false;
	}

	if (muggle_path_join(dirpath, "data", buf, bufsize) != 0) {
		YOAUTH_ERROR("failed get yoauth data dir");
		return false;
	}

	return true;
}

static bool yoauth_user_dir(const char *username, char *buf, size_t bufsize)
{
	char data_dir[MUGGLE_MAX_PATH];
	if (!yoauth_data_dir(data_dir, sizeof(data_dir))) {
		YOAUTH_ERROR("failed get data dir");
		return false;
	}

	if (muggle_path_join(data_dir, username, buf, bufsize) != 0) {
		YOAUTH_ERROR("failed join user dir");
		return false;
	}

	return true;
}

static bool yoauth_user_data_path(const char *username, char *buf,
								  size_t bufsize)
{
	char user_dir[MUGGLE_MAX_PATH];
	if (!yoauth_user_dir(username, user_dir, sizeof(user_dir))) {
		YOAUTH_ERROR("failed get user dir");
		return false;
	}

	if (muggle_path_join(user_dir, "totp.data", buf, bufsize) != 0) {
		YOAUTH_ERROR("failed join user data path");
		return false;
	}

	return true;
}

static int dict_totp_cmp(const void *p1, const void *p2)
{
	return strcmp((const char *)p1, (const char *)p2);
}

void yoauth_message_loop(const char *username)
{
	char user_dir[MUGGLE_MAX_PATH];
	if (!yoauth_user_dir(username, user_dir, sizeof(user_dir))) {
		YOAUTH_ERROR("failed get user dir");
		return;
	}

	// create new user
	if (!muggle_path_exists(user_dir)) {
		if (!yoauth_create_user(username)) {
			YOAUTH_ERROR("failed create user");
		} else {
			YOAUTH_OUTPUT("success create user['%s']", username);
		}
		return;
	}

	// check password
	char password[32];
	yoauth_handle_t handle;
	memset(&handle, 0, sizeof(handle));
	int cnt = 0;
	while (true) {
		yoauth_get_password(password, sizeof(password));
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
		yoauth_tui_move_to(0, 0);
		yoauth_scenes_main(&handle, &tui);
	}

	yoauth_tui_cleanup();
}

void yoauth_show_usage()
{
	YOAUTH_STYLE_TITLE;
	YOAUTH_OUTPUT_SPLIT_LINE;
	YOAUTH_OUTPUT_TITLE("YoAuth version " YOAUTH_VERSION);
	YOAUTH_OUTPUT_SPLIT_LINE;
	YOAUTH_STYLE_NORMAL;

	YOAUTH_OUTPUT("");
	YOAUTH_OUTPUT("Usage:")
	YOAUTH_OUTPUT("  yoauth [commands] [--options]")

	YOAUTH_OUTPUT("");
	YOAUTH_STYLE_TITLE;
	YOAUTH_OUTPUT_SPLIT_LINE;
	YOAUTH_OUTPUT_TITLE("Available commands");
	YOAUTH_OUTPUT_SPLIT_LINE;
	YOAUTH_STYLE_NORMAL;

	YOAUTH_OUTPUT_COMMAND_DESC("yoauth", "login");
	YOAUTH_OUTPUT_COMMAND_DESC(
		"yoauth add -a <account> -s <base32 key>",
		"add account");
	YOAUTH_OUTPUT_COMMAND_DESC("yoauth del -a <account>", "delete account");

	YOAUTH_OUTPUT("");
	YOAUTH_STYLE_TITLE;
	YOAUTH_OUTPUT_SPLIT_LINE;
	YOAUTH_OUTPUT_TITLE("Example");
	YOAUTH_OUTPUT_SPLIT_LINE;
	YOAUTH_STYLE_NORMAL;

	YOAUTH_OUTPUT("yoauth")
	YOAUTH_OUTPUT("  login and show all TOTP code");
	YOAUTH_OUTPUT("");

	YOAUTH_OUTPUT("yoauth add -a google -s ")
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
	if (!yoauth_user_data_path(username, filepath, sizeof(filepath))) {
		YOAUTH_ERROR("failed get user data path");
		return false;
	}

	yoauth_head_t hdr;
	memset(&hdr, 0, sizeof(hdr));
	memcpy(hdr.magic, YOAUTH_DATA_HEAD_MAGIC, 4);
	hdr.ver = YOAUTH_DATA_VERSION;
	hdr.datalen = 0;
	hdr.datatype = YOAUTH_DATA_TYPE_TOTP;
	if (!yoauth_fileutils_store_totp(filepath, passwd1, (unsigned char *)&hdr,
									 sizeof(hdr))) {
		YOAUTH_ERROR("failed create user data file");
		return false;
	}

	return true;
}

void yoauth_get_password(char *buf, size_t bufsize)
{
	yoauth_tui_passwd("enter password: ", buf, bufsize);
	YOAUTH_OUTPUT("")
}

bool yoauth_handle_init(yoauth_handle_t *handle, const char *username,
						const char *password)
{
	memset(handle, 0, sizeof(*handle));
	handle->username = username;
	handle->password = password;

	if (!muggle_avl_tree_init(&handle->dict_totp, dict_totp_cmp, 0)) {
		YOAUTH_ERROR("failed init dict");
		return false;
	}

	char filepath[MUGGLE_MAX_PATH];
	if (!yoauth_user_data_path(username, filepath, sizeof(filepath))) {
		YOAUTH_ERROR("failed load user data");
		return false;
	}

	if (!muggle_path_exists(filepath)) {
		YOAUTH_ERROR("user file not exists");
		return false;
	}

	unsigned long numbytes = 0;
	yoauth_head_t *hdr = (yoauth_head_t *)yoauth_fileutils_load_totp(
		filepath, handle->password, &numbytes);
	if (hdr == NULL) {
		YOAUTH_ERROR("failed load user data, plz check your password");
		return false;
	}

	uint32_t cnt = hdr->datalen / sizeof(yoauth_data_totp_t);
	yoauth_data_totp_t *datas = (yoauth_data_totp_t *)(hdr + 1);
	for (uint32_t i = 0; i < cnt; i++) {
		char *k = (char *)malloc(sizeof(datas[i].account));
		memcpy(k, datas[i].account, sizeof(datas[i].account));
		yoauth_totp_t *v =
			yoauth_totp_init(datas[i].key, sizeof(datas[i].key), NULL);
		muggle_avl_tree_insert(&handle->dict_totp, k, v);
	}

	free(hdr);

	return true;
}

void yoauth_scenes_main(yoauth_handle_t *handle, yoauth_tui_t *tui)
{
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
}
