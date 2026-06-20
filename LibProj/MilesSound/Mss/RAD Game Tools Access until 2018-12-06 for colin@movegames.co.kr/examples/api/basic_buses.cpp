//idoc(page, example_basic_buses)
//-----------------------------------------------------------------------------
//
// basic_buses.cpp - basic bus usage
//
// Copyright (c) 2014, RAD Game Tools, Inc.
//
// Instead of routing directly to the hardware, samples can route through
// buses which allow aggregate volume and effect control.
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
    U32* SourceFile = (U32*)MilesFileRead(PATH_TO_MEDIA"en_US/welcome.wav", 
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

    //
    // Create the bus tree.
    //
    // Sample output -> Bus2 + Bus1 -> Bus3 -> HW Out
    //
    // This example duplicates the audio of a sample across two buses. 
    // That bus then accumulates in to a single bus, which then goes 
    // to the hardware.
    //

    // Bus3 should be created first, since the other buses route to it.
    HMILESBUS Bus3 = MilesBusCreate(Driver, 0, 0, 1.0f);

    // Create the two mono buses that will route in to Bus3.
    HMILESBUS Bus1 = MilesBusCreate(Driver, 1, Bus3, 1.0f);
    HMILESBUS Bus2 = MilesBusCreate(Driver, 1, Bus3, 1.0f);

    // Set each bus to a different ear.
    MilesBusSetPanLevels(Bus1, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    MilesBusSetPanLevels(Bus2, 1.0f, 0.0f, 0.0, 1.0f, 0.0f);

    // Create a sample using the first bus as our output.
    HMILESSAMPLE Sample = MilesSampleCreate(Driver, Bus1, 0);

    // Create an additional output from the sample to the other bus.
    MilesSampleCreateRoute(Sample, Bus2, 1.0f, 0);

    // Set the sample's source.
    if (MilesSampleSetSource(Sample, SourceFile + 1, SourceFile[0], 
        MilesFileTypeAuto) == 0)
    {
        MilesDriverDestroy(Driver);
        MilesFree(SourceFile);
        return 0;
    }

    // Play.
    MilesSamplePlay(Sample);

    // Loop until the MilesSample is done and the last sample has had 
    // a chance to be delivered to the hw.
    S64 EndTimer = -1;
    for (;;)
    {
        if (EndTimer != -1 && MilesGetMsCount() > (U64)EndTimer)
            break;

        // While we play, adjust the bus levels to show each ear is 
        // a different bus.
        U64 Timer = MilesGetMsCount();
        Timer %= 3000;

        F32 LeftEar = Timer / 3000.0f;
        F32 RightEar = 1 - LeftEar;

        LeftEar = MilesLinearToLevel(LeftEar);
        RightEar = MilesLinearToLevel(RightEar);

        MilesBusSetPanLevels(Bus1, 1.0f, 0.0f, LeftEar, 0.0f, 0.0f);
        MilesBusSetPanLevels(Bus2, 1.0f, 0.0f, 0.0f, RightEar, 0.0f);

        if (EndTimer == -1 && 
            MilesSampleGetStatus(Sample) == MilesSampleStatusDone)
        {
            EndTimer = MilesGetMsCount() + 100;
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