#ifndef ParseOptionH
#define ParseOptionH

#ifdef __cplusplus
extern "C"
{
#endif

/*parse_option.c */
#if (MINI_HTTPD_HTTPS_ENABLED == 1)
    void parse_option(int argc, char **argv, char *d, char **portp, char **logp, char **sslp, char *dossl);
#else
void parse_option(int argc, char **argv, char *d, char **portp, char **logp);
#endif

#ifdef __cplusplus
};
#endif

#endif /* ParseOptionH */
