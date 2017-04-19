// Win32 Tutorial (List Boxes)

// Alan Baylis 2004

#include <Windows.h>
#include "tinyxml2.h"
//#include "resource.h"

const TCHAR ClassName[] = L"MainWindowClass";
HWND hWndListBox;
#define IDC_LISTBOX_TEXT 1000
#define IDI_ICON         101
#define EZ_ATTR_BOLD          1
#define EZ_ATTR_ITALIC        2
#define EZ_ATTR_UNDERLINE     4
#define EZ_ATTR_STRIKEOUT     8

HFONT EzCreateFont (HDC hdc, TCHAR * szFaceName, int iDeciPtHeight,
                    int iDeciPtWidth, int iAttributes, BOOL fLogRes)
{
     FLOAT      cxDpi, cyDpi ;
     HFONT      hFont ;
     LOGFONT    lf ;
     POINT      pt ;
     TEXTMETRIC tm ;
     
     SaveDC (hdc) ;
     
     SetGraphicsMode (hdc, GM_ADVANCED) ;
     ModifyWorldTransform (hdc, NULL, MWT_IDENTITY) ;
     SetViewportOrgEx (hdc, 0, 0, NULL) ;
     SetWindowOrgEx   (hdc, 0, 0, NULL) ;
     
     if (fLogRes)
     {
          cxDpi = (FLOAT) GetDeviceCaps (hdc, LOGPIXELSX) ;
          cyDpi = (FLOAT) GetDeviceCaps (hdc, LOGPIXELSY) ;
     }
     else
     {
          cxDpi = (FLOAT) (25.4 * GetDeviceCaps (hdc, HORZRES) /
                                        GetDeviceCaps (hdc, HORZSIZE)) ;
          
          cyDpi = (FLOAT) (25.4 * GetDeviceCaps (hdc, VERTRES) /
                                        GetDeviceCaps (hdc, VERTSIZE)) ;
     }
     
     pt.x = (int) (iDeciPtWidth  * cxDpi / 72) ;
     pt.y = (int) (iDeciPtHeight * cyDpi / 72) ;
     
     DPtoLP (hdc, &pt, 1) ;
     
     lf.lfHeight         = - (int) (fabs (pt.y) / 10.0 + 0.5) ;
     lf.lfWidth          = 0 ;
     lf.lfEscapement     = 0 ;
     lf.lfOrientation    = 0 ;
     lf.lfWeight         = iAttributes & EZ_ATTR_BOLD      ? 700 : 0 ;
     lf.lfItalic         = iAttributes & EZ_ATTR_ITALIC    ?   1 : 0 ;
     lf.lfUnderline      = iAttributes & EZ_ATTR_UNDERLINE ?   1 : 0 ;
     lf.lfStrikeOut      = iAttributes & EZ_ATTR_STRIKEOUT ?   1 : 0 ;
     lf.lfCharSet        = DEFAULT_CHARSET ;
     lf.lfOutPrecision   = 0 ;
     lf.lfClipPrecision  = 0 ;
     lf.lfQuality        = 0 ;
     lf.lfPitchAndFamily = 0 ;
     
     lstrcpy (lf.lfFaceName, szFaceName) ;
     
     hFont = CreateFontIndirect (&lf) ;
     
     if (iDeciPtWidth != 0)
     {
          hFont = (HFONT) SelectObject (hdc, hFont) ;
          
          GetTextMetrics (hdc, &tm) ;
          
          DeleteObject (SelectObject (hdc, hFont)) ;
          
          lf.lfWidth = (int) (tm.tmAveCharWidth *
                                        fabs (pt.x) / fabs (pt.y) + 0.5) ;
          
          hFont = CreateFontIndirect (&lf) ;
     }
     
     RestoreDC (hdc, -1) ;
     return hFont ;
}

LRESULT CALLBACK WndProc( HWND    hWnd,
        UINT    Msg,
        WPARAM  wParam,          
        LPARAM  lParam )
{
    switch (Msg)
    {
        case WM_CREATE:
            {
                hWndListBox = CreateWindow(
                        L"LISTBOX", 
                        NULL, 
                        WS_VISIBLE | WS_CHILD | LBS_STANDARD | LBS_NOTIFY | LBS_DISABLENOSCROLL | WS_VSCROLL, 
                        0, 
                        0, 
                        800, 
                        600, 
                        hWnd, 
                        (HMENU)1000, 
                        (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), 
                        NULL);
                HDC hdc = GetDC(hWnd);
                HFONT hFont = EzCreateFont (hdc, TEXT ("YouYuan"), 
                        120, 0, 0, TRUE) ;
                SendMessage(hWndListBox, WM_SETFONT, (WPARAM)hFont, 0);

                if (!hWnd)
                    MessageBox(NULL, L"ListBox Failed.", L"Error", MB_OK | MB_ICONERROR);

                tinyxml2::XMLDocument doc;
                doc.LoadFile("www.u148.net");
                tinyxml2::XMLElement *ele = doc.FirstChildElement("rss")->FirstChildElement("channel");
                for(tinyxml2::XMLElement *childElement = ele->FirstChildElement();
                        childElement != NULL;
                        childElement = childElement->NextSiblingElement())
                {
                    if(strcmp(childElement->Name(), "item") == 0)
                    {
                        tinyxml2::XMLElement *title = childElement->FirstChildElement("title");
                        const char* wstr = title->GetText();
                        //获取输入缓存大小
                        int sBufSize=strlen(wstr);
                        //获取输出缓存大小
                        DWORD dBufSize=MultiByteToWideChar(CP_UTF8, 0, wstr, -1, 0, 0);

                        wchar_t * dBuf=new wchar_t[dBufSize];
                        memset(dBuf, 0, dBufSize * sizeof(*dBuf));

                        //进行转换
                        int nRet=MultiByteToWideChar(CP_UTF8, 0, wstr, sBufSize, dBuf, dBufSize);
                        SendMessage(GetDlgItem(hWnd, IDC_LISTBOX_TEXT), LB_ADDSTRING, 0, (LPARAM)dBuf);
                        delete[] dBuf;
                    }
                }
            }
            break; 
        case WM_COMMAND: 
            {
                switch(LOWORD(wParam))
                {
                    case IDC_LISTBOX_TEXT:
                        {
                            switch(HIWORD(wParam))
                            {
                                case LBN_SELCHANGE:
                                    {
                                        TCHAR Buffer[256];
                                        int index = SendMessage((HWND)lParam, LB_GETCARETINDEX, 0, 0);
                                        SendMessage((HWND)lParam, LB_GETTEXT, (LPARAM)index, (WPARAM)Buffer);
                                        SetWindowText(hWnd, Buffer); 
                                    }
                            }
                        }
                        break;
                }
                return 0;
            } 
            break; 
        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return (DefWindowProc(hWnd, Msg, wParam, lParam));
    }
    return 0;
}



INT WINAPI WinMain( HINSTANCE  hInstance,
        HINSTANCE  hPrevInstance,
        LPSTR      lpCmdLine,
        INT        nCmdShow )

{
    WNDCLASSEX    wc;
    wc.cbSize           = sizeof(WNDCLASSEX);
    wc.style            = 0;
    wc.lpfnWndProc      = (WNDPROC)WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = hInstance;
    wc.hIcon            = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
    wc.hIconSm          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = ClassName;
    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, L"Failed To Register The Window Class.", L"Error", MB_OK | MB_ICONERROR);
        return 0;
    }
    HWND    hWnd;
    hWnd = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            ClassName,
            L"List Boxes",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            800,
            600,
            NULL,
            NULL,
            hInstance,
            NULL);


    if (!hWnd)

    {
        MessageBox(NULL, L"Window Creation Failed.", L"Error", MB_OK | MB_ICONERROR);
        return 0;
    }
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);
    MSG    Msg;

    while (GetMessage(&Msg, NULL, 0, 0))
    {

        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;

}
