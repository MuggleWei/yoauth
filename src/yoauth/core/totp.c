#include "totp.h"
#include "muggle/c/muggle_c.h"
#include "openssl/evp.h"
#include "openssl/hmac.h"
#include "openssl/err.h"

yoauth_totp_t *yoauth_totp_init(const char *k, uint32_t klen,
								const char *crypto)
{
	yoauth_totp_t *totp = NULL;
	totp = (yoauth_totp_t *)malloc(sizeof(yoauth_totp_t));
	if (totp == NULL) {
		LOG_ERROR("failed allocate memory space for TOTP object");
		goto err_init;
	}

	memset(totp, 0, sizeof(*totp));

	if (crypto == NULL) {
		strncpy(totp->algo, "SHA1", sizeof(totp->algo) - 1);
	} else if (strcmp(crypto, "SHA1") == 0 || strcmp(crypto, "SHA256") == 0 ||
			   strcmp(crypto, "SHA512") == 0) {
		strncpy(totp->algo, crypto, sizeof(totp->algo) - 1);
	} else {
		LOG_ERROR(
			"unsupport crypto algorithm for TOTP, support SHA1/SHA256/SHA512");
		goto err_init;
	}

	if (klen > sizeof(totp->key)) {
		LOG_ERROR("length of key beyond the limit, input key len: %u", klen);
		goto err_init;
	}
	memcpy(totp->key, k, klen);
	totp->keylen = klen;

	totp->return_digits = 6;
	totp->time_step = 30;

	return totp;

err_init:
	if (totp) {
		free(totp);
		totp = NULL;
	}

	return totp;
}

int32_t yoauth_totp_at(yoauth_totp_t *totp, time_t ts)
{
	EVP_MD_CTX *ctx = NULL;
	EVP_MD *algo = NULL;
	unsigned int outlen = 0;
	unsigned char *digest = NULL;
	uint64_t t = 0;

	uint32_t offset = 0;
	int32_t result = -1;

	ctx = EVP_MD_CTX_new();
	if (ctx == NULL) {
		LOG_ERROR("EVP_MD_CTX_new failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto clean_totp_at;
	}
	algo = EVP_MD_fetch(NULL, totp->algo, NULL);
	if (algo == NULL) {
		LOG_ERROR("EVP_MD_fetch %s failed: %s", totp->algo,
				  ERR_reason_error_string(ERR_get_error()));
		goto clean_totp_at;
	}

	outlen = EVP_MD_get_size(algo);
	digest = (unsigned char *)malloc(outlen);
	if (digest == NULL) {
		LOG_ERROR("failed allocate memory space for digest");
		goto clean_totp_at;
	}

	t = (uint64_t)((double)ts / totp->time_step);
	if (MUGGLE_ENDIANNESS == MUGGLE_LITTLE_ENDIAN) {
		t = MUGGLE_ENDIAN_SWAP_64(t);
	}

	if (HMAC(algo, totp->key, totp->keylen, (unsigned char *)&t, sizeof(t),
			 digest, &outlen) == NULL) {
		LOG_ERROR("failed TOTP hmac: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto clean_totp_at;
	}

	// dynamically truncates hash
	offset = digest[outlen - 1] & 0x0f;
	result = (digest[offset] & 0x7f) << 24 | (digest[offset + 1] & 0xff) << 16 |
			 (digest[offset + 2] & 0xff) << 8 | (digest[offset + 3] & 0xff);
	result %= (int32_t)(pow(10, totp->return_digits));

clean_totp_at:
	if (digest) {
		free(digest);
	}

	if (algo) {
		EVP_MD_free(algo);
	}

	if (ctx) {
		EVP_MD_CTX_free(ctx);
	}

	return result;
}

int32_t yoauth_totp_now(yoauth_totp_t *totp)
{
	return yoauth_totp_at(totp, time(NULL));
}

void yoauth_totp_destroy(yoauth_totp_t *totp)
{
	if (totp) {
		free(totp);
	}
}
