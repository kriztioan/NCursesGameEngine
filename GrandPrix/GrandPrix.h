/**
 *  @file   GrandPrix.h
 *  @brief  Grand Prix Game in the NCursesGameEngine
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-29
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef CBNCURSESGAMEENDINGE_GRANDPRIX_H
#define CBNCURSESGAMEENDINGE_GRANDPRIX_H

#include "../NCursesGameEngine.h"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <list>

class GrandPrix : public cb::NCursesGameEngine {

    bool OnUserCreate() override {

        vTrack.emplace_back( 0.0f, 10.0f ); // start/finish

        for( int i = 0; i < 8; i++ ) vTrack.emplace_back( 2.0f * ( (float) rand() / RAND_MAX ) - 1.0f, (float) ( rand() % 500 ) );

        for( auto &pTrack : vTrack ) fTrackDistance += pTrack.second;

        lLapTimes = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        if( IsFocused() ) {

            if( KeyUp(kVK_ANSI_Q ) ) return false;

            if( KeyPressed(kVK_UpArrow ) ) fSpeed += 2.0 * fElapsedTime;
            else fSpeed -= 1.0 * fElapsedTime;

            if( KeyPressed(kVK_LeftArrow) ) fPlayerCurvature -= 0.8f * fElapsedTime * ( 1.0f - fSpeed / 2.0f );

            if( KeyPressed(kVK_RightArrow) ) fPlayerCurvature += 0.8f * fElapsedTime * ( 1.0f - fSpeed / 2.0f );

            if( std::fabs(fTrackCurvature - fPlayerCurvature ) >= 0.8f ) fSpeed -= 5.0f * fElapsedTime;
        }

        if( fSpeed > 1.0f ) fSpeed = 1.0f;

        if( fSpeed < 0.0f ) fSpeed = 0.0f;

        fLapTime += fElapsedTime;

        fDistance += ( 100.0f * fSpeed ) * fElapsedTime;

        fTrackTraveled += ( 100.0f * fSpeed ) * fElapsedTime;

        if ( fTrackTraveled > vTrack.at( nTrack ).second  ) {

            fTrackTraveled -= vTrack.at( nTrack ).second;

            if( ++nTrack == vTrack.size() ) {
                nTrack = 0;

                fDistance = 0.0f;

                lLapTimes.push_front( fLapTime );

                lLapTimes.pop_back();

                fLapTime = 0.0f;
            }
        }

        fCurvature += ( ( vTrack.at( nTrack ).first ) - fCurvature ) * fElapsedTime * fSpeed;

        fTrackCurvature += fCurvature * fElapsedTime * fSpeed;

        for( int y = 0; y <= ScreenHeight() / 2; y++ ) {

            for( int x = 0; x < ScreenWidth(); x++ ) {

                float fPerspective = (float) y / ( ScreenHeight() / 2.0f );

                float fMiddlePoint = 0.5f + fCurvature * powf( 1.0f - fPerspective, 3 );

                float fRoadWidth = 0.1f + 0.8f * fPerspective;

                float fClipWidth = fRoadWidth * 0.15f;

                int nRow = ScreenHeight() / 2 + y;

                short sGrassColor = std::sin( ( ScreenHeight() / 10 ) * powf( 1.0f - fPerspective, 3 ) + 0.05f * fDistance ) > 0.0f ? FG_GREEN : FG_LIME;

                short sClipColor = std::sin( ( 3 * ScreenHeight() / 2 ) * powf( 1.0f - fPerspective, 2 ) + 0.1f * fDistance ) > 0.0f ? FG_RED : FG_WHITE;

                short sTrackColor = nTrack == 0 ? FG_WHITE : FG_GREY;

                fRoadWidth *= 0.5f;

                int nLeftGrass = ( fMiddlePoint - fRoadWidth - fClipWidth ) * ScreenWidth();
                int nLeftClip =  ( fMiddlePoint - fRoadWidth ) * ScreenWidth();
                int nRightGrass = ( fMiddlePoint + fRoadWidth + fClipWidth ) * ScreenWidth();
                int nRightClip = ( fMiddlePoint + fRoadWidth ) * ScreenWidth();

                if( x >= 0 && x < nLeftGrass ) DrawPixel( x, nRow, PIXEL_FULL, sGrassColor );
                else if( x >= nLeftGrass && x < nLeftClip ) DrawPixel( x, nRow, PIXEL_FULL, sClipColor );
                else if( x >= nLeftClip && x < nRightClip ) DrawPixel( x, nRow, PIXEL_FULL, sTrackColor );
                else if( x >= nRightClip && x < nRightGrass ) DrawPixel( x, nRow, PIXEL_FULL, sClipColor );
                else if( x >= nRightGrass && x < ScreenWidth() ) DrawPixel( x, nRow, PIXEL_FULL, sGrassColor );
            }

            for( int y = 0; y < ScreenHeight() / 2; y++ )
                for( int x = 0; x < ScreenWidth(); x++ )
                    DrawPixel( x, y , y < ScreenHeight() / 4 ? PIXEL_DARK : PIXEL_FULL, FG_BLUE );


            for( int x = 0; x < ScreenWidth(); x++ ) {

                int nHillHeight = (int) ( ( (float) ( ScreenHeight() / 3 ) ) * std::fabs(std::sin( x * 0.01f + fTrackCurvature ) ) );

                for( int y = ( ScreenHeight() / 2 ) - nHillHeight; y < ScreenHeight() / 2; y++ )
                    DrawPixel( x, y, PIXEL_FULL, FG_BROWN );
            }

            int nCarXPos = ( ScreenWidth() / 2 ) + ( ScreenWidth() * (fPlayerCurvature - fTrackCurvature ) / 2.0f ) - 8;

            int nCarYPos = ScreenHeight() - ( ScreenHeight() / 12 ) - 8;


            DrawAlphaString( nCarXPos, nCarYPos++, L"      ^      " );
            DrawAlphaString( nCarXPos, nCarYPos++, L"  ||-###-||  " );
            DrawAlphaString( nCarXPos, nCarYPos++, L"      #      " );
            DrawAlphaString( nCarXPos, nCarYPos++, L"||| ##### |||" );
            DrawAlphaString( nCarXPos, nCarYPos++, L"|||-#####-|||" );
            DrawAlphaString( nCarXPos, nCarYPos++, L"||| ##### |||" );
            DrawAlphaString( nCarXPos, nCarYPos++, L"     ###     " );
            DrawAlphaString( nCarXPos, nCarYPos,   L" ~~~~^^^~~~~");


            std::wstringstream ss;

            ss << "SPD: " << std::setw( 3 ) << (int) ( 203.0f * fSpeed ) << " MPH - "
               << "LAP: " << std::fixed << std::setprecision( 2 ) << fLapTime << " SEC - "
               << "DST: " << std::setw( 4 ) << (int) ( fTrackDistance - fDistance );

            DrawString( 1, 1, ss.str() );

            int i = 6;

            for( auto &l : lLapTimes ) {

                ss.str( std::wstring() );

                ss << "Lap " << std::setprecision( 4 ) << ( i - 5 ) << ": " << l;

                DrawString( 2, i++, ss.str() );
            }
        }

        int nMouseX, nMouseY;
        if( MousePressed( BUTTON1_CLICKED , nMouseX, nMouseY ) ) DrawString( nMouseX, nMouseY, L"Hello Mouse!!!" );

        return true;
    }

private:

    float fDistance = 0.0f;

    float fTrackDistance = 0.0f;

    int nTrack = 0;

    std::vector< std::pair< float, float > > vTrack; // curvature, distance

    float fTrackTraveled = 0.0f;

    float fCurvature = 0.0f;

    float fSpeed = 0.0f;

    float fLapTime = 0.0f;

    std::list< float > lLapTimes;

    float fTrackCurvature = 0.0f;

    float fPlayerCurvature = 0.0f;
};

#endif //CBNCURSESGAMEENDINGE_GRANDPRIX_H
