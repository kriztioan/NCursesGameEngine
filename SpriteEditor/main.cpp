/**
 *  @file   main.cpp
 *  @brief  Sprite Editor
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-29
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "SpriteEditor.h"

int main(const int argc, const char *argv[]) {

    SpriteEditor se(argc, argv);

    se.ConstructConsole();
    se.Start();

    return 0;
}