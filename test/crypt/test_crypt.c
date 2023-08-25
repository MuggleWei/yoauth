#include "unity.h"
#include "yoauth/core/core.h"
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
	int cnt_totp_data = 100;
	uint32_t total_bytes =
		sizeof(yoauth_head_t) + cnt_totp_data * sizeof(yoauth_totp_data_t);
	unsigned char *plaintext = (unsigned char *)malloc(total_bytes);

	// head
	yoauth_head_t *hdr = (yoauth_head_t *)plaintext;
	memset(hdr, 0, sizeof(*hdr));
	memcpy(hdr, YOAUTH_DATA_HEAD_MAGIC, 4);
	hdr->ver = YOAUTH_DATA_VERSION;
	hdr->data_type = YOAUTH_DATA_TYPE_TOTP;
	hdr->data_size = sizeof(yoauth_totp_data_t);
	hdr->cnt = cnt_totp_data;

	// data
	yoauth_totp_data_t *datas = (yoauth_totp_data_t *)(hdr + 1);
	for (int i = 0; i < cnt_totp_data; i++) {
		RAND_bytes((unsigned char *)&datas[i], sizeof(yoauth_totp_data_t));
	}

	// encrypt
	const char *password = "hello123666";
	unsigned long ciphertext_len = 0;
	unsigned char *ciphertext =
		yoauth_encrypt(plaintext, total_bytes, password, &ciphertext_len);
	TEST_ASSERT_NOT_NULL(ciphertext);

	// decrypt
	unsigned long plaintext_len = 0;
	unsigned char *ret_plaintext =
		yoauth_decrypt(ciphertext, ciphertext_len, password, &plaintext_len);
	TEST_ASSERT_NOT_NULL(ret_plaintext);
	TEST_ASSERT_EQUAL_UINT32(plaintext_len, total_bytes);
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
