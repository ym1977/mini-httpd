// upload.c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <vector>

#include "mini_httpd_debug.h"

#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

#define BufferLen 1024

#ifdef MINIHTTPD_MODULE
#undef MINIHTTPD_MODULE
#endif

#define MINIHTTPD_MODULE "uploadFile2"

using namespace std;
using namespace cgicc;

int TestCase1()
{
    try
    {
        Cgicc cgi;

        // Send HTTP header
        cout << HTTPHTMLHeader() << endl;

        // Set up the HTML document
        cout << html() << head(title("cgicc example")) << endl;
        cout << body() << endl;

        // Print out the submitted element
        form_iterator name = cgi.getElement("filename");
        if (name != cgi.getElements().end())
        {
            cout << "Your name: " << **name << endl;
        }
        else
        {
            cout << "user input nothing !" << endl;
        }

        // Close the HTML document
        cout << body() << html();
    }
    catch (exception &e)
    {
        // handle any errors - omitted for brevity
        // return -1;
    }

    return 0;
}

int TestCase2()
{
    int idx = 1;
    cgicc::Cgicc cgi;
    std::vector<cgicc::FormEntry>::const_iterator it;

    const std::vector<cgicc::FormEntry> &formElements = cgi.getElements();

    for (it = formElements.begin(); it != formElements.end(); ++it)
    {
        // process each (*it)
        MINIHTTPD_LOG_WRITE("get form element %d, name %s\n", idx++, *it);
    }

    return 0;
}

#if 0
int TestCase3()
{
    cgicc::Cgicc cgi;

    cgicc::form_iterator idIter = cgi.getElement("id");
    cgicc::file_iterator fileIter = cgi.getFile("file");
    if (idIter == cgi.getElements().end())
    {
        // handle error
    }
    int id = (int)idIter->getIntegerValue();

    if (fileIter == cgi.getFiles().end())
    {
        // handle error
    }
    std::ofstream of(boost::lexical_cast<std::string>(id));
    fileIter->fileIter->writeToStream(of);

    return 0;
}
#endif

int main(int argc, char *argv[])
{
#if 0
    if (MINIHTTPD_LOG_INIT("uploadFile2") < 0)
    {
        return 0;
    }
#endif

    return TestCase1();
    // return TestCase2();
}
