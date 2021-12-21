/*
 *
 * ��Ȩ(c) 2017 ��ʯ���⣨�������Ƽ����޹�˾
 * ��Ȩ(c) 2011-2017 ��ʯ���⣨�������Ƽ����޹�˾��Ȩ����
 * 
 * ���ĵ�������Ϣ��˽�ܵ���Ϊ��ʯ����˽�У������񷨺��̷��ж���Ϊ��ҵ������Ϣ��
 * �κ���ʿ�������Ը�ӡ��ɨ����������κη�ʽ���д����������ʯ������Ȩ׷���������Ρ�
 * �Ķ���ʹ�ñ����ϱ�������Ӧ��������Ȩ���е��������κͽ�����Ӧ�ķ���Լ����
 *
 */
  
#ifndef __RS_UA_FLASH_H__
#define __RS_UA_FLASH_H__
#include "rsplatform.h"

/**
* ����˵����flash��ʼ��
* ����˵������
* ����ֵ���ɹ�����rs_true��ʧ��Ϊrs_false
*/
rs_bool rs_ua_flash_init(void);

/**
* ����˵������ȡflash�е����ݵ��ڴ滺������
* ����˵����[IN OUT]destination ���ն�ȡflash�����ݵĻ�����
*           [IN]address flash�ĵ�ַ
*					  [IN]size ��Ҫ��ȡ�ĳ���
*
* ����ֵ���ɹ�����rs_true, ʧ�ܷ���rs_false
*/
rs_bool rs_ua_flash_read(rs_u8 *destination,
                       volatile rs_u8 *address,
                       rs_u32 size);

/**
* ����˵�������ڴ滺��������д�뵽flash��
* ����˵����[IN]address flash�ĵ�ַ
*           [IN]source ��Ҫд������ݻ�����
*					  [IN]size  ���ݳ���
*
* ����ֵ���ɹ�����rs_true, ʧ�ܷ���rs_false
*
* ��ע��address���������ж���block����ģ�size����block�ĳ��ȣ����ÿ��ǲ���������������Ӧ�ò����ع��������ֱ����
				��flash����д��ʱ�򣬶���page����ģ�size��һ��page�Ĵ�С��
*/
rs_bool rs_ua_flash_write(volatile rs_u8 *address,
                       rs_u8 *source,
                       rs_u32 size);

/**
* ����˵������block����flash
* ����˵����[IN]address flash�ĵ�ַ
*					  [IN]size Ҫ���������ݵĳ���
*
* ����ֵ���ɹ�����rs_true, ʧ�ܷ���rs_false
*
* ��ע�����������������л���Ӧ�ò㣬address����block����ģ�size����block�ĳ��ȡ�
*/
rs_bool rs_ua_flash_erase(volatile rs_u8 *address,
                        rs_u32 size);

/**
* ����˵�����ɱ�����������־�ӿ�
* ����˵����[IN]format ��ʽ���ַ���
*	
*
* ����ֵ���ɹ�����rs_true, ʧ�ܷ���rs_false
*
* ��ע��
*/
rs_bool rs_trace(const rs_s8 *format,...);

#endif

