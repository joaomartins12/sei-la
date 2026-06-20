//idoc(page, example_hl_controllers)
//-----------------------------------------------------------------------------
//
// hl_controllers.cpp - settings global and local controllers
//
// Copyright (c) 2014-2016, RAD Game Tools, Inc.
//
// Demonstrates how to set the controller values that designers use in
// Miles Studio to create events that respond to game state.
//
//-----------------------------------------------------------------------------
#include "miles.h"
#include "console_abstract.h"

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
    // Controllers are the primary means of making an event change itself
    // in response to game state.
    //
    // Controllers can be either Global or Local. A local controller has
    // a separate value for each event - for example, the computed occlusion
    // for the sound. A global controller has a single value shared across 
    // all events - for example, the user specified FX volume.
    //

    // Set a global controller value.
    MilesQueueControllerValue(Queue, "DialogueVolume", -10);

    //
    // To set up the local controller value, it MUST first be set once before
    // the event is queued.
    //
    // This function will save the controllers value and apply it
    // to the next event.
    //
    MilesQueueEventControllerValue(Queue, "ReverbOnOff", 1);

    // Start an event that happens to utilize both of these controllers
    // (the DialogueVolume controller affects a bus the voice passes through,
    // and the ReverbOnOff affects a send level on the voice itself).
    U64 EventId = MilesQueueEventRun(Queue, "Startogres");

    MilesQueueSubmit(Queue);

    //
    // Once the event is running, the controllers can continuously be updated
    // to keep the event responding over time.
    //
    // Global controllers use the same function, whereas local controllers
    // need to know what event to affect.
    //
    U64 StartTime = MilesGetMsCount();
    for (;;)
    {
        MilesConsoleHandleEvents();
        MilesSleep(10);

        // Fade the controller values over 5 seconds, then exit.
        const F32 TimeToPlayMs = 5000.0f; 

        U64 CurrentTime = MilesGetMsCount();
        float CompletionPct = (float)(CurrentTime - StartTime) / TimeToPlayMs;
        if (CompletionPct > 1)
            break;

        // Fade the dialogue volume from -10 dB to 0 dB over the time.
        MilesQueueControllerValue(Queue, "DialogueVolume", 
            -10 + CompletionPct*10);

        // Fade the reverb onoff to off over the time. To update an event
        // that has already started, the event's ID must be passed:
        MilesQueueUpdateEventControllerValue(Queue, EventId, 
            "ReverbOnOff", 1 - CompletionPct);

        // Commit all the changes at once.
        MilesQueueSubmit(Queue);
    }

    // Destroy all Miles resources.
    MilesShutdown();

    return 1;
}

// @cdep pre $Defaults
// @cdep post $Build