/* 
����һ���򵥵��ڴ�����ʵ�֣� ������dump״̬ʱ�ļ�ϵͳ���ڴ����
˼·����:
���ڴ�ֿ����.�ڴ�صȷ�Ϊ�̶���С���ڴ��.
����һ���ڴ�״̬��,��Ӧÿ����,�ж��ٸ���,״̬����ж��ٸ�Ԫ��,һһ��Ӧ.
ͨ��״̬���ֵ�жϸÿ��ڴ��Ƿ����(Ϊ0���ʾ����,Ϊ����ֵ���ʾ��ռ����,����ռ�õ��ڴ������,���Ǹ�ֵ������)

��ʼ����ʱ��,״̬���ֵȫ0���������е��ڴ�鶼δ��ռ��.
����Ҫ�����ʱ��,malloc���ڴ�����ߵ�ַ���²���,���ҵ������Ŀ��ڴ���ڵ���Ҫ������ڴ��ʱ��,�����˴η���,
���ص�ַ��Ҫ�����ָ��,���һ��malloc. free��ʱ��,�ͱȽϼ���,ֻҪ�ҵ���������ڴ��Ӧ��״̬���λ��,Ȼ���״̬���ֵ��0����ʵ��free.

�ڴ�ʹ������ͨ����ѯ״̬���ж��ٸ���0ֵ,������ռ����.
*/
#include "ff_dump_mem.h"   
//////////////////////////////////////////////////////////////////////////////////

#ifndef NULL
#define NULL 0
#endif
#define MEM_BLOCK_SIZE   512          //�ڴ���СΪ32�ֽ�
#define MAX_MEM_SIZE   64*1024       //�������ڴ� 10K
#define MEM_ALLOC_TABLE_SIZE MAX_MEM_SIZE/MEM_BLOCK_SIZE //�ڴ���С
//�ڴ���������
struct ff_dump_m_mallco_dev
{
    void (*init)(void);     //��ʼ��
    unsigned char (*perused)(void);         //�ڴ�ʹ����
    unsigned char  membase[MAX_MEM_SIZE];   //�ڴ��
    unsigned short memmap[MEM_ALLOC_TABLE_SIZE];  //�ڴ����״̬��
    unsigned char  memrdy;        //�ڴ�����Ƿ����
};
//extern struct ff_dump_m_mallco_dev mallco_dev;  //��mallco.c���涨��
  
static void ff_dump_mem_init(void);      //�ڴ�����ʼ������(��/�ڲ�����)
static unsigned int ff_dump_mem_malloc(unsigned int size);     //�ڴ����(�ڲ�����)
static unsigned char ff_dump_mem_free(unsigned int offset);     //�ڴ��ͷ�(�ڲ�����)
static unsigned char ff_dump_mem_perused(void);      //����ڴ�ʹ����(��/�ڲ�����)
////////////////////////////////////////////////////////////////////////////////
   
//�ڴ���������
static struct ff_dump_m_mallco_dev mallco_dev=
{
    ff_dump_mem_init, //�ڴ��ʼ��
    ff_dump_mem_perused,//�ڴ�ʹ����
    0,   //�ڴ��
    0,   //�ڴ����״̬��
    0,     //�ڴ����δ����
};
//�����ڴ�
//*des:Ŀ�ĵ�ַ
//*src:Դ��ַ
//n:��Ҫ���Ƶ��ڴ泤��(�ֽ�Ϊ��λ)
static void ff_dump_memcpy(void *des,void *src,unsigned int n)  
{  
    unsigned char *xdes=des;
    unsigned char *xsrc=src;
    while(n--)*xdes++=*xsrc++;  
}  
//�����ڴ�
//*s:�ڴ��׵�ַ
//c :Ҫ���õ�ֵ
//count:��Ҫ���õ��ڴ��С(�ֽ�Ϊ��λ)
static void ff_dump_memset(void *s,unsigned char c,unsigned int count)  
{  
    unsigned char *xs = s;  
    while(count--)*xs++=c;  
}   
//�ڴ�����ʼ��  
static void ff_dump_mem_init(void)  
{  
    ff_dump_memset(mallco_dev.membase, 0, sizeof(mallco_dev.membase));//�ڴ��������������  
    mallco_dev.memrdy=1;//�ڴ�����ʼ��OK  
}  
//��ȡ�ڴ�ʹ����
//����ֵ:ʹ����(0~100)
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
//�ڴ����(�ڲ�����)
//size:Ҫ������ڴ��С(�ֽ�)
//����ֵ:0XFFFFFFFF,�������;����,�ڴ�ƫ�Ƶ�ַ
static unsigned int ff_dump_mem_malloc(unsigned int size)  
{  
    signed long offset=0;  
    unsigned short nmemb; //��Ҫ���ڴ����  
    unsigned short cmemb=0;//�������ڴ����
    unsigned int i;  
    if(!mallco_dev.memrdy)mallco_dev.init();//δ��ʼ��,��ִ�г�ʼ��
    if(size==0)return 0XFFFFFFFF;//����Ҫ����
    nmemb=size/MEM_BLOCK_SIZE;   //��ȡ��Ҫ����������ڴ����
    if(size%MEM_BLOCK_SIZE)nmemb++;  
    for(offset=MEM_ALLOC_TABLE_SIZE-1;offset>=0;offset--)//���������ڴ������  
    {     
         if(!mallco_dev.memmap[offset])cmemb++; //�������ڴ��������
         else cmemb=0;       //�����ڴ������
         if(cmemb==nmemb)      //�ҵ�������nmemb�����ڴ��
         {
            for(i=0;i<nmemb;i++)      //��ע�ڴ��ǿ�
            {  
                mallco_dev.memmap[offset+i]=nmemb;  
            }  
            return (offset*MEM_BLOCK_SIZE);//����ƫ�Ƶ�ַ  
         }
    }  
    return 0XFFFFFFFF;//δ�ҵ����Ϸ����������ڴ��  
}  
//�ͷ��ڴ�(�ڲ�����)
//offset:�ڴ��ַƫ��
//����ֵ:0,�ͷųɹ�;1,�ͷ�ʧ��;  
static unsigned char ff_dump_mem_free(unsigned int offset)  
{  
    int i;  
    if(!mallco_dev.memrdy)//δ��ʼ��,��ִ�г�ʼ��
    {
        mallco_dev.init();   
        return 1;//δ��ʼ��  
    }  
    if(offset<MAX_MEM_SIZE)//ƫ�����ڴ����.
    {  
        int index=offset/MEM_BLOCK_SIZE;//ƫ�������ڴ�����  
        int nmemb=mallco_dev.memmap[index];   //�ڴ������
        for(i=0;i<nmemb;i++)     //�ڴ������
        {  
            mallco_dev.memmap[index+i]=0;  
        }  
        return 0;  
    }else return 2;//ƫ�Ƴ�����.  
}  
//�ͷ��ڴ�(�ⲿ����)
//ptr:�ڴ��׵�ַ
void ff_dump_free(void *ptr)  
{  
    unsigned int offset;  
    if(ptr==NULL)return;//��ַΪ0.  
    offset=(unsigned int)ptr-(unsigned int)&mallco_dev.membase;  
    ff_dump_mem_free(offset);//�ͷ��ڴ�  
    //fatal_printf("ff_dump_free :%x\n", ptr);
}  
//�����ڴ�(�ⲿ����)
//size:�ڴ��С(�ֽ�)
//����ֵ:���䵽���ڴ��׵�ַ.
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
//���·����ڴ�(�ⲿ����)
//*ptr:���ڴ��׵�ַ
//size:Ҫ������ڴ��С(�ֽ�)
//����ֵ:�·��䵽���ڴ��׵�ַ.
void *myrealloc(void *ptr,unsigned int size)  
{  
    unsigned int offset;  
    offset=mem_malloc(size);  
    if(offset==0XFFFFFFFF)return NULL;     
    else  
    {  
        memcpy((void*)((unsigned int)&mallco_dev.membase+offset),ptr,size);//�������ڴ����ݵ����ڴ�   
        myfree(ptr);               //�ͷž��ڴ�
        return (void*)((unsigned int)&mallco_dev.membase+offset);          //�������ڴ��׵�ַ
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
   Stm32_Clock_Init(9);//ϵͳʱ������
delay_init(72);  //��ʱ��ʼ��
uart_init(72,9600); //����1��ʼ��  
LED_Init();
  //LCD_Init();   
ptr=(unsigned char*)mymalloc(100);
if(*ptr)i=0;
i=mallco_dev.perused();//�鿴ʹ����
ptr1=(unsigned short*)mymalloc(2*100);
i=mallco_dev.perused();//�鿴ʹ����
ptr2=(unsigned int*)mymalloc(4*100);
i=mallco_dev.perused();//�鿴ʹ����
myfree(ptr);
i=mallco_dev.perused();//�鿴ʹ����
ptr3=(unsigned int*)mymalloc(4*20);
i=mallco_dev.perused();//�鿴ʹ����
myfree(ptr1);
i=mallco_dev.perused();//�鿴ʹ����
ptr=(unsigned char*)mymalloc(8*32);

myfree(ptr2);
i=mallco_dev.perused();//�鿴ʹ����
myfree(ptr3);
i=mallco_dev.perused();//�鿴ʹ����
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