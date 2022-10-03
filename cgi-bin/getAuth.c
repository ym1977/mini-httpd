#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#ifndef MAXLINE
#define MAXLINE 8192 /* max text line length */
#endif

int main(void)
{
    char *buf, *p;
    char name[MAXLINE], passwd[MAXLINE], content[MAXLINE];

    /* Extract the two arguments */
    if ((buf = getenv("QUERY_STRING")) != NULL)
    {
        p = strchr(buf, '&');
        *p = '\0';
        strcpy(name, buf);
        strcpy(passwd, p + 1);
    }

    /* Make the response body */
    sprintf(content, "Get User Info: %s and %s\r\n<p>", name, passwd);
    sprintf(content, "%s\r\n", content);

    sprintf(content, "%sYou are welcom !\r\n", content);

    /* Generate the HTTP response */
    printf("Content-length: %d\r\n", (int)strlen(content));
    printf("Content-type: text/html\r\n\r\n");
    printf("%s", content);
    fflush(stdout);

    return 0;
}
