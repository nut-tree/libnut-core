#pragma once
#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#include "types.h"
#if defined(USE_X11)
#include "xdisplay.h"
#endif

#include <stdbool.h>

#ifdef __cplusplus
extern "C" 
{
#endif

/* Returns the size of the main display. */
MMSize getMainDisplaySize(void);

/* Convenience function that returns whether the given point is in the bounds
 * of the main screen. */
bool pointVisibleOnMainDisplay(MMPoint point);

void highlight(int32_t x, int32_t y, int32_t width, int32_t height, long duration, float opacity);

#ifdef __cplusplus
}
#endif

#endif /* SCREEN_H */
