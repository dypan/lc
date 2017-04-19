
#include "cwebpage.h"
#include "tinyxml2.h"
#include "macros.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <Windows.h>


HWND	g_hrightwnd	= NULL;
HWND	g_hleftwnd	= NULL;


HWND	hText		= NULL;
extern	HINSTANCE	hInst;
extern	HWND		g_hmain;

typedef struct feed_{
    TCHAR *content;
} feed;

feed data[100];


extern INT		main_Wnd_clr[];
extern INT		left_Wnd_clr[]; 
extern INT		right_Wnd_clr[];
#define IDC_LISTBOX_TEXT 1000
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

void CreateListBox(HWND parent)
{
    HWND hWnd = parent;
    HWND hWndListBox = CreateWindow(
            L"LISTBOX", 
            NULL, 
            WS_VISIBLE | WS_CHILD | LBS_STANDARD | LBS_NOTIFY | LBS_DISABLENOSCROLL | WS_VSCROLL, 
            0, 
            0, 
            800, 
            600, 
            hWnd, 
            (HMENU)IDC_LISTBOX_TEXT, 
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
    int index = 0;
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

            tinyxml2::XMLElement *description = childElement->FirstChildElement("description");
            const char* descriptionStr = description->GetText();
            sBufSize = strlen(descriptionStr);
            dBufSize = MultiByteToWideChar(CP_UTF8, 0, descriptionStr, -1, 0, 0);
            dBuf = new wchar_t[dBufSize];
            memset(dBuf, 0, dBufSize * sizeof(*dBuf));
            nRet=MultiByteToWideChar(CP_UTF8, 0, descriptionStr, sBufSize, dBuf, dBufSize);
            feed f;
            f.content = dBuf;
            data[index++] = f;
        }
    }
}


// Function to handle the messages releated to popup window
INT_PTR CALLBACK PopUpProc(HWND , UINT, WPARAM, LPARAM);

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	PAINTSTRUCT ps;
	HDC hdc;
	static HBRUSH	hmain_Wnd_brush	= NULL;
	static HWND		hclose_button	= NULL;
	static HWND		hcolor_button	= NULL;
	static HCURSOR	hcSizeEW		= NULL;
	static HWND	focus = NULL;

	// Varibles used to calculate the width and height of the 
    // left window while increased or decreased using splitter
    static  int         nleftWnd_width = 0;

	 // Variable which informs whether the splitter is dragged
    // or not
    static  BOOL        xSizing;
	static UINT			nwnd_resizing = CLEAR;

	static HBRUSH	hBrush;
	static HICON	hIcon;

	switch (message)
	{
	case WM_CREATE:

		hcSizeEW = LoadCursor(NULL, IDC_SIZEWE);

		nleftWnd_width = LEFT_WINDOW_WIDTH;		

		GetClientRect(hWnd, &rect);
		// Creates the left window using the width and height read from the XML 
		// files
		g_hleftwnd = CreateWindowEx(WS_EX_CLIENTEDGE, LEFT_WINDOW_CLASS, 
										L"", 
										WS_CHILD | WS_VISIBLE | SS_SUNKEN,
										rect.left, 
										rect.top + TOP_POS,
										LEFT_WINDOW_WIDTH, 
										(rect.bottom - rect.top) - (TOP_POS + BOTTOM_POS),
										hWnd, NULL, hInst, NULL);
        CreateListBox(g_hleftwnd);
		if(NULL != g_hleftwnd)
		{
			ShowWindow(g_hleftwnd, SW_SHOW);
			UpdateWindow(g_hleftwnd);
		}

		//hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));

		if (hIcon != NULL)
		{
				SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		}

		// Creates the right window using the width and height read from the XML
		// files
		g_hrightwnd = CreateWindowEx(WS_EX_CLIENTEDGE, RIGHT_WINDOW_CLASS,
									 L"", 
									 WS_CHILD | WS_VISIBLE | SS_SUNKEN,
									 rect.left + LEFT_WINDOW_WIDTH + SPLITTER_BAR_WIDTH,
									 rect.top + TOP_POS,
									 rect.right - (rect.left + LEFT_WINDOW_WIDTH + SPLITTER_BAR_WIDTH),
									 (rect.bottom - rect.top) - (TOP_POS + BOTTOM_POS),
									 hWnd,
									 NULL, hInst, NULL);

		if(NULL != g_hrightwnd)
		{
			ShowWindow(g_hrightwnd, SW_SHOW);
			UpdateWindow(g_hrightwnd);
		}

		
		hText = CreateWindow(L"Static", L"", 
							WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE | SS_ENDELLIPSIS, 
							rect.left, rect.top, rect.right, STATIC_TEXT_HEIGHT, 
							hWnd, (HMENU)NULL, hInst, NULL);
#if 0
		//Create the buttons in the main window
		hclose_button = CreateWindow(BUTTON_CLASS, "",
                                    WS_CHILD | WS_VISIBLE | 
                                    WS_TABSTOP | BS_PUSHBUTTON,
									rect.right - SPACE_BUTTON_RIGHT, 
									rect.bottom - SPACE_BUTTON_BOTTOM,
                                    CLOSE_BUTTON_WIDTH,
                                    BUTTON_HEIGHT,  
                                    hWnd, NULL,hInst, NULL);
		if(NULL != hclose_button)
		{
			SetWindowText(hclose_button, "Close");
			ShowWindow(hclose_button, SW_SHOW);
			UpdateWindow(hclose_button);
		}

		hcolor_button = CreateWindow(BUTTON_CLASS, "",
                                    WS_CHILD | WS_VISIBLE | 
                                    WS_TABSTOP | BS_PUSHBUTTON,
									rect.right - (SPACE_BUTTON_RIGHT + COLOR_BUTTON_WIDTH + BUTTON_ADJUST), 
									rect.bottom - SPACE_BUTTON_BOTTOM,
                                    COLOR_BUTTON_WIDTH,
                                    BUTTON_HEIGHT,  
                                    hWnd, NULL,hInst, NULL);
		if(NULL != hcolor_button)
		{
				SetWindowText(hcolor_button, "Click To Change Window Color");
				ShowWindow(hcolor_button, SW_SHOW);
				UpdateWindow(hcolor_button);

				SendMessage(hcolor_button, WM_SETFOCUS, 0, 0);
				focus = hcolor_button;
		}
#endif

		return FALSE;
		break;


	case WM_CTLCOLORSTATIC :
		
			//Set the text color inverse of the window color
			SetTextColor((HDC)wParam, RGB(255 - main_Wnd_clr[0], 
										255 - main_Wnd_clr[1],
										255 - main_Wnd_clr[2]));

			SetBkColor((HDC)wParam, RGB(main_Wnd_clr[0], 
									main_Wnd_clr[1],
									main_Wnd_clr[2]));

			if (hBrush != NULL)
			{
					DeleteObject(hBrush);
			}
			hBrush = CreateSolidBrush(RGB(main_Wnd_clr[0], 
										main_Wnd_clr[1],
										main_Wnd_clr[2]));
			return (BOOL)hBrush;
		
		break;


	case WM_SIZE:

		GetClientRect(hWnd, &rect);
		  
		// Call Api to move and adjust the left window postion and its
        // height and width

        MoveWindow(g_hleftwnd, rect.left,
				   rect.top + TOP_POS,
                   rect.left + (nleftWnd_width - WIDTH_ADJUST), 
				   (rect.bottom - (TOP_POS + BOTTOM_POS)),
                   FALSE);
        
        // Call Api to move and adjust the right window postion and its
        // height and width
        MoveWindow(g_hrightwnd, rect.left + nleftWnd_width + WIDTH_ADJUST, 
                      rect.top + TOP_POS,
                      rect.right - (nleftWnd_width + WIDTH_ADJUST),
                      rect.bottom - (TOP_POS + BOTTOM_POS),
                      FALSE);	

		MoveWindow(hclose_button, rect.right - SPACE_BUTTON_RIGHT,
				   rect.bottom - SPACE_BUTTON_BOTTOM, 
				   CLOSE_BUTTON_WIDTH, BUTTON_HEIGHT, FALSE);


		MoveWindow(hcolor_button, 
					rect.right - (SPACE_BUTTON_RIGHT + COLOR_BUTTON_WIDTH + BUTTON_ADJUST), 
					rect.bottom - SPACE_BUTTON_BOTTOM, 
					COLOR_BUTTON_WIDTH, BUTTON_HEIGHT, FALSE);


		MoveWindow(hText, rect.left, rect.top, rect.right, STATIC_TEXT_HEIGHT, FALSE);

		InvalidateRect(hWnd, &rect, TRUE);

		break;

	case WM_COMMAND:
        
		break;

	case WM_KEYDOWN :

		switch(wParam)
		{
			case VK_TAB :
				if (focus == hcolor_button) 
				{					
					SendMessage(focus, WM_KILLFOCUS, 0, 0);
					SendMessage(hclose_button, WM_SETFOCUS, 0, 0);

					focus = hclose_button;
				}
				else if (focus == hclose_button)
				{
					SendMessage(focus, WM_KILLFOCUS, 0, 0);
					SendMessage(hcolor_button, WM_SETFOCUS, 0, 0);


					focus = hcolor_button;
				}


			break;
			
			case VK_RETURN:

				if (IsWindowEnabled(focus))
					SendMessage(hWnd, WM_COMMAND, 0, (LPARAM)focus);

			break;	
		}
		break;


	case WM_PAINT:

		//SetWindowText(hText, "To change the color of the window click on \"Click To Change Window Color\" button");

		hdc = BeginPaint(hWnd, &ps);

		// Painting 
		GetClientRect(hWnd, &rect);
		
		hmain_Wnd_brush = CreateSolidBrush(RGB(main_Wnd_clr[0], 
												   main_Wnd_clr[1],
												   main_Wnd_clr[2]));

		FillRect(hdc, &rect, hmain_Wnd_brush);

		DeleteObject(hmain_Wnd_brush);

		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);

		break;

	// Case statement to handle the left mouse button down message
    // received while the mouse left button is down
   case WM_LBUTTONDOWN :
        {
            int                 xPos;
            int                 yPos;
            
            // Varible used to get the mouse cursor x and y co-ordinates
            xPos = (int)LOWORD(lParam);
            yPos = (int)HIWORD(lParam);

            // Checks whether the mouse is over the splitter window
            xSizing = (xPos > nleftWnd_width - SPLITTER_BAR_WIDTH && xPos < nleftWnd_width + SPLITTER_BAR_WIDTH );

            // If the mouse is over the splitter then set mouse cursor 
            // image to IDC_SIZEWE which helps the user to drag the window
            if(xSizing)
            {
                // Api to capture mouse input
                SetCapture(hWnd);
                if(xSizing)
                {
                    SetCursor(hcSizeEW);
                }
             }
        }
     break;

    case WM_LBUTTONUP :
        if(xSizing)
        {
            RECT    focusrect;
            HDC     hdc;

            // Releases the captured mouse input
            ReleaseCapture();
            // Get the main window dc to draw a focus rectangle
            hdc = GetDC(hWnd);
            GetClientRect(hWnd, &rect);
            if(xSizing)
            {
                SetRect(&focusrect, nleftWnd_width - (WIDTH_ADJUST * 2), rect.top + TOP_POS, 
					    nleftWnd_width + WIDTH_ADJUST, 
						rect.bottom - 80);

                // Call api to vanish the dragging rectangle 
                DrawFocusRect(hdc, &focusrect);
				
				xSizing = FALSE;

            }
            // Release the dc once done 
            ReleaseDC(hWnd, hdc);
        }
        // Post a WM_SIZE message to redraw the windows
        PostMessage(hWnd, WM_SIZE, 0, 0);

    break;

    case WM_MOUSEMOVE :
    {
		int   xPos;
		int   yPos;

		// Get the x and y co-ordinates of the mouse
		xPos = (int)LOWORD(lParam);
		yPos = (int)HIWORD(lParam);

		if (xPos < LEFT_MINIMUM_SPACE || xPos > RIGHT_MINIMUM_SPACE)
		{
			return 0;
		}

        // Checks if the left button is pressed during dragging the splitter
        if(wParam == MK_LBUTTON)   
        {
            // If the window is d`agged using the splitter, get the
            // cursors current postion and draws a focus rectangle 
            if(xSizing)
            {
                RECT    focusrect;
                HDC     hdc;

                hdc = GetDC(hWnd);
                GetClientRect(hWnd, &rect);

                if(xSizing)
                {
                   SetRect(&focusrect, nleftWnd_width - (WIDTH_ADJUST * 2), rect.top + TOP_POS,
                               nleftWnd_width +  WIDTH_ADJUST, 
							   rect.bottom - BOTTOM_POS);

                   DrawFocusRect(hdc, &focusrect);

                   // Get the size of the left window to increase
                   nleftWnd_width = xPos;

                   // Draws a focus rectangle
                   SetRect(&focusrect, nleftWnd_width - (SPLITTER_BAR_WIDTH * 2), rect.top + TOP_POS,
                              nleftWnd_width + SPLITTER_BAR_WIDTH, 
							  rect.bottom - BOTTOM_POS);

                   DrawFocusRect(hdc, &focusrect);

                }
                ReleaseDC(hWnd, hdc);
            }
        }
        // Set the cursor image to east west direction when the mouse is over 
        // the splitter window
        if( (xPos > nleftWnd_width - SPLITTER_BAR_WIDTH && xPos < nleftWnd_width + 
               SPLITTER_BAR_WIDTH))
        {
             SetCursor(hcSizeEW); 
        }
    }
    break;

	case WM_DESTROY:

		if(NULL != hmain_Wnd_brush)
		{
			DeleteObject(hmain_Wnd_brush);
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



LRESULT CALLBACK LeftWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;
	static HBRUSH	hleft_Wnd_brush = NULL;
	
	switch (message)
	{

	case WM_CREATE :

		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		GetClientRect(hWnd, &rect);

		//Paint the window with the color specified by the user
		hleft_Wnd_brush = CreateSolidBrush(RGB(left_Wnd_clr[0], 
												   left_Wnd_clr[1],
												   left_Wnd_clr[2]));

		FillRect(hdc, &rect, hleft_Wnd_brush);

		DeleteObject(hleft_Wnd_brush);
		
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
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
                                SetWindowText(g_hmain, Buffer); 
                                DisplayHTMLStr(g_hrightwnd, data[index].content);
                            }
                    }
                }
                break;
        }
    }

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK RightWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;
	static HBRUSH	hright_Wnd_brush = NULL;
	switch (message)
	{
	
	case WM_CREATE:
        if (OleInitialize(NULL) != S_OK)
        {
            MessageBox(0, L"Can't open OLE!", L"ERROR", MB_OK);
        }
        EmbedBrowserObject(hWnd);

		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		GetClientRect(hWnd, &rect);

		//Paint the window with the color specified by the user
		hright_Wnd_brush = CreateSolidBrush(RGB(right_Wnd_clr[0], 
													right_Wnd_clr[1],
													right_Wnd_clr[2]));

		FillRect(hdc, &rect, hright_Wnd_brush);

		DeleteObject(hright_Wnd_brush);

		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


INT_PTR CALLBACK PopUpProc(HWND hpopup, 
                           UINT message, 
                           WPARAM wParam, 
                           LPARAM lParam)
{
	WORD wmId;
	RECT rect;
	static HWND hComboBox[9];
	char str[5];
	static int j;

    switch(message)
    {
        case WM_INITDIALOG:

			SetWindowText(hText, L"Changing the RGB values will change the color of the windows");

			UINT iLoop, i;

			//Create the handler for all the combo boxes
			for(i = 0; i < 9; i++)
			{
				//hComboBox[i] = GetDlgItem(hpopup, IDC_COMBO1 + i);
			}

			//Set the range for the combo boxes(in this case 0 - 255)
			if(NULL != hComboBox)
			{
				for(i = 0 ; i < 9 ; i++)
				{
					for (iLoop = 0; iLoop < 256; iLoop++)
					{
					  sprintf(str, "%d", iLoop);
					  SendMessage(hComboBox[i], CB_ADDSTRING, 0, (LPARAM) str); 
					}
				}
			}

			//Set the combo box text with the previous value for window and child windows
			for (i = 0, j = 0; i < 3; i++)
			{
					sprintf(str, "%d", main_Wnd_clr[i]);
					SendMessage(hComboBox[j++], WM_SETTEXT, 0, (LPARAM) str); 
			}

			for (i = 0; i < 3; i++)
			{
					sprintf(str, "%d", left_Wnd_clr[i]);
					SendMessage(hComboBox[j++], WM_SETTEXT, 0, (LPARAM) str); 
			}

			for (i = 0; i < 3; i++)
			{
					sprintf(str, "%d", right_Wnd_clr[i]);
					SendMessage(hComboBox[j++], WM_SETTEXT, 0, (LPARAM) str); 
			}


			//SendMessage(GetDlgItem(hpopup, IDC_COMBO1), WM_SETFOCUS, 0, 0);

			ShowWindow(hpopup, SW_SHOW);
			UpdateWindow(hpopup);
			break;


		case WM_COMMAND:
			break;

		case WM_CLOSE :
		case WM_DESTROY :
					EndDialog(hpopup, 0);

			break;

	}

	return FALSE;
}
