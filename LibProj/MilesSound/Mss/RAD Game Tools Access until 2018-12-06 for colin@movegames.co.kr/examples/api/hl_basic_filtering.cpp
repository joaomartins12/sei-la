//idoc(page, example_hl_basic_filtering)
//-----------------------------------------------------------------------------
//
// hl_basic_filtering.cpp - applying events (stop/pause) to specific events
//
// Copyright (c) 2014-2016, RAD Game Tools, Inc.
//
// Demonstrates how to target specific sounds or groups of sounds for stopping,
// pausing, and resuming.
//
//-----------------------------------------------------------------------------
#include "miles.h"
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
    // See hl_basic_events.cpp for details on initialization.
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
        // Error already reported.
        MilesShutdown();
        return 0;        
    }

    HMILESQUEUE Queue = MilesQueueCreate(Driver);

    MilesConsoleHandleEvents();

    console_printf("Stopping sounds 4 different ways:\n");

    //
    // In order to stop voices, Miles needs a way to know what voice
    // from all the running voices the game wants to stop. There are
    // several ways.
    // The voice can be targeted:
    // -- by the event ID that started it.
    //      (MilesQueueEventFilterId)
    // -- by the name of the event that started id.
    //      (MilesQueueEventFilterName)
    // -- by the event TemplateID that started it.
    //      (MilesQueueEventFilterTemplateId)
    // -- by the _exact_ position of the voice.
    //      (MilesQueueEventFilter3DPosition)
    //
    // Each of these can be combined. If so, the voice has to match 
    // all of the filters.
    //
    //
    // Setting the filter is done by calling the relevant queue function
    // PRIOR to the MilesQueueEventRun. Many event state functions operate
    // this way, setting up how the _next_ event will sound.
    //

    // Once the filter has been established, stopping voices is
    // done with the intrinsic event names "STOPNOW" and "STOP".
    //
    // STOPNOW forces the voice to ignore its fade out, and stop
    //      immediatel.
    // STOP stops the voice, allowing the fade to occur (which
    //      can be arbitrarily long).
    //
    // Using _no_ filter will stop _all_ sounds.
    //

    // By ID:
    {
        U64 EventId = MilesQueueEventRun(Queue, "Startwelcome");
        MilesQueueSubmit(Queue);

        MilesSleep(1000);

        console_printf("Stopping ID %llu\n", EventId);        
        MilesQueueEventFilterId(Queue, EventId);
        MilesQueueEventRun(Queue, "STOPNOW"); 
        MilesQueueSubmit(Queue);

        // The sound will stop when the queue is evaluated (next
        // mix)
    }

    // By Name:
    {
        MilesQueueEventRun(Queue, "Startwelcome");
        MilesQueueSubmit(Queue);

        MilesSleep(1000);

        // Stop the sound using its name. CAUTION - the event name used must
        // be a complete name - the relative name can't be used. This means the
        // name must contain the bank and the full path.
        console_printf("Stopping Name %s\n", "/api_bank/Startwelcome");
        MilesQueueEventFilterName(Queue, "/api_bank/Startwelcome");
        MilesQueueEventRun(Queue, "STOPNOW");
        MilesQueueSubmit(Queue);
    }

    // By TemplateId:
    {
        MilesQueueEventRun(Queue, "Startwelcome");
        MilesQueueSubmit(Queue);

        MilesSleep(1000);

        // Get the TemplateId from the name.
        MilesEventTemplateId TemplateId = {};
        MilesEventResolveName(Driver, "Startwelcome", &TemplateId);

        console_printf("Stopping Template %08x:%08x\n", TemplateId.BankId, 
            TemplateId.Index);
        //MilesQueueEventFilterTemplateId(Queue, &TemplateId);
        MilesQueueEventRun(Queue, "STOPNOW");
        MilesQueueSubmit(Queue);
    }

    // By Position:
    {
        MilesQueueEvent3DPosition(Queue, 10, 5, 1);
        MilesQueueEventRun(Queue, "Startwelcome");
        MilesQueueSubmit(Queue);

        MilesSleep(1000);

        // Stop the sound using its position. This must be 
        // bitwise exact.
        console_printf("Stopping Position %.0f, %.0f, %.0f\n", 
            10.0f, 5.0f, 1.0f);
        MilesQueueEventFilter3DPosition(Queue, 10, 5, 1);
        MilesQueueEventRun(Queue, "STOPNOW");
        MilesQueueSubmit(Queue);
    }

    //
    // The same filters can be applied to pausing sounds using
    // the PAUSE event:
    //
    // Voices can be affected by multiple PAUSEs, and will not resume
    // until cleared of all of them.
    U64 IdOfPause;
    U64 IdOfEvent;
    {
        IdOfEvent = MilesQueueEventRun(Queue, "Startwelcome");
        MilesQueueSubmit(Queue);

        MilesSleep(800);

        console_printf("Pausing name %s:\n", "/api_bank/Startwelcome");
        MilesQueueEventFilterName(Queue, "/api_bank/Startwelcome");
        IdOfPause = MilesQueueEventRun(Queue, "PAUSE");
        MilesQueueSubmit(Queue);
    }

    //
    // However, RESUME only targets IDs - usually the ID of the pause.
    //
    // PAUSE associates all of the paused voices with the ID of the 
    // PAUSE event (IdOfPause above). Miles does not support arbitrary
    // resuming of a filter, as even when passing the exact same filter,
    // additional voices could have started that match.
    //
    {
        MilesSleep(800);

        console_printf("Resuming all voices paused by %llu\n", IdOfPause);
        MilesQueueEventFilterId(Queue, IdOfPause);
        MilesQueueEventRun(Queue, "RESUME");
        MilesQueueSubmit(Queue);

        // ALL voices paused by the event IdOfPause are resumed - 
        // assuming they are not paused by other events as well!
    }

    // RESUME can also target the event itself, but ONLY if the pause
    // _only targeted a single event_ using the MilesQueueEventFilterId.
    {
        MilesSleep(800);

        // Pause a _specific_ id.
        console_printf("Pausing ID: %llu\n", IdOfEvent);
        MilesQueueEventFilterId(Queue, IdOfEvent);        
        MilesQueueEventRun(Queue, "PAUSE");
        MilesQueueSubmit(Queue);

        MilesSleep(800);

        // Since the pause was filtered to a single event, we
        // can reference the pause by that event id as well as
        // the pause id. Both work.
        console_printf("Resuming ID: %llu\n", IdOfEvent);
        MilesQueueEventFilterId(Queue, IdOfEvent);
        MilesQueueEventRun(Queue, "RESUME");
        MilesQueueSubmit(Queue);

        MilesSleep(1000);
    }

    MilesSleep(1000);
    MilesShutdown();
    return 1;
}

// @cdep pre $Defaults
// @cdep post $Build