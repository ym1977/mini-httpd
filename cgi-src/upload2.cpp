// 如果是Exe形式的CGI，则使用如下语句编译：
// g++ -g -o upload.cgi upload.cpp
//  -I/usr/local/cgicc/include /usr/local/cgicc/lib/libcgicc.a
// 如果是共享库（Windows平台叫动态库）形式的CGI，则使用如下语句编译：
// g++ -g -o upload.cgi upload.cpp
//  -shared -fPIC -I/usr/local/cgicc/include /usr/local/cgicc/lib/libcgicc.a

#include <stdio.h>
#include <sstream>

#include "cgicc/Cgicc.h"
#include "cgicc/HTMLClasses.h"
#include "cgicc/HTTPHTMLHeader.h"

const char *ENV[24] = {
    "COMSPEC", "DOCUMENT_ROOT", "GATEWAY_INTERFACE",
    "HTTP_ACCEPT", "HTTP_ACCEPT_ENCODING",
    "HTTP_ACCEPT_LANGUAGE", "HTTP_CONNECTION",
    "HTTP_HOST", "HTTP_USER_AGENT", "PATH",
    "QUERY_STRING", "REMOTE_ADDR", "REMOTE_PORT",
    "REQUEST_METHOD", "REQUEST_URI", "SCRIPT_FILENAME",
    "SCRIPT_NAME", "SERVER_ADDR", "SERVER_ADMIN",
    "SERVER_NAME", "SERVER_PORT", "SERVER_PROTOCOL",
    "SERVER_SIGNATURE", "SERVER_SOFTWARE"};

int main(int argc, char *argv[])
{
    try
    {
        cgicc::Cgicc cgi;

        // Output the HTTP headers for an HTML document,
        // and the HTML 4.0 DTD info
        std::cout << cgicc::HTTPHTMLHeader()
                  << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict)
                  << std::endl;
        std::cout << cgicc::html().set("lang", "en").set("dir", "ltr")
                  << std::endl;

        // Set up the page's header and title.
        std::cout << cgicc::head() << std::endl;
        std::cout << cgicc::title() << "GNU cgicc v" << cgi.getVersion()
                  << cgicc::title() << std::endl;
        std::cout << cgicc::head() << std::endl;

        // Start the HTML body
        std::cout << cgicc::body() << std::endl;

        // Print out a message
        std::cout << cgicc::h1("Hello, world from GNU cgicc") << std::endl;
        const cgicc::CgiEnvironment &env = cgi.getEnvironment();

        std::cout << "<p>accept: " << env.getAccept() << std::endl;
        std::cout << "<p>user agent: " << env.getUserAgent() << std::endl;

        std::cout << "<p>cookie: " << std::endl;
        const std::vector<cgicc::HTTPCookie> &cookies = env.getCookieList();
        for (std::vector<cgicc::HTTPCookie>::size_type i = 0; i < cookies.size(); ++i)
        {
            const cgicc::HTTPCookie &cookie = cookies[i];
            std::cout << "<br>    cookie[" << cookie.getName()
                      << "] = " << cookie.getValue() << std::endl;
        }

        std::cout << "<p>query string: " << env.getQueryString() << std::endl;
        std::cout << "<p>remote: " << env.getRemoteAddr() << ":" << env.getServerPort()
                  << std::endl;

        std::cout << "<p>form: " << std::endl;
        const std::vector<cgicc::FormEntry> &form_entries = cgi.getElements();
        for (std::vector<cgicc::FormEntry>::size_type i = 0; i < form_entries.size(); ++i)
        {
            const cgicc::FormEntry &form_entry = form_entries[i];
            std::cout << "<br>    form["
                      << form_entry.getName() << "] = "
                      << form_entry.getValue() << std::endl;
        }

        //
        // 取被上传的文件信息
        //

        // 使用getFile取得指定的被上传文件信息
        cgicc::const_file_iterator file_iter = cgi.getFile("filename");

        // 使用getFiles可以取得所有被上传文件信息
        if (file_iter == cgi.getFiles().end())
        {
            std::cout << "<p>file size: " << cgi.getFiles().size() << std::endl;
        }
        else
        {
            const cgicc::FormFile &file = *file_iter;
            std::cout << "<p>file: " << std::endl;
            std::cout << "<br>    name: "
                      << file.getName() << std::endl;
            std::cout << "<br>    filename: "
                      << file.getFilename() << std::endl;
            std::cout << "<br>    type: "
                      << file.getDataType() << std::endl;
            std::cout << "<br>    size: "
                      << file.getDataLength() << std::endl;
            std::cout << "<br>    content: "
                      << file.getData() << std::endl;
        }

        // Close the document
        std::cout << cgicc::body() << cgicc::html();
    }
    catch (const std::exception &e)
    {
        // handle error condition
    }

    return 0;
}
