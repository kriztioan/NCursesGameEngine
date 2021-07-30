/**
 *  @file   main.cpp
 *  @brief  Tetris
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-29
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Tetris.h"

int main() {

    Tetris t;

    t.ConstructConsole();
    t.Start();

    return 0;
}
