const libnut = require("..");

describe("Bitmap", () => {
  const params = {
    width: "number",
    height: "number",
    byteWidth: "number",
    bitsPerPixel: "number",
    bytesPerPixel: "number",
    image: "object"
  };

  it("Get a bitmap and check the parameters.", function() {
    const img = libnut.screen.capture();

    for (const x in params) {
      expect(typeof img[x]).toEqual(params[x]);
    }
  });

  it("Get a bitmap of a specific size.", function() {
    let size = 10;
    const img = libnut.screen.capture(0, 0, size, size);

    // Support for higher density screens.
    const multi = img.width / size;
    size = size * multi;
    expect(img.height).toEqual(size);
    expect(img.width).toEqual(size);
  });

  it("Get a bitmap and make sure the colorAt works as expected.", function() {
    const img = libnut.screen.capture();
    const hex = img.colorAt(0, 0);

    // t.ok(.it(hex), "colorAt returned valid hex.");
    expect(hex).toMatch(/^[0-9A-F]{6}$/i);

    const screenSize = libnut.getScreenSize();
    let width = screenSize.width;
    let height = screenSize.height;

    // Support for higher density screens.
    const multi = img.width / width;
    width = width * multi;
    height = height * multi;
    expect(() => img.colorAt(0, height)).toThrowError(/are outside the bitmap/);
    expect(() => img.colorAt(0, height - 1)).not.toThrow();
    expect(() => img.colorAt(width, 0)).toThrowError(/are outside the bitmap/);
    expect(() => img.colorAt(9999999999999, 0)).toThrowError(
      /are outside the bitmap/
    );
    expect(() => img.colorAt(0, 9999999999999)).toThrowError(
      /are outside the bitmap/
    );
  });
});
