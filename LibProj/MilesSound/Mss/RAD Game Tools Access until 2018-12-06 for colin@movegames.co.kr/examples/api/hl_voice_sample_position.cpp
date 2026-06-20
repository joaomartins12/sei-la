//idoc(page, example_hl_voice_sample_position)
//-----------------------------------------------------------------------------
//
// hl_voice_sample_position.cpp - getting a voice's current position in time.
//
// Copyright (c) 2014-2016, RAD Game Tools, Inc.
//
// Demonstrates how to create a postbox to get the current voice time.
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
    // Since the current time position of a voice changes constantly (and
    // the game only needs the latest), Miles uses an object that just stores
    // the latest and allows the game to retrieve the latest value. Values
    // sent from the runtime between the last and current retrieval are
    // lost.
    //
    HMILESPOSTBOX Postbox = MilesPostboxCreate(Driver);

    // As with most event modifiers, the postbox is added to the queue before
    // the next event is run. This will post the current time of the first
    // voice started by the event.
    MilesQueueEventPositionPostbox(Queue, Postbox);

    // This event has a dialogue volume (\todo use default cont values!)
    MilesQueueControllerValue(Queue, "DialogueVolume", 0);
    MilesQueueEventRun(Queue, "Startwelcome");
    MilesQueueSubmit(Queue);

    //
    // Once the event is running, retrieve the postbox value periodically
    //
    U64 StartTime = MilesGetMsCount();
    for (;;)
    {
        MilesConsoleHandleEvents();
        MilesSleep(10);

        console_printf("\rVoice At: %.0f ms", MilesPostboxGetValue(Postbox));

        if (MilesGetMsCount() - StartTime > 4000)
            break;
    }

    console_printf("\n");

    // Destroy all Miles resources.
    MilesShutdown();
    return 1;
}

// @cdep pre $Defaults
// @cdep post $Build