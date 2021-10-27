#pragma warning(disable: 4996)
#pragma warning(disable:26451)

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <string>
#include <thread>
#include <CommCtrl.h>

#define DATA_OFFSET_OFFSET 0x000A
#define WIDTH_OFFSET 0x0012
#define HEIGHT_OFFSET 0x0016
#define BITS_PER_PIXEL_OFFSET 0x001C
#define HEADER_SIZE 14
#define INFO_HEADER_SIZE 40
#define NO_COMPRESION 0
#define MAX_NUMBER_OF_COLORS 0
#define ALL_COLORS_REQUIRED 0

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef int(__cdecl* MYPROC)(unsigned char* pixels, unsigned char* newPixels, int oldWidth, int newWidth, double x_ratio, double y_ratio, int size, int totalSize);

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

byte* g_pixels;
unsigned int g_imageWidth;
unsigned int g_imageHeight;
unsigned int g_imageBytesPerPixel;

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

void WriteImage(const char* fileName, byte* pixels, unsigned int width, unsigned int height)
{
    FILE* outputFile = fopen(fileName, "wb");

    const char* BM = "BM";
    fwrite(&BM[0], 1, 1, outputFile);
    fwrite(&BM[1], 1, 1, outputFile);
    int paddedRowSize = (int)(4 * ceil((float)width / 4.0f)) * g_imageBytesPerPixel;
    unsigned int fileSize = paddedRowSize * height + HEADER_SIZE + INFO_HEADER_SIZE;
    fwrite(&fileSize, 4, 1, outputFile);
    unsigned int reserved = 0x0000;
    fwrite(&reserved, 4, 1, outputFile);
    unsigned int dataOffset = HEADER_SIZE + INFO_HEADER_SIZE;
    fwrite(&dataOffset, 4, 1, outputFile);

    unsigned int infoHeaderSize = INFO_HEADER_SIZE;
    fwrite(&infoHeaderSize, 4, 1, outputFile);
    fwrite(&width, 4, 1, outputFile);
    fwrite(&height, 4, 1, outputFile);
    short planes = 1;
    fwrite(&planes, 2, 1, outputFile);
    short bitsPerPixel = g_imageBytesPerPixel * 8;
    fwrite(&bitsPerPixel, 2, 1, outputFile);

    unsigned int compression = NO_COMPRESION;
    fwrite(&compression, 4, 1, outputFile);
    unsigned int imageSize = width * height * g_imageBytesPerPixel;
    fwrite(&imageSize, 4, 1, outputFile);
    unsigned int resolutionX = 11811;
    unsigned int resolutionY = 11811;
    fwrite(&resolutionX, 4, 1, outputFile);
    fwrite(&resolutionY, 4, 1, outputFile);
    unsigned int colorsUsed = MAX_NUMBER_OF_COLORS;
    fwrite(&colorsUsed, 4, 1, outputFile);
    unsigned int importantColors = ALL_COLORS_REQUIRED;
    fwrite(&importantColors, 4, 1, outputFile);

    unsigned int unpaddedRowSize = width * g_imageBytesPerPixel;
    unsigned int totalSize = unpaddedRowSize * height;

    fwrite(pixels, 1, totalSize, outputFile);

    fclose(outputFile);
}

void readImage()
{
    FILE* imageFile = _wfopen(g_imageFile.c_str(), L"rb");
    unsigned int dataOffset;
    fseek(imageFile, DATA_OFFSET_OFFSET, SEEK_SET);
    fread(&dataOffset, 4, 1, imageFile);
    fseek(imageFile, WIDTH_OFFSET, SEEK_SET);
    fread(&g_imageWidth, 4, 1, imageFile);
    fseek(imageFile, HEIGHT_OFFSET, SEEK_SET);
    fread(&g_imageHeight, 4, 1, imageFile);
    short bitsPerPixel;
    fseek(imageFile, BITS_PER_PIXEL_OFFSET, SEEK_SET);
    fread(&bitsPerPixel, 2, 1, imageFile);
    g_imageBytesPerPixel = ((unsigned int)bitsPerPixel) / 8;

    unsigned int unpaddedRowSize = g_imageWidth * g_imageBytesPerPixel;
    unsigned int totalSize = unpaddedRowSize * g_imageHeight;
    g_pixels = new byte[totalSize];

    fseek(imageFile, dataOffset, SEEK_SET);
    fread(g_pixels, 1, totalSize, imageFile);

    fclose(imageFile);
}

void scaleImage(unsigned int newWidth, unsigned int newHeight)
{
    HINSTANCE hinstLib;
    MYPROC ProcAdd;
    BOOL fFreeResult, fRunTimeLinkSuccess = FALSE;

    std::wstring nameOfDLL;

    readImage();
    BYTE* newPixels = new BYTE[g_imageBytesPerPixel * newWidth * newHeight];
    double x_ratio = g_imageWidth / (double)newWidth;
    double y_ratio = g_imageHeight / (double)newHeight;

    unsigned int oldUnpaddedRowSize = g_imageWidth * g_imageBytesPerPixel;
    unsigned int unpaddedRowSize = newWidth * g_imageBytesPerPixel;

    unsigned int totalSize = unpaddedRowSize * newHeight;

    int size = totalSize / g_numberOfThreads;
    int size2 = size;
    int remnant = totalSize % g_numberOfThreads;

    std::thread* threadArray = new std::thread[g_numberOfThreads];

    switch (g_currentDLL)
    {
    case TypeOfDLL::ASM:
        nameOfDLL = L"ImageScalingLibASM";
        break;
    case TypeOfDLL::C:
        nameOfDLL = L"ImageScalingLibC";
        break;
    default:
        break;
    }
    hinstLib = LoadLibrary(nameOfDLL.c_str());

    if (hinstLib != NULL)
    {
        ProcAdd = (MYPROC)GetProcAddress(hinstLib, "scaleImage");

        if (ProcAdd != NULL)
        {
            fRunTimeLinkSuccess = TRUE;
            for (int i = 0; i < g_numberOfThreads; i++)
            {
                if (i == g_numberOfThreads - 1)
                    size2 += remnant;

                threadArray[i] = std::thread(ProcAdd, g_pixels, &newPixels[size * i], oldUnpaddedRowSize, unpaddedRowSize, x_ratio, y_ratio, size2, size*i);
            }
            for (int i = 0; i < g_numberOfThreads; i++)
            {
                threadArray[i].join();
            }
        }

        fFreeResult = FreeLibrary(hinstLib);
    }

    if (!fRunTimeLinkSuccess)
        ::MessageBox(NULL, L"Nie uda³o siê za³adowaæ DLL", L"Error", MB_OK);

    delete[] threadArray;

    WriteImage("output.bmp", newPixels, newWidth, newHeight);
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance,_In_opt_  HINSTANCE hPrevInstance,_In_  PWSTR pCmdLine,_In_  int nCmdShow)
{
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
            unsigned int newWidth = 0;
            unsigned int newHeight = 0;
            DWORD length = GetWindowTextLength(g_hwndEnterTextScaleWidth);
            LPWSTR buffer = (LPWSTR)GlobalAlloc(GPTR, static_cast<SIZE_T>(length) + 1);
            if (buffer)
            {
                GetWindowTextW(g_hwndEnterTextScaleWidth, buffer, length + 1);
                newWidth = _wtoi(buffer);
            }
            length = GetWindowTextLength(g_hwndEnterTextScaleHeight);
            buffer = (LPWSTR)GlobalAlloc(GPTR, static_cast<SIZE_T>(length) + 1);
            if (buffer)
            {
                GetWindowTextW(g_hwndEnterTextScaleHeight, buffer, length + 1);
                newHeight = _wtoi(buffer);
            }
            scaleImage(newWidth, newHeight);
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