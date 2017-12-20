#include "image.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/**
 * Image
 **/
Image::Image(int width_, int height_) {
  assert(width_ > 0);
  assert(height_ > 0);

  width = width_;
  height = height_;
  num_pixels = width * height;
  sampling_method = IMAGE_SAMPLING_POINT;

  data.raw = new uint8_t[num_pixels * 4];
  int b = 0;  // which byte to write to
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      data.raw[b++] = 0;
      data.raw[b++] = 0;
      data.raw[b++] = 0;
      data.raw[b++] = 255;
    }
  }

  assert(data.raw != NULL);
}

Image::Image(const Image& src) {
  width = src.width;
  height = src.height;
  num_pixels = width * height;
  sampling_method = IMAGE_SAMPLING_POINT;

  data.raw = new uint8_t[num_pixels * 4];

  // memcpy(data.raw, src.data.raw, num_pixels);
  *data.raw = *src.data.raw;
}

Image::Image(char* fname) {
  int numComponents;  //(e.g., Y, YA, RGB, or RGBA)
  data.raw = stbi_load(fname, &width, &height, &numComponents, 4);

  if (data.raw == NULL) {
    printf("Error loading image: %s", fname);
    exit(-1);
  }

  num_pixels = width * height;
  sampling_method = IMAGE_SAMPLING_POINT;
}

Image::~Image() {
  delete data.raw;
  data.raw = NULL;
}

void Image::Write(char* fname) {
  int lastc = strlen(fname);

  switch (fname[lastc - 1]) {
    case 'g':  // jpeg (or jpg) or png
      if (fname[lastc - 2] == 'p' || fname[lastc - 2] == 'e')  // jpeg or jpg
        stbi_write_jpg(fname, width, height, 4, data.raw,
                       95);  // 95% jpeg quality
      else                   // png
        stbi_write_png(fname, width, height, 4, data.raw, width * 4);
      break;
    case 'a':  // tga (targa)
      stbi_write_tga(fname, width, height, 4, data.raw);
      break;
    case 'p':  // bmp
    default:
      stbi_write_bmp(fname, width, height, 4, data.raw);
  }
}
