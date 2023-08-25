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
	yoauth_head_t head;
	muggle_array_list_t *datas;
} yoauth_handle_t;

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
 * @brief destroy handle
 *
 * @param handle  handle pointer
 */
void yoauth_handle_destroy(yoauth_handle_t *handle);

EXTERN_C_END

#endif // !YOAUTH_EXE_HANDLE_H_
