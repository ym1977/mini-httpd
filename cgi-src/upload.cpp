#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <cgicc/CgiDefs.h>
#include <cgicc/Cgicc.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTMLClasses.h>
using namespace std;
using namespace cgicc;
int main()
{
    Cgicc cgi;

    /* Generate the HTTP response */
    printf("Content-length: %d\r\n", (int)1024);
    printf("Content-type: text/html\r\n\r\n");
    // printf("%s", content);

    // cout << "Content-type:text/html;charset=utf-8\r\n\r\n";
    cout << "<html>\n";
    cout << "<head>\n";
    cout << "<title>CGI 中的文件上传</title>\n";
    cout << "</head>\n";
    cout << "<body>\n";

    // 获取要被上传的文件列表
    const_file_iterator file = cgi.getFile("file");
    if (file != cgi.getFiles().end())
    {
        string pFileName = file->getFilename();

        // 在 cout 中发送数据类型
        cout << HTTPContentHeader(file->getDataType());
        cout << pFileName << endl;
        string file_path = "./";
        file_path += pFileName;

        // 以写模式打开文件
        ofstream outfile;
        outfile.open(file_path.data());
        // 再次向文件写入用户输入的数据
        outfile << file->getData() << endl;
        // 关闭打开的文件
        outfile.close();
    }

    cout << "<文件上传成功>\n";
    cout << "</body>\n";
    cout << "</html>\n";
    fflush(stdout);

    return 0;
}
