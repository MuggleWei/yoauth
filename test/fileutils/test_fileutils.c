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

void test_fileutils()
{
	int iret = 0;

	// prepare filepath
	char exepath[MUGGLE_MAX_PATH];
	iret = muggle_os_process_path(exepath, sizeof(exepath));
	TEST_ASSERT_EQUAL_INT32(iret, 0);

	char dirpath[MUGGLE_MAX_PATH];
	iret = muggle_path_dirname(exepath, dirpath, sizeof(dirpath));
	TEST_ASSERT_EQUAL_INT32(iret, 0);

	char filepath[MUGGLE_MAX_PATH];
	iret = muggle_path_join(dirpath, "test_fileutils.data", filepath,
							sizeof(filepath));
	TEST_ASSERT_EQUAL_INT32(iret, 0);

	// prepare data
	int cnt_totp_data = 100;
	uint32_t total_bytes =
		sizeof(yoauth_head_t) + cnt_totp_data * sizeof(yoauth_totp_data_t);
	unsigned char *plaintext = (unsigned char *)malloc(total_bytes);
	TEST_ASSERT_NOT_NULL(plaintext);

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

	// write into file
	const char *password = "hello123666";
	bool bret =
		yoauth_fileutils_store_totp(filepath, password, plaintext, total_bytes);
	TEST_ASSERT_TRUE(bret);

	unsigned long plaintext_len = 0;
	unsigned char *ret_plaintext =
		yoauth_fileutils_load_totp(filepath, password, &plaintext_len);
	TEST_ASSERT_NOT_NULL(ret_plaintext);
	TEST_ASSERT_EQUAL_UINT32(plaintext_len, total_bytes);
	int n = memcmp(ret_plaintext, plaintext, plaintext_len);
	TEST_ASSERT_EQUAL_INT(n, 0);

	// cleanup
	free(ret_plaintext);
	free(plaintext);
}

int main()
{
	UNITY_BEGIN();

	RUN_TEST(test_fileutils);

	return UNITY_END();
}
