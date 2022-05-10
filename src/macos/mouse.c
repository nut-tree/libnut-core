#include "../mouse.h"
#include "../microsleep.h"

#include <math.h> /* For floor() */
#include <ApplicationServices/ApplicationServices.h>

#if !defined(M_SQRT2)
#define M_SQRT2 1.4142135623730950488016887 /* Fix for MSVC. */
#endif

#define MMMouseToCGEventType(down, button) \
    (down ? MMMouseDownToCGEventType(button) : MMMouseUpToCGEventType(button))

#define MMMouseDownToCGEventType(button)                                            \
    ((button) == (LEFT_BUTTON) ? kCGEventLeftMouseDown                              \
                               : ((button) == RIGHT_BUTTON ? kCGEventRightMouseDown \
                                                           : kCGEventOtherMouseDown))

#define MMMouseUpToCGEventType(button)                                          \
    ((button) == LEFT_BUTTON ? kCGEventLeftMouseUp                              \
                             : ((button) == RIGHT_BUTTON ? kCGEventRightMouseUp \
                                                         : kCGEventOtherMouseUp))

#define MMMouseDragToCGEventType(button)                                             \
    ((button) == LEFT_BUTTON ? kCGEventLeftMouseDragged                              \
                             : ((button) == RIGHT_BUTTON ? kCGEventRightMouseDragged \
                                                         : kCGEventOtherMouseDragged))

/**
 * Calculate the delta for a mouse move and add them to the event.
 * @param event The mouse move event (by ref).
 * @param point The new mouse x and y.
 */
void calculateDeltas(CGEventRef *event, MMPoint point)
{
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
void moveMouse(MMPoint point)
{
    CGPoint position = CGPointMake(point.x, point.y);
    // Create an HID hardware event source
    CGEventSourceRef src = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);

    CGEventRef evt = NULL;
    if (CGEventSourceButtonState(kCGEventSourceStateHIDSystemState, kCGMouseButtonLeft))
    {
        // Create a left button drag
        evt = CGEventCreateMouseEvent(src, kCGEventLeftMouseDragged, position, kCGMouseButtonLeft);
    }
    else
    {
        if (CGEventSourceButtonState(kCGEventSourceStateHIDSystemState, kCGMouseButtonRight))
        {
            // Create a right button drag
            evt = CGEventCreateMouseEvent(src, kCGEventRightMouseDragged, position, kCGMouseButtonLeft);
        }
        else
        {
            // Create a mouse move event
            evt = CGEventCreateMouseEvent(src, kCGEventMouseMoved, position, kCGMouseButtonLeft);
        }
    }

    // Post mouse event and release
    CGEventPost(kCGHIDEventTap, evt);
    if (evt != NULL)
    {
        CFRelease(evt);
    }
    CFRelease(src);
}

void dragMouse(MMPoint point, const MMMouseButton button)
{
    CGEventSourceRef src = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
    const CGEventType dragType = MMMouseDragToCGEventType(button);
    CGEventRef drag = CGEventCreateMouseEvent(src, dragType, CGPointFromMMPoint(point), (CGMouseButton)button);
    calculateDeltas(&drag, point);

    CGEventPost(kCGHIDEventTap, drag);
    CFRelease(drag);
    CFRelease(src);
}

MMPoint getMousePos()
{
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
 */
void toggleMouse(bool down, MMMouseButton button)
{
    const CGPoint currentPos = CGPointFromMMPoint(getMousePos());
    const CGEventType mouseType = MMMouseToCGEventType(down, button);
    CGEventSourceRef src = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
    CGEventRef event = CGEventCreateMouseEvent(src, mouseType, currentPos, (CGMouseButton)button);
    CGEventPost(kCGHIDEventTap, event);
    CFRelease(event);
    CFRelease(src);
}

void clickMouse(MMMouseButton button)
{
    toggleMouse(true, button);
    toggleMouse(false, button);
}

/**
 * Special function for sending double clicks, needed for Mac OS X.
 * @param button Button to click.
 */
void doubleClick(MMMouseButton button) {
    /* Double click for Mac. */
    const CGPoint currentPos = CGPointFromMMPoint(getMousePos());
    const CGEventType mouseTypeDown = MMMouseToCGEventType(true, button);
    const CGEventType mouseTypeUp = MMMouseToCGEventType(false, button);

    CGEventSourceRef src = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
    CGEventRef event = CGEventCreateMouseEvent(src, mouseTypeDown, currentPos,
                                                button);

    // First down
    CGEventPost(kCGHIDEventTap, event);

    // First up
    CGEventSetType(event, mouseTypeUp);
    CGEventPost(kCGHIDEventTap, event);

    /* Set event to double click. */
    CGEventSetIntegerValueField(event, kCGMouseEventClickState, 2);

    // Second down
    CGEventSetType(event, mouseTypeDown);
    CGEventPost(kCGHIDEventTap, event);

    // Second up
    CGEventSetType(event, mouseTypeUp);
    CGEventPost(kCGHIDEventTap, event);

    CFRelease(event);
    CFRelease(src);
}

void scrollMouse(int x, int y)
{
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