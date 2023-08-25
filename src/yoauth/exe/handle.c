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

	yoauth_totp_data_t *datas = (yoauth_totp_data_t*)(hdr + 1);
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

