#include "handle.h"
#include "path_manager.h"
#include "style.h"

#if MUGGLE_PLATFORM_WINDOWS
	#include <conio.h>
#else
	#include <termios.h>
#endif

#if MUGGLE_PLATFORM_WINDOWS
static void yoauth_handle_passwd(const char *tip, char *buf, size_t bufsize)
{
	fprintf(stdout, "%s", tip);
	fflush(stdout);

	const char ENTER = 13;
	int i = 0;
	int c = 0;
	while ((c = _getch()) != ENTER && c != EOF && i < (int)bufsize - 1) {
		buf[i++] = c;
	}
	buf[i] = '\0';

	YOAUTH_OUTPUT("");
}
#else
static void yoauth_handle_passwd(const char *tip, char *buf, size_t bufsize)
{
	struct termios old_term;
	struct termios cur_term;

	tcgetattr(STDIN_FILENO, &old_term);
	memcpy(&cur_term, &old_term, sizeof(cur_term));

	cur_term.c_lflag &= ~(ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &cur_term);

	fprintf(stdout, "%s", tip);
	fflush(stdout);

	int i = 0;
	int c = 0;
	while ((c = getchar()) != '\n' && c != EOF && i < (int)bufsize - 1) {
		if (c == 8 && i != 0) {
			i--;
			continue;
		}
		buf[i++] = c;
	}
	buf[i] = '\0';

	tcsetattr(STDIN_FILENO, TCSANOW, &old_term);

	YOAUTH_OUTPUT("");
}
#endif

static bool yoauth_handle_create_user(yoauth_handle_t *handle)
{
	YOAUTH_OUTPUT("user['%s'] first login, plz enter password", handle->user);

	char passwd1[YOAUTH_PASSWD_SIZE];
	yoauth_handle_passwd("enter password: ", passwd1, sizeof(passwd1));
	YOAUTH_OUTPUT("")

	char passwd2[YOAUTH_PASSWD_SIZE];
	yoauth_handle_passwd("enter password again: ", passwd2, sizeof(passwd2));
	YOAUTH_OUTPUT("");

	if (strcmp(passwd1, passwd2) != 0) {
		YOAUTH_ERROR("The two entered passwords don't match");
		return false;
	}

	strncpy(handle->password, passwd1, sizeof(handle->password) - 1);

	char filepath[MUGGLE_MAX_PATH];
	if (!yoauth_path_user_file(handle->user, filepath, sizeof(filepath))) {
		YOAUTH_ERROR("failed get user data path");
		return false;
	}

	memcpy(handle->head.magic, YOAUTH_DATA_HEAD_MAGIC, 4);
	handle->head.ver = YOAUTH_DATA_VERSION;
	handle->head.data_type = YOAUTH_DATA_TYPE_TOTP;
	handle->head.data_size = sizeof(yoauth_totp_data_t);
	handle->head.cnt = 0;
	if (!yoauth_fileutils_store_totp(filepath, passwd1,
									 (unsigned char *)&handle->head,
									 sizeof(handle->head))) {
		YOAUTH_ERROR("failed create user data file");
		return false;
	}

	return true;
}

static bool yoauth_handle_load(yoauth_handle_t *handle)
{
	bool ret = true;

	// load files
	char filepath[MUGGLE_MAX_PATH];
	if (!yoauth_path_user_file(handle->user, filepath, sizeof(filepath))) {
		YOAUTH_ERROR("failed load user data");
		return false;
	}

	if (!muggle_path_exists(filepath)) {
		YOAUTH_ERROR("user file not exists");
		return false;
	}

	yoauth_head_t *hdr = NULL;
	unsigned long numbytes = 0;
	hdr = (yoauth_head_t *)yoauth_fileutils_load_totp(
		filepath, handle->password, &numbytes);
	if (hdr == NULL) {
		YOAUTH_ERROR("failed load user data, plz check your password");
		return false;
	}

	// head
	memcpy(&handle->head, hdr, sizeof(handle->head));

	// datas
	handle->datas = (muggle_array_list_t *)malloc(sizeof(muggle_array_list_t));
	if (handle->datas == NULL) {
		YOAUTH_ERROR("failed allocate memory space for datas");
		ret = false;
		goto init_err;
	}

	if (!muggle_array_list_init(handle->datas, hdr->cnt)) {
		free(handle->datas);
		handle->datas = NULL;
		YOAUTH_ERROR("failed init array list");
		ret = false;
		goto init_err;
	}

	yoauth_totp_data_t *datas = (yoauth_totp_data_t *)(hdr + 1);
	for (uint32_t i = 0; i < hdr->cnt; i++) {
		yoauth_totp_data_t *data =
			(yoauth_totp_data_t *)malloc(sizeof(yoauth_totp_data_t));
		memcpy(data, &datas[i], sizeof(*data));
		if (muggle_array_list_append(handle->datas, -1, data) == NULL) {
			free(data);
			YOAUTH_ERROR("failed append data");
			ret = false;
			break;
		}
	}

init_err:
	if (!ret) {
		yoauth_handle_destroy(handle);
	}

	free(hdr);

	return ret;
}

bool yoauth_handle_setup(yoauth_handle_t *handle, const char *user)
{
	memset(handle, 0, sizeof(*handle));
	strncpy(handle->user, user, sizeof(handle->user) - 1);

	char user_dir[MUGGLE_MAX_PATH];
	if (!yoauth_path_user_dir(handle->user, user_dir, sizeof(user_dir))) {
		YOAUTH_ERROR("failed get user dir");
		return false;
	}

	if (!muggle_path_exists(user_dir)) {
		if (!yoauth_handle_create_user(handle)) {
			YOAUTH_ERROR("failed create user: %s", handle->user);
			return false;
		}
		YOAUTH_OUTPUT("success create user['%s']", handle->user);

		if (!yoauth_handle_load(handle)) {
			YOAUTH_ERROR("failed load user['%s'] data", handle->user);
			return false;
		}
	} else {
		int cnt = 0;
		while (true) {
			yoauth_handle_passwd("enter password: ", handle->password,
								 sizeof(handle->password));
			if (yoauth_handle_load(handle)) {
				break;
			}
			if (++cnt == 3) {
				YOAUTH_ERROR("already try 3 times, exit");
				return false;
			}
		}
	}

	return true;
}

void yoauth_handle_destroy(yoauth_handle_t *handle)
{
	if (handle->datas) {
		int n = (int)muggle_array_list_size(handle->datas);
		for (int i = 0; i < n; i++) {
			muggle_array_list_node_t *node =
				muggle_array_list_index(handle->datas, i);
			if (node->data) {
				free(node->data);
			}
		}

		muggle_array_list_destroy(handle->datas, NULL, NULL);
		free(handle->datas);
	}
	handle->datas = NULL;
}

static char *yoauth_handle_datetime(time_t ts, char *buf, size_t bufsize)
{
	struct tm t;
	localtime_r(&ts, &t);
	snprintf(buf, bufsize, "%d-%02d-%02dT%02d:%02d:%02d", t.tm_year + 1900,
			 t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
	return buf;
}

void yoauth_handle_list(yoauth_handle_t *handle, const char *filter)
{
	time_t ts = time(NULL);
	size_t cnt = muggle_array_list_size(handle->datas);
	for (size_t i = 0; i < cnt; i++) {
		muggle_array_list_node_t *node =
			muggle_array_list_index(handle->datas, i);
		yoauth_totp_data_t *data = (yoauth_totp_data_t *)node->data;
		if (filter && filter[0] != '\0') {
			if (muggle_str_find(data->account, filter, 0, 0) == -1) {
				continue;
			}
		}
		int32_t code = yoauth_totp_at(data, ts);
		char v[16];
		snprintf(v, sizeof(v), "%06d", code);

		YOAUTH_OUTPUT_KV(data->account, v, 42);
	}

	time_t next_step_ts = ((time_t)(ts / 30 + 1) * 30);
	char curr_tm[64];
	yoauth_handle_datetime(ts, curr_tm, sizeof(curr_tm));

	YOAUTH_OUTPUT_SPLIT_LINE;
	time_t expired_sec = next_step_ts - ts;
	YOAUTH_TIP("current: %s\nexpire after %llu seconds", curr_tm,
			   (unsigned long long)expired_sec);
}

static int cmp_totp_data(const void *d1, const void *d2)
{
	yoauth_totp_data_t *p1 = (yoauth_totp_data_t *)d1;
	yoauth_totp_data_t *p2 = (yoauth_totp_data_t *)d2;
	return strcmp(p1->account, p2->account);
}

bool yoauth_handle_add(yoauth_handle_t *handle, const char *account,
					   const unsigned char *secret)
{
	unsigned char k[YOAUTH_TOTP_KEY_SIZE];
	int keylen = 0;

	keylen =
		base32_decode((unsigned char *)secret, (unsigned char *)k, sizeof(k));
	if (keylen < 0) {
		YOAUTH_ERROR("failed base32 decode");
		return false;
	}

	yoauth_totp_data_t *data = yoauth_totp_init(k, keylen, NULL);
	if (data == NULL) {
		YOAUTH_ERROR("failed init totp data");
		return false;
	}
	strncpy(data->account, account, sizeof(data->account) - 1);

	if (muggle_array_list_find(handle->datas, 0, data, cmp_totp_data) != -1) {
		YOAUTH_ERROR("account['%s'] already exists", data->account);
		free(data);
		return false;
	}

	if (muggle_array_list_append(handle->datas, -1, data) == NULL) {
		YOAUTH_ERROR("failed append totp data");
		return false;
	}

	handle->head.cnt++;

	return true;
}

bool yoauth_handle_del(yoauth_handle_t *handle, const char *account)
{
	unsigned char secret[64];
	memset(secret, 0, sizeof(secret));
	yoauth_totp_data_t *data = yoauth_totp_init(secret, 16, NULL);
	if (data == NULL) {
		YOAUTH_ERROR("failed init totp data");
	}
	strncpy(data->account, account, sizeof(data->account) - 1);

	int idx = muggle_array_list_find(handle->datas, 0, data, cmp_totp_data);
	free(data);

	if (idx != -1) {
		muggle_array_list_node_t *node =
			muggle_array_list_index(handle->datas, idx);
		free(node->data);
		muggle_array_list_remove(handle->datas, idx, NULL, NULL);
		YOAUTH_OUTPUT("success remove account['%s']", account);
	} else {
		YOAUTH_ERROR("account['%s'] not exists", account);
		return false;
	}

	handle->head.cnt--;

	return true;
}

bool yoauth_handle_store(yoauth_handle_t *handle)
{
	bool ret = true;
	unsigned char *plaintext = NULL;

	size_t cnt = muggle_array_list_size(handle->datas);
	unsigned long numbytes =
		sizeof(yoauth_head_t) + cnt * sizeof(yoauth_totp_data_t);
	plaintext = (unsigned char *)malloc(numbytes);
	if (plaintext == NULL) {
		YOAUTH_ERROR("failed allocate memory space for dump");
		ret = false;
		goto dumps_exit;
	}

	yoauth_head_t *hdr = (yoauth_head_t *)plaintext;
	memcpy(hdr, &handle->head, sizeof(*hdr));

	yoauth_totp_data_t *datas = (yoauth_totp_data_t *)(hdr + 1);
	for (size_t i = 0; i < cnt; i++) {
		muggle_array_list_node_t *node =
			muggle_array_list_index(handle->datas, i);
		memcpy(&datas[i], node->data, sizeof(datas[i]));
	}

	char user_path[MUGGLE_MAX_PATH];
	if (!yoauth_path_user_file(handle->user, user_path, sizeof(user_path))) {
		YOAUTH_ERROR("failed find user path");
		ret = false;
		goto dumps_exit;
	}
	yoauth_fileutils_store_totp(user_path, handle->password, plaintext,
								numbytes);

dumps_exit:
	if (plaintext) {
		free(plaintext);
	}

	return ret;
}

bool yoauth_handle_dump(yoauth_handle_t *handle, const char *filepath)
{
	char dirname[MUGGLE_MAX_PATH];
	if (muggle_path_dirname(filepath, dirname, sizeof(dirname)) != 0) {
		YOAUTH_ERROR("failed get dirname of %s", filepath);
		return false;
	}
	if (!muggle_path_exists(dirname)) {
		if (muggle_os_mkdir(dirname) != 0) {
			YOAUTH_ERROR("failed mkdir %s", dirname);
			return false;
		}
	}

	FILE *fp = NULL;
	fp = fopen(filepath, "wb");
	if (fp == NULL) {
		YOAUTH_ERROR("failed open %s for write", filepath);
		return false;
	}

	size_t cnt = muggle_array_list_size(handle->datas);
	for (size_t i = 0; i < cnt; i++) {
		muggle_array_list_node_t *node =
			muggle_array_list_index(handle->datas, i);
		yoauth_totp_data_t *data = (yoauth_totp_data_t *)node->data;
		char key[YOAUTH_TOTP_KEY_SIZE * 4 + 1];
		memset(key, 0, sizeof(key));
		if (base32_encode((unsigned char *)data->key, data->keylen,
						  (unsigned char *)key, sizeof(key)) == -1) {
			YOAUTH_ERROR("failed base32 encode key for account['%s']",
						 data->account);
			fclose(fp);
			return false;
		}

		fwrite(data->account, 1, strlen(data->account), fp);
		fwrite(",", 1, 1, fp);
		fwrite(key, 1, strlen(key), fp);
		fwrite("\n", 1, 1, fp);
	}

	if (fp) {
		fclose(fp);
	}

	YOAUTH_OUTPUT("success dump to: %s", filepath);

	return true;
}

bool yoauth_handle_load_csv(yoauth_handle_t *handle, const char *filepath)
{
	bool ret = true;

	FILE *fp = NULL;
	char *bytes = NULL;

	fp = fopen(filepath, "rb");
	if (fp == NULL) {
		YOAUTH_ERROR("failed open %s for read", filepath);
		ret = false;
		goto load_exit;
	}

	fseek(fp, 0, SEEK_END);
	long numbytes = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (numbytes == 0) {
		ret = true;
		goto load_exit;
	}

	bytes = (char *)malloc(numbytes + 1);
	if (bytes == NULL) {
		return false;
	}

	fread(bytes, 1, numbytes, fp);
	bytes[numbytes] = '\0';

	char *p = bytes;
	char *k = NULL;
	char *v = NULL;
	while (true) {
		k = p;
		while (*p != ',' && *p != '\0') {
			p++;
		}

		if (*p == '\0') {
			break;
		}

		*p = '\0';
		p++;

		v = p;
		while (*p != '\0' && *p != '\n') {
			p++;
		}

		if (*p == '\n') {
			*p = '\0';
			if (!yoauth_handle_add(handle, k, (unsigned char *)v)) {
				YOAUTH_ERROR("success add account '%s'", k);
				ret = false;
				break;
			} else {
				YOAUTH_OUTPUT_KV(k, v, 32);
			}
			p++;
		} else {
			if (!yoauth_handle_add(handle, k, (unsigned char *)v)) {
				YOAUTH_ERROR("failed add account '%s'", k);
				ret = false;
				break;
			} else {
				YOAUTH_OUTPUT("success add account '%s'", k);
			}
			break;
		}
	}

load_exit:
	if (bytes) {
		free(bytes);
		bytes = NULL;
	}

	if (fp) {
		fclose(fp);
		fp = NULL;
	}

	return ret;
}
