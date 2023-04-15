#include "pch.h"
#include "framework.h"
#include "Win32Main.h"
#include "../Raytracing/Raytracer.h"

//----------------------------------------------------------------------------------------------------------------------------------------
#define MAX_LOADSTRING 100
HINSTANCE hInst;                                
HWND hWnd = nullptr;
WCHAR szTitle[MAX_LOADSTRING];            
WCHAR szWindowClass[MAX_LOADSTRING];
bool isRunning;

BITMAPINFO bitmapInfo;

Raytracer* raytracer = nullptr;

int resolutionShift = 1;
int width = 1280 >> resolutionShift;
int height = 720 >> resolutionShift;

//----------------------------------------------------------------------------------------------------------------------------------------
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                OnResize();

//----------------------------------------------------------------------------------------------------------------------------------------
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    ZeroMemory(&bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

    raytracer = new Raytracer();

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_RAYTRACER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    isRunning = true;

	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	double rcpFreq = 1.0 / freq.QuadPart;

    LARGE_INTEGER start,curr,prev;
    QueryPerformanceCounter(&start);
    prev.QuadPart = start.QuadPart;

    double totalTime = 0;
    double updateFrameTime = 1.0f / 30.0f;
    double frameTimeAcc = 0;

    raytracer->Run();

	MSG msg;
    while (isRunning)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        QueryPerformanceCounter(&curr);
        double deltaTime = (curr.QuadPart - prev.QuadPart) * rcpFreq;
        prev.QuadPart = curr.QuadPart;

        totalTime += deltaTime;

        frameTimeAcc += deltaTime;
        if (frameTimeAcc >= updateFrameTime)
        {
			frameTimeAcc -= updateFrameTime;
            if (raytracer->IsRunning())
            {
                InvalidateRect(hWnd, nullptr, TRUE);
            }
        }

        if (raytracer->PollFinishedFlag())
        {
            InvalidateRect(hWnd, nullptr, TRUE);
        }

        Sleep(16);
    }

    delete raytracer;
    raytracer = nullptr;

    return (int) msg.wParam;
}

//----------------------------------------------------------------------------------------------------------------------------------------
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    ZeroMemory(&wcex,sizeof(wcex));

    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RAYTRACER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//----------------------------------------------------------------------------------------------------------------------------------------
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Instanzenhandle in der globalen Variablen speichern

   int screenWidth = GetSystemMetrics(SM_CXSCREEN);
   int screenHeight = GetSystemMetrics(SM_CYSCREEN);

   DWORD dwStyle = WS_OVERLAPPEDWINDOW;

   RECT rect;
   ZeroMemory(&rect,sizeof(rect));
   rect.right = width;
   rect.bottom = height;
   AdjustWindowRect(&rect, dwStyle, FALSE);

   int width = (rect.right - rect.left);
   int height = (rect.bottom - rect.top);
   int cx = (screenWidth - width) / 2;
   int cy = (screenHeight - height) / 2;

   hWnd = CreateWindowW(    szWindowClass, szTitle, 
                            dwStyle, 
                            cx, cy,
                            width, height,
                            nullptr, nullptr, hInstance, nullptr);
   if (!hWnd)
   {
      return FALSE;
   }

   
   ShowWindow(hWnd, nCmdShow);
   OnResize();
   UpdateWindow(hWnd);

   return TRUE;
}

//----------------------------------------------------------------------------------------------------------------------------------------
void OnResize()
{
	RECT rect;
	GetClientRect(hWnd, &rect);

    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;

    if (w == bitmapInfo.bmiHeader.biWidth && h == bitmapInfo.bmiHeader.biHeight)
        return;

	bitmapInfo.bmiHeader.biWidth = w;
	bitmapInfo.bmiHeader.biHeight = -h;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biSizeImage = w * h * bitmapInfo.bmiHeader.biBitCount >> 3;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 32;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;

    raytracer->Resize(w, h);
}

//----------------------------------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_EXITSIZEMOVE:
        {
            OnResize();
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            if (raytracer != NULL)
            {
                int w = bitmapInfo.bmiHeader.biWidth;
                int h = -bitmapInfo.bmiHeader.biHeight;
                SetDIBitsToDevice(hdc, 0, 0, w, h, 0, 0, 0, h, raytracer->GetResult(), &bitmapInfo, DIB_RGB_COLORS);
            }
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_DESTROY:
        isRunning = false;
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

// Meldungshandler für Infofeld.
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
