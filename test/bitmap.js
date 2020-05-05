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
});
