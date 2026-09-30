#include "wsl_host_support.h"
#include <fcntl.h>
#include <io.h>
#include <algorithm>
#include <cmath>
#include <climits>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

using namespace winrt;
using namespace Windows::Data::Json;

namespace {
constexpr size_t maxRequestBytes = 1024 * 1024;
constexpr size_t maxImageBytes = 128 * 1024 * 1024;
constexpr int protocolVersion = 1;

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

void checked(BOOL result, const char* operation) {
    if (!result) throw std::runtime_error(std::string(operation) + " failed (Windows error " + std::to_string(GetLastError()) + ")");
}

double number(const JsonObject& value, const wchar_t* name, double minimum, double maximum) {
    double result = value.GetNamedNumber(name);
    require(std::isfinite(result) && result >= minimum && result <= maximum, "Number outside supported range");
    return result;
}

int integer(const JsonObject& value, const wchar_t* name, int minimum = INT_MIN, int maximum = INT_MAX) {
    double result = number(value, name, minimum, maximum);
    require(std::floor(result) == result, "Expected an integer");
    return static_cast<int>(result);
}

JsonObject point(int x, int y) {
    JsonObject result;
    result.SetNamedValue(L"x", JsonValue::CreateNumberValue(x));
    result.SetNamedValue(L"y", JsonValue::CreateNumberValue(y));
    return result;
}

JsonObject rectangle(int x, int y, int width, int height) {
    JsonObject result = point(x, y);
    result.SetNamedValue(L"width", JsonValue::CreateNumberValue(width));
    result.SetNamedValue(L"height", JsonValue::CreateNumberValue(height));
    return result;
}

void send(INPUT input) {
    require(SendInput(1, &input, sizeof(input)) == 1, "SendInput failed; input may be blocked by an elevated application or a non-interactive desktop");
}

void checkConnection() {
    HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
    if (GetFileType(input) == FILE_TYPE_PIPE) {
        DWORD available = 0;
        if (!PeekNamedPipe(input, nullptr, 0, nullptr, &available, nullptr))
            throw std::runtime_error("The caller closed the command pipe");
    }
}

void wait(DWORD milliseconds) {
    checkConnection();
    while (milliseconds > 0) {
        DWORD chunk = std::min<DWORD>(milliseconds, 100);
        Sleep(chunk);
        milliseconds -= chunk;
        checkConnection();
    }
}

class Host {
    HWND owner = nullptr;
    std::set<WORD> heldKeys;
    std::set<int> heldButtons;

    static DWORD buttonFlag(int button, bool down) {
        static const DWORD flags[3][2] = {
            {MOUSEEVENTF_LEFTUP, MOUSEEVENTF_LEFTDOWN},
            {MOUSEEVENTF_MIDDLEUP, MOUSEEVENTF_MIDDLEDOWN},
            {MOUSEEVENTF_RIGHTUP, MOUSEEVENTF_RIGHTDOWN}
        };
        require(button >= 0 && button < 3, "Invalid mouse button");
        return flags[button][down ? 1 : 0];
    }

    void button(int value, bool down) {
        INPUT input{};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = buttonFlag(value, down);
        // Track before sending, so shutdown can release input after uncertain failures.
        if (down) heldButtons.insert(value);
        send(input);
        if (!down) heldButtons.erase(value);
    }

    static INPUT keyboardInput(WORD code, bool down) {
        INPUT input{};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = code;
        input.ki.wScan = static_cast<WORD>(MapVirtualKeyW(code, MAPVK_VK_TO_VSC));
        switch (code) {
            case VK_RCONTROL: case VK_RMENU: case VK_INSERT: case VK_DELETE:
            case VK_HOME: case VK_END: case VK_PRIOR: case VK_NEXT:
            case VK_LEFT: case VK_RIGHT: case VK_UP: case VK_DOWN:
            case VK_LWIN: case VK_RWIN: case VK_APPS: case VK_DIVIDE:
                input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
        }
        if (!down) input.ki.dwFlags |= KEYEVENTF_KEYUP;
        return input;
    }

    void key(WORD code, bool down) {
        if (down) heldKeys.insert(code);
        send(keyboardInput(code, down));
        if (!down) heldKeys.erase(code);
    }

    static HWND window(const JsonObject& args) {
        double handle = number(args, L"handle", 0, 9007199254740991.0);
        require(std::floor(handle) == handle, "Invalid window handle");
        HWND result = reinterpret_cast<HWND>(static_cast<uintptr_t>(handle));
        require(IsWindow(result), "Window handle is no longer valid");
        return result;
    }

    static RECT windowRect(HWND hwnd) {
        RECT rect{};
        checked(GetWindowRect(hwnd, &rect), "GetWindowRect");
        return rect;
    }

    template <typename Ready>
    static bool waitForWindow(HWND hwnd, Ready ready) {
        checkConnection();
        return wsl::waitForState([&] {
            require(IsWindow(hwnd), "Window handle is no longer valid");
            return ready();
        }, [](DWORD milliseconds) { wait(milliseconds); }, [] { return GetTickCount64(); });
    }

    static bool showWindow(HWND hwnd, bool minimize) {
        if (!ShowWindowAsync(hwnd, minimize ? SW_MINIMIZE : SW_RESTORE)) return false;
        return waitForWindow(hwnd, [&] { return (IsIconic(hwnd) != FALSE) == minimize; });
    }

    JsonValue clipboard(const JsonObject& args, bool write) {
        checked(OpenClipboard(owner), "OpenClipboard");
        struct Close { ~Close() { CloseClipboard(); } } close;
        if (!write) {
            if (!IsClipboardFormatAvailable(CF_UNICODETEXT)) return JsonValue::CreateStringValue(L"");
            HANDLE data = GetClipboardData(CF_UNICODETEXT);
            require(data != nullptr, "GetClipboardData failed");
            const wchar_t* text = static_cast<const wchar_t*>(GlobalLock(data));
            require(text != nullptr, "GlobalLock failed");
            struct Unlock { HANDLE data; ~Unlock() { GlobalUnlock(data); } } unlock{data};
            size_t count = GlobalSize(data) / sizeof(wchar_t);
            size_t length = 0;
            while (length < count && text[length]) ++length;
            require(length < count, "Clipboard text is not terminated");
            return JsonValue::CreateStringValue(hstring(text, static_cast<uint32_t>(length)));
        }
        hstring text = args.GetNamedString(L"text");
        size_t size = (static_cast<size_t>(text.size()) + 1) * sizeof(wchar_t);
        HGLOBAL data = GlobalAlloc(GMEM_MOVEABLE, size);
        require(data != nullptr, "GlobalAlloc failed");
        void* buffer = GlobalLock(data);
        if (!buffer) { GlobalFree(data); throw std::runtime_error("GlobalLock failed"); }
        memcpy(buffer, text.c_str(), size);
        GlobalUnlock(data);
        if (!EmptyClipboard() || !SetClipboardData(CF_UNICODETEXT, data)) {
            GlobalFree(data);
            throw std::runtime_error("Writing clipboard text failed");
        }
        return JsonValue::CreateNullValue();
    }

    static void highlight(const JsonObject& args) {
        int x = integer(args, L"x"), y = integer(args, L"y");
        int width = integer(args, L"width", 1), height = integer(args, L"height", 1);
        int duration = integer(args, L"duration", 0);
        double opacity = number(args, L"opacity", 0, 1);
        HWND overlay = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            L"LibnutWslHost", L"", WS_POPUP, x, y, width, height, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
        require(overlay != nullptr, "Creating highlight overlay failed");
        struct Destroy { HWND hwnd; ~Destroy() { DestroyWindow(hwnd); } } destroy{overlay};
        checked(SetLayeredWindowAttributes(overlay, 0, static_cast<BYTE>(255 * opacity), LWA_ALPHA), "SetLayeredWindowAttributes");
        ShowWindow(overlay, SW_SHOWNOACTIVATE);
        UpdateWindow(overlay);
        ULONGLONG deadline = GetTickCount64() + duration;
        while (GetTickCount64() < deadline) {
            MSG message{};
            while (PeekMessageW(&message, overlay, 0, 0, PM_REMOVE)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
            wait(static_cast<DWORD>(std::min<ULONGLONG>(10, deadline - std::min(deadline, GetTickCount64()))));
        }
    }

    static JsonObject capture(const JsonObject& args, std::vector<uint8_t>& pixels) {
        int screenWidth = GetSystemMetrics(SM_CXSCREEN), screenHeight = GetSystemMetrics(SM_CYSCREEN);
        int x = args.HasKey(L"x") ? integer(args, L"x", 0, screenWidth - 1) : 0;
        int y = args.HasKey(L"y") ? integer(args, L"y", 0, screenHeight - 1) : 0;
        int width = args.HasKey(L"width") ? integer(args, L"width", 1, screenWidth - x) : screenWidth;
        int height = args.HasKey(L"height") ? integer(args, L"height", 1, screenHeight - y) : screenHeight;
        require(width > 0 && height > 0, "No interactive primary display is available");
        size_t bytes = static_cast<size_t>(width) * height * 4;
        require(bytes <= maxImageBytes, "Screenshot exceeds the protocol image limit");
        HDC screen = GetDC(nullptr);
        require(screen != nullptr, "GetDC failed");
        struct Release { HDC dc; ~Release() { ReleaseDC(nullptr, dc); } } release{screen};
        HDC memory = CreateCompatibleDC(screen);
        require(memory != nullptr, "CreateCompatibleDC failed");
        struct DeleteDc { HDC dc; ~DeleteDc() { DeleteDC(dc); } } deleteDc{memory};
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = width;
        info.bmiHeader.biHeight = -height;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        void* data = nullptr;
        HBITMAP bitmap = CreateDIBSection(screen, &info, DIB_RGB_COLORS, &data, nullptr, 0);
        require(bitmap != nullptr && data != nullptr, "CreateDIBSection failed");
        struct DeleteBitmap { HBITMAP bitmap; ~DeleteBitmap() { DeleteObject(bitmap); } } deleteBitmap{bitmap};
        HGDIOBJ previous = SelectObject(memory, bitmap);
        require(previous != nullptr && previous != HGDI_ERROR, "SelectObject failed");
        struct Restore { HDC dc; HGDIOBJ object; ~Restore() { SelectObject(dc, object); } } restore{memory, previous};
        checked(BitBlt(memory, 0, 0, width, height, screen, x, y, SRCCOPY | CAPTUREBLT), "BitBlt");
        checked(GdiFlush(), "GdiFlush");
        const uint8_t* start = static_cast<const uint8_t*>(data);
        pixels.assign(start, start + bytes);
        for (size_t index = 3; index < bytes; index += 4) pixels[index] = 255;
        JsonObject result = rectangle(0, 0, width, height);
        result.SetNamedValue(L"byteWidth", JsonValue::CreateNumberValue(static_cast<double>(width) * 4));
        return result;
    }

public:
    Host() {
        if (!SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) {
            require(SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2) != nullptr,
                "Cannot configure physical-pixel desktop coordinates");
        }
        WNDCLASSW type{};
        type.lpfnWndProc = DefWindowProcW;
        type.hInstance = GetModuleHandleW(nullptr);
        type.lpszClassName = L"LibnutWslHost";
        type.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        require(RegisterClassW(&type) != 0, "RegisterClass failed");
        owner = CreateWindowW(type.lpszClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, type.hInstance, nullptr);
        require(owner != nullptr, "Creating clipboard owner failed");
    }

    ~Host() {
        if (!releaseInput()) std::cerr << "libnut Windows host: Windows rejected releasing held input during shutdown\n";
        if (owner) DestroyWindow(owner);
        UnregisterClassW(L"LibnutWslHost", GetModuleHandleW(nullptr));
    }

    bool releaseInput() noexcept {
        bool keysReleased = wsl::releaseHeldInputs(heldKeys, [](WORD code) {
            INPUT input = keyboardInput(code, false);
            return SendInput(1, &input, sizeof(input)) == 1;
        });
        bool buttonsReleased = wsl::releaseHeldInputs(heldButtons, [](int value) {
            INPUT input{}; input.type = INPUT_MOUSE; input.mi.dwFlags = buttonFlag(value, false);
            return SendInput(1, &input, sizeof(input)) == 1;
        });
        return keysReleased && buttonsReleased;
    }

    IJsonValue execute(const hstring& command, const JsonObject& args, std::vector<uint8_t>& pixels) {
        int postDelay = args.HasKey(L"delay") ? integer(args, L"delay", 0, 60000) : 0;
        if (command == L"hello") {
            JsonObject result;
            result.SetNamedValue(L"protocol", JsonValue::CreateNumberValue(protocolVersion));
            result.SetNamedValue(L"pid", JsonValue::CreateNumberValue(GetCurrentProcessId()));
            return result;
        }
        if (command == L"screenSize") return rectangle(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
        if (command == L"capture") return capture(args, pixels);
        if (command == L"highlight") { highlight(args); return JsonValue::CreateNullValue(); }
        if (command == L"cursorPosition") {
            POINT position{}; checked(GetCursorPos(&position), "GetCursorPos");
            return point(position.x, position.y);
        }
        if (command == L"moveMouse") {
            checked(SetCursorPos(integer(args, L"x"), integer(args, L"y")), "SetCursorPos");
        } else if (command == L"mouseButton") {
            button(integer(args, L"button", 0, 2), args.GetNamedBoolean(L"down"));
        } else if (command == L"click") {
            int value = integer(args, L"button", 0, 2);
            int count = integer(args, L"count", 1, 2);
            try {
                for (int index = 0; index < count; ++index) {
                    button(value, true); button(value, false);
                    if (index + 1 < count) wait(std::min<DWORD>(50, GetDoubleClickTime() / 2));
                }
            } catch (...) { releaseInput(); throw; }
        } else if (command == L"scroll") {
            int amount = integer(args, L"amount");
            INPUT input{}; input.type = INPUT_MOUSE;
            input.mi.dwFlags = args.GetNamedBoolean(L"horizontal") ? MOUSEEVENTF_HWHEEL : MOUSEEVENTF_WHEEL;
            input.mi.mouseData = static_cast<DWORD>(amount);
            send(input);
        } else if (command == L"keys" || command == L"keyClick") {
            JsonArray names = args.GetNamedArray(L"keys");
            require(names.Size() > 0 && names.Size() <= 32, "Expected 1 to 32 keys");
            std::vector<WORD> codes;
            for (const auto& name : names) codes.push_back(wsl::keyCode(to_string(name.GetString())));
            bool down = command == L"keyClick" || args.GetNamedBoolean(L"down");
            if (!down) std::reverse(codes.begin(), codes.end());
            try {
                for (WORD code : codes) key(code, down);
                if (command == L"keyClick") {
                    std::reverse(codes.begin(), codes.end());
                    for (WORD code : codes) key(code, false);
                }
            }
            catch (...) { releaseInput(); throw; }
        } else if (command == L"type") {
            hstring text = args.GetNamedString(L"text");
            for (wchar_t character : text) {
                checkConnection();
                INPUT input{}; input.type = INPUT_KEYBOARD; input.ki.wScan = character;
                input.ki.dwFlags = KEYEVENTF_UNICODE;
                send(input);
                input.ki.dwFlags |= KEYEVENTF_KEYUP;
                send(input);
                if (postDelay) wait(postDelay);
            }
            return JsonValue::CreateNullValue();
        } else if (command == L"clipboardRead") return clipboard(args, false);
        else if (command == L"clipboardWrite") return clipboard(args, true);
        else if (command == L"clipboardHasText") return JsonValue::CreateBooleanValue(!clipboard(args, false).GetString().empty());
        else if (command == L"clipboardClear") {
            checked(OpenClipboard(owner), "OpenClipboard");
            BOOL result = EmptyClipboard();
            CloseClipboard();
            return JsonValue::CreateBooleanValue(result != FALSE);
        }
        else if (command == L"windows") {
            std::vector<HWND> handles;
            checked(EnumWindows([](HWND hwnd, LPARAM context) -> BOOL {
                reinterpret_cast<std::vector<HWND>*>(context)->push_back(hwnd); return TRUE;
            }, reinterpret_cast<LPARAM>(&handles)), "EnumWindows");
            JsonArray result;
            for (HWND hwnd : handles) result.Append(JsonValue::CreateNumberValue(static_cast<double>(reinterpret_cast<uintptr_t>(hwnd))));
            return result;
        } else if (command == L"activeWindow") return JsonValue::CreateNumberValue(static_cast<double>(reinterpret_cast<uintptr_t>(GetForegroundWindow())));
        else if (command == L"windowTitle") {
            HWND hwnd = window(args);
            std::vector<wchar_t> text(static_cast<size_t>(GetWindowTextLengthW(hwnd)) + 1);
            int size = GetWindowTextW(hwnd, text.data(), static_cast<int>(text.size()));
            return JsonValue::CreateStringValue(hstring(text.data(), size));
        } else if (command == L"windowRegion") {
            RECT rect = windowRect(window(args));
            return rectangle(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
        } else if (command == L"focusWindow") {
            HWND hwnd = window(args);
            if (IsIconic(hwnd) && !showWindow(hwnd, false)) return JsonValue::CreateBooleanValue(false);
            SetForegroundWindow(hwnd);
            return JsonValue::CreateBooleanValue(waitForWindow(hwnd, [&] { return GetForegroundWindow() == hwnd; }));
        } else if (command == L"moveWindow" || command == L"resizeWindow") {
            HWND hwnd = window(args); RECT rect = windowRect(hwnd);
            int x = command == L"moveWindow" ? integer(args, L"x") : rect.left;
            int y = command == L"moveWindow" ? integer(args, L"y") : rect.top;
            int width = command == L"resizeWindow" ? integer(args, L"width", 1) : rect.right - rect.left;
            int height = command == L"resizeWindow" ? integer(args, L"height", 1) : rect.bottom - rect.top;
            UINT flags = SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOZORDER |
                (command == L"moveWindow" ? SWP_NOSIZE : SWP_NOMOVE);
            if (!SetWindowPos(hwnd, nullptr, x, y, width, height, flags)) return JsonValue::CreateBooleanValue(false);
            return JsonValue::CreateBooleanValue(waitForWindow(hwnd, [&] {
                RECT current = windowRect(hwnd);
                return command == L"moveWindow" ? current.left == x && current.top == y :
                    current.right - current.left == width && current.bottom - current.top == height;
            }));
        } else if (command == L"minimizeWindow" || command == L"restoreWindow") {
            HWND hwnd = window(args);
            return JsonValue::CreateBooleanValue(showWindow(hwnd, command == L"minimizeWindow"));
        } else if (command == L"releaseInput") require(releaseInput(), "Windows rejected releasing held input; failed releases remain tracked for another attempt");
        else throw std::runtime_error("Unknown Windows host command");
        if (postDelay) wait(postDelay);
        return JsonValue::CreateNullValue();
    }
};

bool readRequest(std::string& line) {
    line.clear();
    char value;
    while (std::cin.get(value)) {
        if (value == '\n') return true;
        require(line.size() < maxRequestBytes, "Request exceeds the protocol limit");
        line.push_back(value);
    }
    require(line.empty(), "Incomplete request at end of input");
    return false;
}
}

int main() {
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
    try {
        init_apartment(apartment_type::single_threaded);
        Host host;
        std::string line;
        while (readRequest(line)) {
            JsonObject response;
            std::vector<uint8_t> pixels;
            try {
                JsonObject request = JsonObject::Parse(to_hstring(line));
                double id = number(request, L"id", 1, 9007199254740991.0);
                require(std::floor(id) == id, "Invalid request ID");
                response.SetNamedValue(L"id", JsonValue::CreateNumberValue(id));
                response.SetNamedValue(L"result", host.execute(request.GetNamedString(L"command"), request.GetNamedObject(L"args"), pixels));
            } catch (const hresult_error& error) {
                response.SetNamedValue(L"error", JsonValue::CreateStringValue(error.message()));
                pixels.clear();
            } catch (const std::exception& error) {
                response.SetNamedValue(L"error", JsonValue::CreateStringValue(to_hstring(error.what())));
                pixels.clear();
            }
            std::cout << wsl::responseLine(response, pixels) << '\n';
            if (!pixels.empty()) std::cout.write(reinterpret_cast<const char*>(pixels.data()), static_cast<std::streamsize>(pixels.size()));
            std::cout.flush();
            require(std::cout.good(), "Writing the response failed");
        }
        return 0;
    } catch (const hresult_error& error) {
        std::cerr << "libnut Windows host: " << to_string(error.message()) << '\n';
    } catch (const std::exception& error) {
        std::cerr << "libnut Windows host: " << error.what() << '\n';
    }
    return 1;
}
