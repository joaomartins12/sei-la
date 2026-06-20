//idoc(page, example_basic_spatialize)
//-----------------------------------------------------------------------------
//
// basic_spatialize.cpp - basic 3d spatialization
//
// Copyright (c) 2014, RAD Game Tools, Inc.
//
// Samples can be given a position to allow Miles to automatically select
// speaker volume such that a 3d spatialized effect occurs.
//
//-----------------------------------------------------------------------------

#include "miles.h"
#include <stdio.h>
#include <math.h>
#include "console_abstract.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void RADLINK MilesErrorFn(MilesReportType Type, char const* Message)
{
    printf("[%d] - %s\n", Type, Message);
}

// Times and vectors for orienting the channel id wav file.
extern MilesVector Directions[];
extern U32 DirectionEndTimes[];

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CONSOLE_MAIN
{
    MilesConsoleInit();

    //
    // Initialize core Miles system
    //
    MilesStartup(MilesErrorFn);

    //
    // Load the source file to play.
    //
    U32* SourceFile = (U32*)MilesFileRead(PATH_TO_MEDIA"mono_channel_id_48k.binka",
        MilesFileReadWithSize);
    if (SourceFile == 0)
        return 0;

    // Create a default driver.
    HMILESDRIVER Driver = MilesDriverCreate(0);
    if (Driver == 0)
    {
        MilesFree(SourceFile);
        return 0;
    }

    // Allow us to decode BINKA files.
    MilesDriverRegisterBinkAudio(Driver);

    //
    // 3D spatialization is managed per "Route". All samples start
    // with a single route that goes from the sample to the destination
    // passed in MilesSampleCreate. The 3D parameter in that function
    // specifies whether that route should try to spatialize.
    //
    // Routes can only spatialize if the destination channel count
    // matches the channel count on the hardware output (i.e. the
    // number of speakers)
    //

    //
    // Create the 3D sample. By passing 0 as the output, we tell
    // miles to just use the hardware output (which means we'll
    // have the correct channel count as it *is* the output),
    // and by passing 1 to the PrimaryOutputIs3D parameter, we
    // tell miles we want the route to spatialize.
    //
    HMILESSAMPLE Sample = MilesSampleCreate(Driver, 0, 1);

    // Set the sample's source.
    if (MilesSampleSetSource(
        Sample, SourceFile + 1, SourceFile[0], MilesFileTypeAuto)
        == 0)
    {
        MilesDriverDestroy(Driver);
        MilesFree(SourceFile);
        return 0;
    }

    // Play the sample forever so we can hear it move around.
    MilesSamplePlay(Sample);
    MilesSampleSetLoopCount(Sample, 0);

    // Loop until the user presses a key.
    S64 EndTimer = -1;
    for (;;)
    {
        U64 CurrentTime = MilesGetMsCount();
        if (EndTimer != -1 && CurrentTime > (U64)EndTimer)
            break;

        //
        // Based on where the sample is, orient the sample so it lines
        // up with the channel identification in the wav file.
        //
        U32 CurPos = MilesSampleGetPositionMs(Sample);
        U32 TimerOffset = CurPos % DirectionEndTimes[4];
        U32 Index = 0;
        while (Index < 5 && DirectionEndTimes[Index] < TimerOffset)
            Index++;

        MilesSampleSet3DPosition(
            Sample,
            Directions[Index].x,
            Directions[Index].y,
            Directions[Index].z
            );

#if defined(__RADNT__ )
        if (_kbhit() && _getch() == ' ')
        {
            // Stop the sample and start a brief exit timer
            MilesSamplePauseFade(Sample);
            EndTimer = MilesGetMsCount() + 50;
        }
#endif
        MilesSleep(5);
        MilesConsoleHandleEvents();
    }

    MilesFree(SourceFile);
    MilesShutdown();
    return 0;
}


MilesVector Directions[] = {
    {-1, 1, 0}, // front left
    {1, 1, 0},  // front right
    {0, 1, 0},  // center
    {-1, -1, 0},// back left
    {1, -1, 0}  // back right
};
U32 DirectionEndTimes[] = {
    985,
    1975,
    2809,
    3784,
    4857
};

// @cdep pre $Defaults
// @cdep post $Build