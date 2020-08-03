#pragma once
#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <vector>
#include <string>
#include "types.h"

std::vector<int64_t> getWindows();
std::string getWindowTitle(const int64_t windowHandle);
MMRect getWindowRect(const int64_t windowHandle);

#endif
