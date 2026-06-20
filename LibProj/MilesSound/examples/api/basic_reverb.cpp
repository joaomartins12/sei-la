//idoc(page, example_basic_reverb)
//-----------------------------------------------------------------------------
//
// basic_reverb.cpp - basic filter usage
//
// Copyright (c) 2014, RAD Game Tools, Inc.
//
// Filters can be installed directly on to a sample, or on to a bus. This
// example instantiates a reverb filter on to a sample.
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
    MilesConsoleInit();
    
    //
    // Initialize core Miles system
    //
    MilesStartup(MilesErrorFn);

    //
    // Load the source file to play.
    //
    U32* SourceFile = (U32*)MilesFileRead("../media/en_US/welcome.wav", 
        MilesFileReadWithSize);
    if (SourceFile == 0)
    {
        return 0;
    }

    // Create a default driver.
    HMILESDRIVER Driver = MilesDriverCreate(0);
    if (Driver == 0)
    {
        MilesFree(SourceFile);
        return 0;
    }

    // Create a sample using the default output on the driver.
    HMILESSAMPLE Sample = MilesSampleCreate(Driver, 0, 0);
    if (Sample == 0)
    {
        MilesDriverDestroy(Driver);
        MilesFree(SourceFile);
        return 0;
    }

    // Set the sample's source.
    if (MilesSampleSetSource(Sample, SourceFile + 1, SourceFile[0], 
        MilesFileTypeAuto) == 0)
    {
        MilesDriverDestroy(Driver);
        MilesFree(SourceFile);
        return 0;
    }

    // Add a reverb filter to the sample.
    HMILESFILTERSTAGE FilterStage = MilesSampleAddFilterByName(Sample, 
        MilesFilterNameReverb);
    if (FilterStage == 0)
    {
        MilesDriverDestroy(Driver);
        MilesFree(SourceFile);
        return 0;
    }

    MilesSamplePlay(Sample);

    // Loop until the MilesSample is done and the last sample has 
    // had a chance to be delivered to the hw.
    S32 EndTimer = -1;
    for (;;)
    {
        if (EndTimer != -1 && MilesGetMsCount() > (U32)EndTimer)
            break;

        if (EndTimer == -1 && 
            MilesSampleGetStatus(Sample) == MilesSampleStatusDone)
        {
            // If we are done, set a timer so we quit after the 
            // output latency has been consumed.
            EndTimer = 100;
        }

        MilesSleep(1);
        MilesConsoleHandleEvents();
    }

    MilesFree(SourceFile);
    MilesShutdown();
    return 0;
}


// @cdep pre $Defaults
// @cdep post $Build