//-----------------------------------------------------------------------------
//
// basic_controller
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

    MilesProjectLoad(Driver, "basic_controller.mprj", 0, 0);
    HMILESBANK Bank = MilesBankLoad(Driver, "basic_controller.mbnk", "basic_controller_stream.mstr", 0, 0);
    if (MilesBankWaitForLoad(Bank) != MilesBankStatusLoaded)
    {
        printf("Failed to load bank/project, run setup.bat first!\n");
        MilesSleep(3000);
        MilesShutdown();
        return;
    }
    
    HMILESQUEUE queue = MilesQueueCreate(Driver);

    MilesQueueEventControllerValue(queue, "NewController", -96.0f);
    U64 EventId = MilesQueueEventRun(queue, "Startwelcome");
    MilesQueueSubmit(queue);

    U64 StartTick = MilesGetMsCount();
    for (;;)
    {
        MilesSleep(10);

        U64 CurrentTick = MilesGetMsCount();
        if (CurrentTick - StartTick > 3000)
            break;
        float Pct = (CurrentTick - StartTick) / 2000.0f;
        if (Pct > 1)
            Pct = 1;

        MilesQueueUpdateEventControllerValue(queue, EventId, "NewController", MilesLinearToDb(Pct));
        //MilesQueueControllerValue(queue, "NewController", MilesLinearToDb(Pct));
        MilesQueueSubmit(queue);
    }

    MilesShutdown();
}

// @cdep pre $Defaults
// @cdep post $Build