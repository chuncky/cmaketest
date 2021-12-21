#ifndef _ASR_FP_MM_MUTEX_H_
#define _ASR_FP_MM_MUTEX_H_

#include "utils/ui_osa_utils.h"

class Mutex
{
public:

    Mutex()
    {
        mmf_thread_mutex_init(&mMutex, NULL);
    }
	Mutex(const char *name)
	{
        mmf_thread_mutexattr attr;
		attr.name_ptr = name;
	    mmf_thread_mutex_init(&mMutex, &attr);
	}
    ~Mutex()
    {
        mmf_thread_mutex_destroy(&mMutex);
    }

    // lock or unlock the mutex
    inline int lock() {
        return - mmf_thread_mutex_lock(&mMutex);
    }
    inline void unlock()
    {
        mmf_thread_mutex_unlock(&mMutex);
    }
	inline int AquireLock()
    {
        return -mmf_thread_mutex_try_lock(&mMutex);
    }

    // Manages the mutex automatically. It'll be locked when Autolock is
    // constructed and released when Autolock goes out of scope.
    class Autolock {
    public:
        inline  Autolock(Mutex& mutex) : mLock(mutex)  {
            mLock.lock();
        }
        inline  Autolock(Mutex* mutex) : mLock(*mutex) {
            mLock.lock();
        }
        inline ~Autolock() {
            mLock.unlock();
        }
    private:
        Mutex& mLock;
    };

private:
    friend class Condition;
	friend class Semaphore;
    // A mutex cannot be copied
    Mutex(const Mutex&);
    Mutex&      operator = (const Mutex&);

    mmf_thread_mutex mMutex;
};





#endif
