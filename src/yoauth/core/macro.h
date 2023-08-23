/******************************************************************************
 *  @file         macro.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-08-23
 *  @copyright    Copyright 2023 Muggle Wei
 *  @license      LGPL-3.0
 *  @brief        yoauth core macro
 *****************************************************************************/

#ifndef YOAUTH_CORE_MACRO_H_
#define YOAUTH_CORE_MACRO_H_

#include "muggle/c/base/macro.h"
#include "yoauth/config.h"

#if MUGGLE_PLATFORM_WINDOWS && defined(YOAUTH_USE_DLL)
	#ifdef YOAUTH_EXPORTS
		#define YOAUTH_EXPORT __declspec(dllexport)
	#else
		#define YOAUTH_EXPORT __declspec(dllimport)
	#endif
#else
	#define YOAUTH_EXPORT
#endif

#endif // !YOAUTH_MACRO_H_
