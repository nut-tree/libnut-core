#pragma once
#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <vector>
#include <string>
#include "types.h"

std::vector<int32_t> getWindows();
MMRect getWindowRect(const int32_t windowHandle);

#endif
