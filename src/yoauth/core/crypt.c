#include "crypt.h"
#include <string.h>
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
		goto err_gen;
	}

	*iv = malloc(*iv_len);
	if (*iv == NULL) {
		goto err_gen;
	}

	if (EVP_BytesToKey(evp_cipher, EVP_sha512(), salt,
					   (unsigned char *)password, strlen(password), 8, *key,
					   *iv) == 0) {
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

bool yoauth_encrypt(unsigned char *plaintext, unsigned long plaintext_len,
					unsigned char *ciphertext, unsigned long *p_ciphertext_len,
					const char *password)
{
	unsigned char *key = NULL;
	unsigned char *iv = NULL;
	unsigned long key_len = 0;
	unsigned long iv_len = 0;
	EVP_CIPHER_CTX *ctx = NULL;
	const EVP_CIPHER *evp_cipher = EVP_aes_256_cbc();

	if (!gen_key_iv_from_pass(evp_cipher, password, &key, &key_len, &iv,
							  &iv_len)) {
		goto err_encrypt;
	}

	ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL) {
		goto err_encrypt;
	}

	// TODO:

err_encrypt:
	if (ctx) {
		EVP_CIPHER_CTX_free(ctx);
	}

	if (iv) {
		free(iv);
	}

	if (key) {
		free(key);
	}

	return false;
}

bool yoauth_decrypt(unsigned char *ciphertext, unsigned long ciphertext_len,
					unsigned char *plaintext, unsigned long *p_plaintext_len,
					const char *password)
{
	return true;
}
