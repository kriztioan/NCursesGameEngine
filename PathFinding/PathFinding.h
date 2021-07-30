/**
 *  @file   PathFinding.h
 *  @brief  Path Finding in the NCursesGameEngine
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-29
 *  @note   BSD-3 licensed
 *
 ***********************************************/
#ifndef CBNCURSESGAMEENGINE_PATHFINDING_H
#define CBNCURSESGAMEENGINE_PATHFINDING_H

#include "../NCursesGameEngine.h"

#include <list>
#include <tuple>

class PathFinding : public cb::NCursesGameEngine {

    static constexpr int nCellSize = 3;

    static constexpr int nCellPadding = 1;

    int nMapWidth = 0;

    int nMapHeight = 0;

    bool *bCells = nullptr;

    int nStartX = 3;

    int nStartY = 4;

    int nEndX = 9;

    int nEndY = 9;

    int *nFlowFieldZ = nullptr;

    bool OnUserCreate() final {

        nMapHeight = ( ScreenHeight() / nCellSize );

        nMapWidth = ( ScreenWidth() / nCellSize );

        bCells = new bool[ nMapHeight * nMapWidth ] { false };

        nFlowFieldZ = new int[ nMapHeight * nMapWidth ] { 0 };

        Clear( PIXEL_FULL, FG_TEAL );

        return true;
    }

    bool OnUserResize() final {

        nMapHeight = ( ScreenHeight() / nCellSize );

        nMapWidth = ( ScreenWidth() / nCellSize );

        delete bCells;
        bCells = new bool[ nMapHeight * nMapWidth ] { false };

        delete nFlowFieldZ;
        nFlowFieldZ = new int[ nMapHeight * nMapWidth ] { 0 };

        Clear( PIXEL_FULL, FG_BLUE );

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) final {

        if( IsFocused() && KeyPressed( kVK_ANSI_Q ) ) return false;

        int nMouseX, nMouseY;

        if( MousePressed( BUTTON1_CLICKED, nMouseX, nMouseY ) )
            bCells[ ( nMouseX / nCellSize ) + ( nMouseY / nCellSize ) * nMapWidth ] = !bCells[ ( nMouseX / nCellSize ) + ( nMouseY / nCellSize ) * nMapWidth ];

        if( MousePressed( BUTTON1_DOUBLE_CLICKED, nMouseX, nMouseY ) ) {

            nStartX = (nMouseX / nCellSize);

            nStartY = (nMouseY / nCellSize);
        }

        if( MousePressed( BUTTON1_TRIPLE_CLICKED, nMouseX, nMouseY ) ) {

            nEndX = (nMouseX / nCellSize);

            nEndY = (nMouseY / nCellSize);
        }

        for( int x = 0; x < nMapWidth; x++ ) {

            for( int y = 0; y < nMapHeight; y++ ) {

                if ( x == 0 || y == 0 || x == ( nMapWidth - 1 ) || y == ( nMapHeight - 1 ) || bCells[ x + y * nMapWidth ] )
                    nFlowFieldZ[ x + y * nMapWidth ] = -1;
                else nFlowFieldZ[ x + y * nMapWidth ] = 0;
            }
        }

        std::list< std::tuple< int, int, int > > nodes; // 0 = x / 1 = y / 2 = distance

        nodes.emplace_back( nEndX, nEndY, 1 );

        while ( !nodes.empty() ) {

            std::list< std::tuple< int, int, int > > new_nodes;

            for( auto &n : nodes ) {

                int x = std::get< 0 >( n );
                int y = std::get< 1 >( n );
                int d = std::get< 2 >( n );

                nFlowFieldZ[ x + y * nMapWidth ] = d;

                if ( ( x + 1 ) < nMapWidth && nFlowFieldZ[ ( x + 1) + y * nMapWidth ] == 0 ) new_nodes.emplace_back( x + 1, y, d + 1 );
                if ( ( x - 1 ) >= 0 && nFlowFieldZ[ ( x - 1) + y * nMapWidth ] == 0 ) new_nodes.emplace_back( x - 1, y, d + 1 );
                if ( ( y + 1 ) < nMapHeight && nFlowFieldZ[  x + ( y + 1 ) * nMapWidth ] == 0 ) new_nodes.emplace_back( x, y + 1, d + 1 );
                if ( ( y - 1 ) >= 0 && nFlowFieldZ[  x + ( y - 1 ) * nMapWidth ] == 0 ) new_nodes.emplace_back( x, y - 1, d + 1 );
            }

            new_nodes.sort( [ & ] ( const std::tuple< int, int, int > n1, const std::tuple< int, int, int > n2 ) {

                return ( std::get< 0 >( n1 ) + std::get< 1 >( n1 ) * nMapWidth ) < ( std::get< 0 >( n2 ) + std::get< 1 >( n2 ) * nMapWidth );
            } );

            new_nodes.unique( [ & ] ( const std::tuple< int, int, int > n1, const std::tuple< int, int, int > n2 ) {

                return ( std::get< 0 >( n1 ) + std::get< 1 >( n1 ) * nMapWidth ) == ( std::get< 0 >( n2 ) + std::get< 1 >( n2 ) * nMapWidth );
            });

            nodes.clear();

            nodes.insert( nodes.begin(), new_nodes.begin(), new_nodes.end() );
        }

        std::list< std::pair< int, int > > path;

        path.emplace_back( nStartX, nStartY );

        int nCurrentX = nStartX;

        int nCurrentY = nStartY;

        bool bFail = false;

        while( !( nCurrentX == nEndX && nCurrentY == nEndY ) && !bFail ) {

            std::list< std::tuple< int, int, int > > neighbors;

            if( ( nCurrentY - 1 ) >= 0 && nFlowFieldZ[ nCurrentX + ( nCurrentY - 1 ) * nMapWidth ] > 0 ) neighbors.emplace_back( nCurrentX, nCurrentY - 1, nFlowFieldZ[ nCurrentX + ( nCurrentY - 1 ) * nMapWidth ] );
            if( ( nCurrentX + 1 ) < nMapWidth && nFlowFieldZ[ ( nCurrentX + 1 ) + nCurrentY * nMapWidth ] > 0 ) neighbors.emplace_back( nCurrentX + 1, nCurrentY, nFlowFieldZ[ ( nCurrentX + 1 ) + nCurrentY * nMapWidth ] );
            if( ( nCurrentY + 1 ) < nMapHeight && nFlowFieldZ[ nCurrentX + ( nCurrentY + 1 ) * nMapWidth ] > 0 ) neighbors.emplace_back( nCurrentX, nCurrentY + 1 , nFlowFieldZ[ nCurrentX + ( nCurrentY + 1 ) * nMapWidth ] );
            if( ( nCurrentX - 1 ) >= 0 && nFlowFieldZ[ ( nCurrentX - 1 ) + nCurrentY * nMapWidth ] > 0 ) neighbors.emplace_back( nCurrentX - 1, nCurrentY, nFlowFieldZ[ ( nCurrentX - 1 ) + nCurrentY * nMapWidth ] );

            neighbors.sort( [ & ] ( std::tuple< int, int, int > n1, std::tuple< int, int, int > n2) {

                return std::get< 2 >( n1 ) < std::get < 2 >( n2 );
            });

            if ( neighbors.empty() ) bFail = true;
            else {

                nCurrentX = std::get< 0 >( neighbors.front() );

                nCurrentY = std::get< 1 >( neighbors.front() );

                path.emplace_back( nCurrentX, nCurrentY );
            }
        }

        for( int x = 0; x < nMapWidth; x++ ) {

            for (int y = 0; y < nMapHeight; y++) {

                short color = bCells[ x + y * nMapWidth ] ? FG_BLACK : FG_BLUE;

                DrawRectangle(nCellSize * x + nCellPadding, nCellSize * y + nCellPadding,nCellSize * x + nCellSize - nCellPadding, nCellSize * y + nCellSize - nCellPadding, PIXEL_FULL, color);

                //DrawString( x * nCellSize + nCellPadding, y * nCellSize + nCellPadding, std::to_wstring( nFlowFieldZ [ x + y * nMapWidth ] ) );
            }
        }

        for( auto &p : path ) DrawRectangle( nCellSize * p.first + nCellPadding, nCellSize * p.second + nCellPadding, nCellSize * p.first + nCellSize - nCellPadding, nCellSize * p.second + nCellSize - nCellPadding, PIXEL_FULL, FG_YELLOW );

        DrawRectangle( nCellSize * nStartX + nCellPadding, nCellSize * nStartY + nCellPadding, nCellSize * nStartX + nCellSize - nCellPadding, nCellSize * nStartY + nCellSize - nCellPadding, PIXEL_FULL, FG_RED );

        DrawRectangle( nCellSize * nEndX + nCellPadding, nCellSize * nEndY + nCellPadding, nCellSize * nEndX + nCellSize - nCellPadding, nCellSize * nEndY + nCellSize - nCellPadding, PIXEL_FULL, FG_GREEN );

        return true;
    }

    void OnUserDestroy() final {

        delete bCells;

        delete nFlowFieldZ;
    }
};
#endif // CBNCURSESGAMEENGINE_PATHFINDING_H
