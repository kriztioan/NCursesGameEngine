/**
 *  @file   Tetris.h
 *  @brief  Tetris in the NCursesGameEngine
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-29
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef CBNCURSESGAMEENGINE_TETRIS_H
#define CBNCURSESGAMEENGINE_TETRIS_H

#include "../NCursesGameEngine.h"

#include <ctime>
#include <vector>
#include <sstream>
#include <iomanip>
#include <fstream>

class Tetris : public cb::NCursesGameEngine {

    static constexpr int nTetromino[][16] = {{ 0, 0, 0, 0,
                                               0, 0, 0, 0,
                                               0, 0, 0, 0,
                                               0, 0, 0, 0},
                                             { 0, 0, 1, 0,
                                               0, 0, 1, 0,
                                               0, 0, 1, 0,
                                               0, 0, 1, 0},
                                             { 0, 0, 0, 0,
                                               0, 1, 1, 0,
                                               0, 1, 1, 0,
                                               0, 0, 0, 0},
                                             { 0, 0, 1, 0,
                                               0, 1, 1, 0,
                                               0, 0, 1, 0,
                                               0, 0, 0, 0},
                                             { 0, 1, 0, 0,
                                               0, 1, 0, 0,
                                               0, 1, 1, 0,
                                               0, 0, 0, 0},
                                             { 0, 0, 0, 0,
                                               0, 1, 1, 0,
                                               0, 0, 1, 1,
                                               0, 0, 0, 0}};

    int nShape;

    int nNextShape;

    static constexpr int nFieldWidth = 12;

    static constexpr int nFieldHeight = 22;

    int *nField;

    int nXpos;

    int nYpos;

    int nRotation;

    float fTimer;

    float fSpeed;

    int nScore;

    int nHighScore;

    std::vector< int > vLines;

    bool bGameOver = false;

    bool bFinished = false;

    bool OnUserCreate() override {

        srand( time( nullptr ) );

        nField = new int[ nFieldWidth * nFieldHeight ];

        Reset();

        Clear( PIXEL_MEDIUM, FG_GREY );

        return true;
    }

    void OnUserDestroy() override {

        delete nField;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        if( IsFocused() ) {

            fTimer += fElapsedTime;

            Input();

            Draw();

            Logic();
        }

        std::this_thread::sleep_for( std::chrono::milliseconds( 20 ) );

        return !bFinished;
    }

    void Input() {

        if ( KeyDown(kVK_Space ) ) {
            if ( bGameOver ) {
                Reset();
                bGameOver = false;
            } else if( CheckMove(nXpos, nYpos, nRotation + 1 )) ++nRotation;
        }

        if( !bGameOver && KeyDown( kVK_LeftArrow ) && CheckMove(nXpos - 1, nYpos, nRotation) ) --nXpos;
        if( !bGameOver && KeyDown(kVK_RightArrow ) && CheckMove(nXpos + 1, nYpos, nRotation ) ) ++nXpos;
        if( !bGameOver && KeyDown(kVK_DownArrow ) && CheckMove( nXpos, nYpos + 1, nRotation ) ) ++nYpos;
        if (KeyUp(kVK_ANSI_R ) ) Reset();
        if (KeyUp(kVK_ANSI_Q) ) bFinished = true;
    }

    void Draw() {

        int nOffsetX = ( ScreenWidth() - nFieldWidth ) / 2;

        int nOffsetY = ( ScreenHeight() - nFieldHeight ) / 2;

        for( int x = 0; x < nFieldWidth; x++ ) {
            for ( int y = 0; y < nFieldHeight; y++ )
                DrawPixel(nOffsetX + x, nOffsetY + y, PIXEL_FULL, nField[ x + nFieldWidth * y ] + 1);
        }

        for( int px = 0; px < 4; px++ ) {
            for (int py = 0; py < 4; py++)
                if ( nTetromino[ nShape ][ Rotate( px, py, nRotation ) ] > 0 )
                    DrawPixel(nXpos + nOffsetX + px, nYpos + nOffsetY + py, PIXEL_FULL, nShape + 1);
        }

        for( int px = 0; px < 4; px++ ) {
            for (int py = 0; py < 4; py++) {
                if( nTetromino[ nNextShape ][ px + py * 4] > 0 )
                    DrawPixel(nOffsetX + nFieldWidth + px, nOffsetY + py, PIXEL_FULL, nNextShape + 1);
                else
                    DrawPixel(nOffsetX + nFieldWidth + px, nOffsetY + py, PIXEL_MEDIUM, FG_GREY);
            }
        }

        std::wstringstream ss;

        ss << "SCORE: " << std::setw( nFieldWidth - 7 ) << nScore;

        DrawString( nOffsetX, nOffsetY + nFieldHeight + 1, ss.str() );

        ss.str( L"" );

        ss << "HIGH : " << std::setw( nFieldWidth - 7 ) << nHighScore;

        DrawString( nOffsetX, nOffsetY + nFieldHeight + 2, ss.str() );

        if( bGameOver ) DrawString( nOffsetX + 2, nOffsetY + nFieldHeight / 2 - 1, L"GAME OVER", FG_RED );
    }

    void Logic() {

        if ( !vLines.empty() && fTimer >= ( fSpeed / 2 ) ) {

            for( auto &py : vLines ) {
                for( int x = 1; x < ( nFieldWidth - 1 ); x++ )
                    for( int y = py; y > 1; y-- )
                        nField[ y * nFieldWidth + x ] = nField[ ( y - 1 ) * nFieldWidth + x ];
            }

            vLines.clear();
        }

        if( fTimer >= fSpeed && !bGameOver ) {

            if( CheckMove( nXpos, nYpos + 1, nRotation ) ) ++nYpos;
            else {

                for( int px = 0; px < 4; px++ ) {
                    for( int py = 0; py < 4; py++ )
                        if ( nTetromino[ nShape ][ Rotate( px, py, nRotation ) ] > 0 ) nField[ ( nXpos + px ) + ( nYpos + py ) * nFieldWidth ] = nShape;
                }

                for( int py = 0; py < 4; py++ ) {

                    if( nYpos + py < ( nFieldHeight - 1 ) ) {

                        bool bLine = true;

                        for( int x = 1; x < ( nFieldWidth - 1 ); x++ )
                            bLine &= ( nField[ x + ( nYpos + py ) * nFieldWidth ] > 0 );

                        if ( bLine ) {

                            vLines.emplace_back( nYpos + py );

                            for( int x = 1; x < ( nFieldWidth - 1 ); x++ )
                                nField[ x + ( nYpos + py ) * nFieldWidth ] = FG_LIME;
                        }
                    }

                    if( !vLines.empty() ) fSpeed -= 0.005;

                    nScore += ( 50 * vLines.size() ) * vLines.size();
                }

                nXpos = nFieldWidth / 2 - 2;

                nYpos = 1;

                nRotation = 0;

                nShape = nNextShape;

                nNextShape = 1 + ( rand() % 5 );

                if( !CheckMove( nXpos, nYpos, nRotation ) ) {

                    bGameOver = true;

                    if( nScore > nHighScore ) {

                        std::ofstream ofstr( L".tetris" );

                        if( ofstr.good() ) {

                            ofstr << nScore;

                            ofstr.close();
                        }
                    }
                }
            }

            fTimer = 0.0f;
        }
    }

    void Reset() {

        nXpos = nFieldWidth / 2 - 2;

        nYpos = 1;

        nRotation = 0;

        nShape = 1 + ( rand() % 5 );

        nNextShape = 1 + ( rand() % 5 );

        std::fill_n( nField,  nFieldWidth * nFieldHeight, 0 );

        for( int x = 0; x < nFieldWidth; x++ )
            for( int y = 0; y < nFieldHeight; y++ )
                if( x == 0 || y == 0 || x == ( nFieldWidth - 1 ) || y == ( nFieldHeight - 1 ) )
                    nField[ x + nFieldWidth * y ] = FG_WHITE - 1;

        fTimer = 0.0f;

        fSpeed = 0.2f;

        std::ifstream ifstr(L".tetris");
        if( ifstr.good() ) {
            ifstr >> nHighScore;
            ifstr.close();
        } else nHighScore = 0;

        nScore = 0;
    }

    bool CheckMove( int x, int y, int rotation ) {

        for( int px = 0; px < 4; px++ )
            for( int py = 0; py < 4; py++ )
                if( nTetromino[ nShape ][ Rotate( px, py, rotation ) ] > 0 &&
                    nField[ ( x + px ) + ( y + py ) * nFieldWidth ] > 0 ) return false;

        return true;
    }

    int Rotate( int x, int y, int rotation ) {

        int i = 0;

        switch ( rotation % 4  ) {
            case 0: i = y * 4 + x; break;
            case 1: i = 12 + y - ( x * 4 ); break;
            case 2: i = 15 - ( y * 4 ) - x; break;
            case 3: i = 3 - y + ( x * 4 );
        }

        return i;
    }

    bool OnUserResize() override { Clear( PIXEL_MEDIUM, FG_GREY ); return true; }

public:

    Tetris() { bShowFPS = false; };
};

#endif //CBNCURSESGAMEENGINE_TETRIS_H
