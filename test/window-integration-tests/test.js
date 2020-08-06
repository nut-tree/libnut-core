const Application = require("spectron").Application;
const electronPath = require("electron");
const libnut = require("../..");
const { POS_X, POS_Y, WIDTH, HEIGTH, TITLE } = require("./constants");

let app;
const APP_TIMEOUT = 10000;

beforeEach(() => {
    app = new Application({
        path: electronPath,
        args: ['main.js'],
        startTimeout: APP_TIMEOUT,
        waitTimeout: APP_TIMEOUT,
    });
    return app.start();
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

afterEach(() => {
    if (app && app.isRunning()) {
        return app.stop();
    }
});