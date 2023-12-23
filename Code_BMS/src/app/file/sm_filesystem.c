/*
 * sm_filesystem.c
 *
 *  Created on: Jul 12, 2023
 *      Author: Admin
 */

#include <sm_filesystem.h>

lfs_t SM_STDIO_LITTLEFS_CFG_LFS;

int sm_littlefs_flash_read (const struct lfs_config * c,
                            lfs_block_t               block,
                            lfs_off_t                 off,
                            void                    * buffer,
                            lfs_size_t                size);
int sm_littlefs_flash_write (const struct lfs_config * c,
                             lfs_block_t               block,
                             lfs_off_t                 off,
                             const void              * buffer,
                             lfs_size_t                size);
int sm_littlefs_flash_erase (const struct lfs_config * c, lfs_block_t block);
int sm_littlefs_flash_sync (const struct lfs_config * c);

struct lfs_config sm_lfs_config = {
                                      .read = &sm_littlefs_flash_read,
                                      .prog = &sm_littlefs_flash_write,
                                      .erase = &sm_littlefs_flash_erase,
                                      .sync = &sm_littlefs_flash_sync,
                                      .block_cycles  = 1024,
                                      .cache_size = 64,
                                      .lookahead_size = 16,
                                      .read_size = 1,
                                      .prog_size = 1,
                                      .name_max = 24,

};

int sm_file_system_init(sm_file_sys_t *flash)
{
	flash->base = &exflash;
	sm_lfs_config.block_count = flash->base->SectorCount;
	sm_lfs_config.block_size = flash->base->SectorSize;
	sm_lfs_config.context = (void*) flash->base;
//    SM_STDIO_LITTLEFS_CFG_LFS.cfg = &sm_lfs_config;

    int lfs_err = lfs_mount(&SM_STDIO_LITTLEFS_CFG_LFS, &sm_lfs_config);
    if(lfs_err != LFS_ERR_OK){
        lfs_err = lfs_format(&SM_STDIO_LITTLEFS_CFG_LFS, &sm_lfs_config);
        lfs_err = lfs_mount(&SM_STDIO_LITTLEFS_CFG_LFS, &sm_lfs_config);
		lfs_err = lfs_mkdir(&SM_STDIO_LITTLEFS_CFG_LFS, "SBP:/");
    }
    return LFS_ERR_OK;
}

int sm_littlefs_flash_read (const struct lfs_config * c,
                            lfs_block_t               block,
                            lfs_off_t                 off,
                            void                    * buffer,
                            lfs_size_t                size){
    w25qxx_t *flash = (w25qxx_t*) c->context;
    W25qxx_ReadSector(flash, buffer, block, off, size);
    return LFS_ERR_OK;
}
int sm_littlefs_flash_write (const struct lfs_config * c,
                             lfs_block_t               block,
                             lfs_off_t                 off,
                             const void              * buffer,
                             lfs_size_t                size){
    w25qxx_t *flash = (w25qxx_t*) c->context;
    W25qxx_WriteSector(flash,(uint8_t*)buffer , block, off, size);
    return LFS_ERR_OK;
}
int sm_littlefs_flash_erase (const struct lfs_config * c, lfs_block_t block){
    w25qxx_t *flash = (w25qxx_t*) c->context;
    W25qxx_EraseSector(flash, block);
    return LFS_ERR_OK;
}
int sm_littlefs_flash_sync (const struct lfs_config * c){
    (void)(c);
    return LFS_ERR_OK;
}
