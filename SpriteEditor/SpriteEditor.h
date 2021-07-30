/**
 *  @file   SpriteEditor.h
 *  @brief  Sprite Editor in NCursesGameEngine
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-29
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef CBNCURSESGAMEENGINE_SPRITEVIEW_H
#define CBNCURSESGAMEENGINE_SPRITEVIEW_H

#include "../NCursesGameEngine.h"

#include <codecvt>
#include <locale>

class SpriteEditor : public cb::NCursesGameEngine {
public:
  SpriteEditor(const int argc, const char *argv[]) {

    bShowFPS = false;

    if (argc > 1) {
      std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> conv;
      Sprite.ReadolcSprite(conv.from_bytes(argv[1]));
      nSpriteSize = std::max(Sprite.SpriteHeight(), Sprite.SpriteWidth());
    } else {
      nSpriteSize = 40;
      Sprite.Create(nSpriteSize, nSpriteSize);
    }
  }

private:
  bool OnUserCreate() final {

    pixel = {PIXEL_FULL, FG_BLACK};

    nMouseX = 0;
    nMouseY = 0;

    Clear(PIXEL_MEDIUM, FG_GREY);

    return true;
  }

  bool OnUserUpdate(float fElapsed) final {

    if (IsFocused()) {

      if (KeyDown(kVK_ANSI_Q))
        return false;
      if (KeyDown(kVK_ANSI_W))
        Sprite.Write(L"out.sprite");
      if (KeyDown(kVK_ANSI_N))
        Sprite.Create(nSpriteSize, nSpriteSize);
    }

    if (MousePressed(BUTTON1_CLICKED, nMouseX, nMouseY)) {

     if (nMouseY == 0 && nMouseX > 0 && nMouseX < FG_COLORS) {
        pixel.color = nMouseX;
        pixel.character = PIXEL_FULL;
      } else if (nMouseY > 0 && nMouseY < (nSpriteSize + 1) && nMouseX > 0 &&
                 nMouseX < (nSpriteSize + 1))
        Sprite[(nMouseX - 1) + (nMouseY - 1) * nSpriteSize] = pixel;
    }

    Clear(PIXEL_MEDIUM, FG_GREY);

    for (int x = 1; x < FG_COLORS; x++)
      DrawPixel(x, 0, PIXEL_FULL, x);

    DrawSprite(Sprite, 1, 1);

    DrawPixel(1 + nSpriteSize, nSpriteSize, pixel.character, pixel.color);

    DrawString(1, 1 + nSpriteSize,
               L"(" + std::to_wstring(nMouseX - 1) + L", " +
                   std::to_wstring(nMouseY - 1) + L")");

    usleep(23333);

    return true;
  }

  cb::Sprite Sprite;
  int nSpriteSize;
  cb::Pixel pixel;
  int nMouseX;
  int nMouseY;
};

#endif // CBNCURSESGAMEENGINE_SPRITEVIEW_H
