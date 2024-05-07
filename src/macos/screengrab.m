#include "../screengrab.h"
#include "../endian.h"
#include <stdlib.h> /* malloc() */
#include <stdio.h> /* printf() */

#include <ApplicationServices/ApplicationServices.h>
#import <Cocoa/Cocoa.h>

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

    size_t width = CGImageGetWidth(image);
    size_t height = CGImageGetHeight(image);
    CGDataProviderRef provider = CGImageGetDataProvider(image);

    CFDataRef imageData = CGDataProviderCopyData(provider);

    if (!imageData) { return NULL; }

    long bufferSize = CFDataGetLength(imageData);
    size_t bitsPerPixel = CGImageGetBitsPerPixel(image);
    size_t bytesPerPixel = bitsPerPixel / 8;
    size_t bytesPerRow = CGImageGetBytesPerRow(image);
    size_t actualBytesPerRow = width * bytesPerPixel;
    const UInt8 *dataPointer = CFDataGetBytePtr(imageData);

    uint8_t *imageDataWithoutPadding = malloc(height * actualBytesPerRow);

    for (size_t y = 0; y < height; ++y) {
        const UInt8 *rowPtr = dataPointer + y * bytesPerRow;
        memcpy(imageDataWithoutPadding + y * actualBytesPerRow, rowPtr, actualBytesPerRow);
    }

    MMBitmapRef bitmap = createMMBitmap(imageDataWithoutPadding,
                                        width,
                                        height,
                                        actualBytesPerRow,
                                        bitsPerPixel,
                                        bytesPerPixel);

    CFRelease(imageData);
    CGImageRelease(image);

    return bitmap;
}
