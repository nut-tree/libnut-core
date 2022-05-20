#include "../mouse.h"
#include "../deadbeef_rand.h"
#include "../keypress.h"
#include "../microsleep.h"
#include "mouse_utils.h"

#include <ApplicationServices/ApplicationServices.h>
#include <math.h> /* For floor() */

static int32_t DEFAULT_DOUBLE_CLICK_INTERVAL_MS = 200;

#define MMMouseToCGEventType(down, button)                                     \
  (down ? MMMouseDownToCGEventType(button) : MMMouseUpToCGEventType(button))

#define MMMouseDownToCGEventType(button)                                       \
  ((button) == (LEFT_BUTTON)                                                   \
       ? kCGEventLeftMouseDown                                                 \
       : ((button) == RIGHT_BUTTON ? kCGEventRightMouseDown                    \
                                   : kCGEventOtherMouseDown))

#define MMMouseUpToCGEventType(button)                                         \
  ((button) == LEFT_BUTTON                                                     \
       ? kCGEventLeftMouseUp                                                   \
       : ((button) == RIGHT_BUTTON ? kCGEventRightMouseUp                      \
                                   : kCGEventOtherMouseUp))

#define MMMouseDragToCGEventType(button)                                       \
  ((button) == LEFT_BUTTON                                                     \
       ? kCGEventLeftMouseDragged                                              \
       : ((button) == RIGHT_BUTTON ? kCGEventRightMouseDragged                 \
                                   : kCGEventOtherMouseDragged))

/**
 * Calculate the delta for a mouse move and add them to the event.
 * @param event The mouse move event (by ref).
 * @param point The new mouse x and y.
 */
void calculateDeltas(CGEventRef *event, MMPoint point) {
  /**
   * The next few lines are a workaround for games not detecting mouse moves.
   * See this issue for more information:
   * https://github.com/octalmage/robotjs/issues/159
   */
  CGEventRef get = CGEventCreate(NULL);
  CGPoint mouse = CGEventGetLocation(get);

  // Calculate the deltas.
  int64_t deltaX = point.x - mouse.x;
  int64_t deltaY = point.y - mouse.y;

  CGEventSetIntegerValueField(*event, kCGMouseEventDeltaX, deltaX);
  CGEventSetIntegerValueField(*event, kCGMouseEventDeltaY, deltaY);

  CFRelease(get);
}

/**
 * Move the mouse to a specific point.
 * @param point The coordinates to move the mouse to (x, y).
 */
void moveMouse(MMPoint point) {
  CGPoint position = CGPointMake(point.x, point.y);
  // Create an HID hardware event source
  CGEventSourceRef src = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);

  CGEventRef evt = NULL;
  if (CGEventSourceButtonState(kCGEventSourceStateHIDSystemState,
                               kCGMouseButtonLeft)) {
    // Create a left button drag
    evt = CGEventCreateMouseEvent(src, kCGEventLeftMouseDragged, position,
                                  kCGMouseButtonLeft);
  } else {
    if (CGEventSourceButtonState(kCGEventSourceStateHIDSystemState,
                                 kCGMouseButtonRight)) {
      // Create a right button drag
      evt = CGEventCreateMouseEvent(src, kCGEventRightMouseDragged, position,
                                    kCGMouseButtonLeft);
    } else {
      // Create a mouse move event
      evt = CGEventCreateMouseEvent(src, kCGEventMouseMoved, position,
                                    kCGMouseButtonLeft);
    }
  }

  // Post mouse event and release
  CGEventPost(kCGHIDEventTap, evt);
  if (evt != NULL) {
    CFRelease(evt);
  }
  CFRelease(src);
}

void dragMouse(MMPoint point, const MMMouseButton button) {
  CGEventSourceRef src = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
  const CGEventType dragType = MMMouseDragToCGEventType(button);
  CGEventRef drag = CGEventCreateMouseEvent(
      src, dragType, CGPointFromMMPoint(point), (CGMouseButton)button);
  calculateDeltas(&drag, point);

  CGEventPost(kCGHIDEventTap, drag);
  CFRelease(drag);
  CFRelease(src);
}

MMPoint getMousePos() {
  CGEventSourceRef src = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
  CGEventRef event = CGEventCreate(src);
  CGPoint point = CGEventGetLocation(event);
  CFRelease(event);
  CFRelease(src);

  return MMPointFromCGPoint(point);
}

/**
 * Press down a button, or release it.
 * @param down   True for down, false for up.
 * @param button The button to press down or release.
 *
 * This function ships a manual implementation to handle double clicks by
 * tracking the time interval between mouse events. Reason for this is the fact
 * that
 * https://developer.apple.com/documentation/coregraphics/1408790-cgeventsourcesecondssincelasteve?language=objc
 * has a bit of latency and will stop working correctly if the time between two
 * consecutive clicks is not long enough.
 *
 * This implementation captures the current timestamp for up/down events on each
 * of left/middle/right mouse buttons. If the interval between two clicks is
 * lower than
 * https://developer.apple.com/documentation/appkit/nsevent/1528384-doubleclickinterval?language=objc
 * and both clicks happen at the same position, we alter the mouse event to
 * trigger a double click by setting kCGMouseEventClickState = 2 on the event
 */
void toggleMouse(bool down, MMMouseButton button) {
  static ClickTimer clickTimer = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};

  MMPoint currentMMPoint = getMousePos();

  clock_t intervalSinceLastClick =
      timeSinceLastClick(&clickTimer, down, button, clock());

  const CGPoint currentPos = CGPointFromMMPoint(currentMMPoint);
  const CGEventType mouseType = MMMouseToCGEventType(down, button);
  CGEventSourceRef src = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
  CGEventRef event = CGEventCreateMouseEvent(src, mouseType, currentPos,
                                             (CGMouseButton)button);
  double maxInterval = GetDoubleClickTime();
  if (intervalSinceLastClick > 0 && intervalSinceLastClick <= maxInterval &&
      areSamePoint(currentMMPoint, clickTimer.clickLocation)) {
    CGEventSetIntegerValueField(event, kCGMouseEventClickState, 2);
  }
  CGEventSetFlags(event, flagBuffer);
  CGEventPost(kCGHIDEventTap, event);
  CFRelease(event);
  CFRelease(src);
  recordClickTime(&clickTimer, down, button, currentMMPoint);
}

void clickMouse(MMMouseButton button) {
  toggleMouse(true, button);
  toggleMouse(false, button);
}

/**
 * Special function for sending double clicks, needed for Mac OS X.
 * @param button Button to click.
 */
void doubleClick(MMMouseButton button) {
  double maxDoubleClickTime = GetDoubleClickTime();
  clickMouse(button);
  if (maxDoubleClickTime > DEFAULT_DOUBLE_CLICK_INTERVAL_MS) {
    microsleep(DEFAULT_DOUBLE_CLICK_INTERVAL_MS);
  } else {
    microsleep(DEADBEEF_RANDRANGE(1, maxDoubleClickTime));
  }
  clickMouse(button);
}

void scrollMouse(int x, int y) {
  /*
   * Direction should only be considered based on the scrollDirection.
   * This should not interfere.
   * Set up the OS specific solution
   */

  CGEventRef event;

  event = CGEventCreateScrollWheelEvent(NULL, kCGScrollEventUnitPixel, 2, y, x);
  CGEventPost(kCGHIDEventTap, event);

  CFRelease(event);
}