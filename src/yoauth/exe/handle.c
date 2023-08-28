#include "handle.h"
#include "path_manager.h"

bool yoauth_handle_init(yoauth_handle_t *handle, const char *username,
						const char *password)
{
	bool ret = true;

	memset(handle, 0, sizeof(*handle));
	handle->username = username;
	handle->password = password;

	// load files
	char filepath[MUGGLE_MAX_PATH];
	if (!yoauth_path_user_file(username, filepath, sizeof(filepath))) {
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

static int cmp_totp_data(const void *d1, const void *d2)
{
	yoauth_totp_data_t *p1 = (yoauth_totp_data_t *)d1;
	yoauth_totp_data_t *p2 = (yoauth_totp_data_t *)d2;
	return strcmp(p1->account, p2->account);
}

bool yoauth_handle_add(yoauth_handle_t *handle, const char *account,
					   const unsigned char *secret, int keylen)
{
	yoauth_totp_data_t *data = yoauth_totp_init(secret, keylen, NULL);
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

bool yoauth_handle_dumps(yoauth_handle_t *handle)
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
	if (!yoauth_path_user_file(handle->username, user_path,
							   sizeof(user_path))) {
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
