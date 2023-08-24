/******************************************************************************
 *  @file         fileutils.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-08-24
 *  @license      LGPL-3.0
 *  @brief        yoauth file utils
 *****************************************************************************/

#ifndef YOAUTH_CORE_FILEUTILS_H_
#define YOAUTH_CORE_FILEUTILS_H_

#include "yoauth/core/macro.h"
#include <stdbool.h>

EXTERN_C_BEGIN

/**
 * @brief load TOTP file
 *
 * @param filepath  file path of TOTP
 * @param password  user password
 * @param outlen    number of bytes
 *
 * @return plaintext of yoauth_head_t + yoauth_data_totp_t array
 */
YOAUTH_EXPORT
unsigned char *yoauth_fileutils_load_totp(const char *filepath,
										  const char *password,
										  unsigned long *outlen);

/**
 * @brief store TOTP into file
 *
 * @param filepath   file path
 * @param password   user password
 * @param plaintext  plaintext
 * @param numbytes   number of bytes
 *
 * @return 
 */
YOAUTH_EXPORT
bool yoauth_fileutils_store_totp(const char *filepath, const char *password,
								 unsigned char *plaintext,
								 unsigned long numbytes);

EXTERN_C_END

#endif // !YOAUTH_CORE_FILEUTILS_H_
