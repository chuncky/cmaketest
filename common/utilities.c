#include "common.h"
#include "utilities.h"
#include "ptable.h"
#include "asr_nor_flash.h"
#include "loadtable.h"
#include "asr_property.h"
#include "bsp.h"
#include "updater_table.h"
#include "logo_table.h"
#include "LzmaDec.h"
#include "asr_lzma.h"
#include "guilin.h"
#include "ningbo.h"
#include "gpio.h"




static UINT32 m_strlen(const char *string);
static void MrdSwap( MrdAdc_t *p_MrdAdcData, MrdAdc_t *p_MrdAdcData2 );
static void DelayInSecond(unsigned int nSecond);
static BOOL pm803_GetBatteryReadyFlag( void );
static UINT16 pm803_get_share_memory_data( UINT32 addr );
static INT32 pm803_FitBatteryValue( UINT32 *p_FitValue );
static void pm803_ResetBatteryReadyFlag( void );
static void pm803_print_debug_info( void );
static void pm803_reset_dsp_adc_ready_flag( void );

static void pm803_reset_dsp_adc_ready_flag( void )
{
	UINT32 dsp_adc_ready_flag_addr = DSP_ADC_SHARE_MEMORY_ADDR;
	
	*( UINT32 *)dsp_adc_ready_flag_addr = 0xF1F1F1F1;
}


static void DelayInSecond(unsigned int nSecond)
{
	unsigned long startTime, endTime;
    unsigned long temp;
	
	startTime = GetTimer0CNT();
	do{
		endTime = GetTimer0CNT();
		temp = endTime - startTime;
	}while(temp < nSecond);
}

static UINT32 m_strlen(const char *string)
{
   UINT32 count = 0;

   while (*string != '\0')
   {
      string++;
      count++;
   }

   return count;
}

static void MrdSwap( MrdAdc_t *p_MrdAdcData1, MrdAdc_t *p_MrdAdcData2 )
{
	volatile MrdAdc_t temp;

	temp.AdcValue = p_MrdAdcData2->AdcValue;
//	temp.DataValid = p_MrdAdcData2->DataValid;		// as valid is same, so skip this statement
	temp.VoltageValue = p_MrdAdcData2->VoltageValue;

	p_MrdAdcData2->AdcValue = p_MrdAdcData1->AdcValue;
	p_MrdAdcData2->VoltageValue = p_MrdAdcData1->VoltageValue;

	p_MrdAdcData1->AdcValue = temp.AdcValue;
	p_MrdAdcData1->VoltageValue = temp.VoltageValue;
}

void MrdBubbleSort( MrdAdc_t *p_MrdAdcData,const UINT32 DataNum )
{
	INT32 i,j;

	for( i = 0; i < DataNum - 1; i++ ){
		for( j = 0; j < DataNum - 1 - i; j++ ){
			if( p_MrdAdcData[j].AdcValue > p_MrdAdcData[j+1].AdcValue ){
				MrdSwap(&p_MrdAdcData[j],&p_MrdAdcData[j+1]);
			}
		}
	}
}

void uint2str( UINT32 x, char *Str )
{
    UINT32 t;
    char *Ptr;
    char Buf[16];
    INT32 i = 0;

	if( Str == NULL ){
		CP_LOGE("%s,input parameter error\r\n",__func__);
		while(1);
	}
	
    Ptr = Str;
    if(x < 10){  
        *Ptr++ = x + 0x30;
    }else{
        while(x > 0){
            t = x % 10;
            x = x / 10;
            Buf[i++] = t + 0x30; 
        }
        i--;
        for(;i >= 0;i--){  // Reverse the resulting string
            *(Ptr++) = Buf[i];
        }
    }
	
    *Ptr = '\0';
}


void hex2str( UINT32 x, char *Str )
{
    UINT32 t;
    char *Ptr;
    char Buf[16];
    INT32 i = 0;
	const unsigned char hex_num_str[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
	
	if( Str == NULL ){
		CP_LOGE("%s,input parameter error\r\n",__func__);
		while(1);
	}
	
    Ptr = Str;
    if(x < 16){  
        *Ptr++ = hex_num_str[x];
    }else{
        while(x > 0){
            t = x % 16;
            x = x / 16;
            Buf[i++] = hex_num_str[t]; 
        }
        i--;
        for(;i >= 0;i--){  // Reverse the resulting string
            *(Ptr++) = Buf[i];
        }
    }
	
    *Ptr = '\0';
}





UINT32 m_atoi_dec(const char * string_ptr)
{
   /* NOTICE: these codes only support 16 as it's radix without 0x ahead */
   UINT32   string_size;
   UINT32   i;
   char    tmp;
   UINT32   ret;

   string_size = m_strlen(string_ptr);
   //if (!string_size) return UINT32MAX;

   ret = 0;
   for (i = 0; i < string_size; i++)
   {
      tmp = string_ptr[i];
      if ((tmp >= '0') && (tmp <= '9'))
      {
         ret = (ret * 10) + (tmp - '0');
         continue;
      }

      if (tmp == 0)
      {
         continue;
      }

      /* invalid character */
      return 0xFFFFFFFF;
   }

   return ret;
}

UINT32 m_hex_string2i_dec(const char * string_ptr)
{
   /* NOTICE: these codes only support 16 as it's radix without 0x ahead */
   UINT32   string_size;
   UINT32   i;
   char    tmp;
   UINT32   ret;

   string_size = m_strlen(string_ptr);
   //if (!string_size) return UINT32MAX;

   ret = 0;
   for (i = 0; i < string_size; i++)
   {
      tmp = string_ptr[i];
      if( (tmp >= '0') && (tmp <= '9') )
      {
         ret = (ret * 16) + (tmp - '0');
         continue;
      }
      if ((tmp >= 'a') && (tmp <= 'f'))
      {
         ret = (ret * 16) + (tmp - 'a') + 0xA;
         continue;
      }

      if (tmp == 0)
      {
         continue;
      }

      /* invalid character */
      return 0xFFFFFFFF;
   }

   return ret;
}
Dsp_adc_LoadImage_Res_t load_dsp_adc( Dsp_adc_type_t dsp_type )
{
	UINT32 dsp_adc_copy_src_addr;
	UINT32 dsp_adc_copy_dst_addr;
	UINT32 dsp_adc_copy_size;
	_ptentry *dsp_adc_Entry;
	CompressedType_e image_compress_type;
	UINT32 compress_magic;
	
	INT32 ret = -1;
	Dsp_adc_LoadImage_Res_t load_res;

	if( dsp_type <= dsp_adc_type_min_error ||  dsp_type >= dsp_adc_type_max_error ){
		CP_LOGE("func:%s,line:%d,parameter error, dsp_type:%d\r\n",__func__,__LINE__,dsp_type);
		return Dsp_adc_para_error;
	}

	/* [1] init */
	if( dsp_type == dsp_adc_type3_in_updater ){
		dsp_adc_copy_src_addr = updater_get_ext_dsp_adc_flash_start_addr();
		dsp_adc_copy_dst_addr = updater_get_ext_dsp_exec_addr();		
		dsp_adc_copy_size	  = updater_get_ext_dsp_adc_size();
	}else if( dsp_type == dsp_adc_type4_in_logo	 ){
		dsp_adc_copy_src_addr = logo_get_ext_dsp_adc_flash_start_addr();
		dsp_adc_copy_dst_addr = logo_get_ext_dsp_adc_exec_addr();		
		dsp_adc_copy_size	  = logo_get_ext_dsp_adc_size();
	}else{
		dsp_adc_Entry = ( dsp_type == dsp_adc_type1 ? ptable_find_entry("dsp_adc_b") : ptable_find_entry("dsp_adc") );
		if( dsp_adc_Entry == NULL ){
			CP_LOGE("[DSP_ADC] dsp_adc.bin not exist in ptable\r\n");
			return Dsp_adc_ImageNotExist;
		}
		dsp_adc_copy_src_addr = dsp_adc_Entry->vstart;
		dsp_adc_copy_dst_addr = DSP_ADC_EXECUTE_ADDR;
		dsp_adc_copy_size	  = dsp_adc_Entry->vsize;
	}

		// dump the load dsp_adc info
		CP_LOGD("[DSP_ADC] copy from [0x%0.8x] to [0x%0.8x] size=[0x%0.8x]\r\n",
				dsp_adc_copy_src_addr,
				dsp_adc_copy_dst_addr,
				dsp_adc_copy_size);


	/* [2] copy dsp_adc.bin from flash into psram  */	 
	image_compress_type = GetCompressedType( *(UINT32 *)dsp_adc_copy_src_addr );
	CP_LOGI("dsp_adc compress_type:%d\r\n",image_compress_type);
	if( image_compress_type == NO_COMPRESSED ){
    	ret = asr_norflash_read( (void *)dsp_adc_copy_src_addr, (void *)dsp_adc_copy_dst_addr,dsp_adc_copy_size);
	}else if(image_compress_type == LZMA){
#if (defined SMALL_CODE_LOGO)
//		optimize - uncompress time: 20ms
//      not optimize -uncompress time: 30ms
// in order to boot more quick, logo use optimize.
		if( IsLzmaOptimize() == FALSE ){
			LzmaOptimizeSwitch(TRUE);
		}
#endif
		UINT32 compress_src_len = dsp_adc_copy_size;
		ret = LzmaUncompress((void *)dsp_adc_copy_dst_addr, &dsp_adc_copy_size, (void *)dsp_adc_copy_src_addr, &compress_src_len);
//      NOTE: only open and close one time.
//		if( IsLzmaOptimize() == TRUE ){
//			LzmaOptimizeSwitch(FALSE);
//		} 
 		CP_LOGD("compress_src_len:0x%x,decompress_out_len:0x%x\r\n",compress_src_len,dsp_adc_copy_size);
	}
	if(ret == 0){
		CP_LOGD("[DSP_ADC] copy dsp_adc success!\r\n");
		load_res = Dsp_adc_LoadOk;
	}else{
		CP_LOGE("[DSP_ADC] copy dsp_adc fail,ret:%d\r\n",ret);
		load_res = Dsp_adc_LoadFail;
	}

	// check load data is right ?
#ifdef DSP_ADC_DEBUG
	if( image_compress_type == NO_COMPRESSED ){
		ret = memcmp((void *)dsp_adc_copy_src_addr, (void *)dsp_adc_copy_dst_addr,dsp_adc_copy_size);
		if( ret != 0 ){
			CP_LOGE("load error\r\n");
			while(1);
		}
	}
#endif

    return load_res;
}


void boot_dsp_adc_external_from_squ_new(void)
{
    UINT32 code_addr, squ_addr,copy_len,i;
    UINT32 ptcm_addr = PTCM_ADDR;
    volatile UINT32 delay_counter;

	
    squ_addr = DSP_ADC_EXECUTE_ADDR;
    copy_len = DSP_ADC_COPY_LEN;

	/* [1] config dsp_adc.bin run region. ( internal or external, addr ) */
    DSP_ADC_SYS_CTRL |= 0x1; 							//boot from external
    DSP_ADC_VECTOR = DSP_ADC_EXTERNAL_BOOT_ADDR;   		//external boot addr
	
	/* [2] reset dsp_adc hardware */
    PMUM_CPRR |= (BIT_2 | BIT_3);
    PMUM_CPRR |= BIT_5;								// execute is hold
    PMUM_CPRR &= ~(BIT_2 | BIT_3);
    //force delay for DSP_ADC EDAP get ready
    for(delay_counter=0; delay_counter<100*100;delay_counter++){};
	
	/* [3] Copy X1 code to PTCM */
    CP_LOGD("Copy X1 code to PTCM\r\n");
	for(i = 0; i < copy_len/16; i++){
		BU_REG_WRITE(ptcm_addr+16*i+0,	BU_REG_READ(squ_addr+16*i+8));
		BU_REG_WRITE(ptcm_addr+16*i+4,	BU_REG_READ(squ_addr+16*i+12));
		BU_REG_WRITE(ptcm_addr+16*i+8,	BU_REG_READ(squ_addr+16*i+0));
		BU_REG_WRITE(ptcm_addr+16*i+12, BU_REG_READ(squ_addr+16*i+4));
	}

	/* [4] dsp_adc is running */
	PMUM_CPRR &= ~BIT_5;
	
	CP_LOGD("Release X1\r\n");
}

/**
 * @Description:
 * 	[1] memset PTCM ?
 *	[2] reset dsp_adc_hardware
 * @param: NONE
 * @retval: NONE
 */
void hold_dsp_adc( void )
{
    PMUM_CPRR |= BIT_5;								// execute is hold
}


/*--------------------------------------------------*/
INT32 MRDAdcFlagGet_boot(PM803_ADC_TYPE type, ADC_TEMP_t tempType)
{
	UINT32 mrd_flash_addr;
	UINT32 addr;
	UINT32 mrd_extra_offset=0x10000-0x1000;
	UINT8 *p_buf;
	UINT32 flag=0,flag_off/*=4096-4-100-4-4*/;
	INT32 ret = -1;
	PM803_ADC_TYPE adc_type=type;

	if((adc_type > ADC_RTP)||(tempType>=TEMP_TYPE_MAX))
		return -1;

	p_buf = (UINT8 *)malloc(4096);
	if(p_buf == NULL){
		CP_LOGE("func:%s,malloc fail\r\n");
		while(1);
	}

	mrd_flash_addr=get_factory_a_start_address();

	addr = mrd_flash_addr + mrd_extra_offset;
	ret = asr_norflash_read(addr, p_buf, 4096);
	if(ret != 0){
		CP_LOGE("read mrd flag fail,ret:%d\r\n",ret);
		free( p_buf );
		p_buf = NULL;
		while(1);
	}

	if(adc_type == ADC_RTP)
		flag_off=4096-4-104-216-MAX_SECURE_SIZE-4-MAX_DEVID_SIZE-4-4;
	else
	{
		if(tempType==TEMP_NORMAL)
			flag_off=4096-4-100-4-4;
		else
			flag_off=4+(TEMP_TYPE_MAX-1)*4+ITEM_ADC_SIZE-4;
	}

	flag = *(UINT32 *)(p_buf+flag_off);
	free( p_buf );
	p_buf = NULL;
	
	CP_LOGD("%s, flag: %d\r\n",__FUNCTION__,flag);

	if( flag != 1 ){
		return 0;
	}
	
	return flag;
}

INT32 MRDAdcValueGet_boot(UINT32 index, UINT32 *AdcValue, UINT32 *VoltageValue, PM803_ADC_TYPE type, ADC_TEMP_t tempType)
{
	UINT8 data1[MAX_ADC_Data_LEN+1]={0};
	UINT8 data2[MAX_ADC_Data_LEN+1]={0};
	INT32 i=0;
	UINT32 mrd_flash_addr;
	UINT32 addr;
	UINT32 mrd_extra_offset=0x10000-0x1000;
	UINT8 *p_buf;
	UINT32 value=0,value_off=0;
	INT32 ret = -1;
	PM803_ADC_TYPE adc_type=type;

	p_buf = (UINT8 *)malloc(4096);
	if( p_buf == NULL ){
		CP_LOGE("func:%s,malloc fail\r\n");
		while(1);
	}

	if((index>=MAX_ADC_GROUP_CNT)||(AdcValue==NULL)||(VoltageValue==NULL) || (adc_type > ADC_RTP)||(tempType>=TEMP_TYPE_MAX))		
	{
		CP_LOGE("%s, invalid para\r\n",__FUNCTION__);
		free( p_buf );
		p_buf = NULL;
		return -1;
	}
	
	mrd_flash_addr=get_factory_a_start_address();
	addr = mrd_flash_addr + mrd_extra_offset;
	ret = asr_norflash_read(addr, p_buf, 4096);
	if(ret != 0){
		CP_LOGE("read mrd flag fail,ret:%d\r\n",ret);
		free( p_buf );
		p_buf = NULL;
		while(1);
	}	

	if(adc_type == ADC_RTP)
		value_off=4096-4-104-216-MAX_SECURE_SIZE-4-MAX_DEVID_SIZE-4-212+8+index*MAX_ADC_Data_LEN*2;
	else
	{
		if(tempType==TEMP_NORMAL)
			value_off=4096-4-104-212+8+index*MAX_ADC_Data_LEN*2;
		else
			value_off=4+(TEMP_TYPE_MAX-1)*4+8+((tempType-1)*MAX_ADC_GROUP_CNT+index)*MAX_ADC_Data_LEN;
	}
	memcpy(data1, p_buf+value_off, MAX_ADC_Data_LEN);
	memcpy(data2, p_buf+value_off+MAX_ADC_Data_LEN, MAX_ADC_Data_LEN);
	data1[strlen(data1)]='\0';
	data2[strlen(data2)]='\0';

	for(i=0;i<MAX_ADC_Data_LEN;i++){
		if(((data1[i]!=0)&&(data2[i]!=0))&&((data1[i]!=0xFF)&&(data2[i]!=0xFF))){
			break;
		}
	}
	if(i == MAX_ADC_Data_LEN)
	{
#ifdef MRD_DEBUG_CODE		
		CP_LOGD("[DSP_ADC] %s, null value\r\n",__func__);
#endif
		free( p_buf );
		p_buf = NULL;
		return 1;		//NULL value
	}
	
	*AdcValue = m_atoi_dec(data1);
	*VoltageValue = m_atoi_dec(data2);
	if( ( *AdcValue == 0xFFFFFFFF ) || ( *VoltageValue == 0xFFFFFFFF ) )
	{
		CP_LOGW("%s, invalid value\r\n",__func__);
		free( p_buf );
		p_buf = NULL;
		return -1;		//invalid value
	}else{
		free( p_buf );
		p_buf = NULL;
		return 0;		//ok
	}
}

UINT32 MRDAdcCntGet_boot(PM803_ADC_TYPE type, ADC_TEMP_t tempType)
{
	UINT32 mrd_flash_addr;
	UINT32 addr;
	UINT32 mrd_extra_offset = 0x10000-0x1000;
	UINT8 *p_buf = NULL;
	UINT32 cnt=0,cnt_off1=4096-4-104-212-4, cnt_off2=4, cnt_off3=4096-8-100-4-212-2048-4-212-4-MAX_DEVID_SIZE-4;
	INT32 ret = -1;
	PM803_ADC_TYPE adc_type=type;

	if((adc_type > ADC_RTP)||(tempType>=TEMP_TYPE_MAX))
		return -1;

	p_buf = (UINT8 *)malloc(4096);
	if( p_buf == NULL ){
		p_buf = NULL;		
		CP_LOGE("func:%s,malloc fail\r\n");
		while(1);
	}

	mrd_flash_addr = get_factory_a_start_address();

	addr=mrd_flash_addr+mrd_extra_offset;
	ret = asr_norflash_read( addr, p_buf, 4096 );
	if(ret != 0){
		CP_LOGE("read mrd cnt fail,ret:%d\r\n",ret);
		free( p_buf );
		p_buf = NULL;
		while(1);
	}	

	if(adc_type == ADC_RTP)
		cnt=*(UINT32 *)(p_buf+cnt_off3);
	else
	{
		if(tempType==TEMP_NORMAL)
			cnt=*(UINT32 *)(p_buf+cnt_off1);
		else
			cnt=*((UINT32 *)(p_buf+cnt_off2)+tempType-1);
	}
	
	free( p_buf );
	p_buf = NULL;
	CP_LOGD("%s,adc_type:%d,tempType:%d,cnt:%d\r\n",__FUNCTION__,adc_type,tempType, cnt);

	if( cnt > MAX_ADC_GROUP_CNT ){
		CP_LOGE("cnt:%d is more than max value: %d\r\n",cnt,MAX_ADC_GROUP_CNT);
		return 0xFFFFFFFF;
	}
	return cnt;
}

void transmit_data(const char *name, const UINT32 data )
{
	char string_buffer[32] = { 0 };

	/* [1] data switch into string */
	hex2str( data, string_buffer );

	/* [2] store string into share buffer */
	if( 0 != asr_property_set( name, string_buffer ) ){
		CP_LOGE("%s,store string in share buffer error!\r\n",__func__);
		while(1);
	}
}

UINT32 get_transmit_data( const char *name )
{
	char string_buffer[32] = { 0 };
	UINT32 data;

	/* [1] get string buffer from share memroy  */
	if( 0 == asr_property_get_buffer( name,string_buffer ) ){
		CP_LOGE("%s, no \"%s\" in share buffer!!!\r\n",__func__,name);
		/* [NOTE] return a special value if not get share buffer */
		return 0xFFFFFFFF;
	}
//	CP_LOGD("string:%s\r\n",string_buffer);
	
	/* [2] switch string to data  */
	data = m_hex_string2i_dec( string_buffer );
//	CP_LOGD("data:0x%.08x\r\n",data);

	return data;
}





#ifdef MRD_DEBUG_CODE
void test_MRD_api( void )
{
	INT32 MRD_flag;
	INT32 MRD_GetValueRes = -1;
	UINT8 i = 0;
	UINT32 MrdValueNum;
	volatile MrdAdc_t temp_MrdData;
	MrdAdc_t MrdDataStructure[MAX_ADC_GROUP_CNT];
	UINT8 MrdValidDataNum = 0;

	/* [1] get MRD flag */
	MRD_flag = MRDAdcFlagGet_boot(ADC_VBAT, TEMP_NORMAL);
	CP_LOGD("MRD_flag:%d\r\n",MRD_flag);

	/* [2] get MRD value number */
	MrdValueNum = MRDAdcCntGet_boot(ADC_VBAT, TEMP_NORMAL);
	if( MrdValueNum > MAX_ADC_GROUP_CNT || MrdValueNum < 2 ){
		CP_LOGE("[DSP_ADC] MrdValueNum: %d is error!\r\n",MrdValueNum);
		while(1);
	}

	/* [3] get MRD value and sort */
	for( i = 0; i < MAX_ADC_GROUP_CNT; i++ ){
		MRD_GetValueRes = MRDAdcValueGet_boot( i, &temp_MrdData.AdcValue, &temp_MrdData.VoltageValue, ADC_VBAT, TEMP_NORMAL);
		if( MRD_GetValueRes != 0 ){
			CP_LOGD("[DSP_ADC] %s, MRD get value fail, MRD_GetValueRes:%d\r\n",__func__,MRD_GetValueRes);
			continue;
		}
		MrdDataStructure[MrdValidDataNum].AdcValue = temp_MrdData.AdcValue;
		MrdDataStructure[MrdValidDataNum].VoltageValue = temp_MrdData.VoltageValue;
		MrdValidDataNum++;
	}
	if( MrdValidDataNum != MrdValueNum ){
		CP_LOGE("[DSP_ADC] MrdValidDataNum is not match with MrdValueNum!\r\n");
		CP_LOGE("MrdValidDataNum:%d,MrdValueNum:%d\r\n",MrdValidDataNum,MrdValueNum);
		while(1);
	}
	MrdBubbleSort(MrdDataStructure,MrdValidDataNum);

	// dump MRD value
	CP_LOGI("index		%s		%s\r\n","MrdAdc  ","MrdVoltage");
	for( i = 0; i < MrdValueNum; i++ ){
		CP_LOGI("%5d		%.08d		%.08d\r\n",i,MrdDataStructure[i].AdcValue, MrdDataStructure[i].VoltageValue);
	}		
}
#endif


void TestL2SRAM( void )
{
	UINT32 L2SRAM_START_ADDR = 0xD1F00000;
	UINT32 SIZE = 0xB000;
	UINT32 L2SRAM_END_ADDR = L2SRAM_START_ADDR + SIZE;

	CP_LOGD("Before Wirte:\r\n");
	CP_LOGD("@%.08x: %x\r\n",L2SRAM_START_ADDR,*(UINT32 *)L2SRAM_START_ADDR);
	CP_LOGD("@%.08x: %x\r\n",L2SRAM_END_ADDR,*(UINT32 *)L2SRAM_END_ADDR);

	*(UINT32 *)L2SRAM_START_ADDR = 0x12345678;
	*(UINT32 *)L2SRAM_END_ADDR   = 0xF1F1F1F1;
	
	CP_LOGD("After Wirte:\r\n");
	CP_LOGD("@%.08x: %x\r\n",L2SRAM_START_ADDR,*(UINT32 *)L2SRAM_START_ADDR);
	CP_LOGD("@%.08x: %x\r\n",L2SRAM_END_ADDR,*(UINT32 *)L2SRAM_END_ADDR);
}

static UINT8 isDcsMode = 1; //DCS mode as default
UINT8 check_if_DCS_mode(void)
{
	static BOOL init_var = TRUE;

	/* [1] init var */
	if( init_var == TRUE ){
		if (!CHIP_IS_CRANE){
			isDcsMode = *(volatile UINT32*)0xD40C001C & 0x1; //SCS RTC reg
			if (isDcsMode)
			   CP_LOGI("in DCS mode.\r\n");
			else
			   CP_LOGI("in SCS mode.\r\n");
		}
		init_var = FALSE;
	}

	/* [2] get DCS mode. NOTE: CRANE platform only support DCS mode. */
    if(CHIP_IS_CRANE)
        return TRUE; //Crane doesn't support SCS.
    else
        return isDcsMode;
}


void PM803_CHARGER_INIT(void)
{
	static BOOL s_init_flag = FALSE;

	if( FALSE == s_init_flag  ){
		s_init_flag = TRUE;
/*+++++++++++ Customer's Code ++++++++++++++++++*/	


/*--------------------END------------------------------*/	
	}
}


/************** Customer's Config Macro ******************	
 *1. CONFIG_PM803_CHARGER_CONECT_FLAG
 *   0 - remove charger detect code.(defaut value as dkb not connect external charger)
 *   1 - add charger detect code when external charger is conected.
 *2. DELAY_TIME_WHEN_CHARGER_CONNECT ( unit: Macro Second)
 *          Default: 6s.
 *****************************************************/
#define CONFIG_PM803_CHARGER_CONNECT_FLAG (0)
#define DELAY_TIME_IN_MICRO_SECOND_WHEN_CHARGER_CONNECT   (6000000)

BOOL PM803_CHARGER_IS_DETECTED(void)
{
	BOOL ret_val = FALSE; // default value
#if (CONFIG_PM803_CHARGER_CONNECT_FLAG == 1)
	ret_val = PM812_CHARGER_IS_DETECTED();
	CP_LOGD("func:%s,ret_val:%d\r\n",__func__,ret_val);
#endif
	return ret_val;
}


void Startup_SoftwareNotProceedUntilReachOneValue(const UINT32 battery_value)
{
	BOOL IsChargerDectect;
	UINT32 battery_level;
	PM803_Battery_Res_t pm803_battery_res;
	UINT32 battery_threshold = battery_value;
	
	CP_LOGD("Enter func:%s\r\n",__func__);
	if(!PMIC_IS_PM803()){
		return;
	}
	Timer0_enable( TRUE );
	while(1){
		/* [1] get battery voltage */
		pm803_get_battery_voltage( &pm803_battery_res );
		battery_level = pm803_battery_res.BatteryValue;
		
		CP_LOGI("pm803_battery_res:%d\r\n",pm803_battery_res.pm803_status_res);
		if( pm803_battery_res.pm803_status_res == PM803_CalbrationFlagNotSet ){
			CP_LOGW("[PM803] CalbrationFlagNotSet, not intercept!\r\n");
			break;
		}
		if( pm803_battery_res.pm803_status_res == PM803_BatteryFlagNotReady ||
			pm803_battery_res.pm803_status_res == PM803_FitFail ||
			pm803_battery_res.pm803_status_res == PM803_Load_Dsp_adc_Fail ||
			pm803_battery_res.pm803_status_res == PM803_Dsp_adc_Not_Exist ){
			CP_LOGW("[PM803] get battery fail, res:%d\r\n",pm803_battery_res.pm803_status_res);
			break;
		}
	
		/* [2] parse vbat value */
		if(battery_level > battery_threshold){
			CP_LOGD("[PM803] vbat is more than %d\r\n",battery_threshold);
			break;
		}
	
		/* [3] check charger is connected ? */
		PM803_CHARGER_INIT();
		IsChargerDectect = PM803_CHARGER_IS_DETECTED();	
		
		if(IsChargerDectect == FALSE){
			// case1: charger not connect
			CP_LOGI("battery level: %d, not more than threshold: %d\r\n",battery_level,battery_threshold);
			CP_LOGW("Charger disconnect, pmic down\r\n");
			PMIC_PowerDown();
			break;
		}else{
			// case2: charger connect
			CP_LOGI("battery level: %d, not more than threshold: %d\r\n",battery_level,battery_threshold);
			CP_LOGI("Charger Connect\r\n");
			CP_LOGI("In Chargering...\r\n");
			DelayInSecond(DELAY_TIME_IN_MICRO_SECOND_WHEN_CHARGER_CONNECT);		// delay 6s
		}
	}
	Timer0_enable( FALSE );
	return;
}


#ifdef AUX_ADC_ATE
INT16 rtn_pm803_def[2] =
{
    323,/*for 400mV*/
    651 /*for 1600mV*/
};

float ate_auxadc_slope=0;
INT16 ate_auxadc_intercept=0;
/* Vbat = Vin * (R3+R4//(R1+R2)) / (R4//(R1+R2)), 
R3 and R4 should be in Kohm unit. Here "R1+R2" is fixed to be 160*9 (unit: Kohm). */
INT16 auxAdcRtn_Resistor3 = 680; //in Kohm
INT16 auxAdcRtn_Resistor4 = 470; //in Kohm

void rf_rtn_adc_calib_init(void)
{
#define GEU_FUSE_BANK0_239_208         (0xD4201000 + 0x0414)
#define GEU_FUSE_BANK0_255_240         (0xD4201000 + 0x04C8)

    UINT32 value1,value2;
    INT16 adcVal1,adcVal2;
    static UINT8 called=FALSE;

    if (called) return;
    called = TRUE;
    
    *((volatile UINT32 *)0xd4282868) = 0x9;
    value1 = *(volatile UINT32 *)GEU_FUSE_BANK0_239_208;
    value2 = *(volatile UINT32 *)GEU_FUSE_BANK0_255_240;
    *((volatile UINT32 *)0xd4282868) = 0x0;

    if ((value1==0)||(value2==0))
    {
        CP_LOGW("No ATE calibration for RF ADC...\r\n");//no calibration applied
    }
    else
    {
        adcVal1 = ((value2&0x3F)<<4)|(value1>>28); //fuse_bank0[245:236]
        adcVal2 = value2>>6; //fuse_bank0[255:246]

        if(CHIP_IS_CRANEM)
        {
            rtn_pm803_def[0]=adcVal2;
            rtn_pm803_def[1]=adcVal1;
        }
        else if(CHIP_IS_CRANEG)
        {
            rtn_pm803_def[0]=adcVal1;
            rtn_pm803_def[1]=adcVal2;
        }
        CP_LOGI("\r\nRF ADC value %d@400mV, %d@1600mV\r\n", rtn_pm803_def[0],rtn_pm803_def[1]);
    }
    ate_auxadc_slope = (float)(1600-400)/(rtn_pm803_def[1]-rtn_pm803_def[0]);
//    CP_LOGD("vRtn_slope = %.3f\r\n",ate_auxadc_slope);
    ate_auxadc_intercept = 400 - (INT16)(ate_auxadc_slope * rtn_pm803_def[0]);
    CP_LOGD("vRtn_intercept = %d\r\n",ate_auxadc_intercept);
}
#endif

static BOOL pm803_GetBatteryReadyFlag( void )
{
	UINT16 PollingFlag = 0;
	unsigned long startTime, endTime;
	UINT16 CevaExecuteStage = 0;

	/* polling flag */
	startTime = GetTimer0CNT();
	do{
		PollingFlag = pm803_get_share_memory_data( DSP_ADC_SHARE_MEMORY_ADDR );
		endTime = GetTimer0CNT();
		if( endTime - startTime > 2000000){
			// do not get PollingFlag and timeout
			break;
		}
	}while( PollingFlag != DSP_ADC_VBATTERY_MEASURE_READY_FLAG );
	
	if( PollingFlag != DSP_ADC_VBATTERY_MEASURE_READY_FLAG ){
		/* flag not right, return speical flag */
		CP_LOGE("PollingFlag:0x%.08x\r\n",PollingFlag);
		// read the dsp_adc execute stage
		CevaExecuteStage = pm803_get_share_memory_data( DSP_ADC_SHARE_MEMORY_ADDR + 0x10 );
		CP_LOGE("[DSP_ADC] dsp_adc run stage:0x%x\r\n",CevaExecuteStage);		
		return FALSE;
	}
	
	CP_LOGI("polling flag spend time: %d us\r\n",endTime - startTime);
	return TRUE;
}

static UINT16 pm803_get_share_memory_data( UINT32 addr )
{
	return *(UINT16 *)addr;
}

static INT32 pm803_FitBatteryValue( UINT32 *p_FitValue )
{
#ifndef AUX_ADC_ATE 
	UINT32 MrdValueNum;
	volatile MrdAdc_t temp_MrdData;
	MrdAdc_t MrdDataStructure[MAX_ADC_GROUP_CNT];
	UINT8 MrdValidDataNum = 0;
	INT32 MRD_GetValueRes = -1;
	UINT8 i = 0;
	float k = 0.0, b = 0.0;
#else
    INT16 Res3 = auxAdcRtn_Resistor3;
    INT16 Res4 = auxAdcRtn_Resistor4;
    INT16 calRtn,AuxVbat;
    float tmp;
#endif
	/* [1] check input parameter */
	if( p_FitValue == NULL ){
		CP_LOGE("[DSP_ADC] %s, invalid parameter!\r\n",__func__);
		return -1;
	}
#ifndef AUX_ADC_ATE 
	/* [2] get MRD value number */
	MrdValueNum = MRDAdcCntGet_boot(ADC_VBAT, TEMP_NORMAL);
	if( MrdValueNum > MAX_ADC_GROUP_CNT || MrdValueNum < 2 ){
		CP_LOGE("[DSP_ADC] MrdValueNum: %d is error!\r\n",MrdValueNum);
		return -1;
	}

	/* [3] get MRD value and sort */
	for( i = 0; i < MAX_ADC_GROUP_CNT; i++ ){
		MRD_GetValueRes = MRDAdcValueGet_boot( i, &temp_MrdData.AdcValue, &temp_MrdData.VoltageValue, ADC_VBAT, TEMP_NORMAL);
		if( MRD_GetValueRes != 0 ){
			if( MRD_GetValueRes != 1 ){
				CP_LOGD("[DSP_ADC] %s, MRD get value fail, MRD_GetValueRes:%d\r\n",__func__,MRD_GetValueRes);
			}
			continue;
		}
		MrdDataStructure[MrdValidDataNum].AdcValue = temp_MrdData.AdcValue;
		MrdDataStructure[MrdValidDataNum].VoltageValue = temp_MrdData.VoltageValue;
		MrdValidDataNum++;
	}
	if( MrdValidDataNum != MrdValueNum ){
		CP_LOGE("[DSP_ADC] MrdValidDataNum is not match with MrdValueNum!\r\n");
		CP_LOGE("MrdValidDataNum:%d,MrdValueNum:%d\r\n",MrdValidDataNum,MrdValueNum);
		return -1;
	}
	MrdBubbleSort(MrdDataStructure,MrdValidDataNum);	// only spend 4us to execute bubble sort

	// dump MRD value
	CP_LOGD("index		%s		%s\r\n","MrdAdc  ","MrdVoltage");
	for( i = 0; i < MrdValueNum; i++ ){
		CP_LOGD("%5d		%.08d		%.08d\r\n",i,MrdDataStructure[i].AdcValue, MrdDataStructure[i].VoltageValue);
	}

	/* [4] fit value */
	if( *p_FitValue < MrdDataStructure[0].AdcValue ){
		// case 1
		i = 0;
	}else if( *p_FitValue > MrdDataStructure[MrdValidDataNum - 1].AdcValue ){
		// case 2
		i = MrdValidDataNum - 2;
	}else{
		// case 3
		for( i = 0; i < MrdValidDataNum - 1; i++ ){
			if( *p_FitValue > MrdDataStructure[i].AdcValue && *p_FitValue < MrdDataStructure[i+1].AdcValue ){
				break;
			}
		}
	}
	k = ( (float)MrdDataStructure[i+1].VoltageValue - MrdDataStructure[i].VoltageValue ) / ( MrdDataStructure[i+1].AdcValue - MrdDataStructure[i].AdcValue );
	b = (float)MrdDataStructure[i].VoltageValue - MrdDataStructure[i].AdcValue * k ;

	*p_FitValue = (INT32)(k * (*p_FitValue) + b);
#else
    rf_rtn_adc_calib_init();

    calRtn = (INT16)(ate_auxadc_slope * (*p_FitValue)+ate_auxadc_intercept);
    CP_LOGD("calRtnVol = %d, curRtnAdc = %d\r\n",calRtn,(*p_FitValue));

    tmp = (float)Res3/Res4+(float)Res3/(160*9)+1;
    AuxVbat = (INT16)(calRtn * tmp);
    CP_LOGD("GetAuxAdcRtnVbat_customized = %d\r\n",AuxVbat);

	*p_FitValue = AuxVbat;

#endif
	
	CP_LOGD("fit end\r\n");
	return 0;
}


static void pm803_ResetBatteryReadyFlag( void )
{
	memset((void *)DSP_ADC_SHARE_MEMORY_ADDR,0,sizeof(UINT16));
}

static void pm803_print_debug_info( void )
{
	UINT16 debug_measure_vaule[5] = { 0 };
	UINT8 i = 0;
	UINT16 CevaExecuteStage = 0;
	// read the 5 times measure vale
	for( ; i < 5; i++ ){
		debug_measure_vaule[i] = pm803_get_share_memory_data( DSP_ADC_SHARE_MEMORY_ADDR + 4 + i * 2 );
		CP_LOGD("[DSP_ADC] debug_measure_vaule[%d]:0x%.08x\r\n",i,debug_measure_vaule[i]);
	}

	// read the dsp_adc execute stage
	CevaExecuteStage = pm803_get_share_memory_data( DSP_ADC_SHARE_MEMORY_ADDR + 0x10 );
	CP_LOGD("[DSP_ADC] dsp_adc run stage:0x%x\r\n",CevaExecuteStage);
}

void pm803_get_battery_voltage( PM803_Battery_Res_t *p_pm803_battery_res )
{
	Dsp_adc_type_t dsp_adc_type;
	static Dsp_adc_LoadImage_Res_t s_load_res;
	static BOOL s_LoadDspAdcFlag = FALSE;

	if( p_pm803_battery_res == NULL ){
		CP_LOGE("%s,input parameter error!\r\n",__func__);
		while(1);
	}
	
	/* [0] init*/
	p_pm803_battery_res->BatteryValue = 0;
	p_pm803_battery_res->pm803_status_res = PM803_CalbrationFlagNotSet;
	
#ifndef AUX_ADC_ATE 
	/* [1] check calibration flag is set or not */
	if( MRDAdcFlagGet_boot(ADC_VBAT, TEMP_NORMAL) != 1 ){
		CP_LOGW("[MRD] Mrd Calibration Flag not set!\r\n");
		p_pm803_battery_res->pm803_status_res = PM803_CalbrationFlagNotSet;
		return;
	}
#endif

	/* [2] check dsp_adc exist or not ? */
	if( 1 != is_have_dsp_adc() ){
		p_pm803_battery_res->pm803_status_res = PM803_Dsp_adc_Not_Exist;
#if 1		
		CP_LOGW("[DSP_ADC] Not append dsp_adc at the end of image!\r\n");	
		while(1);
#else		
		return;
#endif		
	}

	/* [3] load and release dsp_adc */
	if( FALSE == s_LoadDspAdcFlag ){
		s_LoadDspAdcFlag = TRUE;
		//[3.1] load dsp_adc.bin
#ifdef SMALL_CODE_LOGO
		if(IsLogoAppendDspAdc() == TRUE){
			dsp_adc_type = dsp_adc_type4_in_logo;
		}else{
			dsp_adc_type = dsp_adc_type1;
		}
#elif SMALL_CODE_UPDATER
		if(IsUpdaterAppendDspAdc() == TRUE){
			dsp_adc_type = dsp_adc_type3_in_updater;
		}else{
			dsp_adc_type = dsp_adc_type2;
		}
#endif
		s_load_res = load_dsp_adc( dsp_adc_type );
	}
	CP_LOGD("load end\r\n");
	if( s_load_res == Dsp_adc_LoadFail || s_load_res == Dsp_adc_para_error ){
		CP_LOGE("[DSP_ADC] load fail,load_dsp_adc_res:%d\r\n",s_load_res);
		p_pm803_battery_res->pm803_status_res = PM803_Load_Dsp_adc_Fail;
		return;
	}
	if( s_load_res == Dsp_adc_ImageNotExist ){
		p_pm803_battery_res->pm803_status_res = PM803_Dsp_adc_Not_Exist;
		return;
	}
	
	//[3.2] release dsp_adc.bin
	pm803_reset_dsp_adc_ready_flag();		// reset dsp_adc ready flag before release dsp_adc.bin
	boot_dsp_adc_external_from_squ_new();

	/* [4] check battery flag is ready ?  */
	if( pm803_GetBatteryReadyFlag() != TRUE ){
		CP_LOGE("[DSP_ADC] time out and get battery Ready Flag fail\r\n");
		p_pm803_battery_res->pm803_status_res = PM803_BatteryFlagNotReady;
		return;
	}

	/* [5] get adc measure from dsp_adc share memory */
	p_pm803_battery_res->BatteryValue = ( UINT32 )pm803_get_share_memory_data( DSP_ADC_SHARE_MEMORY_ADDR + 2 );
//	hold_dsp_adc();
	CP_LOGD("Before fitting, meas_val:0x%.08x\r\n",p_pm803_battery_res->BatteryValue);
#ifdef DSP_ADC_DEBUG
	pm803_print_debug_info();
#endif

	/* [6] fit the measure value */
	INT32 ret = 0;
	ret = pm803_FitBatteryValue( &p_pm803_battery_res->BatteryValue );
	if( 0 != ret  ){
		CP_LOGE("[DSP_ADC] fit value fail,ret: %d\r\n",ret);
		p_pm803_battery_res->pm803_status_res = PM803_FitFail;
		return;
	}
	CP_LOGD("After fitting, meas_val:%d\r\n",p_pm803_battery_res->BatteryValue);

	p_pm803_battery_res->pm803_status_res = PM803_GetBatteryOk;
	return;
}


/**
 * vribator trigger description
 * 1. PM813
 *     (1) trigger by pm813 register
 * 2. PM803 
 *     (1) CraneG  --- trigger by GPIO32
 *             Note: this GPIO can be modified according to the customer.
 */

#define Vibrator_GPIO (32)


void VibratorEnable(void)
{
    unsigned char var;
	
	if(PMIC_IS_PM803()){
		if(CHIP_IS_CRANEG){
			cgpio_set_output(Vibrator_GPIO);
			cgpio_set_value(Vibrator_GPIO, 1);
		}
	}else{
	    NingboWrite(NINGBO_BASE_Reg, 0x43, 0x7f); 

	    NingboRead(NINGBO_BASE_Reg, 0x4C, &var);
	    NingboWrite(NINGBO_BASE_Reg, 0x4C, (var|(0x1<<3))); 
	}
}
void VibratorDisable(void)
{
    unsigned char var;
	
	if(PMIC_IS_PM803()){
		if(CHIP_IS_CRANEG){
			cgpio_set_output(Vibrator_GPIO);
			cgpio_set_value(Vibrator_GPIO, 0);
		}
	}else{
	    NingboRead(NINGBO_BASE_Reg, 0x4C, &var);
	    NingboWrite(NINGBO_BASE_Reg, 0x4C, (var&~(0x1<<3)));
	}
}

