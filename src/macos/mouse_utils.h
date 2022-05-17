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

inline bool areSamePoint(MMPoint one, MMPoint other) {
  return one.x == other.x && one.y == other.y;
}

inline void recordClickTime(ClickTimer *timer, bool down,
                            MMMouseButton button,
			    MMPoint currentPoint) {
  clock_t milli = clock();
  if (button == LEFT_BUTTON) {
    if (down) {
      timer->left.down = milli;
    } else {
      timer->left.up = milli;
    }
  } else if (button == CENTER_BUTTON) {
    if (down) {
      timer->middle.down = milli;
    } else {
      timer->middle.up = milli;
    }
  } else if (button == RIGHT_BUTTON) {
    if (down) {
      timer->right.down = milli;
    } else {
      timer->right.up = milli;
    }
  }
  timer->clickLocation = currentPoint;
}

inline clock_t timeSinceLastClick(ClickTimer *timer, bool down, MMMouseButton button, clock_t currentTime) {
  if (button == LEFT_BUTTON) {
    if (down) {
      return currentTime - timer->left.down;
    } else {
      return currentTime - timer->left.up;
    }
  } else if (button == CENTER_BUTTON) {
    if (down) {
      return currentTime - timer->middle.down;
    } else {
      return currentTime - timer->middle.up;
    }
  } else if (button == RIGHT_BUTTON) {
    if (down) {
      return currentTime - timer->right.down;
    } else {
      return currentTime - timer->right.up;
    }
  }
  return 0;
}

#ifdef __cplusplus
}
#endif

#endif