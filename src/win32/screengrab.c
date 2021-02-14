#include "../screengrab.h"
#include "../endian.h"
#include <stdlib.h> /* malloc() */

typedef enum DEVICE_SCALE_FACTOR {
  DEVICE_SCALE_FACTOR_INVALID = 0,
  SCALE_100_PERCENT = 100,
  SCALE_120_PERCENT = 120,
  SCALE_125_PERCENT = 125,
  SCALE_140_PERCENT = 140,
  SCALE_150_PERCENT = 150,
  SCALE_160_PERCENT = 160,
  SCALE_175_PERCENT = 175,
  SCALE_180_PERCENT = 180,
  SCALE_200_PERCENT = 200,
  SCALE_225_PERCENT = 225,
  SCALE_250_PERCENT = 250,
  SCALE_300_PERCENT = 300,
  SCALE_350_PERCENT = 350,
  SCALE_400_PERCENT = 400,
  SCALE_450_PERCENT = 450,
  SCALE_500_PERCENT = 500
} DEVICE_SCALE_FACTOR;

static int getScaleFactor(void)
{
	static HRESULT (*GetScaleFactorForMonitor)(HMONITOR monitor,
						   DEVICE_SCALE_FACTOR *scale);
	static DEVICE_SCALE_FACTOR scale;
	static int initialized;

	HANDLE hnd;
	POINT origin;
	HMONITOR monitor;

	if (initialized)
		return scale;

	scale = 100;
	initialized = 1;
	hnd = LoadLibraryExA("shcore.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (!hnd)
		return scale;

	GetScaleFactorForMonitor =
		(HRESULT (*)(HMONITOR, DEVICE_SCALE_FACTOR *))
		GetProcAddress(hnd, "GetScaleFactorForMonitor");

	if (!GetScaleFactorForMonitor)
		return scale;

	ZeroMemory(&origin, sizeof(origin));
	monitor = MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);
	if (GetScaleFactorForMonitor(monitor, &scale) != S_OK ||
	    scale < 100)
		scale = 100;

	return scale;
}

BOOL copyOrResize(HDC screenMem, HDC screen, MMRect rect, int scaleFactor)
{
	if (scaleFactor == 100)
		return BitBlt(screenMem,
			      (int)0,
			      (int)0,
			      (int)rect.size.width,
			      (int)rect.size.height,
			      screen,
			      (int)rect.origin.x,
			      (int)rect.origin.y,
			      SRCCOPY);
	SetStretchBltMode(screenMem, HALFTONE);
	SetBrushOrgEx(screenMem, 0, 0, NULL);
	return StretchBlt(screenMem,
			  (int)0,
			  (int)0,
			  (int)rect.size.width,
			  (int)rect.size.height,
			  screen,
			  (int)rect.origin.x * scaleFactor / 100,
			  (int)rect.origin.y * scaleFactor / 100,
			  (int)rect.size.width * scaleFactor / 100,
			  (int)rect.size.height * scaleFactor / 100,
			  SRCCOPY);
}

MMBitmapRef copyMMBitmapFromDisplayInRect(MMRect rect)
{
	int scaleFactor = getScaleFactor();
	MMBitmapRef bitmap;
	void *data;
	HDC screen = NULL, screenMem = NULL;
	HBITMAP dib;
	BITMAPINFO bi;

	/* Initialize bitmap info. */
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
   	bi.bmiHeader.biWidth = (long)rect.size.width;
   	bi.bmiHeader.biHeight = -(long)rect.size.height; /* Non-cartesian, please */
   	bi.bmiHeader.biPlanes = 1;
   	bi.bmiHeader.biBitCount = 32;
   	bi.bmiHeader.biCompression = BI_RGB;
   	bi.bmiHeader.biSizeImage = (DWORD)(4 * rect.size.width * rect.size.height);
	bi.bmiHeader.biXPelsPerMeter = 0;
	bi.bmiHeader.biYPelsPerMeter = 0;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;

	screen = GetDC(NULL); /* Get entire screen */
	if (screen == NULL) return NULL;

	/* Get screen data in display device context. */
   	dib = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &data, NULL, 0);

	/* Copy the data into a bitmap struct. */
	if ((screenMem = CreateCompatibleDC(screen)) == NULL ||
	    SelectObject(screenMem, dib) == NULL ||
	    !copyOrResize(screenMem, screen, rect, scaleFactor)) {
		/* Error copying data. */
		ReleaseDC(NULL, screen);
		DeleteObject(dib);
		if (screenMem != NULL) DeleteDC(screenMem);

		return NULL;
	}

	bitmap = createMMBitmap(NULL,
	                        rect.size.width,
	                        rect.size.height,
	                        4 * rect.size.width,
	                        (uint8_t)bi.bmiHeader.biBitCount,
	                        4);

	/* Copy the data to our pixel buffer. */
	if (bitmap != NULL) {
		bitmap->imageBuffer = malloc(bitmap->bytewidth * bitmap->height);
		memcpy(bitmap->imageBuffer, data, bitmap->bytewidth * bitmap->height);
	}

	ReleaseDC(NULL, screen);
	DeleteObject(dib);
	DeleteDC(screenMem);

	return bitmap;
}
