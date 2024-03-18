const {_electron: electron} = require('playwright');
const libnut = require("../..");
const {POS_X, POS_Y, WIDTH, HEIGTH, TITLE} = require("./constants");

const sleep = async (ms) => {
    return new Promise(resolve => setTimeout(resolve, ms));
};

let app;
let page;
let windowHandle;

const APP_TIMEOUT = 10000;

beforeEach(async () => {
    app = await electron.launch({args: ['main.js']});
    page = await app.firstWindow({timeout: APP_TIMEOUT});
    windowHandle = await app.browserWindow(page);
    await page.waitForLoadState("domcontentloaded");
    await windowHandle.evaluate((win) => {
        win.minimize();
        win.restore();
        win.focus();
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

    it("should determine correct coordinates for our application after moving the window", async () => {
        // GIVEN
        const xPosition = 250;
        const yPosition = 300;

        // WHEN
        const activeWindowHandle = libnut.getActiveWindow();
        libnut.moveWindow(activeWindowHandle, {x: xPosition, y: yPosition});
        await sleep(100);
        const activeWindowRect = libnut.getWindowRect(activeWindowHandle);

        // THEN
        expect(activeWindowRect.x).toBe(xPosition);
        expect(activeWindowRect.y).toBe(yPosition);
    });

    it("should determine correct window size for our application after resizing the window", async () => {
        // GIVEN
        const newWidth = 800;
        const newHeight = 650;

        // WHEN
        const activeWindowHandle = libnut.getActiveWindow();
        libnut.resizeWindow(activeWindowHandle, {width: newWidth, height: newHeight});
        await sleep(100);
        const activeWindowRect = libnut.getWindowRect(activeWindowHandle);

        // THEN
        expect(activeWindowRect.width).toBe(newWidth);
        expect(activeWindowRect.height).toBe(newHeight);
    });
});

describe("focusWindow", () => {
    it("should properly focus the correct window", async () => {
        // GIVEN
        const openWindowHandle = libnut.getActiveWindow();

        // WHEN
        const secondApp = await electron.launch({args: ['second.js']});
        const secondPage = await secondApp.firstWindow({timeout: APP_TIMEOUT});
        const handle = await secondApp.browserWindow(secondPage);
        await secondPage.waitForLoadState("domcontentloaded");
        await handle.evaluate((win) => {
            win.minimize();
            win.restore();
            win.focus();
        });

        const result = libnut.focusWindow(openWindowHandle);

        // THEN
        await sleep(2000);
        const activeWindowHandle = libnut.getActiveWindow();
        const activeWindowName = libnut.getWindowTitle(activeWindowHandle);
        expect(activeWindowName).toBe(TITLE);
        expect(result).toBeTruthy();
        if (secondApp) {
            await secondApp.close();
        }
    });
});

afterEach(async () => {
    if (app) {
        await app.close();
    }
});
