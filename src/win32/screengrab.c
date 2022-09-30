#include "../screengrab.h"
#include "../endian.h"
#include <stdlib.h> /* malloc() */
#include <VersionHelpers.h>

MMRect getScaledRect(MMRect input)
{
	size_t scaledDesktopWidth = (size_t)GetSystemMetrics(SM_CXSCREEN);
	size_t scaledDesktopHeight = (size_t)GetSystemMetrics(SM_CYSCREEN);
	// Configure DPI awareness to fetch unscaled display size
	if (IsWindows10OrGreater() && !IsWindowsServer()) {
		// Re-query desktop dimensions after setting the DPI awareness context
		// Only to this on Windows 10 client platforms, since earlier versions of Windows and Windows Server do not support this call
		DPI_AWARENESS_CONTEXT initialDpiAwareness = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		scaledDesktopWidth = (size_t)GetSystemMetrics(SM_CXSCREEN);
		scaledDesktopHeight = (size_t)GetSystemMetrics(SM_CYSCREEN);
		// Reset DPI awareness to avoid inconsistencies on future calls to copyMMBitmapFromDisplayInRect
		SetThreadDpiAwarenessContext(initialDpiAwareness);
	}
	size_t desktopWidth = (size_t)GetSystemMetrics(SM_CXSCREEN);
	size_t desktopHeight = (size_t)GetSystemMetrics(SM_CYSCREEN);

	double scaleX = (double)(desktopWidth / (double)scaledDesktopWidth);
	double scaleY = (double)(desktopHeight / (double)scaledDesktopHeight);

	return MMRectMake(
		input.origin.x / scaleX,
		input.origin.y / scaleY,
		input.size.width / scaleX,
		input.size.height / scaleY
	);
}

MMBitmapRef copyMMBitmapFromDisplayInRect(MMRect rect)
{
	MMBitmapRef bitmap;
	void *data;
	HDC screen = NULL, screenMem = NULL;
	HBITMAP dib;
	BITMAPINFO bi;

	screen = GetWindowDC(NULL); /* Get entire screen */
	MMRect scaledRect = getScaledRect(rect);

	if (screen == NULL) {
		return NULL;
	}

	/* Initialize bitmap info. */
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = (long)scaledRect.size.width;
	bi.bmiHeader.biHeight = -(long)scaledRect.size.height; /* Non-cartesian, please */
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = (DWORD)(4 * scaledRect.size.width * scaledRect.size.height);
	bi.bmiHeader.biXPelsPerMeter = 0;
	bi.bmiHeader.biYPelsPerMeter = 0;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;

	/* Get screen data in display device context. */
	dib = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &data, NULL, 0);

	/* Copy the data into a bitmap struct. */
	if ((screenMem = CreateCompatibleDC(screen)) == NULL ||
	    SelectObject(screenMem, dib) == NULL ||
	    !BitBlt(screenMem,
		    (int)0,
		    (int)0,
		    (int)scaledRect.size.width,
		    (int)scaledRect.size.height,
		    screen,
		    (int)scaledRect.origin.x,
		    (int)scaledRect.origin.y,
		    SRCCOPY))
	{

		/* Error copying data. */
		ReleaseDC(NULL, screen);
		DeleteObject(dib);
		if (screenMem != NULL) {
			DeleteDC(screenMem);
		}

		return NULL;
	}

	bitmap = createMMBitmap(NULL,
				scaledRect.size.width,
				scaledRect.size.height,
				4 * scaledRect.size.width,
				(uint8_t)bi.bmiHeader.biBitCount,
				4);

	/* Copy the data to our pixel buffer. */
	if (bitmap != NULL)
	{
		bitmap->imageBuffer = malloc(bitmap->bytewidth * bitmap->height);
		memcpy(bitmap->imageBuffer, data, bitmap->bytewidth * bitmap->height);
	}

	ReleaseDC(NULL, screen);
	DeleteObject(dib);
	DeleteDC(screenMem);

	return bitmap;
}
