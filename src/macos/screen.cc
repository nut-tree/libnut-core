#include "../screen.h"
#include "../os.h"
#include "../highlightwindow.h"

#include <ApplicationServices/ApplicationServices.h>

MMSize getMainDisplaySize(void)
{
	CGDirectDisplayID displayID = CGMainDisplayID();
	return MMSizeMake(CGDisplayPixelsWide(displayID),
	                  CGDisplayPixelsHigh(displayID));
}

bool pointVisibleOnMainDisplay(MMPoint point)
{
	MMSize displaySize = getMainDisplaySize();
	return point.x < displaySize.width && point.y < displaySize.height;
}

void highlight(int32_t x, int32_t y, int32_t width, int32_t height, long duration, float opacity) {
	showHighlightWindow(x, y, width, height, duration, opacity);
}