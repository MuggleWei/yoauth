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

/**
 * @brief message loop
 *
 * @param username  user name
 */
void yoauth_loop(const char *username);

/**
 * @brief create new user
 *
 * @param username user name
 *
 * @return boolean value
 */
bool yoauth_create_user(const char *username);

/**
 * @brief main scenes
 *
 * @param handle   handle pointer
 * @param tui      tui pointer
 */
void yoauth_scenes_main(yoauth_handle_t *handle, yoauth_tui_t *tui);

EXTERN_C_END

#endif // !YOAUTH_EXE_SCENES_H_
