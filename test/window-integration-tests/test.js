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
    await app.browserWindow.minimize();
    await app.browserWindow.restore();
    await app.browserWindow.focus();
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

    it("should determine correct coordinates for our application after moving the window", async () => {
        // GIVEN
        const xPosition = 42;
        const yPosition = 23;
        await app.browserWindow.setPosition(xPosition, yPosition);

        // WHEN
        const activeWindowHandle = libnut.getActiveWindow();
        const activeWindowRect = libnut.getWindowRect(activeWindowHandle);

        // THEN
        expect(activeWindowRect.x).toBe(xPosition);
        expect(activeWindowRect.y).toBe(yPosition);
    });

    it("should determine correct window size for our application after resizing the window", async () => {
        // GIVEN
        const newWidth = 400;
        const newHeight = 250;
        await app.browserWindow.setSize(newWidth, newHeight);

        // WHEN
        const activeWindowHandle = libnut.getActiveWindow();
        const activeWindowRect = libnut.getWindowRect(activeWindowHandle);

        // THEN
        expect(activeWindowRect.width).toBe(newWidth);
        expect(activeWindowRect.height).toBe(newHeight);
    });
});

afterAll(async () => {
    if (app && app.isRunning()) {
        await app.stop();
    }
});
