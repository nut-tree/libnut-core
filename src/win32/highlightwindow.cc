#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include "../highlightwindow.h"

#define ID_CLOSE_TIMER 1001

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void showHighlightWindow(int32_t x, int32_t y, int32_t width, int32_t height, long duration, float opacity) {
    // Register the window class.
    const wchar_t CLASS_NAME[]  = L"Highlight Window Class";

    WNDCLASS wc = {0};

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = NULL;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = CreateSolidBrush(RGB(255, 0, 0));

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        WS_EX_LAYERED|WS_EX_TOPMOST|WS_EX_TRANSPARENT|WS_EX_TOOLWINDOW,
        CLASS_NAME,
        0,
        WS_POPUP,
        x,
        y,
        width,
        height,
        NULL,
        NULL,
        NULL,
        NULL
        );

    SetLayeredWindowAttributes(hwnd, 0, (BYTE)(255 * opacity), LWA_ALPHA);

    if (hwnd == NULL) {
        return;
    }

    SetTimer(hwnd, ID_CLOSE_TIMER, duration, NULL);
    ShowWindow(hwnd, 1);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        KillTimer(hwnd, ID_CLOSE_TIMER);
        PostQuitMessage(0);
        return 0;
    case WM_TIMER:
        switch(wParam) {
            case ID_CLOSE_TIMER:
                KillTimer(hwnd, ID_CLOSE_TIMER);
                DestroyWindow(hwnd);
                PostQuitMessage(0);
                return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
