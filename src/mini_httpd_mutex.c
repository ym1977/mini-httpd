#include "mini_httpd_mutex.h"

#ifdef __cplusplus
extern "C"
{
#endif

	int mini_httpd_mutex_init(MiniHttpdMutex *pMutex)
	{
#if defined(_WIN32) || defined(_WIN64)
		InitializeCriticalSection(pMutex);
#else
		pthread_mutex_init(pMutex, NULL);
#endif
		return 0;
	}

	int mini_httpd_mutex_uninit(MiniHttpdMutex *pMutex)
	{
#if defined(_WIN32) || defined(_WIN64)
		DeleteCriticalSection(pMutex);
#else
		pthread_mutex_destroy(pMutex);
#endif
		return 0;
	}

	void mini_httpd_mutex_lock(MiniHttpdMutex *pMutex)
	{
#if defined(_WIN32) || defined(_WIN64)
		EnterCriticalSection(pMutex);
#else
		pthread_mutex_lock(pMutex);
#endif
	}

	void mini_httpd_mutex_unlock(MiniHttpdMutex *pMutex)
	{
#if defined(_WIN32) || defined(_WIN64)
		LeaveCriticalSection(pMutex);
#else
		pthread_mutex_unlock(pMutex);
#endif
	}

#ifdef __cplusplus
};
#endif
