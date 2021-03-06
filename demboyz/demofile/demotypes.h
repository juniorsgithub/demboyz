//====== Copyright (c) 2014, Valve Corporation, All rights reserved. ========//
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// Redistributions in binary form must reproduce the above copyright notice, 
// this list of conditions and the following disclaimer in the documentation 
// and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
// THE POSSIBILITY OF SUCH DAMAGE.
//===========================================================================//

#pragma once

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "sourcesdk/vector.h"

#define DEMO_HEADER_ID      "HL2DEMO"
#define DEMO_PROTOCOL       3

#if !defined( MAX_OSPATH )
#define MAX_OSPATH      260         // max length of a filesystem pathname
#endif

// Demo messages
enum
{
    dem_unknown = 0,
    // it's a startup message, process as fast as possible
    dem_signon,
    // it's a normal network packet that we stored off
    dem_packet,
    // sync client clock to demo tick
    dem_synctick,
    // console command
    dem_consolecmd,
    // user input command
    dem_usercmd,
    // network data tables
    dem_datatables,
    // end of time.
    dem_stop,

    dem_stringtables,

    // Last command
    dem_lastcmd     = dem_stringtables
};

inline const char* DemoCmdToString(unsigned int cmd)
{
    static const char* cmdNames[] = 
    {
        "dem_unknown",
        "dem_signon",
        "dem_packet",
        "dem_synctick",
        "dem_consolecmd",
        "dem_usercmd",
        "dem_datatables",
        "dem_stop",
        "dem_stringtables"
    };
    if (cmd >= (sizeof(cmdNames) / sizeof(const char*)))
    {
        return "";
    }
    return cmdNames[cmd];
}

struct demoheader_t
{
    char    demofilestamp[ 8 ];             // Should be HL2DEMO
    int32_t   demoprotocol;                 // Should be DEMO_PROTOCOL
    int32_t   networkprotocol;              // Should be PROTOCOL_VERSION
    char    servername[ MAX_OSPATH ];       // Name of server
    char    clientname[ MAX_OSPATH ];       // Name of client who recorded the game
    char    mapname[ MAX_OSPATH ];          // Name of map
    char    gamedirectory[ MAX_OSPATH ];    // Name of game directory (com_gamedir)
    float   playback_time;                  // Time of track
    int32_t   playback_ticks;               // # of ticks in track
    int32_t   playback_frames;              // # of frames in track
    int32_t   signonlength;                 // length of sigondata in bytes
};

#define FDEMO_NORMAL        0
#define FDEMO_USE_ORIGIN2   ( 1 << 0 )
#define FDEMO_USE_ANGLES2   ( 1 << 1 )
#define FDEMO_NOINTERP      ( 1 << 2 )  // don't interpolate between this an last view

#define MAX_SPLITSCREEN_CLIENTS 1

struct democmdinfo_t
{
    democmdinfo_t( void )
    {
    }

    struct Split_t
    {
        Split_t( void )
        {
            flags = FDEMO_NORMAL;
            viewOrigin.Init();
            viewAngles.Init();
            localViewAngles.Init();

            // Resampled origin/angles
            viewOrigin2.Init();
            viewAngles2.Init();
            localViewAngles2.Init();
        }

        Split_t&    operator=( const Split_t& src )
        {
            if ( this == &src )
                return *this;

            flags = src.flags;
            viewOrigin = src.viewOrigin;
            viewAngles = src.viewAngles;
            localViewAngles = src.localViewAngles;
            viewOrigin2 = src.viewOrigin2;
            viewAngles2 = src.viewAngles2;
            localViewAngles2 = src.localViewAngles2;

            return *this;
        }

        const Vector& GetViewOrigin( void )
        {
            if ( flags & FDEMO_USE_ORIGIN2 )
            {
                return viewOrigin2;
            }
            return viewOrigin;
        }

        const QAngle& GetViewAngles( void )
        {
            if ( flags & FDEMO_USE_ANGLES2 )
            {
                return viewAngles2;
            }
            return viewAngles;
        }
        const QAngle& GetLocalViewAngles( void )
        {
            if ( flags & FDEMO_USE_ANGLES2 )
            {
                return localViewAngles2;
            }
            return localViewAngles;
        }

        void Reset( void )
        {
            flags = 0;
            viewOrigin2 = viewOrigin;
            viewAngles2 = viewAngles;
            localViewAngles2 = localViewAngles;
        }

        int32_t     flags;

        // original origin/viewangles
        Vector      viewOrigin;
        QAngle      viewAngles;
        QAngle      localViewAngles;

        // Resampled origin/viewangles
        Vector      viewOrigin2;
        QAngle      viewAngles2;
        QAngle      localViewAngles2;
    };

    void Reset( void )
    {
        for ( int i = 0; i < MAX_SPLITSCREEN_CLIENTS; ++i )
        {
            u[ i ].Reset();
        }
    }

    Split_t         u[ MAX_SPLITSCREEN_CLIENTS ];
};
