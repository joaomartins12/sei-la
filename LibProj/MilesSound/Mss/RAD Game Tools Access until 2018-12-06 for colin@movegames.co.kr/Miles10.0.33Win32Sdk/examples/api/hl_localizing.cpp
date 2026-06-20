//idoc(page, example_hl_localizing)
//-----------------------------------------------------------------------------
//
// hl_localizing.cpp - loading localized projects.
//
// Copyright (c) 2014-2016, RAD Game Tools, Inc.
//
// Localized projects produce an additional stream (mstr) file for EACH 
// language defined in the project.
//
//-----------------------------------------------------------------------------
#include "miles.h"
#include "console_abstract.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void RADLINK MilesErrorFn(MilesReportType Type, char const* Message)
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
    // The project must know which language it is loading. This name must
    // match the name of the language in Miles Studio under Project->Settings->
    // Languages.
    //
    MilesProjectLoad(Driver, 
        PATH_TO_MEDIA"localize_project.mprj", 
        "English", 
        0);

    //
    // Each bank must then provide a filename to the relevant language.
    // This file must be the file rendered for the language in Miles Studio
    // that matches language name passed to MilesProjectLoad, or the load 
    // will fail. Note that the _filename_ doesn't matter - the mstr file
    // knows which language it was built for, so renaming will have no
    // effect on its validity.
    //
    MilesBank* Bank = MilesBankLoad(Driver, 
        PATH_TO_MEDIA"localize_bank.mbnk",               // bank metadata
        PATH_TO_MEDIA"localize_bank_stream.mstr",        // global audio data
        PATH_TO_MEDIA"localize_bank_English.mstr"        // english audio data
        , 0);

    U32 BankLoadResult = MilesBankWaitForLoad(Bank);
    if (BankLoadResult != MilesBankStatusLoaded)
    {
        // Error already reported via the report function.
        MilesShutdown();
        return 0;        
    }

    //
    // Everything else is the same - events that use a source that is localized
    // will automatically use the version from the English stream. 
    //


    MilesConsoleHandleEvents();

    // Destroy all Miles resources.
    MilesShutdown();

    return 1;
}

// @cdep pre $Defaults
// @cdep post $Build