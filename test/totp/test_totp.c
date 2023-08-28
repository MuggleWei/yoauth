#include "unity.h"
#include "yoauth/core/core.h"
#include <string.h>

void setUp()
{
}

void tearDown()
{
}

void test_totp_sha1()
{
	const char *b32_key = "PFXWQZLMNRXXO33SNRSA";

	unsigned char key[64];
	int keylen = base32_decode((uint8_t*)b32_key, key, sizeof(key));

	yoauth_totp_data_t *totp = yoauth_totp_init(key, keylen, "SHA1");
	TEST_ASSERT_NOT_NULL(totp);

	time_t t = 1693241054;
	int32_t code = yoauth_totp_at(totp, t);
	TEST_ASSERT_EQUAL_INT32(code, 436932);

	yoauth_totp_destroy(totp);
}

int main()
{
	UNITY_BEGIN();

	RUN_TEST(test_totp_sha1);

	return UNITY_END();
}
