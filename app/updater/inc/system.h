#ifndef _SYSTEM_H_
#define _SYSTEM_H_

//#define isb() __asm__ volatile ("isb" : : : "memory")
typedef enum {
	NO_ACCESS     =0x0,
	FULL_ACCESS   =0x3,
	READ_ONLY     =0x6
}access_permission;

//region size
typedef enum {
        REG_4KB		=0x0B,
        REG_8KB		=0x0C,
        REG_16KB	=0x0D,
        REG_32KB	=0x0E,
        REG_64KB	=0x0F,
        REG_128KB	=0x10,
        REG_256KB	=0x11,
        REG_512KB	=0x12,
        REG_1MB		=0x13,
        REG_2MB		=0x14,
        REG_4MB		=0x15,
        REG_8MB		=0x16,
        REG_16MB	=0x17,
        REG_32MB	=0x18,
        REG_64MB	=0x19,
        REG_128MB	=0x1A,
        REG_256MB	=0x1B,
        REG_512MB	=0x1C,
        REG_1GB		=0x1D,
        REG_2GB		=0x1E,
        REG_4GB		=0x1F
}region_size;


#define SCTLR_M    (1 << 0)     /* Enable the MPU */
#define SCTLR_A    (1 << 1)     /* Alignment check enable bit */
#define SCTLR_C    (1 << 2)     /* Cache enable bit */
#define SCTLR_SW   (1 << 10)    /* SWP/SWPB enable bit */
#define SCTLR_Z    (1 << 11)    /* Branch prediction enable bit */
#define SCTLR_I    (1 << 12)    /* Instruction cache enable bit */
#define SCTLR_V    (1 << 13)    /* Vectors relocated to 0xffff0000 */
#define SCTLR_RR   (1 << 14)    /* Not on Cortex-R7 processor. Round Robin bit for cache implementation policy */
#define SCTLR_BR   (1 << 17)    /* Background region bit */
#define SCTLR_DZ   (1 << 19)    /* Divide by zero fault bit */
#define SCTLR_FI   (1 << 21)    /* Fast interrupts configuration enable */
#define SCTLR_U    (1 << 22)    /* Not on Cortex-R7 processor. Indicates use of the alignment model */
#define SCTLR_EE   (1 << 25)    /* Exception endianness */
#define SCTLR_NMFI (1 << 27)    /* Non-maskable FIQ(NMFI) support */
#define SCTLR_TE   (1 << 30)    /* Thumb exception enable */
#define SCTLR_IE   (1 << 31)    /* Instruction Endianness */
#if 1
extern unsigned int sctlr_get(void);
extern void sctlr_set(unsigned int val);
#else
static /*inline*/ unsigned int sctlr_get(void)
{
    unsigned int val;

    __asm__ __volatile__ ("mrc p15, 0, %0, c1, c0, 0" : "=r" (val) : : "memory", "cc");

    return val;
}

static /*inline*/ void sctlr_set(unsigned int val)
{
    __asm__ __volatile__ ("mcr p15, 0, %0, c1, c0, 0" : : "r" (val) : "memory", "cc");
    isb();
}
#endif
/* Region num 0 has the lowest priority */
#define MAX_REGION_NUM 16

#define MPU_REGION_ACCESS_CTRL_B      (1 << 0)              /* Bufferable */
#define MPU_REGION_ACCESS_CTRL_C      (1 << 1)              /* Cacheable */
#define MPU_REGION_ACCESS_CTRL_S      (1 << 2)              /* Shared */
#define MPU_REGION_ACCESS_CTRL_TEX(x) (((x) & 0x7) << 3)    /* Type Extensions */
#define MPU_REGION_ACCESS_CTRL_AP(x)  (((x) & 0x7) << 8)    /* Access Permissions */
#define MPU_REGION_ACCESS_CTRL_XN     (1 << 12)             /* Execute Never */

#define MPU_REGION_SIZE_MASK          (0x1f << 1)           /* Region size mask */
#define MPU_REGION_ENABLE             (1 << 0)              /* Region Enable */

#if 1
extern unsigned int mpu_get_region_num(void);
extern void mpu_set_region_num(unsigned int val);
extern unsigned int mpu_get_region_base_addr(void);
extern void mpu_set_region_base_addr(unsigned int val);
extern unsigned int mpu_get_region_size(void);
extern void mpu_set_region_size(unsigned int size);
extern unsigned int mpu_get_region_size_enable(void);
extern unsigned int mpu_get_region_access_ctrl(void);
extern void mpu_set_region_access_ctrl(unsigned int acc_ctrl);
extern void mpu_enable_region(unsigned int num, unsigned int enable);

#else
static /*inline*/ unsigned int mpu_get_region_num(void)
{
    unsigned int val;

    __asm__ __volatile__ ("mrc p15, 0, %0, c6, c2, 0" : "=r" (val) : : "memory", "cc");

    return val;
}

static /*inline*/ void mpu_set_region_num(unsigned int val)
{
    __asm__ __volatile__ ("mcr p15, 0, %0, c6, c2, 0" : : "r" (val) : "memory", "cc");
    isb();
}

static /*inline*/ unsigned int mpu_get_region_base_addr(void)
{
    unsigned int val;

    __asm__ __volatile__ ("mrc p15, 0, %0, c6, c1, 0" : "=r" (val) : : "memory", "cc");

    return val;
}

static /*inline*/ void mpu_set_region_base_addr(unsigned int val)
{
    __asm__ __volatile__ ("mcr p15, 0, %0, c6, c1, 0" : : "r" (val) : "cc");
    isb();
}

static /*inline*/ unsigned int mpu_get_region_size(void)
{
    unsigned int val;

    __asm__ __volatile__ ("mrc p15, 0, %0, c6, c1, 2" : "=r" (val) : : "memory", "cc");

    return (val & MPU_REGION_SIZE_MASK) >> 1;
}

static /*inline*/ void mpu_set_region_size(unsigned int size)
{
    unsigned val;

    __asm__ __volatile__ ("mrc p15, 0, %0, c6, c1, 2" : "=r" (val) : : "memory", "cc");
    val = (val & ~MPU_REGION_SIZE_MASK) | (size << 1);
    __asm__ __volatile__ ("mcr p15, 0, %0, c6, c1, 2" : : "r" (val) : "cc");
    isb();
}

static /*inline*/ unsigned int mpu_get_region_size_enable(void)
{
    unsigned int val;

    __asm__ __volatile__ ("mrc p15, 0, %0, c6, c1, 2" : "=r" (val) : : "memory", "cc");

    return val;
}

static /*inline*/ unsigned int mpu_get_region_access_ctrl(void)
{
    unsigned int acc_ctrl;

    __asm__ __volatile__ ("mrc p15, 0, %0, c6, c1, 4" : "=r" (acc_ctrl) : : "memory", "cc");

    return acc_ctrl;
}

static /*inline*/ void mpu_set_region_access_ctrl(unsigned int acc_ctrl)
{
    __asm__ __volatile__ ("mcr p15, 0, %0, c6, c1, 4" : : "r" (acc_ctrl) : "cc");
    isb();
}

static /*inline*/ void mpu_enable_region(unsigned int num, unsigned int enable)
{
    unsigned val;

    mpu_set_region_num(num);

    __asm__ __volatile__ ("mrc p15, 0, %0, c6, c1, 2" : "=r" (val) : : "memory", "cc");
    if (enable) {
        val |= MPU_REGION_ENABLE;
    }
    else {
        val &= ~MPU_REGION_ENABLE;
    }
    __asm__ __volatile__ ("mcr p15, 0, %0, c6, c1, 2" : : "r" (val) : "cc");
    isb();
}
#endif
extern int mpu_set_region(unsigned int num, unsigned int base, unsigned int size, unsigned int acc_ctrl);

#endif /* _SYSTEM_H_ */
