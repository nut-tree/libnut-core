#pragma once
#ifndef MMBITMAP_H
#define MMBITMAP_H

#include "types.h"
#include "rgb.h"
#include <assert.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" 
{
#endif

struct _MMBitmap {
	uint8_t *imageBuffer;  /* Pixels stored in Quad I format; i.e., origin is in
	                        * top left. Length should be height * bytewidth. */
	size_t width;          /* Never 0, unless image is NULL. */
	size_t height;         /* Never 0, unless image is NULL. */
	size_t bytewidth;      /* The aligned width (width + padding). */
	uint8_t bitsPerPixel;  /* Should be either 24 or 32. */
	uint8_t bytesPerPixel; /* For convenience; should be bitsPerPixel / 8. */
};

typedef struct _MMBitmap MMBitmap;
typedef MMBitmap *MMBitmapRef;

/* Creates new MMBitmap with the given values.
 * Follows the Create Rule (caller is responsible for destroy()'ing object). */
MMBitmapRef createMMBitmap(uint8_t *buffer, size_t width, size_t height,
                           size_t bytewidth, uint8_t bitsPerPixel,
						   uint8_t bytesPerPixel);

/* Releases memory occupied by MMBitmap. */
void destroyMMBitmap(MMBitmapRef bitmap);

/* Releases memory occupied by MMBitmap. Acts via CallBack method*/
void destroyMMBitmapBuffer(char * bitmapBuffer, void * hint);

/* Returns copy of MMBitmap, to be destroy()'d by caller. */
MMBitmapRef copyMMBitmap(MMBitmapRef bitmap);

/* Returns copy of one MMBitmap juxtaposed in another (to be destroy()'d
 * by the caller.), or NULL on error. */
MMBitmapRef copyMMBitmapFromPortion(MMBitmapRef source, MMRect rect);

#define MMBitmapPointInBounds(image, p) ((p).x < (image)->width && \
                                         (p).y < (image)->height)
#define MMBitmapRectInBounds(image, r)                    \
	(((r).origin.x + (r).size.width <= (int64_t)(image)->width) && \
	 ((r).origin.y + (r).size.height <= (int64_t)(image)->height))

#define MMBitmapGetBounds(image) MMRectMake(0, 0, image->width, image->height)

#ifdef __cplusplus
}
#endif

#endif /* MMBITMAP_H */