#include <Windows.h>
#include <WinInet.h>
#include <vector>
#include <assert.h>
#include <Urlmon.h>
#include <Shlwapi.h>
#include "url_parser.h"
#include "tinyxml2.h"

void WriteNewLine(HANDLE fHandle)
{
    DWORD bytesWritten = 0;
    WriteFile(fHandle, "\r\n", strlen("\r\n"), &bytesWritten, NULL);
}

int main()
{
    char *curl = "https://www.u148.net/rss/";
    parsed_url *info = parse_url(curl);
    
    HINTERNET hInternet =
        InternetOpen("pgen", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

    // http://www.u148.net/rss/
    HINTERNET hConnect =
        InternetConnect(hInternet, info->host /*"www.u148.net"*/, INTERNET_DEFAULT_HTTP_PORT, NULL,
                NULL, INTERNET_SERVICE_HTTP, NULL, NULL);
    PCTSTR rgpszAcceptTypes[] = {("text/*"), NULL};
    HINTERNET hRequest = HttpOpenRequest(
            hConnect, NULL, info->path/*"/rss"*/, "HTTP/1.1", NULL, rgpszAcceptTypes,
            INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE |
            INTERNET_FLAG_RELOAD,
            NULL);
    BOOL sendResult = HttpSendRequest(hRequest, NULL, 0, NULL, 0);
    // WaitForSingleObject(hRequest, INFINITE);
    printf("done\n");

    DWORD dwNumberOfBytesAvailable = 0;
    InternetQueryDataAvailable(hRequest, &dwNumberOfBytesAvailable, 0, 0);

    printf("%d", dwNumberOfBytesAvailable);

    DWORD dwNumberOfBytesRead = 0;
    CHAR szBuffer[1025];
    DWORD dwRead = 0;
    HANDLE hFile = CreateFile(
            info->host,
            /*"feed.txt",*/
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, 0);
    while (InternetReadFile(hRequest, szBuffer, sizeof(szBuffer) - 1, &dwRead) &&
            dwRead) {
        szBuffer[dwRead] = 0;
        DWORD bytesWrite = 0;
        WriteFile(hFile, szBuffer, dwRead, &bytesWrite, NULL);

        dwRead = 0;
    }
    CloseHandle(hFile);

    // TODO close all internet handle
    BOOL closeResult = InternetCloseHandle(hInternet);

    tinyxml2::XMLDocument doc;
    doc.LoadFile(info->host);
    parsed_url_free(info);
    const char *Title = doc.FirstChildElement("rss")->FirstChildElement("channel")->FirstChildElement("title")->GetText();
    HANDLE testFileHandle = CreateFile(
            "test.txt",
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, 0);
    size_t length = strlen(Title);
    DWORD written = 0;
    WriteFile(testFileHandle, Title, length, &written, NULL);
    const char *link = doc.FirstChildElement("rss")->FirstChildElement("channel")->FirstChildElement("link")->GetText();
    WriteNewLine(testFileHandle);
    WriteFile(testFileHandle, link, strlen(link), &written, NULL);

    tinyxml2::XMLElement *levelElement = doc.FirstChildElement("rss")->FirstChildElement("channel");
    for (tinyxml2::XMLElement* child = levelElement->FirstChildElement();
            child != NULL;
            child = child->NextSiblingElement())
    {
        // do something with each child element
        printf(child->Name());
        if(strcmp(child->Name(), "title") == 0)
        {
            WriteFile(testFileHandle, child->Name(), strlen(child->Name()), &written, NULL);
            WriteNewLine(testFileHandle);
        }
        else if(strcmp(child->Name(), "link") == 0)
        {
            WriteFile(testFileHandle, child->Name(), strlen(child->Name()), &written, NULL);
            WriteNewLine(testFileHandle);
        }
        else if(strcmp(child->Name(), "description") == 0)
        {
            WriteFile(testFileHandle, child->Name(), strlen(child->Name()), &written, NULL);
            WriteNewLine(testFileHandle);
        }
        else if(strcmp(child->Name(), "item") == 0)
        {
            for(tinyxml2::XMLElement* itemChild = child->FirstChildElement();
                    itemChild != NULL;
                    itemChild = itemChild->NextSiblingElement())
            {
                WriteFile(testFileHandle, itemChild->GetText(), strlen(itemChild->GetText()), &written, NULL);
                WriteNewLine(testFileHandle);
            }
        }
    }
    CloseHandle(testFileHandle);

    //printf(Title);
    return 0;
}
