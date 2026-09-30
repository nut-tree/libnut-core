// Deterministic support tests. No capture, keyboard/mouse input or window changes.
#include "../src/win32/wsl_host_support.h"
#include <iostream>

using namespace winrt;
using namespace Windows::Data::Json;

void expect(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

void testKeys() {
    const std::map<std::string, int> punctuation = {
        {"[", VK_OEM_4}, {"]", VK_OEM_6}, {";", VK_OEM_1}, {"'", VK_OEM_7},
        {",", VK_OEM_COMMA}, {".", VK_OEM_PERIOD}, {"/", VK_OEM_2},
        {"\\", VK_OEM_5}, {"-", VK_OEM_MINUS}, {"=", VK_OEM_PLUS}, {"`", VK_OEM_3}
    };
    for (const auto& mapping : punctuation) expect(wsl::keyCode(mapping.first) == mapping.second, "Incorrect US punctuation key");
    for (char key = 'a'; key <= 'z'; ++key) expect(wsl::keyCode(std::string(1, key)) == VK_A + key - 'a', "Incorrect letter key");
    for (char key = '0'; key <= '9'; ++key) expect(wsl::keyCode(std::string(1, key)) == VK_0 + key - '0', "Incorrect number key");
    expect(wsl::keyCode("f24") == VK_F24, "Incorrect F24 key");
    expect(wsl::keyCode("numpad_9") == VK_NUMPAD9, "Incorrect keypad key");
    expect(wsl::keyCode("right_control") == VK_RCONTROL, "Incorrect right modifier");
    for (const std::string name : {"unsupported", "f25", "A", "!"}) {
        bool rejected = false;
        try { wsl::keyCode(name); } catch (const std::exception&) { rejected = true; }
        expect(rejected, "Unsupported key was accepted");
    }
}

void testReleases() {
    std::set<WORD> keys{VK_A, VK_LCONTROL};
    std::vector<WORD> attempted;
    bool released = wsl::releaseHeldInputs(keys, [&](WORD key) {
        attempted.push_back(key);
        return key == VK_A;
    });
    expect(!released, "A failed release reported success");
    expect(keys == std::set<WORD>{VK_LCONTROL}, "A failed key release was forgotten");
    expect(attempted.size() == 2, "Cleanup stopped after a failed key");
    expect(wsl::releaseHeldInputs(keys, [](WORD) { return true; }), "Retry did not clear the held key");
    expect(keys.empty(), "Successful retry retained the key");
    std::set<int> buttons{0, 2};
    expect(!wsl::releaseHeldInputs(buttons, [](int) { return false; }), "Failed button releases reported success");
    expect(buttons == std::set<int>({0, 2}), "Failed button releases were forgotten");
    expect(wsl::releaseHeldInputs(buttons, [](int) { return true; }), "Button retry failed");
}

void testResponses() {
    auto response = [](const hstring& value) {
        JsonObject result;
        result.SetNamedValue(L"id", JsonValue::CreateNumberValue(7));
        result.SetNamedValue(L"result", JsonValue::CreateStringValue(value));
        return result;
    };
    std::vector<uint8_t> pixels;
    JsonObject overhead = response(L"");
    size_t base = wsl::responseLine(overhead, pixels).size();
    std::wstring boundary(wsl::maxHeaderBytes - base, L'a');
    std::string exact = wsl::responseLine(response(hstring(boundary)), pixels);
    expect(exact.size() == wsl::maxHeaderBytes, "Exact header boundary was not accepted");
    expect(JsonObject::Parse(to_hstring(exact)).HasKey(L"result"), "Accepted result was replaced");
    boundary.push_back(L'a');
    JsonObject oversized = JsonObject::Parse(to_hstring(wsl::responseLine(response(hstring(boundary)), pixels)));
    expect(oversized.GetNamedNumber(L"id") == 7 && oversized.HasKey(L"error"), "Oversized reply lost its bounded error or ID");
    expect(!oversized.HasKey(L"result") && oversized.GetNamedNumber(L"byteLength") == 0, "Oversized reply retained payload");
    // JSON escaping and UTF-8 bytes, not UTF-16 character counts, determine size.
    for (const std::wstring text : {std::wstring(600000, L'"'), std::wstring(600000, L'\u00e9')}) {
        pixels = {1, 2, 3, 4};
        std::string line = wsl::responseLine(response(hstring(text)), pixels);
        expect(line.size() < wsl::maxHeaderBytes && pixels.empty(), "Oversized reply retained binary data");
        expect(JsonObject::Parse(to_hstring(line)).HasKey(L"error"), "Encoded header limit was not enforced");
    }
    expect(JsonObject::Parse(to_hstring(wsl::responseLine(response(L"next command"), pixels))).GetNamedString(L"result") == L"next command",
        "A later response failed after an oversized one");
    JsonObject error;
    error.SetNamedValue(L"id", JsonValue::CreateNumberValue(8));
    error.SetNamedValue(L"error", JsonValue::CreateStringValue(hstring(std::wstring(wsl::maxHeaderBytes + 1, L'a'))));
    std::string boundedError = wsl::responseLine(error, pixels);
    expect(boundedError.size() < wsl::maxHeaderBytes && JsonObject::Parse(to_hstring(boundedError)).GetNamedNumber(L"id") == 8,
        "An oversized error could not be replaced without a result field");
}

void testWindowWaits() {
    ULONGLONG time = 0;
    int polls = 0;
    expect(!wsl::waitForState([&] { ++polls; return false; }, [&](DWORD duration) { time += duration; }, [&] { return time; }),
        "A hung window reported completion");
    expect(time == 5000 && polls > 1, "Window wait was not bounded");
    time = 0;
    expect(wsl::waitForState([&] { return time >= 20; }, [&](DWORD duration) { time += duration; }, [&] { return time; }),
        "Completed asynchronous state was not observed");
    expect(time == 20, "Completed state waited until timeout");
    bool disconnected = false;
    try {
        wsl::waitForState([] { return false; }, [](DWORD) { throw std::runtime_error("disconnected"); }, [] { return ULONGLONG{0}; });
    } catch (const std::runtime_error&) { disconnected = true; }
    expect(disconnected, "Window wait swallowed caller disconnection");
}

int main() {
    try {
        init_apartment(apartment_type::single_threaded);
        testKeys(); testReleases(); testResponses(); testWindowWaits();
        std::cout << "Windows host support tests passed (keys, failed releases, response limits, bounded window waits)\n";
        return 0;
    } catch (const hresult_error& error) {
        std::cerr << to_string(error.message()) << '\n';
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
    }
    return 1;
}
