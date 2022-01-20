#pragma warning(disable: 4996)
#pragma warning(disable:26451)

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <string>
#include <thread>
#include <CommCtrl.h>
#include <iostream>
#include <vector>
#include <fstream>

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

typedef int(__cdecl* MYPROC)(byte* pixels, byte* newPixels, int oldWidth, int newWidth, float x_ratio, float y_ratio, int partSize, int totalSize);

//_g stands for global variables

HWND g_hwndBackground;
HWND g_hwndButtonDLL;
HWND g_hwndTextDLL;

HWND g_hwndTextThreads;
HWND g_hwndTrackbarThreads;

HWND g_hwndTextFile;
HWND g_hwndEnterTextFile;
HWND g_hwndButtonFile;

HWND g_hwndTextCurrentFile;
HWND g_hwndButtonShowCurrentFile;
HWND g_hwndTextShowFileWarning;

HWND g_hwndTextScale;
HWND g_hwndTrackbarScale;

HWND g_hwndButtonScale;

HWND g_hwndTextResults;

HWND g_hwndTextProgress;
HWND g_hwndButtonSaveResults;
HWND g_hwndTextSaveResultsWarning;

enum class TypeOfDLL {ASM, C};
TypeOfDLL g_currentDLL = TypeOfDLL::ASM;

int g_numberOfThreads = 1;
float g_scale = 10;

std::wstring g_imageFile;

byte* g_pixels;
unsigned int g_imageWidth;
unsigned int g_imageHeight;
unsigned int g_imageBytesPerPixel;

double g_result = 0.0;

bool draw = false;

void OnSize(HWND hwnd, UINT flag, int width, int height)
{
    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

bool LoadAndBlitBitmap(HDC hDC, DWORD dwROP)
{
    HBITMAP hBitmap;
    HDC       memDC;
    BITMAP    Bitmap;
    BOOL      bResult;

    memDC = CreateCompatibleDC(hDC);
    HBITMAP memBM = CreateCompatibleBitmap(hDC, g_imageWidth, g_imageHeight);
    GetObject(memBM, sizeof(BITMAP), (LPSTR)&Bitmap);
    SelectObject(memDC, memBM);
    for (int i = 0; i < g_imageHeight; i++) {
        for (int j = 0; j < g_imageWidth*3; j += 3)
        {
            SetPixel(memDC, j/3, i, RGB(g_pixels[((g_imageHeight - i - 1) * g_imageWidth * 3) + j + 2], g_pixels[((g_imageHeight - i - 1) * g_imageWidth * 3) + j + 1], g_pixels[((g_imageHeight - i - 1) * g_imageWidth * 3) + j]));
        }
    }
    bResult = BitBlt(hDC, 0, 0, g_imageWidth, g_imageHeight, memDC, 0, 0, SRCCOPY);
    DeleteDC(memDC);
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

    size_t row_stride = width * g_imageBytesPerPixel;
    int new_stride = width % 4;
    unsigned char bmpPad[3] = { 0, 0, 0 };

    for (int i = 0; i < height; i++) {
        fwrite(&pixels[row_stride * i], 1, row_stride, outputFile);
        fwrite(bmpPad, 1, new_stride, outputFile);
    }

    fclose(outputFile);
}

void readImage()
{
    //delete[] g_pixels;

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
    g_pixels = new BYTE[totalSize + 24];

    fseek(imageFile, dataOffset, SEEK_SET);

    size_t row_stride = g_imageWidth * g_imageBytesPerPixel;
    int new_stride = g_imageWidth % 4;

    for (int i = 0; i < g_imageHeight; i++) {
        fread(&g_pixels[row_stride * i], 1, row_stride, imageFile);
        dataOffset += row_stride + new_stride;
        fseek(imageFile, dataOffset, SEEK_SET);
    }

    fclose(imageFile);
}

void scaleImage(unsigned int newWidth, unsigned int newHeight)
{
    HINSTANCE hinstLib;
    MYPROC ProcAdd;
    BOOL fFreeResult, fRunTimeLinkSuccess = FALSE;

    std::wstring nameOfDLL;

    BYTE* newPixels = new BYTE[g_imageBytesPerPixel * newWidth * newHeight + 12];
    float x_ratio = g_imageWidth / (float)newWidth;
    float y_ratio = g_imageHeight / (float)newHeight;

    unsigned int oldUnpaddedRowSize = g_imageWidth * g_imageBytesPerPixel;
    unsigned int unpaddedRowSize = newWidth * g_imageBytesPerPixel;

    unsigned int totalSize = unpaddedRowSize * newHeight;

    int size = 0;
    int size2 = 0; 
    int remnant = 0;

    if (totalSize < g_numberOfThreads)
    {
        size = g_numberOfThreads;
        size2 = size;
        remnant = 0;
    }
    else
    {
        size = totalSize / g_numberOfThreads;
        size2 = size;
        remnant = totalSize % g_numberOfThreads;
    }

    

    while (size % 3 != 0)
    {
        size--;
        size2--;
        remnant += g_numberOfThreads;
    }

    std::vector<std::thread> threadArray;

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

                threadArray.push_back(std::thread(ProcAdd, &g_pixels[0], &newPixels[0], oldUnpaddedRowSize, unpaddedRowSize, x_ratio, y_ratio, size2, size * i));
            }
            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < g_numberOfThreads; i++)
            {
                threadArray[i].join();
            }
            auto finish = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = finish - start;
            g_result = elapsed.count();
        }

        fFreeResult = FreeLibrary(hinstLib);
    }

    if (!fRunTimeLinkSuccess)
        ::MessageBox(NULL, L"Nie uda³o siê za³adowaæ DLL", L"Error", MB_OK);

    //WriteImage("output.bmp", newPixels, newWidth, newHeight);
    delete[] newPixels;
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
        WS_EX_CLIENTEDGE,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Skalowanie Obrazu",    // Window text
        (WS_OVERLAPPEDWINDOW),            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 1200, 800,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    g_hwndBackground = CreateWindowEx(
        0,
        L"STATIC",  // Predefined class; Unicode assumed 
        NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        875,
        0,
        325,
        805,
        hwnd,
        NULL,
        hInstance,
        NULL
    );

    g_hwndButtonDLL = CreateWindowEx(
        0,
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"zmieñ",      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        1130,         // x position 
        50,         // y position 
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
        880, 
        50, 
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
        880,
        100,
        300,
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
        880, 
        120,                          // position 
        300, 
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
        880,
        200,
        300,
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
        880,
        220,
        300,
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
        1130,         // x position 
        240,         // y position 
        50,        // Button width
        20,        // Button height
        hwnd,     // Parent window
        NULL,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL        // Pointer not needed.
    );

    g_hwndTextCurrentFile = CreateWindowEx(
        0,
        L"STATIC",  // Predefined class; Unicode assumed 
        NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        880,
        260,
        300,
        40,
        hwnd,
        NULL,
        hInstance,
        NULL
    );
    SetWindowText(g_hwndTextCurrentFile, L"Aktualnie przetwarzany plik .bmp:");

    g_hwndButtonShowCurrentFile = CreateWindowEx(
        0,
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"Poka¿ obraz",      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        880,         // x position 
        320,         // y position 
        100,        // Button width
        20,        // Button height
        hwnd,     // Parent window
        NULL,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL        // Pointer not needed.
    );

    g_hwndTextShowFileWarning = CreateWindowEx(
        0,
        L"STATIC",  // Predefined class; Unicode assumed 
        NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        880,
        340,
        300,
        40,
        hwnd,
        NULL,
        hInstance,
        NULL
    );
    SetWindowText(g_hwndTextShowFileWarning, L"Uwaga: mo¿e zaj¹æ d³ugo dla du¿ych obrazów");

    g_hwndTextScale = CreateWindowEx(
        0,
        L"STATIC",  // Predefined class; Unicode assumed 
        NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        880,
        400,
        300,
        20,
        hwnd,
        NULL,
        hInstance,
        NULL
    );
    std::wstring scaleText = L"Powiêkszenie obrazu x" + std::to_wstring(g_scale);
    SetWindowText(g_hwndTextScale, scaleText.c_str());

    g_hwndTrackbarScale = CreateWindowEx(
        0,                               // no extended styles 
        TRACKBAR_CLASS,                  // class name 
        L"",              // title (caption) 
        WS_CHILD |
        WS_VISIBLE |
        TBS_AUTOTICKS |
        TBS_ENABLESELRANGE,                // style 
        880,
        420,                          // position 
        300,
        20,                         // size 
        hwnd,                         // parent window 
        NULL,                    // control identifier 
        hInstance,                         // instance 
        NULL                             // no WM_CREATE parameter 
    );

    SendMessage(g_hwndTrackbarScale, TBM_SETRANGE,
        (WPARAM)TRUE,                   // redraw flag 
        (LPARAM)MAKELONG(10, 40));  // min. & max. positions

    SendMessage(g_hwndTrackbarScale, TBM_SETPAGESIZE,
        0, (LPARAM)40);                  // new page size 

    SendMessage(g_hwndTrackbarScale, TBM_SETSEL,
        (WPARAM)FALSE,                  // redraw flag 
        (LPARAM)MAKELONG(10, 40));

    SendMessage(g_hwndTrackbarScale, TBM_SETPOS,
        (WPARAM)TRUE,                   // redraw flag 
        (LPARAM)10);

    g_hwndButtonScale = CreateWindowEx(
        0,
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"skaluj",      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        1080,         // x position 
        440,         // y position 
        100,        // Button width
        20,        // Button height
        hwnd,     // Parent window
        NULL,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL        // Pointer not needed.
    );

    g_hwndTextResults = CreateWindowEx(
        0,
        L"STATIC",  // Predefined class; Unicode assumed 
        NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        880,
        500,
        300,
        20,
        hwnd,
        NULL,
        hInstance,
        NULL
    );
    SetWindowText(g_hwndTextResults, L"Czas wykonania: ");

    g_hwndTextProgress = CreateWindowEx(
        0,
        L"STATIC",  // Predefined class; Unicode assumed 
        NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        880,
        520,
        300,
        40,
        hwnd,
        NULL,
        hInstance,
        NULL
    );
    SetWindowText(g_hwndTextProgress, L" ");

    g_hwndButtonSaveResults = CreateWindowEx(
        0,
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"Eksportuj porównanie do pliku",      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        880,         // x position 
        540,         // y position 
        300,        // Button width
        20,        // Button height
        hwnd,     // Parent window
        NULL,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL        // Pointer not needed.
    );

    g_hwndTextSaveResultsWarning = CreateWindowEx(
        0,
        L"STATIC",  // Predefined class; Unicode assumed 
        NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        880,
        560,
        300,
        60,
        hwnd,
        NULL,
        hInstance,
        NULL
    );
    SetWindowText(g_hwndTextSaveResultsWarning, L"Uwaga: porównanie wyników powoduje wykonanie programu 1280 razy (10 razy dla ka¿dej z bibliotek na od 1 do 64 w¹tków)");

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
                if (_wfopen(g_imageFile.c_str(), L"rb"))
                {
                    LPCWSTR imageFile = g_imageFile.c_str();
                    std::wstring fileString = L"Aktualnie przetwarzany plik .bmp: " + g_imageFile;
                    SetWindowText(g_hwndTextCurrentFile, fileString.c_str());
                    readImage();
                    g_imageFile.clear();
                }
                else
                {
                    ::MessageBox(NULL, L"B³¹d wczytywania pliku", L"Error", MB_OK);
                }
            }
            else
            {
                ::MessageBox(NULL, L"B³¹d wczytywania pliku", L"Error", MB_OK);
            }
            SetWindowText(g_hwndEnterTextFile, NULL);
            GlobalFree(buffer);
        }
        else if ((HWND)lParam == g_hwndButtonShowCurrentFile)
        {
            draw = true;
            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        }
        else if ((HWND)lParam == g_hwndButtonScale)
        {
            scaleImage(g_imageWidth * g_scale, g_imageHeight * g_scale);
            std::wstring resultText = L"Czas wykonania: " + std::to_wstring(g_result);
            SetWindowText(g_hwndTextResults, resultText.c_str());
        }
        else if ((HWND)lParam == g_hwndButtonSaveResults)
        {
            int numberOfThreads = g_numberOfThreads;
            TypeOfDLL currentDLL = g_currentDLL;

            std::ofstream myfile;
            myfile.open("results.csv");

            g_currentDLL = TypeOfDLL::ASM;
            float progress = 0.0f;

            myfile << " ,";
            for (int t = 1; t <= 64; t++)
            {
                myfile << t << ',';
            }
            myfile << '\n';
            myfile << "ASM,";
            
            for (int t = 1; t <= 64; t++)
            {
                double result = 0.0;
                for (int i = 0; i < 10; i++)
                {
                    g_numberOfThreads = t;
                    scaleImage(g_imageWidth * g_scale, g_imageHeight * g_scale);
                    result += g_result;
                    progress += 0.078125f;
                    std::wstring progressString = std::to_wstring(progress) + L"%";
                    SetWindowText(g_hwndTextProgress, progressString.c_str());
                }
                result /= 10;
                myfile << result << ',';
            }
           
            myfile << '\n';
            myfile << "C,";
            g_currentDLL = TypeOfDLL::C;
            for (int t = 1; t <= 64; t++)
            {
                double result = 0.0;
                for (int i = 0; i < 10; i++)
                {
                    g_numberOfThreads = t;
                    scaleImage(g_imageWidth * g_scale, g_imageHeight * g_scale);
                    result += g_result;
                    progress += 0.078125f;
                    std::wstring progressString = std::to_wstring(progress) + L"%";
                    SetWindowText(g_hwndTextProgress, progressString.c_str());
                }
                result /= 10;
                myfile << result << ',';
            }

            g_numberOfThreads = numberOfThreads;
            g_currentDLL = currentDLL;
            myfile.close();
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
        else if ((HWND)lParam == g_hwndTrackbarScale)
        {
            std::wstring scaleText;
            switch (LOWORD(wParam)) {
                LRESULT dwPos;
            case TB_ENDTRACK:
                dwPos = SendMessage(g_hwndTrackbarScale, TBM_GETPOS, 0, 0);
                g_scale = (int)dwPos;
                g_scale /= 10;
                scaleText = L"Powiêkszenie obrazu x" + std::to_wstring(g_scale);
                SetWindowText(g_hwndTextScale, scaleText.c_str());
                break;

            default:
                break;
            }
        }
        break;

    case WM_PAINT:
        PAINTSTRUCT ps;
        hdc = BeginPaint(hwnd, &ps);
        if (draw)
        {
            LoadAndBlitBitmap(hdc, SRCCOPY);
            draw = false;
        }
        EndPaint(hwnd, &ps);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}