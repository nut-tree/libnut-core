const libnut = require("bindings")("libnut");

try {
    const permissions = require("node-mac-permissions");

    const wrapWithWarning = (message, nativeFunction) => (...args) => {
        console.warn(message);
        return nativeFunction(...args);
    };

    const askForAccessibility = (nativeFunction, functionName) => {
        if (process.platform !== 'darwin') {
            return nativeFunction;
        }
        const accessibilityStatus = permissions.getAuthStatus("accessibility");

        if (accessibilityStatus === 'authorized') {
            return nativeFunction;
        } else if (accessibilityStatus === 'not determined' || accessibilityStatus === 'denied') {
            permissions.askForAccessibilityAccess();
            return wrapWithWarning(`##### WARNING! The application running this script tries to access accessibility features to execute ${functionName}! Please grant requested access and visit https://github.com/nut-tree/nut.js#macos for further information. #####`, nativeFunction);
        }
    }
    const askForScreenRecording = (nativeFunction, functionName) => {
        if (process.platform !== 'darwin') {
            return nativeFunction;
        }
        const screenCaptureStatus = permissions.getAuthStatus("screen");

        if (screenCaptureStatus === 'authorized') {
            return nativeFunction;
        } else if (screenCaptureStatus === 'not determined' || screenCaptureStatus === 'denied') {
            permissions.askForScreenCaptureAccess();
            return wrapWithWarning(`##### WARNING! The application running this script tries to access accessibility features to execute ${functionName}! Please grant the requested access and visit https://github.com/nut-tree/nut.js#macos for further information. #####`, nativeFunction);
        }
    }

    const accessibilityAccess = [
        "dragMouse",
        "moveMouse",
        "getMousePos",
        "mouseClick",
        "mouseToggle",
        "scrollMouse",
        "setMouseDelay",
        "keyTap",
        "keyToggle",
        "typeString",
        "setKeyboardDelay",
        "getScreenSize",
        "highlight",
        "captureScreen",
        "getWindows",
        "getActiveWindow",
        "getWindowRect",
        "getWindowTitle",
    ];
    const screenCaptureAccess = [
        "getWindowTitle",
    ];

    for (const functionName of accessibilityAccess) {
        libnut[functionName] = askForAccessibility(libnut[functionName], functionName);
    }
    for (const functionName of screenCaptureAccess) {
        libnut[functionName] = askForScreenRecording(libnut[functionName], functionName);
    }
} catch (e) {
    console.warn(`Encountered error establishing macOS permission checks:`, e.message);
    console.warn(`Returning original module.`);
} finally {
    module.exports = libnut;
}