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
    int length = 0;
    char content[MAXLINE], data[MAXLINE];

    if ((buf = getenv("CONTENT-LENGTH")) != NULL)
    {
        length = atol(buf);
    }

    p = fgets(data, length + 1, stdin);
    if (p == NULL)
        sprintf(content, "Something is wrong\r\n");
    else
        sprintf(content, "OK, Get User Post Info: %d, %s\r\n", length, data);

    printf("Content-length: %d\r\n", (int)strlen(content));
    printf("Content-type: text/html\r\n\r\n");
    printf("%s", content);
    fflush(stdout);

    return 0;
}
