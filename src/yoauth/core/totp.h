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

#define YOAUTH_TOTP_KEY_SIZE 64
#define YOAUTH_TOTP_DEFAULT_TIME_STEP 30

typedef struct yoauth_totp {
	char algo[16]; //!< crypto function to use
	char key[YOAUTH_TOTP_KEY_SIZE]; //!< secret key
	uint32_t keylen; //!< length of secret key
	uint32_t return_digits; //!< number of digits to return
	uint64_t time_step; //!< TOTP time step in seconds
} yoauth_totp_t;

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
yoauth_totp_t *yoauth_totp_init(const char *k, uint32_t klen,
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
int32_t yoauth_totp_at(yoauth_totp_t *totp, time_t ts);

/**
 * @brief generate current time TOTP code
 *
 * @param totp  TOTP object pointer
 *
 * @return 
 */
YOAUTH_EXPORT
int32_t yoauth_totp_now(yoauth_totp_t *totp);

/**
 * @brief 
 *
 * @param totp  TOTP object pointer
 */
YOAUTH_EXPORT
void yoauth_totp_destroy(yoauth_totp_t *totp);

EXTERN_C_END

#endif // !YOAUTH_TOTP_H_
