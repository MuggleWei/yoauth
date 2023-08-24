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
 * @param plaintext
 * @param plaintext_len
 * @param ciphertext
 * @param p_ciphertext_len
 * @param password
 *
 * @return 
 */
bool yoauth_encrypt(unsigned char *plaintext, unsigned long plaintext_len,
					unsigned char *ciphertext, unsigned long *p_ciphertext_len,
					const char *password);

/**
 * @brief decrypt
 *
 * @param ciphertext
 * @param ciphertext_len
 * @param plaintext
 * @param p_plaintext_len
 * @param password
 *
 * @return 
 */
bool yoauth_decrypt(unsigned char *ciphertext, unsigned long ciphertext_len,
					unsigned char *plaintext, unsigned long *p_plaintext_len,
					const char *password);

EXTERN_C_END

#endif // !YOAUTH_CORE_CRYPT_H_
