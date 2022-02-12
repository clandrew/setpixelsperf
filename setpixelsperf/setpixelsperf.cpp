// setpixelsperf.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "framework.h"
#include "setpixelsperf.h"
#include "MemoryBitmap.h"
#include "Timer.h"
#include "Mode.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

int g_width = 0;
int g_height = 0;
Mode g_mode = Mode::SetDIBits;
bool g_outputLog = true;

Timer g_timer;
MemoryBitmap g_memoryBitmap;
bool g_readyToTakeMeasurements = false;

UINT g_colors[] = { 0xFF00FF, 0x00FF00, 0xFF0000, 0x0000FF};
int g_colorIndex;

FILE* g_logFile;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Options(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    g_mode = Mode::SetDIBits;
    g_timer.Initialize();

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SETPIXELSPERF, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SETPIXELSPERF));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SETPIXELSPERF));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SETPIXELSPERF);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
void InitializeBenchmark(HWND hWnd)
{
    DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, Options);

    RECT clientRect{};
    clientRect.right = g_width;
    clientRect.bottom = g_height;
    AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, TRUE);

    if (clientRect.left < 0)
    {
        clientRect.right -= clientRect.left;
        clientRect.left = 0;
    }
    if (clientRect.top < 0)
    {
        clientRect.bottom -= clientRect.top;
        clientRect.top = 0;
    }

    MoveWindow(hWnd, clientRect.left, clientRect.top, clientRect.right, clientRect.bottom, FALSE);

    g_memoryBitmap.Initialize(g_width, g_height, hWnd);

    g_readyToTakeMeasurements = true;
    InvalidateRect(hWnd, nullptr, FALSE);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 1000, 1000, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   InitializeBenchmark(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case ID_FILE_RUNBENCHMARK:
                InitializeBenchmark(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            if (g_readyToTakeMeasurements)
            {
                g_timer.Start();

                if (g_mode == Mode::SetPixel)
                {
                    for (int y = 0; y < g_height; ++y)
                    {
                        for (int x = 0; x < g_width; ++x)
                        {
                            SetPixel(hdc, x, y, g_colors[g_colorIndex]);
                        }
                    }
                }
                else
                {
                    assert(g_mode == Mode::SetDIBits);

                    for (int y = 0; y < g_height; ++y)
                    {
                        for (int x = 0; x < g_width; ++x)
                        {
                            g_memoryBitmap.SetPixel(x, y, g_colors[g_colorIndex]);
                        }
                    }
                    g_memoryBitmap.Commit();

                    BitBlt(hdc, 0, 0, g_width, g_height, g_memoryBitmap.m_hdc, 0, 0, SRCCOPY);
                }

                g_timer.Stop();
            }

            EndPaint(hWnd, &ps);

            if (g_readyToTakeMeasurements)
            {
                std::wstring briefReport = g_timer.GetBriefReport();
                MessageBox(hWnd, briefReport.c_str(), L"Result", MB_OK);

                if (g_outputLog)
                {
                    std::wfstream strm(L"log.txt", std::ios::app);

                    std::wstring fullReport = g_timer.GetFullReport(g_width, g_height, g_mode);

                    strm << fullReport;
                    OutputDebugString(fullReport.c_str());
                }

                g_colorIndex = (g_colorIndex + 1) % 4;
                g_readyToTakeMeasurements = false;
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Options(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        HWND hControl = GetDlgItem(hDlg, IDC_TARGETWIDTH);
        SetWindowText(hControl, L"1000");

        hControl = GetDlgItem(hDlg, IDC_TARGETHEIGHT);
        SetWindowText(hControl, L"1000");

        hControl = GetDlgItem(hDlg, IDC_MODE);
        std::wstring s = L"SetPixel";
        SendMessage(hControl, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)s.c_str());
        s = L"SetDIBits";
        SendMessage(hControl, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)s.c_str());

        SendMessage(hControl, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
    }
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            int width, height;
            Mode newMode;
            {
                HWND hControl = GetDlgItem(hDlg, IDC_TARGETWIDTH);
                wchar_t buffer[255]{};
                GetWindowText(hControl, buffer, 255);
                width = _wtoi(buffer);
            }
            {
                HWND hControl = GetDlgItem(hDlg, IDC_TARGETHEIGHT);
                wchar_t buffer[255]{};
                GetWindowText(hControl, buffer, 255);
                height = _wtoi(buffer);
            }
            {
                HWND hControl = GetDlgItem(hDlg, IDC_MODE);
                wchar_t buffer[255]{};
                GetWindowText(hControl, buffer, 255);
                if (wcscmp(L"SetDIBits", buffer) == 0)
                {
                    newMode = Mode::SetDIBits;
                }
                else if (wcscmp(L"SetPixel", buffer) == 0)
                {
                    newMode = Mode::SetPixel;
                }
            }
            g_width = width;
            g_height = height;
            g_mode = newMode;

            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
