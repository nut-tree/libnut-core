#include "../screengrab.h"

#include <ApplicationServices/ApplicationServices.h>
#include <memory>

std::shared_ptr<Bitmap> copyMMBitmapFromDisplayInRect(MMRect rect)
{
	CGDirectDisplayID displayID = CGMainDisplayID();

	CGImageRef image = CGDisplayCreateImageForRect(displayID,
		CGRectMake(rect.origin.x,
			rect.origin.y,
			rect.size.width,
			rect.size.height));

	if (!image) { 
    throw new std::runtime_error("Failed to fetch screen content");
  }

	CFDataRef imageData = CGDataProviderCopyData(CGImageGetDataProvider(image));

	if (!imageData) { 
    throw new std::runtime_error("Failed to retrieve image data");
  }

	size_t bufferSize = CFDataGetLength(imageData);
  uint8_t* buffer = new uint8_t[bufferSize];

	CFDataGetBytes(imageData, CFRangeMake(0,bufferSize), buffer);

  std::shared_ptr<Bitmap> bitmap(new Bitmap(buffer,
		CGImageGetWidth(image),
		CGImageGetHeight(image),
		CGImageGetBytesPerRow(image),
		CGImageGetBitsPerPixel(image)));

	CFRelease(imageData);

	CGImageRelease(image);

	return bitmap;
}
