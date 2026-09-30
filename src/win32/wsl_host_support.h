#pragma once
#define NOMINMAX
#include "../keycode.h"
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Data.Json.h>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>

namespace wsl {
constexpr size_t maxHeaderBytes = 1024 * 1024;

// Key represents US keyboard keys, not characters in the active application
// layout. Text is sent separately using KEYEVENTF_UNICODE.
inline WORD keyCode(const std::string& name) {
    static const std::map<std::string, int> keys = {
        {"escape", K_ESCAPE}, {"backspace", K_BACKSPACE}, {"tab", K_TAB},
        {"return", K_RETURN}, {"enter", K_ENTER}, {"space", K_SPACE},
        {"delete", K_DELETE}, {"insert", K_INSERT}, {"home", K_HOME},
        {"end", K_END}, {"pageup", K_PAGEUP}, {"pagedown", K_PAGEDOWN},
        {"left", K_LEFT}, {"right", K_RIGHT}, {"up", K_UP}, {"down", K_DOWN},
        {"shift", K_SHIFT}, {"right_shift", K_RIGHTSHIFT},
        {"control", K_CONTROL}, {"right_control", K_RIGHTCONTROL},
        {"alt", K_ALT}, {"right_alt", K_RIGHTALT},
        {"command", K_WIN}, {"win", K_WIN}, {"cmd", K_WIN}, {"meta", K_WIN},
        {"right_win", K_RIGHTWIN}, {"right_cmd", K_RIGHTWIN}, {"right_meta", K_RIGHTWIN},
        {"caps_lock", K_CAPSLOCK}, {"num_lock", K_NUMLOCK}, {"scroll_lock", K_SCROLL_LOCK},
        {"printscreen", K_PRINTSCREEN}, {"pause", K_PAUSE}, {"menu", K_MENU},
        {"clear", K_CLEAR}, {"add", K_ADD}, {"subtract", K_SUBTRACT},
        {"multiply", K_MULTIPLY}, {"divide", K_DIVIDE}, {"numpad_decimal", K_NUMPAD_DECIMAL},
        {"audio_mute", K_AUDIO_VOLUME_MUTE}, {"audio_vol_down", K_AUDIO_VOLUME_DOWN},
        {"audio_vol_up", K_AUDIO_VOLUME_UP}, {"audio_play", K_AUDIO_PLAY},
        {"audio_pause", K_AUDIO_PAUSE}, {"audio_stop", K_AUDIO_STOP},
        {"audio_prev", K_AUDIO_PREV}, {"audio_next", K_AUDIO_NEXT},
        {",", K_COMMA}, {".", K_PERIOD}, {"/", K_SLASH}, {";", K_SEMICOLON},
        {"'", K_QUOTE}, {"[", K_LEFTBRACKET}, {"]", K_RIGHTBRACKET},
        {"\\", K_BACKSLASH}, {"-", K_MINUS}, {"=", K_EQUAL}, {"`", K_GRAVE}
    };
    auto found = keys.find(name);
    if (found != keys.end()) return static_cast<WORD>(found->second);
    if (name.size() == 1 && name[0] >= 'a' && name[0] <= 'z')
        return static_cast<WORD>(VK_A + name[0] - 'a');
    if (name.size() == 1 && name[0] >= '0' && name[0] <= '9')
        return static_cast<WORD>(VK_0 + name[0] - '0');
    if (name.size() >= 2 && name[0] == 'f') {
        size_t consumed = 0;
        int value = std::stoi(name.substr(1), &consumed);
        if (consumed == name.size() - 1 && value >= 1 && value <= 24)
            return static_cast<WORD>(VK_F1 + value - 1);
    }
    if (name.size() == 8 && name.substr(0, 7) == "numpad_" && name[7] >= '0' && name[7] <= '9')
        return static_cast<WORD>(VK_NUMPAD0 + name[7] - '0');
    throw std::runtime_error("Unsupported Windows key: " + name);
}

// A failed release remains owned by the helper so a later attempt can retry it.
template <typename Input, typename Release>
bool releaseHeldInputs(std::set<Input>& held, Release release) {
    for (auto input = held.begin(); input != held.end();) {
        if (release(*input)) input = held.erase(input);
        else ++input;
    }
    return held.empty();
}

// Window requests must not wait on a foreign application's message thread.
// Poll only local window state, with a bounded, disconnect-aware wait.
template <typename Ready, typename Pause, typename Clock>
bool waitForState(Ready ready, Pause pause, Clock now, DWORD timeout = 5000) {
    ULONGLONG deadline = now() + timeout;
    for (;;) {
        if (ready()) return true;
        ULONGLONG current = now();
        if (current >= deadline) return false;
        pause(static_cast<DWORD>(std::min<ULONGLONG>(10, deadline - current)));
    }
}

inline std::string responseLine(winrt::Windows::Data::Json::JsonObject response, std::vector<uint8_t>& pixels) {
    using namespace winrt::Windows::Data::Json;
    response.SetNamedValue(L"byteLength", JsonValue::CreateNumberValue(static_cast<double>(pixels.size())));
    std::string line = winrt::to_string(response.Stringify());
    if (line.size() > maxHeaderBytes) {
        // Keep the request ID, but do not transmit a result the client cannot read.
        if (response.HasKey(L"result")) response.Remove(L"result");
        response.SetNamedValue(L"error", JsonValue::CreateStringValue(L"Windows host response exceeds the 1 MiB JSON limit"));
        response.SetNamedValue(L"byteLength", JsonValue::CreateNumberValue(0));
        pixels.clear();
        line = winrt::to_string(response.Stringify());
    }
    return line;
}
}
