/**
 *  @file   NCursesGameEngine.h
 *  @brief  Game Engine in NCurses
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-30
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef CBNCURSESGAMEENGINE_NCURSESGAMEENGINE_H
#define CBNCURSESGAMEENGINE_NCURSESGAMEENGINE_H

extern "C" {
#include <Carbon/Carbon.h>
#include <X11/Xlib.h>
#include <ncurses.h>
}
#include "Sprite.h"
#include <algorithm>
#include <chrono>
#include <clocale>
#include <cstdlib>
#include <ctime>
#include <string>
#include <thread>
#include <vector>

namespace cb {
class NCursesGameEngine;
};

class cb::NCursesGameEngine {

public:
  enum [[maybe_unused]] colors : short{
      FG_NONE = 0, FG_BLACK,  FG_RED,     FG_GREEN,  FG_YELLOW, FG_BLUE,
      FG_MAGENTA,  FG_GREY,   FG_WHITE,   FG_CYAN,   FG_MAROON, FG_LIME,
      FG_BROWN,    FG_NAVY,   FG_FUCHSIA, FG_TEAL,   FG_GREY1,  FG_GREY2,
      FG_GREY3,    FG_GREY4,  FG_GREY5,   FG_GREY6,  FG_GREY7,  FG_GREY8,
      FG_GREY9,    FG_GREY10, FG_GREY11,  FG_GREY12, FG_GREY13, FG_GREY14,
      FG_GREY15,   FG_GREY16, FG_GREY17,  FG_GREY18, FG_GREY20, FG_GREY21,
      FG_GREY22,   FG_GREY23, FG_GREY24,  FG_GREY25, FG_GREY26, FG_COLORS};

  enum [[maybe_unused]] pixels : short{
      PIXEL_FULL = L'\u2588', PIXEL_LIGHT = L'\u2591', PIXEL_MEDIUM = L'\u2592',
      PIXEL_DARK = L'\u2593'};

  NCursesGameEngine() {

    nScreenWidth = 0;

    nScreenHeight = 0;

    nColors = 0;

    nScreenBufferColors = nullptr;

    nScreenBufferCharacters = nullptr;

    memset(&kmKeys, 0, sizeof(kmKeys));

    memset(&kmKeysPrevious, 0, sizeof(kmKeysPrevious));

    mEvent = {};

    bShowFPS = true;

    m_bAtomActive = false;

    XDisplay = nullptr;

    nWindowID = 0;
  }

  ~NCursesGameEngine() {

    OnUserDestroy();

    if (has_colors())
      if (can_change_color())
        for (short i; i < static_cast<short>(nColors); i++)
          init_color(i, rgb[i].r, rgb[i].g, rgb[i].b);

    endwin();

    delete nScreenBufferColors;

    delete nScreenBufferCharacters;

    if (focus.joinable())
      focus.join();

    if (nullptr != XDisplay)
      XCloseDisplay(XDisplay);
  }

  void ConstructConsole() {

    XDisplay = XOpenDisplay(nullptr);

    const char *pWindowID = getenv("WINDOWID");

    if (nullptr != pWindowID)
      nWindowID = std::strtol(pWindowID, nullptr, 10);

    setlocale(LC_ALL, "");

    initscr();

    if (has_colors()) {

      start_color();

      nColors = std::min(COLORS, static_cast<int>(FG_COLORS));

      if (can_change_color()) {
        short r, g, b;
        for (short i = 0; i < static_cast<short>(nColors); i++) {
          color_content(i, &r, &g, &b);
          init_color(i, rgb[i].r, rgb[i].g, rgb[i].b);
          rgb[i] = {r, g, b};
        }

        assume_default_colors(8, 0);
      }

      for (short i = 0; i < COLORS - 1; i++)
        init_pair(i + 1, i, 0);
    }

    attron(A_NORMAL);

    noecho();

    cbreak();

    nodelay(stdscr, true);

    curs_set(false);

    keypad(stdscr, true);

    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, nullptr);

    getmaxyx(stdscr, nScreenHeight, nScreenWidth);

    nScreenBufferColors = new short[nScreenWidth * nScreenHeight];

    std::fill_n(nScreenBufferColors, nScreenWidth * nScreenHeight, FG_BLACK);

    nScreenBufferCharacters = new wchar_t[nScreenWidth * nScreenHeight];

    std::fill_n(nScreenBufferCharacters, nScreenWidth * nScreenHeight,
                PIXEL_FULL);
  }

  [[maybe_unused]] void Start() {

    m_bAtomFocused = true;

    if (nullptr != XDisplay && 0 != nWindowID)
      focus = std::thread(&NCursesGameEngine::FocusThread, this);

    m_bAtomActive = true;

    loop = std::thread(&NCursesGameEngine::GameThread, this);

    loop.join();
  }

  [[maybe_unused]] inline bool KeyPressed(uint16_t nKey) {

    return kmKeys[(nKey) / 32].bigEndianValue & (1 << ((nKey) % 32));
  }

  [[maybe_unused]] inline bool KeyDown(uint16_t nKey) {

    return !(kmKeysPrevious[(nKey) / 32].bigEndianValue &
             (1 << ((nKey) % 32))) &&
           KeyPressed(nKey);
  }

  [[maybe_unused]] inline bool KeyUp(uint16_t nKey) {

    return (kmKeysPrevious[(nKey) / 32].bigEndianValue &
            (1 << ((nKey) % 32))) &&
           (!KeyPressed(nKey));
  }

  [[maybe_unused]] inline bool MousePressed(long button, int &x, int &y) const {

    if (button & mEvent.bstate) {

      x = mEvent.x;

      y = mEvent.y;

      return true;
    }

    return false;
  }

  [[maybe_unused]] inline void DrawPixel(int x, int y,
                                         wchar_t character = PIXEL_FULL,
                                         short color = FG_WHITE) {

    if (x < 0 || x >= nScreenWidth || y < 0 || y >= nScreenHeight)
      return;

    nScreenBufferColors[x + y * nScreenWidth] = color;

    nScreenBufferCharacters[x + y * nScreenWidth] = character;
  }

  [[maybe_unused]] inline void DrawLine(int x1, int y1, int x2, int y2,
                                        wchar_t character = PIXEL_FULL,
                                        short color = FG_WHITE) {

    int dx = std::abs(x2 - x1);
    int sx = (x1 < x2) ? 1 : -1;
    int dy = -std::abs(y2 - y1);
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx + dy;

    while (true) {
      DrawPixel(x1, y1, character, color);
      if ((x1 == x2) && (y1 == y2))
        break;
      int e2 = 2 * err;
      if (e2 >= dy) {
        err += dy;
        x1 += sx;
      }
      if (e2 <= dx) {
        err += dx;
        y1 += sy;
      }
    }
  }

  [[maybe_unused]] inline void DrawTriangle(int x1, int y1, int x2, int y2,
                                            int x3, int y3,
                                            wchar_t character = PIXEL_FULL,
                                            short color = FG_WHITE) {

    DrawLine(x1, y1, x2, y2, character, color);
    DrawLine(x2, y2, x3, y3, character, color);
    DrawLine(x3, y3, x1, y1, character, color);
  }

  [[maybe_unused]] inline void
  DrawFilledTriangle(int x1, int y1, int x2, int y2, int x3, int y3,
                     wchar_t character = PIXEL_FULL, short color = FG_WHITE) {

    if (y1 > y2) {

      std::swap(y1, y2);
      std::swap(x1, x2);
    }

    if (y2 > y3) {

      std::swap(y2, y3);
      std::swap(x2, x3);
    }

    if (y1 > y2) {

      std::swap(y1, y2);
      std::swap(x1, x2);
    }

    int a, b, y, last;

    if (y1 == y3) {

      a = b = x1;

      if (x2 < a)
        a = x2;
      else if (x2 > b)
        b = x2;

      if (x3 < a)
        a = x3;
      else if (x3 > b)
        b = x3;

      DrawLine(a, y1, b, y1, character, color);

      return;
    }

    int dx12 = x2 - x1, dy12 = y2 - y1, dx13 = x3 - x1, dy13 = y3 - y1,
        dx23 = x3 - x2, dy23 = y3 - y2;

    int sa = 0, sb = 0;

    if (y2 == y3)
      last = y2;
    else
      last = y2 - 1;

    for (y = y1; y <= last; y++) {

      a = x1 + sa / dy12;
      b = x1 + sb / dy13;
      sa += dx12;
      sb += dx13;
      DrawLine(a, y, b, y, character, color);
    }

    sa = dx23 * (y - y2);
    sb = dx13 * (y - y1);
    for (; y <= y3; y++) {
      a = x2 + sa / dy23;
      b = x1 + sb / dy13;
      sa += dx23;
      sb += dx13;
      DrawLine(a, y, b, y, character, color);
    }
  }

  [[maybe_unused]] inline void DrawCircle(int xc, int yc, int r,
                                          wchar_t character = PIXEL_FULL,
                                          short color = FG_WHITE) {

    int x = 0;
    int y = r;
    int d = 3 - (2 * r);

    auto Bresenham = [this](int xc, int yc, int x, int y, wchar_t character,
                            short color) {
      DrawPixel(x + xc, y + yc, character, color);
      DrawPixel(x + xc, -y + yc, character, color);
      DrawPixel(-x + xc, -y + yc, character, color);
      DrawPixel(-x + xc, y + yc, character, color);
      DrawPixel(y + xc, x + yc, character, color);
      DrawPixel(y + xc, -x + yc, character, color);
      DrawPixel(-y + xc, -x + yc, character, color);
      DrawPixel(-y + xc, x + yc, character, color);
    };

    Bresenham(xc, yc, x, y, character, color);
    while (x <= y) {
      if (d <= 0)
        d = d + (4 * x++) + 6;
      else
        d = d + (4 * x++) - (4 * y--) + 10;
      Bresenham(xc, yc, x, y, character, color);
    }
  }

  [[maybe_unused]] inline void DrawFilledCircle(int xc, int yc, int r,
                                                wchar_t character = PIXEL_FULL,
                                                short color = FG_WHITE) {

    int x = 0;
    int y = r;
    int d = 3 - (2 * r);

    auto Bresenham = [this](int xc, int yc, int x, int y, wchar_t character,
                            short color) {
      auto ScanLine = [this](int x0, int x1, int y, wchar_t character,
                             short color) {
        for (int x = x0; x <= x1; x++)
          DrawPixel(x, y, character, color);
      };

      ScanLine(xc - x, xc + x, yc - y, character, color);
      ScanLine(xc - y, xc + y, yc - x, character, color);
      ScanLine(xc - x, xc + x, yc + y, character, color);
      ScanLine(xc - y, xc + y, yc + x, character, color);
    };

    Bresenham(xc, yc, x, y, character, color);
    while (x <= y) {
      if (d <= 0)
        d = d + (4 * x++) + 6;
      else
        d = d + (4 * x++) - (4 * y--) + 10;
      Bresenham(xc, yc, x, y, character, color);
    }
  }

  [[maybe_unused]] inline void DrawRectangle(int x1, int y1, int x2, int y2,
                                             wchar_t character = PIXEL_FULL,
                                             short color = FG_WHITE) {

    if (x1 > x2)
      std::swap(x1, x2);

    if (y1 > y2)
      std::swap(y1, y2);

    DrawLine(x1, y1, x2, y1, character, color);
    DrawLine(x2, y1, x2, y2, character, color);
    DrawLine(x2, y2, x1, y2, character, color);
    DrawLine(x1, y2, x1, y1, character, color);
  }

  [[maybe_unused]] inline void
  DrawFilledRectangle(int x1, int y1, int x2, int y2,
                      wchar_t character = PIXEL_FULL, short color = FG_WHITE) {

    if (x1 > x2)
      std::swap(x1, x2);

    if (y1 > y2)
      std::swap(y1, y2);

    for (int x = x1; x <= x2; x++) {

      if (x < 0 || x > nScreenWidth)
        continue;

      for (int y = y1; y <= y2; y++) {

        if (y < 0 || y > nScreenHeight)
          continue;

        nScreenBufferColors[x + y * nScreenWidth] = color;

        nScreenBufferCharacters[x + y * nScreenWidth] = character;
      }
    }
  }

  [[maybe_unused]] inline void DrawString(int x, int y,
                                          const std::wstring &str, int color = FG_WHITE) {

    if (x < 0 || x >= nScreenWidth || y < 0 || y >= nScreenHeight)
      return;

    for (auto &c : str) {

      nScreenBufferColors[x + y * nScreenWidth] = color;

      nScreenBufferCharacters[x + y * nScreenWidth] = c;

      if (++x >= nScreenWidth)
        break;
    }
  }

  [[maybe_unused]] inline void DrawAlphaString(int x, int y,
                                               const std::wstring &str, int color = FG_WHITE) {

    if (x < 0 || x >= nScreenWidth || y < 0 || y >= nScreenHeight)
      return;

    for (auto &c : str) {

      if (c != L' ') {

        nScreenBufferColors[x + y * nScreenWidth] = color;

        nScreenBufferCharacters[x + y * nScreenWidth] = c;
      }

      if (++x >= nScreenWidth)
        break;
    }
  }

  [[maybe_unused]] inline void DrawSprite(cb::Sprite &s, int x0, int y0) {

    for (int x = 0; x < s.SpriteWidth(); x++) {
      for (int y = 0; y < s.SpriteHeight(); y++) {
        Pixel pixel = s[x + y * s.SpriteWidth()];
        DrawPixel(x0 + x, y0 + y, pixel.character, pixel.color);
      }
    }
  }

  [[maybe_unused]] inline void DrawSprite(cb::Sprite &s, int x0, int y0, int sx,
                                          int sy, int width, int height) {

    for (int x = 0; x < width; x++) {
      for (int y = 0; y < height; y++) {
        Pixel pixel = s[(x + sx) + (y + sy) * s.SpriteWidth()];
        DrawPixel(x0 + x, y0 + y, pixel.character, pixel.color);
      }
    }
  }

  [[maybe_unused]] inline void Clear(wchar_t character = PIXEL_FULL,
                                     short color = FG_WHITE) {

    std::fill_n(nScreenBufferCharacters, nScreenWidth * nScreenHeight,
                character);

    std::fill_n(nScreenBufferColors, nScreenWidth * nScreenHeight, color);
  }

  [[maybe_unused]] inline int ScreenWidth() const { return nScreenWidth; }

  [[maybe_unused]] inline int ScreenHeight() const { return nScreenHeight; }

  [[maybe_unused]] inline bool IsFocused() { return m_bAtomFocused; }

  [[maybe_unused]] virtual void OnUserDestroy(){};

  [[maybe_unused]] virtual bool OnUserResize() { return true; }

  [[maybe_unused]] virtual bool OnUserCreate() = 0;

  [[maybe_unused]] virtual bool OnUserUpdate(float fElapsedTime) = 0;

private:
  [[maybe_unused]] void FocusThread() {

    Window w;
    int r;

    while (m_bAtomActive) {

      XGetInputFocus(XDisplay, &w, &r);

      m_bAtomFocused = (nWindowID == w);

      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  }

  [[maybe_unused]] void GameThread() {

    if (OnUserCreate()) {

      struct timespec sStartTimespec {
      }, sStopTimespec{};

      float fElapsedTime = 0.0f;

      clock_gettime(CLOCK_MONOTONIC_RAW, &sStartTimespec);

      while (m_bAtomActive) {

        clock_gettime(CLOCK_MONOTONIC_RAW, &sStopTimespec);

        memcpy(&kmKeysPrevious, &kmKeys, sizeof(kmKeys));

        GetKeys(kmKeys);

        m_bAtomActive = OnUserUpdate(fElapsedTime);

        fElapsedTime = (float)(sStopTimespec.tv_sec - sStartTimespec.tv_sec) +
                       (float)(sStopTimespec.tv_nsec - sStartTimespec.tv_nsec) /
                           1000000000.0f;

        short nColor = *nScreenBufferColors;

        color_set(nColor, NULL);

        move(0, 0);

        for (int pixel = 0; pixel < nScreenWidth * nScreenHeight; pixel++) {

          if (nColor != nScreenBufferColors[pixel]) {

            nColor = nScreenBufferColors[pixel];
            color_set(nColor, NULL);
          }

          printw("%lc", nScreenBufferCharacters[pixel]);
        }

        int nKey;

        mEvent = {};

        if (bShowFPS) {
          color_set(FG_GREEN, NULL);
          mvprintw(nScreenHeight - 1, 0, "FPS: %10d",
                   (int)(1.0f / fElapsedTime));
        }

        while ((nKey = getch()) != ERR)
          if (nKey == KEY_MOUSE)
            getmouse(&mEvent);
          else if (nKey == KEY_RESIZE) {

            getmaxyx(stdscr, nScreenHeight, nScreenWidth);

            delete nScreenBufferColors;
            nScreenBufferColors =
                new short[nScreenWidth * nScreenHeight]{FG_BLACK};

            delete nScreenBufferCharacters;
            nScreenBufferCharacters =
                new wchar_t[nScreenWidth * nScreenHeight]{PIXEL_FULL};

            m_bAtomActive = OnUserResize();
          }

        sStartTimespec = sStopTimespec;
      }
    }
  }

  int nScreenWidth;

  int nScreenHeight;

  Display *XDisplay;

  Window nWindowID;

  int nColors;

  short *nScreenBufferColors;

  wchar_t *nScreenBufferCharacters;

  KeyMap kmKeys{};

  KeyMap kmKeysPrevious{};

  MEVENT mEvent{};

  std::thread loop;

  std::thread focus;

  std::atomic<bool> m_bAtomActive{};

  std::atomic<bool> m_bAtomFocused{};

  struct {
    short r;
    short g;
    short b;
  } rgb[41] = {
      // https://jonasjacek.github.io/colors/
      {0, 0, 0},       // black
      {999, 0, 0},     // red
      {0, 999, 0},     // green
      {999, 999, 0},   // yellow
      {0, 0, 999},     // blue
      {999, 0, 999},   // magenta
      {753, 753, 753}, // grey
      {999, 999, 999}, // white
      {999, 999, 0},   // cyan
      {499, 0, 0},     // maroon
      {753, 999, 0},   // lime
      {499, 499, 0},   // brown
      {0, 0, 499},     // navy
      {945, 611, 733}, // fuchsia
      {0, 499, 499},   // teal
      {999, 999, 999}, {31, 31, 31},    {71, 71, 71},    {111, 111, 111},
      {151, 151, 151}, {191, 191, 191}, {231, 231, 231}, {271, 271, 271},
      {311, 311, 311}, {341, 341, 341}, {381, 381, 381}, {421, 421, 421},
      {461, 461, 461}, {501, 501, 501}, {541, 541, 541}, {581, 581, 581},
      {621, 621, 621}, {661, 661, 661}, {701, 701, 701}, {741, 741, 741},
      {781, 781, 781}, {821, 821, 821}, {861, 861, 861}, {901, 901, 901},
      {941, 941, 941}, {981, 981, 981}};

protected:
  bool bShowFPS;
};
#endif // CBNCURSESGAMEENGINE_NCURSESGAMEENGINE_H
