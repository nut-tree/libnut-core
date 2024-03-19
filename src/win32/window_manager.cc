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
    auto foregroundWindow = GetForegroundWindow();
    if (IsWindow(foregroundWindow)) {
        return reinterpret_cast<WindowHandle>(foregroundWindow);
    }
    return -1;
}

MMRect getWindowRect(const WindowHandle windowHandle) {
    auto hWnd = reinterpret_cast<HWND>(windowHandle);
    RECT windowRect;
    if (IsWindow(hWnd) && GetWindowRect(hWnd, &windowRect)) {
        return MMRectMake(windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
    }
    return MMRectMake(0, 0, 0, 0);
}

std::string getWindowTitle(const WindowHandle windowHandle) {
    auto hWnd = reinterpret_cast<HWND>(windowHandle);
    if (IsWindow(hWnd)) {
        auto BUFFER_SIZE = GetWindowTextLength(hWnd) + 1;
        if (BUFFER_SIZE) {
            auto windowTitle = new CHAR[BUFFER_SIZE];
            if (GetWindowText(hWnd, windowTitle, BUFFER_SIZE)) {
                return {windowTitle};
            }
        }
    }
    return "";
}

bool focusWindow(const WindowHandle windowHandle) {
    auto hWnd = reinterpret_cast<HWND>(windowHandle);
    if (GetForegroundWindow() == hWnd) {
        return true;
    }
    if (IsWindow(hWnd)) {
        // Restore the window if it's minimized
        if (IsIconic(hWnd)) {
            ShowWindow(hWnd, SW_RESTORE);
        }

        auto processId = GetCurrentProcessId();
        // const auto allowSetForeground = AllowSetForegroundWindow(ASFW_ANY);
        const auto allowSetForeground = AllowSetForegroundWindow(processId);
        const auto setTopLevel = BringWindowToTop(hWnd);
        const auto setForeground = SetForegroundWindow(hWnd);

        // Try to set the window to the foreground
        return allowSetForeground && setTopLevel && setForeground;
    }
    return false;
}

bool resizeWindow(const WindowHandle windowHandle, const MMSize newSize) {
    auto hWnd = reinterpret_cast<HWND>(windowHandle);
    if (IsWindow(hWnd)) {
        //size
        const auto width = newSize.width;
        const auto height = newSize.height;

        RECT currentPosition;
        GetWindowRect(reinterpret_cast<HWND>(windowHandle), &currentPosition);

        //origin
        const auto x = currentPosition.left;
        const auto y = currentPosition.top;

        return MoveWindow(hWnd, x, y, width, height, TRUE);
    }
    return false;
}

bool moveWindow(const WindowHandle windowHandle, const MMPoint newOrigin) {
    auto hWnd = reinterpret_cast<HWND>(windowHandle);
    if (IsWindow(hWnd)) {
        // origin
        const auto x = newOrigin.x;
        const auto y = newOrigin.y;

        RECT currentPosition;
        GetWindowRect(reinterpret_cast<HWND>(windowHandle), &currentPosition);

        // size
        const auto width = currentPosition.right - currentPosition.left;
        const auto height = currentPosition.bottom - currentPosition.top;

        return MoveWindow(hWnd, x, y, width, height, TRUE);
    }
    return false;
}