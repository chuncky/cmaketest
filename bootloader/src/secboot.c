#include "secboot.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reg.h"
#include "ptable.h"
#include "loadtable.h"
#include "fip.h"

extern int check_integrity(void *img, unsigned int img_len);
extern int get_auth_param(const auth_param_type_desc_t *type_desc,
		void *img, unsigned int img_len,
		void **param, unsigned int *param_len);
extern int verify_signature(void *data_ptr, unsigned int data_len,
			    void *sig_ptr, unsigned int sig_len,
			    void *sig_alg, unsigned int sig_alg_len,
			    void *pk_ptr, unsigned int pk_len
				);
extern int verify_hash(void *data_ptr, unsigned int data_len,
		       void *digest_info_ptr, unsigned int digest_info_len);

//static sb_issue_item_list
static const sb_issue_item_map_t sb_issue_item_list[] = {
	 {ITEM_CP   ,"CP"   ,ASR_OS_CP_KEY_CERT_ID     ,ASR_OS_CP_IMAGE_ID    ,NON_TRUSTED_CP_HASH_OID     }
	,{ITEM_DSP  ,"DSP"  ,ASR_OS_DSP_KEY_CERT_ID    ,ASR_OS_DSP_IMAGE_ID   ,NON_TRUSTED_DSP_HASH_OID    }
	,{ITEM_APP  ,"APP"  ,ASR_OS_APP_KEY_CERT_ID    ,ASR_OS_APP_IMAGE_ID   ,NON_TRUSTED_APP_HASH_OID    }
	,{ITEM_USER1,"USER1",ASR_OS_USER1_KEY_CERT_ID  ,ASR_OS_USER1_IMAGE_ID ,NON_TRUSTED_USER1_HASH_OID  }
	,{ITEM_USER2,"USER2",ASR_OS_USER2_KEY_CERT_ID  ,ASR_OS_USER2_IMAGE_ID ,NON_TRUSTED_USER2_HASH_OID  }
	,{ITEM_USER3,"USER3",ASR_OS_USER3_KEY_CERT_ID  ,ASR_OS_USER3_IMAGE_ID ,NON_TRUSTED_USER3_HASH_OID  }
};

void secboot_init()
{
	//_rt_heap init
	#define M_HEAP_SIZE 1024*16
	static char m_heap[M_HEAP_SIZE]={0};
	_init_alloc(m_heap,m_heap+M_HEAP_SIZE);

	//parser the fwcerts.bin setup fip_image_t of image & certification contents
    _ptentry *fipEntry = ptable_find_entry("fwcerts");
	fip_open(fipEntry->vstart);
}

void secboot_item_check(sb_issue_item_id_t item_id_idx)
{
    unsigned long startTime,endTime;
	int rc = 0;

    startTime = GetTimer0CNT(); 

	SECBOOT_DBG("[SECBOOT]sb_item item_id = [%d] check\r\n",item_id_idx);

	//parse the sb_issue_item
	sb_issue_item_map_t * m_sb_item_ptr = sb_issue_item_list;
	do{
		if(m_sb_item_ptr->item_id >= ITEM_END){
			SECBOOT_DBG("[SECBOOT]sb_item [%d][%d][%d] index error \r\n",
					item_id_idx,m_sb_item_ptr->item_id,ITEM_END);
            goto secboot_error;
		}
		if(m_sb_item_ptr->item_id == item_id_idx){
			//bingo
			SECBOOT_DBG("[SECBOOT]sb_item [%d][%s][%d][%d][%s]\r\n",
					m_sb_item_ptr->item_id,
					m_sb_item_ptr->item_name,
					m_sb_item_ptr->item_cert_id,
					m_sb_item_ptr->item_img_id,
					m_sb_item_ptr->item_hash_oid
					);
			break;
		}

		m_sb_item_ptr++;
	}while(1);
	
	//certification fip image parser setup
	fip_image_t cert_toc_fip;
	fip_image_handle cert_toc_fih = &cert_toc_fip;
	rc = fip_open_image(m_sb_item_ptr->item_cert_id, cert_toc_fih);
	if(rc == FIP_ERR_IMG_MISS)
	{
		uart_printf("[SECBOOT]fip_open_image [%d] with FIP_ERR_IMG_MISS, just skip\r\n",m_sb_item_ptr->item_cert_id);
		return;
	}else if (rc != FIP_SUCCEED){
		goto secboot_error;
	}
	//image fip image parser setup
	fip_image_t img_toc_fip;
	fip_image_handle img_toc_fih = &img_toc_fip;
	rc = (fip_open_image(m_sb_item_ptr->item_img_id, img_toc_fih));
	if (rc != FIP_SUCCEED){
		goto secboot_error;
	}
#if 0
	SECBOOT_DBG("[SECBOOT]cert_toc_fih->id       = [0x%0.8x]\r\n",cert_toc_fih->id);
	SECBOOT_DBG("[SECBOOT]cert_toc_fih->start    = [0x%0.8x]\r\n",cert_toc_fih->start);
	SECBOOT_DBG("[SECBOOT]cert_toc_fih->offset   = [0x%0.8x]\r\n",cert_toc_fih->offset);
	SECBOOT_DBG("[SECBOOT]cert_toc_fih->size     = [0x%0.8x]\r\n",cert_toc_fih->size);
	SECBOOT_DBG("[SECBOOT]img_toc_fih->id        = [0x%0.8x]\r\n",img_toc_fih->id);
	SECBOOT_DBG("[SECBOOT]img_toc_fih->start     = [0x%0.8x]\r\n",img_toc_fih->start);
	SECBOOT_DBG("[SECBOOT]img_toc_fih->offset    = [0x%0.8x]\r\n",img_toc_fih->offset);
	SECBOOT_DBG("[SECBOOT]img_toc_fih->size      = [0x%0.8x]\r\n",img_toc_fih->size);
#endif

	//STEP2
	//check integrity of the certification content
	SECBOOT_DBG("[SECBOOT][%s]cert check_integrity start\r\n",m_sb_item_ptr->item_name);
	rc = check_integrity(cert_toc_fih->start+cert_toc_fih->offset,
					cert_toc_fih->size);
	SECBOOT_DBG("[SECBOOT][%s]cert check_integrity done rc = %d\r\n",m_sb_item_ptr->item_name,rc);
	if(rc)
		goto secboot_error;


	//STEP3
	//verify signature
	auth_param_type_desc_t sig_param_desc		={AUTH_PARAM_SIG	,0};
	char *sig_ptr;
	unsigned int sig_len;
	get_auth_param(&sig_param_desc,NULL,NULL,&sig_ptr,&sig_len);
#if 0
	SECBOOT_DBG("[SECBOOT]DUMP SIG\r\n");
	display_binary(sig_ptr,sig_len);
#endif

	auth_param_type_desc_t sig_alg_param_desc	={AUTH_PARAM_SIG_ALG,0};
	char *sig_alg_ptr;
	unsigned int sig_alg_len;
	get_auth_param(&sig_alg_param_desc,NULL,NULL,&sig_alg_ptr,&sig_alg_len);
#if 0
	SECBOOT_DBG("[SECBOOT]DUMP SIG_ALG\r\n");
	display_binary(sig_alg_ptr,sig_alg_len);
#endif

	auth_param_type_desc_t pk_param_desc		={AUTH_PARAM_PUB_KEY,0};
	char *pk_ptr;
	unsigned int pk_len;
	get_auth_param(&pk_param_desc,NULL,NULL,&pk_ptr,&pk_len);
#if 0
	SECBOOT_DBG("[SECBOOT]DUMP PK\r\n");
	display_binary(pk_ptr,pk_len);
#endif

	auth_param_type_desc_t tbs_param_desc		={AUTH_PARAM_RAW_DATA,0};
	char *tbs_ptr;
	unsigned int tbs_len;
	get_auth_param(&tbs_param_desc,NULL,NULL,&tbs_ptr,&tbs_len);
#if 0
	SECBOOT_DBG("[SECBOOT]DUMP TBS\r\n");
	display_binary(tbs_ptr,tbs_len);
#endif


	rc = verify_signature((void*)tbs_ptr,tbs_len,
			sig_ptr,sig_len,
			sig_alg_ptr,sig_alg_len,
			pk_ptr,pk_len
			);
	if(rc){
		SECBOOT_DBG("[SECBOOT][%s] verify_signature failed with rc=[%d]\r\n",rc);
		goto secboot_error;
	}

	//STEP4
	//verify image
	auth_param_type_desc_t img_auth_param_desc={AUTH_PARAM_HASH,m_sb_item_ptr->item_hash_oid};
	char *img_hash_ptr;
	unsigned int img_hash_len;
	get_auth_param(&img_auth_param_desc,NULL,NULL,&img_hash_ptr,&img_hash_len);
#if 0
	SECBOOT_DBG("[SECBOOT]DUMP EXT hash_oid[]\r\n");
	display_binary(img_hash_ptr,img_hash_len);
#endif


	SECBOOT_DBG("[SECBOOT][%s]img_content verify_hash start\r\n",m_sb_item_ptr->item_name);
	rc = verify_hash((void*)(img_toc_fih->start+img_toc_fih->offset),img_toc_fih->size,
			img_hash_ptr,img_hash_len);
	SECBOOT_DBG("[SECBOOT][%s]img_content verify_hash done rc = %d\r\n",m_sb_item_ptr->item_name,rc);
	if(rc)
		goto secboot_error;

    endTime = GetTimer0CNT(); 
	SECBOOT_DBG("[SECBOOT][%s]secboot check done SUCCEED\r\n",m_sb_item_ptr->item_name);
	SECBOOT_DBG("[SECBOOT][%s]secboot time consumption = %ld ms\r\n",
			m_sb_item_ptr->item_name,Timer0IntervalInMilli(startTime,endTime));

	return;

secboot_error:
    endTime = GetTimer0CNT(); 
	SECBOOT_DBG("[SECBOOT][%s]secboot check done FAILED\r\n",m_sb_item_ptr->item_name);
	SECBOOT_DBG("[SECBOOT][%s]secboot time consumption = %ld ms\r\n",
			m_sb_item_ptr->item_name,Timer0IntervalInMilli(startTime,endTime));
	PM812_SW_PDOWN();
	return;
}
