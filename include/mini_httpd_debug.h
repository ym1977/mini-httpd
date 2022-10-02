#ifndef MiniHttpdDebugH
#define MiniHttpdDebugH

#include <stdio.h>

#if defined(__ANDROID__)
#include "android/log.h"
#endif

#ifndef MINIHTTPD_LOG_ENABLE_MODE
#define MINIHTTPD_LOG_ENABLE_MODE 1
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define MINIHTTPD_MODULE "Unknown"

#define MINIHTTPD_CONSOLE_TRACE(FORMAT, ...)                \
    do                                                      \
    {                                                       \
        printf(":: [" MINIHTTPD_MODULE "] [%s,%d] " FORMAT, \
               __func__,                                    \
               __LINE__,                                    \
               ##__VA_ARGS__);                              \
    } while (0);

#define MINIHTTPD_CONSOLE_DEBUG(FORMAT, ...)        \
    do                                              \
    {                                               \
        printf(":: [" MINIHTTPD_MODULE "] " FORMAT, \
               ##__VA_ARGS__);                      \
    } while (0);

#if defined(__ANDROID__)

#if 0
// 定义debug信息
#define MINIHTTPD_LOG_DEBUG(...) __android_log_print(ANDROID_LOG_DEBUG, MINIHTTPD_MODULE, __VA_ARGS__)
// 定义debug信息
#define MINIHTTPD_LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, MINIHTTPD_MODULE, __VA_ARGS__)
// 定义error信息
#define MINIHTTPD_LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, MINIHTTPD_MODULE, __VA_ARGS__)
#else
#define MINIHTTPD_ANDROID_LOG_DEBUG(FORMAT, ...)                 \
    do                                                           \
    {                                                            \
        __android_log_print(ANDROID_LOG_DEBUG, MINIHTTPD_MODULE, \
                            "[%s,%d] " FORMAT,                   \
                            __FILE__,                            \
                            __LINE__,                            \
                            ##__VA_ARGS__);                      \
    } while (0);

#define MINIHTTPD_ANDROID_LOG_INFO(FORMAT, ...)                 \
    do                                                          \
    {                                                           \
        __android_log_print(ANDROID_LOG_INFO, MINIHTTPD_MODULE, \
                            "[%s,%d] " FORMAT,                  \
                            __FILE__,                           \
                            __LINE__,                           \
                            ##__VA_ARGS__);                     \
    } while (0);

#define MINIHTTPD_ANDROID_LOG_ERROR(FORMAT, ...)                 \
    do                                                           \
    {                                                            \
        __android_log_print(ANDROID_LOG_ERROR, MINIHTTPD_MODULE, \
                            "[%s,%d] " FORMAT,                   \
                            __FILE__,                            \
                            __LINE__,                            \
                            ##__VA_ARGS__);                      \
    } while (0);

#define MINIHTTPD_LOG_DEBUG(...) MINIHTTPD_ANDROID_LOG_DEBUG(__VA_ARGS__)
#define MINIHTTPD_LOG_INFO(...) MINIHTTPD_ANDROID_LOG_INFO(__VA_ARGS__)
#define MINIHTTPD_LOG_ERROR(...) MINIHTTPD_ANDROID_LOG_ERROR(__VA_ARGS__)
#endif
#else

int MINIHTTPD_LOG_INIT(const char *pFilePrefix);
int MINIHTTPD_LOG_WRITE(const char *format, ...);
int MINIHTTPD_LOG_UNINIT();

#define MINIHTTPD_LOG_DEBUG_V1(FORMAT, ...)                 \
    do                                                      \
    {                                                       \
        printf(":: [" MINIHTTPD_MODULE "] [%s,%d] " FORMAT, \
               __func__,                                    \
               __LINE__,                                    \
               ##__VA_ARGS__);                              \
    } while (0);

#define MINIHTTPD_LOG_INFO_V1(FORMAT, ...)                  \
    do                                                      \
    {                                                       \
        printf(":: [" MINIHTTPD_MODULE "] [%s,%d] " FORMAT, \
               __func__,                                    \
               __LINE__,                                    \
               ##__VA_ARGS__);                              \
    } while (0);

#define MINIHTTPD_LOG_ERROR_V1(FORMAT, ...)                 \
    do                                                      \
    {                                                       \
        printf(":: [" MINIHTTPD_MODULE "] [%s,%d] " FORMAT, \
               __func__,                                    \
               __LINE__,                                    \
               ##__VA_ARGS__);                              \
    } while (0);

#define MINIHTTPD_LOG_DEBUG(FORMAT, ...)                                 \
    do                                                                   \
    {                                                                    \
        MINIHTTPD_LOG_WRITE(":: [" MINIHTTPD_MODULE "] [%s,%d] " FORMAT, \
                            __func__,                                    \
                            __LINE__,                                    \
                            ##__VA_ARGS__);                              \
    } while (0);

#endif

#if (MINIHTTPD_LOG_ENABLE_MODE == 0)
#undef MINIHTTPD_LOG_DEBUG
#define MINIHTTPD_LOG_DEBUG(FORMAT, ...)
#endif

#if 0
#define ENTER(module) MINIHTTPD_DEBUG(":: %s %s enter\n", module, __FUNCTION__)
#define LEAVE(module) MINIHTTPD_DEBUG(":: %s %s enter\n", module, __FUNCTION__)
//#define ENTER() MINIHTTPD_DEBUG(":: [%s] %s enter.\n", MINIHTTPD_MODULE, __FUNCTION__)
//#define LEAVE() MINIHTTPD_DEBUG(":: [%s] %s leave.\n", MINIHTTPD_MODULE, __FUNCTION__)
#else
#define ENTER() MINIHTTPD_LOG_DEBUG("%s enter.\n", __FUNCTION__)
#define LEAVE() MINIHTTPD_LOG_DEBUG("%s leave.\n", __FUNCTION__)
#endif

#define MINIHTTPD_Debug_HexDump(a, b) MINIHTTPD_Debug_HexDump_Ex(__FILE__, __LINE__, a, b)

    int MINIHTTPD_Debug_HexDump_Ex(const char *pFile, int line, unsigned char *pData, int size);

#ifdef __cplusplus
};
#endif

#endif /* MiniHttpdDebugH */
