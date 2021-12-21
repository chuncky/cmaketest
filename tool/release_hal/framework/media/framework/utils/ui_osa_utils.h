#ifndef __UI_OSA_UTILS_H__
#define __UI_OSA_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif
#ifndef INT64_MAX
#define INT64_MAX  0x7FFFFFFFFFFFFFFFll
#endif

#ifndef UINT32_MAX
#define  UINT32_MAX 0xFFFFFFFF
#endif

#include "ui_os_api.h"
#include "task_cnf.h"
#include "fs_api.h"

#define MEDIA_TASK_MEDIARECORDER_DEFAULT_PRIORITY MMI_VIDEO_RECORDER_TASK_PRIORITY
#define MEDIA_TASK_DEFAULT_PRIORITY    MMI_VIDEO_CODECS_TASK_PRIORITY
#define MEDIA_TASK_DEFAULT_STACK_SIZE  MMI_VIDEO_CODECS_SIZE
#define MEDIA_TASK_MEDIARECORDER_STACK_SIZE  (1024*12)
#define MEDIA_TASK_MEDIAPLAYER_STACK_SIZE    (1024*8)

typedef u8  mmf_thread_mutex;
typedef HANDLE mmf_sem;

typedef struct
{
	HANDLE sem;
} mmf_thread_cond;

typedef void *(*MMF_THREAD_ENTRY)(void*);

typedef struct {
    HANDLE task_handle;
	PTASK_ENTRY task_entry;
	void *arg;
	mmf_sem sem_done;	
} mmf_thread;

typedef struct
{
    const char *name_ptr;
    unsigned int stack_size;
    unsigned int priority;
    unsigned int preempt_threshold;
    unsigned long time_slice;
    unsigned int auto_start;
} mmf_thread_attr;

typedef struct {
    const char *name_ptr;
    unsigned int priority_inherit;
} mmf_thread_mutexattr;


typedef struct {
    unsigned int val;
} mmf_thread_condattr;


typedef int mmf_file_fd;


#define    MMF_SEEK_SET  0
#define    MMF_SEEK_CUR  1
#define    MMF_SEEK_END  2

#define MMF_FILE_MODE_RO FS_O_RDONLY
#define MMF_FILE_MODE_WO (FS_O_WRONLY | FS_O_CREAT)
#define MMF_FILE_MODE_RW (FS_O_RDWR | FS_O_CREAT)



int mmf_thread_create(mmf_thread * tid, mmf_thread_attr * attr, void *(*start) (void *),void *arg);
int mmf_thread_join(mmf_thread *thread,void **value_ptr);
int mmf_thread_cancel(mmf_thread *thread);
int mmf_thread_resume(mmf_thread *tid);
int mmf_thread_mutex_init(mmf_thread_mutex * mutex, mmf_thread_mutexattr * attr);
int mmf_thread_mutex_lock(mmf_thread_mutex * mutex);
int mmf_thread_mutex_try_lock(mmf_thread_mutex * mutex);
int mmf_thread_mutex_unlock(mmf_thread_mutex * mutex);
int mmf_thread_mutex_destroy(mmf_thread_mutex * mutex);
int mmf_thread_cond_wait(mmf_thread_cond * cond, mmf_thread_mutex * mutex);
int mmf_thread_cond_signal (mmf_thread_cond * cond);
int mmf_thread_cond_init(mmf_thread_cond * cond, mmf_thread_condattr * attr);
int mmf_thread_cond_destroy(mmf_thread_cond * cond);
int mmf_usleep(unsigned int time);
int64_t mmf_systemtime();

/** Initializes the semaphore at a given value
 *
 * @param tsem the semaphore to initialize
 *
 * @param val the initial value of the semaphore
 */
void mmf_sem_init(mmf_sem *tsem, unsigned int val);
/** Destroy the semaphore
 *
 * @param tsem the semaphore to destroy
 */
void mmf_sem_deinit(mmf_sem *tsem);
/** Decreases the value of the semaphore. Blocks if the semaphore
 * value is zero.
 *
 * @param tsem the semaphore to decrease
 */
void mmf_sem_down(mmf_sem *tsem, mmf_thread_mutex *mutex);
/** Increases the value of the semaphore
 *
 * @param tsem the semaphore to increase
 */
void mmf_sem_up(mmf_sem *tsem);
/** Reset the value of the semaphore
 *
 * @param tsem the semaphore to reset
 */
void mmf_sem_reset(mmf_sem *tsem);
/** Wait on the condition.
 *
 * @param tsem the semaphore to wait
 */
void mmf_sem_wait(mmf_sem *tsem, mmf_thread_mutex *mutex);
/** Signal the condition,if waiting
 *
 * @param tsem the semaphore to signal
 */
void mmf_sem_signal(mmf_sem *tsem);
int mmf_sem_count(mmf_sem *tsem);
#ifdef MMF_MEM_DEBUG
void *mmf_malloc_dbg(unsigned int size, const char *filename, unsigned int line);
void *mmf_calloc_dbg(unsigned int nBlock,unsigned int size, const char *filename, unsigned int line);
unsigned int mmf_free_dbg(void *, const char *filename, unsigned int line);

#define mmf_malloc(x) mmf_malloc_dbg(x, __FILE__, __LINE__)
#define mmf_calloc(x, y) mmf_calloc_dbg(x, y, __FILE__, __LINE__)
#define mmf_free(x) mmf_free_dbg(x, __FILE__, __LINE__)
#else
void *mmf_malloc(unsigned int size);
void *mmf_calloc(unsigned int nBlock,unsigned int size);
unsigned int mmf_free(void *);
#endif

mmf_file_fd mmf_file_open(const char *filename, int mode);
void  mmf_file_close(mmf_file_fd handle);
void mmf_file_delete(const char *name);
int mmf_file_read(void *buffer, unsigned int nBlock, unsigned int size, mmf_file_fd handle);
int mmf_file_write(const void *buffer, unsigned int nBlock, unsigned int size, mmf_file_fd handle);
int mmf_file_seek(int offset, int fromwhere, mmf_file_fd handle);
int mmf_file_getsize(mmf_file_fd handle);
int mmf_file_flush(mmf_file_fd fd);
const char *mmf_get_library_version();
#ifdef __cplusplus
}
#endif

#endif
