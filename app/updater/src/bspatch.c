/*-
 * Copyright 2003-2005 Colin Percival
 * Copyright 2012 Matthew Endsley
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions 
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "bspatch.h"
#include "malloc.h"
#include "LzmaDecode.h"
#include "ptable.h"

//8 more bytes is to store old file size
#define HEADER_EXT

#ifdef HEADER_EXT
#define HEADER_SIZE   40
#else
#define HEADER_SIZE   32
#endif

#define CMP_LZMA

static UINT32 Dfota_Image_ID=0x1FFFFFFF;
static UINT32 Dfota_Flash_Start_Address=0; 

#define LZMA_HEADER_EXT

#ifdef LZMA_HEADER_EXT
#define LZMA_HEADER_SIZE   8
#else
#define LZMA_HEADER_SIZE   0
#endif



//#define crane_qspi_read(addr,buf_addr,size) asr_norflash_read((addr+QSPI_FLASH_BASE), buf_addr,size)
//#define crane_qspi_erase(addr,size)  asr_norflash_erase((addr+QSPI_FLASH_BASE),size)
//#define crane_qspi_write(addr,buf_addr,size) asr_norflash_write((addr+QSPI_FLASH_BASE),buf_addr,size)





void fota_dump_buffer(char *prefix, unsigned char *data, int offs, int length) 
{
	int i,j;
	int count;
	char a[16];
	
	uart_printf("%s :Dumping %d bytes from address 0x%08Lx using an offset of %d bytes\r\n", prefix, length, data, offs); 
	if(length == 0) {
		uart_printf("%s => nothing to dump\r\n", prefix);
		return;
	}
	
	count= length/16;

	for(i=0;i<count;i++)
	{
		for(j=0;j<16;j++)
		{
			a[j]= *((char*)data+j+i*16);
		}
		uart_printf("[%d-%d] %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x \r\n",
		      		i*16,(i+1)*16,a[0],a[1],a[2],a[3],a[4],a[5],a[6],a[7],a[8],a[9],a[10],a[11],a[12],a[13],a[14],a[15]);
	}

	count = length%16;
	uart_printf("[%d-%d]",(length/16)*16,(length/16)*16+count-1);
	for(i=0;i<count;i++)
	{
		a[i]=*((char*)data+i+(length/16)*16);
		uart_printf(" %02x",a[i]);
	}
	uart_printf("\r\n");
}


UINT32 CalcImageChecksum( UINT32* DownloadArea, UINT32 ImageLength,UINT32 checksum)
{
    UINT32 ImageChecksum = checksum;
    UINT32* ptr32 = DownloadArea;
    UINT32* pEnd = ptr32 + (ImageLength / sizeof(UINT32));
    UINT32 BytesSummed = 0;

    while ( ptr32 < pEnd )
    {
        // checksum format version 2 algorithm as defined by flasher
        ImageChecksum ^= (*ptr32);
        ptr32++;
        BytesSummed += sizeof(UINT32);
    }
    return ImageChecksum;
}


static int64_t offtin(uint8_t *buf)
{
	int64_t y;

	y=buf[7]&0x7F;
	y=y*256;y+=buf[6];
	y=y*256;y+=buf[5];
	y=y*256;y+=buf[4];
	y=y*256;y+=buf[3];
	y=y*256;y+=buf[2];
	y=y*256;y+=buf[1];
	y=y*256;y+=buf[0];

	if(buf[7]&0x80) y=-y;

	return y;
}

/* Using control info in patch to calculate oldfile size */
uint32_t get_oldsize(char *patch)
{
#ifdef HEADER_EXT
	uint32_t oldlen;
	if(memcmp(patch, "BSDIFF40", 8) != 0){
		uart_printf("Corrupt patch\n\r");
		return 0;
	}

	oldlen = offtin((uint8_t *)patch + 32);
	uart_printf("get_oldsize: 0x%08x\n\r", oldlen);

	return oldlen;
#else
	uint32_t ctr_len;
	char *header;
	BZFILE *cpfbz2;
	int cbz2err;
	int ctrl[3];
	int i, j, lenread;
	int oldpos = 0, newpos=0, newsize;
	BFILE *cpf;
	uint8_t buf[8];

	header = patch;
	if(memcmp(header, "BSDIFF40", 8) != 0){
		printf("Corrupt patch\n\r");
		return 0;
	}

	ctr_len = offtin(header + 8);
	newsize = offtin(header + 24);

	cpf = bfopen(patch+32, ctr_len);
	if ((cpfbz2 = BZ2_bzReadOpen(&cbz2err, cpf, 0, 0, NULL, 0)) == NULL){
		printf("BZ2_bzReadOpen, bz2err = %d\n\r", cbz2err);
		return 0;
	}

	while (newpos < newsize) {
		/* Read control data */
		for (i = 0;i <= 2;i++) {
			lenread = BZ2_bzRead(&cbz2err, cpfbz2, buf, 8);
			if ((lenread < 8) || ((cbz2err != BZ_OK) &&
				(cbz2err != BZ_STREAM_END)))
			{
				printf("Corrupt patch %d %d\n\r", __LINE__, cbz2err);
				return 0;
			}
			ctrl[i] = offtin(buf);
		};

		/* Sanity-check */
		if (newpos + ctrl[0] > newsize){
			printf("Corrupt patch %d\n\r", __LINE__);
			return 0;
		}

		/* Adjust pointers */
		newpos += ctrl[0];
		oldpos += ctrl[0];

		/* Sanity-check */
		if (newpos + ctrl[1] > newsize){
			printf("Corrupt patch %d\n\r", __LINE__);
			return 0;
		}

		/* Adjust pointers */
		newpos += ctrl[1];
		oldpos += ctrl[2];
	};
	BZ2_bzReadClose(&cbz2err, cpfbz2);
	bfclose(cpf);
	oldpos -= ctrl[2];
	return oldpos;
#endif
}

int is_patch_empty(char *patch)
{
#ifndef CMP_LZMA
	char *header;
	uint32_t newsize, lenread, ctr_len;
	long ctrl[3];
	BFILE *cpf;
	BZFILE * cpfbz2;
	header = patch;
	int cbz2err, i;
	uint8_t buf[8];

	if(memcmp(header, "BSDIFF40", 8) != 0){
		return 0;
	}

	ctr_len = offtin(header + 8);
	newsize = offtin(header + 24);

	cpf = bfopen(patch+HEADER_SIZE, ctr_len);
	if ((cpfbz2 = BZ2_bzReadOpen(&cbz2err, cpf, 0, 0, NULL, 0)) == NULL){
		printf("BZ2_bzReadOpen, cbz2err = %d\n\r", cbz2err);
		return 0;
	}

	/* Read control data */
	for (i = 0;i <= 2;i++) {
		lenread = BZ2_bzRead(&cbz2err, cpfbz2, buf, 8);
		if ((lenread < 8) || ((cbz2err != BZ_OK) &&
			(cbz2err != BZ_STREAM_END)))
			return 0;
		ctrl[i] = offtin(buf);
	};

	if( (ctrl[0] == newsize) && ((ctrl[0] + ctrl[2]) == 0) && 
		(ctrl[1] == 0) )
		return 1; //newfile == oldfile, patch is empty
#endif
	return 0;
}

char *bzpacth(char* in, int in_len, char* patch, int patch_len, int *out_len, int OldFileOffset)
{
	uint32_t ctr_len, data_len, extra_len, new_size;
	char *header, *newfile;
#ifndef CMP_LZMA
	char *pctrl, *pdata, *pextra; 
	BFILE *cpf, *dpf, *epf;
	BZFILE * cpfbz2, *dpfbz2, *epfbz2;
	int cbz2err, dbz2err, ebz2err;
#endif
	long ctrl[3];
	uint8_t buf[8];

	uint32_t i, oldpos, newpos;
#ifndef CMP_LZMA
	uint32_t lenread;
	cpf = dpf = epf = NULL;
	cpfbz2 = dpfbz2 = epfbz2 = NULL;
#endif
#ifdef CMP_LZMA
	uint8_t *cpfbz2_buf=NULL;
	uint8_t *dpfbz2_buf=NULL;
	uint8_t *epfbz2_buf=NULL;
#endif

	header = patch;
	if(memcmp(header, "BSDIFF40", 8) != 0){
		goto err_corrupt_patch;
	}

	ctr_len = offtin((uint8_t *)header + 8);
	data_len = offtin((uint8_t *)header + 16);
	new_size = offtin((uint8_t *)header + 24);
	extra_len = patch_len - ctr_len - data_len;
	//printf("ctr_len: %d, data_len: %d, extra_len: %d, new_size: %d\n\r", ctr_len, data_len, extra_len, new_size);

	newfile = malloc(new_size+1);
	if(newfile == NULL){
		uart_printf("Malloc error\n\r");
		goto err;
	}

#ifndef CMP_LZMA
	cpf = bfopen(patch+HEADER_SIZE, ctr_len);
	if ((cpfbz2 = BZ2_bzReadOpen(&cbz2err, cpf, 0, 0, NULL, 0)) == NULL){
		uart_printf("BZ2_bzReadOpen, cbz2err = %d\n\r", cbz2err);
		goto err;
	}

	dpf = bfopen(patch+HEADER_SIZE+ctr_len, data_len);
	if ((dpfbz2 = BZ2_bzReadOpen(&dbz2err, dpf, 0, 0, NULL, 0)) == NULL){
		uart_printf("BZ2_bzReadOpen, dbz2err = %d\n\r", dbz2err);
		goto err;
	}

	epf = bfopen(patch+HEADER_SIZE+ctr_len+data_len, extra_len);
	if ((epfbz2 = BZ2_bzReadOpen(&ebz2err, epf, 0, 0, NULL, 0)) == NULL){
		uart_printf("BZ2_bzReadOpen, ebz2err = %d\n\r", ebz2err);
		goto err;
	}
#endif
#ifdef CMP_LZMA
		cpfbz2_buf = (uint8_t *)(patch + HEADER_SIZE);
		dpfbz2_buf = (uint8_t *)(patch + HEADER_SIZE + ctr_len);
		epfbz2_buf = (uint8_t *)(patch + HEADER_SIZE + ctr_len + data_len);
#endif
	oldpos = 0;newpos = 0;
	while (newpos < new_size) {
		/* Read control data */
		for (i = 0;i <= 2;i++) {
#ifdef CMP_LZMA
			memcpy(buf,cpfbz2_buf,8);
			cpfbz2_buf += 8;
#else
			lenread = BZ2_bzRead(&cbz2err, cpfbz2, buf, 8);
			if ((lenread < 8) || ((cbz2err != BZ_OK) &&
				(cbz2err != BZ_STREAM_END)))
				goto err_corrupt_patch;
#endif
			ctrl[i] = offtin(buf);
		};

		/* Sanity-check */
		if (newpos + ctrl[0] > new_size)
			goto err_corrupt_patch;

		/* Read diff string */
#ifdef CMP_LZMA
		memcpy(newfile + newpos,dpfbz2_buf,ctrl[0]);
		dpfbz2_buf += ctrl[0];
#else
		lenread = BZ2_bzRead(&dbz2err, dpfbz2, newfile + newpos, ctrl[0]);
		if ((lenread < ctrl[0]) ||
			((dbz2err != BZ_OK) && (dbz2err != BZ_STREAM_END)))
		{
			goto err_corrupt_patch;
		}
#endif
		/* Add pold data to diff string */
		for (i = 0;i < ctrl[0];i++){
			if ((oldpos + i >= 0) && (oldpos + i < in_len)){
				newfile[newpos + i] += in[oldpos + i];
			}
		}
				

		/* Adjust pointers */
		newpos += ctrl[0];
		oldpos += ctrl[0];

		/* Sanity-check */
		if (newpos + ctrl[1] > new_size)
			goto err_corrupt_patch;

		/* Read extra string */
#ifdef CMP_LZMA
		//lenread=read(epf,new + newpos,ctrl[1]);
		memcpy(newfile + newpos,epfbz2_buf,ctrl[1]);
		epfbz2_buf += ctrl[1];
#else
		lenread = BZ2_bzRead(&ebz2err, epfbz2, newfile + newpos, ctrl[1]);
		if ((lenread < ctrl[1]) ||
			((ebz2err != BZ_OK) && (ebz2err != BZ_STREAM_END)))
		{
			goto err_corrupt_patch;
		}
#endif

		/* Adjust pointers */
		newpos += ctrl[1];
		oldpos += ctrl[2];
	};

	*out_len = new_size;
	
#ifndef CMP_LZMA
	BZ2_bzReadClose(&cbz2err, cpfbz2);
	BZ2_bzReadClose(&dbz2err, dpfbz2);
	BZ2_bzReadClose(&ebz2err, epfbz2);
	bfclose(cpf);
	bfclose(dpf);
	bfclose(epf);
#endif

	return newfile;

err_corrupt_patch:
	uart_printf("Bspatch: corrupt patch\n\r");
err:
#ifndef CMP_LZMA
	if(cpfbz2) BZ2_bzReadClose(&cbz2err, cpfbz2);
	if(dpfbz2) BZ2_bzReadClose(&dbz2err, dpfbz2);
	if(epfbz2) BZ2_bzReadClose(&ebz2err, epfbz2);
	if(cpf) bfclose(cpf);
	if(dpf) bfclose(dpf);
	if(epf) bfclose(epf);
#endif
	if(newfile) free(newfile);

	return NULL;
}

void fota_delay()
{
	int i,j,k;
	for(i=0;i<10000;i++){
		for(j=0;j<1000;j++){
			k++;
		}
	}
}

UINT32 OTA_Update_FotaParam(UINT32 buf, UINT32 size, UINT32 flash_Addr)
{
	UINT32 Retval = GeneralError;
	char *temp=NULL;
	uart_printf("OTA_Update_FotaParam, flash_Addr[0x%08X],size[0x%08X]\n\r", flash_Addr,size);	
	
	temp =(char *) malloc(FLASH_4K_SIZE);
	memcpy(temp,(char *)buf,size);
	
	
	Retval=asr_norflash_erase(flash_Addr, FLASH_4K_SIZE);
	if (Retval != NoError)
		goto exit;
	
	#if 0
	uart_printf("+++++++++++++++++++++++++++++OTA_Update_FotaParam,delay for poweroff test++++++++++++++\n\r");	
	fota_delay();
	fota_delay();
	fota_delay();
	uart_printf("-----------------------------OTA_Update_FotaParam,delay end----------------------------\n\r");	
	#endif
	
	Retval = asr_norflash_write(flash_Addr, temp, FLASH_4K_SIZE);
	if (Retval != NoError)
		goto exit;
exit:
	free(temp);
	uart_printf("OTA_Update_FotaParam end\n\r");	
	return Retval;
	
}


UINT32 OTA_Update_FotaImage(UINT32 buf, UINT32 size, UINT32 flash_Addr)
{
	UINT32 Retval = GeneralError;
	char *temp=NULL;
	UINT32 index=0;
	uart_printf("OTA_Update_FotaImage: flash_Addr[0x%08x], size: 0x%08x\n\r", flash_Addr, size);
	flash_Addr=flash_Addr+QSPI_FLASH_BASE;
	temp =(char *) malloc(FLASH_4K_SIZE);


	while(size > FLASH_4K_SIZE){
		memcpy(temp,(char *)(buf+index),FLASH_4K_SIZE);
		
		Retval=asr_norflash_erase(flash_Addr  + index, FLASH_4K_SIZE);
		if (Retval != NoError)
			return Retval;

		if(index==0){
#if 0
			uart_printf("+++++++++++++++++++++++++++++OTA_Update_FotaImage,delay for poweroff test++++++++++++++\n\r"); 
			fota_delay();
			uart_printf("-----------------------------OTA_Update_FotaImage,delay end----------------------------\n\r"); 
#endif

		}
		if(index%FLASH_1M_SIZE==0){
			uart_printf("\r\n");
		}

		if(index%CRANE_NOR_BLOCKSIZE==0){
			uart_printf("...");
		}
		
		Retval = asr_norflash_write(flash_Addr  + index, temp, FLASH_4K_SIZE);
		if (Retval != NoError)
			return Retval;
		
		index += FLASH_4K_SIZE;
		size -= FLASH_4K_SIZE;
		
	}


	if(size>0){
		memset(temp,0,FLASH_4K_SIZE);
		memcpy(temp,(char *)(buf+index),size);
		
		Retval=asr_norflash_erase(flash_Addr + index, FLASH_4K_SIZE);
		if (Retval != NoError)
			return Retval;

		Retval = asr_norflash_write(flash_Addr + index, temp, FLASH_4K_SIZE);
		if (Retval != NoError)
			return Retval;
		
		index += size;
		size -= size;
	}
	free(temp);
	uart_printf("OTA_Update_FotaImage end\n\r");
	return Retval;
	
}





UINT32 ImageDFotaDecode(UINT32 OldFileOffset, UINT32 OldFileLen, char * PatchAddr,
	UINT32 PatchLen, UINT32 NewFileOffset, UINT32 *NewFileLen, UINT32 *NewFileAddr)
{
	char *Patch=NULL, *OldFile=NULL, *NewFile=NULL;
	UINT32 NewLen = 0;
	UINT32 OldLen = OldFileLen;
	UINT32 Retval = NoError;
	//P_FlashProperties_T pFlashP = GetFlashProperties(BOOT_FLASH);

	
#ifdef CMP_LZMA

	void * pDst=NULL;
	unsigned long pulDstLen=0;
	int ii;
	Patch = (char *)PatchAddr + 5;
	for (ii = 0; ii < 4; ii++)
	{
		unsigned char b;
		b = *Patch;
		Patch ++;
		pulDstLen += (unsigned int)(b) << (ii * 8);
	}

	pulDstLen += 100;
	pDst = (void *)malloc(pulDstLen+1);
	memset(pDst,0,pulDstLen+1);
	
	unsigned long ret=LZMA_Decompress(pDst, &pulDstLen, (void *)PatchAddr, PatchLen);
	uart_printf("LZMA_Decompress ret %d\n\r",ret);
	if(ret!=0){
		Retval = DFota_LZMADecompressFailed;
		goto error;
	}


#endif

	OldFile = NewFile = NULL;
	*NewFileLen = 0;

#ifdef CMP_LZMA
	Patch = (char *)pDst;
	PatchLen = pulDstLen;
#else
	Patch = (char *)PatchAddr;
#endif


	//Sometimes, we don't know the orignal old file size, we can calculate
	//it through control information in the patch, however, it may be a little
	//smaller than the real value, increase it by 0x1000.
	//TODO, need to find a better to get the old file size
	if(OldLen == 0){
		OldLen = get_oldsize(Patch);
		//if(OldLen < 0){
		//	Retval = DFota_GetOldFileSizeError;
		//	goto error;
		//}
		OldLen += 0x1000;
	}

	OldFile = malloc(OldLen);
	if(OldFile == NULL)
	{
		uart_printf("Malloc Error\n\r");
		Retval = NULLPointer;
		goto error;
	}
	
	if(OldLen > 0x1000){
		Retval = asr_norflash_read((OldFileOffset+QSPI_FLASH_BASE), OldFile, OldLen-0x1000);
		if (Retval != NoError){
			goto error;
		}
	}


	NewFile = bzpacth(OldFile, OldLen, Patch, PatchLen, (int *)&NewLen,OldFileOffset);
	if(NewFile == 0 || NewLen == 0)
	{
		Retval = DFota_BspatchFailed;
		goto error;
	}

	//EraseLen=((NewLen%BlockSize) > 0) ? ((NewLen + BlockSize) & (~(BlockSize - 1))) : NewLen;

	//EraseLen = (NewLen + BlockSize) & (~(BlockSize - 1)); //Align with Block size
	
	//uart_printf("Flash_Start_Address: 0x%08X, NewLen: 0x%08X, EraseLen: 0x%08X\n\r", NewFileOffset, NewLen,EraseLen);

	Retval = OTA_Update_FotaImage( (UINT32)NewFile, NewLen, NewFileOffset);
	if(Retval != NoError) goto error;

	*NewFileLen = NewLen;
	*NewFileAddr = (UINT32)NewFile;

	if(OldFile) free(OldFile);
	if(pDst) free(pDst);
	
	return NoError;

error:
	*NewFileLen = 0;
	uart_printf("ImageDFotaDecode Error!!! %x\n\r",Retval);
	if(pDst) free(pDst);
	if(OldFile) free(OldFile);
	if(NewFile) free(NewFile);

	return Retval;
}





UINT32 check_old_image_checksum(UINT32 flash_address, UINT32 length, UINT32 oldCheckSum)
{
#define CHECKSUM_IMAGE_BLOCK	128
	char temp[CHECKSUM_IMAGE_BLOCK];
	UINT32 index=0;
	UINT32 ImageChecksum=0;
	uart_printf("check_old_image_checksum flash_address[%08x] length[%08x]\n\r", flash_address,length);
	while(length>CHECKSUM_IMAGE_BLOCK){
		memcpy(temp,(char *)(flash_address+index),CHECKSUM_IMAGE_BLOCK);
		ImageChecksum=CalcImageChecksum((UINT32*)temp,CHECKSUM_IMAGE_BLOCK,ImageChecksum);
		length -= CHECKSUM_IMAGE_BLOCK;
		index += CHECKSUM_IMAGE_BLOCK;
	}
	if(length>0){
		memcpy(temp,(char*)(flash_address+index),length);
		ImageChecksum=CalcImageChecksum((UINT32*)temp,length,ImageChecksum);
	}
	uart_printf("check_old_image_checksum oldCheckSum[%08x]:ImageChecksum[%08x]\n\r", oldCheckSum,ImageChecksum);
	if(oldCheckSum!=ImageChecksum){
		return DFota_CRCFailedError;
	}else{
		return NoError;
	}
}



UINT32 DFOTA_Bspatch(struct fota_firmwar_flag * pFOTA_T, UINT32 FotaParamStartAddress, 
	UINT32 old_file_flash_address,UINT32 LoadAddr,UINT32 Image_ID,
	UINT32 image_num,UINT32 NewFileFlashOffset)
{
	UINT32 Retval = NoError;
	MasterBlockHeader *pMasterHeader = NULL;
	PDeviceHeader_11 pDevHeader_11=NULL;
	PImageStruct_11 pImage_11 = NULL;

	UINT32 temp_p = NULL;
	UINT32 imageChecksum = 0;
	UINT32 imagenum;
	UINT32 NewFileAddr = 0, NewFileLen = 0;
	//P_FlashProperties_T pFlashP = GetFlashProperties(BOOT_FLASH);

	UINT32 EraseLen;
	UINT32 ImageAddr;
	int new_id=0;
	int old_id=0;


	pMasterHeader = (MasterBlockHeader *)LoadAddr;
	temp_p = pMasterHeader->deviceHeaderOffset[0] + LoadAddr;
	pDevHeader_11 = (PDeviceHeader_11)temp_p;
	
	uart_printf("ENTER DFOTA_Bspatch, old_file_flash_address[0x%08X],Image_ID[0x%08X]\n\r", 
		old_file_flash_address, Image_ID);


	for ( imagenum = pDevHeader_11->nOfImages; imagenum > 0; imagenum -- )
	{
		temp_p = (UINT32)&pDevHeader_11->imageStruct_11[imagenum - 1];
		pImage_11 = (PImageStruct_11)temp_p;


		if( pFOTA_T->image_status[imagenum-1]==IMAGE_BSPATCH_READY
			|| pImage_11->Image_ID != Image_ID
			|| imagenum == image_num)
			continue;

		if(old_file_flash_address == pImage_11->Old_File_Flash_Address){
			new_id = (pImage_11->Flash_Start_Address - Dfota_Flash_Start_Address)/pImage_11->Flash_erase_size + 1;
			if(pImage_11->Old_File_Flash_Address==0){
				old_id = 0;
			}else{
				old_id = (pImage_11->Old_File_Flash_Address-Dfota_Flash_Start_Address)/pImage_11->Flash_erase_size + 1;
			}
			uart_printf("%s: handle_file_%d_%d\n\r",__FUNCTION__,new_id,old_id);

			
			Retval=DFOTA_Bspatch( pFOTA_T, FotaParamStartAddress, pImage_11->Flash_Start_Address,LoadAddr,Image_ID,imagenum,NewFileFlashOffset);
			if(Retval != NoError)
				goto error;
			if(Retval == NoError){
				ImageAddr = LoadAddr+((pImage_11->First_Sector+7)<<ALIGN_1K_SIZE);
				
				if(pImage_11->ChecksumFormatVersion2==0){
						continue;
				}
			
				// add checksum for images in FBF file
				imageChecksum = CalcImageChecksum((UINT32*)ImageAddr, pImage_11->length,0);
				if (imageChecksum != pImage_11->ChecksumFormatVersion2){
					Retval = CRCFailedError;
					goto error;
				}
		
		
				Retval = (UINT32)ImageDFotaDecode(
						pImage_11->Old_File_Flash_Address, 		//oldfile in flash
						0,										//oldfile len
						(char *)ImageAddr,								//patch addr in memory
						pImage_11->length,						 //patch len
						NewFileFlashOffset, 					 //new 
						&NewFileLen,
						&NewFileAddr
						);

				if(Retval != NoError || NewFileLen == 0) {
					Retval = DFota_ImageDFotaDecodeFailed;
					goto error;
				}
		
		
				//power off protection
				pFOTA_T->DFota_nOfImages = imagenum - 1;
				pFOTA_T->DFota_NeedCopyOnly =1; 
				pFOTA_T->DFota_CopyLen = NewFileLen;
				pFOTA_T->image_status[imagenum - 1]=IMAGE_BSPATCH_READY;
				pFOTA_T->index ++;
				pFOTA_T->checksum = CalcImageChecksum((UINT32*)pFOTA_T,sizeof(struct fota_firmwar_flag)-sizeof(unsigned int),0);
				Retval=OTA_Update_FotaParam((UINT32)pFOTA_T,sizeof(struct fota_firmwar_flag),FotaParamStartAddress);
				if (Retval != NoError )
					goto error;	
				
				EraseLen = (pImage_11->Flash_erase_size > NewFileLen) ? pImage_11->Flash_erase_size : NewFileLen;
			
				uart_printf("%s: Flash_Start_Address: 0x%08X, Old_File_Flash_Address: 0x%08X NewFileLen: 0x%08X, EraseLen: 0x%08X, length=%d\n\r", 
					__FUNCTION__,
					pImage_11->Flash_Start_Address, 
					pImage_11->Old_File_Flash_Address,
					NewFileLen,
					EraseLen,
					pImage_11->length);

				Retval= OTA_Update_FotaImage(NewFileAddr, NewFileLen, pImage_11->Flash_Start_Address);
				if (Retval != NoError )
					goto error;

				if(NewFileAddr){
					free((void *)NewFileAddr);
					NewFileAddr = 0;
				}
				//power off protection
				//pFOTA_T->DFota_nOfImages = imagenum - 1;
				pFOTA_T->DFota_NeedCopyOnly = 0; 
				pFOTA_T->DFota_CopyLen = 0;
				pFOTA_T->checksum = CalcImageChecksum((UINT32*)pFOTA_T,sizeof(struct fota_firmwar_flag)-sizeof(unsigned int),0);
				Retval = OTA_Update_FotaParam((UINT32)pFOTA_T,sizeof(struct fota_firmwar_flag),FotaParamStartAddress + FLASH_4K_SIZE);
				if (Retval != NoError )
					goto error;
			}

				
		}

	}

	uart_printf("LEAVE DFOTA_Bspatch\n\r");

	return NoError;
error:
	if(NewFileAddr) free((void*)NewFileAddr);
	uart_printf("DFOTA_Bspatch Error!!! Retval=%x\n\r",Retval);
	return Retval;	
}



UINT32 DFOTA_Power_Off_Protection(struct fota_firmwar_flag* pFOTA_T,UINT32 FotaParamStartAddress,UINT32 NewFileFlashOffset,UINT32 LoadAddr)
{
	UINT32 Retval = NoError;
	UINT32 imagenum;
	UINT32 temp_p = NULL;
	PImageStruct_11 pImage_11 = NULL;
	UINT32 NewFileAddr = 0, NewFileLen = 0;

	MasterBlockHeader *pMasterHeader = NULL;
	PDeviceHeader_11 pDevHeader_11=NULL;
	
	uart_printf("ENTER DFOTA_Power_Off_Protection\n\r");
	imagenum = pFOTA_T->DFota_nOfImages;	
	pMasterHeader = (MasterBlockHeader *)LoadAddr;

	temp_p = pMasterHeader->deviceHeaderOffset[0] + LoadAddr;
	pDevHeader_11 = (PDeviceHeader_11)temp_p;

	temp_p = (UINT32)&pDevHeader_11->imageStruct_11[imagenum];
	pImage_11 = (PImageStruct_11)temp_p;	
		
	
	NewFileLen = pFOTA_T->DFota_CopyLen;
	NewFileAddr  = (UINT32)malloc(NewFileLen);
	if(NewFileAddr == 0){
		Retval = NULLPointer;
		goto error;
	}

    Retval = asr_norflash_read((NewFileFlashOffset+QSPI_FLASH_BASE),NewFileAddr,NewFileLen);
	if(Retval != NoError){
		goto error;
	}

	
	//EraseLen = (pImage_11->Flash_erase_size > NewFileLen) ? pImage_11->Flash_erase_size : NewFileLen;

	Retval=OTA_Update_FotaImage( NewFileAddr, NewFileLen, pImage_11->Flash_Start_Address);
	if (Retval != NoError )
		goto error;
	
	if(NewFileAddr) 
	{
		free((void *)NewFileAddr);
		NewFileAddr = 0;
	}
	
	//power off protection
	//pFOTA_T->DFota_nOfImages = imagenum - 1;
	pFOTA_T->DFota_NeedCopyOnly = 0; 
	pFOTA_T->DFota_CopyLen = 0;
	pFOTA_T->checksum = CalcImageChecksum((UINT32*)pFOTA_T,sizeof(struct fota_firmwar_flag)-sizeof(unsigned int),0);
	Retval=OTA_Update_FotaParam((UINT32)pFOTA_T,sizeof(struct fota_firmwar_flag),FotaParamStartAddress+FLASH_4K_SIZE);
	if (Retval != NoError )
		goto error;
	
	return NoError;
error:
	uart_printf("DFOTA_Power_Off_Protection failed: 0x%.08x\n\r", Retval);

	if(NewFileAddr) free((void *)NewFileAddr);
	return Retval;

}


UINT32 check_old_segment_image_checksum(UINT32 nOfImages, PDeviceHeader_11 pDevHeader_11)
{
	UINT32 imagenum;
	UINT32 temp_p = NULL;

	UINT32 Retval;
	PImageStruct_11 pImage_11 = NULL;
	for ( imagenum = nOfImages; imagenum > 0; imagenum -- )
	{
		temp_p = (UINT32)&pDevHeader_11->imageStruct_11[imagenum - 1];
		pImage_11 = (PImageStruct_11)temp_p;
		if(pImage_11->Old_Image_len==0)
			continue;
		Retval=check_old_image_checksum(pImage_11->Old_File_Flash_Address+QSPI_FLASH_BASE,pImage_11->Old_Image_len,pImage_11->Old_Image_Checksum);
		if(Retval!=NoError){
			return DFota_CRCFailedError;
		}
	}
	return NoError;
}

UINT32 DFOTA_Upgrade(struct fota_firmwar_flag * pFOTA_T, unsigned long FotaParamStartAddress)
{
	UINT32 Retval = GeneralError;
	UINT32 LoadAddr = 0;
	MasterBlockHeader *pMasterHeader = NULL;
	PDeviceHeader_11 pDevHeader_11=NULL;
	PImageStruct_11 pImage_11 = NULL;


	UINT32 temp_p = NULL;

	UINT32 imageChecksum = 0;
	UINT32 imagenum;
	UINT32 NewFileFlashOffset, NewFileAddr = 0, NewFileLen = 0;
	//P_FlashProperties_T pFlashP = GetFlashProperties(BOOT_FLASH);
	UINT32 BlockSize = CRANE_NOR_BLOCKSIZE;//64K
	UINT32 EraseLen;
	UINT32 ImageAddr;
	int new_id=0;
	int old_id=0;


	/*
	if(pFOTA_T->checksum!=CalcImageChecksum(pFOTA_T,sizeof(struct fota_firmwar_flag)-sizeof(unsigned int),0)){
		uart_printf("fota param checsum failed: [%08x]:[%08x]\n\r", pFOTA_T->checksum,CalcImageChecksum(pFOTA_T,sizeof(struct fota_firmwar_flag)-sizeof(unsigned int),0));
	    Retval = DFota_CRCFailedError;
		goto error;

	}
	if(pFOTA_T->DFota_nOfImages==0xFFFFFFFF){
		memcpy(&FOTA_PARA_2,FotaParamStartAddress)
	}
	*/
#if 0
    UINT32 stack=0x7e900000;
	//The bspatch/lzma lib needs malloc/free APIs
	UINT32 poolLen = 0x00600000;
	malloc_init(stack, poolLen);
#endif
	uart_printf("FBF_Size: 0x%08x\n\r", pFOTA_T->fbf_file_size);
	uart_printf("fbf_flash_address: 0x%08x\n\r", pFOTA_T->fbf_flash_address);
	uart_printf("get_fota_pkg_start_addr: 0x%08x\n\r", get_fota_pkg_start_addr());

	
	LoadAddr = (UINT32)malloc(pFOTA_T->fbf_file_size);
	if(LoadAddr == 0){
		uart_printf("malloc error\n\r");
		return NULLPointer;
	}
    //uart_printf("LoadAddr: 0x%08x\n\r", LoadAddr);
	memset((void *)LoadAddr,0,pFOTA_T->fbf_file_size);

	
	
	memcpy((void *)LoadAddr,(void*)(pFOTA_T->fbf_flash_address+QSPI_FLASH_BASE),pFOTA_T->fbf_file_size);
	//Retval = asr_norflash_read((pFOTA_T->fbf_flash_address,LoadAddr+QSPI_FLASH_BASE),pFOTA_T->fbf_file_size);
	//if (Retval != NoError){
	//	goto error;
	//}
	//fota_dump_buffer("fbf_flash",(unsigned char *)LoadAddr,0,sizeof(MasterBlockHeader));

	NewFileFlashOffset = pFOTA_T->fbf_flash_address + pFOTA_T->fbf_file_size;
	NewFileFlashOffset = (NewFileFlashOffset + BlockSize) & (~(BlockSize - 1)); //Align with 64KB(Block size)

	uart_printf("NewFileFlashOffset[0x%08x]\n\r",NewFileFlashOffset);	
	pMasterHeader = (MasterBlockHeader *)LoadAddr;

	if (pMasterHeader->Format_Version != 11){
	    Retval = FBF_VersionNotMatch;
		goto error;
	}

	if (pMasterHeader->nOfDevices != 1){
	    Retval = FBF_DeviceMoreThanOne;
		goto error;
	}

	temp_p = pMasterHeader->deviceHeaderOffset[0] + LoadAddr;
	pDevHeader_11 = (PDeviceHeader_11)temp_p;

    /*
	// skip blocks
	skip_blocks = pDevHeader_11->FlashOpt.Skip_Blocks_Struct.Total_Number_Of_SkipBlocks;
	if (skip_blocks > 0)
		//pSkipAddress = &pDevHeader_11->FlashOpt.Skip_Blocks_Struct;
	else
		//pSkipAddress = NULL;

    */
    
    
	if(pFOTA_T->DFota_nOfImages==0xFFFFFFFF){
		pFOTA_T->DFota_nOfImages= pDevHeader_11->nOfImages;
		imagenum = pDevHeader_11->nOfImages;
		temp_p = (UINT32)&pDevHeader_11->imageStruct_11[imagenum - 1];
		pImage_11 = (PImageStruct_11)temp_p;

		Retval=check_old_image_checksum(pImage_11->Flash_Start_Address+QSPI_FLASH_BASE, pDevHeader_11->FlashOpt.Length, pDevHeader_11->FlashOpt.CheckSum);
		if (Retval != NoError){
			Retval = DFota_CRCFailedError;
			goto error;
		}

	}
	
	uart_printf("pFOTA_T->DFota_nOfImages: %d,pDevHeader_11->nOfImages: %d\n\r", pFOTA_T->DFota_nOfImages,pDevHeader_11->nOfImages);

	if(pFOTA_T->DFota_NeedCopyOnly){
		Retval = DFOTA_Power_Off_Protection(pFOTA_T,FotaParamStartAddress,NewFileFlashOffset,LoadAddr);
		if (Retval != NoError){
			goto error;
		}
	}

	for ( imagenum = pDevHeader_11->nOfImages; imagenum > 0; imagenum -- )
	{
		temp_p = (UINT32)&pDevHeader_11->imageStruct_11[imagenum - 1];
		pImage_11 = (PImageStruct_11)temp_p;

		if(Dfota_Image_ID!=pImage_11->Image_ID){
			uart_printf("Get New Image_ID: 0x%08x\n\r", pImage_11->Image_ID);
			uart_printf("Dfota_Flash_Start_Address: 0x%08x\n\r", pImage_11->Flash_Start_Address);
			Dfota_Image_ID = pImage_11->Image_ID;
			Dfota_Flash_Start_Address = pImage_11->Flash_Start_Address;
		}


		if(pFOTA_T->image_status[imagenum-1]==IMAGE_BSPATCH_READY){
			continue;
		}	
		ImageAddr = LoadAddr+((pImage_11->First_Sector+7)<<ALIGN_1K_SIZE);




		if(pImage_11->ChecksumFormatVersion2==0){
			continue;
		}
		

		{
			
			new_id = (pImage_11->Flash_Start_Address - Dfota_Flash_Start_Address)/pImage_11->Flash_erase_size + 1;
			if(pImage_11->Old_File_Flash_Address==0){
				old_id = 0;
			}else{
				old_id = (pImage_11->Old_File_Flash_Address-Dfota_Flash_Start_Address)/pImage_11->Flash_erase_size + 1;
			}
			uart_printf("++++++++++++++++++++++++handle_file_%d_%d++++++++++++++++++++++++++\n\r",new_id,old_id);	
			//uart_printf("%s: handle_file_%d_%d\n\r",__FUNCTION__,new_id,old_id);
			
			// add checksum for images in FBF file
			imageChecksum = CalcImageChecksum((UINT32*)ImageAddr, pImage_11->length,0);
			if (imageChecksum != pImage_11->ChecksumFormatVersion2){
				Retval = CRCFailedError;
				goto error;
			}
			
			Retval=DFOTA_Bspatch(pFOTA_T,FotaParamStartAddress,pImage_11->Flash_Start_Address,LoadAddr,pImage_11->Image_ID,imagenum,NewFileFlashOffset);
			if(Retval != NoError){
				goto error;
			}
			

			Retval = (UINT32)ImageDFotaDecode(
					pImage_11->Old_File_Flash_Address,      //oldfile in flash
					0,                                      //oldfile len
					(char *)ImageAddr,  							//patch addr in memory
					pImage_11->length,                       //patch len
					NewFileFlashOffset,                      //new 
					&NewFileLen,
					&NewFileAddr);

			if(Retval != NoError || NewFileLen == 0) {
				Retval = DFota_ImageDFotaDecodeFailed;
				goto error;
			}

			//power off protection
			pFOTA_T->DFota_nOfImages = imagenum - 1;
			pFOTA_T->DFota_NeedCopyOnly =1; 
			pFOTA_T->DFota_CopyLen = NewFileLen;
			pFOTA_T->image_status[imagenum - 1] = IMAGE_BSPATCH_READY;
			pFOTA_T->index ++;
			pFOTA_T->checksum = CalcImageChecksum((UINT32*)pFOTA_T,sizeof(struct fota_firmwar_flag)-sizeof(unsigned int),0);
			Retval=OTA_Update_FotaParam((UINT32)pFOTA_T,sizeof(struct fota_firmwar_flag),FotaParamStartAddress);
			if (Retval != NoError )
				goto error;
			
		}
		
		EraseLen = ((NewFileLen%BlockSize) > 0) ? ((NewFileLen + BlockSize) & (~(BlockSize - 1))) : NewFileLen; //Align with Block size

		uart_printf("%s:Flash_Start_Address: 0x%08X, NewFileLen: 0x%08X, EraseLen: 0x%08X, length=0x%08X\n\r", 
			__FUNCTION__,pImage_11->Flash_Start_Address, NewFileLen,EraseLen,pImage_11->length);
		Retval=OTA_Update_FotaImage( NewFileAddr, NewFileLen, pImage_11->Flash_Start_Address);
		if (Retval != NoError )
			goto error;
		
		if(NewFileAddr){ 
			free((void *)NewFileAddr);
			NewFileAddr = 0;
		}

		//power off protection
		//pFOTA_T->DFota_nOfImages = imagenum - 1;
		pFOTA_T->DFota_NeedCopyOnly = 0; 
		pFOTA_T->DFota_CopyLen = 0;
		pFOTA_T->checksum = CalcImageChecksum((UINT32*)pFOTA_T,sizeof(struct fota_firmwar_flag)-sizeof(unsigned int),0);
		Retval=OTA_Update_FotaParam((UINT32)pFOTA_T,sizeof(struct fota_firmwar_flag),FotaParamStartAddress+FLASH_4K_SIZE);
		if (Retval != NoError )
			goto error;
		uart_printf("-------------------------------end---------------------------------\n\r");	

		
	}  //for loop

	free((void*)LoadAddr);
	LoadAddr=0;

	return NoError;


error:
	//free buffers
	uart_printf("DFOTA_Upgrade Error!!! Retval=%x\n\r", Retval);
	if(NewFileAddr)free((void*)NewFileAddr);
	if(LoadAddr) free((void*)LoadAddr);

	return Retval;

}

UINT32 FOTA_Upgrade(struct fota_firmwar_flag * pFOTA_T, unsigned long FotaParamStartAddress)
{
    UINT32 Retval = GeneralError;
    UINT32 LoadAddr = 0;
    MasterBlockHeader *pMasterHeader = NULL;
    PDeviceHeader_11 pDevHeader_11=NULL;
    PImageStruct_11 pImage_11 = NULL;
    UINT32 temp_p = NULL;
    UINT32 imageChecksum = 0;
    UINT32 imagenum;
    UINT32 ImageAddr;

    uart_printf("FBF_Size: 0x%08x\n\r", pFOTA_T->fbf_file_size);
    uart_printf("fbf_flash_address: 0x%08x\n\r", pFOTA_T->fbf_flash_address);
    uart_printf("get_fota_pkg_start_addr: 0x%08x\n\r", get_fota_pkg_start_addr());

    
    LoadAddr = (UINT32)malloc(pFOTA_T->fbf_file_size);
    if(LoadAddr == 0){
        uart_printf("malloc error\n\r");
        return NULLPointer;
    }
    //uart_printf("LoadAddr: 0x%08x\n\r", LoadAddr);
    memset((void *)LoadAddr,0,pFOTA_T->fbf_file_size);


    
    memcpy((void *)LoadAddr,(void*)(pFOTA_T->fbf_flash_address+QSPI_FLASH_BASE),pFOTA_T->fbf_file_size);
 
    pMasterHeader = (MasterBlockHeader *)LoadAddr;

    if (pMasterHeader->Format_Version != 11){
        Retval = FBF_VersionNotMatch;
        goto error;
    }

    if (pMasterHeader->nOfDevices != 1){
        Retval = FBF_DeviceMoreThanOne;
        goto error;
    }

    temp_p = pMasterHeader->deviceHeaderOffset[0] + LoadAddr;
    pDevHeader_11 = (PDeviceHeader_11)temp_p;
    
    uart_printf("pDevHeader_11->nOfImages: %d\n\r",pDevHeader_11->nOfImages);


    for ( imagenum = pDevHeader_11->nOfImages; imagenum > 0; imagenum -- )
    {
        temp_p = (UINT32)&pDevHeader_11->imageStruct_11[imagenum - 1];
        pImage_11 = (PImageStruct_11)temp_p;
 
        ImageAddr = LoadAddr+((pImage_11->First_Sector+7)<<ALIGN_1K_SIZE);
        
        // add checksum for images in FBF file
        imageChecksum = CalcImageChecksum((UINT32*)ImageAddr, pImage_11->length,0);
        if (imageChecksum != pImage_11->ChecksumFormatVersion2){
            Retval = CRCFailedError;
            uart_printf("fbf file CORRUPT,checksum failed\n\r");
            goto error;
        }

    }

    for ( imagenum = pDevHeader_11->nOfImages; imagenum > 0; imagenum -- )
    {
        temp_p = (UINT32)&pDevHeader_11->imageStruct_11[imagenum - 1];
        pImage_11 = (PImageStruct_11)temp_p;
 
        ImageAddr = LoadAddr+((pImage_11->First_Sector+7)<<ALIGN_1K_SIZE);

        if(pImage_11->ChecksumFormatVersion2==0){
            continue;
        }

        
        uart_printf("%s:Flash_Start_Address: 0x%08X, EraseLen: 0x%08X, length=0x%08X\n\r", 
            __FUNCTION__,pImage_11->Flash_Start_Address,pImage_11->Flash_erase_size,pImage_11->length);
        Retval=OTA_Update_FotaImage( ImageAddr, pImage_11->length, pImage_11->Flash_Start_Address);
        if (Retval != NoError )
            goto error;
        
    }  //for loop

    free((void*)LoadAddr);
    LoadAddr=0;

    return NoError;


error:
    //free buffers
    uart_printf("FOTA_Upgrade Error!!! Retval=%x\n\r", Retval);
    if(LoadAddr) free((void*)LoadAddr);

    return Retval;

}



UINT32 Mini_Sys_Check_Dfota_File(unsigned char* buf,asr_master_head_info *pMasterHeader)
{
	UINT32 i=0,j=0,checksum=0,imageIdex=0;
	imageIdex = *(UINT32*)(buf+8);
	imageIdex += 12;
	for ( i=0;i< (pMasterHeader->device[0].nOfImages+pMasterHeader->device[1].nOfImages); i++ )
	{
		checksum = 0;
		if(i<pMasterHeader->device[0].nOfImages)
		{
			checksum=CalcImageChecksum(buf+imageIdex,pMasterHeader->device[0].Image[i].ImageLen,0);
			if(checksum!=pMasterHeader->device[0].Image[i].ImageCheckSum){
				uart_printf("%s: device_1 image[%d],checksum[%08x] ImageCheckSum[%08x]\n\r",__FUNCTION__,i+1,checksum,pMasterHeader->device[0].Image[i].ImageCheckSum);
				return CRCFailedError;
			}
			//uart_printf("%s: device_1 image[%d],checksum[%08x] ImageCheckSum[%08x]\n\r",__FUNCTION__,i+1,checksum,pMasterHeader->device[0].Image[i].ImageCheckSum);
			imageIdex += pMasterHeader->device[0].Image[i].ImageLen;
		}else{
			j = i -	pMasterHeader->device[0].nOfImages;
			checksum=CalcImageChecksum(buf+imageIdex,pMasterHeader->device[1].Image[j].ImageLen,0);
			if(checksum!=pMasterHeader->device[1].Image[j].ImageCheckSum){
				uart_printf("%s: device_2 image[%d],checksum[%08x] ImageCheckSum[%08x]\n\r",__FUNCTION__,j+1,checksum,pMasterHeader->device[1].Image[j].ImageCheckSum);
				return CRCFailedError;
			}
			//uart_printf("%s: device_2 image[%d],checksum[%08x] ImageCheckSum[%08x]\n\r",__FUNCTION__,j+1,checksum,pMasterHeader->device[1].Image[j].ImageCheckSum);
			imageIdex += pMasterHeader->device[1].Image[j].ImageLen;

		}
	}
	return NoError;
}

UINT32 Mini_System_Bspatch(asr_master_head_info *pMasterHeader,UINT32 image_num,UINT32 NewFileFlashOffset,UINT32 deviceNum,unsigned char *image_status)
{
	UINT32 Retval = NoError;
	UINT32 imageChecksum = 0;
	UINT32 i;
	UINT32 NewFileAddr = 0, NewFileLen = 0;
	UINT32 ImageAddr;
	int old_id=0;

	for ( i=0;i< pMasterHeader->device[deviceNum].nOfImages; i++ )
	{

		if( *(image_status+i)==IMAGE_BSPATCH_READY|| i == image_num)
			continue;

		if((pMasterHeader->device[deviceNum].oldDeviceFlashAddress +image_num*CRANE_NOR_BLOCKSIZE) == pMasterHeader->device[deviceNum].Image[i].BaseFlashAddress){
			//new_id = (pImage_11->Flash_Start_Address - Dfota_Flash_Start_Address)/pImage_11->Flash_erase_size + 1;
			if(pMasterHeader->device[deviceNum].Image[i].BaseFlashAddress==0){
				old_id = 0;
			}else{
				old_id = (pMasterHeader->device[deviceNum].Image[i].BaseFlashAddress-pMasterHeader->device[deviceNum].oldDeviceFlashAddress)/CRANE_NOR_BLOCKSIZE + 1;
			}
			uart_printf("%s: handle_device_%d_file_%d_%d\n\r",__FUNCTION__,deviceNum,i+1,old_id);

			
			Retval=Mini_System_Bspatch( pMasterHeader, i,NewFileFlashOffset,deviceNum,image_status);
			if(Retval != NoError)
				goto error;
			if(Retval == NoError){
				ImageAddr = (UINT32)((char *)pMasterHeader - pMasterHeader->device[deviceNum].Image[i].ImageOffset);
			
				// add checksum for images in FBF file
				imageChecksum = CalcImageChecksum((UINT32*)ImageAddr, pMasterHeader->device[deviceNum].Image[i].ImageLen,0);
				if (imageChecksum != pMasterHeader->device[deviceNum].Image[i].ImageCheckSum){
					Retval = CRCFailedError;
					goto error;
				}
		
		
				Retval = (UINT32)ImageDFotaDecode(
						pMasterHeader->device[deviceNum].Image[i].BaseFlashAddress,//oldfile in flash
						0,										//oldfile len
						(char *)ImageAddr,								//patch addr in memory
						pMasterHeader->device[deviceNum].Image[i].ImageLen,						 //patch len
						NewFileFlashOffset, 					 //new 
						&NewFileLen,
						&NewFileAddr
						);

				if(Retval != NoError || NewFileLen == 0) {
					Retval = DFota_ImageDFotaDecodeFailed;
					goto error;
				}
		
				uart_printf("%s: Flash_Start_Address: 0x%08X, Old_File_Flash_Address: 0x%08X NewFileLen: 0x%08X, length=%d\n\r", 
					__FUNCTION__,
					pMasterHeader->device[deviceNum].oldDeviceFlashAddress +i*CRANE_NOR_BLOCKSIZE, 
					pMasterHeader->device[deviceNum].Image[i].BaseFlashAddress,
					NewFileLen,
					pMasterHeader->device[deviceNum].Image[i].newImageLen);

				Retval= OTA_Update_FotaImage(NewFileAddr, NewFileLen, pMasterHeader->device[deviceNum].oldDeviceFlashAddress +i*CRANE_NOR_BLOCKSIZE);
				if (Retval != NoError )
					goto error;

				if(NewFileAddr){
					free((void *)NewFileAddr);
					NewFileAddr = 0;
				}
				*(image_status+i)=IMAGE_BSPATCH_READY;

			}

				
		}

	}

	uart_printf("LEAVE DFOTA_Bspatch\n\r");

	return NoError;
error:
	if(NewFileAddr) free((void *)NewFileAddr);
	uart_printf("DFOTA_Bspatch Error!!! Retval=%x\n\r",Retval);
	return Retval;	
}



UINT32 Mini_System_Handle_Device(asr_master_head_info *pMasterHeader, unsigned long NewFileFlashOffset,UINT32 device_num)
{
	unsigned char image_status[MAX_NUMBER_OF_IMAGE_STRUCTS_IN_MASTER_HEADER];
	UINT32 i,checksum,offset=0;
	UINT32 ImageAddr;
	int old_id=0;
	UINT32 NewFileAddr = 0, NewFileLen = 0;
	UINT32 Retval = GeneralError;

	
	memset(image_status,IMAGE_BSPATCH_NO_READY,MAX_NUMBER_OF_IMAGE_STRUCTS_IN_MASTER_HEADER);
	uart_printf("%s device_num[%d] nOfImages[%d] oldDeviceFlashAddress[%08x]\n\r",
		__func__,
		device_num,
		pMasterHeader->device[device_num].nOfImages,
		pMasterHeader->device[device_num].oldDeviceFlashAddress);

	for(i=0;i<pMasterHeader->device[device_num].nOfImages;i++){
		uart_printf("%s offset[%08x] newImageLen[%08x]\n\r",
			__func__,
			offset,
			pMasterHeader->device[device_num].Image[i].newImageLen);

		checksum=CalcImageChecksum((UINT32*)(pMasterHeader->device[device_num].oldDeviceFlashAddress+offset+QSPI_FLASH_BASE),pMasterHeader->device[device_num].Image[i].newImageLen,0);
		if(checksum==pMasterHeader->device[device_num].Image[i].newImageCheckSum){
			image_status[i] = IMAGE_BSPATCH_READY;
			offset += pMasterHeader->device[device_num].Image[i].newImageLen;
			uart_printf("%s, %d IMAGE_BSPATCH_READY\n\r",__func__,i);
		}else{
			checksum=CalcImageChecksum((UINT32*)(NewFileFlashOffset+QSPI_FLASH_BASE),pMasterHeader->device[device_num].Image[i].newImageLen,0);
			if(checksum==pMasterHeader->device[device_num].Image[i].newImageCheckSum){
				OTA_Update_FotaImage(NewFileFlashOffset+QSPI_FLASH_BASE,pMasterHeader->device[device_num].Image[i].newImageLen,pMasterHeader->device[device_num].oldDeviceFlashAddress+offset);
				image_status[i] = IMAGE_BSPATCH_READY;
				offset += pMasterHeader->device[device_num].Image[i].newImageLen;
				uart_printf("%s, %d IMAGE_BSPATCH_READY\n\r",__func__,i);

			}else{
				offset += pMasterHeader->device[device_num].Image[i].newImageLen;
			}			
		}
	}


	for(i=0;i<pMasterHeader->device[device_num].nOfImages;i++)
	{
		if(image_status[i] == IMAGE_BSPATCH_READY)
			continue;

		ImageAddr = (UINT32)((char *)pMasterHeader - pMasterHeader->device[device_num].Image[i].ImageOffset);

		if(pMasterHeader->device[device_num].Image[i].BaseFlashAddress==0){
			old_id = 0;
		}else{
			old_id = (pMasterHeader->device[device_num].Image[i].BaseFlashAddress-pMasterHeader->device[device_num].oldDeviceFlashAddress)/CRANE_NOR_BLOCKSIZE + 1;
		}
		uart_printf("++++++++++++++++++++++++handle_device_%d_file_%d_%d++++++++++++++++++++++++++\n\r",device_num,i+1,old_id);
		
		checksum = CalcImageChecksum((UINT32*)ImageAddr, pMasterHeader->device[device_num].Image[i].ImageLen,0);
		if (checksum != pMasterHeader->device[device_num].Image[i].ImageCheckSum){

			Retval = CRCFailedError;
			uart_printf("error!!!ImageAddr[%08x],ImageLen[%08x],checksum[%08x] ImageCheckSum[%08x]\n\r",ImageAddr,pMasterHeader->device[device_num].Image[i].ImageLen,checksum,pMasterHeader->device[device_num].Image[i].ImageCheckSum);
			//fota_dump_buffer(NULL,ImageAddr,0,pMasterHeader->device[device_num].Image[i].ImageLen);
			goto error;
		}
		
		Retval=Mini_System_Bspatch(pMasterHeader,i,NewFileFlashOffset,device_num,image_status);
		if(Retval != NoError){
			goto error;
		}


	
		Retval = (UINT32)ImageDFotaDecode(
				pMasterHeader->device[device_num].Image[i].BaseFlashAddress,      //oldfile in flash
				0,                                      //oldfile len
				(char *)ImageAddr,  							//patch addr in memory
				pMasterHeader->device[device_num].Image[i].ImageLen,                       //patch len
				NewFileFlashOffset,                      //new 
				&NewFileLen,
				&NewFileAddr);

		if(Retval != NoError || NewFileLen == 0) {
			Retval = DFota_ImageDFotaDecodeFailed;
			goto error;
		}

		uart_printf("%s:Flash_Start_Address: 0x%08X, NewFileLen: 0x%08X, length=0x%08X\n\r", 
			__FUNCTION__,
			pMasterHeader->device[device_num].oldDeviceFlashAddress + i*CRANE_NOR_BLOCKSIZE,
			NewFileLen,
			pMasterHeader->device[device_num].Image[i].ImageLen);
		
		Retval=OTA_Update_FotaImage( NewFileAddr, NewFileLen, pMasterHeader->device[device_num].oldDeviceFlashAddress + i*CRANE_NOR_BLOCKSIZE);
		if (Retval != NoError )
			goto error;
		
		if(NewFileAddr){ 
			free((void *)NewFileAddr);
			NewFileAddr = 0;
		}
		image_status[i] = IMAGE_BSPATCH_READY;
		uart_printf("-------------------------------end---------------------------------\n\r");	


	}

	return NoError;
	
	
error:
	//free buffers
	uart_printf("DFOTA_Upgrade Error!!! Retval=0x%x\n\r", Retval);
	if(NewFileAddr)free((void *)NewFileAddr);

	return Retval;

}


UINT32 Mini_System_Move_Device(asr_master_head_info *pMasterHeader,unsigned long NewFileFlashOffset)
{
	UINT32 Retval = NoError;
	int i = 0, j = 0;
	UINT32 checksum = 0;
	UINT32 offset = 0;
	if(pMasterHeader->device[1].newDeviceFlashAddress < pMasterHeader->device[1].oldDeviceFlashAddress)
	{
		for(i=0;i<pMasterHeader->device[1].nOfImages;i++){
			uart_printf("%s offset[%08x] newImageLen[%08x]\n\r",
				__func__,
				offset,
				pMasterHeader->device[1].Image[i].newImageLen);
		
			checksum=CalcImageChecksum((UINT32*)(pMasterHeader->device[1].newDeviceFlashAddress+offset+QSPI_FLASH_BASE),pMasterHeader->device[1].Image[i].newImageLen,0);
			if(checksum==pMasterHeader->device[1].Image[i].newImageCheckSum){
				uart_printf("%s, %d already be moved\n\r",__func__,i);
				offset += pMasterHeader->device[1].Image[i].newImageLen;
			}else{
				checksum=CalcImageChecksum((UINT32*)(NewFileFlashOffset+QSPI_FLASH_BASE),pMasterHeader->device[1].Image[i].newImageLen,0);
				if(checksum==pMasterHeader->device[1].Image[i].newImageCheckSum){
					OTA_Update_FotaImage(NewFileFlashOffset+QSPI_FLASH_BASE,pMasterHeader->device[1].Image[i].newImageLen,pMasterHeader->device[1].newDeviceFlashAddress+offset);

					uart_printf("%s, %d already be moved to temp\n\r",__func__,i);
					j=i+1;
					offset += pMasterHeader->device[1].Image[i].newImageLen;
					break;
		
				}else{
					j=i;
					uart_printf("%s, %d not be moved\n\r",__func__,i);
					break;
				}
				
			}
		}
		
		for(i=j;i<pMasterHeader->device[1].nOfImages;i++){
			uart_printf("%s offset[%08x] newImageLen[%08x]\n\r",
				__func__,
				offset,
				pMasterHeader->device[1].Image[i].newImageLen);

			OTA_Update_FotaImage((pMasterHeader->device[1].oldDeviceFlashAddress+offset+QSPI_FLASH_BASE),pMasterHeader->device[1].Image[i].newImageLen,NewFileFlashOffset);

			OTA_Update_FotaImage(NewFileFlashOffset+QSPI_FLASH_BASE,pMasterHeader->device[1].Image[i].newImageLen,pMasterHeader->device[1].newDeviceFlashAddress+offset);

			offset += pMasterHeader->device[1].Image[i].newImageLen;
		}

	}
	else{
		offset=pMasterHeader->device[1].newDeviceLen;
		
		for(i=pMasterHeader->device[1].nOfImages-1;i>=0;i--){
			
			offset -= pMasterHeader->device[1].Image[i].newImageLen;
			
			uart_printf("%s offset[%08x] newImageLen[%08x]\n\r",
				__func__,
				offset,
				pMasterHeader->device[1].Image[i].newImageLen);
			
			checksum=CalcImageChecksum((UINT32*)(pMasterHeader->device[1].newDeviceFlashAddress+offset+QSPI_FLASH_BASE),pMasterHeader->device[1].Image[i].newImageLen,0);
			if(checksum==pMasterHeader->device[1].Image[i].newImageCheckSum){
				uart_printf("%s, %d already be moved\n\r",__func__,i);
			}else{
				checksum=CalcImageChecksum((UINT32*)(NewFileFlashOffset+QSPI_FLASH_BASE),pMasterHeader->device[1].Image[i].newImageLen,0);
				if(checksum==pMasterHeader->device[1].Image[i].newImageCheckSum){
					OTA_Update_FotaImage(NewFileFlashOffset+QSPI_FLASH_BASE,pMasterHeader->device[1].Image[i].newImageLen,pMasterHeader->device[1].newDeviceFlashAddress+offset);
					uart_printf("%s, %d already be moved to temp\n\r",__func__,i);
					j=i-1;
					offset -= pMasterHeader->device[1].Image[j].newImageLen;
					break;
		
				}else{
					j=i;
					uart_printf("%s, %d not be moved\n\r",__func__,i);
					break;
				}
				
			}
		}
		
		for(i=j;i>=0;i--){
			uart_printf("%s offset[%08x] newImageLen[%08x]\n\r",
				__func__,
				offset,
				pMasterHeader->device[1].Image[i].newImageLen);

			OTA_Update_FotaImage((pMasterHeader->device[1].oldDeviceFlashAddress+offset+QSPI_FLASH_BASE),pMasterHeader->device[1].Image[i].newImageLen,NewFileFlashOffset);

			OTA_Update_FotaImage(NewFileFlashOffset+QSPI_FLASH_BASE,pMasterHeader->device[1].Image[i].newImageLen,pMasterHeader->device[1].newDeviceFlashAddress+offset);
			if(i>0){
				offset -= pMasterHeader->device[1].Image[i-1].newImageLen;
			}

		}

	}

	return Retval;
}


UINT32 Mini_System_Backup_Device(asr_master_head_info *pMasterHeader,struct fota_firmwar_flag * pFOTA_T)
{
	UINT32 Retval = GeneralError;
	UINT32 nonFotaSize,reservedSize;
	UINT32 nonFotaAddress,reservedAddress;

	nonFotaAddress = pMasterHeader->device[0].newDeviceFlashAddress + pMasterHeader->device[0].newDeviceLen;
	if(nonFotaAddress&(FLASH_4K_SIZE-1)){
		nonFotaAddress = (nonFotaAddress/FLASH_4K_SIZE + 1)*FLASH_4K_SIZE;
	}

	if(pMasterHeader->device[1].newDeviceFlashAddress > pMasterHeader->device[1].oldDeviceFlashAddress){
		nonFotaSize = pMasterHeader->device[1].oldDeviceFlashAddress - nonFotaAddress;
	}else{
		nonFotaSize = pMasterHeader->device[1].newDeviceFlashAddress - nonFotaAddress;
	}

	if((pMasterHeader->device[1].oldDeviceFlashAddress+pMasterHeader->device[1].newDeviceLen) 
		> (pMasterHeader->device[1].newDeviceFlashAddress+pMasterHeader->device[1].newDeviceLen))
	{
		reservedAddress = pMasterHeader->device[1].oldDeviceFlashAddress + pMasterHeader->device[1].newDeviceLen;
	}else{
		reservedAddress = pMasterHeader->device[1].newDeviceFlashAddress + pMasterHeader->device[1].newDeviceLen;
	}
		
	if(reservedAddress&(FLASH_4K_SIZE-1)){
		reservedAddress = (reservedAddress/FLASH_4K_SIZE + 1)*FLASH_4K_SIZE;
	}

	if(pFOTA_T->second_flash_address && pFOTA_T->second_flash_address){
		reservedSize = (pFOTA_T->second_flash_address + pFOTA_T->second_file_size - 2*FLASH_4K_SIZE) 
				-  reservedAddress;
	}else{
		reservedSize = (pFOTA_T->fbf_flash_address + pFOTA_T->fbf_file_size - 2*FLASH_4K_SIZE) 
						-  reservedAddress;

	}
	uart_printf("nonFotaAddress[%08x] nonFotaSize[%08x] reservedAddress[%08x] reservedSize[%08x]\n\r", 
		nonFotaAddress,nonFotaSize,reservedAddress,reservedSize);

	if(nonFotaSize > pMasterHeader->device[1].newDeviceLen){
		Retval=OTA_Update_FotaImage( pMasterHeader->device[1].oldDeviceFlashAddress+QSPI_FLASH_BASE, 
			pMasterHeader->device[1].newDeviceLen, 
			nonFotaAddress);
		if (Retval != NoError )
			goto error;	

	}else if(reservedSize > pMasterHeader->device[1].newDeviceLen){
		Retval=OTA_Update_FotaImage( pMasterHeader->device[1].oldDeviceFlashAddress+QSPI_FLASH_BASE, 
			pMasterHeader->device[1].newDeviceLen, 
			reservedAddress);
		if (Retval != NoError )
			goto error;	

	}else if((nonFotaSize+reservedSize) > pMasterHeader->device[1].newDeviceLen){
		Retval=OTA_Update_FotaImage( pMasterHeader->device[1].oldDeviceFlashAddress+QSPI_FLASH_BASE, 
			nonFotaSize, 
			nonFotaAddress);
		if (Retval != NoError )
			goto error;
		
		Retval=OTA_Update_FotaImage( pMasterHeader->device[1].oldDeviceFlashAddress+nonFotaSize+QSPI_FLASH_BASE, 
				pMasterHeader->device[1].newDeviceLen-nonFotaSize, 
				reservedAddress);
		if (Retval != NoError )
			goto error;

	}
		
error:
	return Retval;

}

UINT32 Mini_System_Copy_Device(asr_master_head_info *pMasterHeader,struct fota_firmwar_flag * pFOTA_T)
{
	UINT32 Retval = GeneralError;
	UINT32 nonFotaSize,reservedSize;
	UINT32 nonFotaAddress,reservedAddress;

	nonFotaAddress = pMasterHeader->device[0].newDeviceFlashAddress + pMasterHeader->device[0].newDeviceLen;
	if(nonFotaAddress&(FLASH_4K_SIZE-1)){
		nonFotaAddress = (nonFotaAddress / FLASH_4K_SIZE + 1) * FLASH_4K_SIZE;
	}

	if(pMasterHeader->device[1].newDeviceFlashAddress > pMasterHeader->device[1].oldDeviceFlashAddress){
		nonFotaSize = pMasterHeader->device[1].oldDeviceFlashAddress - nonFotaAddress;
	}else{
		nonFotaSize = pMasterHeader->device[1].newDeviceFlashAddress - nonFotaAddress;
	}

	if((pMasterHeader->device[1].oldDeviceFlashAddress+pMasterHeader->device[1].newDeviceLen) 
		> (pMasterHeader->device[1].newDeviceFlashAddress+pMasterHeader->device[1].newDeviceLen))
	{
		reservedAddress = pMasterHeader->device[1].oldDeviceFlashAddress + pMasterHeader->device[1].newDeviceLen;
	}else{
		reservedAddress = pMasterHeader->device[1].newDeviceFlashAddress + pMasterHeader->device[1].newDeviceLen;
	}
		
	if(reservedAddress&(FLASH_4K_SIZE-1)){
		reservedAddress = (reservedAddress / FLASH_4K_SIZE + 1) * FLASH_4K_SIZE;
	}


	if(pFOTA_T->second_flash_address && pFOTA_T->second_flash_address){
		reservedSize = (pFOTA_T->second_flash_address + pFOTA_T->second_file_size - 2*FLASH_4K_SIZE) 
				-  reservedAddress;
	}else{
		reservedSize = (pFOTA_T->fbf_flash_address + pFOTA_T->fbf_file_size - 2*FLASH_4K_SIZE) 
						-  reservedAddress;

	}

	uart_printf("nonFotaAddress[%08x] nonFotaSize[%08x] reservedAddress[%08x] reservedSize[%08x]\n\r", 
		nonFotaAddress,nonFotaSize,reservedAddress,reservedSize);

	if(nonFotaSize > pMasterHeader->device[1].newDeviceLen){
		Retval=OTA_Update_FotaImage( nonFotaAddress+QSPI_FLASH_BASE, 
			pMasterHeader->device[1].newDeviceLen, 
			pMasterHeader->device[1].newDeviceFlashAddress);
		if (Retval != NoError )
			goto error;	

	}else if(reservedSize > pMasterHeader->device[1].newDeviceLen){
		Retval=OTA_Update_FotaImage( reservedAddress+QSPI_FLASH_BASE, 
			pMasterHeader->device[1].newDeviceLen, 
			pMasterHeader->device[1].newDeviceFlashAddress);
		if (Retval != NoError )
			goto error;	

	}else if((nonFotaSize+reservedSize) > pMasterHeader->device[1].newDeviceLen){
		Retval=OTA_Update_FotaImage( nonFotaAddress+QSPI_FLASH_BASE, 
			nonFotaSize, 
			pMasterHeader->device[1].newDeviceFlashAddress);
		if (Retval != NoError )
			goto error;
		
		Retval=OTA_Update_FotaImage( reservedAddress+QSPI_FLASH_BASE, 
				pMasterHeader->device[1].newDeviceLen-nonFotaSize, 
				pMasterHeader->device[1].newDeviceFlashAddress + nonFotaSize);
		if (Retval != NoError )
			goto error;

	}
		
error:
	return Retval;

}


UINT32 Mini_System_DFOTA_Upgrade(struct fota_firmwar_flag * pFOTA_T, unsigned long FotaParamStartAddress)
{
	UINT32 Retval = NoError;
	UINT32 LoadAddr = 0;
	asr_master_head_info *pMasterHeader = NULL;
	UINT32 NewFileFlashOffset;


	uart_printf("dfoat first file Size: 0x%08x\n\r", pFOTA_T->fbf_file_size);
	uart_printf("dfoat first file flash address: 0x%08x\n\r", pFOTA_T->fbf_flash_address);
	uart_printf("dfoat second file Size: 0x%08x\n\r", pFOTA_T->second_file_size);
	uart_printf("dfoat second file flash address: 0x%08x\n\r", pFOTA_T->second_flash_address);
#ifdef SUPPORT_FOTA_NVM
	uart_printf("dfoat third file Size: 0x%08x\n\r", pFOTA_T->third_file_size);
	uart_printf("dfoat third file flash address: 0x%08x\n\r", pFOTA_T->third_flash_address);
#endif
	uart_printf("dfota status: 0x%08x\n\r", pFOTA_T->mini_dfota_status);

#ifdef SUPPORT_FOTA_NVM
	LoadAddr = (UINT32)malloc(pFOTA_T->fbf_file_size + pFOTA_T->second_file_size + pFOTA_T->third_file_size);
	if(LoadAddr == 0){
		uart_printf("malloc error\n\r");
		return NULLPointer;
	}
	//uart_printf("LoadAddr: 0x%08x\n\r", LoadAddr);
	memset((void *)LoadAddr,0,pFOTA_T->fbf_file_size + pFOTA_T->second_file_size + pFOTA_T->third_file_size);

#else
	LoadAddr = (UINT32)malloc(pFOTA_T->fbf_file_size+pFOTA_T->second_file_size);
	if(LoadAddr == 0){
		uart_printf("malloc error\n\r");
		return NULLPointer;
	}
	//uart_printf("LoadAddr: 0x%08x\n\r", LoadAddr);
	memset((void *)LoadAddr,0,pFOTA_T->fbf_file_size + pFOTA_T->second_file_size);
#endif
	memcpy((void*)LoadAddr,(void*)(pFOTA_T->fbf_flash_address+QSPI_FLASH_BASE),pFOTA_T->fbf_file_size);

#ifdef SUPPORT_FOTA_NVM
	if(pFOTA_T->third_flash_address && pFOTA_T->third_file_size){
		memcpy((void*)(LoadAddr + pFOTA_T->fbf_file_size),(void *)(pFOTA_T->second_flash_address+QSPI_FLASH_BASE),pFOTA_T->second_file_size);
		memcpy((void*)(LoadAddr + pFOTA_T->fbf_file_size + pFOTA_T->second_file_size),(void *)(pFOTA_T->third_flash_address+QSPI_FLASH_BASE),pFOTA_T->third_file_size);
	}else
#endif
	if(pFOTA_T->second_file_size && pFOTA_T->second_flash_address){
		memcpy((void*)(LoadAddr + pFOTA_T->fbf_file_size),(void *)(pFOTA_T->second_flash_address+QSPI_FLASH_BASE),pFOTA_T->second_file_size);
	}

#ifdef SUPPORT_FOTA_NVM
	if(pFOTA_T->third_flash_address && pFOTA_T->third_file_size){
		NewFileFlashOffset = pFOTA_T->third_flash_address + pFOTA_T->third_file_size;

	}else
#endif
	if(pFOTA_T->second_file_size && pFOTA_T->second_flash_address){
		NewFileFlashOffset = pFOTA_T->second_flash_address + pFOTA_T->second_file_size;
	}else{
		NewFileFlashOffset = pFOTA_T->fbf_flash_address + pFOTA_T->fbf_file_size;
	}
	
	//NewFileFlashOffset = (NewFileFlashOffset + BlockSize) & (~(BlockSize - 1)); //Align with 64KB(Block size)

	uart_printf("NewFileFlashOffset[0x%08x]\n\r",NewFileFlashOffset);
#ifdef SUPPORT_FOTA_NVM
	pMasterHeader = (asr_master_head_info *)(LoadAddr+(pFOTA_T->fbf_file_size+pFOTA_T->second_file_size+pFOTA_T->third_file_size) - sizeof(asr_master_head_info));
#else
	pMasterHeader = (asr_master_head_info *)(LoadAddr+(pFOTA_T->fbf_file_size+pFOTA_T->second_file_size) - sizeof(asr_master_head_info));
#endif

	if (pMasterHeader->magic != FOTA_MAGIC){
		Retval = DFota_MINI_MagicError;
		goto error;
	}

	
	if(pFOTA_T->mini_dfota_status == MINI_SYS_DFOTA_START){
		
		Retval=Mini_Sys_Check_Dfota_File((unsigned char*)LoadAddr,pMasterHeader);
		if (Retval != NoError){
			Retval = DFota_CRCFailedError;
			goto error;
		}

		
		Retval=check_old_image_checksum(pMasterHeader->device[0].oldDeviceFlashAddress+QSPI_FLASH_BASE, pMasterHeader->device[0].oldDeviceLen, pMasterHeader->device[0].oldDeviceCheckSum);
		if (Retval != NoError){
			Retval = DFota_CRCFailedError;
			goto error;
		}

		Retval=check_old_image_checksum(pMasterHeader->device[1].oldDeviceFlashAddress+QSPI_FLASH_BASE, pMasterHeader->device[1].oldDeviceLen, pMasterHeader->device[1].oldDeviceCheckSum);
		if (Retval != NoError){
			Retval = DFota_CRCFailedError;
			goto error;
		}

		Retval=asr_norflash_erase(NewFileFlashOffset+QSPI_FLASH_BASE, CRANE_NOR_BLOCKSIZE);
		if (Retval != NoError)
			goto error;

		
		pFOTA_T->mini_dfota_status ++;
		pFOTA_T->checksum = CalcImageChecksum((UINT32*)pFOTA_T,sizeof(struct fota_firmwar_flag)-sizeof(unsigned int),0);
		Retval=OTA_Update_FotaParam((UINT32)pFOTA_T,sizeof(struct fota_firmwar_flag),FotaParamStartAddress);
		Retval=OTA_Update_FotaParam((UINT32)pFOTA_T,sizeof(struct fota_firmwar_flag),FotaParamStartAddress + FLASH_4K_SIZE);


		
	}
	while(pFOTA_T->mini_dfota_status<MINI_SYS_DFOTA_SECOND_DONE){
		
		switch(pFOTA_T->mini_dfota_status){
			case MINI_SYS_DFOTA_FIRST_DOING:
				Retval=Mini_System_Handle_Device(pMasterHeader,NewFileFlashOffset,0);
				if(Retval!=NoError){
					goto error;
				}
				pFOTA_T->mini_dfota_status ++;
				pFOTA_T->checksum = CalcImageChecksum((UINT32*)pFOTA_T,sizeof(struct fota_firmwar_flag)-sizeof(unsigned int),0);
				Retval=OTA_Update_FotaParam((UINT32)pFOTA_T,sizeof(struct fota_firmwar_flag),FotaParamStartAddress);
				Retval=OTA_Update_FotaParam((UINT32)pFOTA_T,sizeof(struct fota_firmwar_flag),FotaParamStartAddress + FLASH_4K_SIZE);
				
				break;
			case MINI_SYS_DFOTA_SECOND_DOING:
				Retval=Mini_System_Handle_Device(pMasterHeader,NewFileFlashOffset,1);
				if(Retval!=NoError){
					goto error;
				}
				pFOTA_T->mini_dfota_status ++;
				pFOTA_T->checksum = CalcImageChecksum((UINT32*)pFOTA_T,sizeof(struct fota_firmwar_flag)-sizeof(unsigned int),0);
				Retval=OTA_Update_FotaParam((UINT32)pFOTA_T,sizeof(struct fota_firmwar_flag),FotaParamStartAddress);
				Retval=OTA_Update_FotaParam((UINT32)pFOTA_T,sizeof(struct fota_firmwar_flag),FotaParamStartAddress + FLASH_4K_SIZE);
					
				break;
			case MINI_SYS_DFOTA_SECOND_BACKUP:
				uart_printf("oldDeviceFlashAddress[%08x] newDeviceFlashAddress[%08x]\r\n",
					pMasterHeader->device[1].oldDeviceFlashAddress,
					pMasterHeader->device[1].newDeviceFlashAddress);
				
				if(pMasterHeader->device[1].oldDeviceFlashAddress != pMasterHeader->device[1].newDeviceFlashAddress){
					uart_printf("MINI_SYS_DFOTA_SECOND_BACKUP\r\n");
					Retval=Mini_System_Move_Device(pMasterHeader,NewFileFlashOffset);
					if(Retval!=NoError){
						goto error;
					}
					pFOTA_T->mini_dfota_status = MINI_SYS_DFOTA_SECOND_DONE;
					pFOTA_T->checksum = CalcImageChecksum((UINT32*)pFOTA_T,sizeof(struct fota_firmwar_flag)-sizeof(unsigned int),0);
					Retval=OTA_Update_FotaParam((UINT32)pFOTA_T,sizeof(struct fota_firmwar_flag),FotaParamStartAddress);
					Retval=OTA_Update_FotaParam((UINT32)pFOTA_T,sizeof(struct fota_firmwar_flag),FotaParamStartAddress + FLASH_4K_SIZE);


					/*
					Retval=Mini_System_Backup_Device(pMasterHeader,pFOTA_T);
					if(Retval!=NoError){
						goto error;
					}
	
					pFOTA_T->mini_dfota_status ++;
					pFOTA_T->checksum = CalcImageChecksum((UINT32*)pFOTA_T,sizeof(struct fota_firmwar_flag)-sizeof(unsigned int),0);
					Retval=OTA_Update_FotaParam((UINT32)pFOTA_T,sizeof(struct fota_firmwar_flag),FotaParamStartAddress);
					Retval=OTA_Update_FotaParam((UINT32)pFOTA_T,sizeof(struct fota_firmwar_flag),FotaParamStartAddress + FLASH_4K_SIZE);
					*/
				}else{
					uart_printf("no need backup\r\n");
					pFOTA_T->mini_dfota_status = MINI_SYS_DFOTA_SECOND_DONE;
					pFOTA_T->checksum = CalcImageChecksum((UINT32*)pFOTA_T,sizeof(struct fota_firmwar_flag)-sizeof(unsigned int),0);
					Retval=OTA_Update_FotaParam((UINT32)pFOTA_T,sizeof(struct fota_firmwar_flag),FotaParamStartAddress);
					Retval=OTA_Update_FotaParam((UINT32)pFOTA_T,sizeof(struct fota_firmwar_flag),FotaParamStartAddress + FLASH_4K_SIZE);
				}
				break;
			case MINI_SYS_DFOTA_SECOND_COPYING:
				uart_printf("MINI_SYS_DFOTA_SECOND_COPYING\r\n");
				Retval=Mini_System_Copy_Device(pMasterHeader,pFOTA_T);
				if(Retval!=NoError){
					goto error;
				}
				pFOTA_T->mini_dfota_status ++;
				pFOTA_T->checksum = CalcImageChecksum((UINT32*)pFOTA_T,sizeof(struct fota_firmwar_flag)-sizeof(unsigned int),0);
				Retval=OTA_Update_FotaParam((UINT32)pFOTA_T,sizeof(struct fota_firmwar_flag),FotaParamStartAddress);
				Retval=OTA_Update_FotaParam((UINT32)pFOTA_T,sizeof(struct fota_firmwar_flag),FotaParamStartAddress + FLASH_4K_SIZE);
				
				break;
		}

	}
	
#ifdef SUPPORT_FOTA_NVM
	if(pFOTA_T->third_flash_address && pFOTA_T->third_file_size){
		_ptentry *nvm_Entry=ptable_find_entry("nvm");
		uart_printf("erase nvm start[%08x] size[%08x]\n\r", nvm_Entry->start,nvm_Entry->size);
		
		Retval=asr_norflash_erase(nvm_Entry->start+QSPI_FLASH_BASE, nvm_Entry->size);
		if (Retval != NoError)
			goto error;
	}
#endif

	free((void *)LoadAddr);
	LoadAddr=0;
	


	return NoError;


error:
	//free buffers
	uart_printf("DFOTA_Upgrade Error!!! Retval=%x\n\r", Retval);
	if(LoadAddr) free((void*)LoadAddr);

	return Retval;

}


int datamodule_fota()
{
    struct fota_firmwar_flag fotaPara_1;
	struct fota_firmwar_flag fotaPara_2;
	struct fota_firmwar_flag *fotaPara;

	struct fota_param param={0};

    int Retval = NoError;

	UINT32 stack=0x7e900000;
	//The bspatch/lzma lib needs malloc/free APIs
	UINT32 poolLen = 0x00700000;
	malloc_init(stack, poolLen);

	unsigned int FotaParamStartAddress_v;
	FotaParamStartAddress_v=get_fota_param_start_addr() + FLASH_4K_SIZE;


    uart_printf("FotaParamStartAddress[%08x] read len[%d]\r\n",FotaParamStartAddress_v,sizeof(struct fota_firmwar_flag));
   	//memcpy(&param,get_fota_param_start_addr(),sizeof(struct fota_param));
	uart_printf("mini_sys_enable[%08x]\r\n",param.mini_sys_enable);
	
    Retval = asr_norflash_read(FotaParamStartAddress_v,&fotaPara_1,sizeof(struct fota_firmwar_flag));
	if(Retval != NoError){
	    uart_printf("FotaParamStartAddress read failed\r\n");
		return GeneralError;
	}

	uart_printf("Flag header: %08x\r\n", fotaPara_1.header);
	uart_printf("Flag upgrade: %08x\r\n", fotaPara_1.upgrade_flag);
	uart_printf("Flag upgrade_method: %08x\r\n", fotaPara_1.upgrade_method);
	uart_printf("Flag fbf flash address: %08x\r\n", fotaPara_1.fbf_flash_address);
	uart_printf("Flag fbf file size: %d\r\n", fotaPara_1.fbf_file_size);

    Retval = asr_norflash_read(FotaParamStartAddress_v + FLASH_4K_SIZE,&fotaPara_2,sizeof(struct fota_firmwar_flag));
	if(Retval != NoError){
	    uart_printf("FotaParamStartAddress read failed\r\n");
		return GeneralError;
	}

	if(fotaPara_1.header != FOTA_HEADER_MAGIC && fotaPara_2.header != FOTA_HEADER_MAGIC){
		uart_printf("No need to upgrade\r\n");
		return GeneralError;
	}
		
	if(fotaPara_1.checksum!=CalcImageChecksum((UINT32*)&fotaPara_1,sizeof(struct fota_firmwar_flag)-sizeof(unsigned int),0)){
		if(fotaPara_2.checksum!=CalcImageChecksum((UINT32*)&fotaPara_2,sizeof(struct fota_firmwar_flag)-sizeof(unsigned int),0)){
			uart_printf("fota param checksum failed, fotaPara_1[%08x]:[%08x] fotaPara_2[%08x]:[%08x] \n\r",
				fotaPara_1.checksum,CalcImageChecksum((UINT32*)&fotaPara_1,sizeof(struct fota_firmwar_flag)-sizeof(unsigned int),0),
				fotaPara_2.checksum,CalcImageChecksum((UINT32*)&fotaPara_2,sizeof(struct fota_firmwar_flag)-sizeof(unsigned int),0));
			return DFota_CRCFailedError;
		}else{
			if(param.mini_sys_enable==1){
				uart_printf("fotaPara_1 illegal,update fotaPara_1 from fotaPara_2\r\n");
				memcpy(&fotaPara_1,&fotaPara_2,sizeof(struct fota_firmwar_flag));
				fotaPara_1.mini_dfota_status ++;
				fotaPara_1.checksum = CalcImageChecksum((UINT32*)&fotaPara_1,sizeof(struct fota_firmwar_flag)-sizeof(unsigned int),0);
				Retval=OTA_Update_FotaParam((UINT32)&fotaPara_1,sizeof(struct fota_firmwar_flag),FotaParamStartAddress_v);
				fotaPara_2.mini_dfota_status ++;
				fotaPara_2.checksum = CalcImageChecksum((UINT32*)&fotaPara_2,sizeof(struct fota_firmwar_flag)-sizeof(unsigned int),0);
				Retval=OTA_Update_FotaParam((UINT32)&fotaPara_2,sizeof(struct fota_firmwar_flag),FotaParamStartAddress_v + FLASH_4K_SIZE);
				fotaPara = &fotaPara_1;
			}else{
				if(fotaPara_2.DFota_nOfImages==0xFFFFFFFF){
					fotaPara = &fotaPara_2;
					uart_printf("first using fotaPara_2,not copy temp flash\r\n");
				}else{
					fotaPara = &fotaPara_2;
					uart_printf("fotaPara_1 illegal,using fotaPara_2,not copy temp flash\r\n");
				}
			}
		}

	}
	else{
		if(param.mini_sys_enable==1){
			if(fotaPara_2.checksum!=CalcImageChecksum((UINT32*)&fotaPara_2,sizeof(struct fota_firmwar_flag)-sizeof(unsigned int),0)){
				uart_printf("fotaPara_2 illegal,update fotaPara_2 from fotaPara_1\r\n");
				memcpy(&fotaPara_2,&fotaPara_1,sizeof(struct fota_firmwar_flag));
				Retval=OTA_Update_FotaParam((UINT32)&fotaPara_2,sizeof(struct fota_firmwar_flag),FotaParamStartAddress_v + FLASH_4K_SIZE);
				fotaPara = &fotaPara_1;
			}else{
				if(fotaPara_1.mini_dfota_status != fotaPara_2.mini_dfota_status){
					uart_printf("using fotaPara_1!=fotaPara_2,sync fotaPara_2\r\n");
					memcpy(&fotaPara_2,&fotaPara_1,sizeof(struct fota_firmwar_flag));
					Retval=OTA_Update_FotaParam((UINT32)&fotaPara_2,sizeof(struct fota_firmwar_flag),FotaParamStartAddress_v + FLASH_4K_SIZE);
					fotaPara = &fotaPara_1;
				}else{
					uart_printf("using fotaPara_1=fotaPara_2\r\n");
					fotaPara = &fotaPara_1;
				}
				
			}

		}
		else{
			if(fotaPara_1.DFota_nOfImages==0xFFFFFFFF){
				fotaPara = &fotaPara_1;
				uart_printf("first using fotaPara_1,not copy temp flash\r\n");
			}else{
				if(fotaPara_2.checksum!=CalcImageChecksum(&fotaPara_2,sizeof(struct fota_firmwar_flag)-sizeof(unsigned int),0)){
					fotaPara = &fotaPara_1;
					uart_printf("fotaPara_2 illegal,using fotaPara_1,need copy temp flash\r\n");
				}else{
					if(fotaPara_1.index > fotaPara_2.index){
						fotaPara = &fotaPara_1;
						uart_printf("using fotaPara_1,need copy temp flash\r\n");
					}else{
						fotaPara = &fotaPara_2;
						uart_printf("using fotaPara_2,not copy temp flash\r\n");
					}
					
				}
			}
		}

	}


	
    if (fotaPara->header != FOTA_HEADER_MAGIC)
    {
        uart_printf("FBF Flag mismatch,Header[%.08x]\r\n",fotaPara->header);
        return NotFoundError;
    }

    if (fotaPara->upgrade_flag != 1)
    {
        uart_printf("No need to upgrade[%d]\r\n",fotaPara->upgrade_flag);
        return NotFoundError;
    }
	
	if(param.mini_sys_enable==1){
		//uart_printf("mini_dfota_status %08x!!!\r\n",fotaPara->mini_dfota_status);
		switch(fotaPara->mini_dfota_status){
			case MINI_SYS_DFOTA_START:
				uart_printf("*************MINI_SYS_DFOTA_START*********\r\n");
				break;
			case MINI_SYS_DFOTA_FIRST_DOING:
				uart_printf("********MINI_SYS_DFOTA_FIRST_DOING********\r\n");
				break;
			case MINI_SYS_DFOTA_SECOND_DOING:
				uart_printf("*******MINI_SYS_DFOTA_SECOND_DOING********\r\n");
				break;
			case MINI_SYS_DFOTA_SECOND_BACKUP:
				uart_printf("******MINI_SYS_DFOTA_SECOND_BACKUP********\r\n");
				break;
			case MINI_SYS_DFOTA_SECOND_COPYING:
				uart_printf("******MINI_SYS_DFOTA_SECOND_COPYING*******\r\n");
				break;
			case MINI_SYS_DFOTA_SECOND_DONE:
				uart_printf("*******MINI_SYS_DFOTA_SECOND_DONE*********\r\n");
				break;
		}
	
		if (fotaPara->mini_dfota_status >= MINI_SYS_DFOTA_SECOND_DONE)
		{
			uart_printf("mini system Dfota done!!!\r\n");
			return NoError;
		}
	}



    switch (fotaPara->upgrade_method)
    {
        case 1: // full system
            uart_printf("DFOTA full system\r\n");
            Retval = FOTA_Upgrade(fotaPara,FotaParamStartAddress_v);
			if (Retval != NoError){
				uart_printf("upgrade failed!!! Retval=%x\n",Retval);
			}
            break;
        case 4: // DFOTA
            uart_printf("DFOTA upgrade\r\n");
            Retval = DFOTA_Upgrade(fotaPara,FotaParamStartAddress_v);
			if (Retval != NoError){
				uart_printf("upgrade failed!!! Retval=%x\n",Retval);
			}
            break;
		case 5: // MINI SYSTEM DFOTA
            uart_printf("MINI SYSTEM DFOTA upgrade\r\n");
            Retval = Mini_System_DFOTA_Upgrade(fotaPara,FotaParamStartAddress_v);
			if (Retval != NoError){
				uart_printf("upgrade failed!!! Retval=%x\n",Retval);
			}
            break;
        default:
            uart_printf("Unspported upgrade method Upgrade_Method[%.08x]\r\n",fotaPara->upgrade_method);
			Retval = NotSupportedError;
            break;
    }


	if(param.mini_sys_enable != 1){
		// clear fota param
		fotaPara->header = 0;
		fotaPara->upgrade_flag = 0;
		fotaPara->upgrade_method = 0;
		fotaPara->checksum = 0;
		Retval = OTA_Update_FotaParam(fotaPara, sizeof(struct fota_firmwar_flag),FotaParamStartAddress_v);
		if (Retval != NoError)
			return Retval;
	
		Retval = OTA_Update_FotaParam(fotaPara, sizeof(struct fota_firmwar_flag),FotaParamStartAddress_v + FLASH_4K_SIZE);
		if (Retval != NoError)
			return Retval;
		uart_printf("Clear fota param OK\n");
	
		memset(param.fotaFlag,0,ASR_FOTA_FLAG_LEN_MAX);
		param.mini_sys_enable = 0;
	
		OTA_Update_FotaParam(&param, sizeof(struct fota_param),get_fota_param_start_addr());
	
		uart_printf("Clear upgrade flag OK\n"); 
	}

    return Retval;

}

