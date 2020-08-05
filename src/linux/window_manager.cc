#include <X11/Xlib.h>
#include "../window_manager.h"

Display* connectToX() {
    Display* xDisplay = XOpenDisplay(NULL);

    if (xDisplay == NULL) {
        return nullptr;
    }
    return xDisplay;
}

int32_t disconnectFromX(Display* connection) {
    return XCloseDisplay(connection);
}

std::vector<WindowHandle> getWindows() {
    Display* xServer = connectToX();
    Window window;
    int32_t revertToWindow;
    if (XGetInputFocus(xServer, &window, &revertToWindow)) {
        disconnectFromX(xServer);
        return std::vector<WindowHandle>{static_cast<WindowHandle>(window)};
    }
    return std::vector<WindowHandle>();
}

std::string getWindowTitle(const WindowHandle windowHandle) {
    Display* xServer = connectToX();
    if (xServer != nullptr) {
        char* windowName = NULL;
        if (XFetchName(xServer, windowHandle, &windowName)) {
            std::string wndName = std::string(windowName);
            XFree(windowName);
            disconnectFromX(xServer);
            return wndName;
        }
        disconnectFromX(xServer);
        return "";
    }
    return "";
}

MMRect getWindowRect(const WindowHandle windowHandle) {
    Display* xServer = connectToX();
    if (xServer != nullptr) {
        Window rootWindow;
        int32_t x, y;
        uint32_t width, height, border_width, border_height;
        if (XGetGeometry(xServer, windowHandle, &rootWindow, &x, &y, &width, &height, &border_width, &border_height)) {
            disconnectFromX(xServer);
            return MMRectMake(x, y, width, height);
        }
        disconnectFromX(xServer);
        return MMRectMake(0, 0, 0, 0);
    }
    return MMRectMake(0, 0, 0, 0);
}