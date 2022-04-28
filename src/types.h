#pragma once
#ifndef TYPES_H
#define TYPES_H

#include "os.h"
#include "inline_keywords.h" /* For H_INLINE */
#include <stddef.h>
#include <stdint.h>

/* Some generic, cross-platform types. */

struct _MMPoint {
	int64_t x;
	int64_t y;
};

typedef struct _MMPoint MMPoint;

struct _MMSize {
	int64_t width;
	int64_t height;
};

typedef struct _MMSize MMSize;

struct _MMRect {
	MMPoint origin;
	MMSize size;
};

typedef struct _MMRect MMRect;

H_INLINE MMPoint MMPointMake(int64_t x, int64_t y)
{
	MMPoint point;
	point.x = x;
	point.y = y;
	return point;
}

H_INLINE MMSize MMSizeMake(int64_t width, int64_t height)
{
	MMSize size;
	size.width = width;
	size.height = height;
	return size;
}

H_INLINE MMRect MMRectMake(int64_t x, int64_t y, int64_t width, int64_t height)
{
	MMRect rect;
	rect.origin = MMPointMake(x, y);
	rect.size = MMSizeMake(width, height);
	return rect;
}

#define MMPointZero MMPointMake(0, 0)

#if defined(IS_MACOSX)

#define CGPointFromMMPoint(p) CGPointMake((CGFloat)(p).x, (CGFloat)(p).y)
#define MMPointFromCGPoint(p) MMPointMake((int64_t)(p).x, (int64_t)(p).y)

#elif defined(IS_WINDOWS)

#define MMPointFromPOINT(p) MMPointMake((int64_t)p.x, (int64_t)p.y)

#endif

typedef int64_t WindowHandle;

#endif /* TYPES_H */
