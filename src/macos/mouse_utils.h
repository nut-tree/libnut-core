#ifndef MOUSE_UTILS_H
#define MOUSE_UTILS_H

#include "../mouse.h"
#include "../types.h"
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint64_t down;
  uint64_t up;
} ClickTime;

typedef struct {
  ClickTime left;
  ClickTime middle;
  ClickTime right;
  MMPoint clickLocation;
} ClickTimer;

double GetDoubleClickTime();

bool areSamePoint(MMPoint one, MMPoint other);

void recordClickTime(ClickTimer *timer, bool down,
                            MMMouseButton button,
			    MMPoint currentPoint);

clock_t timeSinceLastClick(ClickTimer *timer, bool down, MMMouseButton button, clock_t currentTime); 

#ifdef __cplusplus
}
#endif

#endif
