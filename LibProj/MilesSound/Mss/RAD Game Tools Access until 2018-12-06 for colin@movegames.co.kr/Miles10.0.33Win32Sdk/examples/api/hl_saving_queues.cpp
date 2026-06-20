//idoc(page, example_hl_saving_queues)
//-----------------------------------------------------------------------------
//
// hl_saving_queues.cpp - Saving queue dumps for bug repros.
//
// Copyright (c) 2014-2016, RAD Game Tools, Inc.
//
// Demonstrates how to save all queues sent to Miles for reproduction. Saved
// queues can be sent to support with a render of the project for quick and
// accurate issue reproduction.
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

static FILE* QueueOutputFile = 0;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void RADLINK MilesQueueSaveFn(void* Bytes, U32 ByteCount)
{
#ifdef __RADNT__
    // Queues can be saved on every platform, but the game will have to be
    // responsible for saving them to an accessible place, using hostio, the
    // dev hard drive, or whatever the platform provides. This example just 
    // assumes Windows.

    if (QueueOutputFile == (FILE*)-1)
        return; // we had an error opening the file.

    if (QueueOutputFile == 0)
    {
        QueueOutputFile = fopen("hl_saving_queues.miles_repro_queue", "wb");
        if (QueueOutputFile == 0)
        {
            QueueOutputFile = (FILE*)-1;
            return;
        }
    }

    fwrite(Bytes, ByteCount, 1, QueueOutputFile);

#endif // __RADNT__
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

    //
    // When bizarre issues come up, often support requests can be sped up
    // by providing the queue dump to Miles support. These dumps contain
    // all of the queues that Miles has processed, in the order that they
    // were processed in. When combined with a render of the project, the
    // entirety of the game's audio can be replayed, often demonstrating
    // bugs in either the content or Miles itself in a reproducible way.
    //

    //
    // Note that the function is called from the thread that processes it.
    //
    MilesProjectSetQueueSaveFn(Driver, MilesQueueSaveFn);

    HMILESQUEUE Queue = MilesQueueCreate(Driver);

    // When the queue is processed by the mixer thread, the save function
    // will be called with all the information carried in the queue.
    MilesQueueEventRun(Queue, "Startwelcome");
    MilesQueueSubmit(Queue);

    // Let the voice complete.
    U64 StartTime = MilesGetMsCount();
    for (;;)
    {
        MilesConsoleHandleEvents();
        MilesSleep(10);
        if (MilesGetMsCount() - StartTime > 3000)
            break;
    }

    // Close the file containing the repro queue.
    if (QueueOutputFile && QueueOutputFile != (FILE*)-1)
        fclose(QueueOutputFile);

    // Destroy all Miles resources.
    MilesShutdown();
    return 1;
}

// @cdep pre $Defaults
// @cdep post $Build