#ifndef __PMUM_H__
#define __PMUM_H__

typedef struct _PMUM_TypeDef
{
    volatile unsigned int CPCR;
    volatile unsigned int CPSR;
    volatile unsigned int FCCR;
    volatile unsigned int POCR;
    volatile unsigned int POSR;
    volatile unsigned int SUCCR;
    volatile unsigned int VRCR;
    volatile unsigned int RESERVED0;
    volatile unsigned int CPRR;
    volatile unsigned int CCGR;
    volatile unsigned int CRSR;
    volatile unsigned int XDCR;
    volatile unsigned int GPCR;
    volatile unsigned int PLL2CR;
    volatile unsigned int SCCR;    //0x0038
    volatile unsigned int MCCR;
    volatile unsigned int ISCCRX[2];
    volatile unsigned int CWUCRS;
    volatile unsigned int CWUCRM;  //0X004C
    volatile unsigned int RESERVED1[(0x100-0x4c)/4-1];
    volatile unsigned int DSOC;//0X100
    volatile unsigned int RESERVED2[(0x200-0x100)/4-1];
    volatile unsigned int WDTPCR;//0X200
    volatile unsigned int RESERVED3[(0x400-0x200)/4-1];
    volatile unsigned int CMPRX[5];//0X400
    volatile unsigned int RESERVED5[(0x1000-0x410)/4-1];
    volatile unsigned int APCR;//0X1000
    volatile unsigned int APSR;//0X1004
    volatile unsigned int RESERVED6[(0x1020-0x1004)/4-1];
    volatile unsigned int APRR;//0x1020
    volatile unsigned int ACGR;
    //volatile unsigned int ARSR;//0X1028
    volatile unsigned int RESERVED7[(0x20)/4-1];
    volatile unsigned int AWUCRS;//0X1048
    volatile unsigned int AWUCRM;
} PMUM_TypeDef;


#define    PMUM_BASE      0xD4050000
#define    PMUM  ((PMUM_TypeDef *) PMUM_BASE )

#endif
