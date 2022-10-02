#ifndef MiniHttpdLogH
#define MiniHttpdLogH

#include <time.h>

/* for mini_httpd_log.c */

// extern FILE *logfp;
#define MAXLINELEN 8192

#ifdef __cplusplus
extern "C"
{
#endif

    void initlog(const char *logp);
    void writetime();
    void writelog(const char *buf);
    char *timeModify(time_t timeval, char *time);

#ifdef __cplusplus
};
#endif

#endif /* MiniHttpdLogH */
