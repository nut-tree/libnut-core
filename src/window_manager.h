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
std::string getWindowTitle(WindowHandle windowHandle);
/**
 * `getWindowRect` returns an MMRect struct representing the window's size and position.
 * Windows are adressed via their window handle.
 * That is, a window's top left position given as `x` and `y` coordinate, as well as it's window size given as `width` and `height`
 * The respective window handle may be aquired via `getWindows` or `getActiveWindow`
 */
MMRect getWindowRect(WindowHandle windowHandle);

/**
 * `focusWindow` focuses on the window specified by its window handle.
 * It brings the specified window to the foreground and gives it input focus.
 * The respective window handle may be acquired via `getWindows` or `getActiveWindow`.
 * @param windowHandle The window handle of the window to be focused.
 * @return Returns a boolean indicating whether the window focus operation was successful.
 */
bool focusWindow(WindowHandle windowHandle);

/**
 * `resizeWindow` resizes the window specified by its window handle to the given width and height.
 * The respective window handle may be acquired via `getWindows` or `getActiveWindow`.
 * @param windowHandle The window handle of the window to be resized.
 * @param newSize The new size of the window.
 * @param newSize.width The new width of the window.
 * @param newSize.height The new height of the window.
 * @return Returns a boolean indicating whether the window resize operation was successful.
 */
bool resizeWindow(WindowHandle windowHandle, MMSize newSize);

/**
 * `resizeWindow` resizes the window specified by its window handle to the given width and height.
 * The respective window handle may be acquired via `getWindows` or `getActiveWindow`.
 * @param windowHandle The window handle of the window to be resized.
 * @param rect The new position and size of the window.
 * @param rect.x The new x coordinate of the window's top left corner.
 * @param rect.y The new y coordinate of the window's top left corner.
 * @param rect.width The new width of the window.
 * @param rect.height The new height of the window.
 * @return Returns a boolean indicating whether the window resize operation was successful.
 */
bool moveWindow(WindowHandle windowHandle, MMPoint newOrigin);

#endif
