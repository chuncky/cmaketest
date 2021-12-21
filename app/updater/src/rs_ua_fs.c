/*
 *
 * ��Ȩ(c) 2015 ��ʯ���⣨�������Ƽ����޹�˾
 * ��Ȩ(c) 2011-2015 ��ʯ���⣨�������Ƽ����޹�˾��Ȩ����
 * 
 * ���ĵ�������Ϣ��˽�ܵ���Ϊ��ʯ����˽�У������񷨺��̷��ж���Ϊ��ҵ������Ϣ��
 * �κ���ʿ�������Ը�ӡ��ɨ����������κη�ʽ���д����������ʯ������Ȩ׷���������Ρ�
 * �Ķ���ʹ�ñ����ϱ�������Ӧ��������Ȩ���е��������κͽ�����Ӧ�ķ���Լ����
 *
 */



#include "rs_ua_fs.h"
#include "rs_ua_porting.h"
#include "rsplatform.h"

extern rs_bool rs_trace(const rs_s8 *format,...);

/*
����˵�������ļ�
����˵����[IN]fileName  �ļ���·��
		      [IN]openMode  �򿪵�ģʽ���μ�rs_fs_openMode
����ֵ���ɹ������ļ������ʧ�ܷ���RS_FS_INVALID
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
����˵�����ر��ļ�
����˵����[IN]fh  �ļ��򿪺�ľ��
����ֵ���ɹ�����rs_true��ʧ��Ϊrs_false
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
����˵���������ݶ�ȡ����������
����˵����[IN]fh  �ļ��򿪺�ľ��
		      [IN OUT]buffer  ��Ҫ��������ݵĻ�����
		      [IN]size  ��Ҫд������ݵĳ���
����ֵ���ɹ���ȡ�ļ����ݵĳ��ȣ�ʧ�ܷ���-1
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
����˵����������д�뵽�ļ���
����˵����[IN]fh  �ļ��򿪺�ľ��
		      [IN]buffer  ��Ҫд�������
		      [IN]size  ��Ҫд������ݵĳ���
����ֵ���ɹ�Ϊд���ļ��ĳ��ȣ�ʧ�ܷ���-1
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
����˵���������ļ�ָ���λ��
����˵����[IN]fh  �ļ��򿪺�ľ��
		      [IN]offset  �����origin��ƫ����
		      [in]origin  �ļ�ͷ β ��ǰλ��,�μ�rs_ua_fs_seekMode
����ֵ������rs_true��ʧ�ܷ���rs_false
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
����˵������ȡ�ļ��Ĵ�С
����˵����[IN]fh  �ļ��򿪺�ľ��
����ֵ���ļ��Ĵ�С
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
����˵�����ж��ļ�ϵͳ�е��ļ��Ƿ����
����˵����[IN]fileName  �ļ���·��
����ֵ������Ϊrs_true��������Ϊrs_false
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
����˵����ɾ���ļ�ϵͳ�е��ļ�
����˵����[IN]fileName  �ļ���·��
����ֵ���ɹ�Ϊrs_true��ʧ��Ϊrs_false��
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


