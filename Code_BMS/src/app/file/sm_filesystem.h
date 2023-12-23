/*
 * sm_filesystem.h
 *
 *  Created on: Jul 12, 2023
 *      Author: Admin
 */

#ifndef CORES_FS_SM_FILESYSTEM_H_
#define CORES_FS_SM_FILESYSTEM_H_

#include <sm_fileconfig.h>
#include "lfs.h"
#include "lfs_util.h"

#include "exflash/exflash.h"
#include "hc05_hal.h"

typedef struct sm_file_sys
{
	w25qxx_t *base;
} sm_file_sys_t;

int sm_file_system_init(sm_file_sys_t *flash);

#endif /* CORES_FS_SM_FILESYSTEM_H_ */
