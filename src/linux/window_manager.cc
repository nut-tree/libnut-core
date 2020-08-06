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