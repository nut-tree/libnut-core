const {app, ipcMain, BrowserWindow} = require('electron')
const libnut = require("../..");
const path = require('path');

const POS_X = 50;
const POS_Y = 100;
const WIDTH = 800;
const HEIGTH = 600;

function test(description, assertion) {
    console.log(`${description}: ${assertion}`);
    if (!assertion) {
        process.exit(1);
    }
}

function createWindow() {
    const mainWindow = new BrowserWindow({
        width: WIDTH,
        height: HEIGTH,
        webPreferences: {
            nodeIntegration: true,
            preload: path.join(__dirname, 'preload.js')
        }
    });
    mainWindow.loadFile(path.join(__dirname, "index.html"));
    mainWindow.setPosition(POS_X, POS_Y);

    setTimeout(() => {
        const windowNames = libnut.getWindows().map(
            handle => libnut.getWindowTitle(handle)
        );
        test("list windows", windowNames.includes("libnut window test"));

        const activeWindow = libnut.getActiveWindow();
        const activeWindowRect = libnut.getWindowRect(activeWindow);
        const activeWindowTitle = libnut.getWindowTitle(activeWindow);
        test("title", activeWindowTitle === "libnut window test");
        test("posX", activeWindowRect.x === POS_X);
        test("posY", activeWindowRect.y === POS_Y);
        test("width", activeWindowRect.width === WIDTH);
        test("height", activeWindowRect.height === HEIGTH);
        process.exit(0);
    }, 5000);
}

ipcMain.on("main", (event, args) => {
    if (args === "quit") {
        app.quit();
    }
});

app.whenReady().then(() => {
    setTimeout(() => process.exit(1), 15000);
    createWindow()

    app.on('activate', function () {
        if (BrowserWindow.getAllWindows().length === 0) createWindow()
    })
})

app.on('window-all-closed', function () {
    console.log("Bye!");
    app.quit();
})
