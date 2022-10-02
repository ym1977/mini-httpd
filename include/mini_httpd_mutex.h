#ifndef MiniHttpdMutexH
#define MiniHttpdMutexH

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <pthread.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
typedef CRITICAL_SECTION MiniHttpdMutex;

typedef unsigned int MiniHttpdThreadID;
#define THREADID_NULL -1
#else
typedef pthread_mutex_t MiniHttpdMutex;

typedef pthread_t MiniHttpdThreadID;
#define THREADID_NULL 0
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	int mini_httpd_mutex_init(MiniHttpdMutex *pMutex);
	int mini_httpd_mutex_uninit(MiniHttpdMutex *pMutex);

	void mini_httpd_mutex_lock(MiniHttpdMutex *pMutex);
	void mini_httpd_mutex_unlock(MiniHttpdMutex *pMutex);

#ifdef __cplusplus
};
#endif

#endif // MiniHttpdMutexH
