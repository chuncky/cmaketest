#include "utlAtParser.h"
#include "telatci.h"
#include "telatparamdef.h"
#include "ci_dev.h"
#include"utlMalloc.h"
#include "utlTypes.h"
#include "teldev.h"
#include "mat_response.h"

extern CiServiceHandle 	  gAtciSvgHandle[];
#ifndef PLATFORM_FOR_PS_LW

static utlAtParameter_T starLTEBAND_params[] = { utlDEFINE_DECIMAL_AT_PARAMETER(	utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_REQUIRED),
	utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),
	utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),
	utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),
	utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),
	utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),
	utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),	
	utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),
	utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL), 
	utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),
	utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL), 
	utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),
	utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),
	utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),
	utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),
	utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),
	utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),	
	utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),
	utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL), 
	utlDEFINE_DECIMAL_AT_PARAMETER(    utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),};


/************************************************************************************
 * F@: ciBAND - GLOBAL API for AT*LTEBAND command
 *
 */
RETURNCODE_T  ciSetLTEBand(            const utlAtParameterOp_T op,
				 const char                      *command_name_p,
				 const utlAtParameterValue_P2c parameter_values_p,
				 const size_t num_parameters,
				 const char                      *info_text_p,
				 unsigned int                    *xid_p,
				 void                            *arg_p)
{

	UNUSEDPARAM(command_name_p);
	UNUSEDPARAM(info_text_p);

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;

	UINT8		numOfLteBands=0;
	int 		lteBands[CI_DEV_MAX_LTE_BAND_ORDER_NUM]={0};
	UINT8 index=0;
	BOOL cmdValid = TRUE;


    UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);


	/*
	 *  Put parser index into the variable
	 */
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 * process operation
	 */
	switch ( op )
	{

		case TEL_EXT_GET_CMD:         /* AT*LTEBAND? */
		{
			ret = DEV_GetLTEBand(atHandle);
			break;
		}

	    case TEL_EXT_SET_CMD: /* AT*LTEBAND= */
	    {
			for(index=0;index<num_parameters;index++)
			{
				if(parameter_values_p[index].is_default!=TRUE)
				{
					if(getExtValue(parameter_values_p, index, &(lteBands[index]), TEL_AT_LTEBAND_NW_MODE_VAL_MIN, TEL_AT_LTEBAND_NW_MODE_VAL_MAX, TEL_AT_LTEBAND_NW_MODE_VAL_DEFAULT ) != TRUE)
					{
						cmdValid=FALSE;
						break;

					}

				}
				else
				{
						if(index==0)cmdValid=FALSE;
						break;


				}


	    	}
			if(cmdValid==FALSE)
			{
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);

			}
			else
			{
				numOfLteBands=index;
				ret = DEV_SetLTEBand(atHandle, numOfLteBands, lteBands);


			}
			break;
		}
		case TEL_EXT_TEST_CMD:
		case TEL_EXT_ACTION_CMD:           /* AT*LTEBAND */
		default:
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
			break;
		}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}

CiReturnCode DEV_GetLTEBand(UINT32 atHandle)

{

	CiReturnCode ret = CIRC_FAIL;

	CiDevPrimGetLteBandOrderReq		  *GetLteBandOrderReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_GET_LTE_BAND_ORDER_REQ,
		 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_GET_LTE_BAND_ORDER_REQ), (void*)GetLteBandOrderReq);

	return ret;


}

CiReturnCode DEV_SetLTEBand(UINT32 atHandle, UINT8 numOfLteBands, int*lteBands)
{


	CiReturnCode ret = CIRC_FAIL;
	UINT8 index=0;
	CiDevPrimSetLteBandOrderReq		  *SetLteBandOrderReq = NULL;
	SetLteBandOrderReq = (CiDevPrimSetLteBandOrderReq *)utlCalloc(1, sizeof(CiDevPrimSetLteBandOrderReq));
	if (SetLteBandOrderReq == NULL)
		return CIRC_FAIL;

	SetLteBandOrderReq->numOfLteBands=numOfLteBands;
	

	DIAG_FILTER(CI, MMI, DEV_SetLTEBand_1, DIAG_INFORMATION)
	diagPrintf("numOfLteBands: %d" ,SetLteBandOrderReq->numOfLteBands);

	for(index=0;index<numOfLteBands;index++)
	{

		SetLteBandOrderReq->lteBands[index]=lteBands[index];


	}
	
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_SET_LTE_BAND_ORDER_REQ,
	 	MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_SET_LTE_BAND_ORDER_REQ), (void*)SetLteBandOrderReq);

	return ret;



}

static void processGetLTEBandConf(UINT32 atHandle, const void *paras)
{

	UINT8 index=0;
	char RspBuf[100]={0};
	CiDevPrimGetLteBandOrderCnf *GetLteBandOrderCnf = (CiDevPrimGetLteBandOrderCnf *)paras;
	if(GetLteBandOrderCnf->rc == CIRC_DEV_SUCCESS)
	{

		if(GetLteBandOrderCnf->numOfLteBands>0)
		{

			snprintf(RspBuf, sizeof(RspBuf), "*LTEBAND:");

			for(index=0;index<GetLteBandOrderCnf->numOfLteBands;index++)
			{
				snprintf(RspBuf+strlen(RspBuf), sizeof(RspBuf)-strlen(RspBuf), ",%d", GetLteBandOrderCnf->lteBands[index]);
			}
			
			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, RspBuf);
		}

		
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
	}
	return;

	
}

static void processSetLTEBandConf(UINT32 atHandle, const void *paras)
{

	CiDevPrimSetLteBandOrderCnf *SetLteBandOrderCnf = (CiDevPrimSetLteBandOrderCnf *)paras;
	if(SetLteBandOrderCnf->rc==CIRC_DEV_SUCCESS)
	ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
	else
	ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
	
}
#endif
BOOL mmiCnf(CiServiceGroupID  svgId,
		CiPrimitiveID     primId,
		CiRequestHandle   reqHandle,
		void*             paras)
{
	UINT32 atHandle = GET_AT_HANDLE(reqHandle);
	
	if (!GET_SIM1_FLAG(reqHandle)) 
	{
		/*sim card 0*/
	} else 
	{
		/*sim card 1*/
	

	}

	/*TODO: MMI process CI response here*/
	
	if(svgId == CI_SG_ID_DEV)
	{
		switch(primId)
		{
			#ifndef PLATFORM_FOR_PS_LW
			case CI_DEV_PRIM_SET_LTE_BAND_ORDER_CNF:
			{
				processSetLTEBandConf(atHandle, paras);	
				break;
			}

			case CI_DEV_PRIM_GET_LTE_BAND_ORDER_CNF:
			{
				processGetLTEBandConf(atHandle, paras); 
				break;
			}
			#endif
			default:
				return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	/* free up the confirmation memory */
	atciSvgFreeCnfMem( svgId, primId, paras );

	DIAG_FILTER(CI, MMI, mmiCnf_1, DIAG_INFORMATION)
	diagPrintf("mmiCnf handled: svgId %d, primId %d, reqHandle 0x%lx" ,svgId,primId,reqHandle);

	return TRUE;
}


utlAtCommand_T shell_commands_mmi[]=
{
	utlDEFINE_EXTENDED_AT_COMMAND(MAT_starLTEBAND,"*LTEBAND", starLTEBAND_params, "*LTEBAND:", ciSetLTEBand, ciSetLTEBand),
	/*TODO: MMI add new at commands here*/
};
const unsigned int shell_commands_num_mmi = utlNumberOf(shell_commands_mmi);

void ATTestHandleRespInd(TelAtParserID sAtpIndex, const char* response, int size, MATReturnPara *retData)
{
	MATRspParseRet ret;
	
	MATSimId sim_id = NUM_OF_MAT_SIM;
	BOOL isIndication = FALSE;
	
	#define MATTracePrintf uart_printf
	if((sAtpIndex == MAT_IND_CHANNEL) || (sAtpIndex == MAT_IND_CHANNEL_1))	
	{
		isIndication = TRUE;
	}

	MATTracePrintf("%s[%s]:  size %d, data %s\r\n", __FUNCTION__,isIndication?"indication":"response",size,response);
	if(isIndication)	
	{
		ret = MATHandleIndication(response, size,retData);
	}
	else
	{
		ret = MATHandleResponse(sAtpIndex, response,size, retData);
	}
	
	MATTracePrintf("%s: parse result = %d\r\n", __FUNCTION__,ret);
	if(ret != MAT_RSP_PARSER_WAIT_FOR_TERMINAL)
	{
		/*For MMI, please implement code to handle AT response/indication*/
		
		MATTracePrintf("%s:  dump parmeters value of MATReturnPara\r\n", __FUNCTION__);
		//dump structure MATReturnPara
		MATConfIndCB_Dump(sim_id, retData,MAT_RET_CONFIRM);

		MATTracePrintf("%s:  free the memory allocated during parse AT responst string\r\n", __FUNCTION__);
        // free the memory allocated during parse
		MATConfIndFree(retData);
	}

	#undef MATTracePrintf
}


void at_conf_ind_callback(TelAtParserID sAtpIndex, const char* response, int size)
{
	MATReturnPara resp_return;
		
	//parse at response/indication string to structure MATReturnPara
	ATTestHandleRespInd(sAtpIndex,response,strlen(response),&resp_return);
}


//ICAT EXPORTED FUNCTION - VALI_IF,MAT,MMI_Command_Test
void MMI_Command_Test()
{
	char *get_LTEBAND = "AT*LTEBAND?\r\n";

	//set callback function to receive at response/indication
	
	MATSetConfIndCB(TEL_AT_CMD_ATP_0,at_conf_ind_callback);
	MATSetConfIndCB(TEL_AT_CMD_ATP_36,at_conf_ind_callback);

	MATSetConfIndCB(TEL_AT_CMD_ATP_6,at_conf_ind_callback);
	MATSetConfIndCB(TEL_AT_CMD_ATP_44,at_conf_ind_callback);
	
	if(IsAtCmdSrvReady())
	{
		//send AT*LTEBAND?
		ATRecv(TEL_AT_CMD_ATP_6, get_LTEBAND, strlen(get_LTEBAND));
		ATRecv(TEL_AT_CMD_ATP_44, get_LTEBAND, strlen(get_LTEBAND));
	}
}
