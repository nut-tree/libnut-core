#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "../window_manager.h"
#include "../xdisplay.h"

WindowHandle getActiveWindow() {
    Display* xServer = XGetMainDisplay();
    Window window;
    if (xServer != nullptr) {
        int32_t revertToWindow;
        XGetInputFocus(xServer, &window, &revertToWindow);
        XCloseMainDisplay();
        return window;
    }
    return NULL;
}

std::vector<WindowHandle> getWindows() {
    Display* xServer = XGetMainDisplay();
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
        XCloseMainDisplay();
    }
    return windowHandles;
}

std::string getWindowTitle(const WindowHandle windowHandle) {
    Display* xServer = XGetMainDisplay();
    std::string windowName = "";
    if (xServer != nullptr) {
        XTextProperty windowTextProperty;
        Status getWMNameResult = XGetWMName(xServer, windowHandle, &windowTextProperty);
        if (getWMNameResult > 0) {
            windowName = std::string(reinterpret_cast<const char*>(windowTextProperty.value));
        }
        XCloseMainDisplay();
    }
    return windowName; 
}

MMRect getWindowRect(const WindowHandle windowHandle) {
    Display* xServer = XGetMainDisplay();
    MMRect windowRect = MMRectMake(0, 0, 0, 0);
    if (xServer != nullptr) {
        Window rootWindow;
        int32_t x, y;
        uint32_t width, height, border_width, border_height;
        Status getXGeometryResult = XGetGeometry(xServer, windowHandle, &rootWindow, &x, &y, &width, &height, &border_width, &border_height);
        if (getXGeometryResult > 0) {
            windowRect = MMRectMake(x, y, width, height);
        }
        XCloseMainDisplay();
    }
    return windowRect;
}