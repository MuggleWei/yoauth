#include "path_manager.h"
#include "muggle/c/muggle_c.h"
#include "tui.h"

bool yoauth_path_root_dir(char *buf, size_t bufsize)
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

bool yoauth_path_data_dir(char *buf, size_t bufsize)
{
	char dirpath[MUGGLE_MAX_PATH];
	if (!yoauth_path_root_dir(dirpath, sizeof(dirpath))) {
		YOAUTH_ERROR("failed get yoauth root dir");
		return false;
	}

	if (muggle_path_join(dirpath, "data", buf, bufsize) != 0) {
		YOAUTH_ERROR("failed get yoauth data dir");
		return false;
	}

	return true;
}

bool yoauth_path_user_dir(const char *username, char *buf, size_t bufsize)
{
	char data_dir[MUGGLE_MAX_PATH];
	if (!yoauth_path_data_dir(data_dir, sizeof(data_dir))) {
		YOAUTH_ERROR("failed get data dir");
		return false;
	}

	if (muggle_path_join(data_dir, username, buf, bufsize) != 0) {
		YOAUTH_ERROR("failed join user dir");
		return false;
	}

	return true;
}

bool yoauth_path_user_file(const char *username, char *buf, size_t bufsize)
{
	char user_dir[MUGGLE_MAX_PATH];
	if (!yoauth_path_user_dir(username, user_dir, sizeof(user_dir))) {
		YOAUTH_ERROR("failed get user dir");
		return false;
	}

	if (muggle_path_join(user_dir, "totp.data", buf, bufsize) != 0) {
		YOAUTH_ERROR("failed join user data path");
		return false;
	}

	return true;
}
