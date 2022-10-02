#ifndef MiniHttpdHelperH
#define MiniHttpdHelperH

#include "wrap.h"
#include "parse.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void get_requesthdrs(rio_t *rp);
    void post_requesthdrs(rio_t *rp, int *length);
    int parse_uri(const char *pCwd, char *uri, char *filename, char *cgiargs);
    void serve_static(int fd, char *filename, int filesize);
    void serve_dir(int fd, char *filename);
    void get_filetype(const char *filename, char *filetype);
    void get_dynamic(int fd, char *filename, char *cgiargs);
    void post_dynamic(int fd, char *filename, int contentLength, rio_t *rp);
    void clienterror(int fd, char *cause, char *errnum,
                     char *shortmsg, char *longmsg);

    void mini_httpd_ssl_enable();
    void mini_httpd_ssl_disable();
    int mini_httpd_ssl_enabled();

#if (MINI_HTTPD_HTTPS_ENABLED == 1)
    /* $begin ssl */
    int ssl_prepare(int fd, char *pBuffer, int size);
    void ssl_init(void);
    void https_getlength(char *buf, int *length);
    /* $end ssl */
#endif

#ifdef __cplusplus
};
#endif

#endif /* MiniHttpdHelperH */
