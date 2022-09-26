#include "../screengrab.h"
#include "../endian.h"

#include <ApplicationServices/ApplicationServices.h>

MMBitmapRef copyMMBitmapFromDisplayInRect(MMRect rect)
{
	CGDirectDisplayID displayID = CGMainDisplayID();

	CGImageRef image = CGDisplayCreateImageForRect(displayID,
		CGRectMake(rect.origin.x,
			rect.origin.y,
			rect.size.width,
			rect.size.height));

	if (!image) { return NULL; }

	CFDataRef imageData = CGDataProviderCopyData(CGImageGetDataProvider(image));

	if (!imageData) { return NULL; }

	size_t bufferSize = CFDataGetLength(imageData);
	uint8_t *buffer = new uint8_t[bufferSize];

	CFDataGetBytes(imageData, CFRangeMake(0,bufferSize), buffer);

	MMBitmapRef bitmap = createMMBitmap(buffer,
		CGImageGetWidth(image),
		CGImageGetHeight(image),
		CGImageGetBytesPerRow(image),
		CGImageGetBitsPerPixel(image),
		CGImageGetBitsPerPixel(image) / 8);

	CFRelease(imageData);

	CGImageRelease(image);

	return bitmap;
}
