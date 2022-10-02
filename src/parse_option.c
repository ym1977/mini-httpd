#include "parse_option.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

/*
command options parser:
 -d(--daemon)   daemon process
 -p(--port)     assign http port
 -s(--port)     assign https port
 -l(--log)      log path
 */

static void ShowUsage(const char *pProcName)
{
	fprintf(stderr,
			"Usage:\n"
			"\t%s [-d --daemon] [-p --port] [-s --sslport] [-l --log] [-v --version] [-h --help]\n\n",
			pProcName);

	exit(1);
}

static void ShowVersion(void)
{
	fprintf(stderr,
			"版本:1.0\n"
			"功能:迷你Web服务器实现\n"
			"1) 提供基础GET/POST功能\n"
			"2) 实现SSL安全连接\n"
			"3) 提供目录访问和简单的访问控制\n\n"
			"说明：\n\n"
			"a) SSL功能基于OPENSSL库实现\n\n");

	exit(1);
}

/* $start parse_option  */
#if (MINI_HTTPD_HTTPS_ENABLED == 1)
void parse_option(int argc, char **argv, char *d, char **portp, char **logp, char **sslp, char *dossl)
#else
void parse_option(int argc, char **argv, char *d, char **portp, char **logp)
#endif
{
	int opt;
	static char port[16];
#if (MINI_HTTPD_HTTPS_ENABLED == 1)
	static char sslport[16];
#endif
	static char log[64];

	struct option longopts[] =
	{
		{"daemon", 0, NULL, 'd'}, /* 0->hasn't arg   1-> has arg */
		{"port", 1, NULL, 'p'},
#if (MINI_HTTPD_HTTPS_ENABLED == 1)
		{"sslport", 1, NULL, 's'},
		{"extent", 0, NULL, 'e'}, /* extent function -> https */
#endif
		{"log", 1, NULL, 'l'},
		{"help", 0, NULL, 'h'},
		{"version", 0, NULL, 'v'},
		{0, 0, 0, 0}
	}; /* the last must be a zero array */

#if (MINI_HTTPD_HTTPS_ENABLED == 1)
	while ((opt = getopt_long(argc, argv, ":dp:s:l:ehv", longopts, NULL)) != -1)
#else
	while ((opt = getopt_long(argc, argv, ":dp:l:hv", longopts, NULL)) != -1)
#endif
	{
		switch (opt)
		{
		case 'd':
			*d = 1;
			break;
		case 'p':
			strncpy(port, optarg, 15);
			*portp = port;
			break;
#if (MINI_HTTPD_HTTPS_ENABLED == 1)
		case 's':
			strncpy(sslport, optarg, 15);
			*sslp = sslport;
			break;
		case 'e':
			*dossl = 1;
			break;
#endif
		case 'l':
			strncpy(log, optarg, 63);
			*logp = log;
			break;
		case ':':
			fprintf(stderr, "-%c:option needs a value.\n", optopt);
			exit(1);
			break;
		case 'h':
			ShowUsage(argv[0]);
			break;
		case 'v':
			ShowVersion();
			break;
		case '?':
			fprintf(stderr, "unknown option:%c\n", optopt);
			ShowUsage(argv[0]);
			break;
		}
	}
}
/* $end parse_option  */

/* parse_option test
int main(int argc,char **argv)
{

	char d=0,*p=NULL;

	parse_option(argc,argv,&d,&p);
	if(d==1)
		printf("daemon\n");
	if(p!=NULL)
		printf("%s\n",p);

}
*/
