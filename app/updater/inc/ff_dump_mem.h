#ifndef __MALLOC_H
#define __MALLOC_H
//////////////////////////////////////////////////////////////////////////////////  
//********************************************************************************
//没有更新信息
//////////////////////////////////////////////////////////////////////////////////   
/*typedef unsigned int  unsigned int;
typedef unsigned short unsigned short;
typedef unsigned char  unsigned char;*/   

//用户调用函数
void ff_dump_free(void *ptr);       //内存释放(外部调用)
void *ff_dump_malloc(unsigned int size);     //内存分配(外部调用)
//void *myrealloc(void *ptr,unsigned int size);  //重新分配内存(外部调用)
void ff_dump_memset(void *s,unsigned char c,unsigned int count);  //设置内存
void ff_dump_memcpy(void *des,void *src,unsigned int n);//复制内存
   
#endif
