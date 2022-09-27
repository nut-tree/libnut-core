#include "../screen.h"
#include "../os.h"
#include "../highlightwindow.h"

MMSize getMainDisplaySize(void)
{
	return MMSizeMake((size_t)GetSystemMetrics(SM_CXSCREEN),
	                  (size_t)GetSystemMetrics(SM_CYSCREEN));
}

bool pointVisibleOnMainDisplay(MMPoint point)
{
	MMSize displaySize = getMainDisplaySize();
	return point.x < displaySize.width && point.y < displaySize.height;
}

void highlight(int32_t x, int32_t y, int32_t width, int32_t height, long duration, float opacity) {
	showHighlightWindow(x, y, width, height, duration, opacity);
}