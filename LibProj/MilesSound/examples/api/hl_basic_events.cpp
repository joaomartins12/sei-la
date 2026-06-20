//idoc(page, example_hl_basic_events)
//-----------------------------------------------------------------------------
//
// hl_basic_events.cpp - basic high level system startup and usage
//
// Copyright (c) 2014-2016, RAD Game Tools, Inc.
//
// Miles 10 features a tighter integration and focus on the high level systems.
// This demonstrates the code necessary to start up and play events authored
// by the Miles Sound Studio.
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
    // Helper function for cross-platform examples.
    MilesConsoleInit();

    // Initializes the Miles runtime, creating global management structures.
    // The function passed in is the Report function, used for passing
    // information back to the development team about usage or content errors.
    MilesStartup(MilesErrorFn);

    // Create the driver and register the bink audio decoder. By default
    // Miles Studio compresses to the Bink Audio format. If other codes are
    // used by your sound designers, the appropriate decoder will need to
    // be registered prior to project load.
    MilesDriver* Driver = MilesDriverCreate(0);
    MilesDriverRegisterBinkAudio(Driver);

    //
    // Miles High Level works with Projects and Banks.
    //
    // Projects contain global information about your game such as the 
    // buses and controllers.
    //
    // Banks contain content like events and audio.
    //
    // By default, Projects and Banks are loaded asynchronously. It is safe
    // to start a load on a bank before the project load is complete:
    //
    MilesProjectLoad(Driver, PATH_TO_MEDIA"api_project.mprj", 0, 0);
    MilesBank* Bank = MilesBankLoad(Driver, PATH_TO_MEDIA"api_bank.mbnk", 
        PATH_TO_MEDIA"api_bank_stream.mstr", 0, 0);

    //
    // The loads above cause Miles to issue reads on the async thread,
    // which upon completion, will (on the mixer thread) create the 
    // low level state required to represent the project and run events
    // from the supplied bank. 
    //
    // No Miles "Low Level" assets need to be manually created for 
    // event use (buses, samples, filters)
    //

    //
    // Since this example has no other work to do, we just spinloop
    // until the loads complete.
    //
    U32 BankLoadResult = MilesBankWaitForLoad(Bank);
    if (BankLoadResult != MilesBankStatusLoaded)
    {
        // Error already reported via the report function.
        MilesShutdown();
        return 0;        
    }

    //
    // The Miles High Level API focuses on never taking a long term lock
    // during gameplay. As a result, most actions are placed in a queue
    // that is built on the calling thread, and submitted at once in a rapid
    // handoff.
    //

    // Create a queue for submitting events.
    HMILESQUEUE queue = MilesQueueCreate(Driver);

    // Queue an event to run.
    // Events are referenced by name. A fully qualified name looks a lot
    // like a filename - it starts with a slash, has the bank name, and
    // then the folders, concluding with the event name. In this example,
    // /api_bank/Startwelcome could also be used. Additional information
    // in the help file under Core Concepts/EventNaming.
    MilesQueueEventRun(queue, "Startwelcome");

    // Submit the queue for processing. At this point the queue is handed
    // off to the mixer thread for processing. All events will be run during
    // the next audio mix.
    MilesQueueSubmit(queue);

    // Helper function for cross-platform examples.
    MilesConsoleHandleEvents();

    // Wait for the sound to play and stop.
    MilesSleep(2400);

    // Destroy all Miles resources.
    MilesShutdown();

    return 1;
}

// @cdep pre $Defaults
// @cdep post $Build