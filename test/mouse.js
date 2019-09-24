const libnut = require("..");
let lastKnownPos, currentPos;

//Increase delay to help it reliability.
libnut.setMouseDelay(100);

describe("Mouse", () => {
  describe("movement", () => {
    it("get the initial mouse position.", function() {
      expect((lastKnownPos = libnut.getMousePos())).toBeTruthy();
      expect(lastKnownPos.x !== undefined).toBeTruthy();
      expect(lastKnownPos.y !== undefined).toBeTruthy();
    });

    xit("move the mouse to valid coordinates", () => {
      lastKnownPos = libnut.moveMouse(0, 0);
      expect(libnut.moveMouse(100, 100) === 1).toBeTruthy();
      currentPos = libnut.getMousePos();
      expect(currentPos).toEqual({ x: 100, y: 100 });
    });

    it("should throw on more than three parameters", () => {
      expect(() => libnut.moveMouse(0, 1, 2, 3)).toThrowError(/Invalid number/);
    });

    it("should throw on less than two parameters", () => {
      expect(() => libnut.moveMouse(0)).toThrowError(/Invalid number/);

      expect(libnut.moveMouse(0, 0) === 1).toBeTruthy();
    });
  });

  describe("smooth movement", () => {
    xit("move the mouse smoothly.", () => {
      lastKnownPos = libnut.moveMouseSmooth(0, 0);
      expect(libnut.moveMouseSmooth(100, 100) === 1).toBeTruthy();
      currentPos = libnut.getMousePos();
      expect(currentPos).toEqual({ x: 100, y: 100 });

      expect(() => {
        libnut.moveMouseSmooth(0, 1, 2, 3);
      }).toThrowError(/Invalid number/);

      expect(() => {
        libnut.moveMouseSmooth(0);
      }).toThrowError(/Invalid number/);

      expect(libnut.moveMouseSmooth(0, 0) === 1).toBeTruthy();
    });
  });

  describe("click", () => {
    it("left", () => {
      expect(libnut.mouseClick()).toBeTruthy();
      expect(libnut.mouseClick("left") === 1).toBeTruthy();
    });
    it("middle", () => {
      expect(libnut.mouseClick("middle") === 1).toBeTruthy();
    });
    it("right", () => {
      expect(libnut.mouseClick("right") === 1).toBeTruthy();
    });
    it("doubleclick", () => {
      expect(libnut.mouseClick("left", true)).toBeTruthy();
    });

    it("should throw on invalid input", () => {
      expect(() => {
        libnut.mouseClick("0");
      }).toThrowError(/Invalid mouse/);
      expect(() => {
        libnut.mouseClick("party");
      }).toThrowError(/Invalid mouse/);
    });

    it("should throw too much parameters", () => {
      expect(() => {
        libnut.mouseClick("left", 0, "it");
      }).toThrowError(/Invalid number/);
    });
  });

  describe("drag", () => {
    it("should not throw work", () => {
      expect(libnut.dragMouse(5, 5) === 1).toBeTruthy();
    });

    it("should throw on too few arguments", () => {
      expect(function() {
        libnut.dragMouse(0);
      }).toThrowError(/Invalid number/);
    });

    it("should throw on too many arguments", () => {
      expect(function() {
        libnut.dragMouse(1, 1, "left", 5);
      }).toThrowError(/Invalid number/);
    });

    it("should throw on invalid button arguments", () => {
      expect(function() {
        libnut.dragMouse(2, 2, "party");
      }).toThrowError(/Invalid mouse/);
    });
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
