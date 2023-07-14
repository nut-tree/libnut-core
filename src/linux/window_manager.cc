#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "../window_manager.h"
extern "C" {
    #include "../xdisplay.h"
}

WindowHandle getActiveWindow() {
    Display* xServer = XGetMainDisplay();
    Window window;
    if (xServer != NULL) {
        int32_t revertToWindow;
        XGetInputFocus(xServer, &window, &revertToWindow);
        return window;
    }
    return -1;
}

std::vector<WindowHandle> getWindows() {
    Display* xServer = XGetMainDisplay();
    std::vector<WindowHandle> windowHandles;
    if (xServer != NULL) {
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
    }
    return windowHandles;
}

std::string getWindowTitle(const WindowHandle windowHandle) {
    Display* xServer = XGetMainDisplay();
    std::string windowName = "";
    if (xServer != NULL && windowHandle >= 0) {
        /*
         * While there's also `XFetchName` to retrieve a window's `WM_NAME` property, in my tests `XFetchName` always failed and return 0
         * `XGetWMName` on the other hand just worked as expected.
         * Keep that in mind in case you're considering changing this implementation
         */
        XTextProperty windowTextProperty;
        Status getWMNameResult = XGetWMName(xServer, windowHandle, &windowTextProperty);
        if (getWMNameResult > 0) {
            windowName = std::string(reinterpret_cast<const char*>(windowTextProperty.value));
        }
    }
    return windowName;
}

MMRect getWindowRect(const WindowHandle windowHandle) {
    Display* xServer = XGetMainDisplay();
    MMRect windowRect = MMRectMake(0, 0, 0, 0);
    if (xServer != NULL && windowHandle >= 0) {
        Window rootWindow;
        int32_t x, y;
        uint32_t width, height, border_width, border_height;
        Status getXGeometryResult = XGetGeometry(xServer, windowHandle, &rootWindow, &x, &y, &width, &height, &border_width, &border_height);
        if (getXGeometryResult > 0) {
            windowRect = MMRectMake(x, y, width, height);
        }
    }
    return windowRect;
}

bool focusWindow(const WindowHandle windowHandle) {
    Display* display = XGetMainDisplay();
    if (display != NULL && windowHandle >= 0) {
        // Try to set the window to the foreground
        XSetInputFocus(display, windowHandle, RevertToParent, CurrentTime);
        XRaiseWindow(display, windowHandle);
        XFlush(display);

        return true;
    }
    return false;
}

bool resizeWindow(const WindowHandle windowHandle, const MMRect& rect) {
    Display* display = XGetMainDisplay();
    if (display != NULL && windowHandle >= 0) {
        XWindowChanges changes;
        
        //size
        changes.width = rect.size.width;
        changes.height = rect.size.height;
        
        //origin
        changes.x = rect.origin.x;
        changes.y = rect.origin.y;
    
        // Resize and move the window
        XConfigureWindow(display, windowHandle, CWX | CWY | CWWidth | CWHeight, &changes);
        XFlush(display);
        
        return true;
    }
    return false;
}