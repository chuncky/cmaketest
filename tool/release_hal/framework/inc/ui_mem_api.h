
#ifndef _UI_MEM_API_H
#define _UI_MEM_API_H

#include "plat_config_defs.h"
#include "plat_types.h"

#ifndef __PLAT_TYPES_H__

#ifndef size_t
typedef unsigned int size_t;
#endif

#endif

#ifdef __cplusplus
  namespace std {
      extern "C" {
#endif  /* __cplusplus */

void *calloc(size_t /*nmemb*/, size_t /*size*/);
   /*
    * allocates space for an array of nmemb objects, each of whose size is
    * 'size'. The space is initialised to all bits zero.
    * Returns: either a null pointer or a pointer to the allocated space.
    */
void free(void * /*ptr*/);
   /*
    * causes the space pointed to by ptr to be deallocated (i.e., made
    * available for further allocation). If ptr is a null pointer, no action
    * occurs. Otherwise, if ptr does not match a pointer earlier returned by
    * calloc, malloc or realloc or if the space has been deallocated by a call
    * to free or realloc, the behaviour is undefined.
    */
void *malloc(size_t /*size*/);
   /*
    * allocates space for an object whose size is specified by 'size' and whose
    * value is indeterminate.
    * Returns: either a null pointer or a pointer to the allocated space.
    */
void *realloc(void * /*ptr*/, size_t /*size*/);
   /*
    * changes the size of the object pointed to by ptr to the size specified by
    * size. The contents of the object shall be unchanged up to the lesser of
    * the new and old sizes. If the new size is larger, the value of the newly
    * allocated portion of the object is indeterminate. If ptr is a null
    * pointer, the realloc function behaves like a call to malloc for the
    * specified size. Otherwise, if ptr does not match a pointer earlier
    * returned by calloc, malloc or realloc, or if the space has been
    * deallocated by a call to free or realloc, the behaviour is undefined.
    * If the space cannot be allocated, the object pointed to by ptr is
    * unchanged. If size is zero and ptr is not a null pointer, the object it
    * points to is freed.
    * Returns: either a null pointer or a pointer to the possibly moved
    *          allocated space.
    */
#ifdef __cplusplus
			}  /* extern "C" */
		 }	/* namespace std */
#endif /* __cplusplus */

#ifdef __cplusplus
      extern "C" {
#endif  /* __cplusplus */

/* Add UI_MALLOC ... for GUI memory allocation adaption.
** In GUI, because of the heavy use of malloc, which is conflict with CP SDK, 
** so we need to replace all malloc related calling into UI_MALLOC, then it's
** suitable and convenient to redefine other memory malloc macro APIs.
*/
void *dbg_calloc(size_t, size_t, const char*, const char*, size_t);
void  dbg_free(void *, const char*, const char*, size_t);
void *dbg_malloc(size_t, const char*, const char*, size_t);
void *dbg_realloc(void *, size_t, const char*, const char*, size_t);
void* UOS_Malloc(size_t /*size*/);
void* UOS_Realloc(void * /*ptr*/, size_t /*size*/);
BOOL   UOS_Free(void * /*ptr*/);
void* UOS_DBG_Malloc(size_t, const char*, const char*, size_t );
int   UOS_DBG_Free	(void *, const char*, const char*, size_t );
const char *ui_parse_file_name (const char *);

#ifdef	CONFIG_DEBUG_UIMEM
#define UI_MALLOC(size)			dbg_malloc((size_t)(size), ui_parse_file_name(__FILE__), __func__, __LINE__)
#define UI_FREE(p)				dbg_free((void*)(p), ui_parse_file_name(__FILE__), __func__, __LINE__)
#define UI_REALLOC(p, size)		dbg_realloc((void*)(p), (size_t)(size), ui_parse_file_name(__FILE__), __func__, __LINE__)
#define UI_CALLOC(nmemb, size)	dbg_calloc((size_t)(nmemb), (size_t)(size), ui_parse_file_name(__FILE__), __func__, __LINE__)
#else
#define UI_MALLOC(size)			malloc((size_t)(size))
#define UI_FREE(p)				free((void*)(p))
#define UI_REALLOC(p, size)		realloc((void*)(p), (size_t)(size))
#define UI_CALLOC(nmemb, size)	calloc((size_t)(nmemb), (size_t)(size))
#endif

//#define CONFIG_SUPPORT_UIMEM
#ifdef	CONFIG_SUPPORT_UIMEM
#error "CONFIG_SUPPORT_UIMEM Defined"
	#ifdef CONFIG_DEBUG_UIMEM
	#define UOS_MALLOC(size)	UOS_DBG_Malloc((size_t)(size), __FILE__, __func__, __LINE__)
	#define UOS_FREE(p)			UOS_DBG_Free((void*)(p), __FILE__, __func__, __LINE__)
	#else
	#define UOS_MALLOC(size)	UOS_Malloc((size_t)(size))
	#define UOS_FREE(p)			UOS_Free((void*)(p))
	#endif
#define UOS_REALLOC(ptr, size) 	UOS_Realloc(ptr, size)
#else	//CONFIG_SUPPORT_UIMEM
#define UOS_MALLOC(size) 		UI_MALLOC(size)
#define UOS_FREE(p)   			UI_FREE(p)
#define UOS_REALLOC(ptr, size) 	UI_REALLOC(ptr, size)
#endif	//CONFIG_SUPPORT_UIMEM

#ifdef CONFIG_DEBUG_UIMLK
	void ui_dbg_mem_leak_onoff(unsigned int onoff);
	void ui_dbg_mem_leak_set(const char* pClassName, unsigned int addr, unsigned int CallerAddr, void *ext);
	void ui_dbg_mem_leak_clear(unsigned int addr);
	void ui_dbg_mem_leak_dump(void);
	void ui_dbg_mem_leak_reset(void);
#endif

#ifdef __cplusplus
     }  /* extern "C" */
#endif /* __cplusplus */

#ifdef __cplusplus
  using ::std::calloc;
  using ::std::free;
  using ::std::malloc;
  using ::std::realloc;
#endif /* __cplusplus */

#endif /* _UI_MEM_API_H */

