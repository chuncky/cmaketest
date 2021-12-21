#ifndef _SDIO_API_H_
#define _SDIO_API_H_

typedef int                     STATUS;
typedef unsigned long           UNSIGNED;
typedef unsigned char *         BYTE_PTR;
typedef unsigned int            UNSIGNED_INT;

/**  
 *  @brief This function writes data into card register.
 *  This function use function 1, because function 1 represent wifi  
 *
 *  @reg:          register offset
 *  @data:         value
 *
 *  @return 0 if succeed, otherwise Err number
*/
STATUS
NU_SDIODrv_Write_Register(UNSIGNED reg, UNSIGNED data);

/**  
 *  This function reads data from card register.
 *  This function use function 1, because function 1 represent wifi  
 *
 *  @reg:          register offset
 *  @data:         value
 *
 *  @return 0 if succeed, otherwise Err number
*/
STATUS
NU_SDIODrv_Read_Register(UNSIGNED reg, UNSIGNED *data);

/**  
 *  This function write a command/data packet to card.
 *  This function use function 1, because function 1 represent wifi
 *
 *  @port: 	    Port number for sent
 *  @buffer:   Pointer to buffer data
 *  @buf_len:	    Buffer length
 *
 *  @return 0 if succeed, otherwise Err number
*/
STATUS
NU_SDIODrv_Write_Memory(UNSIGNED_INT ioport, BYTE_PTR buffer, UNSIGNED buf_len);

/**  
 *  This function read data packet/event/command from card.
 *  This function use function 1, because function 1 represent wifi
 *
 *  @port: 	    Port number for sent
 *  @buffer:       Pointer to buffer data
 *  @buf_len:	    Buffer length
 *
 *  @return 0 if succeed, otherwise Err number
*/
STATUS
NU_SDIODrv_Read_Memory(UNSIGNED_INT ioport, BYTE_PTR buffer, UNSIGNED buf_len);

#endif //_SDIO_API_H_

