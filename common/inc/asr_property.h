typedef struct asr_prop_area asr_prop_area;
typedef struct asr_prop_info asr_prop_info;

#define ASR_PROP_AREA_MAGIC         0x504f5250 //"PROP"
#define ASR_PROP_AREA_VERSION       0x00000002
#define ASR_PROP_AREA_BASE          0xB0020C00
#define ASR_PROP_AREA_TOTAL_SIZE    0x00001000

//asr_prop_info_size =  PROP_NAME_MAX + PROP_VALUE_MAX + sizeof(asr_prop_info.serial)
//ASR_PA_COUNT_MAX   =  (ASR_PA_SIZE - ASR_PA_INFO_START)/asr_prop_info_size
//ASR_PA_INFO_START  >= (ASR_PA_COUNT_MAX*4) + sizeof[asr_prop_area(magic+count+serial+version+reserved[4])]

#define PROP_NAME_MAX     20
#define PROP_VALUE_MAX    40
#define ASR_PA_INFO_START 256

//for preboot & boot2
//the limitation of PROPERTY_ASRE size should be 1k
//[0xB0020C00]~[0xB0021000] 1K PROPERTY
//after boot2 but before CP
//the limitation of PROPERTY_ASRE size should be 4k
//[0xB0020C00]~[0xB0021C00] 4K PROPERTY
#ifdef TINY_PROP_AREA
#define ASR_PA_COUNT_MAX  12
#define ASR_PA_SIZE       1024
#else
#define ASR_PA_COUNT_MAX  60
#define ASR_PA_SIZE       (ASR_PROP_AREA_TOTAL_SIZE)
#endif

#define TOC_NAME_LEN(toc)       ((toc) >> 24)
#define TOC_TO_INFO(area, toc)  ((asr_prop_info*) (((char*) area) + ((toc) & 0xFFFFFF)))

struct asr_prop_area {
    unsigned magic;
    unsigned volatile count;
    unsigned volatile serial;
    unsigned version;
    unsigned reserved[4];
	//a pointer here
    unsigned toc[1];
};

#define SERIAL_VALUE_LEN(serial) ((serial) >> 24)
#define SERIAL_DIRTY(serial)     ((serial) & 1)

/*
** Rules:
**
** - there is only one writer, but many readers
** - asr_prop_area.count will never decrease in value
** - once allocated, a prop_info's name will not change
** - once allocated, a prop_info's offset will not change
** - reading a value requires the following steps
**   1. serial = pi->serial
**   2. if SERIAL_DIRTY(serial), wait*, then goto 1
**   3. memcpy(local, pi->value, SERIAL_VALUE_LEN(serial) + 1)
**   4. if pi->serial != serial, goto 2
**
** - writing a value requires the following steps
**   1. pi->serial = pi->serial | 1
**   2. memcpy(pi->value, local_value, value_len)
**   3. pi->serial = (value_len << 24) | ((pi->serial + 1) & 0xffffff)
**
*/

struct asr_prop_info {
    char name[PROP_NAME_MAX];
    unsigned volatile serial;
    char value[PROP_VALUE_MAX];
};


//COMMON_API
int asr_property_area_init(int force_init);
int asr_property_dump(void);
int asr_property_set(const char *name, const char *value);
const char* asr_property_get(const char *name);
int asr_property_get_buffer(const char *name, char *buffer);
