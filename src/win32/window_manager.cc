#include "../window_manager.h"
#include <windows.h>

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lparam) {
    std::vector<int64_t>* windowHandles = reinterpret_cast<std::vector<int64_t>*>(lparam);
    if (windowHandles != nullptr) {
        windowHandles->push_back(reinterpret_cast<int64_t>(hwnd));
    }
    return TRUE;
}

std::vector<int64_t> getWindows() {
    std::vector<int64_t> windowHandles;

    EnumWindows (&EnumWindowsProc, reinterpret_cast<LPARAM>(&windowHandles));

    return windowHandles;
}

MMRect getWindowRect(const int64_t windowHandle) {
    HWND hWnd = reinterpret_cast<HWND>(windowHandle);
    RECT windowRect;
    GetWindowRect(hWnd, &windowRect);
    return MMRectMake(windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
}

std::string getWindowTitle(const int64_t windowHandle) {
    HWND hWnd = reinterpret_cast<HWND>(windowHandle);
    auto BUFFER_SIZE = GetWindowTextLength(hWnd) + 1;
    LPSTR windowTitle = new CHAR[BUFFER_SIZE];

    GetWindowText(hWnd, windowTitle, BUFFER_SIZE);

    return std::string(windowTitle);
}