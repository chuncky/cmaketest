#ifndef __MALLOC_H
#define __MALLOC_H
//////////////////////////////////////////////////////////////////////////////////  
//********************************************************************************
//û�и�����Ϣ
//////////////////////////////////////////////////////////////////////////////////   
/*typedef unsigned int  unsigned int;
typedef unsigned short unsigned short;
typedef unsigned char  unsigned char;*/   

//�û����ú���
void ff_dump_free(void *ptr);       //�ڴ��ͷ�(�ⲿ����)
void *ff_dump_malloc(unsigned int size);     //�ڴ����(�ⲿ����)
//void *myrealloc(void *ptr,unsigned int size);  //���·����ڴ�(�ⲿ����)
void ff_dump_memset(void *s,unsigned char c,unsigned int count);  //�����ڴ�
void ff_dump_memcpy(void *des,void *src,unsigned int n);//�����ڴ�
   
#endif
