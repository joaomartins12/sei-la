//idoc(page, example_hl_active_event_list)
//-----------------------------------------------------------------------------
//
// hl_active_event_list.cpp - tracking events
//
// Copyright (c) 2014-2016, RAD Game Tools, Inc.
//
// Demonstrates how to keep track of running events in Miles.
//
//-----------------------------------------------------------------------------
#include "miles.h"
#include "console_abstract.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void RADLINK MilesErrorFn(MilesReportType Type, char const* Message)
{
    console_printf("[%d] - %s\n", Type, Message);
}

struct MilesActiveEvent
{
    U64 EventId;
    char const* EventName;
    F32 X, Y;
    MilesActiveEvent* Next;
};

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

    // We must use the event queue to track events (hl_event_info_queue.cpp)
    MilesEventInfoQueueEnable(Driver);

    // We play a lot of events so tone the volume down a bit
    MilesDriverSetMasterVolume(Driver, MilesDbToLevel(-20));

    // Track the events in a linked list.
    MilesActiveEvent* ListOfActiveEvents = 0;
    U32 ActiveEventCount = 0;

    //
    // Any event that we want to track must have the RequestEventInfo
    // flag passed. In general, this should be done for every event that
    // isn't a code event (i.e. STOP STOPNOW PAUSE RESUME). While
    // nothing goes wrong if passed to a code event, it will increase
    // data traffic back through the event info queue, which can 
    // affect performance under high event loads (hundreds per frame).
    //

    U64 EndTick = MilesGetMsCount() + 10000; // only run for 10 seconds.
    U64 NextQueueTick = 0;
    U64 LastPrintTick = 0;
    for (;;)
    {
        MilesConsoleHandleEvents();

        U64 CurrentTick = MilesGetMsCount();
        if (NextQueueTick < CurrentTick)
        {
            // Set the timer for the next event in 100-200 ms
            NextQueueTick = CurrentTick + 100 + rand()%100;

            // Mark that we want to track the next event.
            MilesQueueEventInfoMask(Queue, MilesEventInfoRequestEventInfo);

            // Queue an event at a random position around the listener.
            float RandomTheta = .002f*3.14159f*(rand()%100);
            float RandomDistance = (float)(rand()%100);
            float X = RandomDistance*cosf(RandomTheta);
            float Y = RandomDistance*sinf(RandomTheta);
            MilesQueueEvent3DPosition(Queue, X, Y, 0);
            U64 EventId = MilesQueueEventRun(Queue, "Startwelcome3D");
            MilesQueueSubmit(Queue);

            // Create management structure and add to list.
            MilesActiveEvent* NewEvent = 
                (MilesActiveEvent*)MilesAlloc(sizeof(MilesActiveEvent));
            NewEvent->EventId = EventId;
            NewEvent->EventName = "Startwelcome3D";
            NewEvent->X = X;
            NewEvent->Y = Y;
            NewEvent->Next = ListOfActiveEvents;
            ListOfActiveEvents = NewEvent;

            ActiveEventCount++;
        }

        if (CurrentTick - LastPrintTick > 1000)
        {
            LastPrintTick = CurrentTick;

            // Print the current list of events.
            if (ListOfActiveEvents == 0)
                console_printf("<No Events!>\n");
            else
            {
                console_printf("%d Events:\n", ActiveEventCount);
                MilesActiveEvent* Current = ListOfActiveEvents;
                for (; Current; Current = Current->Next)
                {
                    console_printf("    ID: %llu:%s @ %.1f, %.1f\n", 
                        Current->EventId, 
                        Current->EventName, 
                        Current->X,
                        Current->Y);
                }
            }
        }

        if (CurrentTick > EndTick)
            break;

        // Track any completions.
        MilesEventInfoHeader* Header = 0;
        while (MilesEventInfoQueueEnum(Driver, &Header))
        {
            // Only care about completion
            if (Header->Type != MilesEventInfoTypeEventVoicesComplete)
                continue;

            // The voices complete message is sent when all voices
            // started for an event completes.
            MilesEventInfoEventVoicesComplete* Complete = 
                (MilesEventInfoEventVoicesComplete*)Header;

            U64 CompleteId = MilesAssemble64BitId(
                Complete->EventIdLow, Complete->EventIdHigh);

            // Find the active event and free it.
            MilesActiveEvent* Last = 0;
            MilesActiveEvent* Find = ListOfActiveEvents;
            for (; Find; Last = Find, Find = Find->Next)
            {
                if (Find->EventId == CompleteId)
                {
                    // All voices started by this event are complete! If the
                    // event started no voices, this occurs immediately

                    // Unlink and free
                    if (Last)
                        Last->Next = Find->Next;
                    else
                        ListOfActiveEvents = Find->Next;
                    MilesFree(Find);
                    break;
                }
            }
            assert(Find != 0); // assert if we didn't find the event.
            ActiveEventCount--;
        }

        MilesSleep(10);
    }

    // Free tracking list
    {
        MilesActiveEvent* Next, *Cur = ListOfActiveEvents;
        for (; Cur; Cur = Next)
        {
            Next = Cur->Next;
            MilesFree(Cur);
        }
    }

    // Destroy all Miles resources.
    MilesShutdown();

    return 1;
}

// @cdep pre $Defaults
// @cdep post $Build