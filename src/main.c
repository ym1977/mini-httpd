#include "mini_httpd_helper.h"

#include "mini_httpd_debug.h"
#include "mini_httpd_log.h"

#include "parse_config.h"
#include "parse_option.h"
#include "secure_access.h"

#include <unistd.h>

#ifdef MINIHTTPD_MODULE
#undef MINIHTTPD_MODULE
#endif

#define MINIHTTPD_MODULE "APP"

/*$sigChldHandler to protect zimble process */
static void sigChldHandler(int signo)
{
	Waitpid(-1, NULL, WNOHANG);
	return;
}
/*$end sigChldHandler */

int main(int argc, char *argv[])
{
	char isdaemon = 0, *portp = NULL, *logp = NULL, tmpcwd[MAXLINE];

	pid_t pid;

	int isShowdir = 1;
	char *cwd = NULL;

	int port = 80;
	int listenfd;
	int clientfd;

	socklen_t clientlen;
	struct sockaddr_in clientaddr;

#if (MINI_HTTPD_HTTPS_ENABLED == 1)
	int sslport = 443;
	char dossl = 0, *sslportp = NULL;
#endif

	openlog(argv[0], LOG_NDELAY | LOG_PID, LOG_DAEMON);

	cwd = (char *)get_current_dir_name();
	strcpy(tmpcwd, cwd);
	strcat(tmpcwd, "/");
	/* parse argv */

#if (MINI_HTTPD_HTTPS_ENABLED == 1)
	parse_option(argc, argv, &isdaemon, &portp, &logp, &sslportp, &dossl);
	sslportp == NULL ? (sslport = atoi(GetConfig(cwd, "https"))) : (sslport = atoi(sslportp));

	if (dossl == 1 || strcmp(GetConfig(cwd, "dossl"), "yes") == 0)
		dossl = 1;
#else
	parse_option(argc, argv, &isdaemon, &portp, &logp);
#endif

	// portp == NULL ? (port = atoi(GetConfig(cwd, "http"))) : (port = atoi(portp));
	if (portp == NULL)
	{
		port = atoi(GetConfig(cwd, "http"));
	}
	else
	{
		port = atoi(portp);
	}

	Signal(SIGCHLD, sigChldHandler);

	/* init log */
	if (logp == NULL)
		logp = GetConfig(cwd, "log");
	initlog(strcat(tmpcwd, logp));

	/* whethe show dir */
	if (strcmp(GetConfig(cwd, "dir"), "no") == 0)
		isShowdir = 0;

	clientlen = sizeof(clientaddr);

	if (isdaemon == 1 || strcmp(GetConfig(cwd, "daemon"), "yes") == 0)
		Daemon(1, 1);

	writePid(1);

/* $https start  */
#if (MINI_HTTPD_HTTPS_ENABLED == 1)
	if (dossl)
	{
		if ((pid = Fork()) == 0)
		{
			listenfd = Open_listenfd(sslport);
			ssl_init(cwd);

			MINIHTTPD_LOG_DEBUG("start https service on port %d\n", sslport);

			while (1)
			{
				clientfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
				if (access_ornot(cwd, inet_ntoa(clientaddr.sin_addr)) == 0)
				{
					clienterror(clientfd, "maybe this web server not open to you!", "403", "Forbidden", "mini-httpd couldn't read the file");
					continue;
				}

				if ((pid = Fork()) > 0)
				{
					Close(clientfd);
					continue;
				}
				else if (pid == 0)
				{
					mini_httpd_ssl_enable();
					DoInteraction(cwd, isShowdir, clientfd);
					exit(1);
				}
			}
		}
	}
#endif

	/* $end https */

	listenfd = Open_listenfd(port);
	MINIHTTPD_LOG_DEBUG("start http service on port %d\n", port);

	while (1)
	{
		clientfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
		if (access_ornot(cwd, inet_ntoa(clientaddr.sin_addr)) == 0)
		{
			clienterror(clientfd, "maybe this web server not open to you!", "403", "Forbidden", "mini-httpd couldn't read the file");
			continue;
		}

		if ((pid = Fork()) > 0)
		{
			Close(clientfd);
			continue;
		}
		else if (pid == 0)
		{
			DoInteraction(cwd, isShowdir, clientfd);

			exit(1);
		}
	}
}
/* $end main */
