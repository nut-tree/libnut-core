#include "mouse_utils.h"
#import <AppKit/AppKit.h>

double GetDoubleClickTime() {
  double doubleClickInterval = [NSEvent doubleClickInterval];
  return doubleClickInterval * 1000;
}