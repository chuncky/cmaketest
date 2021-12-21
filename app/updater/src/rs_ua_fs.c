/*
 *
 * 版权(c) 2015 红石阳光（北京）科技有限公司
 * 版权(c) 2011-2015 红石阳光（北京）科技有限公司版权所有
 * 
 * 本文档包含信息是私密的且为红石阳光私有，且在民法和刑法中定义为商业秘密信息。
 * 任何人士不得擅自复印，扫描或以其他任何方式进行传播，否则红石阳光有权追究法律责任。
 * 阅读和使用本资料必须获得相应的书面授权，承担保密责任和接受相应的法律约束。
 *
 */



#include "rs_ua_fs.h"
#include "rs_ua_porting.h"
#include "rsplatform.h"

extern rs_bool rs_trace(const rs_s8 *format,...);

/*
函数说明：打开文件
参数说明：[IN]fileName  文件的路径
		      [IN]openMode  打开的模式，参见rs_fs_openMode
返回值：成功返回文件句柄，失败返回RS_FS_INVALID
*/
RS_FILE rs_ua_fs_open(const rs_s8* fileName, rs_fs_openMode openMode)
{
#if !defined(RS_DIFF_PACKAGE_ON_FLASH)
	return FS_Open((WCHAR*)fileName, openMode);
#else
	return -1;
#endif
}

/*
函数说明：关闭文件
参数说明：[IN]fh  文件打开后的句柄
返回值：成功返回rs_true，失败为rs_false
*/
rs_bool rs_ua_fs_close(RS_FILE fh)
{
#if !defined(RS_DIFF_PACKAGE_ON_FLASH)
		FS_Close(fh);
#else
		return rs_false;
#endif
}

/*
函数说明：把数据读取到缓冲区内
参数说明：[IN]fh  文件打开后的句柄
		      [IN OUT]buffer  需要读入的数据的缓冲区
		      [IN]size  需要写入的数据的长度
返回值：成功读取文件数据的长度，失败返回-1
*/
rs_s32 rs_ua_fs_read(RS_FILE fh, void* buffer, rs_u32 size)
{
#if !defined(RS_DIFF_PACKAGE_ON_FLASH)
	rs_s32	status;
	rs_u32 read = 0;

	status = FS_Read(fh, buffer, size, &read);
	//Check the result of FS API, and convert it to BL error code
	if(status >= 0)
	{
		return read;
	}
	else
	{
		rs_trace("%s failed!\n\r", __func__);
		return -1;
   }
#else
	return -1;
#endif
}

/*
函数说明：把数据写入到文件中
参数说明：[IN]fh  文件打开后的句柄
		      [IN]buffer  需要写入的数据
		      [IN]size  需要写入的数据的长度
返回值：成功为写入文件的长度，失败返回-1
*/
rs_s32 rs_ua_fs_write(RS_FILE fh, const void* buffer, rs_u32 size)
{
#if !defined(RS_DIFF_PACKAGE_ON_FLASH)
	int fs_status;
	rs_u32 written = 0;
	fs_status = FS_Write(fh, (void*)buffer, size, &written);
	if (fs_status != FS_NO_ERROR || written != size) {
		rs_trace("%s, write error: status[%x], size[%d], written[%d]\n\r", __func__, fs_status, size, written);
	}
	return written;
#else
	return -1;
#endif
}

/*
函数说明：设置文件指针的位置
参数说明：[IN]fh  文件打开后的句柄
		      [IN]offset  相对于origin的偏移量
		      [in]origin  文件头 尾 当前位置,参见rs_ua_fs_seekMode
返回值：返回rs_true，失败返回rs_false
*/
rs_bool rs_ua_fs_seek(RS_FILE fh, rs_s32 offset, rs_s32 origin)
{
#if !defined(RS_DIFF_PACKAGE_ON_FLASH)
	rs_u32 offset_p;
	ASSERT(origin == 0);
	offset_p =	FS_Seek(fh, offset, 0);
	return rs_true;
#else
	return rs_false;
#endif
}

/*
函数说明：获取文件的大小
参数说明：[IN]fh  文件打开后的句柄
返回值：文件的大小
*/
rs_u32 rs_ua_fs_size(RS_FILE fh)
{ 
#if !defined(RS_DIFF_PACKAGE_ON_FLASH)
	rs_s32 filesize = 0;
	FS_GetFileSize(fh,&filesize);
	return filesize;
#else
	return -1;
#endif
}

/*
函数说明：判断文件系统中的文件是否存在
参数说明：[IN]fileName  文件的路径
返回值：存在为rs_true，不存在为rs_false
*/
rs_bool rs_ua_fs_exists(const rs_s8* fileName)
{
#if !defined(RS_DIFF_PACKAGE_ON_FLASH)
	RS_FILE f;
	f = rs_ua_fs_open(fileName , RS_FS_OPEN_READ);
	if (f != RS_FS_INVALID)
	{
		// it existes
		rs_ua_fs_close(f);
		rs_trace("%s, file[%s] is exist\n\r", __func__, fileName);
		return 1;
	}
	else
	{
		// not exist
		rs_trace("%s, file[%s] is not exist\n\r", __func__, fileName);
		return 0;
	}
#else
	return -1;
#endif
}


/*
函数说明：删除文件系统中的文件
参数说明：[IN]fileName  文件的路径
返回值：成功为rs_true，失败为rs_false。
*/
rs_bool rs_ua_fs_remove(const rs_s8* fileName)
{
#if !defined(RS_DIFF_PACKAGE_ON_FLASH)
	int fs_status;

	fs_status = FS_Delete((WCHAR*)fileName);
	if (fs_status < FS_NO_ERROR) {
		rs_trace("%s, remove error: status[%x]\n\r", __func__, fs_status);
		return 0;
	}
	return 1;
#else
	return 0;
#endif
}


