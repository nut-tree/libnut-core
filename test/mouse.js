const libnut = require("..");
let lastKnownPos, currentPos;

//Increase delay to help it reliability.
libnut.setMouseDelay(100);

describe("Mouse", () => {
  it("Get the initial mouse position.", function() {
    expect((lastKnownPos = libnut.getMousePos())).toBeTruthy();
    expect(lastKnownPos.x !== undefined).toBeTruthy();
    expect(lastKnownPos.y !== undefined).toBeTruthy();
  });

  it("Move the mouse.", function() {
    lastKnownPos = libnut.moveMouse(0, 0);
    expect(libnut.moveMouse(100, 100) === 1).toBeTruthy();
    currentPos = libnut.getMousePos();
    expect(currentPos.x === 100).toBeTruthy();
    expect(currentPos.y === 100).toBeTruthy();

    expect(function() {
      libnut.moveMouse(0, 1, 2, 3);
    }).toThrowError(/Invalid number/);

    expect(function() {
      libnut.moveMouse(0);
    }).toThrowError(/Invalid number/);

    expect(libnut.moveMouse(0, 0) === 1).toBeTruthy();
  });

  it("Move the mouse smoothly.", function() {
    lastKnownPos = libnut.moveMouseSmooth(0, 0);
    expect(libnut.moveMouseSmooth(100, 100) === 1).toBeTruthy();
    currentPos = libnut.getMousePos();
    expect(currentPos.x).toEqual(100);
    expect(currentPos.y).toEqual(100);

    expect(function() {
      libnut.moveMouseSmooth(0, 1, 2, 3);
    }).toThrowError(/Invalid number/);

    expect(function() {
      libnut.moveMouseSmooth(0);
    }).toThrowError(/Invalid number/);

    expect(libnut.moveMouseSmooth(0, 0) === 1).toBeTruthy();
  });

  it("Click the mouse.", function() {
    expect(libnut.mouseClick()).toBeTruthy();
    expect(libnut.mouseClick("left") === 1).toBeTruthy();
    expect(libnut.mouseClick("middle") === 1).toBeTruthy();
    expect(libnut.mouseClick("right") === 1).toBeTruthy();

    expect(libnut.mouseClick("left", true)).toBeTruthy();

    expect(function() {
      libnut.mouseClick("party");
    }).toThrowError(/Invalid mouse/);

    expect(function() {
      libnut.mouseClick("0");
    }).toThrowError(/Invalid mouse/);

    expect(function() {
      libnut.mouseClick("left", 0, "it");
    }).toThrowError(/Invalid number/);
  });

  it("Drag the mouse.", function() {
    expect(libnut.dragMouse(5, 5) === 1).toBeTruthy();

    expect(function() {
      libnut.dragMouse(0);
    }).toThrowError(/Invalid number/);

    expect(function() {
      libnut.dragMouse(1, 1, "left", 5);
    }).toThrowError(/Invalid number/);

    expect(function() {
      libnut.dragMouse(2, 2, "party");
    }).toThrowError(/Invalid mouse/);
  });

  it("Mouse scroll.", function() {
    expect((lastKnownPos = libnut.getMousePos())).toBeTruthy();
    expect(libnut.mouseClick() === 1).toBeTruthy();
    expect(libnut.scrollMouse(0, 1 * 120) === 1).toBeTruthy();
    expect(libnut.scrollMouse(0, 20 * 120) === 1).toBeTruthy();
    expect(libnut.scrollMouse(0, -5 * 120) === 1).toBeTruthy();
    expect(libnut.scrollMouse(1 * 120, 0) === 1).toBeTruthy();
    expect(libnut.scrollMouse(20 * 120, 0) === 1).toBeTruthy();
    expect(libnut.scrollMouse(-5 * 120, 0) === 1).toBeTruthy();
    expect(libnut.scrollMouse(-5 * 120, -5 * 120) === 1).toBeTruthy();
  });

  it("Mouse Toggle", function() {
    expect((lastKnownPos = libnut.getMousePos())).toBeTruthy();
    expect(libnut.mouseToggle("up", "right") === 1).toBeTruthy();
  });
});
