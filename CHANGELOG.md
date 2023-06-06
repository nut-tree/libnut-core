# libnut change log

All notable changes to this project will be documented in this file.

## 2.5.2

- Bugfix: Screen capture broken on macOS 13 [(nut-tree/nut.js#469)](https://github.com/nut-tree/nut.js/issues/469)
- Enhancement: Enable newly introduced keys to be used as modifiers [(nut-tree/nut.js#490)](https://github.com/nut-tree/nut.js/issues/490)
- Bugfix: screen.highlight closes Electron window [(nut-tree/nut.js#505)](https://github.com/nut-tree/nut.js/issues/505)

## 2.5.1
- Bugfix: Fix rounding to mouse move on Window to fix mouse drift [(#126)](https://github.com/nut-tree/libnut-core/issues/126)

## 2.5.0
- Bugfix: Update permissionCheck.js to cache permission check results and only ask for permissions on actual function call by wrapping it in a HOF [(nut-tree/nut.js#477)](https://github.com/nut-tree/nut.js/issues/477)
- Bugfix: Add rounding to mouse move on Window to fix mouse drift [(#126)](https://github.com/nut-tree/libnut-core/issues/126)
- Enhancement: Add additional keys [(nut-tree/nut.js#457)](https://github.com/nut-tree/nut.js/issues/457)

## 2.4.1

- Bugfix: Update permission handling on macOS [(nut-tree/nut.js#469)](https://github.com/nut-tree/nut.js/issues/469)

## 2.4.0

- Bugfix: Fix `ReferenceError: b is not defined` [(PR#148)](https://github.com/nut-tree/libnut/pull/148)
- Enhancement: Improved permission handling on macOS [(#141)](https://github.com/nut-tree/libnut/issues/141)
- Bugfix: Limit calls to SetThreadDPIAwarenessContext to Windows 10 clients [(#136)](https://github.com/nut-tree/libnut/issues/136)

## 2.3.0

- Bugfix: Segmentation Fault when retrieving window title [(nut-tree/nut-js#377)](https://github.com/nut-tree/nut.js/issues/377)
- Enhancement: Automatically check and request required permissions on macOS [(nut-tree/nut-js#377)](https://github.com/nut-tree/nut.js/issues/377)

## 2.2.0

- Enhancement: Add Windows runtime files [(PR #130)](https://github.com/nut-tree/libnut/pull/130)
- Bugfix: Fix capture region x,y offset when DPI scaling on Windows [(PR #131)](https://github.com/nut-tree/libnut/pull/131)
- Bugfix: Fix wrong keycode for CapsLock [(PR #132)](https://github.com/nut-tree/libnut/pull/132)

## 2.1.8

- Bugfix: Modifier keys are not properly released on macOS [(nut-tree/nut-js#264)](https://github.com/nut-tree/nut.js/issues/264)
- Bugfix: Fix mouse clicks with modifiers on macOS [(nut-tree/nut-js#273)](https://github.com/nut-tree/nut.js/issues/273)

## 2.1.7

- Enhancement: Disable microsleep between keypresses on Windows and Linux [(#101)](https://github.com/nut-tree/libnut/issues/101)
- Enhancement: Add mappings for missing numpad keys [(#102)](https://github.com/nut-tree/libnut/pull/102)
- Enhancement: Added missing key mappings [(#103)](https://github.com/nut-tree/libnut/pull/103)
- Bugfix: Revert keyboardInput to use scancodes [(PR #107)](https://github.com/nut-tree/libnut/pull/107)
- Bugfix: Updated doubleClick implementation to fire two up/down cycles [(nut-tree/nut-js#373)](https://github.com/nut-tree/nut.js/issues/373)
- Enhancement: Determine Windows doubleclick interval [(#116)](https://github.com/nut-tree/libnut/issues/116)
- Bugfix: Mouse click doesn't work on external monitor with negative x and y [(#71)](https://github.com/nut-tree/libnut/issues/71)
- Bugfix: macOS doubleclick fires two doubleclick events [(#120)](https://github.com/nut-tree/libnut/issues/120)

## 2.1.6

- Enhancement: Numpad buttons don't work on Linux [(nut-tree/nut.js#360)](https://github.com/nut-tree/nut.js/issues/360)
- Bugfix: Issue with keyboard.type in to Spotlight on MacOS [(nut-tree/nut.js#152)](https://github.com/nut-tree/nut.js/issues/152)

## 2.1.5

- Bugfix: Keypresses not properly caught on Windows [(#94)](https://github.com/nut-tree/libnut/issues/94)
- Enhancement: Enable some kind of warning / info message in case system requirements are not met [(#91)](https://github.com/nut-tree/libnut/issues/91)

## 2.1.4

- Bugfix: Windows display scaling is applied in wrong direction [(#92)](https://github.com/nut-tree/libnut/issues/92)

## 2.1.3

- Enhancement: Fix undefined behaviour of BufferFinalizer [(#47)](https://github.com/nut-tree/libnut/issues/47)
- Bugfix: Fix for screen highlight window minimized intead close [(PR #77)](https://github.com/nut-tree/libnut/pull/77)
- Enhancement: SendInput for mouse movement on Windows [(#26)](https://github.com/nut-tree/libnut/issues/26)
- Bugfix: Windows Scaling issues: screen functions broken [(nut-tree/nut.js#249)](https://github.com/nut-tree/nut.js/issues/249)
- Enhancement: Support Apple Silicon [(nut-tree/nut.js#306)](https://github.com/nut-tree/nut.js/issues/306)
- Bugfix: Remove scan code KEYUP block [(PR #87)](https://github.com/nut-tree/libnut/pull/87)
 
## 2.1.2

- Maintenance: Upgrade CI [(#56)](https://github.com/nut-tree/libnut/issues/56)
- Enhancement: Fix compiler warnings [(#58)](https://github.com/nut-tree/libnut/issues/58)
- Enhancement: Windows: Support for HDPI displays [(#59)](https://github.com/nut-tree/libnut/issues/59)
- Enhancement: macOS: Support for M1 chips [(PR #50)](https://github.com/nut-tree/libnut/pull/50)
- Enhancement: Remove static keyboard delay [(PR #64)](https://github.com/nut-tree/libnut/pull/64)
 
## 2.1.1

- Enhancement: Snapshot releases [(#4)](https://github.com/nut-tree/libnut/issues/4)
- Enhancement: Update CI configs [(#7)](https://github.com/nut-tree/libnut/issues/7)
- Enhancement: Split OS specific implementation into separate files [(#20)](https://github.com/nut-tree/libnut/issues/20)
- Enhancement: Linux: Update XGetMainDisplay to avoid receiving `Invalid MIT-MAGIC-COOKIE-1 key` [(#27)](https://github.com/nut-tree/libnut/issues/27)
- Enhancement: Enable GitHub Actions [(#40)](https://github.com/nut-tree/libnut/issues/40)
- Enhancement: Trigger snapshot build for https://github.com/nut-tree/libnut-install after snapshot release [(#42)](https://github.com/nut-tree/libnut/issues/42)
- Bugfix: Region captures can't capture the whole screen [(#45)](https://github.com/nut-tree/libnut/issues/45)

## 2.1.0

- Enhancement: Retrieve coordinates of current active window [(#15)](https://github.com/nut-tree/libnut/issues/15)
- Enhancement: Retrieve dimensions for a window specified via its window handle [(#17)](https://github.com/nut-tree/libnut/issues/17)
- Enhancement: Separate folders for OS specific implementation [(#19)](https://github.com/nut-tree/libnut/issues/19)
- Enhancement: Retrieve window name for a given window handle [(#22)](https://github.com/nut-tree/libnut/issues/22)

## 2.0.1

- Bugfix: Fix hanging shift key after keyboard input on Windows [(#12)](https://github.com/nut-tree/libnut/issues/12)
