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
        int32_t getWMNameResult = XGetWMName(xServer, windowHandle, &windowTextProperty);
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
        XWindowAttributes windowAttributes;
        int32_t getXGeometryResult = XGetWindowAttributes(xServer, windowHandle, &windowAttributes);
        printf("Border width: %d", windowAttributes.border_width);
        if (getXGeometryResult > 0) {
            windowRect = MMRectMake(windowAttributes.x - windowAttributes.border_width, \
                                    windowAttributes.y - windowAttributes.border_width, \
                                    windowAttributes.width + windowAttributes.border_width, \
                                    windowAttributes.height + windowAttributes.border_width);
        }
        disconnectFromX(xServer);
    }
    return windowRect;
}