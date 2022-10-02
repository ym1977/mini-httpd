#ifndef SecureAccessH
#define SecureAccessH

#ifdef __cplusplus
extern "C"
{
#endif

    /* secure_access.c */
    int access_ornot(const char *pCwd, const char *destip); // 0 -> not 1 -> ok

#ifdef __cplusplus
};
#endif

#endif /* SecureAccessH */
