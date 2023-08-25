#include "fileutils.h"
#include "muggle/c/muggle_c.h"
#include "yoauth/core/crypt.h"

unsigned char *yoauth_fileutils_load_totp(const char *filepath,
										  const char *password,
										  unsigned long *outlen)
{
	FILE *fp = NULL;
	unsigned char *ciphertext = NULL;
	unsigned char *plaintext = NULL;

	fp = fopen(filepath, "rb");
	if (fp == NULL) {
		LOG_ERROR("failed open file: %s", filepath);
		goto load_exit;
	}

	fseek(fp, 0, SEEK_END);
	long numbytes = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	ciphertext = (unsigned char *)malloc(numbytes);
	if (ciphertext == NULL) {
		LOG_ERROR("failed allocate memory space for ciphertext");
		goto load_exit;
	}

	if (fread(ciphertext, 1, numbytes, fp) != (unsigned long)numbytes) {
		LOG_ERROR("failed read bytes from file");
		goto load_exit;
	}

	plaintext = yoauth_decrypt(ciphertext, numbytes, password, outlen);
	if (plaintext == NULL) {
		LOG_ERROR("failed decrypt TOTP file");
		goto load_exit;
	}

	yoauth_head_t *hdr = (yoauth_head_t *)plaintext;
	if (*(uint32_t *)hdr->magic != *(uint32_t *)YOAUTH_DATA_HEAD_MAGIC) {
		LOG_ERROR("file head field 'magic' is incorrect");
		free(plaintext);
		plaintext = NULL;
		goto load_exit;
	}
	if (*outlen != hdr->datalen + sizeof(yoauth_head_t)) {
		LOG_ERROR("file head 'datalen' is incorrect");
		free(plaintext);
		plaintext = NULL;
		goto load_exit;
	}
	if (hdr->datatype != YOAUTH_DATA_TYPE_TOTP) {
		LOG_ERROR("file head 'datatype' is incorrect");
		free(plaintext);
		plaintext = NULL;
		goto load_exit;
	}

load_exit:
	if (ciphertext) {
		free(ciphertext);
	}

	if (fp) {
		fclose(fp);
	}

	return plaintext;
}

bool yoauth_fileutils_store_totp(const char *filepath, const char *password,
								 unsigned char *plaintext,
								 unsigned long numbytes)
{
	unsigned char *ciphertext = NULL;
	FILE *fp = NULL;

	char dirpath[MUGGLE_MAX_PATH];
	if (muggle_path_dirname(filepath, dirpath, sizeof(dirpath)) != 0) {
		LOG_ERROR("failed get dir path: %s", filepath);
		goto err_store;
	}

	if (!muggle_path_exists(dirpath)) {
		if (muggle_os_mkdir(dirpath) != 0) {
			LOG_ERROR("failed create directory: %s", dirpath);
			goto err_store;
		}
	}

	unsigned long outlen = 0;
	ciphertext = yoauth_encrypt(plaintext, numbytes, password, &outlen);
	if (ciphertext == NULL) {
		LOG_ERROR("failed encrypt TOTP content");
		goto err_store;
	}

	char tmp_path[MUGGLE_MAX_PATH];
	memset(tmp_path, 0, sizeof(tmp_path));
	snprintf(tmp_path, sizeof(tmp_path), "%s.tmp", filepath);

	fp = fopen(tmp_path, "wb");
	if (fp == NULL) {
		LOG_ERROR("failed open file to write: %s", tmp_path);
		goto err_store;
	}

	if (fwrite(ciphertext, 1, outlen, fp) != outlen) {
		LOG_ERROR("failed write bytes");
		goto err_store;
	}

	fclose(fp);
	fp = NULL;

	if (ciphertext) {
		free(ciphertext);
		ciphertext = NULL;
	}

	if (muggle_os_rename(tmp_path, filepath) != 0) {
		LOG_ERROR("failed rename file");
		goto err_store;
	}

	return true;

err_store:
	if (fp) {
		fclose(fp);
	}

	if (ciphertext) {
		free(ciphertext);
	}

	return false;
}
