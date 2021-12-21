#ifndef _REG_H_
#define _REG_H_

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* low level macros for accessing memory mapped hardware registers */
#define REG64(addr) ((volatile uint64_t *)(addr))
#define REG32(addr) ((volatile uint32_t *)(addr))
#define REG16(addr) ((volatile uint16_t *)(addr))
#define REG8(addr) ((volatile uint8_t *)(addr))

#define RMWREG64(addr, startbit, width, val) *REG64(addr) = (*REG64(addr) & ~(((1 << (width)) - 1) << (startbit))) | ((val) << (startbit))
#define RMWREG32(addr, startbit, width, val) *REG32(addr) = (*REG32(addr) & ~(((1 << (width)) - 1) << (startbit))) | ((val) << (startbit))
#define RMWREG16(addr, startbit, width, val) *REG16(addr) = (*REG16(addr) & ~(((1 << (width)) - 1) << (startbit))) | ((val) << (startbit))
#define RMWREG8(addr, startbit, width, val) *REG8(addr) = (*REG8(addr) & ~(((1 << (width)) - 1) << (startbit))) | ((val) << (startbit))

#define writel(v, a) (*REG32(a) = (v))
#define readl(a) (*REG32(a))

#define writeb(v, a) (*REG8(a) = (v))
#define readb(a) (*REG8(a))

#define writehw(v, a) (*REG16(a) = (v))
#define readhw(a) (*REG16(a))

#define writew(v, a) (*REG16(a) = (v))
#define readw(a) (*REG16(a))

#ifdef __cplusplus
extern "C" {
#endif

#endif /* _REG_H_ */

