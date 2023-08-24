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

void test_fileutils()
{
	// TODO:
}

int main()
{
	UNITY_BEGIN();

	RUN_TEST(test_fileutils);

	return UNITY_END();
}

