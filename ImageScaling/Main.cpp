#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <string>
#include <CommCtrl.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef int(__cdecl* MYPROC)(LPWSTR);

//_g stands for global variables

HWND g_hwndButtonDLL;
HWND g_hwndTextDLL;

HWND g_hwndTextThreads;
HWND g_hwndTrackbarThreads;

HWND g_hwndTextFile;
HWND g_hwndEnterTextFile;
HWND g_hwndButtonFile;

HWND g_hwndTextScaleWidth;
HWND g_hwndEnterTextScaleWidth;
HWND g_hwndTextScaleHeight;
HWND g_hwndEnterTextScaleHeight;
HWND g_hwndButtonScale;

enum class TypeOfDLL {ASM, C};
TypeOfDLL g_currentDLL = TypeOfDLL::ASM;

int g_numberOfThreads = 1;

std::wstring g_imageFile;

void OnSize(HWND hwnd, UINT flag, int width, int height)
{
    // Handle resizing
}

//function from https://xoax.net/cpp/crs/win32/lessons/Lesson9/
bool LoadAndBlitBitmap(LPCWSTR szFileName, HDC hWinDC)
{
    //Load the bitmap image file
    HBITMAP hBitmap;
    hBitmap = (HBITMAP)::LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    //Verify that image was loaded
    if (hBitmap == NULL)
    {
        ::MessageBox(NULL, L"Nie uda³o siê za³adowaæ obrazu", L"Error", MB_OK);
        return false;
    }

    //Create a device context that is compatible with the window
    HDC hLocalDC;
    hLocalDC = ::CreateCompatibleDC(hWinDC);
    //Verift that the device context was created
    if (hLocalDC == NULL)
    {
        ::MessageBox(NULL, L"Urz¹dzenie nie jest kompatybilne", L"Error", MB_OK);
        return false;
    }

    BITMAP qBitmap;
    int iReturn = GetObject(reinterpret_cast<HGDIOBJ>(hBitmap), sizeof(BITMAP), reinterpret_cast<LPVOID>(&qBitmap));
    if (!iReturn)
    {
        ::MessageBox(NULL, L"Nie uda³o siê za³adowaæ obrazu", L"Error", MB_OK);
        return false;
    }

    //Select the loadem bitmap into the device context
    HBITMAP hOldBmp = (HBITMAP)::SelectObject(hLocalDC, hBitmap);
    if (hOldBmp == NULL)
    {
        ::MessageBox(NULL, L"Nie uda³o siê za³adowaæ obrazu", L"Error", MB_OK);
        return false;
    }

    //Blit the dc which holds the bitmap onto the window's dc
    BOOL qRetBlit = ::BitBlt(hWinDC, 500, 0, qBitmap.bmWidth, qBitmap.bmHeight, hLocalDC, 0, 0, SRCCOPY);
    if (!qRetBlit)
    {
        ::MessageBox(NULL, L"Nie uda³o siê za³adowaæ obrazu", L"Error", MB_OK);
        return false;
    }

    //Unitialize and deallocate resources
    ::SelectObject(hLocalDC, hOldBmp);
    ::DeleteDC(hLocalDC);
    ::DeleteObject(hBitmap);
    return true;
}

void scaleImage()
{
    HINSTANCE hinstLib;
    MYPROC ProcAdd;
    BOOL fFreeResult, fRunTimeLinkSuccess = FALSE;

    // Get a handle to the DLL module.

    hinstLib = LoadLibrary(L"../x64/Debug/ImageScalingLibC.dll");

    // If the handle is valid, try to get the function address.
    //LPFNDLLFUNC lpfnDllFunc1;
    if (hinstLib != NULL)
    {
        ProcAdd = (MYPROC)GetProcAddress(hinstLib, "scaleImage2");

        // If the function address is valid, call the function.

        if (NULL != ProcAdd)
        {
            fRunTimeLinkSuccess = TRUE;
            (ProcAdd)((LPWSTR)L"lala\n");
        }
        // Free the DLL module.

        fFreeResult = FreeLibrary(hinstLib);
    }

    // If unable to call the DLL function, use an alternative.
    if (!fRunTimeLinkSuccess)
        ::MessageBox(NULL, L"Nie uda³o siê za³adowaæ DLL", L"Error", MB_OK);
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance,_In_opt_  HINSTANCE hPrevInstance,_In_  PWSTR pCmdLine,_In_  int nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"AppWindow";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0x00000010L,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Skalowanie Obrazu",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    g_hwndButtonDLL = CreateWindowEx(
        0,
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"zmieñ",      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        250,         // x position 
        0,         // y position 
        50,        // Button width
        20,        // Button height
        hwnd,     // Parent window
        NULL,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL        // Pointer not needed.
    );      

    g_hwndTextDLL = CreateWindowEx(
        0,
        L"STATIC",  // Predefined class; Unicode assumed 
        NULL, 
        WS_CHILD | WS_VISIBLE | WS_BORDER, 
        0, 
        0, 
        250, 
        20,
        hwnd, 
        NULL, 
        hInstance, 
        NULL
    );
    SetWindowText(g_hwndTextDLL, L"Aktualnie u¿ywana DLL: ASM");

    g_hwndTextThreads = CreateWindowEx(
        0,
        L"STATIC",  // Predefined class; Unicode assumed 
        NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0,
        40,
        250,
        20,
        hwnd,
        NULL,
        hInstance,
        NULL
    );
    std::wstring numberOfThreads = L"Liczba w¹tków: " + std::to_wstring(g_numberOfThreads);
    SetWindowText(g_hwndTextThreads, numberOfThreads.c_str());

    g_hwndTrackbarThreads = CreateWindowEx(
        0,                               // no extended styles 
        TRACKBAR_CLASS,                  // class name 
        L"Aktualna iloœæ w¹tków",              // title (caption) 
        WS_CHILD |
        WS_VISIBLE |
        TBS_AUTOTICKS |
        TBS_ENABLESELRANGE,                // style 
        0, 
        60,                          // position 
        250, 
        20,                         // size 
        hwnd,                         // parent window 
        NULL,                    // control identifier 
        hInstance,                         // instance 
        NULL                             // no WM_CREATE parameter 
    );

    SendMessage(g_hwndTrackbarThreads, TBM_SETRANGE,
        (WPARAM)TRUE,                   // redraw flag 
        (LPARAM)MAKELONG(1, 64));  // min. & max. positions

    SendMessage(g_hwndTrackbarThreads, TBM_SETPAGESIZE,
        0, (LPARAM)4);                  // new page size 

    SendMessage(g_hwndTrackbarThreads, TBM_SETSEL,
        (WPARAM)FALSE,                  // redraw flag 
        (LPARAM)MAKELONG(1, 64));

    SendMessage(g_hwndTrackbarThreads, TBM_SETPOS,
        (WPARAM)TRUE,                   // redraw flag 
        (LPARAM)1);

    g_hwndTextFile = CreateWindowEx(
        0,
        L"STATIC",  // Predefined class; Unicode assumed 
        NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0,
        100,
        250,
        20,
        hwnd,
        NULL,
        hInstance,
        NULL
    );
    SetWindowText(g_hwndTextFile, L"WprowadŸ nazwê pliku .bmp:");

    g_hwndEnterTextFile = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"Edit",  // Predefined class; Unicode assumed 
        NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0,
        120,
        250,
        20,
        hwnd,
        NULL,
        hInstance,
        NULL
    );

    g_hwndButtonFile = CreateWindowEx(
        0,
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"ok",      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        250,         // x position 
        120,         // y position 
        50,        // Button width
        20,        // Button height
        hwnd,     // Parent window
        NULL,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL        // Pointer not needed.
    );

    g_hwndTextScaleWidth = CreateWindowEx(
        0,
        L"STATIC",  // Predefined class; Unicode assumed 
        NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0,
        160,
        175,
        20,
        hwnd,
        NULL,
        hInstance,
        NULL
    );
    SetWindowText(g_hwndTextScaleWidth, L"Nowa szerokoœæ obrazu:");

    g_hwndEnterTextScaleWidth = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"Edit",  // Predefined class; Unicode assumed 
        NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        175,
        160,
        75,
        20,
        hwnd,
        NULL,
        hInstance,
        NULL
    );

    g_hwndTextScaleHeight = CreateWindowEx(
        0,
        L"STATIC",  // Predefined class; Unicode assumed 
        NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0,
        180,
        175,
        20,
        hwnd,
        NULL,
        hInstance,
        NULL
    );
    SetWindowText(g_hwndTextScaleHeight, L"Nowa wysokoœæ obrazu:");

    g_hwndEnterTextScaleHeight = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"Edit",  // Predefined class; Unicode assumed 
        NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        175,
        180,
        75,
        20,
        hwnd,
        NULL,
        hInstance,
        NULL
    );

    g_hwndButtonScale = CreateWindowEx(
        0,
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"skaluj",      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        0,         // x position 
        200,         // y position 
        100,        // Button width
        20,        // Button height
        hwnd,     // Parent window
        NULL,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL        // Pointer not needed.
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int width;
    int height;
    HDC hdc;

    switch (uMsg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_SIZE:
        width = LOWORD(lParam);  // Macro to get the low-order word.
        height = HIWORD(lParam); // Macro to get the high-order word.

        // Respond to the message:
        OnSize(hwnd, (UINT)wParam, width, height);
    break;

    case WM_COMMAND:
        if ((HWND)lParam == g_hwndButtonDLL)
            if (g_currentDLL == TypeOfDLL::ASM)
            {
                SetWindowText(g_hwndTextDLL, L"Aktualnie u¿ywana DLL: C");
                g_currentDLL = TypeOfDLL::C;
            }
            else
            {
                SetWindowText(g_hwndTextDLL, L"Aktualnie u¿ywana DLL: ASM");
                g_currentDLL = TypeOfDLL::ASM;
            }
        else if ((HWND)lParam == g_hwndButtonFile)
        {
            DWORD length = GetWindowTextLength(g_hwndEnterTextFile);
            LPWSTR buffer = (LPWSTR)GlobalAlloc(GPTR, static_cast<SIZE_T>(length) + 1);
            if (buffer)
            {
                GetWindowTextW(g_hwndEnterTextFile, buffer, length + 1);
                g_imageFile = buffer;
                g_imageFile += L".bmp";
                RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
                g_imageFile.clear();
            }
            else
            {
                ::MessageBox(NULL, L"B³¹d wczytywania pliku", L"Error", MB_OK);
            }
            SetWindowText(g_hwndEnterTextFile, NULL);
            GlobalFree(buffer);
        }
        else if ((HWND)lParam == g_hwndButtonScale)
        {
            scaleImage();
        }
        break;

    case WM_HSCROLL:
        if ((HWND)lParam == g_hwndTrackbarThreads)
        {
            std::wstring numberOfThreads;
            switch (LOWORD(wParam)) {
            LRESULT dwPos;
            case TB_ENDTRACK:
                dwPos = SendMessage(g_hwndTrackbarThreads, TBM_GETPOS, 0, 0);
                g_numberOfThreads = (int)dwPos;
                numberOfThreads = L"Liczba w¹tków: " + std::to_wstring(g_numberOfThreads);
                SetWindowText(g_hwndTextThreads, numberOfThreads.c_str());
                break;

            default:
                break;
            }
        }
        break;

    case WM_PAINT:
        PAINTSTRUCT ps;
        hdc = BeginPaint(hwnd, &ps);
        if (!g_imageFile.empty())
        {
            LPCWSTR imageFile = g_imageFile.c_str();
            LoadAndBlitBitmap(imageFile, hdc);
        }
        EndPaint(hwnd, &ps);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}