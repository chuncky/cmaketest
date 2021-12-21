/*
 *
 * 版权(c) 2014 红石阳光（北京）科技有限公司
 * 版权(c) 2007-2014 红石阳光（北京）科技有限公司版权所有
 * 
 * 本文档包含信息是私密的且为红石阳光私有，且在民法和刑法中定义为商业秘密信息。
 * 任何人士不得擅自复印，扫描或以其他任何方式进行传播，否则红石阳光有权追究法律责任。
 * 阅读和使用本资料必须获得相应的书面授权，承担保密责任和接受相应的法律约束。
 *
 */

#ifndef __RSPLATFORM_H__
#define __RSPLATFORM_H__


/** Null Definition */
#ifdef NULL
  #define RS_NULL_POINTER       NULL
#else
  #define RS_NULL_POINTER       0x0
#endif

/** Type Definitions */

/** Signed 8-bit */
typedef char               rs_s8;

/** Signed 16-bit */
typedef short             rs_s16;

/** Signed 32-bit */
typedef int               rs_s32;

/** Unsigned 8-bit */
typedef unsigned char      rs_u8;

/** Unsigned 16-bit */
typedef unsigned short    rs_u16;

/** Unsigned 32-bit */
typedef unsigned int      rs_u32;


/** Update Agent Boolean Definition */
typedef int              rs_bool;

/** True Definition */
#define rs_true               0x1

/** False Definition */
#define rs_false              0x0

#define RS_INVALID_ADDRESS      ((rs_u8*)-1)

#endif
