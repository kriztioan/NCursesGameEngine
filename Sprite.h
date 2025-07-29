/**
 *  @file   Sprite.h
 *  @brief  Sprites for the NCursesGameEngine
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-30
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef CBNCURSESGAMEENGINE_SPRITE_H
#define CBNCURSESGAMEENGINE_SPRITE_H

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>

extern "C" {
#include <zlib.h>
}

namespace cb {
class Sprite;
typedef struct {
  wchar_t character;
  short color;
} Pixel;
}; // namespace cb

class cb::Sprite {

public:
  Sprite() {

    nSpriteWidth = 0;
    nSpriteHeight = 0;
    pPixels = nullptr;
  }

  [[maybe_unused]] bool Create(int nWidth, int nHeight) {

    nSpriteWidth = nWidth;
    nSpriteHeight = nHeight;

    if (nSpriteWidth * nSpriteHeight <= 0)
      return false;

    delete pPixels;
    pPixels = new cb::Pixel[nSpriteWidth * nSpriteHeight];
    std::fill_n(pPixels, nSpriteWidth * nSpriteHeight, (cb::Pixel){L' ', 0});

    return true;
  }

  [[maybe_unused]] bool Read(const std::filesystem::path &filename) {

    std::ifstream ifstr(filename, std::ios::in | std::ios::binary);

    if (!ifstr.fail())
      return ReadFromStream(ifstr);

    return false;
  }

  [[maybe_unused]] bool Load(const char *d, std::streamsize s) {

    std::istringstream istrstr(d, s);

    return ReadFromStream(istrstr);
  }

  [[maybe_unused]] bool z_Load(const char *z, std::streamsize s_z, uLongf s) {

    char *d = new char[s];

    uncompress(reinterpret_cast<Bytef *>(d), &s,
               reinterpret_cast<const Bytef *>(z), s_z);

    std::istringstream istrstr(d, s);

    bool ok = ReadFromStream(istrstr);

    delete[] d;

    return ok;
  }

  [[maybe_unused]] bool ReadolcSprite(const std::filesystem::path &filename) {

    std::ifstream ifstr;

    ifstr.open(filename, std::ios::in | std::ios::binary);

    if (ifstr.fail())
      return false;

    ifstr.read(reinterpret_cast<char *>(&nSpriteWidth), sizeof(int));
    ifstr.read(reinterpret_cast<char *>(&nSpriteHeight), sizeof(int));

    auto *colors = new short[nSpriteWidth * nSpriteHeight];
    auto *glyphs = new short[nSpriteWidth * nSpriteHeight];

    ifstr.read(reinterpret_cast<char *>(colors),
               nSpriteWidth * nSpriteHeight * sizeof(short));
    ifstr.read(reinterpret_cast<char *>(glyphs),
               nSpriteWidth * nSpriteHeight * sizeof(short));

    ifstr.close();

    delete[] pPixels;

    pPixels = new cb::Pixel[nSpriteWidth * nSpriteHeight];

    for (int i = 0; i < nSpriteWidth * nSpriteHeight; i++) {

      if (glyphs[i] == ' ') {

        pPixels[i].color = 0;
        pPixels[i].character = ' ';
      } else {

        pPixels[i].color = colors[i];
        pPixels[i].character = glyphs[i]; // == ' ' ? ' ' : L'\u2588';
      }
    }

    delete[] colors;
    delete[] glyphs;

    return true;
  }

  [[maybe_unused]] bool Write(const std::filesystem::path &filename) {

    if (nSpriteWidth * nSpriteHeight <= 0)
      return false;

    std::ofstream ofstr(filename, std::ios::binary);

    if (ofstr.fail())
      return false;

    ofstr.write(reinterpret_cast<char *>(&nSpriteWidth), sizeof(int));
    ofstr.write(reinterpret_cast<char *>(&nSpriteHeight), sizeof(int));
    ofstr.write(reinterpret_cast<char *>(pPixels),
                nSpriteWidth * nSpriteHeight * sizeof(cb::Pixel));

    return true;
  }

  ~Sprite() { delete pPixels; }

  [[maybe_unused]] cb::Pixel &operator[](unsigned i) { return pPixels[i]; }

  [[maybe_unused]] [[nodiscard]] inline int SpriteWidth() const {
    return nSpriteWidth;
  }

  [[maybe_unused]] [[nodiscard]] inline int SpriteHeight() const {
    return nSpriteHeight;
  }

private:
  int nSpriteWidth;
  int nSpriteHeight;
  cb::Pixel *pPixels;

  bool ReadFromStream(std::istream &in) {

    in.read(reinterpret_cast<char *>(&nSpriteWidth), sizeof(int));
    in.read(reinterpret_cast<char *>(&nSpriteHeight), sizeof(int));

    if (nSpriteWidth * nSpriteHeight <= 0)
      return false;

    delete pPixels;
    pPixels = new cb::Pixel[nSpriteWidth * nSpriteHeight];
    in.read(reinterpret_cast<char *>(pPixels),
            nSpriteWidth * nSpriteHeight * sizeof(cb::Pixel));

    return true;
  }
};

#endif // CBNCURSESGAMEENGINE_SPRITE_H
