/******************************************************************************
 *  @file         path_manager.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-08-25
 *  @license      LGPL-3.0
 *  @brief        yoauth scenes manager
 *****************************************************************************/

#ifndef YOAUTH_EXE_PATH_MANAGER_H_
#define YOAUTH_EXE_PATH_MANAGER_H_

#include "yoauth/core/core.h"

EXTERN_C_BEGIN

/**
 * @brief get root directory
 *
 * @param buf      buffer store root directory
 * @param bufsize  buffer size
 *
 * @return boolean value
 */
bool yoauth_path_root_dir(char *buf, size_t bufsize);

/**
 * @brief get data directory
 *
 * @param buf      buffer store root directory
 * @param bufsize  buffer size
 *
 * @return boolean value
 */
bool yoauth_path_data_dir(char *buf, size_t bufsize);

/**
 * @brief get user directory
 *
 * @param buf      buffer store root directory
 * @param bufsize  buffer size
 *
 * @return boolean value
 */
bool yoauth_path_user_dir(const char *username, char *buf, size_t bufsize);

/**
 * @brief get user filepath
 *
 * @param buf      buffer store root directory
 * @param bufsize  buffer size
 *
 * @return boolean value
 */
bool yoauth_path_user_file(const char *username, char *buf, size_t bufsize);

EXTERN_C_END

#endif // !YOAUTH_EXE_PATH_MANAGER_H_
