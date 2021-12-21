/*
 *
 * ��Ȩ(c) 2014 ��ʯ���⣨�������Ƽ����޹�˾
 * ��Ȩ(c) 2007-2014 ��ʯ���⣨�������Ƽ����޹�˾��Ȩ����
 * 
 * ���ĵ�������Ϣ��˽�ܵ���Ϊ��ʯ����˽�У������񷨺��̷��ж���Ϊ��ҵ������Ϣ��
 * �κ���ʿ�������Ը�ӡ��ɨ����������κη�ʽ���д����������ʯ������Ȩ׷���������Ρ�
 * �Ķ���ʹ�ñ����ϱ�������Ӧ��������Ȩ���е��������κͽ�����Ӧ�ķ���Լ����
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
