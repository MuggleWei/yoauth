#include "crypt.h"
#include "muggle/c/muggle_c.h"
#include "openssl/err.h"
#include "openssl/evp.h"

static bool gen_key_iv_from_pass(const EVP_CIPHER *evp_cipher,
								 const char *password, unsigned char **key,
								 unsigned long *key_len, unsigned char **iv,
								 unsigned long *iv_len)
{
	static const unsigned char *salt = (const unsigned char *)"$yoauth#";
	*key_len = EVP_CIPHER_get_key_length(EVP_aes_256_cbc());
	*iv_len = EVP_CIPHER_get_iv_length(EVP_aes_256_cbc());

	*key = malloc(*key_len);
	if (*key == NULL) {
		LOG_ERROR("failed allocate memory space for key");
		goto err_gen;
	}

	*iv = malloc(*iv_len);
	if (*iv == NULL) {
		LOG_ERROR("failed allocate memory space for iv");
		goto err_gen;
	}

	if (EVP_BytesToKey(evp_cipher, EVP_sha512(), salt,
					   (unsigned char *)password, strlen(password), 8, *key,
					   *iv) == 0) {
		LOG_ERROR("failed EVP_BytesToKey: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto err_gen;
	}

	return true;

err_gen:
	if (*key) {
		free(*key);
		*key = NULL;
	}
	if (*iv) {
		free(*iv);
		*iv = NULL;
	}

	return false;
}

unsigned char *yoauth_encrypt(unsigned char *plaintext,
							  unsigned long plaintext_len, const char *password,
							  unsigned long *p_ciphertext_len)
{
	unsigned char *key = NULL;
	unsigned char *iv = NULL;
	unsigned long key_len = 0;
	unsigned long iv_len = 0;
	EVP_CIPHER_CTX *ctx = NULL;
	const EVP_CIPHER *cipher = EVP_aes_256_cbc();
	unsigned char *ciphertext = NULL;

	if (!gen_key_iv_from_pass(cipher, password, &key, &key_len, &iv, &iv_len)) {
		LOG_ERROR("failed generate key & iv from password");
		goto err_encrypt;
	}

	ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL) {
		LOG_ERROR("failed new cipher ctx");
		goto err_encrypt;
	}

	if (EVP_CipherInit_ex2(ctx, cipher, key, iv, 1, NULL) == 0) {
		LOG_ERROR("failed cipher init: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto err_encrypt;
	}

	int outlen = plaintext_len + EVP_MAX_BLOCK_LENGTH;
	ciphertext = (unsigned char *)malloc(outlen);
	if (ciphertext == NULL) {
		LOG_ERROR("failed allocate memory space for ciphertext");
		goto err_encrypt;
	}

	outlen = 0;
	*p_ciphertext_len = 0;
	if (!EVP_CipherUpdate(ctx, ciphertext, &outlen, plaintext, plaintext_len)) {
		LOG_ERROR("failed EVP_CipherUpdate: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto err_encrypt;
	}
	*p_ciphertext_len += outlen;

	if (!EVP_CipherFinal_ex(ctx, ciphertext + *p_ciphertext_len, &outlen)) {
		LOG_ERROR("failed EVP_CipherFinal_ex: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto err_encrypt;
	}
	*p_ciphertext_len += outlen;

	return ciphertext;

err_encrypt:
	if (ciphertext) {
		free(ciphertext);
	}

	if (ctx) {
		EVP_CIPHER_CTX_free(ctx);
	}

	if (iv) {
		free(iv);
	}

	if (key) {
		free(key);
	}

	return NULL;
}

unsigned char *yoauth_decrypt(unsigned char *ciphertext,
							  unsigned long ciphertext_len,
							  const char *password,
							  unsigned long *p_plaintext_len)
{
	unsigned char *key = NULL;
	unsigned char *iv = NULL;
	unsigned long key_len = 0;
	unsigned long iv_len = 0;
	EVP_CIPHER_CTX *ctx = NULL;
	const EVP_CIPHER *cipher = EVP_aes_256_cbc();
	unsigned char *plaintext = NULL;

	if (!gen_key_iv_from_pass(cipher, password, &key, &key_len, &iv, &iv_len)) {
		LOG_ERROR("failed generate key & iv from password");
		goto err_decrypt;
	}

	ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL) {
		LOG_ERROR("failed EVP_CIPHER_CTX_new: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto err_decrypt;
	}

	if (EVP_CipherInit_ex2(ctx, cipher, key, iv, 0, NULL) == 0) {
		LOG_ERROR("failed EVP_CipherInit_ex2: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto err_decrypt;
	}

	int outlen = ciphertext_len;
	plaintext = (unsigned char *)malloc(outlen);
	if (plaintext == NULL) {
		LOG_ERROR("failed allocate memory space for plaintext");
		goto err_decrypt;
	}

	outlen = 0;
	*p_plaintext_len = 0;
	if (!EVP_CipherUpdate(ctx, plaintext, &outlen,
						  (const unsigned char *)ciphertext, ciphertext_len)) {
		LOG_ERROR("failed EVP_CipherUpdate: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto err_decrypt;
	}
	*p_plaintext_len += outlen;

	if (!EVP_CipherFinal_ex(ctx, plaintext + *p_plaintext_len, &outlen)) {
		LOG_ERROR("failed EVP_CipherFinal_ex: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto err_decrypt;
	}
	*p_plaintext_len += outlen;

	return plaintext;

err_decrypt:
	if (plaintext) {
		free(plaintext);
	}

	if (ctx) {
		EVP_CIPHER_CTX_free(ctx);
	}

	if (iv) {
		free(iv);
	}

	if (key) {
		free(key);
	}

	return NULL;
}
