/******************************************************************************
 *  @file         struct.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-08-24
 *  @license      LGPL-3.0
 *  @brief        yoauth struct
 *****************************************************************************/

#ifndef YOAUTH_CORE_STRUCT_H_
#define YOAUTH_CORE_STRUCT_H_

#include "yoauth/core/macro.h"
#include <stdint.h>
#include <stdbool.h>

EXTERN_C_BEGIN

#define YOAUTH_DATA_VERSION 1
#define YOAUTH_DATA_HEAD_MAGIC "YOAU"

enum {
	YOAUTH_DATA_TYPE_NULL = 0,
	YOAUTH_DATA_TYPE_TOTP,
	MAX_YOAUTH_DATA_TYPE,
};

#pragma pack(push, 8)

typedef struct yoauth_head {
	char magic[4]; //!< magic word
	uint32_t ver; //!< data version
	uint32_t datalen; //!< data length
	uint32_t datatype; //!< data type
} yoauth_head_t;

typedef struct yoauth_data_totp {
	char account[16];
	char key[16];
} yoauth_data_totp_t;

#pragma pack(pop)

EXTERN_C_END

#endif // !YOAUTH_CORE_STRUCT_H_
