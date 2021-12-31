#include "os.h"
#include <iostream>

#if defined(IS_MACOSX)

#include <ApplicationServices/ApplicationServices.h>

static void startupCheck(void) __attribute__((constructor));
static void startupCheck(void)
{
	bool isTrustedProcess = AXIsProcessTrusted();
	if (!isTrustedProcess) {
		std::cout << "##### WARNING! The application running this script is not a trusted process! Please visit https://github.com/nut-tree/nut.js#macos #####" << std::endl;
	} 
}

#endif