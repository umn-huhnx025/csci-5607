#include "image.h"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <random>
#include <string>

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
  int b = 0; // which byte to write to
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      data.raw[b++] = 0;
      data.raw[b++] = 0;
      data.raw[b++] = 0;
      data.raw[b++] = 0;
    }
  }

  assert(data.raw != NULL);
}

Image::Image(const Image &src) {
  width = src.width;
  height = src.height;
  num_pixels = width * height;
  sampling_method = IMAGE_SAMPLING_POINT;

  data.raw = new uint8_t[num_pixels * 4];

  memcpy(data.raw, src.data.raw, num_pixels*4);
  // *data.raw = *src.data.raw;
}

Image::Image(char *fname) {
  int numComponents; //(e.g., Y, YA, RGB, or RGBA)
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

void Image::Write(char *fname) {
  int lastc = std::strlen(fname);

  switch (fname[lastc - 1]) {
  case 'g': // jpeg (or jpg) or png
    if (fname[lastc - 2] == 'p' || fname[lastc - 2] == 'e') // jpeg or jpg
      stbi_write_jpg(fname, width, height, 4, data.raw,
                     95); // 95% jpeg quality
    else                  // png
      stbi_write_png(fname, width, height, 4, data.raw, width * 4);
    break;
  case 'a': // tga (targa)
    stbi_write_tga(fname, width, height, 4, data.raw);
    break;
  case 'p': // bmp
  default:
    stbi_write_bmp(fname, width, height, 4, data.raw);
  }
}

void Image::AddNoise(double factor) {
  int x, y;
  for (x = 0; x < Width(); x++) {
    for (y = 0; y < Height(); y++) {
      Pixel p = GetPixel(x, y);
      float p_r = p.r;
      float p_g = p.g;
      float p_b = p.b;
      Pixel noise = PixelRandom();
      float n_r = noise.r;
      float n_g = noise.g;
      float n_b = noise.b;
      float noisy_r = fmod(p_r * (1 - factor) + n_r * factor, 255);
      float noisy_g = fmod(p_g * (1 - factor) + n_g * factor, 255);
      float noisy_b = fmod(p_b * (1 - factor) + n_b * factor, 255);
      GetPixel(x, y).Set(noisy_r, noisy_g, noisy_b);
    }
  }
}

void Image::Brighten(double factor) {
  int x, y;
  for (x = 0; x < Width(); x++) {
    for (y = 0; y < Height(); y++) {
      Pixel p = GetPixel(x, y);
      Pixel scaled_p = p * factor;
      GetPixel(x, y) = scaled_p;
    }
  }
}

void Image::ChangeContrast(double factor) {
  int x, y;

  double total_lum = 0;
  for (x = 0; x < Width(); x++) {
    for (y = 0; y < Height(); y++) {
      total_lum += GetPixel(x, y).Luminance();
    }
  }
  double avg_lum = total_lum / NumPixels();
  Pixel gray = Pixel(avg_lum, avg_lum, avg_lum);

  for (x = 0; x < Width(); x++) {
    for (y = 0; y < Height(); y++) {
      Pixel p = GetPixel(x, y);
      GetPixel(x, y) = PixelLerp(gray, p, factor);
    }
  }
}

void Image::ChangeSaturation(double factor) {
  int x, y;
  for (x = 0; x < Width(); x++) {
    for (y = 0; y < Height(); y++) {
      Component lum = GetPixel(x, y).Luminance();
      Pixel gray = Pixel(lum, lum, lum);
      Pixel p = GetPixel(x, y);
      GetPixel(x, y) = PixelLerp(gray, p, factor);
    }
  }
}

Image *Image::Crop(int x, int y, int w, int h) {
  Image *cropped = new Image(w, h);
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      cropped->GetPixel(i, j) = GetPixel(x + i, y + j);
    }
  }
  return cropped;
}

void Image::ExtractChannel(int channel) {
  int x, y;
  for (x = 0; x < Width(); x++) {
    for (y = 0; y < Height(); y++) {
      Pixel p = GetPixel(x, y);
      switch (channel) {
      case IMAGE_CHANNEL_RED:
        GetPixel(x, y).Set(p.r, 0, 0);
        break;
      case IMAGE_CHANNEL_GREEN:
        GetPixel(x, y).Set(0, p.g, 0);
        break;
      case IMAGE_CHANNEL_BLUE:
        GetPixel(x, y).Set(0, 0, p.b);
        break;
      }
    }
  }
}

void Image::Quantize(int nbits) {
  int x, y;
  for (x = 0; x < Width(); x++) {
    for (y = 0; y < Height(); y++) {
      Pixel p = GetPixel(x, y);
      GetPixel(x, y) = PixelQuant(p, nbits);
    }
  }
}

void Image::RandomDither(int nbits) {
  AddNoise(0.4);

  int x, y;
  for (x = 0; x < Width(); x++) {
    for (y = 0; y < Height(); y++) {
      Pixel p = GetPixel(x, y);
      GetPixel(x, y) = PixelQuant(p, nbits);
    }
  }
}

static int Bayer4[4][4] = {
    {15, 7, 13, 5}, {3, 11, 1, 9}, {12, 4, 14, 6}, {0, 8, 2, 10}};

void Image::OrderedDither(int nbits) {
  int bayer_n = 4;
  for (int x = 0; x < Width(); x++) {
    for (int y = 0; y < Height(); y++) {
      float new_r, new_g, new_b;

      int i = x % bayer_n;
      int j = y % bayer_n;
      Pixel p = GetPixel(x, y);
      Pixel q = PixelQuant(p, nbits);
      Pixel error = p + q * -1;

      int b = Bayer4[i][j] * 15;
      new_r = error.r > b ? q.r : 0;
      new_g = error.g > b ? q.g : 0;
      new_b = error.b > b ? q.b : 0;
      GetPixel(x, y).SetClamp(new_r, new_g, new_b);
    }
  }
}

/* Error-diffusion parameters */
const double ALPHA = 7.0 / 16.0, BETA = 3.0 / 16.0, GAMMA = 5.0 / 16.0,
             DELTA = 1.0 / 16.0;

void Image::FloydSteinbergDither(int nbits) {
  int bayer_n = 4;
  for (int y = 0; y < Height(); y++) {
    for (int x= 0; x < Width(); x++) {
      float new_r, new_g, new_b;

      Pixel p = GetPixel(x, y);
      // printf("p: %d %d %d\n", p.r, p.g, p.b);
      Pixel q = PixelQuant(p, nbits);
      // printf("q: %d %d %d\n", q.r, q.g, q.b);
      float error_r = p.r + q.r * -1;
      float error_g = p.g + q.g * -1;
      float error_b = p.b + q.b * -1;
      // printf("error: %d %d %d\n", error.r, error.g, error.b);

      GetPixel(x, y) = q;
      if (ValidCoord(x+1, y)) {
        Pixel pa = GetPixel(x+1, y);
        pa.r += error_r * ALPHA;
        pa.g += error_g * ALPHA;
        pa.b += error_b * ALPHA;
        GetPixel(x+1, y).SetClamp(pa.r, pa.g, pa.b);
      }
      if (ValidCoord(x+1, y+1)) {
        Pixel pd = GetPixel(x+1, y+1);
        pd.r += error_r * DELTA;
        pd.g += error_g * DELTA;
        pd.b += error_b * DELTA;
        GetPixel(x+1, y+1).SetClamp(pd.r, pd.g, pd.b);
      }
      if (ValidCoord(x-1, y+1)) {
        Pixel pb = GetPixel(x-1, y+1);
        pb.r += error_r * BETA;
        pb.g += error_g * BETA;
        pb.b += error_b * BETA;
        GetPixel(x-1, y+1).SetClamp(pb.r, pb.g, pb.b);
      }
      if (ValidCoord(x, y+1)) {
        Pixel pc = GetPixel(x, y+1);
        pc.r += error_r * GAMMA;
        pc.g += error_g * GAMMA;
        pc.b += error_b * GAMMA;
        GetPixel(x, y+1).SetClamp(pc.r, pc.g, pc.b);
      }
    }
  }
}

void Image::Blur(int n) {
  Image *new_image = new Image(Width(), Height());

  // Make filter
  double f[n][n];
  int fx, fy;
  double fsum = 0;
  double sigma = n;
  for (fx = 0; fx < n; fx++) {
    for (fy = 0; fy < n; fy++) {
      float x = fx + 0.5 - n / 2.;
      float y = fy + 0.5 - n / 2.;
      f[fx][fy] = exp(
          -(pow(x, 2) / (2 * pow(sigma, 2)) + pow(y, 2) / (2 * pow(sigma, 2))));
      fsum += f[fx][fy];
      // printf("%.2f ", f[fx][fy]);
    }
    // printf("\n");
  }

  // Apply filter
  int x, y;
  for (x = 0; x < new_image->Width(); x++) {
    for (y = 0; y < new_image->Height(); y++) {
      float new_r = 0, new_g = 0, new_b = 0;
      for (int fx = 0; fx < n; fx++) {
        for (int fy = 0; fy < n; fy++) {
          int ix = x + fx - n / 2;
          int iy = y + fy - n / 2;

          ix = std::clamp(ix, 0, Width() - 1);
          iy = std::clamp(iy, 0, Height() - 1);
          Pixel p = GetPixel(ix, iy);
          new_r += p.r * f[fx][fy] / fsum;
          new_g += p.g * f[fx][fy] / fsum;
          new_b += p.b * f[fx][fy] / fsum;
        }
      }
      new_image->GetPixel(x, y).SetClamp(new_r, new_g, new_b);
    }
  }
  *this = *new_image;
}

void Image::Sharpen(int n) {
  Image* new_image = new Image(Width(), Height());
  Image blurred_image = Image(*this);
  blurred_image.Blur(n);

  for (int x = 0; x < Width(); x++) {
    for (int y = 0; y < Height(); y++) {
      Pixel p = GetPixel(x, y);
      Pixel bp = blurred_image.GetPixel(x, y);
      new_image->GetPixel(x, y) = PixelLerp(GetPixel(x, y), blurred_image.GetPixel(x, y), -1);
      Pixel np = new_image->GetPixel(x, y);
    }
  }

  *this = *new_image;
}

void Image::EdgeDetect() {
  Image *new_image = new Image(Width(), Height());
  double f[3][3] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}};
  int fwidth = 3;

  for (int x = 0; x < Width(); x++) {
    for (int y = 0; y < Height(); y++) {
      float new_r = 0, new_g = 0, new_b = 0;
      for (int fx = 0; fx < fwidth; fx++) {
        for (int fy = 0; fy < fwidth; fy++) {
          int ix = x + fx - fwidth / 2;
          int iy = y + fy - fwidth / 2;

          ix = std::clamp(ix, 0, Width() - 1);
          iy = std::clamp(iy, 0, Height() - 1);
          Pixel p = GetPixel(ix, iy);
          new_r += p.r * f[fx][fy];
          new_g += p.g * f[fx][fy];
          new_b += p.b * f[fx][fy];
        }
      }
      new_image->GetPixel(x, y).SetClamp(new_r, new_g, new_b);
    }
  }
  *this = *new_image;
}

Image *Image::Scale(double sx, double sy) {
  int new_width = (int)(sx * Width());
  int new_height = (int)(sy * Height());
  Image *new_image = new Image(new_width, new_height);
  for (int x = 0; x < new_width; x++) {
    for (int y = 0; y < new_height; y++) {
      new_image->GetPixel(x, y) = Sample(x / sx, y / sy);
    }
  }
  return new_image;
}

Image *Image::Rotate(double angle) {
  angle *= M_PI / 180.;

  int new_width = Height() * std::abs(std::sin(angle)) +
                  Width() * std::abs(std::sin(M_PI / 2 - angle));
  int new_height = Height() * std::abs(std::cos(angle)) +
                   Width() * std::abs(std::cos(M_PI / 2 - angle));

  int x_diff = (new_width - Width()) / 2;
  int y_diff = (new_height - Height()) / 2;

  Image *new_image = new Image(new_width, new_height);

  int half_w = Width() / 2;
  int half_h = Height() / 2;

  for (int x = 0; x < new_width; x++) {
    for (int y = 0; y < new_height; y++) {
      int new_x = x - x_diff;
      int new_y = y - y_diff;

      int samp_x = (new_x - half_w) * std::cos(angle) -
                   (new_y - half_h) * std::sin(angle) + half_w;
      int samp_y = (new_x - half_w) * std::sin(angle) +
                   (new_y - half_h) * std::cos(angle) + half_h;

      if (!ValidCoord(samp_x, samp_y)) {
        new_image->GetPixel(x, y).Set(0, 0, 0, 0);
      } else {
        new_image->GetPixel(x, y) = Sample(samp_x, samp_y);
      }
    }
  }
  return new_image;
}

void Image::Fun() {
  Image old_image = Image(*this);
  old_image.sampling_method = sampling_method;

  int new_width = Width();
  int new_height = Height();

  int half_w = Width() / 2;
  int half_h = Height() / 2;

  for (int x = 0; x < new_width; x++) {
    for (int y = 0; y < new_height; y++) {
      double dx = x - half_w;
      double dy = y - half_h;
      double angle = M_PI / 1.5 * std::sqrt(dx*dx+dy*dy) / std::sqrt(half_w*half_w + half_h*half_h);
      int samp_x = (x - half_w) * std::cos(angle) -
                    (y - half_h) * std::sin(angle) + half_w;
      int samp_y = (x - half_w) * std::sin(angle) +
                    (y - half_h) * std::cos(angle) + half_h;

      if (!old_image.ValidCoord(samp_x, samp_y)) {
        GetPixel(x, y).Set(0, 0, 0, 0);
      } else {
        GetPixel(x, y) = old_image.Sample(samp_x, samp_y);
      }
    }
  }
}

/**
 * Image Sample
 **/
void Image::SetSamplingMethod(int method) {
  assert((method >= 0) && (method < IMAGE_N_SAMPLING_METHODS));
  sampling_method = method;
}

Pixel Image::Sample(double u, double v) {
  Pixel new_pixel;
  switch (sampling_method) {
  case IMAGE_SAMPLING_POINT: {
    int new_x = (int)(u+0.5);
    int new_y = (int)(v+0.5);
    if (ValidCoord(new_x, new_y)) {
      new_pixel = GetPixel(new_x, new_y);
    } else {
      new_pixel = Pixel();
    }
    break;
  }
  case IMAGE_SAMPLING_BILINEAR: {
    double d;
    if (!modf(u, &d) && !modf(v, &d)) {
      new_pixel = GetPixel((int)u, (int)v);
      break;
    }
    if (!ValidCoord(ceil(u), ceil(v))) {
      new_pixel = Pixel();
      break;
    }
    Pixel p00 = GetPixel(floor(u), floor(v));
    Pixel p01 = GetPixel(ceil(u), floor(v));
    Pixel p10 = GetPixel(floor(u), ceil(v));
    Pixel p11 = GetPixel(ceil(u), ceil(v));
    if (u == floor(u)) {
      new_pixel = p00*(1-(v-floor(v))) + p10*(1-(ceil(v)-v));
      break;
    } else if (v == floor(v)) {
      new_pixel = p00*(1-(u-floor(u))) + p01*(1-(ceil(u)-u));
      break;
    }
    Pixel px1 = p00*(1-(u-floor(u))) + p01*(1-(ceil(u)-u));
    Pixel px2 = p10*(1-(u-floor(u))) + p11*(1-(ceil(u)-u));
    Pixel pxy = px1*(1-(v-floor(v))) + px2*(1-(ceil(v)-v));
    new_pixel = pxy;
    break;
  }
  case IMAGE_SAMPLING_GAUSSIAN: {
    int r = 2;
    float sigma = r;
    float new_r = 0, new_g = 0, new_b = 0;

    float p_dist_sum = 0;
    for (int i = -r; i < r; i++) {
      for (int j = -r; j < r; j++) {
        int x = (int)(u + i + 0.5);
        int y = (int)(v + j + 0.5);
        if (ValidCoord(x, y) && i*i+j*j <= r*r) {
          Pixel p = GetPixel(x, y);
          float p_dist = exp(-(pow(u-x, 2) / (2 * pow(sigma, 2)) + pow(v-y, 2) / (2 * pow(sigma, 2))));
          p_dist_sum += p_dist;
          new_r += p.r*p_dist;
          new_g += p.g*p_dist;
          new_b += p.b*p_dist;
        }
      }
    }
    new_r /= p_dist_sum;
    new_g /= p_dist_sum;
    new_b /= p_dist_sum;
    new_pixel = Pixel(new_r, new_g, new_b);
    break;
  }
  }
  return new_pixel;
}
