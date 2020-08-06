const Application = require("spectron").Application;
const electronPath = require("electron");
const libnut = require("../..");
const { POS_X, POS_Y, WIDTH, HEIGTH, TITLE } = require("./constants");

let app;
const APP_TIMEOUT = 10000;

beforeAll(async () => {
    app = new Application({
        path: electronPath,
        args: ['main.js'],
        startTimeout: APP_TIMEOUT,
        waitTimeout: APP_TIMEOUT,
    });
    await app.start();
    await app.client.waitUntilWindowLoaded();
    await app.client.waitUntil(async () => {
        await app.browserWindow.setAlwaysOnTop(true);
        await app.browserWindow.focus();
        return await app.browserWindow.isFocused();
    });
});

describe("getWindows", () => {
    it("should list our started application window", () => {
        // GIVEN

        // WHEN
        const windowNames = libnut.getWindows().map(handle => libnut.getWindowTitle(handle));

        // THEN
        expect(windowNames).toContain(TITLE);
    });
});

describe("getActiveWindow", () => {
    it("should return our started application window", () => {
        // GIVEN

        // WHEN
        const activeWindowHandle = libnut.getActiveWindow();
        const activeWindowName = libnut.getWindowTitle(activeWindowHandle);

        // THEN
        expect(activeWindowName).toBe(TITLE);
    });

    it("should determine correct coordinates for our application", () => {
        // GIVEN

        // WHEN
        const activeWindowHandle = libnut.getActiveWindow();
        const activeWindowRect = libnut.getWindowRect(activeWindowHandle);

        // THEN
        expect(activeWindowRect.x).toBe(POS_X);
        expect(activeWindowRect.y).toBe(POS_Y);
        expect(activeWindowRect.width).toBe(WIDTH);
        expect(activeWindowRect.height).toBe(HEIGTH);
    });
});

afterAll(async () => {
    if (app && app.isRunning()) {
        await app.stop();
    }
});