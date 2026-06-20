//idoc(page, example_basic_reverb)
//-----------------------------------------------------------------------------
//
// basic_reverb.cpp - basic filter usage
//
// Copyright (c) 2014, RAD Game Tools, Inc.
//
// Filters can be installed directly on to a sample, or on to a bus. This
// example instantiates a reverb filter on to a sample.
//
//-----------------------------------------------------------------------------

#include "miles.h"
#include <stdio.h>
#include "console_abstract.h"

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
    
    //
    // Initialize core Miles system
    //
    MilesStartup(MilesErrorFn);

    //
    // Load the impulse response to use.
    //
    U32* IRFile = (U32*)MilesFileRead("../media/living_room_in_bedroom_1.wav",
        MilesFileReadWithSize);
    if (IRFile == 0)
    {
        return 0;
    }

    //
    // Parse the .wav file to get the samples.
    //
    U16 IRChannels=0;
    U32 IROffsetToSamples=0;
    U32 IRRate=0;
    U16 IRBlock=0;
    U16 IRBits=0;
    U32 IRSampleCount=0;
    MilesGetWavMetadata(IRFile + 1, *IRFile, &IRChannels, &IRRate, 0, &IRBlock, 
        &IRSampleCount, &IRBits, &IROffsetToSamples, 0);

    if (IRRate != 48000)
    {
        console_printf("[Warning] Impulse Response is not 48khz!\n");
    }

    if (IRBits != 16)
    {
        MilesFree(IRFile);
        console_printf("Impulse Response must be 16 bit wav file.\n");
        return 0;
    }

    if (IRBlock != 0)
    {
        MilesFree(IRFile);
        console_printf("Impulse Response must not be ADPCM.\n");
        return 0;
    }

    if (IRChannels > 2)
    {
        MilesFree(IRFile);
        console_printf("Impulse Response must be mono or stereo.\n");
        return 0;
    }

    S16* IRSamples = (S16*)((char*)(IRFile + 1) + IROffsetToSamples);

    //
    // Bake the IR in to a format the filter can use.
    //
    void* BakedIR;
    U32 BakedIRSize;
    MilesBakeImpulseResponse(IRSamples, IRSampleCount, 
        IRChannels == 2 ? 1 : 0, 0, 0, &BakedIR, &BakedIRSize);

    // Done with the source file.
    MilesFree(IRFile);

    //
    // Load the source file to play.
    //
    U32* SourceFile = (U32*)MilesFileRead("../media/en_US/welcome.wav", 
        MilesFileReadWithSize);
    if (SourceFile == 0)
    {
        MilesFree(BakedIR);
        return 0;
    }

    // Create a default driver.
    HMILESDRIVER Driver = MilesDriverCreate(0);
    if (Driver == 0)
    {
        MilesFree(BakedIR);
        MilesFree(SourceFile);
        return 0;
    }

    //
    // Register the impulse on the driver for use by filters.
    //
    MilesDriverRegisterImpulseResponse(Driver, 1, BakedIR);

    // The driver makes a copy, so we are done with it
    MilesFree(BakedIR);

    // Create a sample using the default output on the driver.
    HMILESSAMPLE Sample = MilesSampleCreate(Driver, 0, 0);
    if (Sample == 0)
    {
        MilesDriverDestroy(Driver);
        MilesFree(SourceFile);
        return 0;
    }

    // Set the sample's source.
    if (MilesSampleSetSource(Sample, SourceFile + 1, SourceFile[0], 
        MilesFileTypeAuto) == 0)
    {
        MilesDriverDestroy(Driver);
        MilesFree(SourceFile);
        return 0;
    }

    // Add a convolution filter to the sample.
    HMILESFILTERSTAGE FilterStage = MilesSampleAddFilterByName(Sample, 
        MilesFilterNameConvolution);
    if (FilterStage == 0)
    {
        MilesDriverDestroy(Driver);
        MilesFree(SourceFile);
        return 0;
    }

    // Set the impulse index on the filter to the impulse
    // we registered on the driver.
    MilesFilterSetPropertyValueByName(FilterStage, "Impulse", 1);

    // Convolutions often increase the total energy due to room
    // reflections, so attenuate it down a bit.
    MilesFilterSetPropertyValueByName(FilterStage, "Gain", -6);

    MilesSamplePlay(Sample);

    // Loop until the MilesSample is done and the last sample has 
    // had a chance to be delivered to the hw.
    S32 EndTimer = -1;
    for (;;)
    {
        if (EndTimer != -1 && MilesGetMsCount() > (U32)EndTimer)
            break;

        if (EndTimer == -1 && 
            MilesSampleGetStatus(Sample) == MilesSampleStatusDone)
        {
            // If we are done, set a timer so we quit after the 
            // output latency has been consumed.
            EndTimer = 100;
        }

        MilesSleep(1);
        MilesConsoleHandleEvents();
    }

    MilesFree(SourceFile);
    MilesShutdown();
    return 0;
}


// @cdep pre $Defaults
// @cdep post $Build