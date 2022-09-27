#pragma once
#ifndef BITMAP_H
#define BITMAP_H

#include <memory>

class Bitmap {
  public:
    Bitmap(uint8_t* buffer, size_t width, size_t height,
           size_t byteWidth, size_t bitsPerPixel);

    ~Bitmap();

    size_t getBufferSize() noexcept;
    uint8_t* getImageBuffer() noexcept;
    
    size_t getWidth() noexcept;
    size_t getHeight() noexcept;
    size_t getByteWidth() noexcept;
    size_t getBitsPerPixel() noexcept;
    size_t getBytesPerPixel() noexcept;

  private:
    uint8_t* imageBuffer;
    size_t width;
    size_t height;
    size_t byteWidth;
    size_t bitsPerPixel;
};

#endif
