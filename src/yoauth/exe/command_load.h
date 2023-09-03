/******************************************************************************
 *  @file         command_load.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-09-03
 *  @license      LGPL-3.0
 *  @brief        yoauth handle
 *****************************************************************************/

#ifndef YOAUTH_EXE_COMMAND_LOAD_H_
#define YOAUTH_EXE_COMMAND_LOAD_H_

#include "yoauth/core/core.h"

EXTERN_C_BEGIN

void yoauth_usage_command_load();

void yoauth_run_command_load(int argc, char **argv);

EXTERN_C_END

#endif // !YOAUTH_EXE_COMMAND_LOAD_H_
