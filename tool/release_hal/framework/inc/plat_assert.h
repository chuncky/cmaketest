
#ifndef __PLAT_ASSERT_H__
#define __PLAT_ASSERT_H__

#include "plat_arm_defs.h"
#include "plat_config_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef assert
#undef ASSERT
#undef UI_ASSERT
#undef UI_PASSERT
#undef DBG_ASSERT

#ifdef __NDEBUG

#define assert(cOND)		((void)0)
#define ASSERT(cOND)		((void)0)
#define UI_ASSERT(cOND)		((void)0)
#define UI_PASSERT(cOND, fmt, ...)
#define DBG_ASSERT(cOND, fmt, ...)

#else	//__NDEBUG

extern int  fatal_printf(const char* fmt, ...);
extern unsigned long disableInterrupts(void);
extern void utilsAssertFail(const char      *cond,
                            const char      *file,
                            signed short    line,
                            unsigned char   allowDiag);
extern void UI_Assert(const char *cond,
                      const char *file,
                      int    line,
                      char	 *assert_info);
extern char *UI_SaveAssertInfo (const char *format, ...);

#ifdef CONFIG_DEBUG_UIASS
#define UI_ASSERT(cOND)		(cOND) ? (void)0 : UI_Assert(#cOND, __MODULE__, __LINE__, NULL)
#define UI_PASSERT(cOND, PRINT_STR)		(cOND) ? (void)0 : UI_Assert(#cOND, __MODULE__, __LINE__, UI_SaveAssertInfo PRINT_STR)
#else
#define UI_ASSERT(cOND)		(cOND) ? (void)0 : utilsAssertFail(#cOND, __FILE__, __LINE__, 1)
#define UI_PASSERT(cOND, PRINT_STR)		{ if (!(cOND))	{ fatal_printf##PRINT_STR;  utilsAssertFail(#cOND, __FILE__, __LINE__, 1);	   }  }
#endif
/* Add ASSERT and assert Macro definiton for GUI adaption.In GUI, because of 
** the heavy use of ASSERT and assert, it would be much work and fragmentary 
** modification if replaced ASSERT and assert with UI_ASSERT, so keep them 
** here, and redefine the standard lib __assert and abort APIs.
*/

#define ASSERT		UI_ASSERT
#define assert 		UI_ASSERT

#define DBG_ASSERT(cOND, fmt, ...)		UI_PASSERT(cOND,(fmt, ##__VA_ARGS__))

#endif	//__NDEBUG

#ifdef __cplusplus
}
#endif

#endif
