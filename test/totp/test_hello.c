#include "unity.h"
#include "yoauth/core/totp.h"
#include <string.h>

void setUp()
{
}

void tearDown()
{
}

void test_totp_sha1()
{
	const char *key = "BNsXVvBuaYMxt2b7";

	yoauth_totp_data_t *totp = yoauth_totp_init(key, strlen(key), "SHA1");
	TEST_ASSERT_NOT_NULL(totp);

	int32_t code = yoauth_totp_at(totp, 1692770521);
	TEST_ASSERT_EQUAL_INT32(code, 455377);

	yoauth_totp_destroy(totp);
}

int main()
{
	UNITY_BEGIN();

	RUN_TEST(test_totp_sha1);

	return UNITY_END();
}
