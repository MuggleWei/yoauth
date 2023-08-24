#include "unity.h"
#include "yoauth/core/crypt.h"
#include "openssl/evp.h"
#include "openssl/rand.h"
#include "muggle/c/muggle_c.h"

void setUp()
{
}

void tearDown()
{
}

void test_crypt()
{
	const char *password = "hello123666";

	int cnt_totp_data = 100;
	unsigned char *plaintext = (unsigned char *)malloc(
		sizeof(yoauth_head_t) + cnt_totp_data * sizeof(yoauth_data_totp_t));

	// head
	yoauth_head_t *hdr = (yoauth_head_t *)plaintext;
	memcpy(hdr, YOAUTH_DATA_HEAD_MAGIC, 4);
	hdr->ver = YOAUTH_DATA_VERSION;
	hdr->datalen = cnt_totp_data * sizeof(yoauth_data_totp_t);
	hdr->datatype = YOAUTH_DATA_TYPE_TOTP;

	// data
	yoauth_data_totp_t *datas = (yoauth_data_totp_t *)(hdr + 1);
	for (int i = 0; i < cnt_totp_data; i++) {
		RAND_bytes((unsigned char *)&datas[i], sizeof(yoauth_data_totp_t));
	}

	// encrypt
	unsigned long ciphertext_len = 0;
	unsigned char *ciphertext =
		yoauth_encrypt(plaintext, sizeof(yoauth_head_t) + hdr->datalen,
					   password, &ciphertext_len);
	TEST_ASSERT_NOT_NULL(ciphertext);

	// decrypt
	unsigned long plaintext_len = 0;
	unsigned char *ret_plaintext =
		yoauth_decrypt(ciphertext, ciphertext_len, password, &plaintext_len);
	TEST_ASSERT_NOT_NULL(ret_plaintext);
	TEST_ASSERT_EQUAL_UINT32(plaintext_len,
							 sizeof(yoauth_head_t) + hdr->datalen);
	int n = memcmp(ret_plaintext, plaintext, plaintext_len);
	TEST_ASSERT_EQUAL_INT(n, 0);

	// free
	free(plaintext);
	free(ciphertext);
	free(ret_plaintext);
}

int main()
{
	UNITY_BEGIN();

	RUN_TEST(test_crypt);

	return UNITY_END();
}
