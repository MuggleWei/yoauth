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
	// TODO:
	return true;
}
