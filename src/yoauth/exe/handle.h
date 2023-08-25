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

#include "yoauth/exe/tui.h"
#include "yoauth/core/core.h"
#include "muggle/c/muggle_c.h"

EXTERN_C_BEGIN

typedef struct yoauth_handle {
	const char *username;
	const char *password;
	muggle_avl_tree_t dict_totp;
} yoauth_handle_t;

/**
 * @brief message loop
 *
 * @param username  user name
 */
void yoauth_message_loop(const char *username);

/**
 * @brief show Usage information
 */
void yoauth_show_usage();

/**
 * @brief create new user
 *
 * @param username  user name
 *
 * @return boolean value
 */
bool yoauth_create_user(const char *username);

/**
 * @brief get user password
 *
 * @param buf      buffer store the password
 * @param bufsize  buffer size
 */
void yoauth_get_password(char *buf, size_t bufsize);

/**
 * @brief init handle
 *
 * @param handle     handle pointer
 * @param username   user name
 * @param password   password
 *
 * @return boolean value
 */
bool yoauth_handle_init(yoauth_handle_t *handle, const char *username,
						const char *password);

/**
 * @brief main scenes
 *
 * @param handle   handle pointer
 * @param tui      tui pointer
 */
void yoauth_scenes_main(yoauth_handle_t *handle, yoauth_tui_t *tui);

EXTERN_C_END

#endif // !YOAUTH_EXE_HANDLE_H_
