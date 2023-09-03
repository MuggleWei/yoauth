/******************************************************************************
 *  @file         totp.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-08-23
 *  @license      LGPL-3.0
 *  @brief        yoauth core TOTP
 *****************************************************************************/

#ifndef YOAUTH_CORE_TOTP_H_
#define YOAUTH_CORE_TOTP_H_

#include "yoauth/core/macro.h"
#include <stdint.h>
#include <time.h>

EXTERN_C_BEGIN

#define YOAUTH_TOTP_ACCOUNT_SIZE 64

// accord to RFC 6238
// Seed for HMAC-SHA1 - 20 bytes
// Seed for HMAC-SHA256 - 32 bytes
// Seed for HMAC-SHA512 - 64 bytes
#define YOAUTH_TOTP_KEY_SIZE 64

#define YOAUTH_TOTP_ALGO_SIZE 16
#define YOAUTH_TOTP_DEFAULT_TIME_STEP 30

#pragma pack(push, 8)

typedef struct yoauth_totp_data {
	char account[YOAUTH_TOTP_ACCOUNT_SIZE]; //!< account
	char key[YOAUTH_TOTP_KEY_SIZE]; //!< secret key
	char algo[YOAUTH_TOTP_ALGO_SIZE]; //!< crypto function to use
	uint32_t keylen; //!< length of secret key
	uint32_t return_digits; //!< number of digits to return
	uint64_t time_step; //!< TOTP time step in seconds
} yoauth_totp_data_t;

#pragma pack(pop)

/**
 * @brief init TOTP object
 *
 * @param k       secret key
 * @param klen    length of secret key
 * @param crypto  crypto function name, if NULL, use SHA1
 *
 * @return  TOTP object pointer
 */
YOAUTH_EXPORT
yoauth_totp_data_t *yoauth_totp_init(const unsigned char *k, uint32_t klen,
									 const char *crypto);

/**
 * @brief generate TOTP code at the timestamp
 *
 * @param totp  TOTP object pointer
 * @param ts    timestamp in seconds
 *
 * @return TOTP code
 */
YOAUTH_EXPORT
int32_t yoauth_totp_at(yoauth_totp_data_t *totp, time_t ts);

/**
 * @brief generate current time TOTP code
 *
 * @param totp  TOTP object pointer
 *
 * @return 
 */
YOAUTH_EXPORT
int32_t yoauth_totp_now(yoauth_totp_data_t *totp);

/**
 * @brief 
 *
 * @param totp  TOTP object pointer
 */
YOAUTH_EXPORT
void yoauth_totp_destroy(yoauth_totp_data_t *totp);

EXTERN_C_END

#endif // !YOAUTH_TOTP_H_
