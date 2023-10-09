const libnut = require("..");
const os = require("os");

// TODO: Need tests for keyToggle, typeString, typeStringDelayed, and setKeyboardDelay.

describe("Keyboard", () => {
    it("Tap a key.", function () {
        expect(() => libnut.keyTap("a")).not.toThrow();
        expect(() => libnut.keyTap("a", "control")).not.toThrow();
        expect(() => libnut.keyTap()).toThrowError(/A string was expected/);
    });

    it("Tap a key with modifier array.", function () {
        expect(() => libnut.keyTap("a", ["alt", "control"])).not.toThrow();
    });

    // This it won't fail if there's an issue, but it will help you identify an issue if ran locally.
    it("Tap all keys.", function () {
        const chars = "abcdefghijklmnopqrstuvwxyz1234567890,./;'[]\\".split("");

        for (const x in chars) {
            expect(() => libnut.keyTap(chars[x])).not.toThrow();
        }
    });

    // This it won't fail if there's an issue, but it will help you identify an issue if ran locally.
    it("Tap all numpad keys.", function () {
        const nums = "0123456789".split("");

        for (const x in nums) {
            expect(() => libnut.keyTap("numpad_" + nums[x])).not.toThrow();
        }
    });
});
