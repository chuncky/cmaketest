#ifndef _ASR_NOR_FLASH_H_
#define _ASR_NOR_FLASH_H_
#define FLASH_SECTOR_SIZE (0x00001000) //4*1024
#define FLASH_SECTOR_MASK (FLASH_SECTOR_SIZE-0x1) //4*1024 - 1
#define FLASH_BLOCK_SIZE (0x00010000) //64*1024
#define FLASH_BLOCK_MASK (FLASH_BLOCK_SIZE-0x1) //64*1024 - 1

int asr_norflash_erase_4k(unsigned int addr, unsigned int size);
int asr_norflash_eraseblock(unsigned int addr, unsigned int size);
int asr_norflash_read(uint32_t addr, uint8_t* buf_addr, uint32_t size);
int asr_norflash_write(uint32_t start, uint8_t* src, uint32_t size);
int asr_norflash_erasewrite(uint32_t start, uint8_t* src, uint32_t size);
int asr_norflash_erasewriteblock(uint32_t start, uint8_t* src, uint32_t size);
void erase_fota_pkg(void);
extern UINT32 CalcImageChecksum( UINT32* DownloadArea, UINT32 ImageLength,UINT32 checksum);
#define SPINOR_BASEADDR 0x90000000U
void clear_fota_magic(void);

#endif
