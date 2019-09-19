var libnut = require("bindings")("libnut.node");

module.exports = libnut;

module.exports.screen = {};

function bitmap(width, height, byteWidth, bitsPerPixel, bytesPerPixel, image) {
  this.width = width;
  this.height = height;
  this.byteWidth = byteWidth;
  this.bitsPerPixel = bitsPerPixel;
  this.bytesPerPixel = bytesPerPixel;
  this.image = image;

  this.colorAt = function(x, y) {
    return libnut.getColor(this, x, y);
  };
}

module.exports.screen.capture = function(x, y, width, height) {
  //If coords have been passed, use them.
  if (
    typeof x !== "undefined" &&
    typeof y !== "undefined" &&
    typeof width !== "undefined" &&
    typeof height !== "undefined"
  ) {
    b = libnut.captureScreen(x, y, width, height);
  } else {
    b = libnut.captureScreen();
  }

  return new bitmap(
    b.width,
    b.height,
    b.byteWidth,
    b.bitsPerPixel,
    b.bytesPerPixel,
    b.image
  );
};
