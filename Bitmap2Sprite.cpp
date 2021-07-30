/**
 *  @file   Bitmap2Sprite.cpp
 *  @brief  Convert Bitmap to Sprite
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-29
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

typedef struct {
  wchar_t character;
  short color;
} Pixel;

typedef struct {
  float r;
  float g;
  float b;
} RGB;

RGB indexed[41] = {{0.0f, 0.0f, 0.0f},       // black
                   {255.0f, 0.0f, 0.0f},     // red
                   {0.0f, 255.0f, 0.0f},     // green
                   {255.0f, 255.0f, 0.0f},   // yellow
                   {0.0f, 0.0f, 255.0f},     // blue
                   {255.0f, 0.0f, 255.0f},   // magenta
                   {192.0f, 192.0f, 192.0f}, // grey
                   {255.0f, 255.0f, 255.0f}, // white
                   {255.0f, 255.0f, 0.0f},   // cyan
                   {128.0f, 0.0f, 0.0f},     // maroon
                   {192.0f, 255.0f, 0.0f},   // lime
                   {139.0f, 69.0f, 19.0f},   // brown
                   {0.0f, 0.0f, 128.0f},     // navy
                   {241.0f, 156.0f, 187.0f}, // fuchsia
                   {0.0f, 128.0f, 128.0f},   // teal
                   {255.0f, 255.0f, 255.0f}, {8.0f, 8.0f, 8.0f},
                   {18.0f, 18.0f, 18.0f},    {28.0f, 28.0f, 28.0f},
                   {38.0f, 38.0f, 38.0f},    {48.0f, 48.0f, 48.0f},
                   {58.0f, 58.0f, 58.0f},    {68.0f, 68.0f, 68.0f},
                   {78.0f, 78.0f, 78.0f},    {88.0f, 88.0f, 88.0f},
                   {98.0f, 98.0f, 98.0f},    {108.0f, 108.0f, 108.0f},
                   {118.0f, 118.0f, 118.0f}, {128.0f, 128.0f, 128.0f},
                   {138.0f, 138.0f, 138.0f}, {148.0f, 148.0f, 148.0f},
                   {158.0f, 158.0f, 158.0f}, {168.0f, 168.0f, 168.0f},
                   {178.0f, 178.0f, 178.0f}, {188.0f, 188.0f, 188.0f},
                   {198.0f, 198.0f, 198.0f}, {208.0f, 208.0f, 208.0f},
                   {218.0f, 218.0f, 218.0f}, {228.0f, 228.0f, 228.0f},
                   {238.0f, 238.0f, 238.0f}, {248.0f, 248.0f, 248.0f}};

int main(const int argc, const char *argv[]) {

  if (argc < 3) {

    std::cerr << "usage: " << argv[0] << " bitmap sprite [colors]\n";
    return 1;
  }

  int ncolors = argc > 3 ? atoi(argv[3]) : 40;

  for (int i = 0; i < ncolors; i++) {

    indexed[i].r /= 255.0f;
    indexed[i].g /= 255.0f;
    indexed[i].b /= 255.0f;
  }

  std::ifstream ifstr(argv[1], std::ios::in | std::ios::binary);

  if (ifstr.fail())
    return 2;

  unsigned char header[54];

  ifstr.read(reinterpret_cast<char *>(&header), sizeof(header));

  int width;

  std::memcpy(&width, header + 18, sizeof(int));

  int height;

  std::memcpy(&height, header + 22, sizeof(int));

  if (width * height <= 0)
    return 3;

  Pixel *pixels = new Pixel[width * height];

  int nPadded = (3 * width + 3) & (~3);

  unsigned char *row = new unsigned char[nPadded];

  RGB rgb;

  for (int y = 0; y < height; y++) {
    ifstr.read(reinterpret_cast<char *>(row), nPadded);
    for (int x = 0; x < width; x++) {
      rgb.b = static_cast<float>(row[3 * x + 0]) / 255.0f;
      rgb.g = static_cast<float>(row[3 * x + 1]) / 255.0f;
      rgb.r = static_cast<float>(row[3 * x + 2]) / 255.0f;

      wchar_t c = '.';
      if (rgb.r == 1.0f && rgb.g == 1.0f && rgb.b == 1.0f)
        c = L' ';

      pixels[x + (height - 1 - y) * width].character = c;

      short closest = 0;

      float distance =
          2.0f * (indexed[closest].r - rgb.r) * (indexed[closest].r - rgb.r) +
          4.0f * (indexed[closest].g - rgb.g) * (indexed[closest].g - rgb.g) +
          3.0f * (indexed[closest].b - rgb.b) * (indexed[closest].b - rgb.b);

      for (short i = 0; i < ncolors; i++) {
        float d = 2.0f * (indexed[i].r - rgb.r) * (indexed[i].r - rgb.r) +
                  4.0f * (indexed[i].g - rgb.g) * (indexed[i].g - rgb.g) +
                  3.0f * (indexed[i].b - rgb.b) * (indexed[i].b - rgb.b);

        if (d < distance) {
          closest = i;
          distance = d;
        }
      }
      pixels[x + (height - 1 - y) * width].color = closest + 1;
    }
  }

  delete[] row;

  std::ofstream ofstr(argv[2], std::ios::binary);

  if (ofstr.fail())
    return 4;

  ofstr.write(reinterpret_cast<char *>(&width), sizeof(int));

  ofstr.write(reinterpret_cast<char *>(&height), sizeof(int));

  ofstr.write(reinterpret_cast<char *>(pixels), width * height * sizeof(Pixel));

  delete[] pixels;

  return 0;
}
