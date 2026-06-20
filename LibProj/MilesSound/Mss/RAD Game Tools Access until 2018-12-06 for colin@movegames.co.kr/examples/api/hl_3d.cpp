//idoc(page, example_hl_3d)
//-----------------------------------------------------------------------------
//
// hl_3d.cpp - setting listener and event 3d properties.
//
// Copyright (c) 2014-2016, RAD Game Tools, Inc.
//
// Demonstrates how to set up a 3d soundscape with the high level API.
//
//-----------------------------------------------------------------------------
#include "miles.h"
#include "console_abstract.h"
#include <math.h>
#ifndef M_PI_2
    #define M_PI_2 (3.1415926535f / 2.0f)
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void RADLINK MilesErrorFn(MilesReportType Type, char const* Message)
{
    console_printf("[%d] - %s\n", Type, Message);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CONSOLE_MAIN
{
    // See hl_basic_events.cpp for initialization details
    MilesConsoleInit();
    MilesStartup(MilesErrorFn);
    MilesDriver* Driver = MilesDriverCreate(0);
    MilesDriverRegisterBinkAudio(Driver);
    MilesProjectLoad(Driver, PATH_TO_MEDIA"api_project.mprj", 0, 0);
    MilesBank* Bank = MilesBankLoad(Driver, PATH_TO_MEDIA"api_bank.mbnk", 
        PATH_TO_MEDIA"api_bank_stream.mstr", 0, 0);
    U32 BankLoadResult = MilesBankWaitForLoad(Bank);
    if (BankLoadResult != MilesBankStatusLoaded)
    {
        MilesShutdown();
        return 0;        
    }

    HMILESQUEUE Queue = MilesQueueCreate(Driver);

    //
    // Up to 32 possible listeners are possible, with each voice spatializing
    // to any number of them. This will focus on a single listener (index 0).
    //

    // Set the listener position and orientation. Set the listener (index 0) 
    // a bit back, facing the origin.
    MilesQueueListener3DPosition(Queue, 0, 0, -1, 0);
    MilesQueueListener3DOrientation(Queue, 0, cosf(M_PI_2), sinf(M_PI_2),
        0, 0, 0, 1);

    // Set the 3d position of the next event. We want to sweep it from right
    // to left.
    MilesQueueEvent3DPosition(Queue, 1, 1, 0);

    // Also set the 3d orientation. This is used for angle computation in
    // controllers of the Angle type. We make the event face off to the side
    // to demonstrate angles. It has no effect on the 3d pan or the distance
    // falloff.
    MilesQueueEvent3DOrientation(Queue, cosf(2*M_PI_2), sinf(2*M_PI_2), 0,
        0, 0, 1);

    // Set the doppler factor for the voice.
    float EventVelocity[3] = {-1, 0, 0};
    MilesQueueEventDopplerFactor(Queue, 1.0f, 0, EventVelocity, 1);

    // Start an event that has voices with 3d sends.
    U64 EventId = MilesQueueEventRun(Queue, "Startwelcome3D");

    MilesQueueSubmit(Queue);

    //
    // Once the event is running, update its position as it flies by.
    //
    U64 StartTime = MilesGetMsCount();
    for (;;)
    {
        MilesConsoleHandleEvents();
        MilesSleep(10);

        // Move the event over 3 seconds, then exit.
        const F32 TimeToPlayMs = 3000.0f; 
        U64 CurrentTime = MilesGetMsCount();
        float CompletionPct = (float)(CurrentTime - StartTime) / TimeToPlayMs;
        if (CompletionPct > 1)
            break;

        // Move from 1 to -1
        MilesQueueUpdateEvent3DPosition(Queue, EventId, 
            1 - 2*CompletionPct, 1, 0);
        MilesQueueSubmit(Queue);
    }

    // Destroy all Miles resources.
    MilesShutdown();

    return 1;
}

// @cdep pre $Defaults
// @cdep post $Build