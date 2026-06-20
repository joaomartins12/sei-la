//idoc(page, example_hl_basic_auditioning)
//-----------------------------------------------------------------------------
//
// hl_basic_auditioning.cpp - basic high level system auditioning.
//
// Copyright (c) 2014-2016, RAD Game Tools, Inc.
//
// Miles 10 allows the game the connect to Miles Studio and download all
// assets. When connected in this way, edits to the assets in Miles Studio
// can be pushed to the game without restarting.
//
//-----------------------------------------------------------------------------
#include "miles.h"
#include "console_abstract.h"

#include <stdlib.h>

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

    MilesStartup(MilesErrorFn);
    MilesDriver* Driver = MilesDriverCreate(0);
    MilesDriverRegisterBinkAudio(Driver);

    // We enable the info queue to show some audition relevant messages.
    MilesEventInfoQueueEnable(Driver);

    //
    // Using auditioning affects the load sequence. Miles has to know how
    // it is auditioning prior to all of the project and bank loads. There are
    // two types of auditioning:
    //  Download - All assets are downloaded from Miles Studio. As a result,
    //      the correct project must be loaded in Miles Studio, and the project
    //      must be in working order. If this is not the case, errors will be
    //      sent to the Report function passed in MilesStartup. Under this mode
    //      the rendered projects on disc are NOT used - filenames aren't even
    //      necessary.
    //
    //  Profile - The rendered projects on disc are used as normal, but Miles
    //      will connect to Miles Studio and send profiling information. This
    //      information is viewable in the Client Debugger as with Download.
    //      No project needs to be loaded in Miles Studio for this to work.
    //
    MilesAuditionStartup(Driver, MilesAuditionTypeDownload);

    // This function does not return until the connection is established.
    MilesAuditionConnect("127.0.0.1");

    //
    // When auditioning a project, Miles needs to know the name of the 
    // desired project and bank as the filename might not match. Since we 
    // are downloading the project and bank from Miles Studio, we don't need
    // a filename at all - nothing actually gets loaded off of disc. 
    //
    // If the filenames happen to match, then the auditioning name is not
    // needed - Miles will assume the base filename without extension is the
    // audition name (e.g. "/path/to/project.mprj" will assume "project").
    //
    MilesProjectLoad(Driver, PATH_TO_MEDIA"api_project.mprj", 0, 
        "api_project");
    MilesBank* Bank = MilesBankLoad(Driver, PATH_TO_MEDIA"api_bank.mbnk", 
        0, 0, "api_bank");

    //
    // Wait for the load to complete from the server. Since the server has to
    // compile and send the project, this will likely take longer than loading
    // off disc.
    //
    U32 BankLoadResult = MilesBankWaitForLoad(Bank);
    if (BankLoadResult != MilesBankStatusLoaded)
    {
        // If the connection failed, print a message since they
        // probably don't know how to fix it.
        printf("Failed to open the bank!\n");
        printf("Load examples/api_project/api_project.prj in Miles Studio.\n");
        MilesShutdown();
        return 0;
    }


    // Create a queue for all the things we want to do.
    HMILESQUEUE Queue = MilesQueueCreate(Driver);

    // Print the names of all of the events that are in the bank we downloaded
    // from Miles Studio.
    console_printf("Events: \n");
    U32 EventCount = MilesBankGetEventCount(Bank);

    // Cap to 9 since we only track 1-9 for input keys.
    if (EventCount > 9)
        EventCount = 9;

    for (U32 i = 0; i < EventCount; i++)
        console_printf("  %d: %s\n", i + 1, MilesBankGetEventName(Bank, i));
    console_printf("--------------------\n");
    console_printf("  0: stop all sounds\n");
    console_printf("esc: exit\n");

    for (;;)
    {
        MilesConsoleHandleEvents();
        
        MilesEventInfoHeader* Header = 0;
        while (MilesEventInfoQueueEnum(Driver, &Header))
        {
            if (Header->Type == MilesEventInfoTypeAuditionPost)
            {
                // Miles Studio has added a new event to the system.
                MilesEventInfoAuditionPost* Post = 
                    (MilesEventInfoAuditionPost*)Header;

                // Sources can be added, which are not queueable (they are just
                // sources other events can use). However, if the source has
                // a Default Event, then queueing it by name will cause that
                // event to be used, except forcing it to use that source 
                // instead of its own source tree (see Core Concepts/Event 
                // Naming).
                bool ReportDetails = false;
                if (Post->IsSource)
                {
                    if (Post->SourceHasDefaultEvent)
                    {
                        console_printf("Added Source '%s' (queueable)\n",
                            Post->NameWithBank);
                        ReportDetails = true;
                    }
                    else
                    {
                        console_printf("Added Source '%s' (not queuable)\n",
                            Post->NameWithBank);
                    }
                }
                else
                {
                    console_printf("Added Event '%s'\n", Post->NameWithBank);
                    ReportDetails = true;
                }

                if (ReportDetails)
                {
                    float Radius = 0;
                    float Duration = 0;

                    if (MilesEventGetDetails(Driver, &Post->TemplateId, 0, 0, &Radius, &Duration))
                    {
                        console_printf("Radius & Duration: %.1f/%.1f\n", Radius, Duration);
                    }
                }
            }
            else if (Header->Type == MilesEventInfoTypeAuditionRecreate)
            {
                // If the designer changes any global setting (buses, 
                // controllers, snapshots, etc) then a sync causes Miles to
                // recreate the entire system. Miles tries to retain state 
                // but occasionally this may fail and voices may get dropped
                // or otherwise change.
                console_printf("Event system recreated (project change).\n");
            }
        }

        if (_kbhit())
        {
            U32 value = _getch();
            switch (value)
            {
            case '0':
                MilesQueueEventRun(Queue, "STOP");
                MilesQueueSubmit(Queue);
                break;
            case 27: goto exit; // escape
            default:
                {
                    if (value < '1' || value > '9')
                        break;
                    U32 index = value - '1';
                    MilesQueueEventRun(Queue, 
                        MilesBankGetEventName(Bank, index));
                    MilesQueueSubmit(Queue);
                    break;
                }
            }
        }

        MilesSleep(10);
    }

exit:



    MilesShutdown();
    return 1;
}

// @cdep pre $Defaults
// @cdep post $Build