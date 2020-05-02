#include "screen.h"
#include "os.h"
#include "window.h"

#if defined(IS_MACOSX)
	#include <ApplicationServices/ApplicationServices.h>
#elif defined(USE_X11)
	#include <X11/Xlib.h>
	#include "xdisplay.h"
#endif

MMSize getMainDisplaySize(void)
{
#if defined(IS_MACOSX)
	CGDirectDisplayID displayID = CGMainDisplayID();
	return MMSizeMake(CGDisplayPixelsWide(displayID),
	                  CGDisplayPixelsHigh(displayID));
#elif defined(USE_X11)
	Display *display = XGetMainDisplay();
	const int screen = DefaultScreen(display);

	return MMSizeMake((size_t)DisplayWidth(display, screen),
	                  (size_t)DisplayHeight(display, screen));
#elif defined(IS_WINDOWS)
	return MMSizeMake((size_t)GetSystemMetrics(SM_CXSCREEN),
	                  (size_t)GetSystemMetrics(SM_CYSCREEN));
#endif
}

bool pointVisibleOnMainDisplay(MMPoint point)
{
	MMSize displaySize = getMainDisplaySize();
	return point.x < displaySize.width && point.y < displaySize.height;
}

void highlight(int x, int y, int width, int height, int duration, float opacity) {
    Window highlightWindow(x, y, width, height);
	highlightWindow.show(duration, opacity);
}