#include <windows.h>
#include <Wininet.h>
#include <stdio.h>

int main() {

  HINTERNET hInternet =
      InternetOpen("pgen", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
  // http://www.u148.net/rss/
  HINTERNET hConnect =
      InternetConnect(hInternet, "u148.net", INTERNET_DEFAULT_HTTP_PORT, NULL,
                      NULL, INTERNET_SERVICE_HTTP, NULL, NULL);
  PCTSTR rgpszAcceptTypes[] = {("text/*"), NULL};
  HINTERNET hRequest =
      HttpOpenRequest(hConnect, NULL, "/rss", "HTTP/1.1", NULL, rgpszAcceptTypes,
                      INTERNET_FLAG_KEEP_CONNECTION |
                          INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_RELOAD,
                      NULL);

  BOOL sendResult = HttpSendRequest(hRequest, NULL, 0, NULL, 0);
  // WaitForSingleObject(hRequest, INFINITE);
  printf("done\n");

  DWORD dwNumberOfBytesAvailable = 0;
  InternetQueryDataAvailable(hRequest, &dwNumberOfBytesAvailable, 0, 0);
  printf("%d", dwNumberOfBytesAvailable);
  char *buf = (char *)malloc(sizeof(char) * dwNumberOfBytesAvailable);
  DWORD dwNumberOfBytesRead = 0;
  CHAR szBuffer[1025];
  DWORD dwRead = 0;
  HANDLE hFile = CreateFile(
							"feed.txt",
							GENERIC_READ | GENERIC_WRITE,
							0,
							NULL,
							CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL ,
							NULL);
  while (InternetReadFile(hRequest, szBuffer, sizeof(szBuffer) - 1, &dwRead) &&
		 dwRead) {
	  szBuffer[dwRead] = 0;
	  //printf("%s", szBuffer);
	  DWORD bytesWrite = 0;
	  WriteFile(
				hFile,
				szBuffer,
				dwRead,
				&bytesWrite,
				NULL);

	  //OutputDebugStringA(szBuffer);
	  dwRead = 0;
  }
  free(buf);
  printf("%s", buf);

  // TODO close all internet handle
  BOOL closeResult = InternetCloseHandle(hInternet);
  return 0;
}
