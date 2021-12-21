#ifndef __UPDATER_OTA_SPI_H__
#define __UPDATER_OTA_SPI_H__

#define UPDATER_PSRAM_BASE_ADDR (0x7E400000)
#define PSRAM_END_ADDR	(0x7F000000)
/* Exec Region 1 */
#define INIT_BASE_ADDR	UPDATER_PSRAM_BASE_ADDR								//0x7E400000
#define INIT_SIZE		(0x00010000)

#define UPDATER_LOAD_MAX_SIZE	(0x00020000)								//128K											

/* Exec Region 2 */
#define CODE_BASE_ADDR  (0x7E480000)										//0x7E480000
#define CODE_SIZE  		(0x00080000)

/* Exec Region 3 */
#define DATA_BASE_ADDR  (CODE_BASE_ADDR + CODE_SIZE)					    //0x7E500000	
#define DATA_SIZE  		(0x00100000)

/* Exec Region 4 */
#define NONCACHE_DATA_BASE_ADDR  			 (DATA_BASE_ADDR+DATA_SIZE)		//0x7E600000	
#define NONCACHE_DATA_SIZE  				 (0x00100000)

#endif
