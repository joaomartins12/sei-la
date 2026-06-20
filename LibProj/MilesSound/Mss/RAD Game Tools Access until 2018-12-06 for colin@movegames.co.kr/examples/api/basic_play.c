//idoc(page, example_basic_play)
//-----------------------------------------------------------------------------
//
// basic_play.c - the most basic of examples, showing how to play a wav file.
//
// Copyright (c) 2014, RAD Game Tools, Inc.
//
// At the simplest level, Miles works on samples and drivers. A driver
// contains which hardware to supply audio to, and samples contain what
// audio to supply to said hardware.
//
//-----------------------------------------------------------------------------

#include "miles.h"
#include <stdio.h>
#include "console_abstract.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void RADLINK MilesErrorFn(MilesReportType Type, char const* Message)
{
    printf("[%d] - %s\n", Type, Message);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CONSOLE_MAIN
{
    U32* WavData = 0;
    HMILESDRIVER Driver = 0;
    HMILESSAMPLE Sample = 0;
    S64 EndTimer = -1;

    MilesConsoleInit();
    
    //
    // Initialize core Miles system
    //
    MilesStartup(MilesErrorFn);

    // Create a default driver. It will supply audio to the default output 
    // for the platform.
    Driver = MilesDriverCreate(0);
    if (Driver == 0)
        return 0;

    //
    // Load the source file to play. MilesFileReadWithSize places the size 
    // in the first 4 bytes returned.
    //
    WavData = (U32*)MilesFileRead("../media/en_US/welcome.wav", 
        MilesFileReadWithSize);
    if (WavData == 0)
    {
        MilesDriverDestroy(Driver);
        return 0;
    }

    // Create a non 3d sample using the default output on the driver (0).
    Sample = MilesSampleCreate(Driver, 0, 0);
    if (Sample == 0)
    {
        MilesDriverDestroy(Driver);
        MilesFree(WavData);
        return 0;
    }

    // Set the sample's source data. Per MilesFileReadWithSize, the first 
    // 4 bytes are the size (WavData[0]), so the actual file data is at 
    // (WavData + 1).
    if (MilesSampleSetSource(Sample, WavData + 1, WavData[0], 
        MilesFileTypeAuto) == 0)
    {
        MilesDriverDestroy(Driver);
        MilesFree(WavData);
        return 0;
    }

    MilesSamplePlay(Sample);

    // Loop until the sample is done and the last bit of audio has had a 
    // chance to be delivered to the hw.
    for (;;)
    {
        if (EndTimer != -1 && MilesGetMsCount() > (U64)EndTimer)
            break;

        if (EndTimer == -1 && 
            MilesSampleGetStatus(Sample) == MilesSampleStatusDone)
        {
            // If we are done, set a timer so we quit after the output 
            // latency has been consumed.
            EndTimer = MilesGetMsCount() + 100;
        }

        MilesSleep(1);

        MilesConsoleHandleEvents();
    }

    MilesFree(WavData);

    // Free everything
    MilesShutdown();
    return 0;
}


// @cdep pre $Defaults
// @cdep post $Build