/******************************************************************************
 *  @file         command_del.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-09-03
 *  @license      LGPL-3.0
 *  @brief        yoauth handle
 *****************************************************************************/

#ifndef YOAUTH_EXE_COMMAND_DEL_H_
#define YOAUTH_EXE_COMMAND_DEL_H_

#include "yoauth/core/core.h"

EXTERN_C_BEGIN

void yoauth_usage_command_del();

void yoauth_run_command_del(int argc, char **argv);

EXTERN_C_END

#endif // !YOAUTH_EXE_COMMAND_DEL_H_
