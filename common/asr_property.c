#include <string.h>

#include "asr_property.h"

/**************************************
 *  for the execution image before B33,
 *  those RO properties could be cached into BTCM only
 *  before B33 jump to CP.bin , load all the properties into PSRAM.
 *
 **************************************/
static asr_prop_area *__system_property_area__ = (asr_prop_area *)(ASR_PROP_AREA_BASE);
static asr_prop_info *pa_info_array = 0;
static int asr_property_area_inited = 0;

static const asr_prop_info *__system_property_find(const char *name)
{
    asr_prop_area *pa = __system_property_area__;
    unsigned count = pa->count;
    unsigned *toc = pa->toc;
    unsigned len = strlen(name);
    asr_prop_info *pi;

    while(count--) {
        unsigned entry = *toc++;
        if(TOC_NAME_LEN(entry) != len) continue;
        
        pi = TOC_TO_INFO(pa, entry);
        if(memcmp(name, pi->name, len)) continue;

        return pi;
    }

    return 0;
}

static int __system_property_read(const asr_prop_info *pi, char *name, char *value)
{
    unsigned serial, len;
    
    for(;;) {
        serial = pi->serial;
#if 0//NO MUTEX
        while(SERIAL_DIRTY(serial)) {
            __futex_wait((volatile void *)&pi->serial, serial, 0);
            serial = pi->serial;
        }
#endif
        len = SERIAL_VALUE_LEN(serial);
        memcpy(value, pi->value, len + 1);
        if(serial == pi->serial) {
            if(name != 0) {
                strcpy(name, pi->name);
            }
            return len;
        }
    }
}

static const asr_prop_info *__system_property_find_nth(unsigned n)
{
    asr_prop_area *pa = __system_property_area__;

    if(n >= pa->count) {
        return 0;
    } else {
        return TOC_TO_INFO(pa, pa->toc[n]);
    }
}

static void update_prop_info(asr_prop_info *pi, const char *value, unsigned len)
{
    pi->serial = pi->serial | 1;
    memcpy(pi->value, value, len + 1);
    pi->serial = (len << 24) | ((pi->serial + 1) & 0xffffff);
#if 0//NO MUTEX
    __futex_wake(&pi->serial, INT32_MAX);
#endif
}

/**
 ** COMMON_API
 ** NOTICE: for arg force_init
 ** TRUE  : should be invoke by preboot.
 ** FALSE : should be the other exec bin
 **/
int asr_property_area_init(int force_init)
{
    asr_prop_area *pa = __system_property_area__;
    pa_info_array = (void*) (((char*) pa) + ASR_PA_INFO_START);
    char * m_anchor;

    if(!force_init){
	    if(pa->magic == ASR_PROP_AREA_MAGIC){
		    // property info already set from pervois exec period
		    uart_printf("ASR_PROP_AREA_MAGIC detect\r\n");

		    // force reset the reset of unused memory ,as preboot/boot2 may use the 1K [TINY_PROP_AREA],
		    // the next exec bin like boot33 ,should force init the append 3K unused area.
		    m_anchor = (unsigned int)__system_property_find_nth(pa->count - 1) + sizeof(asr_prop_info);
		    memset(m_anchor,0,(ASR_PROP_AREA_BASE + ASR_PA_SIZE - (int)m_anchor));

		    asr_property_area_inited = 1;
		    return -1;
	}
    }

    //clean zero init
    memset(pa, 0, ASR_PA_SIZE);
    pa->magic	= ASR_PROP_AREA_MAGIC;
    pa->version = ASR_PROP_AREA_VERSION;

    asr_property_area_inited = 1;

    return 0;
}


/**
 ** COMMON_API
 ** load the <value> into [arg=buffer] with index key <name>
 **/
int asr_property_get_buffer(const char *name, char *buffer)
{
    const asr_prop_info *pi = __system_property_find(name);

	if(!asr_property_area_inited)
		asr_property_area_init(0);

    if(pi != 0) {
        return __system_property_read(pi, 0, buffer);
    } else {
        buffer[0] = 0;
        return 0;
    }
}

/**
 ** COMMON_API
 ** queue the <value> with index key <name>
 **/
const char* asr_property_get(const char *name)
{
    asr_prop_info *pi;

    if(strlen(name) >= PROP_NAME_MAX) return 0;

	if(!asr_property_area_inited)
		asr_property_area_init(0);

    pi = (asr_prop_info*) __system_property_find(name);

    if(pi != 0) {
        return pi->value;
    } else {
        return 0;
    }
}

/**
 ** COMMON_API
 ** create or udpate the property <name> with <value>
 **/
int asr_property_set(const char *name, const char *value)
{
    asr_prop_area *pa;
    asr_prop_info *pi;

    int namelen = strlen(name);
    int valuelen = strlen(value);

    if(namelen >= PROP_NAME_MAX) return -1;
    if(valuelen >= PROP_VALUE_MAX) return -1;
    if(namelen < 1) return -1;

	if(!asr_property_area_inited)
		asr_property_area_init(0);

    pi = (asr_prop_info*) __system_property_find(name);

    if(pi != 0) {
        pa = __system_property_area__;
        update_prop_info(pi, value, valuelen);
#if 0//NO MUTEX
        pa->serial++;
        __futex_wake(&pa->serial, INT32_MAX);
#endif
    } else {
        pa = __system_property_area__;
        if(pa->count == ASR_PA_COUNT_MAX) return -1;

        pi = pa_info_array + pa->count;
        pi->serial = (valuelen << 24);
        memcpy(pi->name, name, namelen + 1);
        memcpy(pi->value, value, valuelen + 1);

        pa->toc[pa->count] =
            (namelen << 24) | (((unsigned) pi) - ((unsigned) pa));

        pa->count++;
#if 0 //NO MUTEX
        pa->serial++;
        __futex_wake(&pa->serial, INT32_MAX);
#endif
    }

    return 0;
}

/**
 ** COMMON_API
 ** uart dump all configed property with [<name>]=[<value>] format
 **/
int asr_property_dump(void)
{
    char name[PROP_NAME_MAX];
    char value[PROP_VALUE_MAX];
    const asr_prop_info *pi;
    unsigned n;

	if(!asr_property_area_inited)
		asr_property_area_init(0);

    uart_printf("[ver_%x][%s]\r\n",ASR_PROP_AREA_VERSION,__func__);

    for(n = 0; (pi = __system_property_find_nth(n)); n++) {
        __system_property_read(pi, name, value);
		uart_printf("[%s]=[%s]\r\n",name,value);
    }
    return 0;
}
