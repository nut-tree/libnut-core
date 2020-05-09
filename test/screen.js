const libnut = require("..");

describe("Screen", () => {
  it("Get screen size.", function() {
    let screenSize;
    expect((screenSize = libnut.getScreenSize())).toBeTruthy();
    expect(screenSize.width !== undefined).toBeTruthy();
    expect(screenSize.height !== undefined).toBeTruthy();
  });
});
