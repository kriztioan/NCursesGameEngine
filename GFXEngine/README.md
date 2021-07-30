# GFXEngine

`GFXEngine` demonstrates the use of a  [projection matrix](https://en.wikipedia.org/wiki/Projection_matrix) to display 3D graphics. It is inspired by a One Lone Coder [YouTube video](https://youtu.be/ih20l3pJoeU) ([GitHub](https://github.com/OneLoneCoder/videos/blob/master/OneLoneCoder_olcEngine3D_Part1.cpp)).

`GFXEngine` is written using the [`NCurses Game Engine`](../README.md).

## Usage

`GFXEngine` is compiled with:

```shell
cmake .
make
```

This results in a binary executable called `GFXEngine`, which is invoked as:

```shell
./GFXEngine models/Teapot.obj
```

The first command line parameter points to a file defining a mesh object. These are simple text files containing the vertices. [Blender](https://www.blender.org) is capable of exporting to this file format. The [models](models/)-directory holds the files of a few objects.

Press `q` to quit.

## Notes

1. Set `TERM` to `xterm-256colors` in your terminal for the best results.
2. For a 'retro' feel and square pixels, install a `classic text mode font` from [The Ultimate Oldschool PC Font Pack](https://int10h.org/oldschool-pc-fonts/).
3. `GFXEngine` allows dynamic resizing of the terminal window.

## BSD-3 License

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
