/*
 *
 * 版权(c) 2017 红石阳光（北京）科技有限公司
 * 版权(c) 2011-2017 红石阳光（北京）科技有限公司版权所有
 * 
 * 本文档包含信息是私密的且为红石阳光私有，且在民法和刑法中定义为商业秘密信息。
 * 任何人士不得擅自复印，扫描或以其他任何方式进行传播，否则红石阳光有权追究法律责任。
 * 阅读和使用本资料必须获得相应的书面授权，承担保密责任和接受相应的法律约束。
 *
 */
  
#ifndef __RS_UA_FLASH_H__
#define __RS_UA_FLASH_H__
#include "rsplatform.h"

/**
* 函数说明：flash初始化
* 参数说明：无
* 返回值：成功返回rs_true，失败为rs_false
*/
rs_bool rs_ua_flash_init(void);

/**
* 函数说明：读取flash中的数据到内存缓冲区中
* 参数说明：[IN OUT]destination 接收读取flash中数据的缓冲区
*           [IN]address flash的地址
*					  [IN]size 需要读取的长度
*
* 返回值：成功返回rs_true, 失败返回rs_false
*/
rs_bool rs_ua_flash_read(rs_u8 *destination,
                       volatile rs_u8 *address,
                       rs_u32 size);

/**
* 函数说明：把内存缓冲区数据写入到flash中
* 参数说明：[IN]address flash的地址
*           [IN]source 需要写入的数据缓冲区
*					  [IN]size  数据长度
*
* 返回值：成功返回rs_true, 失败返回rs_false
*
* 备注：address升级过程中都是block对齐的，size都是block的长度，不用考虑不对齐的情况。而在应用层下载过程中如果直接往
				往flash里面写的时候，都是page对齐的，size是一个page的大小。
*/
rs_bool rs_ua_flash_write(volatile rs_u8 *address,
                       rs_u8 *source,
                       rs_u32 size);

/**
* 函数说明：按block擦除flash
* 参数说明：[IN]address flash的地址
*					  [IN]size 要擦出的数据的长度
*
* 返回值：成功返回rs_true, 失败返回rs_false
*
* 备注：不管在升级过程中或者应用层，address都是block对齐的，size都是block的长度。
*/
rs_bool rs_ua_flash_erase(volatile rs_u8 *address,
                        rs_u32 size);

/**
* 函数说明：可变参数的输出日志接口
* 参数说明：[IN]format 格式化字符串
*	
*
* 返回值：成功返回rs_true, 失败返回rs_false
*
* 备注：
*/
rs_bool rs_trace(const rs_s8 *format,...);

#endif

