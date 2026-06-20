//-----------------------------------------------------------------------------
//
// adv_sidechain
//
// This is the example code to run and demonstrate the associated 
// Miles Studio project. It is not necessarily meant to be a complete
// example for a programmer, and as a result error checking will
// generally be omitted. In addition, common or rote code will not
// be commented or explained - see the API examples for details
// on how to properly initialize and use Miles.
//
// Copyright (c) 2014-2015, RAD Game Tools, Inc.
//
//-----------------------------------------------------------------------------
#include "miles.h"
#include <stdio.h>
#include <math.h>

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

    MilesProjectLoad(Driver, "adv_sidechain.mprj", 0, 0);
    HMILESBANK Bank = MilesBankLoad(Driver, "adv_sidechain.mbnk", "adv_sidechain_stream.mstr", 0, 0);
    if (MilesBankWaitForLoad(Bank) != MilesBankStatusLoaded)
    {
        printf("Failed to load bank/project, run setup.bat first!\n");
        MilesSleep(3000);
        MilesShutdown();
        return;
    }
    
    HMILESQUEUE queue = MilesQueueCreate(Driver);
    MilesQueueEventRun(queue, "play_music");
    MilesQueueSubmit(queue);

    // Start the dialog after a second.
    U64 StartTick = MilesGetMsCount();
    U32 HasPlayed = 0;
    for (;;)
    {
        MilesSleep(16);

        U64 CurrentTick = MilesGetMsCount();
        if (CurrentTick - StartTick > 1000)
        {
            if (HasPlayed == 0)
            {
                HasPlayed = 1;
                MilesQueueEventRun(queue, "play_dialog");
                MilesQueueSubmit(queue);
            }
        }
        if (CurrentTick - StartTick > 4000)
            break;
    }

    // Stop the sound and give it a bit to drain in the hardware.
    MilesQueueEventRun(queue, "STOP");
    MilesQueueSubmit(queue);
    MilesSleep(16);

    // Destroy everything
    MilesShutdown();
}

// @cdep pre $Defaults
// @cdep pre $AddLib(audition)
// @cdep post $Build