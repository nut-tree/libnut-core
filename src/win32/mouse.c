#include "../mouse.h"
#include "../screen.h"
#include "../microsleep.h"
#include "../deadbeef_rand.h"

#include <math.h> /* For floor() */

#if !defined(M_SQRT2)
#define M_SQRT2 1.4142135623730950488016887 /* Fix for MSVC. */
#endif

/**
 * This constant is required as Windows divides the entire
 *	screen space into 65536 segments in both X and Y axes
 * irrespective of resolution
 * https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-mouseinput#remarks
 */
#define ABSOLUTE_COORD_CONST 65536

#define MMMouseToMEventF(down, button) \
    (down ? MMMouseDownToMEventF(button) : MMMouseUpToMEventF(button))

#define MMMouseUpToMEventF(button) \
    ((button) == LEFT_BUTTON ? MOUSEEVENTF_LEFTUP \
    : ((button) == RIGHT_BUTTON ? MOUSEEVENTF_RIGHTUP \
    : MOUSEEVENTF_MIDDLEUP))

#define MMMouseDownToMEventF(button) \
    ((button) == LEFT_BUTTON ? MOUSEEVENTF_LEFTDOWN \
    : ((button) == RIGHT_BUTTON ? MOUSEEVENTF_RIGHTDOWN \
    : MOUSEEVENTF_MIDDLEDOWN))

static int32_t DEFAULT_DOUBLE_CLICK_INTERVAL_MS = 200;

MMPoint CalculateAbsoluteCoordinates(MMPoint point) {
    MMSize displaySize = getMainDisplaySize();
    return MMPointMake(
            ceil((float)(point.x * ABSOLUTE_COORD_CONST) / displaySize.width),
            ceil((float)(point.y * ABSOLUTE_COORD_CONST) / displaySize.height)
    );
}

/**
 * Move the mouse to a specific point.
 * @param point The coordinates to move the mouse to (x, y).
 */
void moveMouse(MMPoint point) {
    INPUT mouseInput;
    MMPoint pointAbsolute = CalculateAbsoluteCoordinates(point);
    mouseInput.type = INPUT_MOUSE;
    mouseInput.mi.dx = pointAbsolute.x;
    mouseInput.mi.dy = pointAbsolute.y;
    mouseInput.mi.mouseData = 0;
    mouseInput.mi.time = 0;
    mouseInput.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    mouseInput.mi.dwExtraInfo = 0;
    SendInput(1, &mouseInput, sizeof(mouseInput));
}

void dragMouse(MMPoint point, const MMMouseButton button) {
    (void) button;
    moveMouse(point);
}

MMPoint getMousePos() {
    POINT point;
    GetCursorPos(&point);

    return MMPointFromPOINT(point);
}

/**
 * Press down a button, or release it.
 * @param down   True for down, false for up.
 * @param button The button to press down or release.
 */
void toggleMouse(bool down, MMMouseButton button) {
    INPUT mouseInput;
    mouseInput.type = INPUT_MOUSE;
    mouseInput.mi.mouseData = 0;
    mouseInput.mi.dx = 0;
    mouseInput.mi.dy = 0;
    mouseInput.mi.time = 0;
    mouseInput.mi.dwFlags = MMMouseToMEventF(down, button);
    SendInput(1, &mouseInput, sizeof(mouseInput));
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
    UINT maxDoubleClickTime = GetDoubleClickTime();
    /* Double click for everything else. */
    clickMouse(button);
    if (maxDoubleClickTime > DEFAULT_DOUBLE_CLICK_INTERVAL_MS) {
        microsleep(DEFAULT_DOUBLE_CLICK_INTERVAL_MS);
    } else {
        microsleep(DEADBEEF_RANDRANGE(1, maxDoubleClickTime));
    }
    clickMouse(button);
}

void scrollMouse(int x, int y) {
    INPUT mouseScrollInputH;
    INPUT mouseScrollInputV;

    mouseScrollInputH.type = INPUT_MOUSE;
    mouseScrollInputH.mi.dx = 0;
    mouseScrollInputH.mi.dy = 0;
    mouseScrollInputH.mi.dwFlags = MOUSEEVENTF_HWHEEL;
    mouseScrollInputH.mi.time = 0;
    mouseScrollInputH.mi.dwExtraInfo = 0;
    // Flip x to match other platforms.
    mouseScrollInputH.mi.mouseData = -x;

    mouseScrollInputV.type = INPUT_MOUSE;
    mouseScrollInputV.mi.dx = 0;
    mouseScrollInputV.mi.dy = 0;
    mouseScrollInputV.mi.dwFlags = MOUSEEVENTF_WHEEL;
    mouseScrollInputV.mi.time = 0;
    mouseScrollInputV.mi.dwExtraInfo = 0;
    mouseScrollInputV.mi.mouseData = y;

    SendInput(1, &mouseScrollInputH, sizeof(mouseScrollInputH));
    SendInput(1, &mouseScrollInputV, sizeof(mouseScrollInputV));
}
