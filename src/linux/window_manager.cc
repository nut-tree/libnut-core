#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "../window_manager.h"

Display* connectToX() {
    Display* xDisplay = XOpenDisplay(NULL);

    if (xDisplay == NULL) {
        return nullptr;
    }
    return xDisplay;
}

void disconnectFromX(Display* connection) {
    XCloseDisplay(connection);
}

WindowHandle getActiveWindow() {
    Display* xServer = connectToX();
    Window window;
    if (xServer != nullptr) {
        int32_t revertToWindow;
        XGetInputFocus(xServer, &window, &revertToWindow);
        disconnectFromX(xServer);
        return window;
    }
    return NULL;
}

std::vector<WindowHandle> getWindows() {
    Display* xServer = connectToX();
    std::vector<WindowHandle> windowHandles;
    if (xServer != nullptr) {
        Window defaultRootWindow = DefaultRootWindow(xServer);
        Window rootWindow;
        Window parentWindow;
        Window* windowList;
        uint32_t windowCount;

        Status queryTreeResult = XQueryTree(xServer, defaultRootWindow, &rootWindow, &parentWindow, &windowList, &windowCount);
        if (queryTreeResult > 0) {
            for (size_t idx = 0; idx < windowCount; ++idx) {
                windowHandles.push_back(windowList[idx]);
            }
        }
        disconnectFromX(xServer);
    }
    return windowHandles;
}

std::string getWindowTitle(const WindowHandle windowHandle) {
    Display* xServer = connectToX();
    std::string windowName = "";
    if (xServer != nullptr) {
        XTextProperty windowTextProperty;
        Status getWMNameResult = XGetWMName(xServer, windowHandle, &windowTextProperty);
        if (getWMNameResult > 0) {
            windowName = std::string(reinterpret_cast<const char*>(windowTextProperty.value));
        }
        disconnectFromX(xServer);
    }
    return windowName; 
}

MMRect getWindowRect(const WindowHandle windowHandle) {
    Display* xServer = connectToX();
    MMRect windowRect = MMRectMake(0, 0, 0, 0);
    if (xServer != nullptr) {
        Window rootWindow;
        int32_t x, y;
        uint32_t width, height, border_width, border_height;
        Status getXGeometryResult = XGetGeometry(xServer, windowHandle, &rootWindow, &x, &y, &width, &height, &border_width, &border_height);
        if (getXGeometryResult > 0) {
            windowRect = MMRectMake(x - border_width, y - border_height, width + border_width, height + border_height);
        }
        disconnectFromX(xServer);
    }
    return windowRect;
}