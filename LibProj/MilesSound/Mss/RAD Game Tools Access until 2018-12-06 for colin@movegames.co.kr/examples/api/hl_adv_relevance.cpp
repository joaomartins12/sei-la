//idoc(page, example_hl_adv_relevance)
//-----------------------------------------------------------------------------
//
// hl_adv_relevance - demonstrates relevancy filtering for client/server.
//
// Copyright (c) 2014, RAD Game Tools, Inc.
//
// Miles 10 allows the event context to be constructed and a conservative
// estimation to be made as to whether the event is relevant to a given
// set of listeners (client).
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
    MilesConsoleInit();
    MilesStartup(MilesErrorFn);

    //
    // In order to model a client/server situation, two separate drivers
    // will be created.
    //
    HMILESDRIVER ServerDriver = MilesDriverCreate(0);
    MilesDriverRegisterBinkAudio(ServerDriver);

    HMILESDRIVER ClientDriver = MilesDriverCreate(0);
    MilesDriverRegisterBinkAudio(ClientDriver);

    //
    // The same banks need to be loaded on the server and client,
    // for obvious reasons.
    //

    // Server...
    MilesProjectLoad(ServerDriver, PATH_TO_MEDIA"api_project.mprj", 0, 0);
    HMILESBANK Bank = MilesBankLoad(ServerDriver, PATH_TO_MEDIA"api_bank.mbnk", 
        PATH_TO_MEDIA"api_bank_stream.mstr", 0, 0);
    U32 BankLoadResult = MilesBankWaitForLoad(Bank);
    if (BankLoadResult != MilesBankStatusLoaded)
    {
        // Error already reported.
        MilesShutdown();
        return 0;        
    }

    // Client...
    MilesProjectLoad(ClientDriver, PATH_TO_MEDIA"api_project.mprj", 0, 0);
    Bank = MilesBankLoad(ClientDriver, PATH_TO_MEDIA"api_bank.mbnk", 
        PATH_TO_MEDIA"api_bank_stream.mstr", 0, 0);
    BankLoadResult = MilesBankWaitForLoad(Bank);
    if (BankLoadResult != MilesBankStatusLoaded)
    {
        // Error already reported.
        MilesShutdown();
        return 0;        
    }

    //
    // For server/client communications, in order to save bandwidth, it may
    // be desirable to filter the events the server sends to individual clients.
    //
    // What metric to use will vary between games, but for this example,
    // events that are shorter than 10 seconds and have their volume at
    // less than -40 dB will be considered to be irrelevant to the client.
    //
    // In addition, events can be sent using a TemplateId, which reduces
    // the amount of bandwidth required to send the event name.
    //

    // Grab the template id for the desired event
    MilesEventTemplateId WelcomeId;
    MilesEventResolveName(ServerDriver, "Startwelcome3D", &WelcomeId);


    // The details function only returns the duration of the first source
    // in the event. The duration is modified by the play count and estimated
    // pitch adjustment, but depending on the event, could be off.
    float FirstSourceDuration = 0;

    MilesEventDetailsFlag Flags;
    float EstimatedRadius = 0;
    MilesEventGetDetails(ServerDriver, &WelcomeId, 
        0,   // We already know the name.
        &Flags,
        &EstimatedRadius,
        &FirstSourceDuration);

    U32 SendTheEvent = 1;

    console_printf("The event is %.1f seconds long.\n", FirstSourceDuration);
    console_printf("The event falls below the designer specified level at a distance of %.1f.\n",
        EstimatedRadius);

    // Assume for the sake of example that the player is 60 game units away
    // from the event's position.
    const float DistanceToEvent = 60.0f;

    if (DistanceToEvent > EstimatedRadius &&
        FirstSourceDuration < 5.0f)
    {
        console_printf("The event is beyond the -40 dB cutoff and is ");
        console_printf("shorter than 5 seconds (not sending)\n");
        SendTheEvent = 0;
    }

    if (Flags & MilesEventDetailsFlagControl)
    {
        // If the event does something other that simply start a voice,
        // then it is important as it affects the soundscape in some way.
        console_printf("The event affects the soundscape (sending).\n");
        SendTheEvent = 1;
    }
    if (Flags & MilesEventDetailsFlagZeroPriority)
    {
        // The sound designer has marked this event as zero priority, meaning
        // it can never be evicted by a voice limit. Most likely this means
        // the voice should always be sent, but it is game dependent.
        console_printf("The event is Zero Priority (sending).\n");
        SendTheEvent = 1;
    }

    if (SendTheEvent)
    {
        //
        // We mimic sending the event by queuing the event on the client driver
        // instead of the server driver.
        //
        HMILESQUEUE ClientQueue = MilesQueueCreate(ClientDriver);
        MilesQueueEventRunByTemplateId(ClientQueue, &WelcomeId);
        MilesQueueSubmit(ClientQueue);

        MilesConsoleHandleEvents();
        
        // Wait for the sound to stop, then exit. Add a bit to drain HW.
        MilesSleep((U32)(FirstSourceDuration * 1000) + 16);        
    }


    MilesShutdown();
    return 1;
}

// @cdep pre $Defaults
// @cdep post $Build