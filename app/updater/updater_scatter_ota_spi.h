#ifndef __UPDATER_OTA_SPI_H__
#define __UPDATER_OTA_SPI_H__

#define UPDATER_OTA_SPI_PSRAM_BASE_ADDR (0x7E400000)
#define PSRAM_END_ADDR	(0x7F000000)
/* Exec Region 1 */
#define INIT_BASE_ADDR	UPDATER_OTA_SPI_PSRAM_BASE_ADDR										//0x7E400000
#define INIT_SIZE		(0x00001000)

#define UPDATER_LOAD_MAX_SIZE	(0x00020000)	//128K											

/* Exec Region 2 */
#define CODE_BASE_ADDR  (INIT_BASE_ADDR + UPDATER_LOAD_MAX_SIZE)							//0x7E420000
#define CODE_SIZE  		(0x00020000)

/* Exec Region 3 */
#define DATA_BASE_ADDR  (CODE_BASE_ADDR + CODE_SIZE)										//0x7E440000	
#define DATA_SIZE  		(0x00070000)

/* Exec Region 4 */
#define NONCACHE_DATA_BASE_ADDR  			 (DATA_BASE_ADDR+DATA_SIZE)						//0x7E4B0000	
#define NONCACHE_DATA_SIZE  				 (0x00020000)

/* Exec Region 5 */
#define ASR_SPI_OTA_BUFFER_BASE_ADDR  		 (NONCACHE_DATA_BASE_ADDR+NONCACHE_DATA_SIZE)				//0x7E4D0000	
#define ASR_SPI_OTA_BUFFER_SIZE  			 (PSRAM_END_ADDR - ASR_SPI_OTA_BUFFER_BASE_ADDR)

/* Exec Region 6 */
//#define ASR_OTA_SPI_BUFFER_END			 	 (ASR_SPI_OTA_BUFFER_BASE_ADDR+ASR_SPI_OTA_BUFFER_SIZE)
//#define ASR_OTA_SPI_BUFFER_END_ADDR	         (ASR_OTA_SPI_BUFFER_END)
//#define ASR_OTA_SPI_BUFFER_END_SIZE	         (0)

#endif
