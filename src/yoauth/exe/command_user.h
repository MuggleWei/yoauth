/******************************************************************************
 *  @file         command_user.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-08-24
 *  @license      LGPL-3.0
 *  @brief        yoauth user command
 *****************************************************************************/

#ifndef YOAUTH_EXE_COMMAND_USER_H_
#define YOAUTH_EXE_COMMAND_USER_H_

#include "yoauth/exe/handle.h"

EXTERN_C_BEGIN

/**
 * @brief handle command: user
 *
 * @param handle  yoauth handle pointer
 *
 * @return  boolean value
 */
bool yoauth_cmd_user(yoauth_handle_t *handle);

/**
 * @brief show user command usage
 */
void yoauth_cmd_user_show_usage();

/**
 * @brief handle command: user login
 *
 * @param handle  yoauth handle pointer
 *
 * @return  boolean value
 */
bool yoauth_cmd_user_login(yoauth_handle_t *handle);

EXTERN_C_END

#endif // !YOAUTH_EXE_COMMAND_USER_H_
