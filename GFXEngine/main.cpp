/**
 *  @file   main.cpp
 *  @brief  3D Graphics Engine
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-29
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "GFXEngine.h"

int main(const int argc, const char *argv[]) {

    GFXEngine gfx(argc, argv);

    gfx.ConstructConsole();

    gfx.Start();

    return 0;
}
