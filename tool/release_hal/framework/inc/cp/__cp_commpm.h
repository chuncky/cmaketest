/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

/************************************************************************/
/*  COPYRIGHT (C) 2002 Intel Corporation.                               */
/*                                                                      */
/*  This file and the software in it is furnished under                 */
/*  license and may only be used or copied in accordance with the terms */
/*  of the license. The information in this file is furnished for       */
/*  informational use only, is subject to change without notice, and    */
/*  should not be construed as a commitment by Intel Corporation.       */
/*  Intel Corporation assumes no responsibility or liability for any    */
/*  errors or inaccuracies that may appear in this document or any      */
/*  software that may be provided in association with this document.    */
/*  Except as permitted by such license, no part of this document may   */
/*  be reproduced, stored in a retrieval system, or transmitted in any  */
/*  form or by any means without the express written consent of Intel   */
/*  Corporation.                                                        */
/*                                                                      */
/* Title: Comm. Power Manager Header File                               */
/*                                                                      */
/* Filename: commpm.h                                                   */
/*                                                                      */
/* Author:   Raviv Levi                                                 */
/*                                                                      */
/* Project, Target, subsystem: Tavor, Arbel, HOP     					*/
/*																		*/
/* Remarks: -                                                           */
/*    													                */
/* Created: 19/9/2005                                                   */
/*                                                                      */
/* Modified:                                                            */
/************************************************************************/


#ifndef __CP_COMMPM_H_
#define __CP_COMMPM_H_
#include "plat_basic_api.h"

typedef int (*MMIC1PrepareFunc)(void);
typedef int (*MMIC1RecoverFunc)(void);

typedef int (*MMID2PrepareFunc)(void);
typedef int (*MMID2RecoverFunc)(BOOL ExitFromD2);

void mmiD2CallbackRegister(UINT32 prepare_func, UINT32 recover_func);
void mmiC1CallbackRegister(UINT32 prepare_func, UINT32 recover_func);
void mmiStatusCallbackRegister(UINT32 statusFunc);

int MMID2Prepare(void);
int MMID2Recover(BOOL ExitFromD2);
int MMIC1Prepare(void);
int MMIC1Recover(void);
int MMIStatus(void);

#endif //_COMMPM_H_

