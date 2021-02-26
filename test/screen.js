const libnut = require("..");

describe("Screen", () => {
  it("Get screen size.", function() {
    let screenSize;
    expect((screenSize = libnut.getScreenSize())).toBeTruthy();
    expect(screenSize.width !== undefined).toBeTruthy();
    expect(screenSize.height !== undefined).toBeTruthy();
  });
});

describe("Capture", () => {
  it("fullScreen capture", () => {
    // GIVEN
    const screenSize = libnut.getScreenSize()

    // WHEN
    const capture = () => libnut.screen.capture(0, 0, screenSize.width, screenSize.height);

    // THEN
    expect(capture).not.toThrowError("Error: Given width exceeds display dimensions");
  });
});