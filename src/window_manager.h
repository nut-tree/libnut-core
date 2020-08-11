#pragma once
#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <vector>
#include <string>
#include "types.h"

/**
 * `getWindows` returns a list of 64bit window handles listing all top-level windows
 */
std::vector<WindowHandle> getWindows();
/**
 * `getActiveWindow` returns the 64bit window handle of the currently active top-level window.
 * If no window is active, the return value is platform dependent.
 * On Windows, if no particular window is focused, it may return either size and position of the Windows task bar (might be focues unintentionally),
 * or size and position of the desktop window (which results in (0, 0, X_RESOLUTION, Y_RESOLUTION)).
 * On macOS, if no window is focused the return value will be the error return of window handle -1 and (0, 0, 0, 0) for size and position.
 * On Linux it returns the desktop window as well (which results in (0, 0, X_RESOLUTION, Y_RESOLUTION)).
 */
WindowHandle getActiveWindow();

/**
 * `getWindowTitle` returns an std::string holding the window title for a window adressed via its window handle.
 * The respective window handle may be aquired via `getWindows` or `getActiveWindow`
 */
std::string getWindowTitle(const WindowHandle windowHandle);
/**
 * `getWindowRect` returns an MMRect struct representing the window's size and position.
 * Windows are adressed via their window handle.
 * That is, a window's top left position given as `x` and `y` coordinate, as well as it's window size given as `width` and `height`
 * The respective window handle may be aquired via `getWindows` or `getActiveWindow`
 */
MMRect getWindowRect(const WindowHandle windowHandle);

#endif
