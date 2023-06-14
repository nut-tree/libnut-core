#include "../window_manager.h"
#include <windows.h>

/**
 * Documentation regarding the `EnumWindowsProc` already seems a bit stale (https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms633498(v=vs.85))
 * We should keep this in mind for the future, just in case there should be any deprecations or strange behaviour
 */
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lparam) {
    std::vector<WindowHandle>* windowHandles = reinterpret_cast<std::vector<WindowHandle>*>(lparam);
    if (windowHandles != nullptr) {
        windowHandles->push_back(reinterpret_cast<WindowHandle>(hwnd));
    }
    return TRUE;
}

std::vector<WindowHandle> getWindows() {
    std::vector<WindowHandle> windowHandles;

    EnumWindows (&EnumWindowsProc, reinterpret_cast<LPARAM>(&windowHandles));

    return windowHandles;
}

WindowHandle getActiveWindow() {
    HWND foregroundWindow = GetForegroundWindow();
    if (IsWindow(foregroundWindow)) {
        return reinterpret_cast<WindowHandle>(foregroundWindow);
    }
    return -1;
}

MMRect getWindowRect(const WindowHandle windowHandle) {
    HWND hWnd = reinterpret_cast<HWND>(windowHandle);
    RECT windowRect;
    if (IsWindow(hWnd) && GetWindowRect(hWnd, &windowRect)) {
        return MMRectMake(windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
    }
    return MMRectMake(0, 0, 0, 0);
}

std::string getWindowTitle(const WindowHandle windowHandle) {
    HWND hWnd = reinterpret_cast<HWND>(windowHandle);
    if (IsWindow(hWnd)) {
        auto BUFFER_SIZE = GetWindowTextLength(hWnd) + 1;
        if (BUFFER_SIZE) {
            LPSTR windowTitle = new CHAR[BUFFER_SIZE];
            if (GetWindowText(hWnd, windowTitle, BUFFER_SIZE)) {
                return std::string(windowTitle);
            }
        }
    }
    return "";
}

bool focusWindow(const WindowHandle windowHandle) {
    HWND hWnd = reinterpret_cast<HWND>(windowHandle);
    if (IsWindow(hWnd)) {
        // Restore the window if it's minimized
        if (IsIconic(hWnd)) {
            ShowWindow(hWnd, SW_RESTORE);
        }
        
        // Try to set the window to the foreground
        return SetForegroundWindow(hWnd);
    }
    return false;
}

bool resizeWindow(const WindowHandle windowHandle, const MMRect& rect) {
    HWND hWnd = reinterpret_cast<HWND>(windowHandle);
    if (IsWindow(hWnd)) {
        //size
        int width = rect.size.width;
        int height = rect.size.height;

        //origin
        int x = rect.origin.x;
        int y = rect.origin.y;

        return MoveWindow(hWnd, x, y, width, height, TRUE);
    }
    return false;
}
