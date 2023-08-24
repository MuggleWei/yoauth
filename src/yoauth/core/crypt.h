/******************************************************************************
 *  @file         crypt.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-08-24
 *  @license      LGPL-3.0
 *  @brief        yoauth crtype
 *****************************************************************************/

#ifndef YOAUTH_CORE_CRYPT_H_
#define YOAUTH_CORE_CRYPT_H_

#include "yoauth/core/macro.h"
#include "yoauth/core/struct.h"

EXTERN_C_BEGIN

/**
 * @brief encrypt
 *
 * @param plaintext         input plaintext
 * @param plaintext_len     length of plaintext
 * @param password          password for encrypt
 * @param p_ciphertext_len  length of output ciphertext
 *
 * @return 
 *   - on success, return output ciphertext
 *   - on failed, return NULL
 */
YOAUTH_EXPORT
unsigned char *yoauth_encrypt(unsigned char *plaintext,
							  unsigned long plaintext_len, const char *password,
							  unsigned long *p_ciphertext_len);

/**
 * @brief decrypt
 *
 * @param ciphertext        input ciphertext
 * @param ciphertext_len    length of ciphertext
 * @param password          password for decrypt
 * @param p_plaintext_len   length of output plaintext
 *
 * @return 
 */
unsigned char *yoauth_decrypt(unsigned char *ciphertext,
							  unsigned long ciphertext_len,
							  const char *password,
							  unsigned long *p_plaintext_len);

EXTERN_C_END

#endif // !YOAUTH_CORE_CRYPT_H_
