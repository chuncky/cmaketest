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
 
#ifndef __RS_UA_FS_H__
#define __RS_UA_FS_H__

#include "rsplatform.h"

#define RS_FS_MAX_PATH          256

typedef int RS_FILE;

#define RS_FS_INVALID           ((RS_FILE)-1)

typedef enum
{
	RS_FS_OPEN_CREATE,      // 创建文件
	RS_FS_OPEN_READ,        // 以读的方式打开
	RS_FS_OPEN_WRITE,		// 以写的方式打开                  
} rs_fs_openMode;

typedef enum 
{
	RS_FS_SEEK_SET,   
	RS_FS_SEEK_CUR,    
	RS_FS_SEEK_END,  
} rs_fs_seekMode;

/*
函数说明：打开文件
参数说明：[IN]fileName  文件的路径
		      [IN]openMode  打开的模式，参见rs_fs_openMode
返回值：成功返回文件句柄，失败返回RS_FS_INVALID
*/
RS_FILE rs_ua_fs_open(const rs_s8* fileName, rs_fs_openMode openMode);

/*
函数说明：关闭文件
参数说明：[IN]fh  文件打开后的句柄
返回值：成功返回rs_true，失败为rs_false
*/
rs_bool rs_ua_fs_close(RS_FILE fh);

/*
函数说明：把数据读取到缓冲区内
参数说明：[IN]fh  文件打开后的句柄
		      [IN OUT]buffer  需要读入的数据的缓冲区
		      [IN]size  需要写入的数据的长度
返回值：成功读取文件数据的长度，失败返回-1
*/
rs_s32 rs_ua_fs_read(RS_FILE fh, void* buffer, rs_u32 size);

/*
函数说明：把数据写入到文件中
参数说明：[IN]fh  文件打开后的句柄
		      [IN]buffer  需要写入的数据
		      [IN]size  需要写入的数据的长度
返回值：成功为写入文件的长度，失败返回-1
*/
rs_s32 rs_ua_fs_write(RS_FILE fh, const void* buffer, rs_u32 size);

/*
函数说明：设置文件指针的位置
参数说明：[IN]fh  文件打开后的句柄
		      [IN]offset  相对于origin的偏移量
		      [in]origin  文件头 尾 当前位置,参见rs_fs_seekMode
返回值：返回rs_true，失败返回rs_false
*/
rs_bool rs_ua_fs_seek(RS_FILE fh, rs_s32 offset, rs_s32 origin);

/*
函数说明：获取文件的大小
参数说明：[IN]fh  文件打开后的句柄
返回值：文件的大小
*/
rs_u32 rs_ua_fs_size(RS_FILE fh);

/*
函数说明：判断文件系统中的文件是否存在
参数说明：[IN]fileName  文件的路径
返回值：存在为rs_true，不存在为rs_false
*/
rs_bool rs_ua_fs_exists(const rs_s8* fileName);

/*
函数说明：删除文件系统中的文件
参数说明：[IN]fileName  文件的路径
返回值：成功为rs_true，失败为rs_false。
*/
rs_bool rs_ua_fs_remove(const rs_s8* fileName);

#endif
