#include "os.h"

#if defined(IS_MACOSX)

#include <iostream>
#include <ApplicationServices/ApplicationServices.h>

static void startupCheck(void) __attribute__((constructor));
static void startupCheck(void)
{
	bool isTrustedProcess = AXIsProcessTrusted();
	if (!isTrustedProcess) {
		std::cout << "##### WARNING! The application running this script is not a trusted process! Please visit https://github.com/nut-tree/nut.js#macos #####" << std::endl;
	} 
}

#elif defined(USE_X11)

	#include "xdisplay.h"

	Display *display = XGetMainDisplay();

	int32_t majorOpcode;
	int32_t firstEvent;
	int32_t firstError;
	bool isXTestAvailable = XQueryExtension(display, "XTEST", &majorOpcode, &firstEvent, &firstEvent);

	if (!isXTestAvailable) {
		std::cout << "##### WARNING! Your system is missing libXtst! Please visit https://github.com/nut-tree/nut.js#linux #####" << std::endl;
	}

#endif