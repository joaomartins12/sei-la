//-----------------------------------------------------------------------------
//
// multipleoutputs.cpp - routing a sample to two hardware outputs.
//
// Copyright (c) 2014, RAD Game Tools, Inc.
//
// Drivers can manage more than one hardware output, and routes
// from samples or buses to these additional outputs work as you
// would expect.
//
//-----------------------------------------------------------------------------

#include "miles.h"

#include <memory.h>
#include <math.h>
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
int main()
{
    MilesExamplesInitConsole();
    MilesStartup(MilesErrorFn);

    // Load a file.
    char const* WavFileName = "..\\media\\en_US\\welcome.wav";
    U32* WavData = (U32*)MilesFileRead(WavFileName, MilesFileReadWithSize);

    // Get the default output for this platform.
    // Additional output APIs may be available for your platform.
    // 
    U32 OutputRate;
    MilesChannelSpec OutputSpec;
    HMILESHARDWAREOUTPUT DefaultOutput = MilesOutputGetDefault(&OutputRate, &OutputSpec);

    // Create the first driver using the default output
    // This serves as the output that is used if zero is ever
    // passed for an output.
    HMILESDRIVER Driver = MilesDriverCreateEx(OutputRate, OutputSpec, DefaultOutput, 0);

    // Create a second output on the same driver.
    // In this case the actual hardware selected is the same, but
    // it can be anything.
    HMILESOUTPUT SecondOutput = MilesOutputCreate(Driver, OutputRate, OutputSpec, DefaultOutput, 0);

    // Create a mono bus that outputs to the second output instead of default.
    HMILESBUS ReverbBus = MilesBusCreate(Driver, 1, SecondOutput, 1.0f);

    // Add a reverb to that bus to distinguish it.
    MilesBusAddFilterByName(ReverbBus, "Reverb");

    // Create a sample with its output going to the driver's default outptut (0)
    HMILESSAMPLE Sample = MilesSampleCreate(Driver, 0, 0);

    // Add a route from the Sample to our ReverbBus, so we have a complete path for
    // audio from the route to the second output.
    MilesSampleCreateRoute(Sample, ReverbBus, 1.0f, 0);

    // Set the sample source and play.
    MilesSampleSetSource(Sample, WavData + 1, WavData[0], MilesFileTypeAuto);
    MilesSamplePlay(Sample);

    S32 LatencyEndTime = -1;
    for (;;)
    {
        // If the sample has ended, check the end time.
        if (LatencyEndTime != -1)
        {
            if (MilesGetMsCount() > (U32)LatencyEndTime)
                break; // exit.
        }

        // Once the sample is done, wait for the output latency before exiting.
        if (LatencyEndTime == -1 &&
            MilesSampleGetStatus(Sample) == MilesSampleStatusDone)
        {
            LatencyEndTime = MilesGetMsCount() + (U32)MilesOutputGetLatency(MilesDriverGetOutput(Driver, 0));
        }

        MilesSleep(1);
    }

    MilesFree(WavData);
    MilesShutdown();
    return 0;
}


// @cdep pre $Defaults
// @cdep post $Build