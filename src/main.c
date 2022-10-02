#include "mini_httpd_helper.h"

#include <unistd.h>

#define PID_FILE "pid.file"

static void doit(int fd);
static void writePid(int option);

static void sigChldHandler(int signo);

static int isShowdir = 1;
char *cwd;

int main(int argc, char *argv[])
{
	int listenfd, connfd, port;
	socklen_t clientlen;
	pid_t pid;
	struct sockaddr_in clientaddr;
	char isdaemon = 0, *portp = NULL, *logp = NULL, tmpcwd[MAXLINE];

#if (MINI_HTTPD_HTTPS_ENABLED == 1)
	int sslport;
	char dossl = 0, *sslportp = NULL;
#endif

	openlog(argv[0], LOG_NDELAY | LOG_PID, LOG_DAEMON);
	cwd = (char *)get_current_dir_name();
	strcpy(tmpcwd, cwd);
	strcat(tmpcwd, "/");
	/* parse argv */

#if (MINI_HTTPD_HTTPS_ENABLED == 1)
	parse_option(argc, argv, &isdaemon, &portp, &logp, &sslportp, &dossl);
	sslportp == NULL ? (sslport = atoi(GetConfig("https"))) : (sslport = atoi(sslportp));

	if (dossl == 1 || strcmp(GetConfig("dossl"), "yes") == 0)
		dossl = 1;
#else
	parse_option(argc, argv, &isdaemon, &portp, &logp);
#endif

	// portp == NULL ? (port = atoi(GetConfig("http"))) : (port = atoi(portp));
	if (portp == NULL)
	{
		port = atoi(GetConfig("http"));
	}
	else
	{
		port = atoi(portp);
	}

	Signal(SIGCHLD, sigChldHandler);

	/* init log */
	if (logp == NULL)
		logp = GetConfig("log");
	initlog(strcat(tmpcwd, logp));

	/* whethe show dir */
	if (strcmp(GetConfig("dir"), "no") == 0)
		isShowdir = 0;

	clientlen = sizeof(clientaddr);

	if (isdaemon == 1 || strcmp(GetConfig("daemon"), "yes") == 0)
		Daemon(1, 1);

	writePid(1);

/* $https start  */
#if (MINI_HTTPD_HTTPS_ENABLED == 1)
	if (dossl)
	{
		if ((pid = Fork()) == 0)
		{
			listenfd = Open_listenfd(sslport);
			ssl_init();

			while (1)
			{
				connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
				if (access_ornot(inet_ntoa(clientaddr.sin_addr)) == 0)
				{
					clienterror(connfd, "maybe this web server not open to you!", "403", "Forbidden", "mini-httpd couldn't read the file");
					continue;
				}

				if ((pid = Fork()) > 0)
				{
					Close(connfd);
					continue;
				}
				else if (pid == 0)
				{
					mini_httpd_ssl_enable();
					doit(connfd);
					exit(1);
				}
			}
		}
	}
#endif

	/* $end https */

	listenfd = Open_listenfd(port);
	while (1)
	{
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
		if (access_ornot(inet_ntoa(clientaddr.sin_addr)) == 0)
		{
			clienterror(connfd, "maybe this web server not open to you!", "403", "Forbidden", "mini-httpd couldn't read the file");
			continue;
		}

		if ((pid = Fork()) > 0)
		{
			Close(connfd);
			continue;
		}
		else if (pid == 0)
		{
			doit(connfd);
			exit(1);
		}
	}
}
/* $end main */

/*$sigChldHandler to protect zimble process */
static void sigChldHandler(int signo)
{
	Waitpid(-1, NULL, WNOHANG);
	return;
}
/*$end sigChldHandler */

/*
 * doit - handle one HTTP request/response transaction
 */
/* $begin doit */
static void doit(int fd)
{
	int is_static, contentLength = 0, isGet = 1;
	struct stat sbuf;
	char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
	char filename[MAXLINE], cgiargs[MAXLINE];
	// char httpspostdata[MAXLINE];
	rio_t rio;

	memset(buf, 0, MAXLINE);

#if (MINI_HTTPD_HTTPS_ENABLED == 1)
	if (mini_httpd_ssl_enabled())
	{
		ssl_prepare(fd, buf, sizeof(buf));
	}
	else
#endif
	{
		/* Read request line and headers */
		Rio_readinitb(&rio, fd);
		Rio_readlineb(&rio, buf, MAXLINE);
	}

	sscanf(buf, "%s %s %s", method, uri, version);

	if (strcasecmp(method, "GET") != 0 && strcasecmp(method, "POST") != 0)
	{
		clienterror(fd, method, "501", "Not Implemented",
					"mini-httpd does not implement this method");
		return;
	}

	/* Parse URI from GET request */
	is_static = parse_uri(cwd, uri, filename, cgiargs);

	if (lstat(filename, &sbuf) < 0)
	{
		clienterror(fd, filename, "404", "Not found",
					"mini-httpd couldn't find this file");
		return;
	}

	if (S_ISDIR(sbuf.st_mode) && isShowdir)
		serve_dir(fd, filename);

	if (strcasecmp(method, "POST") == 0)
		isGet = 0;

	if (is_static)
	{ /* Serve static content */

#if (MINI_HTTPD_HTTPS_ENABLED == 1)
		if (!mini_httpd_ssl_enabled())
#endif
			get_requesthdrs(&rio); /* because https already read the headers -> SSL_read()  */

		if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode))
		{
			clienterror(fd, filename, "403", "Forbidden",
						"mini-httpd couldn't read the file");
			return;
		}
		serve_static(fd, filename, sbuf.st_size);
	}
	else
	{ /* Serve dynamic content */
		if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode))
		{
			clienterror(fd, filename, "403", "Forbidden",
						"mini-httpd couldn't run the CGI program");
			return;
		}

		if (isGet)
		{
#if (MINI_HTTPD_HTTPS_ENABLED == 1)
			if (!mini_httpd_ssl_enabled())
#endif
				get_requesthdrs(&rio); /* because https already read headers by SSL_read() */

			get_dynamic(fd, filename, cgiargs);
		}
		else
		{
#if (MINI_HTTPD_HTTPS_ENABLED == 1)
			if (mini_httpd_ssl_enabled())
				https_getlength(buf, &contentLength);
			else
#endif
				post_requesthdrs(&rio, &contentLength);

			post_dynamic(fd, filename, contentLength, &rio);
		}
	}
}
/* $end doit */

/* $begin writePid  */
/* if the process is running, the interger in the pid file is the pid, else is -1  */
static void writePid(int option)
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
