//idoc(page, example_hl_subtitles)
//-----------------------------------------------------------------------------
//
// hl_subtitles.cpp - demonstrates how to drive subtitles using Miles 10
//
// Copyright (c) 2014, RAD Game Tools, Inc.
//
// A reasonably complex example detailing how to use actor queries to
// sync dialog data with audio.
//
// Eventually, we want to have a system for the subtitle data to
// be potentially set in Miles Studio, but for the moment you have to
// supply the subtitle data externally
//
//-----------------------------------------------------------------------------

#include "miles.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "console_abstract.h"
//
// Each dialog piece has one or more subtitle entries
// specifying timing
//
struct SubtitleEntry
{
    U32 delay_time_ms;
    char const* utf8_string;
    U32 visible_time_ms; // Must be ~0U for the last entry
};

// Subtitles for a couple sources we expect.
const SubtitleEntry ogres_subtitle[] = 
{
    {0, "It's possible,", 1600},
    {300, "but the ogres aren't going to let me", 2000},
    {0, "just stroll in for an inspection.", ~0U}
};

const SubtitleEntry welcome_subtitle[] = 
{
    {0, "Welcome to the Miles Sound System!", ~0U}
};

static HMILESDRIVER driver;
static HMILESQUEUE queue;
static HMILESPOSTBOX postbox;
static S32 total_count = 0;
static S32 current_entry_index = 0;
static S32 currently_playing = 0;
static SubtitleEntry const* current_entries = 0;

//-----------------------------------------------------------------------------
//
// Update the dialog text and start new voices on completion.
//
//-----------------------------------------------------------------------------
static void TickDialog()
{
    MilesEventInfoHeader* Header = 0;
    while (MilesEventInfoQueueEnum(driver, &Header))
    {
        if (Header->Type == MilesEventInfoTypeEventVoicesComplete)
        {
            currently_playing = 0; // done with the current voice
            current_entries = 0;
        }
        else if (Header->Type == MilesEventInfoTypeVoiceSource)
        {
            MilesEventInfoVoiceSource* source = (MilesEventInfoVoiceSource*)Header;
            { // Find the entries for the current voice
                // source name is of the form "optional_folders/source_name"
        
                char const* source_name = strrchr(source->SourceName, '/');
                if (source_name)
                    source_name++;
                else
                    source_name = source->SourceName;

                if (strcmp(source_name, "welcome") == 0)
                    current_entries = welcome_subtitle;
                else if (strcmp(source_name, "ogres") == 0)
                    current_entries = ogres_subtitle;

                if (current_entries == 0)
                {
                    printf("Can't find subtitle data!!!\n");
                    exit(1);
                }

                current_entry_index = 0;
            }
        }
    }

    // If there is no outstanding work and there are no voices after an update,
    // then we can start a new one.
    if (currently_playing == 0)
    {
        printf("\n");

        // Start a new sound, alternating between the two options.
        MilesQueueEventInfoMask(queue, MilesEventInfoRequestEventInfo | MilesEventInfoRequestVoiceSource);
        MilesQueueEventPositionPostbox(queue, postbox);
        if (total_count % 2)
            MilesQueueEventRun(queue, "startwelcome");
        else
            MilesQueueEventRun(queue, "startogres");
        total_count++;

        MilesQueueSubmit(queue);
        currently_playing = 1;

        // We just queued another voice so there's no subtitle yet.
        return;
    }

    if (current_entries == 0)
        return; // waiting for the voice to start

    // Otherwise, update the subtitle.
    U32 current_ms = (U32)MilesPostboxGetValue(postbox);   

    // Find where we are in the subtitle.
    SubtitleEntry const* current_entry = current_entries;
    char const* current_line = 0;
    S32 checking_index = 0;
    for (;;)
    {
        if (current_ms < current_entry->delay_time_ms)
        {
            // We are in the delay.
            break;
        }
        if (current_entry->visible_time_ms == ~0U ||
            current_ms < current_entry->delay_time_ms + current_entry->visible_time_ms)
        {
            // We are in the text or on the last entry.
            current_line = current_entry->utf8_string;
            break;
        }
        current_ms -= (current_entry->delay_time_ms + current_entry->visible_time_ms);

        // we are past this entry.
        current_entry++;
        checking_index++;
    }

    if (checking_index != current_entry_index)
    {
        // We are on a new entry, so make a new line before printing it.
        printf("\n");
        current_entry_index = checking_index;
    }

    printf("\r");
    if (current_line)
        printf("%s", current_line);
}

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
    
    // Init global miles state.
    MilesStartup(MilesErrorFn);

    // Create the driver and register decoders
    MilesDriver* Driver = MilesDriverCreate(0);
    MilesDriverRegisterBinkAudio(Driver);
    MilesEventInfoQueueEnable(Driver);
    driver = Driver;

    // Load the project and bank, then wait for the async loads to complete.
    MilesProjectLoad(Driver, PATH_TO_MEDIA"api_project.mprj", 0, 0);
    MilesBank* Bank = MilesBankLoad(Driver, PATH_TO_MEDIA"api_bank.mbnk", PATH_TO_MEDIA"api_bank_stream.mstr", 0, 0, 0);
    MilesBankWaitForLoad(Bank);

    // Create a queue for all the things we want to do.
    queue = MilesQueueCreate(Driver);
    postbox = MilesPostboxCreate(Driver);

    for (;;)
    {
        TickDialog();

#ifdef __RADNT__
        if (_kbhit())
        {
            switch (_getch())
            {
            case 27: // esc
                {
                    MilesShutdown();
                    return 0;
                }
            }
        }
#endif
        
        // Let the other threads do some stuff.
        MilesSleep(10);

        MilesConsoleHandleEvents();
    }
}


// @cdep pre $Defaults
// @cdep post $Build