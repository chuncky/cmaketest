#ifdef YMODEM_EEH_DUMP
/******************************************************************************
*(C) Copyright 2018 ASR Micro
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: ymodem.c
 *
 *  Description: Implementation of Ymodem sender for sending dump files when system assert
 *
 *  Author: yanglei@asrmicro.com
 *  History:
 *
 *
 *  Notes:
 *
 ******************************************************************************/

#include "osa.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>
#include "mvUsbModem.h"
#include "usb2_device.h"

#define PACKET_SEQNO_INDEX      (1)
#define PACKET_SEQNO_COMP_INDEX (2)

#define PACKET_HEADER           (3)
#define PACKET_TRAILER          (2)
#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE             (128)
#define PACKET_1K_SIZE          (1024)

#define FILE_NAME_LENGTH        (128)
#define FILE_SIZE_LENGTH        (16)

#define SOH                     (0x01)  /* start of 128-byte data packet */
#define STX                     (0x02)  /* start of 1024-byte data packet */
#define EOT                     (0x04)  /* end of transmission */
#define ACK                     (0x06)  /* acknowledge */
#define NAK                     (0x15)  /* negative acknowledge */
#define CA                      (0x18)  /* two of these in succession aborts transfer */
#define CRC16                   (0x43)  /* 'C' == 0x43, request 16-bit CRC */

#define ABORT1                  (0x41)  /* 'A' == 0x41, abort by user */
#define ABORT2                  (0x61)  /* 'a' == 0x61, abort by user */

#define NAK_TIMEOUT             (0x100000)
#define MAX_ERRORS              (5)

#define YMLOG(fmt, args...)  do{/*uart_printf("YM-"fmt, ##args);*/}while(0)

typedef struct _YmMsg
{
	unsigned long  sATPInd;
	unsigned long  length;
	char *data;
} YmMsg, *PYmMsg;

OSMsgQRef gYMMsg = NULL;
static OSTaskRef gYMtask = NULL;

UINT8 usbtxbuffer[1200];

extern UINT8 usbRxbuffer[10];
extern volatile UINT32  _usbDeviceDebug_int_int;

extern UINT8 usbmodemtxint;
extern UINT8 usbmodemrxint;

static unsigned char gblkNumber;

unsigned short calcrc(unsigned char *ptr, int count)
{
    unsigned int crc, i;

    crc = 0;
    while (--count >= 0) {
       crc = crc ^ (int)*ptr++ << 8;
       for (i = 0; i < 8; ++i)
           if (crc & 0x8000)
           		crc = crc << 1 ^ 0x1021;
           else
           		crc = crc << 1;
    }
    return (crc & 0xFFFF);
}

unsigned char CalChecksum(const unsigned char* data, unsigned int size)
{
	unsigned int sum = 0;
	const unsigned char* dataEnd = data+size;

	while(data < dataEnd )
		sum += *data++;

	return (sum & 0xffu);
}

static int Send_Byte (unsigned char c)
{
	usbmodemtxint = 0;
	memcpy(usbtxbuffer, &c, 1);
	mvUsbModemSendDataYmodem(1, usbtxbuffer, 1);


	while(!usbmodemtxint)
		USB2DriverInterruptloopHandler();

	return 0;
}

static int Receive_Byte (unsigned char *c, int timeout)
{
#if 0
	OSA_STATUS  osaStatus;
	YmMsg msg;
    memset(&msg, 0, sizeof(YmMsg));
	osaStatus = OSAMsgQRecv(gYMMsg, (unsigned char *)&msg, sizeof(msg), 200);
	if (osaStatus == OS_SUCCESS)
	{
		//YMLOG("%d,%d,%x\n", msg.length, msg.sATPInd,msg.data[0]);
		*c = msg.data[0];
	    if(msg.data)
	        free(msg.data);

		YMLOG("Receive_Byte:%x\n", *c);

		return 0;
	}
#endif
    //YMLOG("Receive_Byte");
	int i = 0;
	usbmodemrxint = 0;

	while(usbmodemrxint == 0){
		USB2DriverInterruptloopHandler();
		if (timeout > 0) {
			if(i < timeout)
				i++;
			else
				break;
		}
	}

	
    //YMLOG("Receive_Byte1:%x", usbRxbuffer[0]);
	if (usbmodemrxint == 1) {
		*c = usbRxbuffer[0];
		return 0;
	}
	else
		return -1;

}

void Ymodem_SendPacket(unsigned char *data, unsigned short length)
{
	//uart_printf("Ymodem send %d bytes\n", length);
	//char *buf = (char *)malloc(length);

	usbmodemtxint = 0;

	memcpy(usbtxbuffer, data, length);
	mvUsbModemSendDataYmodem(1, usbtxbuffer, length);

	//uart_printf("Ymodem send %02x,%02x,%02x\n", usbtxbuffer[0], usbtxbuffer[1], usbtxbuffer[2]);

	//_usb_dci_vusb20_get_dtd_information();

	while(usbmodemtxint == 0)
		USB2DriverInterruptloopHandler();

	//uart_printf("Ymodem send %d bytes ok\n", length);
	return;
}


void Ymodem_PrepareIntialPacket(unsigned char *data, const char * fileName, unsigned int length)
{
	char file_ptr[40] = {'\0'};

	memset(data, 0, PACKET_SIZE + PACKET_HEADER);

	/* Make first three packet */
	data[0] = SOH;
	data[1] = 0x00;
	data[2] = 0xff;

	memcpy(data + PACKET_HEADER, fileName, strlen(fileName) + 1);

	if ((int)length >= 0)
	{
		sprintf(file_ptr, "%lu", length);
		memcpy(data + PACKET_HEADER + strlen(fileName) + 1, file_ptr, strlen(file_ptr));
	}
}

void Ymodem_PreparePacket(unsigned char *SourceBuf, unsigned char *data, unsigned char pktNo, unsigned int sizeBlk)
{
	unsigned short i, size, packetSize;
	unsigned char* file_ptr;

	/* Make first three packet */
	packetSize = sizeBlk >= PACKET_1K_SIZE ? PACKET_1K_SIZE : PACKET_SIZE;
	size = sizeBlk < packetSize ? sizeBlk :packetSize;
	if (packetSize == PACKET_1K_SIZE)
	{
		data[0] = STX;
	} else {
		data[0] = SOH;
	}
	data[1] = pktNo;
	data[2] = 0xFF - pktNo;
	file_ptr = SourceBuf;

	for (i = PACKET_HEADER; i < size + PACKET_HEADER; i++)
	{
		data[i] = *file_ptr++;
	}
	if ( size  <= packetSize)
	{
		for (i = size + PACKET_HEADER; i < packetSize + PACKET_HEADER; i++)
		{
			data[i] = 0x1A; /* EOF (0x1A) or 0x00 */
		}
	}
}

int ymodem_wait_recv_ready(void)
{
	unsigned char receivedC[2];
	do
	{
		//usbmodemrxint = 0;
		//while(usbmodemrxint == 0);
		//	USB2DriverInterruptHandlerwait();
		if(Receive_Byte(&receivedC[0], -1) == 0)
		{
			if (receivedC[0] == CRC16)
				break;
		}
	}while (1);

	//uart_printf("start trans\n");
	return 0;
}

int ymodem_send_complete_packet(void)
{
	unsigned char packet_data[PACKET_SIZE + PACKET_OVERHEAD];
	unsigned char receivedC[2];
	unsigned int errors, ackReceived, i;
	unsigned short tempCRC;

	ackReceived = 0;
	receivedC[0] = 0x00;
	errors = 0;

	packet_data[0] = SOH;
	packet_data[1] = 0;
	packet_data[2] = 0xFF;

	for (i = PACKET_HEADER; i < (PACKET_SIZE + PACKET_HEADER); i++)
	{
		packet_data[i] = 0x00;
	}

	do
	{
		tempCRC = calcrc(&packet_data[3], PACKET_SIZE);
		packet_data[PACKET_SIZE + PACKET_HEADER] = (tempCRC >> 8);
		packet_data[PACKET_SIZE + PACKET_HEADER + 1] = (tempCRC & 0xFF);
		Ymodem_SendPacket(packet_data, PACKET_SIZE + PACKET_HEADER + PACKET_TRAILER);

		/* Wait for Ack and 'C' */
		if (Receive_Byte(&receivedC[0], -1) == 0)
		{
			if (receivedC[0] == ACK)
			{
				/* Packet transferred correctly */
				ackReceived = 1;
			}
		}
		else
		{
			errors++;
		}

	}while (!ackReceived && (errors < 0x0A));

	/* Resend packet if NAK  for a count of 10  else end of communication */
	if (ackReceived == 0 && errors >=  0x0A)
	{
		YMLOG("---%s   %d error : %d--\n\r", __FUNCTION__, __LINE__, errors);
		return errors;
	}

  	YMLOG("trans complete");

  	return 0;
}

int ymodem_intialPacketAndSend(char *name, unsigned int sizeFile)
{
	unsigned char packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD];
	unsigned char *buf_ptr, tempCheckSum;
	unsigned short tempCRC;
	unsigned char blkNumber;
	unsigned char receivedC[2], CRC16_F = 0, i;
	unsigned int errors, ackReceived, size = 0, pktSize;

	errors = 0;
	ackReceived = 0;
	CRC16_F = 1;

	gblkNumber = 0x01;

	//uart_printf("%s,%s,%d\n",__FUNCTION__, name, sizeFile);

	/* Prepare first block */
	Ymodem_PrepareIntialPacket(packet_data, name, sizeFile);

	do
	{
		/* Send CRC or Check Sum based on CRC16_F */
		if (CRC16_F)
		{
			tempCRC = calcrc(&packet_data[3], PACKET_SIZE);
			packet_data[PACKET_SIZE + PACKET_HEADER] = (tempCRC >> 8);
			packet_data[PACKET_SIZE + PACKET_HEADER + 1] = (tempCRC & 0xFF);
		}
		else
		{
			tempCheckSum = CalChecksum (&packet_data[3], PACKET_SIZE);
			packet_data[PACKET_SIZE + PACKET_HEADER] = tempCheckSum;
		}

		/* Send Packet */
		Ymodem_SendPacket(packet_data, PACKET_SIZE + PACKET_HEADER + PACKET_TRAILER);

		/* Wait for Ack and 'C' */
		if (Receive_Byte(&receivedC[0], -1) == 0 && (receivedC[0] == ACK || receivedC[0] == CRC16))
		{
			ackReceived = 1;
		}
		else
		{
			errors++;
		}
	}while (!ackReceived && (errors < 0x0A));

	if (ackReceived == 0 && errors >=  0x0A)
	{
  		uart_printf("---%s   %d error : %d--\n\r", __FUNCTION__, __LINE__, errors);
		return errors;
	}

	return 0;
}

int ymodem_transDataPacket(unsigned char *buf, unsigned int len)
{
	unsigned char packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD];
	unsigned char *buf_ptr, tempCheckSum;
	unsigned short tempCRC;
	unsigned char receivedC[2], CRC16_F = 0, i;
	unsigned int errors, ackReceived, size = 0, pktSize;

	errors = 0;
	ackReceived = 0;
	CRC16_F = 1;

	buf_ptr = buf;
	size = len;

	while (size)
	{
		Ymodem_PreparePacket(buf_ptr, packet_data, gblkNumber, size);
		ackReceived = 0;
		receivedC[0]= 0;
		errors = 0;
		do
		{
			/* Send next packet */
			if (size >= PACKET_1K_SIZE)
			{
				pktSize = PACKET_1K_SIZE;
			}
			else
			{
				pktSize = PACKET_SIZE;
			}

			/* Send CRC or Check Sum based on CRC16_F */
			if (CRC16_F)
			{
				tempCRC = calcrc(&packet_data[3], pktSize);
				packet_data[pktSize + PACKET_HEADER] = (tempCRC >> 8);
				packet_data[pktSize + PACKET_HEADER + 1] = (tempCRC & 0xFF);
			}
			else
			{
				tempCheckSum = CalChecksum (&packet_data[3], pktSize);
				Send_Byte(tempCheckSum);
			}

			Ymodem_SendPacket(packet_data, pktSize + PACKET_HEADER + PACKET_TRAILER);

			/* Wait for Ack */
			if ((Receive_Byte(&receivedC[0], -1) == 0)  && (receivedC[0] == ACK))
			{
				ackReceived = 1;
				if (size > pktSize)
				{
					buf_ptr += pktSize;
					size -= pktSize;
					if (gblkNumber == 0xFF)
					{
						gblkNumber = 0;
					}
					else
					{
						gblkNumber++;
					}
				}
				else
				{
					buf_ptr += pktSize;
					size = 0;
				}
			}
			else
			{
				errors++;
			}
		}while(!ackReceived && (errors < 0x0A));
		/* Resend packet if NAK  for a count of 10 else end of communication */

		if (ackReceived == 0 && errors >=  0x0A)
		{
			uart_printf("---%s   %d error : %d--\n\r", __FUNCTION__, __LINE__, errors);
			return errors;
		}
	}

	return 0;
}

int ymodem_transEot(void)
{
	unsigned char receivedC[2];
	unsigned int ackReceived = 0;
	unsigned int errors = 0;

	do
	{
		Send_Byte(EOT);
		/* Send (EOT); */
		/* Wait for Ack */
		if ((Receive_Byte(&receivedC[0], -1) == 0)  && receivedC[0] == ACK)
		{
			ackReceived = 1;
		}
		else
		{
			errors++;
		}
	}while (!ackReceived && (errors < 0x0A));

	gblkNumber = 0;

	if (ackReceived == 0 && errors >=  0x0A)
	{
		uart_printf("---%s   %d error : %d--\n\r", __FUNCTION__, __LINE__);
		return errors;
	}

	return 0;
}

int ymodem_trans(unsigned char *buf, char *name, unsigned int sizeFile)
{
	unsigned char packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD];
	unsigned char *buf_ptr, tempCheckSum;
	unsigned short tempCRC;
	unsigned char blkNumber;
	unsigned char receivedC[2], CRC16_F = 0, i;
	unsigned int errors, ackReceived, size = 0, pktSize;

	errors = 0;
	ackReceived = 0;
	CRC16_F = 1;

	//uart_printf("%s,%d\n",name,sizeFile);
	//uart_printf("trans1\n");

	/* Prepare first block */
	Ymodem_PrepareIntialPacket(packet_data, name, sizeFile);

	do
	{
		/* Send CRC or Check Sum based on CRC16_F */
		if (CRC16_F)
		{
			tempCRC = calcrc(&packet_data[3], PACKET_SIZE);
			packet_data[PACKET_SIZE + PACKET_HEADER] = (tempCRC >> 8);
			packet_data[PACKET_SIZE + PACKET_HEADER + 1] = (tempCRC & 0xFF);
		}
		else
		{
			tempCheckSum = CalChecksum (&packet_data[3], PACKET_SIZE);
			packet_data[PACKET_SIZE + PACKET_HEADER] = tempCheckSum;
		}

		/* Send Packet */
		Ymodem_SendPacket(packet_data, PACKET_SIZE + PACKET_HEADER + PACKET_TRAILER);

  		//uart_printf("trans1-a");
		/* Wait for Ack and 'C' */
		if (Receive_Byte(&receivedC[0], -1) == 0 && (receivedC[0] == ACK || receivedC[0] == CRC16))
		{
			ackReceived = 1;
		}
		else
		{
			errors++;
		}
	}while (!ackReceived && (errors < 0x0A));

	if (ackReceived == 0 && errors >=  0x0A)
	{
  		uart_printf("---%s   %d error : %d--\n\r", __FUNCTION__, __LINE__, errors);
		return errors;
	}

	//uart_printf("trans2\n");
	buf_ptr = buf;
	size = sizeFile;
	blkNumber = 0x01;

	ackReceived = 0;

	while (size)
	{
		/* Prepare next packet */
		Ymodem_PreparePacket(buf_ptr, packet_data, blkNumber, size);
		ackReceived = 0;
		receivedC[0]= 0;
		errors = 0;
		do
		{
			/* Send next packet */
			if (size >= PACKET_1K_SIZE)
			{
				pktSize = PACKET_1K_SIZE;
			}
			else
			{
				pktSize = PACKET_SIZE;
			}


			/* Send CRC or Check Sum based on CRC16_F */
			if (CRC16_F)
			{
				tempCRC = calcrc(&packet_data[3], pktSize);
				packet_data[pktSize + PACKET_HEADER] = (tempCRC >> 8);
				packet_data[pktSize + PACKET_HEADER + 1] = (tempCRC & 0xFF);
			}
			else
			{
				tempCheckSum = CalChecksum (&packet_data[3], pktSize);
				Send_Byte(tempCheckSum);
			}

			Ymodem_SendPacket(packet_data, pktSize + PACKET_HEADER + PACKET_TRAILER);

			/* Wait for Ack */
			if ((Receive_Byte(&receivedC[0], -1) == 0)  && (receivedC[0] == ACK))
			{
				ackReceived = 1;
				if (size > pktSize)
				{
					buf_ptr += pktSize;
					size -= pktSize;
					if (blkNumber == 0xFF)
					{
						blkNumber = 0;
					}
					else
					{
						blkNumber++;
					}
				}
				else
				{
					buf_ptr += pktSize;
					size = 0;
				}
			}
			else
			{
				errors++;
			}
		}while(!ackReceived && (errors < 0x0A));
		/* Resend packet if NAK  for a count of 10 else end of communication */

		if (ackReceived == 0 && errors >=  0x0A)
		{
			uart_printf("---%s   %d error : %d--\n\r", __FUNCTION__, __LINE__, errors);
			return errors;
		}
	}

	//uart_printf("trans3\n");
	ackReceived = 0;
	receivedC[0] = 0x00;
	errors = 0;
	do
	{
		Send_Byte(EOT);
		/* Send (EOT); */
		/* Wait for Ack */
		if ((Receive_Byte(&receivedC[0], -1) == 0)  && receivedC[0] == ACK)
		{
			ackReceived = 1;
		}
		else
		{
			errors++;
		}
	}while (!ackReceived && (errors < 0x0A));

	if (ackReceived == 0 && errors >=  0x0A)
	{
		uart_printf("---%s   %d error : %d--\n\r", __FUNCTION__, __LINE__);
		return errors;
	}

	return 0; /* file transmitted successfully */
}


#if 0
void ymodem_test(void *argv)
{
	unsigned char *buf = (unsigned char *)(0x6000000);
	ymodem_wait_recv_ready();
	ymodem_trans(buf, "cp0", 0x800000);
	buf = (unsigned char *)(0x6800000);
	ymodem_trans(buf, "cp1", 0x800000);
	ymodem_send_complete_packet();
}


void ymodemtask (void)
{

    OSA_STATUS	status;
    void *pStack = NULL;

    status = OSAMsgQCreate(&gYMMsg,
#ifdef  OSA_QUEUE_NAMES
                          "ymmsq",
#endif
                          sizeof(YmMsg),
                          128,
                          OSA_FIFO);
	ASSERT(status == OS_SUCCESS);

    pStack = malloc(1024*4);
    ASSERT(pStack != NULL);

	status = OSATaskCreate(&gYMtask,
                           pStack,
                           (1024*4),
                           90,
                           "ymodem",
                           ymodem_test,
                           0);
    ASSERT(status == OS_SUCCESS);

}
#endif
#endif
