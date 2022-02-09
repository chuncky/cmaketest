/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "sdio.h"
//#include "pmic_rtc.h"
#define fatal_printf uart_printf

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */
#define DEV_FLASH		3	/* Example: Map FLASH MSD to physical drive 3 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;
    //uart_printf("%s %d\r\n", __func__, pdrv);
	switch (pdrv) {
	case DEV_RAM :
		//result = RAM_disk_status();

		// translate the reslut code here

		return stat;

	case DEV_MMC :
		if (sdcard_is_ready() == TRUE)
			return 0;

		return STA_NODISK;

	case DEV_USB :
		//result = USB_disk_status();

		// translate the reslut code here

		return stat;
	case DEV_FLASH :
		return 0;
		
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = 0;
	int result;
    //uart_printf("%s %d\r\n", __func__, pdrv);
	switch (pdrv) {
	case DEV_RAM :
		//result = RAM_disk_initialize();

		// translate the reslut code here

		return stat;

	case DEV_MMC :
		//result = MMC_disk_initialize();

		// translate the reslut code here

		return stat;

	case DEV_USB :
		//result = USB_disk_initialize();

		// translate the reslut code here

		return stat;
	case DEV_FLASH:
		return stat;
		
	}
	return STA_NOINIT;
}

#pragma arm section rwdata="SDBuf", zidata="SDBuf"
//UINT8 __align(8) back_buf[SINGLE_BLOCK_SIZE];
UINT8 __align(8) align_buf[64*1024];
#pragma arm section rwdata, zidata
static UINT8 sd_full_status = 0;


int MMC_disk_read(char *buff, DWORD sector, UINT count)
{
    UINT base = SD1_HOST_CTRL_ADDR;
	UINT ret = 0;
    if (count == 0)
		return 0;
    if (sector > sdcard_get_blknum() || (sector + count) > sdcard_get_blknum()) {
		fatal_printf("sdcard read err\r\n");
		sd_full_status = 1;
		return -1;
    }
	if(eeGetSystemAssertFlag()==TRUE)
	{
			if(count == 1)
				ret = sdcard_read_singleblk(base, align_buf, sector);
			else
				ret = sdcard_read_multiblks(base, align_buf, sector, count);
			memcpy(buff, align_buf, SINGLE_BLOCK_SIZE*count);
	}else
	{
			if(count == 1)
				ret = sdcard_read_singleblk(base, buff,sector);
			else
				ret = sdcard_read_multiblks(base, buff, sector, count);
	}
	return ret;
}
#if 0
extern unsigned int qspi_nvm_start_block; //16*5 16K*64=1024K
extern unsigned int qspi_nvm_total_size;
extern unsigned int qspi_nvm_block_size;

int flash_disk_read(char *buff, DWORD sector, UINT count)
{
    unsigned int start_address = qspi_nvm_start_block * qspi_nvm_block_size + qspi_nvm_total_size;
    unsigned int addr = sector * 512 + start_address;
    //int ret = spi_nor_do_erase_4k(addr,count * 512);
	int ret  = spi_nor_do_read(addr, buff,count * 512);
    return ret;
}
#endif

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	int result;
    UINT base = SD1_HOST_CTRL_ADDR;
	//uart_printf("%s %d\r\n", __func__, pdrv);
	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

		//result = RAM_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_MMC :
		// translate the arguments here

		result = MMC_disk_read(buff, sector, count);
        if (result != 0)
			return RES_ERROR;
		// translate the reslut code here

		return RES_OK;

	case DEV_USB :
		// translate the arguments here

		//result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;
	#if 0
	case DEV_FLASH:
		result = flash_disk_read(buff, sector, count);
		if (result != 0)
			return RES_ERROR;
		return RES_OK;
	#endif
	default:
		return RES_PARERR;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

UINT MMC_disk_write(void *buff, DWORD sector, UINT count)
{
    UINT base = SD1_HOST_CTRL_ADDR;
	UINT ret = 0;
	//char *p = buff;
	//uart_printf("SDCARD :%s:%d :%d:%d\n", __FILE__, __LINE__, sector, count);
	if(count == 0)
		return 0;
	//uart_printf("SDCARD :%s:%d\n", __FILE__, __LINE__);
    if (sector > sdcard_get_blknum() || (sector + count) > sdcard_get_blknum()) {
		fatal_printf("sdcard write err\r\n");
		sd_full_status = 1;
		return -1;
    }
	//fatal_printf("diskWrite: %02x, %02x, %02x, %02x\n", *p, *(p+1), *(p+2), *(p+3));
	if(eeGetSystemAssertFlag()==TRUE)
	{
		memcpy(align_buf, buff, SINGLE_BLOCK_SIZE * count);
		if(count == 1)
			ret =  sdcard_write_singleblk(base, align_buf, sector);
		else
			ret = sdcard_write_multiblks(base, align_buf, sector, count);
	}
	else
	{
		if(count == 1)
			ret = sdcard_write_singleblk(base, buff, sector);
		else
			ret = sdcard_write_multiblks(base, buff, sector, count);
	}

	return ret;

}
#if 0
static char flash_disk_read_buf[8 * 512];
int flash_disk_write(char *buff, DWORD sector, UINT count)
{
    unsigned int start_address = qspi_nvm_start_block * qspi_nvm_block_size + qspi_nvm_total_size;
    unsigned int addr = sector * 512 + start_address;
	int count_4k = count/8;
	int count_last = count%8;
	int ret;
	if (sector%8 != 0) {
		addr = addr - sector%8 * 512;
		ret = spi_nor_do_read(addr, flash_disk_read_buf, 8* 512);
		if (ret != 0) {
			return ret;
		}
		memcpy(flash_disk_read_buf+sector%8*512, buff, 8*512 - sector%8*512);
		ret = spi_nor_do_erase_4k(addr,8 * 512);
		if (ret != 0)
			return ret;
		ret = spi_nor_do_write(addr, flash_disk_read_buf, 8 * 512);
		if (ret != 0) 
			return ret;
		if((sector+count) * 512 <= (addr+8* 512))
			return 0;
		buff += (8*512- sector%8*512);
		count -= (8 - sector % 8);
		sector += (8 - sector % 8);
		count_4k = count/8;
		count_last = count%8;
		addr += (8*512- sector%8*512);
	}
	if (count_4k != 0) {
        ret = spi_nor_do_erase_4k(addr,count_4k * 8 * 512);
	    if (ret != 0 ) {
		    return ret;
	    }
	    ret  = spi_nor_do_write(addr, buff, count_4k * 8 * 512);
		if (ret != 0)
			return ret;
	}
	addr = addr + count_4k * 8 * 512;
	if (count_last != 0) {
		//read out
		ret = spi_nor_do_read(addr, flash_disk_read_buf, 8* 512);
		if (ret != 0) {
			return ret;
		}
		memcpy(flash_disk_read_buf, buff + count_4k * 8 * 512, count_last * 512);
		ret = spi_nor_do_erase_4k(addr,8 * 512);
		if (ret != 0)
			return ret;
		ret = spi_nor_do_write(addr, flash_disk_read_buf, 8 * 512);
	}
    return ret;
}
#endif

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;
    //uart_printf("%s %d\r\n", __func__, pdrv);
	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

		//result = RAM_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_MMC :
		// translate the arguments here

		result = MMC_disk_write(buff, sector, count);

		// translate the reslut code here
        if (result != 0)
			return RES_ERROR;
		return RES_OK;

	case DEV_USB :
		// translate the arguments here

		//result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
	case DEV_FLASH:
		#if 0
		result = flash_disk_write(buff, sector, count);
		if (result != 0)
			return RES_ERROR;
		#endif
		return RES_OK;
	default:
		//ASSERT(0);
	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
#define FLASH_DISK_TOTAL_SIZE 0x200000
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;
    //uart_printf("%s %d cmd:%d\r\n", __func__, pdrv, cmd);
	switch (pdrv) {
	case DEV_RAM :

		// Process of the command for the RAM drive

		return res;

	case DEV_MMC :

		// Process of the command for the MMC/SD card
		switch(cmd) {
		case CTRL_SYNC:
			res = RES_OK;
			break;
		case GET_SECTOR_COUNT:
			*(DWORD *)buff = sdcard_get_blknum();
			res = RES_OK;
			break;
		case GET_SECTOR_SIZE:
			*(DWORD *)buff = SINGLE_BLOCK_SIZE;
			res = RES_OK;
			break;
		case GET_BLOCK_SIZE:
			*(DWORD *)buff = SINGLE_BLOCK_SIZE;
			res = RES_OK;
			break;
		case ISDIO_READ:
		case ISDIO_WRITE:
		default:
		    res = RES_ERROR;
			
		}

		return res;

	case DEV_USB :

		// Process of the command the USB drive

		return res;
	#if 0
	case DEV_FLASH:

		// Process of the command for the Flash
		switch(cmd) {
		case CTRL_SYNC:
			res = RES_OK;
			break;
		case GET_SECTOR_COUNT:
			*(DWORD *)buff = FLASH_DISK_TOTAL_SIZE/SINGLE_BLOCK_SIZE;
			res = RES_OK;
			break;
		case GET_SECTOR_SIZE:
			*(DWORD *)buff = SINGLE_BLOCK_SIZE;
			res = RES_OK;
			break;
		case GET_BLOCK_SIZE:
			*(DWORD *)buff = SINGLE_BLOCK_SIZE;
			res = RES_OK;
			break;
		case ISDIO_READ:
		case ISDIO_WRITE:
		default:
		    res = RES_ERROR;
			
		}
		return res;
	#endif
	default:
		return RES_PARERR;
	}

	return RES_PARERR;
}
DWORD get_fattime(void)
{
    //t_rtc tmp;
	DWORD t;
	//memset(&tmp, 0, sizeof(t_rtc));
	//PMIC_RTC_GetTime(&tmp,APP_OFFSET);
    //if (tmp.tm_year < 1980)
	//	tmp.tm_year = 1980;
	//t = (tmp.tm_year - 1980)<<25 | tmp.tm_mon<<21 | tmp.tm_mday<<16 | tmp.tm_hour<<11 | tmp.tm_min<<5 | tmp.tm_sec;
	t =  (2019)<<25 | 5<<21 | 24<<16 | 12<<11 | 12<<5 | 12;
	return t;
}




#include <stdio.h>
#include <string.h>
#include "ff.h"         /* Declarations of sector size */
#include "diskio.h"     /* Declarations of disk functions */
static
DWORD pn (		/* Pseudo random number generator */
    DWORD pns	/* 0:Initialize, !0:Read */
)
{
    static DWORD lfsr;
    UINT n;


    if (pns) {
        lfsr = pns;
        for (n = 0; n < 32; n++) pn(0);
    }
    if (lfsr & 1) {
        lfsr >>= 1;
        lfsr ^= 0x80200003;
    } else {
        lfsr >>= 1;
    }
    return lfsr;
}


int test_diskio (
    BYTE pdrv,      /* Physical drive number to be checked (all data on the drive will be lost) */
    UINT ncyc,      /* Number of test cycles */
    DWORD* buff,    /* Pointer to the working buffer */
    UINT sz_buff    /* Size of the working buffer in unit of byte */
)
{
    UINT n, cc, ns;
    DWORD sz_drv, lba, lba2, sz_eblk, pns = 1;
    WORD sz_sect;
    BYTE *pbuff = (BYTE*)buff;
    DSTATUS ds;
    DRESULT dr;


    fatal_printf("test_diskio(%u, %u, 0x%08X, 0x%08X)\n", pdrv, ncyc, (UINT)buff, sz_buff);

    if (sz_buff < FF_MAX_SS + 4) {
        fatal_printf("Insufficient work area to run program.\n");
        return 1;
    }

    for (cc = 1; cc <= ncyc; cc++) {
        fatal_printf("**** Test cycle %u of %u start ****\n", cc, ncyc);

        fatal_printf(" disk_initalize(%u)", pdrv);
        ds = disk_initialize(pdrv);
        if (ds & STA_NOINIT) {
            fatal_printf(" - failed.\n");
            return 2;
        } else {
            fatal_printf(" - ok.\n");
        }

        fatal_printf("**** Get drive size ****\n");
        fatal_printf(" disk_ioctl(%u, GET_SECTOR_COUNT, 0x%08X)", pdrv, (UINT)&sz_drv);
        sz_drv = 0;
        dr = disk_ioctl(pdrv, GET_SECTOR_COUNT, &sz_drv);
        if (dr == RES_OK) {
            fatal_printf(" - ok.\n");
        } else {
            fatal_printf(" - failed.\n");
            return 3;
        }
        if (sz_drv < 128) {
            fatal_printf("Failed: Insufficient drive size to test.\n");
            return 4;
        }
        fatal_printf(" Number of sectors on the drive %u is %lu.\n", pdrv, sz_drv);

#if FF_MAX_SS != FF_MIN_SS
        fatal_printf("**** Get sector size ****\n");
        fatal_printf(" disk_ioctl(%u, GET_SECTOR_SIZE, 0x%X)", pdrv, (UINT)&sz_sect);
        sz_sect = 0;
        dr = disk_ioctl(pdrv, GET_SECTOR_SIZE, &sz_sect);
        if (dr == RES_OK) {
            fatal_printf(" - ok.\n");
        } else {
            fatal_printf(" - failed.\n");
            return 5;
        }
        fatal_printf(" Size of sector is %u bytes.\n", sz_sect);
#else
        sz_sect = FF_MAX_SS;
#endif

        fatal_printf("**** Get block size ****\n");
        fatal_printf(" disk_ioctl(%u, GET_BLOCK_SIZE, 0x%X)", pdrv, (UINT)&sz_eblk);
        sz_eblk = 0;
        dr = disk_ioctl(pdrv, GET_BLOCK_SIZE, &sz_eblk);
        if (dr == RES_OK) {
            fatal_printf(" - ok.\n");
        } else {
            fatal_printf(" - failed.\n");
        }
        if (dr == RES_OK || sz_eblk >= 2) {
            fatal_printf(" Size of the erase block is %lu sectors.\n", sz_eblk);
        } else {
            fatal_printf(" Size of the erase block is unknown.\n");
        }

        /* Single sector write test */
        fatal_printf("**** Single sector write test 1 ****\n");
        lba = 0;
        for (n = 0, pn(pns); n < sz_sect; n++) pbuff[n] = (BYTE)pn(0);
        fatal_printf(" disk_write(%u, 0x%X, %lu, 1)", pdrv, (UINT)pbuff, lba);
        dr = disk_write(pdrv, pbuff, lba, 1);
        if (dr == RES_OK) {
            fatal_printf(" - ok.\n");
        } else {
            fatal_printf(" - failed.\n");
            return 6;
        }
        fatal_printf(" disk_ioctl(%u, CTRL_SYNC, NULL)", pdrv);
        dr = disk_ioctl(pdrv, CTRL_SYNC, 0);
        if (dr == RES_OK) {
            fatal_printf(" - ok.\n");
        } else {
            fatal_printf(" - failed.\n");
            return 7;
        }
        memset(pbuff, 0, sz_sect);
        fatal_printf(" disk_read(%u, 0x%X, %lu, 1)", pdrv, (UINT)pbuff, lba);
        dr = disk_read(pdrv, pbuff, lba, 1);
        if (dr == RES_OK) {
            fatal_printf(" - ok.\n");
        } else {
            fatal_printf(" - failed.\n");
            return 8;
        }
        for (n = 0, pn(pns); n < sz_sect && pbuff[n] == (BYTE)pn(0); n++) ;
        if (n == sz_sect) {
            fatal_printf(" Data matched.\n");
        } else {
            fatal_printf("Failed: Read data differs from the data written.\n");
            return 10;
        }
        pns++;

        fatal_printf("**** Multiple sector write test ****\n");
        lba = 1; ns = sz_buff / sz_sect;
        if (ns > 4) ns = 4;
        for (n = 0, pn(pns); n < (UINT)(sz_sect * ns); n++) pbuff[n] = (BYTE)pn(0);
        fatal_printf(" disk_write(%u, 0x%X, %lu, %u)", pdrv, (UINT)pbuff, lba, ns);
        dr = disk_write(pdrv, pbuff, lba, ns);
        if (dr == RES_OK) {
            fatal_printf(" - ok.\n");
        } else {
            fatal_printf(" - failed.\n");
            return 11;
        }
        fatal_printf(" disk_ioctl(%u, CTRL_SYNC, NULL)", pdrv);
        dr = disk_ioctl(pdrv, CTRL_SYNC, 0);
        if (dr == RES_OK) {
            fatal_printf(" - ok.\n");
        } else {
            fatal_printf(" - failed.\n");
            return 12;
        }
        memset(pbuff, 0, sz_sect * ns);
        fatal_printf(" disk_read(%u, 0x%X, %lu, %u)", pdrv, (UINT)pbuff, lba, ns);
        dr = disk_read(pdrv, pbuff, lba, ns);
        if (dr == RES_OK) {
            fatal_printf(" - ok.\n");
        } else {
            fatal_printf(" - failed.\n");
            return 13;
        }
        for (n = 0, pn(pns); n < (UINT)(sz_sect * ns) && pbuff[n] == (BYTE)pn(0); n++) ;
        if (n == (UINT)(sz_sect * ns)) {
            fatal_printf(" Data matched.\n");
        } else {
            fatal_printf("Failed: Read data differs from the data written.\n");
            return 14;
        }
        pns++;

        fatal_printf("**** Single sector write test (misaligned address) ****\n");
        lba = 5;
        for (n = 0, pn(pns); n < sz_sect; n++) pbuff[n+3] = (BYTE)pn(0);
        fatal_printf(" disk_write(%u, 0x%X, %lu, 1)", pdrv, (UINT)(pbuff+3), lba);
        dr = disk_write(pdrv, pbuff+3, lba, 1);
        if (dr == RES_OK) {
            fatal_printf(" - ok.\n");
        } else {
            fatal_printf(" - failed.\n");
            return 15;
        }
        fatal_printf(" disk_ioctl(%u, CTRL_SYNC, NULL)", pdrv);
        dr = disk_ioctl(pdrv, CTRL_SYNC, 0);
        if (dr == RES_OK) {
            fatal_printf(" - ok.\n");
        } else {
            fatal_printf(" - failed.\n");
            return 16;
        }
        memset(pbuff+5, 0, sz_sect);
        fatal_printf(" disk_read(%u, 0x%X, %lu, 1)", pdrv, (UINT)(pbuff+5), lba);
        dr = disk_read(pdrv, pbuff+5, lba, 1);
        if (dr == RES_OK) {
            fatal_printf(" - ok.\n");
        } else {
            fatal_printf(" - failed.\n");
            return 17;
        }
        for (n = 0, pn(pns); n < sz_sect && pbuff[n+5] == (BYTE)pn(0); n++) ;
        if (n == sz_sect) {
            fatal_printf(" Data matched.\n");
        } else {
            fatal_printf("Failed: Read data differs from the data written.\n");
            return 18;
        }
        pns++;

        fatal_printf("**** 4GB barrier test ****\n");
        if (sz_drv >= 128 + 0x80000000 / (sz_sect / 2)) {
            lba = 6; lba2 = lba + 0x80000000 / (sz_sect / 2);
            for (n = 0, pn(pns); n < (UINT)(sz_sect * 2); n++) pbuff[n] = (BYTE)pn(0);
            fatal_printf(" disk_write(%u, 0x%X, %lu, 1)", pdrv, (UINT)pbuff, lba);
            dr = disk_write(pdrv, pbuff, lba, 1);
            if (dr == RES_OK) {
                fatal_printf(" - ok.\n");
            } else {
                fatal_printf(" - failed.\n");
                return 19;
            }
            fatal_printf(" disk_write(%u, 0x%X, %lu, 1)", pdrv, (UINT)(pbuff+sz_sect), lba2);
            dr = disk_write(pdrv, pbuff+sz_sect, lba2, 1);
            if (dr == RES_OK) {
                fatal_printf(" - ok.\n");
            } else {
                fatal_printf(" - failed.\n");
                return 20;
            }
            fatal_printf(" disk_ioctl(%u, CTRL_SYNC, NULL)", pdrv);
            dr = disk_ioctl(pdrv, CTRL_SYNC, 0);
            if (dr == RES_OK) {
            fatal_printf(" - ok.\n");
            } else {
                fatal_printf(" - failed.\n");
                return 21;
            }
            memset(pbuff, 0, sz_sect * 2);
            fatal_printf(" disk_read(%u, 0x%X, %lu, 1)", pdrv, (UINT)pbuff, lba);
            dr = disk_read(pdrv, pbuff, lba, 1);
            if (dr == RES_OK) {
                fatal_printf(" - ok.\n");
            } else {
                fatal_printf(" - failed.\n");
                return 22;
            }
            fatal_printf(" disk_read(%u, 0x%X, %lu, 1)", pdrv, (UINT)(pbuff+sz_sect), lba2);
            dr = disk_read(pdrv, pbuff+sz_sect, lba2, 1);
            if (dr == RES_OK) {
                fatal_printf(" - ok.\n");
            } else {
                fatal_printf(" - failed.\n");
                return 23;
            }
            for (n = 0, pn(pns); pbuff[n] == (BYTE)pn(0) && n < (UINT)(sz_sect * 2); n++) ;
            if (n == (UINT)(sz_sect * 2)) {
                fatal_printf(" Data matched.\n");
            } else {
                fatal_printf("Failed: Read data differs from the data written.\n");
                return 24;
            }
        } else {
            fatal_printf(" Test skipped.\n");
        }
        pns++;

        fatal_printf("**** Test cycle %u of %u completed ****\n\n", cc, ncyc);
    }

    return 0;
}


DWORD buffx[FF_MAX_SS];

int ff_test0 (void)
{
    int rc;
      /* Working buffer (4 sector in size) */

    /* Check function/compatibility of the physical drive #0 */
    //rc = test_diskio(1, 1, buffx, sizeof buffx);
    rc = test_diskio(1, 1, buffx, sizeof buffx);
    if (rc) {
        fatal_printf("Sorry the function/compatibility test failed. (rc=%d)\nFatFs will not work with this disk driver.\n", rc);
    } else {
        fatal_printf("Congratulations! The disk driver works well.\n");
    }

    return rc;
}
int ff_test(void)
{
    FIL file;
	UINT ret;
	char buf[12];
	FATFS fs;
	FRESULT res;
	//res = f_mkfs("F:", FM_ANY, 0, buffx, sizeof(buffx));
	//fatal_printf("fatfs format :%d\r\n", res);
	//return 0;
	res = f_mount(&fs,"D:", 1);
	fatal_printf("fatfs mount err:%d\n", res);
	if(res != FR_OK)
		return 0;
    res = f_open(&file, "D:\\1.txt", FA_WRITE|FA_CREATE_ALWAYS);
	if (res != FR_OK) {
		fatal_printf("fatfs open err:%d\n", res);
		return -1;
	}
	res = f_write(&file, "123456789", 9, &ret);
	fatal_printf("fatafs write result:%d\n", res);
	f_close(&file);
	res = f_open(&file, "D:\\1.txt", FA_READ);
	if (res != FR_OK) {
		fatal_printf("fatfs open err:%d\n", res);
		return -1;
	}
	res = f_read(&file, buffx, 10, &ret);
	fatal_printf("fatafs write result:%d len:%d, :%s\n", res, ret, buffx);
	f_close(&file);
	
	DIR dj;
	FILINFO fno;
	memset(&dj, 0, sizeof(dj));
	memset(&fno, 0, sizeof(fno));
	res = f_findfirst(&dj, &fno, "D:\\","*.txt");
	while(res == FR_OK && fno.fname[0]){
		uart_printf("%s\n", fno.fname);
		memset(&fno, 0, sizeof(fno));
		res = f_findnext(&dj, &fno);
	}
	f_closedir(&dj);
	return 0;
}


