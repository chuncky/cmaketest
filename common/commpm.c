#include "common.h"
#include "bsp.h"
#include "os_hal.h"


volatile UINT32 prepareUSBdown = 0;
volatile UINT32  usb_shutdown = 0;
#define USB_PHY_REG0A           0xD4207028
#define USB_PHY_REG0B           0xD420702C
#define USB_PHY_REG28           0xD42070A0

/* Usb Phy Power Down flag */
BOOL UsbPhyPowerDown = FALSE;

void craneCommPMPowerdownUsbPhy(void)
{
    volatile UINT32 tmp = 0;
    if(!((*(volatile UINT32 *)0xD428287C) & BIT_15))
    {
        UsbPhyPowerDown = TRUE;
        
        tmp = *(volatile UINT32*)(0xD420702C);
        *(volatile UINT32*)0xD420702C = tmp & ~((0x3 << 12)|(0x1 << 7));
        tmp = *(volatile UINT32*)(0xD420702C);
        *(volatile UINT32*)0xD420702C = tmp | (0x1 << 6);

        tmp = *(volatile UINT32*)(0xD4207028);
        *(volatile UINT32*)0xD4207028 = tmp | ((0x3 << 12)|(0x1 << 6));

		/*0xd4207094 bit[11-8] = 4`b0101 */
		tmp = *(volatile UINT32*)(0xD4207094);
		tmp = tmp & ~((0x1 << 11)|(0x1 << 9));
		*(volatile UINT32*)0xD4207094 = tmp | ((0x1 << 10)|(0x1 << 8));

		/*0xd42070a0 bit[14] = 1 */
		tmp = *(volatile UINT32*)(0xd42070a0);
		*(volatile UINT32*)(0xd42070a0) |= (0x1 << 14);


    }



}

void craneCommPMPowerupUsbPhy(void)
{
    volatile UINT32 tmp = 0;

    if(UsbPhyPowerDown)
    {
        tmp = *(volatile UINT32*)(0xD420702C);

        *(volatile UINT32*)0xD420702C = tmp | ((0x3 << 12)|(0x1 << 7));
    	tmp = *(volatile UINT32*)(0xD420702C);
    	*(volatile UINT32*)0xD420702C = tmp & ~(0x1 << 6);

        tmp = *(volatile UINT32*)(0xD4207028);

        *(volatile UINT32*)0xD4207028 = tmp & ~((0x3 << 12) |(0x1 << 6));

    	/*0xd42070a0 bit[14] = 0 */
    	tmp = *(volatile UINT32*)(0xd42070a0);
    	*(volatile UINT32*)(0xd42070a0) = tmp & ~(0x1 << 14);

    	/*0xd4207094 bit[11-8] = 4`b1001*/
    	tmp = *(volatile UINT32*)(0xD4207094);
    	tmp = tmp & ~((0x1 << 10)|(0x1 << 9));
    	*(volatile UINT32*)0xD4207094 = tmp | ((0x1 << 11)|(0x1 << 8));
    	UsbPhyPowerDown = FALSE;
    }
}











































