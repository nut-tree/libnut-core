#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "highlightwindow.h"

void showHighlightWindow(int x, int y, int width, int height, int duration, float opacity)
{
    Display *d = XOpenDisplay(NULL);
    Window root = DefaultRootWindow(d);
    int default_screen = XDefaultScreen(d);

    XSetWindowAttributes attrs;
    attrs.override_redirect = True;

    XVisualInfo vinfo;
    if (!XMatchVisualInfo(d, DefaultScreen(d), 32, TrueColor, &vinfo)) {
        return;
    }
    attrs.colormap = XCreateColormap(d, root, vinfo.visual, AllocNone);
    int colorValue = 255 * opacity;
    attrs.background_pixel = (colorValue << 24 | colorValue << 16);
    attrs.border_pixel = 0;

    Window overlay = XCreateWindow(
        d, root,
        x, y, width, height, 0,
        vinfo.depth, InputOutput, 
        vinfo.visual,
        CWOverrideRedirect | CWColormap | CWBackPixel | CWBorderPixel, &attrs
    );

    XMapWindow(d, overlay);

    XFlush(d);

    sleep(duration);

    XUnmapWindow(d, overlay);
    XCloseDisplay(d);
}
