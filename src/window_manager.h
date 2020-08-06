#pragma once
#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <vector>
#include <string>
#include "types.h"

std::vector<WindowHandle> getWindows();
WindowHandle getActiveWindow();

std::string getWindowTitle(const WindowHandle windowHandle);
MMRect getWindowRect(const WindowHandle windowHandle);

#endif
