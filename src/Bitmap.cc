#include "Bitmap.h"

Bitmap::Bitmap(uint8_t *buffer, size_t width, size_t height, size_t byteWidth,
               size_t bitsPerPixel)
    : imageBuffer(buffer), width(width), height(height), byteWidth(byteWidth),
      bitsPerPixel(bitsPerPixel) {}

Bitmap::~Bitmap() {
  delete[] this->imageBuffer;
}

size_t Bitmap::getBufferSize() noexcept {
  return this->byteWidth * this->height;
}

uint8_t *Bitmap::getImageBuffer() noexcept { return this->imageBuffer; }

size_t Bitmap::getWidth() noexcept { return this->width; }

size_t Bitmap::getHeight() noexcept { return this->height; }

size_t Bitmap::getByteWidth() noexcept { return this->byteWidth; }

size_t Bitmap::getBitsPerPixel() noexcept { return this->bitsPerPixel; }

size_t Bitmap::getBytesPerPixel() noexcept { return this->bitsPerPixel / 8; }
