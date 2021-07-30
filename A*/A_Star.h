/**
 *  @file   A_Star.h
 *  @brief  A* in the NCursesGameEngine
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-29
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef CBNCURSESGAMEENGINE__A_STAR_H
#define CBNCURSESGAMEENGINE__A_STAR_H

#include "../NCursesGameEngine.h"

#include <list>
#include <vector>
#include <ctime>
#include <cmath>

class A_Star : public cb::NCursesGameEngine {

    struct sNode {
        bool bValid = true;
        bool bVisited = false;
        float fGlobalGoal;
        float fLocalGoal;
        int x;
        int y;
        sNode *parent = nullptr;
        std::vector< sNode *> siblings;
    };

    sNode *nodes = nullptr;

    sNode *p0 = nullptr;

    sNode *p1 = nullptr;

    static constexpr int nCellSize = 10;

    static constexpr int nCellPadding = 2;

    int nMapWidth = 0;

    int nMapHeight = 0;

    bool OnUserCreate() override {

        nMapHeight = ( ScreenHeight() / nCellSize );

        nMapWidth = ( ScreenWidth() / nCellSize );

        nodes = new sNode[ nMapWidth * nMapHeight ];

        for( int x = 0; x < nMapWidth; x++ ) {

            for( int y = 0; y < nMapHeight; y++ ) {

                nodes[ x + y * nMapWidth ].x = x;

                nodes[ x + y * nMapWidth ].y = y;

                if ( ( y - 1 ) >= 0 ) nodes[ x + y * nMapWidth ].siblings.emplace_back( &nodes[ x + ( y - 1 ) * nMapWidth ] );
                if ( ( x - 1 ) >= 0 ) nodes[ x + y * nMapWidth ].siblings.emplace_back( &nodes[ ( x - 1 ) + y * nMapWidth ] );
                if ( ( y + 1 ) < nMapHeight ) nodes[ x + y * nMapWidth ].siblings.emplace_back( &nodes[ x + ( y + 1 ) * nMapWidth ] );
                if ( ( x + 1 ) < nMapWidth ) nodes[ x + y * nMapWidth ].siblings.emplace_back( &nodes[ ( x + 1 ) + y * nMapWidth ] );
            }
        }

        srand( time( nullptr ) );

        p0 = &nodes[ ( ( rand() % nMapWidth ) + ( rand() % nMapHeight ) * nMapWidth ) ];

        p1 = &nodes[ ( ( rand() % nMapWidth ) + ( rand() % nMapHeight ) * nMapWidth ) ];

        Solve();

        return true;
    };

    void OnUserDestroy() override { delete nodes; };

    bool OnUserUpdate( float fElapsedTime ) override {

        if( IsFocused() && KeyUp( kVK_ANSI_Q ) ) return false;

        Input();

        Draw();

        return true;
    }

    inline float Distance( sNode *n1, sNode *n2 ) { return std::sqrt( ( n1->x - n2->x ) * ( n1->x - n2->x ) + ( n1->y - n2->y ) * ( n1->y - n2->y ) ); }

    void Solve() {

        for( int i = 0; i < nMapWidth * nMapHeight; i++ ){

            nodes[ i ].bVisited = false;
            nodes[ i ].fGlobalGoal = INFINITY;
            nodes[ i ].fLocalGoal = INFINITY;
            nodes[ i ].parent = nullptr;
        }

        sNode* current = p0;
        current->fLocalGoal = 0.0f;
        current->fGlobalGoal = Distance( p0, p1 );

        std::list< sNode * > TestList;

        TestList.emplace_back( p0 );

        while( !TestList.empty() && current != p1 ) {

            TestList.sort([](const sNode *n1, const sNode *n2) {

                return n1->fGlobalGoal < n2->fGlobalGoal;
            });

            TestList.remove_if([](const sNode *n) {

                return n->bVisited;
            });

            if (TestList.empty()) break;

            current = TestList.front();

            current->bVisited = true;

            for (auto &node : current->siblings) {

                if (!node->bVisited && node->bValid) TestList.emplace_back(node);

                float fGoal = current->fLocalGoal + Distance(current, node);

                if (fGoal < node->fLocalGoal) {

                    node->parent = current;

                    node->fLocalGoal = fGoal;

                    node->fGlobalGoal = node->fLocalGoal + Distance(node, p1);
                }
            }
        }
    }

    void Input() {

        int nMouseX, nMouseY;

        if( MousePressed( BUTTON1_CLICKED, nMouseX, nMouseY ) ) {
            nodes[ ( nMouseX / nCellSize ) + ( nMouseY / nCellSize ) * nMapWidth ].bValid = !nodes[ ( nMouseX / nCellSize ) + ( nMouseY / nCellSize ) * nMapWidth ].bValid;
            Solve();
        }

        if( MousePressed( BUTTON1_DOUBLE_CLICKED, nMouseX, nMouseY ) ) {
            p0 = &nodes[ ( nMouseX / nCellSize ) + ( nMouseY / nCellSize ) * nMapWidth ];
            Solve();
        }

        if( MousePressed( BUTTON1_TRIPLE_CLICKED, nMouseX, nMouseY ) ) {
            p1 = &nodes[ ( nMouseX / nCellSize ) + ( nMouseY / nCellSize ) * nMapWidth ];
            Solve();
        }
    }

    void Draw() {

        Clear( PIXEL_FULL, FG_TEAL );

        for( int i = 0; i < nMapWidth * nMapHeight; i++ ) {

            if( !nodes[ i ].bValid ) continue;

            for( auto &node : nodes[ i ].siblings ) {

                if( !node->bValid ) continue;

                DrawRectangle(nCellSize * nodes[ i ].x + ( nCellSize / 2 ),nCellSize * nodes[ i ].y + ( nCellSize / 2 ),nCellSize * node->x + ( nCellSize / 2 ),nCellSize * node->y + ( nCellSize / 2 ), PIXEL_FULL, FG_YELLOW );
            }
        }

        for( int i = 0; i < ( nMapWidth * nMapHeight ); i++ ) {

            short color = nodes[ i ].bValid ? FG_BLUE : FG_GREY;

            DrawRectangle( nCellSize * nodes[ i ].x + nCellPadding, nCellSize * nodes[ i ].y + nCellPadding, nCellSize * nodes[ i ].x + nCellSize - nCellPadding, nCellSize * nodes[ i ].y + nCellSize - nCellPadding, PIXEL_FULL, color );
        }

        sNode *n = p1;

        while( n->parent != nullptr ) {

            DrawRectangle(nCellSize * n->x + ( nCellSize / 2 ),nCellSize * n->y + ( nCellSize / 2 ), nCellSize * n->parent->x + ( nCellSize / 2 ), nCellSize * n->parent->y + ( nCellSize / 2 ), PIXEL_FULL, FG_GREEN );

            DrawFilledCircle( nCellSize * n->x + ( nCellSize / 2 ), nCellSize * n->y + ( nCellSize / 2 ), nCellSize / 4, PIXEL_FULL, FG_GREEN );

            n = n->parent;
        }

        /*for( int i = 0; i < ( nMapWidth * nMapHeight ); i++ ) {

            if(!nodes[ i ].bVisited)continue;

            DrawRectangle( nCellSize * nodes[ i ].x + nCellPadding, nCellSize * nodes[ i ].y + nCellPadding, nCellSize * nodes[ i ].x + nCellSize - nCellPadding, nCellSize * nodes[ i ].y + nCellSize - nCellPadding, PIXEL_FULL, FG_YELLOW );
        }*/

        DrawRectangle( nCellSize * p0->x + nCellPadding, nCellSize * p0->y + nCellPadding, nCellSize * p0->x + nCellSize - nCellPadding, nCellSize * p0->y + nCellSize - nCellPadding, PIXEL_FULL, FG_RED );

        DrawRectangle( nCellSize * p1->x + nCellPadding, nCellSize * p1->y + nCellPadding, nCellSize * p1->x + nCellSize - nCellPadding, nCellSize * p1->y + nCellSize - nCellPadding, PIXEL_FULL, FG_GREEN );
    }

    bool OnUserResize() override { return true; }
};

#endif // CBNCURSESGAMEENGINE__A_STAR_H
