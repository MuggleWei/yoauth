/******************************************************************************
 *  @file         handle.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-08-24
 *  @license      LGPL-3.0
 *  @brief        yoauth handle
 *****************************************************************************/

#ifndef YOAUTH_EXE_HANDLE_H_
#define YOAUTH_EXE_HANDLE_H_

#include "yoauth/core/core.h"
#include "muggle/c/muggle_c.h"

EXTERN_C_BEGIN

#define YOAUTH_USER_SIZE 256
#define YOAUTH_PASSWD_SIZE 64

typedef struct yoauth_handle {
	char user[YOAUTH_USER_SIZE];
	char password[YOAUTH_PASSWD_SIZE];
	yoauth_head_t head;
	muggle_array_list_t *datas;
} yoauth_handle_t;

/**
 * @brief setup handle
 *
 * @param handle  handle pointer
 * @param user    user name
 *
 * @return boolean value
 */
bool yoauth_handle_setup(yoauth_handle_t *handle, const char *user);

/**
 * @brief destroy handle
 *
 * @param handle  handle pointer
 */
void yoauth_handle_destroy(yoauth_handle_t *handle);

/**
 * @brief list TOTP codes
 *
 * @param handle  handle pointer
 * @param filter  filter
 */
void yoauth_handle_list(yoauth_handle_t *handle, const char *filter);

/**
 * @brief add account
 *
 * @param handle     handle pointer
 * @param account    new account
 * @param secret     secret key in base32 format
 *
 * @return boolean value
 */
bool yoauth_handle_add(yoauth_handle_t *handle, const char *account,
					   const unsigned char *secret);

/**
 * @brief delete account
 *
 * @param handle     handle pointer
 * @param account    secret key
 *
 * @return boolean value
 */
bool yoauth_handle_del(yoauth_handle_t *handle, const char *account);

/**
 * @brief store to file
 *
 * @param handle  handle pointer
 *
 * @return 
 */
bool yoauth_handle_store(yoauth_handle_t *handle);

/**
 * @brief dump to file
 *
 * @param handle    handle pointer
 * @param filepath  filepath
 *
 * @return 
 */
bool yoauth_handle_dump(yoauth_handle_t *handle, const char *filepath);

/**
 * @brief load from file
 *
 * @param handle    handle pointer
 * @param filepath  filepath
 *
 * @return 
 */
bool yoauth_handle_load_csv(yoauth_handle_t *handle, const char *filepath);

EXTERN_C_END

#endif // !YOAUTH_EXE_HANDLE_H_
