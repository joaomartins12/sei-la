//idoc(page, example_hl_patches)
//-----------------------------------------------------------------------------
//
// hl_patches.cpp - Loading patches
//
// Copyright (c) 2014-2016, RAD Game Tools, Inc.
//
// Demonstrates how to load all files necessary to run a patched project.
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
    MilesConsoleInit();
    MilesStartup(MilesErrorFn);
    MilesDriver* Driver = MilesDriverCreate(0);
    MilesDriverRegisterBinkAudio(Driver);

    //
    // Patch loading doesn't affect projects, as projects are exclusively
    // metadata, and are always regenerated when a patch is created.
    //
    MilesProjectLoad(Driver, PATH_TO_MEDIA"api_project.mprj", 0, 0);

    //
    // Patch loading DOES affect banks. The metadata for each bank is
    // regenerated completely, however the sound data is retained. When
    // loading, Miles will report back how many patches are expected once
    // the metadata for the bank is loaded.
    //

    //
    // Kick off the normal load.
    //
    MilesBank* Bank = MilesBankLoad(Driver, PATH_TO_MEDIA"api_bank.mbnk", 
        PATH_TO_MEDIA"api_bank_stream.mstr", 0, 0);

    //
    // The game can track the number of patches separately. If so, then
    // MilesBankPatch can be called without waiting. In this example,
    // we don't know ahead of time.
    //
    U32 AlreadyQueued = 0;
    for (;;)
    {
        MilesSleep(1);
        U32 ExpectedPatchCount = ~0U;
        U32 BankStatus = MilesBankGetStatus(Bank, &ExpectedPatchCount);
        if (BankStatus & 0x80000000)
        {
            console_printf("Error loading bank.\n");
            MilesShutdown();
            return 1;
        }
        
        if (BankStatus == MilesBankStatusLoaded)
            break; // load complete.
        
        // MilesBankGetStatus doesn't set the patch count until it knows
        // the answer, and will not return MilesBankStatusLoaded until
        // all patches have been loaded. So once we get the patch count,
        // then queue the files.        
        if (ExpectedPatchCount != ~0U &&
            AlreadyQueued == 0)
        {
            AlreadyQueued = 1;
            // We only reserve 2 digits for the patch index.
            assert(ExpectedPatchCount < 100); 

            // Add each patch file to the bank for loading.
            for (U32 PatchIndex = 0; PatchIndex < ExpectedPatchCount; PatchIndex++)
            {
                // reserve room for the path and two digits of patch count.
                char PatchFileName[] = PATH_TO_MEDIA"api_bank_stream_patch_00.mbnk";
                sprintf(PatchFileName, PATH_TO_MEDIA"api_bank_stream_patch_%d.mbnk",
                    PatchIndex);
                MilesBankPatch(Bank, PatchFileName, 0);
            }
        }
    }
    
    //
    // Wait for all of the patches to be loaded.
    //
    U32 BankLoadResult = MilesBankWaitForLoad(Bank);
    if (BankLoadResult != MilesBankStatusLoaded)
    {
        MilesShutdown();
        return 0;        
    }

    MilesConsoleHandleEvents();

    // Destroy all Miles resources.
    MilesShutdown();
    return 1;
}

// @cdep pre $Defaults
// @cdep post $Build