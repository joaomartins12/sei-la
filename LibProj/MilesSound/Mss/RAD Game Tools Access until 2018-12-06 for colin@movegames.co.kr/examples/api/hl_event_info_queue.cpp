//idoc(page, example_hl_event_info_queue)
//-----------------------------------------------------------------------------
//
// hl_event_info_queue.cpp - receiving information back from Miles.
//
// Copyright (c) 2014-2016, RAD Game Tools, Inc.
//
// Demonstrates how to receive information about events and voices back from
// Miles.
//
//-----------------------------------------------------------------------------
#include "miles.h"
#include "console_abstract.h"
#include <assert.h>

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
    // Enable the info queue for the driver. By enabling this, we are agreeing
    // to retrieve the information periodically, otherwise the backlog will 
    // continue to grow.
    //
    MilesEventInfoQueueEnable(Driver);

    //
    // Each event has a set of flags which determine what information
    // the event will send back through the info queue. Only pass what
    // you need - it is very possible in game of 100 active voices to get 
    // tons of data back from Miles and spend appreciable CPU time processing 
    // it - especially with MilesEventInfoRequestVoiceSource as it sends
    // the entire source name.
    //

    // Pass that we want event dispatch and voice completion info for the
    // next event, as well as the name of the source that was played.
    MilesQueueEventInfoMask(Queue, MilesEventInfoRequestEventInfo |
        MilesEventInfoRequestVoiceSource);
    U64 EventId = MilesQueueEventRun(Queue, "Startwelcome");
    EventId = EventId; // unused in release

    MilesQueueSubmit(Queue);

    for (;;)
    {
        MilesConsoleHandleEvents();

        U32 EventIsComplete = 0;
        U64 CurrentlyProcessingId = 0;
        MilesEventInfoHeader* Header = 0;
        while (MilesEventInfoQueueEnum(Driver, &Header))
        {
            switch (Header->Type)
            {
            case MilesEventInfoTypeEventDispatchStart:
                {
                    MilesEventInfoEventDispatchStart* Start = 
                        (MilesEventInfoEventDispatchStart*)Header;
                    U64 StartId = MilesAssemble64BitId(
                        Start->EventIdLow, Start->EventIdHigh);
                    console_printf("Now processing event: %llu\n", StartId);
                    CurrentlyProcessingId = StartId;
                    break;
                }
            case MilesEventInfoTypeEventDispatchEnd:
                {
                    // This type has no payload - as it just applies to
                    // previous start.
                    console_printf("Done processing event: %llu\n", 
                        CurrentlyProcessingId);
                    CurrentlyProcessingId = 0;
                    break;
                }
            case MilesEventInfoTypeVoiceSource:
                {
                    MilesEventInfoVoiceSource* Voice = 
                        (MilesEventInfoVoiceSource*)Header;
                    console_printf("Event %llu started source %s\n",
                        CurrentlyProcessingId, Voice->SourceName);
                    break;
                }
            case MilesEventInfoTypeEventVoicesComplete:
                {
                    // The voices complete message is sent when all voices
                    // started for an event completes.
                    MilesEventInfoEventVoicesComplete* Complete = 
                        (MilesEventInfoEventVoicesComplete*)Header;

                    U64 CompleteId = MilesAssemble64BitId(
                        Complete->EventIdLow, Complete->EventIdHigh);

                    console_printf("Voices for event %llu are complete",
                        CompleteId);
                    assert(CompleteId == EventId);
                    EventIsComplete = 1;
                    break;
                }
            }
        }

        if (EventIsComplete)
            break;

        MilesSleep(10);
    }

    // Destroy all Miles resources.
    MilesShutdown();

    return 1;
}

// @cdep pre $Defaults
// @cdep post $Build