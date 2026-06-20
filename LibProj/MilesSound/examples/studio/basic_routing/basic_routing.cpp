//-----------------------------------------------------------------------------
//
// basic_routing
//
// This is the example code to run and demonstrate the associated 
// Miles Studio project. It is not necessarily meant to be a complete
// example for a programmer, and as a result error checking will
// generally be omitted. In addition, common or rote code will not
// be commented or explained - see the API examples for details
// on how to properly initialize and use Miles.
//
// Copyright (c) 2014, RAD Game Tools, Inc.
//
//-----------------------------------------------------------------------------
#include "miles.h"
#include <stdio.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void RADLINK MilesErrorFn(MilesReportType Type, char const* Message)
{
    printf("[%d] - %s\n", Type, Message);
}

void main()
{
    MilesStartup(MilesErrorFn);

    MilesDriver* Driver = MilesDriverCreate(0);
    MilesDriverRegisterBinkAudio(Driver);

    MilesProjectLoad(Driver, "basic_routing.mprj", 0, 0);
    HMILESBANK Bank = MilesBankLoad(Driver, "basic_routing.mbnk", "basic_routing_stream.mstr", 0, 0);
    if (MilesBankWaitForLoad(Bank) != MilesBankStatusLoaded)
    {
        printf("Failed to load bank/project, run setup.bat first!\n");
        MilesSleep(3000);
        MilesShutdown();
        return;
    }
    
    // Create a queue for our events
    HMILESQUEUE queue = MilesQueueCreate(Driver);

    // Queue the example event.
    MilesQueueEventRun(queue, "Startwelcome");
    MilesQueueSubmit(queue);

    // Wait for the sound to play
    MilesSleep(3000);

    // Destroy everything
    MilesShutdown();
}

// @cdep pre $Defaults
// @cdep post $Build