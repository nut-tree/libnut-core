#include "../mouse.h"
#include "../microsleep.h"

#include <math.h> /* For floor() */

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <stdlib.h>
#include "../xdisplay.h"

#if !defined(M_SQRT2)
#define M_SQRT2 1.4142135623730950488016887 /* Fix for MSVC. */
#endif

/**
 * Move the mouse to a specific point.
 * @param point The coordinates to move the mouse to (x, y).
 */
void moveMouse(MMPoint point)
{
	Display *display = XGetMainDisplay();
	XWarpPointer(display, None, DefaultRootWindow(display), 0, 0, 0, 0, point.x, point.y);
	XSync(display, false);
}

void dragMouse(MMPoint point, const MMMouseButton button)
{
	moveMouse(point);
}

MMPoint getMousePos()
{
	int x, y;			 /* This is all we care about. Seriously. */
	Window garb1, garb2; /* Why you can't specify NULL as a parameter */
	int garb_x, garb_y;	 /* is beyond me. */
	unsigned int more_garbage;

	Display *display = XGetMainDisplay();
	XQueryPointer(display, XDefaultRootWindow(display), &garb1, &garb2, &x, &y, &garb_x, &garb_y, &more_garbage);

	return MMPointMake(x, y);
}

/**
 * Press down a button, or release it.
 * @param down   True for down, false for up.
 * @param button The button to press down or release.
 */
void toggleMouse(bool down, MMMouseButton button)
{
	Display *display = XGetMainDisplay();
	XTestFakeButtonEvent(display, button, down ? True : False, CurrentTime);
	XSync(display, false);
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
void doubleClick(MMMouseButton button)
{
	clickMouse(button);
	microsleep(200);
	clickMouse(button);
}

void scrollMouse(int x, int y)
{
	/*
	X11 Mouse Button Numbering
	1 = left button
	2 = middle button (pressing the scroll wheel)
	3 = right button
	4 = turn scroll wheel up
	5 = turn scroll wheel down
	6 = push scroll wheel left
	7 = push scroll wheel right
	8 = 4th button (aka browser backward button)
	9 = 5th button (aka browser forward button)
	*/
	int ydir = 4; // Button 4 is up, 5 is down.
	int xdir = 6; // Button 6 is left, 7 is right.
	Display *display = XGetMainDisplay();

	if (y < 0)
	{
		ydir = 5;
	}
	if (x < 0)
	{
		xdir = 7;
	}

	int xi;
	int yi;
	for (xi = 0; xi < abs(x); xi++)
	{
		XTestFakeButtonEvent(display, xdir, 1, CurrentTime);
		XTestFakeButtonEvent(display, xdir, 0, CurrentTime);
	}
	for (yi = 0; yi < abs(y); yi++)
	{
		XTestFakeButtonEvent(display, ydir, 1, CurrentTime);
		XTestFakeButtonEvent(display, ydir, 0, CurrentTime);
	}

	XSync(display, false);
}
