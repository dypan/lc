#include <Windows.h>
#include "cwebpage.h"
#include "tinyxml2.h"

/****************************** WindowProc() ***************************
 * Our message handler for our window to host the browser.
 */
DWORD WindowCount = 0;
static const TCHAR	ClassName[] = L"Browser Example";
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CREATE)
	{
		// Embed the browser object into our host window. We need do this only
		// once. Note that the browser object will start calling some of our
		// IOleInPlaceFrame and IOleClientSite functions as soon as we start
		// calling browser object functions in EmbedBrowserObject().
		if (EmbedBrowserObject(hwnd)) return(-1);

		// Another window created with an embedded browser object
		++WindowCount;

		// Success
		return(0);
	}

	if (uMsg == WM_DESTROY)
	{
		// Detach the browser object from this window, and free resources.
		UnEmbedBrowserObject(hwnd);

		// One less window
		--WindowCount;

		// If all the windows are now closed, quit this app
		if (!WindowCount) PostQuitMessage(0);

		return(TRUE);
	}

	// NOTE: If you want to resize the area that the browser object occupies when you
	// resize the window, then handle WM_SIZE and use the IWebBrowser2's put_Width()
	// and put_Height() to give it the new dimensions.

	return(DefWindowProc(hwnd, uMsg, wParam, lParam));
}





/****************************** WinMain() ***************************
 * C program entry point.
 *
 * This creates a window to host the web browser, and displays a web
 * page.
 */

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hInstNULL, LPSTR lpszCmdLine, int nCmdShow)
{
	MSG			msg;

	// Initialize the OLE interface. We do this once-only.
	if (OleInitialize(NULL) == S_OK)
	{
		WNDCLASSEX		wc;

		// Register the class of our window to host the browser. 'WindowProc' is our message handler
		// and 'ClassName' is the class name. You can choose any class name you want.
		ZeroMemory(&wc, sizeof(WNDCLASSEX));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.hInstance = hInstance;
		wc.lpfnWndProc = WindowProc;
		wc.lpszClassName = &ClassName[0];
		RegisterClassEx(&wc);

		// Create a window. NOTE: We embed the browser object duing our WM_CREATE handling for
		// this window.
		if ((msg.hwnd = CreateWindowEx(0, &ClassName[0], L"An HTML string", WS_OVERLAPPEDWINDOW,
							CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
							HWND_DESKTOP, NULL, hInstance, 0)))
		{
			// For this window, display a string in the BODY of a web page.
            
            tinyxml2::XMLDocument doc;
            doc.LoadFile("www.u148.net");
            tinyxml2::XMLElement *ele = doc.FirstChildElement("rss")->FirstChildElement("channel")->FirstChildElement("item")->FirstChildElement("description");

            const char* wstr = ele->GetText();
            //获取输入缓存大小
            int sBufSize=strlen(wstr);
            //获取输出缓存大小
            //VC++ 默认使用ANSI，故取第一个参数为CP_ACP
            DWORD dBufSize=MultiByteToWideChar(CP_UTF8, 0, wstr, sBufSize, NULL, 0);

            wchar_t * dBuf=new wchar_t[dBufSize];
            //ZeroMemory(dBuf, dBufSize);
            memset(dBuf, 0, dBufSize * sizeof(*dBuf));

            //进行转换
            int nRet=MultiByteToWideChar(CP_UTF8, 0, wstr, sBufSize, dBuf, dBufSize);
             
			//DisplayHTMLStr(msg.hwnd, "<H2><CENTER>HTML string test</CENTER></H2><P><FONT COLOR=RED>This is a <U>HTML string</U> in memory.</FONT>");
            DisplayHTMLStr(msg.hwnd, dBuf);

			// Show the window.
			ShowWindow(msg.hwnd, nCmdShow);
			UpdateWindow(msg.hwnd);
		}

		// Do a message loop until WM_QUIT.
		while (GetMessage(&msg, 0, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Free the OLE library.
		OleUninitialize();

		return(0);
	}

	MessageBox(0, L"Can't open OLE!", L"ERROR", MB_OK);
	return(-1);
}

