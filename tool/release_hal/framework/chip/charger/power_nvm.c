#include <stdio.h>
#include <string.h>
////#include "sdkfs.h"
//#include "FDI_EXT.h"
//#include "FDI_FILE.h"
//#include "FDI_ERR.h"
//#include "nvm_header.h"

typedef unsigned int FILE_ID;

//ICAT EXPORTED STRUCT
typedef struct NVM_Header
{
    unsigned long   StructSize;// the size of the user structure below
    unsigned long   NumofStructs;// >1 in case of array of structs (default is 1).
    char    StructName[64];// the name of the user structure below
    char    Date[32];// date updated by the ICAT when the file is saved. Filled by calibration SW.
    char    time[32];// time updated by the ICAT when the file is saved. Filled by calibration SW.
    char    Version[64];// user version - this field is updated by the SW eng. Every time they update the UserStruct.
    char    HW_ID[32];// signifies the board number. Filled by calibration SW.
    char    CalibVersion[32];// signifies the calibration SW version. Filled by calibration SW.
} NVM_Header_ts;


typedef signed char     INT8;         /* Signed 8-bit quantity          */
typedef signed short    INT16;        /* Signed 16-bit quantity         */
typedef signed long     INT32;        /* Signed 32-bit quantity         */
typedef unsigned char   UINT8;        /* Unsigned 8-bit quantity        */
typedef unsigned short  UINT16;       /* Unsigned 16-bit quantity       */
typedef unsigned long   UINT32;       /* Unsigned 32-bit quantity       */
typedef unsigned long   BU_U32;       /* Unsigned 32-bit quantity       */

/* Standard typedefs */
typedef unsigned char   BOOL;         /* Boolean                        */

#ifndef TRUE
  #define FALSE   0
  #define TRUE    1
#endif /* TRUE */

extern FILE_ID FDI_fopen(const char *filename_ptr, const char *mode);
extern int FDI_fclose(FILE_ID stream);
extern size_t FDI_fread(void *buffer_ptr, size_t element_size, size_t count, FILE_ID stream);
extern size_t FDI_fwrite(const void *buffer_ptr,size_t element_size,size_t count,FILE_ID stream);
extern int FDI_fseek(FILE_ID stream, long offset, int wherefrom);

/*------------------------------------------CHARGE CFG START------------------------------------------*/
/*charge config Setting */
const char* const chargerCfgFileName="charger_config.nvm";

//ICAT EXPORTED ENUM
typedef enum {
    CHARGER_DEBUG_DIS = 0,
    CHARGER_DEBUG_EN
} CHARGE_DEBUG_TYPE;

//ICAT EXPORTED STRUCT
typedef struct {
    CHARGE_DEBUG_TYPE chargerDebugType;
} ChargerCfg_t;

static ChargerCfg_t chargerCfg =
{
    CHARGER_DEBUG_DIS,
};

#define NVM_HEAD_SET_STRING(header,field,value) strncpy(header.field,value,sizeof(header.field)-1)

static BOOL chargerWriteNvmHeader(FILE_ID fdiID)
{
    NVM_Header_ts header;
    UINT32 ret;

    FDI_fseek(fdiID,0,SEEK_SET);
    header.StructSize = sizeof(ChargerCfg_t);
    header.NumofStructs = 1;
    NVM_HEAD_SET_STRING(header,StructName,"ChargerCfg_t");
    NVM_HEAD_SET_STRING(header,Date,"");
    NVM_HEAD_SET_STRING(header,time,"");
    NVM_HEAD_SET_STRING(header,Version,  "1.0");
    NVM_HEAD_SET_STRING(header,HW_ID,"");
    NVM_HEAD_SET_STRING(header,CalibVersion,"");

    ret = FDI_fwrite(&header,sizeof(NVM_Header_ts),1,fdiID);
    if(ret != 1)
        return FALSE;
    return TRUE;
}

BOOL chargerCfgGetSetting(void)
{
    UINT32 ret;
    NVM_Header_ts header;
    FILE_ID fdiID;
    UINT16 count;

    fdiID = FDI_fopen(chargerCfgFileName,"rb");
    if(fdiID == 0) {
        fdiID = FDI_fopen(chargerCfgFileName,"wb");
        if(fdiID) {
            if(!chargerWriteNvmHeader(fdiID)) {
                FDI_fclose(fdiID);
                return FALSE;
            }
            ret = FDI_fwrite(&chargerCfg, sizeof(ChargerCfg_t), 1, fdiID);
            FDI_fclose(fdiID);
            if(ret != 1)
                return FALSE;
            else
                return TRUE;
        }
        else
            return FALSE;
    } else {
        count = FDI_fread(&header, sizeof(NVM_Header_ts), 1, fdiID );
        if(count != 1) {
            FDI_fclose(fdiID);
            return FALSE;
        }
        count = FDI_fread(&chargerCfg, sizeof(ChargerCfg_t), 1, fdiID );
        FDI_fclose(fdiID);
        if(count != 1)
            return FALSE;
        else
            return TRUE;
    }
}

BOOL IschargerDebugEn(void)
{
    return (chargerCfg.chargerDebugType == CHARGER_DEBUG_EN)? TRUE: FALSE;
}
/*------------------------------------------CHARGE CFG END------------------------------------------*/

