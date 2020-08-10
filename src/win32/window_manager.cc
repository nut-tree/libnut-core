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
    if (foregroundWindow != NULL) {
        return reinterpret_cast<WindowHandle>(foregroundWindow);
    }
    return -1;
}

MMRect getWindowRect(const WindowHandle windowHandle) {
    HWND hWnd = reinterpret_cast<HWND>(windowHandle);
    RECT windowRect;
    if (GetWindowRect(hWnd, &windowRect)) {
        return MMRectMake(windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
    }
    return MMRectMake(0, 0, 0, 0);
}

std::string getWindowTitle(const WindowHandle windowHandle) {
    HWND hWnd = reinterpret_cast<HWND>(windowHandle);
    auto BUFFER_SIZE = GetWindowTextLength(hWnd) + 1;
    if (BUFFER_SIZE) {
        LPSTR windowTitle = new CHAR[BUFFER_SIZE];
        if (GetWindowText(hWnd, windowTitle, BUFFER_SIZE)) {
            return std::string(windowTitle);
        }
    }
    return "";
}
