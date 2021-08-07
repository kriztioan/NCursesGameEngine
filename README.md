# NCurses Game Engine

The `NCurses Game Engine` is a `C++`-header-only library for developing games for in the terminal. It relies on the `ncurses` `API` for terminal interactions. The engine itself was inspired by the One Lone Coder's [olcConsoleGameEngine](https://github.com/OneLoneCoder/videos/blob/master/olcConsoleGameEngine.h) and many of the projects from his [YouTube videos](https://www.youtube.com/channel/UC-yuWVUplUJZvieEligKBkA).

The library has `MacOS` in mind. This is because the `ncurses` `API` does not provide a key being pressed and being released as separate events. Therefore, the `NCurses Game Engine` relies on capturing key presses and releases system-wide. This does have the unfortunate side effect that input is still processed even when the terminal does not has the focus.

However, a work-around is in place when using `xterm`, which requires the [XQuartz](https://www.xquartz.org) X11 window manager. The `NCurses Game Engine` polls the `X11` display server every 0.5s to check whether its terminal window has the current focus. When not, key presses and releases are ignored. The polling is done in a separate thread to not impact the game loop.

## Usage

The library consists of three header files, which are listed in the table below together with their usage.

|header|usage|
-------|------
|`NCursesGameEngine.h`|main library|
|`Sprite.h`|handle sprites|
|`GFXToolKit.h`|2D and 3D vector/matrix math|

Note that the library is set in the`namespace` `cb::`.

A number of projects are available in subdirectories. See each of them for details on how to use the `NCurses Game Engine`.

## Bitmap2Sprite

`Bitmap2Sprite` is a tool for converting a [Bitmap](https://en.wikipedia.org/wiki/BMP_file_format) file into a format that can be read as a `cb::Sprite`. The tool is compiled with:

```shell
make
```

This results in a binary executable called `Bitmap2Sprite`, which is invoked as:

```shell
./Bitmap2Sprite picture.bmp picture.sprite
```

A third optional parameters sets the number of colors, out of 41, to use in the conversion.

```shell
./Bitmap2Sprite picture.bmp picture.sprite 30
```

## Notes

1. Set `TERM` to `xterm-256colors` in your terminal for the best results.
2. For a 'retro' feel and square pixels, install a `classic text mode font` from [The Ultimate Oldschool PC Font Pack](https://int10h.org/oldschool-pc-fonts/).
3. Most projects allow dynamic resizing of the terminal window.

## BSD-3 License

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
