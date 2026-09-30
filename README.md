## Building

Please ensure you have the required dependencies before installing:

* Windows
  * windows-build-tools npm package (`npm install --global --production windows-build-tools` from an elevated PowerShell or CMD.exe)
* Mac
  * Xcode Command Line Tools.
* Linux
  * cmake
  * A C/C++ compiler like GCC.
  * libxtst-dev and libpng++-dev (`sudo apt-get install libxtst-dev libpng++-dev`).

### Release build

```
npm install
npm run build:release
```

### Debug build

```
npm install
npm run build:debug
```

## Windows host helper for WSL

Windows builds also produce `build/Release/libnut-wsl-host.exe`. It is a C++17
executable used by the WSL providers in
[nut.js](https://github.com/nut-tree/nut.js). It runs independently of Node.js on
Windows and uses Windows APIs for mouse, keyboard, primary-screen capture,
clipboard text and window operations. The executable is included in the
existing Windows native package.

For a helper-only build, use a Windows SDK and Visual C++ toolchain:

```powershell
cmake -S . -B build -A x64 -DLIBNUT_BUILD_ADDON=OFF
cmake --build build --config Release
node test/wsl-host-protocol.js
```

The last command tests the protocol without capturing the desktop or sending
input. Build the helper for the same processor architecture as the Windows
native package. `LIBNUT_BUILD_WSL_HOST=OFF` disables this target when it is not
needed.

Add `-DLIBNUT_BUILD_WSL_TESTS=ON` to configure the native support tests, then run
`ctest --test-dir build -C Release --output-on-failure` after building. These
tests check key mappings, failed-release tracking, reply limits and bounded
window waits without sending input or capturing the desktop.

The helper must run in the signed-in user's interactive Windows session. A
service or SSH session is not a substitute. Windows may block input into
elevated applications or the secure desktop. Coordinates are physical pixels;
screen capture covers the primary monitor, as the nut.js screen interface
requires.

### Internal protocol

The helper reads one UTF-8 JSON request per line from standard input:

```json
{"id":1,"command":"hello","args":{}}
```

Each reply starts with a UTF-8 JSON line containing the matching `id`, either
`result` or `error`, and `byteLength`. A capture reply is followed immediately
by exactly `byteLength` raw, top-down BGRA pixel bytes. Other replies have a zero
byte length. The `hello` result reports protocol version 1; clients must check
it before sending desktop commands.

Requests execute in order. Mouse clicks and keyboard clicks are single helper
commands. Requests and JSON reply lines are limited to 1 MiB; captured pixel
data is limited to 128 MiB. An oversized reply is replaced with a bounded error
for that request, so subsequent commands can still use the connection.
Diagnostic messages go to standard error, never into the reply stream.

Key names map to the same fixed US keyboard codes as the native backend. Text
uses Unicode input instead. Window changes are requested asynchronously and
wait at most five seconds for each state change, returning false if it does not
complete. The waits also check whether the caller has disconnected.

Closing the input pipe cancels long typing or highlight operations and attempts
to release keys and mouse buttons held by the helper. Failed releases remain
tracked for another attempt while the helper runs. The explicit `releaseInput`
command reports failure, and shutdown reports rejected releases on standard
error. Windows restrictions can prevent cleanup from succeeding.
Clients must not automatically retry an interrupted command: its input may
already have reached an application.
The helper opens no network listener and performs no runtime compilation.
