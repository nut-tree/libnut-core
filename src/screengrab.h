#pragma once
#ifndef SCREENGRAB_H
#define SCREENGRAB_H

#include "types.h"
#include "Bitmap.h"

#if defined(USE_X11)
#include "xdisplay.h"
#endif

/* Returns a raw bitmap of screengrab of the display (to be destroyed()'d by
 * caller), or NULL on error. */
std::shared_ptr<Bitmap> copyMMBitmapFromDisplayInRect(MMRect rect);

#endif /* SCREENGRAB_H */
