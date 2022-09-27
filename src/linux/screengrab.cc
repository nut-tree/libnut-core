#include "../screengrab.h"
#include "../endian.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "../xdisplay.h"

std::shared_ptr<Bitmap> copyMMBitmapFromDisplayInRect(MMRect rect)
{
	Display *display = XOpenDisplay(NULL);
	XImage *image = XGetImage(display,
	                          XDefaultRootWindow(display),
	                          (int)rect.origin.x,
	                          (int)rect.origin.y,
	                          (unsigned int)rect.size.width,
	                          (unsigned int)rect.size.height,
	                          AllPlanes, ZPixmap);
	XCloseDisplay(display);
	if (image == NULL) return NULL;

	std::shared_ptr<Bitmap> bitmap(new Bitmap((uint8_t *)image->data,
	                        rect.size.width,
	                        rect.size.height,
	                        image->bytes_per_line,
	                        image->bits_per_pixel));
	image->data = NULL; /* Steal ownership of bitmap data so we don't have to
	                     * copy it. */
	XDestroyImage(image);

	return bitmap;
}
