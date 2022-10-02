#include "mini_httpd_debug.h"

#include "mini_httpd_mutex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>

#if defined(_WIN32) || defined(_WIN64)
#include <direct.h>
#include <io.h>
#include <time.h>
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#if !defined(__ANDROID__)
static int LogFileOutputMode = 0;
static char LogFilePrefix[64] = {'\0'};
static MiniHttpdMutex LogFileLocker;

#define MINIHTTPD_LOG_FILE_MODE_ENV "MINIHTTPD_LOG_FILE_MODE"
#endif

#ifdef MINIHTTPD_MODULE
#undef MINIHTTPD_MODULE
#endif

#define MINIHTTPD_MODULE "Debug"

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(__ANDROID__)
    int MINIHTTPD_LOG_INIT(const char *pFilePrefix)
    {
        // TODO:
        pid_t pid = getpid();
        char *pLogFileModeEnv = getenv(MINIHTTPD_LOG_FILE_MODE_ENV);

        if (pLogFileModeEnv != NULL)
        {
            int mode = atoi(pLogFileModeEnv);
            LogFileOutputMode = mode == 0 ? 0 : 1;
        }

        if (pFilePrefix != NULL)
        {
            sprintf(LogFilePrefix, "%s_%.4d", pFilePrefix, pid);
        }
        else
        {
            // strcpy(LogFilePrefix, pFilePrefix);
            sprintf(LogFilePrefix, "%s_%.4d", "Sinwt", pid);
        }

        mini_httpd_mutex_init(&LogFileLocker);

        return 0;
    }

    int MINIHTTPD_LOG_WRITE(const char *format, ...)
    {
        int writen;
        FILE *pFD = stdout;

        va_list args;

        mini_httpd_mutex_lock(&LogFileLocker);

        // TODO:
        if (LogFileOutputMode == 1)
        {
            //
            char today[32];
            time_t nowTime;
            struct tm *pInfo;

            char filename[128];

            time(&nowTime);
            pInfo = localtime(&nowTime);

            // strftime(today, sizeof(today), "%Y-%m-%d", pInfo);
            strftime(today, sizeof(today), "%m-%d-%H", pInfo);
            sprintf(filename, "%s_%s.log", LogFilePrefix, today);

#if defined(_WIN32) || defined(_WIN64)
            pFD = fopen(filename, "ab+");
#else
            pFD = fopen(filename, "a+");
#endif

            // write time stamp.
            strftime(today, sizeof(today), "%H:%M:%S ", pInfo);
            fwrite(today, strlen(today), 1, pFD);
        }

        va_start(args, format);
        writen = vfprintf(pFD, format, args);
        va_end(args);

        if (LogFileOutputMode == 1)
        {
            fclose(pFD);
        }

        mini_httpd_mutex_unlock(&LogFileLocker);

        return writen;
    }

    int MINIHTTPD_LOG_UNINIT()
    {
        // TODO:
        mini_httpd_mutex_uninit(&LogFileLocker);

        return 0;
    }
#endif

    int MINIHTTPD_Debug_HexDump_Ex(const char *pFile, int line, unsigned char *pData, int size)
    {
        // ENTER();

        if (pData == NULL || size <= 0)
        {
            MINIHTTPD_LOG_DEBUG("invalid parameters: %p, %d.\n", pData, size);
            return -1;
        }

        MINIHTTPD_LOG_DEBUG("[%s:%d] HexDump data size %d.\n", pFile, line, size);

        {
            int i, rows, remain;
            const unsigned char *p = pData;

            rows = size / 16;
            remain = size % 16;

            for (i = 0; i < rows; i++)
            {
                printf("%.4x : %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x | %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
                       i * 16,
                       p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
                       p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);

                p += 16;
            }

            if (remain)
            {
                if (remain > 8)
                {
                    printf("%.4x : %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x |",
                           rows * 16, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

                    p += 8;
                    remain -= 8;

                    for (i = 0; i < remain; i++)
                    {
                        printf(" %.2x", p[i]);
                    }
                }
                else
                {
                    printf("%.4x :", rows * 16);

                    for (i = 0; i < remain; i++)
                    {
                        printf(" %.2x", p[i]);
                    }
                }

                printf("\n");
            }
        }

        // LEAVE();

        return 0;
    }

#ifdef __cplusplus
};
#endif
