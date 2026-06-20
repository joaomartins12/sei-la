//idoc(page, example_basic_stream)
//-----------------------------------------------------------------------------
//
// basic_stream.cpp - basic streaming usage
//
// Copyright (c) 2014, RAD Game Tools, Inc.
//
// Streams are a type of source for a sample. Switch between a streamed and
// buffered source by just changing one function call.
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

    // Create a default driver.
    HMILESDRIVER Driver = MilesDriverCreate(0);
    if (Driver == 0)
    {
        return 0;
    }

    // Create a sample using the default output on the driver.
    HMILESSAMPLE Sample = MilesSampleCreate(Driver, 0, 0);
    if (Sample == 0)
    {
        MilesDriverDestroy(Driver);
        return 0;
    }

    // Set the sample's source to stream a file
    if (MilesSampleSetSourceStream(Sample, "../media/en_US/welcome.wav", 
        2048, 0, 0) == 0)
    {
        MilesDriverDestroy(Driver);
        return 0;
    }

    MilesSamplePlay(Sample);

    // Loop until the MilesSample is done and the last sample 
    // has had a chance to be delivered to the hw.
    S32 EndTimer = -1;
    for (;;)
    {
        if (EndTimer != -1 && MilesGetMsCount() > (U32)EndTimer)
            break;

        if (EndTimer == -1 && 
            MilesSampleGetStatus(Sample) == MilesSampleStatusDone)
        {
            // If we are done, set a timer so we quit after the 
            //output latency has been consumed.
            EndTimer = 100;
        }

        MilesSleep(1);

        MilesConsoleHandleEvents();
    }

    MilesSampleDestroy(Sample);

    // Only here for unit testing to allow the background to free the stream
    MilesSleep(100);
    
    MilesDriverDestroy(Driver);
    MilesShutdown();
    return 0;
}


// @cdep pre $Defaults
// @cdep post $Build