/******************************************************************************
 *  @file         scenes.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-08-25
 *  @license      LGPL-3.0
 *  @brief        yoauth scenes manager
 *****************************************************************************/

#ifndef YOAUTH_EXE_SCENES_H_
#define YOAUTH_EXE_SCENES_H_

#include "yoauth/core/core.h"
#include "handle.h"

EXTERN_C_BEGIN

typedef struct sys_args {
	char user[16];
	char password[32];
	char add_account[16];
	char del_account[16];
	char secret[16];
} sys_args_t;

/**
 * @brief message loop
 *
 * @param args  system input arguments
 */
void yoauth_loop(sys_args_t *args);

/**
 * @brief create new user
 *
 * @param args  system input arguments
 *
 * @return boolean value
 */
bool yoauth_create_user(sys_args_t *args);

/**
 * @brief main scenes
 *
 * @param handle   handle pointer
 * @param tui      tui pointer
 */
void yoauth_scenes_main(yoauth_handle_t *handle, yoauth_tui_t *tui);

EXTERN_C_END

#endif // !YOAUTH_EXE_SCENES_H_
