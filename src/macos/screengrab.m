#include "../screengrab.h"
#include "../endian.h"
#include <stdlib.h> /* malloc() */

#include <ApplicationServices/ApplicationServices.h>
#import <Cocoa/Cocoa.h>

static double getPixelDensity() {
    @autoreleasepool
    {
        NSScreen * mainScreen = [NSScreen
        mainScreen];
        if (mainScreen) {
            return mainScreen.backingScaleFactor;
        } else {
            return 1.0;
        }
    }
}

MMBitmapRef copyMMBitmapFromDisplayInRect(MMRect rect) {

    CGDirectDisplayID displayID = CGMainDisplayID();

    CGImageRef image = CGDisplayCreateImageForRect(displayID,
                                                   CGRectMake(
                                                           rect.origin.x,
                                                           rect.origin.y,
                                                           rect.size.width,
                                                           rect.size.height
                                                   )
    );

    if (!image) { return NULL; }

    CFDataRef imageData = CGDataProviderCopyData(CGImageGetDataProvider(image));

    if (!imageData) { return NULL; }

    long bufferSize = CFDataGetLength(imageData);
    size_t bytesPerPixel = (size_t) (CGImageGetBitsPerPixel(image) / 8);
    double pixelDensity = getPixelDensity();
    long expectedBufferSize = rect.size.width * pixelDensity * rect.size.height * pixelDensity * bytesPerPixel;

    if (expectedBufferSize < bufferSize) {
        size_t reportedByteWidth = CGImageGetBytesPerRow(image);
        size_t expectedByteWidth = expectedBufferSize / (rect.size.height * pixelDensity);

        uint8_t *buffer = malloc(expectedBufferSize);

        const uint8_t *dataPointer = CFDataGetBytePtr(imageData);
        size_t parts = bufferSize / reportedByteWidth;

        for (size_t idx = 0; idx < parts - 1; ++idx) {
            memcpy(buffer + (idx * expectedByteWidth),
                   dataPointer + (idx * reportedByteWidth),
                   expectedByteWidth
            );
        }

        MMBitmapRef bitmap = createMMBitmap(buffer,
                                            rect.size.width * pixelDensity,
                                            rect.size.height * pixelDensity,
                                            expectedByteWidth,
                                            CGImageGetBitsPerPixel(image),
                                            CGImageGetBitsPerPixel(image) / 8);

        CFRelease(imageData);
        CGImageRelease(image);

        return bitmap;
    } else {
        uint8_t *buffer = malloc(bufferSize);
        CFDataGetBytes(imageData, CFRangeMake(0, bufferSize), buffer);
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
}
