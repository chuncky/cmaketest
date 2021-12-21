#ifndef _ASR_FP_MM_SEMAPHORE_H_
#define _ASR_FP_MM_SEMAPHORE_H_

#include "utils/ui_osa_utils.h"
#include "utils/Mutex.h"

class Semaphore {
public:

    Semaphore();
    ~Semaphore();
    // Wait on the condition variable.  Lock the mutex before calling.
    // Note that spurious wake-ups may happen.
    void wait(Mutex& mutex);
	void down(Mutex& mutex);
    void signal();
	void up();
	UINT32 id();

private:
    mmf_sem mSem;
};


inline Semaphore::Semaphore() {
	mmf_sem_init(&mSem, 0);
}
inline Semaphore::~Semaphore() {
    mmf_sem_deinit(&mSem);
}
inline void Semaphore::wait(Mutex& mutex) {
    mmf_sem_wait(&mSem, &mutex.mMutex);
}
inline void Semaphore::signal() {
    mmf_sem_signal(&mSem);
}
inline void Semaphore::down(Mutex& mutex) {
    mmf_sem_down(&mSem, &mutex.mMutex);
}
inline void Semaphore::up() {
    mmf_sem_up(&mSem);
}
inline  UINT32 Semaphore::id()
{
    return (UINT32)mSem;
}

#endif // _LIBS_UTILS_CONDITON_H

