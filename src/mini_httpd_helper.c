#include "mini_httpd_helper.h"

#include "mini_httpd_log.h"
#include "parse_config.h"

#include <dirent.h>
#include <syslog.h>

#if (MINI_HTTPD_HTTPS_ENABLED == 1)
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

#define PID_FILE "pid.file"

#ifdef __cplusplus
extern "C"
{
#endif

#if (MINI_HTTPD_HTTPS_ENABLED == 1)
    static SSL_CTX *ssl_ctx;
    static SSL *ssl;
    static char *certfile;
    static int ishttps = 0;
    static char httpspostdata[MAXLINE];
#endif

    /* $begin writePid  */
    /* if the process is running, the interger in the pid file is the pid, else is -1  */
    void writePid(int option)
    {
        int pid;
        FILE *fp = Fopen(PID_FILE, "w+");
        if (option)
            pid = (int)getpid();
        else
            pid = -1;
        fprintf(fp, "%d", pid);
        Fclose(fp);
    }
    /* $end writePid  */

    /*
     * read_requesthdrs - read and parse HTTP request headers
     */
    /* $begin get_requesthdrs */
    void get_requesthdrs(rio_t *rp)
    {
        char buf[MAXLINE];

        Rio_readlineb(rp, buf, MAXLINE);
        writetime(); /* write access time in log file */
        // printf("%s", buf);
        while (strcmp(buf, "\r\n"))
        {
            Rio_readlineb(rp, buf, MAXLINE);
            writelog(buf);
            // printf("%s", buf);
        }
        return;
    }
    /* $end get_requesthdrs */

    /* $begin post_requesthdrs */
    void post_requesthdrs(rio_t *rp, int *length)
    {
        char buf[MAXLINE];
        char *p;

        Rio_readlineb(rp, buf, MAXLINE);
        writetime(); /* write access time in log file */
        while (strcmp(buf, "\r\n"))
        {
            Rio_readlineb(rp, buf, MAXLINE);
            if (strncasecmp(buf, "Content-Length:", 15) == 0)
            {
                p = &buf[15];
                p += strspn(p, " \t");
                *length = atol(p);
            }
            writelog(buf);
            // printf("%s", buf);
        }
        return;
    }
    /* $end post_requesthdrs */

    /* $begin post_dynamic */
    void serve_dir(int fd, char *filename)
    {
        DIR *dp;
        struct dirent *dirp;
        struct stat sbuf;
        struct passwd *filepasswd;
        int num = 1;
        char files[MAXLINE], buf[MAXLINE], name[MAXLINE], img[MAXLINE], modifyTime[MAXLINE], dir[MAXLINE];
        char *p;

        /*
         * Start get the dir
         * for example: /home/yihaibo/kerner/web/doc/dir -> dir[]="dir/";
         */
        p = strrchr(filename, '/');
        ++p;
        strcpy(dir, p);
        strcat(dir, "/");
        /* End get the dir */

        if ((dp = opendir(filename)) == NULL)
            syslog(LOG_ERR, "cannot open dir:%s", filename);

        sprintf(files, "<html><title>Dir Browser</title>");
        sprintf(files, "%s<style type="
                       "text/css"
                       "> a:link{text-decoration:none;} </style>",
                files);
        sprintf(files, "%s<body bgcolor="
                       "ffffff"
                       " font-family=Arial color=#fff font-size=14px>\r\n",
                files);

        while ((dirp = readdir(dp)) != NULL)
        {
            if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
                continue;
            sprintf(name, "%s/%s", filename, dirp->d_name);
            Stat(name, &sbuf);
            filepasswd = getpwuid(sbuf.st_uid);

            if (S_ISDIR(sbuf.st_mode))
            {
                sprintf(img, "<img src="
                             "dir.png"
                             " width="
                             "24px"
                             " height="
                             "24px"
                             ">");
            }
            else if (S_ISFIFO(sbuf.st_mode))
            {
                sprintf(img, "<img src="
                             "fifo.png"
                             " width="
                             "24px"
                             " height="
                             "24px"
                             ">");
            }
            else if (S_ISLNK(sbuf.st_mode))
            {
                sprintf(img, "<img src="
                             "link.png"
                             " width="
                             "24px"
                             " height="
                             "24px"
                             ">");
            }
            else if (S_ISSOCK(sbuf.st_mode))
            {
                sprintf(img, "<img src="
                             "sock.png"
                             " width="
                             "24px"
                             " height="
                             "24px"
                             ">");
            }
            else
                sprintf(img, "<img src="
                             "file.png"
                             " width="
                             "24px"
                             " height="
                             "24px"
                             ">");

            sprintf(files, "%s<p><pre>%-2d %s "
                           "<a href=%s%s"
                           ">%-15s</a>%-10s%10d %24s</pre></p>\r\n",
                    files, num++, img, dir, dirp->d_name, dirp->d_name, filepasswd->pw_name, (int)sbuf.st_size, timeModify(sbuf.st_mtime, modifyTime));
        }
        closedir(dp);
        sprintf(files, "%s</body></html>", files);

        /* Send response headers to client */
        sprintf(buf, "HTTP/1.0 200 OK\r\n");
        sprintf(buf, "%sServer: mini-httpd Web Server\r\n", buf);
        sprintf(buf, "%sContent-length: %d\r\n", buf, (int)strlen(files));
        sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, "text/html");

#if (MINI_HTTPD_HTTPS_ENABLED == 1)
        if (ishttps)
        {
            SSL_write(ssl, buf, strlen(buf));
            SSL_write(ssl, files, strlen(files));
        }
        else
#endif
        {
            Rio_writen(fd, buf, strlen(buf));
            Rio_writen(fd, files, strlen(files));
        }
        exit(0);
    }
    /* $end serve_dir */

    /* $begin post_dynamic */
    void post_dynamic(int fd, char *filename, int contentLength, rio_t *rp)
    {
        char buf[MAXLINE], length[32], *emptylist[] = {NULL}, data[MAXLINE];
        int p[2];

#if (MINI_HTTPD_HTTPS_ENABLED == 1)
        int httpsp[2];
#endif

        sprintf(length, "%d", contentLength);
        memset(data, 0, MAXLINE);

        Pipe(p);

        /*       The post data is sended by client,we need to redirct the data to cgi stdin.
         *  	 so, child read contentLength bytes data from fp,and write to p[1];
         *    parent should redirct p[0] to stdin. As a result, the cgi script can
         *    read the post data from the stdin.
         */

        /* https already read all data ,include post data  by SSL_read() */

        if (Fork() == 0)
        { /* child  */
            Close(p[0]);
#if (MINI_HTTPD_HTTPS_ENABLED == 1)
            if (ishttps)
            {
                Write(p[1], httpspostdata, contentLength);
            }
            else
#endif
            {
                Rio_readnb(rp, data, contentLength);
                Rio_writen(p[1], data, contentLength);
            }
            exit(0);
        }

        /* Send response headers to client */
        sprintf(buf, "HTTP/1.0 200 OK\r\n");
        sprintf(buf, "%sServer: mini-httpd Web Server\r\n", buf);

#if (MINI_HTTPD_HTTPS_ENABLED == 1)
        if (ishttps)
            SSL_write(ssl, buf, strlen(buf));
        else
#endif
            Rio_writen(fd, buf, strlen(buf));

        // Wait(NULL);
        Dup2(p[0], STDIN_FILENO); /* Redirct p[0] to stdin */
        Close(p[0]);

        Close(p[1]);
        setenv("CONTENT-LENGTH", length, 1);

#if (MINI_HTTPD_HTTPS_ENABLED == 1)
        if (ishttps) /* if ishttps,we couldnot redirct stdout to client,we must use SSL_write */
        {
            Pipe(httpsp);

            if (Fork() == 0)
            {
                Dup2(httpsp[1], STDOUT_FILENO); /* Redirct stdout to https[1] */
                Execve(filename, emptylist, environ);
            }
            // Wait(NULL);
            Read(httpsp[0], data, MAXLINE);
            SSL_write(ssl, data, strlen(data));
        }
        else
#endif
        {
            Dup2(fd, STDOUT_FILENO); /* Redirct stdout to client */
            Execve(filename, emptylist, environ);
        }
    }
    /* $end post_dynamic */

    /*
     * parse_uri - parse URI into filename and CGI args
     *             return 0 if dynamic content, 1 if static
     */
    /* $begin parse_uri */
    int parse_uri(const char *pCwd, char *uri, char *filename, char *cgiargs)
    {
        char *ptr;
        char tmpcwd[MAXLINE];
        strcpy(tmpcwd, pCwd);
        strcat(tmpcwd, "/");

        if (!strstr(uri, "cgi-bin"))
        {
            /* content */
            strcpy(cgiargs, "");
            strcpy(filename, strcat(tmpcwd, GetConfig(pCwd, "root")));
            strcat(filename, uri);
            if (uri[strlen(uri) - 1] == '/')
                strcat(filename, "home.html");
            return 1;
        }
        else
        { /* Dynamic content */
            ptr = index(uri, '?');
            if (ptr)
            {
                strcpy(cgiargs, ptr + 1);
                *ptr = '\0';
            }
            else
                strcpy(cgiargs, "");
            strcpy(filename, pCwd);
            strcat(filename, uri);
            return 0;
        }
    }
    /* $end parse_uri */

    /*
     * serve_- copy a file back to the client
     */
    /* $begin serve_*/
    void serve_static(int fd, char *filename, int filesize)
    {
        int srcfd;
        char *srcp, filetype[MAXLINE], buf[MAXBUF];

        /* Send response headers to client */
        get_filetype(filename, filetype);
        sprintf(buf, "HTTP/1.0 200 OK\r\n");
        sprintf(buf, "%sServer: mini-httpd Web Server\r\n", buf);
        sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
        sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);

        /* Send response body to client */
        srcfd = Open(filename, O_RDONLY, 0);
        srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
        Close(srcfd);

#if (MINI_HTTPD_HTTPS_ENABLED == 1)
        if (ishttps)
        {
            SSL_write(ssl, buf, strlen(buf));
            SSL_write(ssl, srcp, filesize);
        }
        else
#endif
        {
            Rio_writen(fd, buf, strlen(buf));
            Rio_writen(fd, srcp, filesize);
        }
        Munmap(srcp, filesize);
    }

    /*
     * get_filetype - derive file type from file name
     */
    void get_filetype(const char *filename, char *filetype)
    {
        if (strstr(filename, ".html"))
            strcpy(filetype, "text/html");
        else if (strstr(filename, ".gif"))
            strcpy(filetype, "image/gif");
        else if (strstr(filename, ".jpg"))
            strcpy(filetype, "image/jpeg");
        else if (strstr(filename, ".png"))
            strcpy(filetype, "image/png");
        else
            strcpy(filetype, "text/plain");
    }
    /* $end serve_*/

    /*
     * serve_dynamic - run a CGI program on behalf of the client
     */
    /* $begin get_dynamic */
    void get_dynamic(int fd, char *filename, char *cgiargs)
    {
        char buf[MAXLINE], *emptylist[] = {NULL};
#if (MINI_HTTPD_HTTPS_ENABLED == 1)
        char httpsbuf[MAXLINE];
        int p[2];
#endif

        /* Return first part of HTTP response */
        sprintf(buf, "HTTP/1.0 200 OK\r\n");
        sprintf(buf, "%sServer: mini-httpd Web Server\r\n", buf);

#if (MINI_HTTPD_HTTPS_ENABLED == 1)
        if (ishttps)
            SSL_write(ssl, buf, strlen(buf));
        else
#endif
            Rio_writen(fd, buf, strlen(buf));

#if (MINI_HTTPD_HTTPS_ENABLED == 1)
        if (ishttps)
        {
            Pipe(p);
            if (Fork() == 0)
            { /* child  */
                Close(p[0]);
                setenv("QUERY_STRING", cgiargs, 1);
                Dup2(p[1], STDOUT_FILENO);            /* Redirect stdout to p[1] */
                Execve(filename, emptylist, environ); /* Run CGI program */
            }
            Close(p[1]);
            Read(p[0], httpsbuf, MAXLINE); /* parent read from p[0] */
            SSL_write(ssl, httpsbuf, strlen(httpsbuf));
        }
        else
#endif
        {
            if (Fork() == 0)
            { /* child */
                /* Real server would set all CGI vars here */
                setenv("QUERY_STRING", cgiargs, 1);
                Dup2(fd, STDOUT_FILENO);              /* Redirect stdout to client */
                Execve(filename, emptylist, environ); /* Run CGI program */
            }
        }
        // Wait(NULL); /* Parent waits for and reaps child */
    }
    /* $end get_dynamic */

    /*
     * clienterror - returns an error message to the client
     */
    /* $begin clienterror */
    void clienterror(int fd, char *cause, char *errnum,
                     char *shortmsg, char *longmsg)
    {
        char buf[MAXLINE], body[MAXBUF];

        /* Build the HTTP response body */
        sprintf(body, "<html><title>mini-httpd Error</title>");
        sprintf(body, "%s<body bgcolor="
                      "ffffff"
                      ">\r\n",
                body);
        sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
        sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
        sprintf(body, "%s<hr><em>mini-httpd Web server</em>\r\n", body);

        /* Print the HTTP response */
        sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
        sprintf(buf, "%sContent-type: text/html\r\n", buf);
        sprintf(buf, "%sContent-length: %d\r\n\r\n", buf, (int)strlen(body));

#if (MINI_HTTPD_HTTPS_ENABLED == 1)
        if (ishttps)
        {
            SSL_write(ssl, buf, strlen(buf));
            SSL_write(ssl, body, strlen(body));
        }
        else
#endif
        {
            Rio_writen(fd, buf, strlen(buf));
            Rio_writen(fd, body, strlen(body));
        }
    }
    /* $end clienterror */

    void mini_httpd_ssl_enable()
    {
        ishttps = 1;
    }

    void mini_httpd_ssl_disable()
    {
        ishttps = 0;
    }

    int mini_httpd_ssl_enabled()
    {
        return ishttps;
    }

/* $begin ssl init  */
#if (MINI_HTTPD_HTTPS_ENABLED == 1)
    int ssl_prepare(int fd, char *pBuffer, int size)
    {
        ssl = SSL_new(ssl_ctx);
        SSL_set_fd(ssl, fd);
        if (SSL_accept(ssl) == 0)
        {
            ERR_print_errors_fp(stderr);
            exit(1);
        }
        SSL_read(ssl, pBuffer, size);
    }

    void ssl_init(const char *pCwd)
    {
        static char crypto[] = "RC4-MD5";
        certfile = GetConfig(pCwd, "ca");

        SSL_load_error_strings();
        SSLeay_add_ssl_algorithms();
        ssl_ctx = SSL_CTX_new(SSLv23_server_method());

        if (certfile[0] != '\0')
            if (SSL_CTX_use_certificate_file(ssl_ctx, certfile, SSL_FILETYPE_PEM) == 0 || SSL_CTX_use_PrivateKey_file(ssl_ctx, certfile, SSL_FILETYPE_PEM) == 0 || SSL_CTX_check_private_key(ssl_ctx) == 0)
            {
                ERR_print_errors_fp(stderr);
                exit(1);
            }
        if (crypto != (char *)0)
        {

            if (SSL_CTX_set_cipher_list(ssl_ctx, crypto) == 0)
            {
                ERR_print_errors_fp(stderr);
                exit(1);
            }
        }
    }
#endif
/* $end ssl init */

/* $begin https_getlength */
#if (MINI_HTTPD_HTTPS_ENABLED == 1)
    void https_getlength(char *buf, int *length)
    {
        char *p, line[MAXLINE];
        char *tmpbuf = buf;
        int lengthfind = 0;

        while (*tmpbuf != '\0')
        {
            p = line;
            while (*tmpbuf != '\n' && *tmpbuf != '\0')
                *p++ = *tmpbuf++;
            *p = '\0';
            if (!lengthfind)
            {
                if (strncasecmp(line, "Content-Length:", 15) == 0)
                {
                    p = &line[15];
                    p += strspn(p, " \t");
                    *length = atoi(p);
                    lengthfind = 1;
                }
            }

            if (strncasecmp(line, "\r", 1) == 0)
            {
                strcpy(httpspostdata, ++tmpbuf);
                break;
            }
            ++tmpbuf;
        }
        return;
    }
#endif
    /* $end https_getlength  */

#ifdef __cplusplus
};
#endif
