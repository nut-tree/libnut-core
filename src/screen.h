#pragma once
#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#include "types.h"

#if defined(_MSC_VER)
	#include "ms_stdbool.h"
#else
	#include <stdbool.h>
#endif

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
