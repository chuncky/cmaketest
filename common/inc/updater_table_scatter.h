#ifndef __UPDATER_TABLE_H__
#define __UPDATER_TABLE_H__


/* 1. Header info table */
// NOTE: Header info start address is coupling with cp.bin and can not modify.
/*--------------
 ADDRESS
 1. updater Header info start address		(updater_start_address) - (UPDATER_CRC_INFO_MAX_SIZE) -   (UPDATER_TAIL_INFO_MAX_SIZE)           					
 ...
 x. crc info start address					(updater_image_size) - (UPDATER_CRC_INFO_MAX_SIZE)                											    
 */
#define UPDATER_HEADER_OFFSET_START           ( 0 )
#define UPDATER_HEADER_TABLE_MAX_SIZE         ( 512 + 256 )



/* 2. Tail info table */
/*--------------
 ADDRESS ( note: updater)
 1. updater tail info start address			(updater_image_size) - (UPDATER_CRC_INFO_MAX_SIZE) -   (UPDATER_TAIL_INFO_MAX_SIZE)           					
 ...
 x. crc info start address					(updater_image_size) - (UPDATER_CRC_INFO_MAX_SIZE)                											    
 */
#define UPDATER_TAIL_TABLE_MAX_SIZE           ( 256 ) 
#define UPDATER_CRC_INFO_MAX_SIZE 	    	  ( 4 )  

#endif
