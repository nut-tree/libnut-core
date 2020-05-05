#pragma once
#ifndef HIGHLIGHT_WINDOW_H
#define HIGHLIGHT_WINDOW_H

#ifdef __cplusplus
extern "C" 
{
#endif

#include <stdint.h>

void showHighlightWindow(int32_t x, int32_t y, int32_t width, int32_t height, long duration, float opacity);

#ifdef __cplusplus
}
#endif

#endif