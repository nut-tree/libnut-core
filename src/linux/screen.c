#include "../screen.h"
#include "../os.h"
#include "../highlightwindow.h"

#include <X11/Xlib.h>
#include "../xdisplay.h"

MMSize getMainDisplaySize(void)
{
	Display *display = XGetMainDisplay();
	const int screen = DefaultScreen(display);

	return MMSizeMake((size_t)DisplayWidth(display, screen),
	                  (size_t)DisplayHeight(display, screen));
}

bool pointVisibleOnMainDisplay(MMPoint point)
{
	MMSize displaySize = getMainDisplaySize();
	return point.x < displaySize.width && point.y < displaySize.height;
}

void highlight(int32_t x, int32_t y, int32_t width, int32_t height, long duration, float opacity) {
	showHighlightWindow(x, y, width, height, duration, opacity);
}