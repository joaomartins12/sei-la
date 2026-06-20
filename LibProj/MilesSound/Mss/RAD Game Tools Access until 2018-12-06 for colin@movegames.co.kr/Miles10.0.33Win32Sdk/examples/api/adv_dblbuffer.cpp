//idoc(page, example_adv_dblbuffer)
//-----------------------------------------------------------------------------
//
// adv_dblbuffer - shows how to stream a series of buffers to a Miles sample.
//
// Copyright (c) 2014, RAD Game Tools, Inc.
//
// Generally it is assumed that streaming buffers to Miles will be due to
// another game system producing PCM samples that need to be added to the
// sound scape. As a result, this example demonstrates providing raw data
// to Miles.
//
//-----------------------------------------------------------------------------
#include "miles.h"

#include <memory.h>
#include <math.h>
#include "console_abstract.h"
#include <stdio.h>


#define PART_COUNT 8

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct StreamingSource
{
    char* StreamBuffers[PART_COUNT];
    U32 StreamBufferSizes[PART_COUNT];
    U32 StreamNextIndex;
    U32 Done;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void RADLINK EndOfBufferCB(HMILESSAMPLE sample, void* completed_buffer, void* user)
{
    StreamingSource* source = (StreamingSource*)user;

    // Queue the next index as long as we aren't completed.
    if (source->StreamNextIndex < PART_COUNT)
    {
        MilesSampleAddBuffer(sample,
            source->StreamBuffers[source->StreamNextIndex],
            source->StreamBufferSizes[source->StreamNextIndex]);
        source->StreamNextIndex++;
    }

    // We've exhausted all buffers, so mark that we're done so we can exit.
    if (MilesSampleGetBufferCount(sample) == 0)
        source->Done = 1;
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
    
    MilesStartup(MilesErrorFn);

    // Read a file to stream to miles.
    char const* SourceFile = "../media/en_US/welcome.wav";
    U32* SourceWav = (U32*)MilesFileRead(SourceFile, MilesFileReadWithSize);
    if (SourceWav == 0)
        return 0;

    U32 Rate = 0;
    U16 Channels = 0;
    U32 DataOffset = 0, DataLen = 0;
    if (MilesGetWavMetadata(SourceWav + 1, *SourceWav, &Channels, &Rate, 
        0, 0, 0, 0, &DataOffset, &DataLen) == 0)
        return 0;

    // We are assuming that we loaded a PCM wav file that is 16 bits, for
    // brevity.
    StreamingSource Source = {};

    // The raw PCM data is offset from the file pointer due to
    // RIFF headers.
    char* SamplesDataStart = (char*)(SourceWav + 1);
    SamplesDataStart += DataOffset;

    //
    // Break the file up in to a bunch of parts, so we can pass
    // them in chunks. We could just pass chunks from the actual
    // file but for demonstration purposes each will be separate.
    //
    U32 PartByteCount = (DataLen / PART_COUNT);
    U32 TotalPartBytes = 0;
    for (U32 i = 0; i < PART_COUNT; i++)
    {
        U32 ThisSize = PartByteCount;

        // Make sure the last chunk has the rest of the file.
        if (i == PART_COUNT-1)
            ThisSize = DataLen - TotalPartBytes;

        void* ptr = MilesAlloc(ThisSize);
        Source.StreamBuffers[i] = (char*)ptr;
        Source.StreamBufferSizes[i] = ThisSize;

        memcpy(ptr, SamplesDataStart + TotalPartBytes, ThisSize);
        TotalPartBytes += ThisSize;
    }

    // Create our driver and sample
    MilesDriver* Driver = MilesDriverCreate(0);
    MilesSample* Sample = MilesSampleCreate(Driver, 0, 0);

    // Set up the first buffer. If we needed Miles to decode the data
    // we'd use $MilesSampleSetSource, but since we are passing raw
    // PCM data we don't.
    MilesSampleSetSourceRaw(Sample, 
        Source.StreamBuffers[0],
        Source.StreamBufferSizes[0],
        Rate,
        Channels);

    // Mark what buffer we are going to supply next.
    Source.StreamNextIndex = 1;

    // Tell Miles that buffers are to be discarded once played.
    //
    // Since we are managing the buffers, Miles can't keep track of 
    // where we are in the file. As a result, anything that requires
    // knowing that will fail (looping, setting position). This is
    // all handled on our end by providing the buffers that do what
    // we want.
    //
    // This also has the side effect that Miles can't tell the difference
    // between the end of the file and a starvation - so the sample
    // will never end on its own, it will have to be stopped via
    // $MilesSamplePause once the game has determined the buffers
    // are complete.
    //
    MilesSampleSetSourceDiscards(Sample);

    // Set the callback responsible for streaming new buffers.
    MilesSampleSetEOBCallback(Sample, EndOfBufferCB, &Source);

    MilesSamplePlay(Sample);

    U64 ExitTimer = ~0ULL;
    for (;;)
    {
        MilesConsoleHandleEvents();
        MilesSleep(10);

        //
        // Normally we'd check the sample status to determine
        // completion, however discarding samples never end.
        // So we have to manage it on our side.
        //
        if (Source.Done && ExitTimer == ~0ULL)
            ExitTimer = MilesGetMsCount(); // allow the HW to drain.

        if (ExitTimer != ~0ULL &&
            MilesGetMsCount() - ExitTimer > 100)
            break;
    }

    MilesSampleDestroy(Sample);
    MilesFree(SourceWav);
    for (U32 i = 0; i < PART_COUNT; i++)
        MilesFree(Source.StreamBuffers[i]);

    MilesShutdown();
    return 0;
}


// @cdep pre $Defaults
// @cdep post $Build