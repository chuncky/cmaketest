/* 
这是一个简单的内存管理的实现， 用来在dump状态时文件系统的内存分配
思路如下:
将内存分块管理.内存池等分为固定大小的内存块.
建立一个内存状态表,对应每个块,有多少个块,状态表就有多少个元素,一一对应.
通过状态表的值判断该块内存是否可用(为0则表示可用,为其他值则表示被占用了,而且占用的内存块数量,就是该值的数字)

初始化的时候,状态表的值全0，代表所有的内存块都未被占用.
当需要分配的时候,malloc从内存块的最高地址往下查找,查找到连续的空内存大于等于要分配的内存的时候,结束此次分配,
返回地址给要分配的指针,完成一次malloc. free的时候,就比较简单了,只要找到所分配的内存对应在状态表的位置,然后把状态表的值清0，及实现free.

内存使用率则通过查询状态表有多少个非0值,来计算占用率.
*/
#include "ff_dump_mem.h"   
//////////////////////////////////////////////////////////////////////////////////

#ifndef NULL
#define NULL 0
#endif
#define MEM_BLOCK_SIZE   512          //内存块大小为32字节
#define MAX_MEM_SIZE   64*1024       //最大管理内存 10K
#define MEM_ALLOC_TABLE_SIZE MAX_MEM_SIZE/MEM_BLOCK_SIZE //内存表大小
//内存管理控制器
struct ff_dump_m_mallco_dev
{
    void (*init)(void);     //初始化
    unsigned char (*perused)(void);         //内存使用率
    unsigned char  membase[MAX_MEM_SIZE];   //内存池
    unsigned short memmap[MEM_ALLOC_TABLE_SIZE];  //内存管理状态表
    unsigned char  memrdy;        //内存管理是否就绪
};
//extern struct ff_dump_m_mallco_dev mallco_dev;  //在mallco.c里面定义
  
static void ff_dump_mem_init(void);      //内存管理初始化函数(外/内部调用)
static unsigned int ff_dump_mem_malloc(unsigned int size);     //内存分配(内部调用)
static unsigned char ff_dump_mem_free(unsigned int offset);     //内存释放(内部调用)
static unsigned char ff_dump_mem_perused(void);      //获得内存使用率(外/内部调用)
////////////////////////////////////////////////////////////////////////////////
   
//内存管理控制器
static struct ff_dump_m_mallco_dev mallco_dev=
{
    ff_dump_mem_init, //内存初始化
    ff_dump_mem_perused,//内存使用率
    0,   //内存池
    0,   //内存管理状态表
    0,     //内存管理未就绪
};
//复制内存
//*des:目的地址
//*src:源地址
//n:需要复制的内存长度(字节为单位)
static void ff_dump_memcpy(void *des,void *src,unsigned int n)  
{  
    unsigned char *xdes=des;
    unsigned char *xsrc=src;
    while(n--)*xdes++=*xsrc++;  
}  
//设置内存
//*s:内存首地址
//c :要设置的值
//count:需要设置的内存大小(字节为单位)
static void ff_dump_memset(void *s,unsigned char c,unsigned int count)  
{  
    unsigned char *xs = s;  
    while(count--)*xs++=c;  
}   
//内存管理初始化  
static void ff_dump_mem_init(void)  
{  
    ff_dump_memset(mallco_dev.membase, 0, sizeof(mallco_dev.membase));//内存池素有数据清零  
    mallco_dev.memrdy=1;//内存管理初始化OK  
}  
//获取内存使用率
//返回值:使用率(0~100)
static unsigned char ff_dump_mem_perused(void)  
{  
    unsigned short used=0;  
    unsigned int i;  
    for(i=0;i<MEM_ALLOC_TABLE_SIZE;i++)  
    {  
        if(mallco_dev.memmap)used++;
    }  
    return used*100/MEM_ALLOC_TABLE_SIZE;  
}  
//内存分配(内部调用)
//size:要分配的内存大小(字节)
//返回值:0XFFFFFFFF,代表错误;其他,内存偏移地址
static unsigned int ff_dump_mem_malloc(unsigned int size)  
{  
    signed long offset=0;  
    unsigned short nmemb; //需要的内存块数  
    unsigned short cmemb=0;//连续空内存块数
    unsigned int i;  
    if(!mallco_dev.memrdy)mallco_dev.init();//未初始化,先执行初始化
    if(size==0)return 0XFFFFFFFF;//不需要分配
    nmemb=size/MEM_BLOCK_SIZE;   //获取需要分配的连续内存块数
    if(size%MEM_BLOCK_SIZE)nmemb++;  
    for(offset=MEM_ALLOC_TABLE_SIZE-1;offset>=0;offset--)//搜索整个内存控制区  
    {     
         if(!mallco_dev.memmap[offset])cmemb++; //连续空内存块数增加
         else cmemb=0;       //连续内存块清零
         if(cmemb==nmemb)      //找到了连续nmemb个空内存块
         {
            for(i=0;i<nmemb;i++)      //标注内存块非空
            {  
                mallco_dev.memmap[offset+i]=nmemb;  
            }  
            return (offset*MEM_BLOCK_SIZE);//返回偏移地址  
         }
    }  
    return 0XFFFFFFFF;//未找到符合分配条件的内存块  
}  
//释放内存(内部调用)
//offset:内存地址偏移
//返回值:0,释放成功;1,释放失败;  
static unsigned char ff_dump_mem_free(unsigned int offset)  
{  
    int i;  
    if(!mallco_dev.memrdy)//未初始化,先执行初始化
    {
        mallco_dev.init();   
        return 1;//未初始化  
    }  
    if(offset<MAX_MEM_SIZE)//偏移在内存池内.
    {  
        int index=offset/MEM_BLOCK_SIZE;//偏移所在内存块号码  
        int nmemb=mallco_dev.memmap[index];   //内存块数量
        for(i=0;i<nmemb;i++)     //内存块清零
        {  
            mallco_dev.memmap[index+i]=0;  
        }  
        return 0;  
    }else return 2;//偏移超区了.  
}  
//释放内存(外部调用)
//ptr:内存首地址
void ff_dump_free(void *ptr)  
{  
    unsigned int offset;  
    if(ptr==NULL)return;//地址为0.  
    offset=(unsigned int)ptr-(unsigned int)&mallco_dev.membase;  
    ff_dump_mem_free(offset);//释放内存  
    //fatal_printf("ff_dump_free :%x\n", ptr);
}  
//分配内存(外部调用)
//size:内存大小(字节)
//返回值:分配到的内存首地址.
void *ff_dump_malloc(unsigned int size)  
{  
    unsigned int offset;
	void *ret = 0;
    offset=ff_dump_mem_malloc(size);  
    if(offset==0XFFFFFFFF)return NULL;  
    else {
		 ret = (void*)((unsigned int)&mallco_dev.membase+offset);
		 //fatal_printf("ff_dump_alloclen:%u, addr:%x\n",size, ret);
		 return ret;
    }
}
#if 0  
//重新分配内存(外部调用)
//*ptr:旧内存首地址
//size:要分配的内存大小(字节)
//返回值:新分配到的内存首地址.
void *myrealloc(void *ptr,unsigned int size)  
{  
    unsigned int offset;  
    offset=mem_malloc(size);  
    if(offset==0XFFFFFFFF)return NULL;     
    else  
    {  
        memcpy((void*)((unsigned int)&mallco_dev.membase+offset),ptr,size);//拷贝旧内存内容到新内存   
        myfree(ptr);               //释放旧内存
        return (void*)((unsigned int)&mallco_dev.membase+offset);          //返回新内存首地址
    }  
}
#endif
#if 0
int main(void)
{ 
unsigned char *ptr;
unsigned short *ptr1;
unsigned int *ptr2;
  unsigned int *ptr3;

unsigned char i;
   Stm32_Clock_Init(9);//系统时钟设置
delay_init(72);  //延时初始化
uart_init(72,9600); //串口1初始化  
LED_Init();
  //LCD_Init();   
ptr=(unsigned char*)mymalloc(100);
if(*ptr)i=0;
i=mallco_dev.perused();//查看使用率
ptr1=(unsigned short*)mymalloc(2*100);
i=mallco_dev.perused();//查看使用率
ptr2=(unsigned int*)mymalloc(4*100);
i=mallco_dev.perused();//查看使用率
myfree(ptr);
i=mallco_dev.perused();//查看使用率
ptr3=(unsigned int*)mymalloc(4*20);
i=mallco_dev.perused();//查看使用率
myfree(ptr1);
i=mallco_dev.perused();//查看使用率
ptr=(unsigned char*)mymalloc(8*32);

myfree(ptr2);
i=mallco_dev.perused();//查看使用率
myfree(ptr3);
i=mallco_dev.perused();//查看使用率
if(i)i=0;
usmart_dev.init();
POINT_COLOR=RED;      
   while(1)
{     
  LED0=!LED0;      
  delay_ms(500);
}               
}
#endif