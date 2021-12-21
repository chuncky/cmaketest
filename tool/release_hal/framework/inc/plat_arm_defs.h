#ifndef __PLAT_ARM_DEFS_H__
#define __PLAT_ARM_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#if ((__ARMCC_VERSION / 1000000) >= 6)

#include <arm_compat.h>

extern unsigned int __builtin_return_address_(void);

#define __CC_ARM	1
#define __MODULE__	__FILE__
#define __return_address()	__builtin_return_address_()

#endif

#ifdef __cplusplus
}
#endif

#endif
