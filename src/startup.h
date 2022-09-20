#include "os.h"
#include <iostream>

#if defined(IS_MACOSX)

#include <ApplicationServices/ApplicationServices.h>

static void startupCheck(void) __attribute__((constructor));
static void startupCheck(void)
{
  int showDialog = 1;
  CFNumberRef showDialogNumber = CFNumberCreate(0, kCFNumberSInt32Type, &showDialog);
  const void *keys[1] = { kAXTrustedCheckOptionPrompt };
  const void *values[1] = { showDialogNumber };

  CFDictionaryRef opts = CFDictionaryCreate(NULL, keys, values, 1, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

  bool isTrustedProcess = AXIsProcessTrustedWithOptions(opts);
	if (!isTrustedProcess) {
		std::cout << "##### WARNING! The application running this script is not a trusted process! Please visit https://github.com/nut-tree/nut.js#macos #####" << std::endl;
	} 

  bool hasScreenCaptureAccess = CGPreflightScreenCaptureAccess();	
  if (!hasScreenCaptureAccess) {
    CGRequestScreenCaptureAccess();
		std::cout << "##### WARNING! The application running this script is not allowed to capture screen content! Please visit https://github.com/nut-tree/nut.js#macos #####" << std::endl;
    
	} 

  CFRelease(showDialogNumber);
  CFRelease(opts);
}

#endif
