#ifndef	_UI_I2C_API_H_
#define	_UI_I2C_API_H_
#include "plat_types.h"

typedef enum
{
    STANDARD_MODE = 0,	/*100Kbps*/
    FAST_MODE = 1,		/*400Kbps*/
    HS_MODE = 2,		/*3.4 Mbps slave/3.3 Mbps master,standard mode when not doing a high speed transfer*/
    HS_MODE_FAST = 3,	/*3.4 Mbps slave/3.3 Mbps master,fast mode when not doing a high speed transfer*/
} I2C_FAST_MODE;
int i2c_init(uint8 i2c_no, uint32 fastMode);
int i2c_write(uint8 i2c_no, uint8 slaveaddress, uint8 *cmd,uint32 cmdlen, uint8 *data, uint32 datalen);
int i2c_read(uint8 i2c_no, uint8 slaveaddress, uint8 *cmd,uint32 cmdlen, uint8 *data, uint32 datalen);
void TWSI_Init(uint32 mode, unsigned int i2c_no);
int TWSI_REG_READ_BYTE(uint8 i2c_no, uint8 slaveaddress, uint8 addr);
int TWSI_REG_WRITE_BYTE(uint8 i2c_no, uint8 slaveaddress, uint8 addr, uint8 data);
int TWSI_REG_READ_SHORT(uint8 i2c_no, uint8 slaveaddress, uint16 addr);
int TWSI_REG_WRITE_SHORT(uint8 i2c_no, uint8 slaveaddress, uint16 addr, uint16 data);
int TWSI_REG_READ_WORD(uint8 i2c_no, uint8 slaveaddress, uint16 addr);
int TWSI_REG_WRITE_WORD(uint8 i2c_no, uint8 slaveaddress, uint16 addr, uint32 data);
int TWSI_REG_WRITE_CAM(uint8 i2c_no, uint8 slaveaddress, uint16 addr, uint8 data);
int TWSI_REG_READ_CAM(uint8 i2c_no, uint8 slaveaddress, uint16 addr);

#endif	/* _UI_I2C_H_ */
