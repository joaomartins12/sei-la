//idoc(page, example_basic_capture)
//-----------------------------------------------------------------------------
//
// basic_capture.cpp - shows how to open a capture device and replay the audio
//
// Copyright (c) 2017, RAD Game Tools, Inc.
//
// Opens a specified capture device, and creates a sample to echo out the 
// captured audio.
//
//-----------------------------------------------------------------------------

#include "miles.h"
#include "console_abstract.h"
#include <memory.h> // memcpy

// The number of buffers to capture before playback. This increases the latency
// in echoing the audio back out, however devices might batch or otherwise miss
// scheduling deadlines, which could cause dropouts.
const int backlog = 15;

// The list of buffers that have been placed on the sample.
static U32 outstanding_buffers_count, outstanding_buffers_alloc;
static HMILESBUFFERREFERENCE* outstanding_buffers_array;

// The list of buffers that we have received from the capture device, and are
// waiting to be queued on the sample.
static U32 queue_count, queue_alloc;
static HMILESBUFFERREFERENCE* queue_array;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static HMILESBUFFERREFERENCE dequeue_buffer(U32* ptr_count, HMILESBUFFERREFERENCE* array)
{
    // for demonstration purposes only - dequeue function to avoid depending on c++
    // stdlib
    
    if (*ptr_count == 0)
        return 0;
    
    HMILESBUFFERREFERENCE ret = array[0];
    memcpy(array, array + 1, (*ptr_count - 1)*sizeof(HMILESBUFFERREFERENCE));
    ptr_count[0]--;
    return ret;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void enqueue_buffer(U32* ptr_count, U32* ptr_alloc, HMILESBUFFERREFERENCE** ptr_array, HMILESBUFFERREFERENCE buffer)
{
    // for demonstration purposes only - enqueue function to avoid depending on c++
    // stdlib
    
    if (*ptr_count + 1 > *ptr_alloc)
    {
        U32 new_count = *ptr_alloc;
        new_count *= 2;
        if (new_count < 4)
            new_count = 4;
        HMILESBUFFERREFERENCE* new_array = (HMILESBUFFERREFERENCE*)MilesAlloc(sizeof(HMILESBUFFERREFERENCE) * new_count);
        memcpy(new_array, *ptr_array, sizeof(HMILESBUFFERREFERENCE) * ptr_count[0]);
        MilesFree(*ptr_array);
        *ptr_array = new_array;
        *ptr_alloc = new_count;
    }
    
    (*ptr_array)[*ptr_count] = buffer;
    ptr_count[0]++;
}

static short last_max_signal = 0;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void FillSample(HMILESSAMPLE Sample)
{
    U32 ToFill = MilesMaxSampleBuffers - MilesSampleGetBufferCount(Sample);
    if (ToFill == 0)
        return;

    U16 MaxSignal = 0;
    for (U32 i = 0; i < ToFill; i++)    
    {
        HMILESBUFFERREFERENCE next = dequeue_buffer(&queue_count, queue_array);
        if (next == 0)
            break;
        
        enqueue_buffer(&outstanding_buffers_count, &outstanding_buffers_alloc, &outstanding_buffers_array, next);

        U32 ByteCount = 0;
        void* Bytes = MilesBufferReferenceGetData(next, &ByteCount);


        short* Samples = (short*)Bytes;
        for (U32 i = 0; i < ByteCount/sizeof(short); i++)
        {
            if (abs(Samples[i]) > MaxSignal)
                MaxSignal = abs(Samples[i]);
        }

        MilesSampleAddBuffer(Sample, Bytes, ByteCount);
    }

    last_max_signal = MaxSignal;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void RADLINK DoneBuffer(HMILESSAMPLE Sample, void* buffer, void* user)
{
    MilesBufferReferenceRelease(dequeue_buffer(&outstanding_buffers_count, outstanding_buffers_array));

    // Fill up to the limit.
    FillSample(Sample);
}


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
    
    //
    // Initialize core Miles system
    //
    MilesStartup(MilesErrorFn);

    //
    // Open the desired capture device.
    //
    HMILESCAPTURE Capture = MilesCaptureCreate((void*)"plughw:1,0");

    //
    // Get the format the capture opened at.
    //
    U32 Rate;
    MilesChannelSpec Chans;
    MilesCaptureGetFormat(Capture, &Rate, &Chans, 0);

    //
    // Open the desired output device.
    //
    U32 DefRate;
    MilesChannelSpec DefSpec;
    MilesDriverParameters Params = {0};
    Params.HardwareOutput = MilesOutputGetDefault(&DefRate, &DefSpec);
    Params.HardwareOutputParam = (UINTa)"plughw:1,0";

    HMILESDRIVER Driver = MilesDriverCreate(&Params);

    //
    // Create a sample to play the captured data.
    //
    HMILESSAMPLE Sample = MilesSampleCreate(Driver, 0, 0);

    //
    // Set up the sample format to the data type we get from the capture
    // device.
    //
    MilesSampleSetSourceRaw(Sample, 0, 0, Rate, (U16)Chans);

    // Tell miles that once a buffer is played, discard it.    
    MilesSampleSetSourceDiscards(Sample);

    // Set up the callback that tells us when a buffer is complete so we can
    // free it.
    MilesSampleSetEOBCallback(Sample, DoneBuffer, 0);

    //
    // Add a low pass filter to the sample
    //
    HMILESFILTERSTAGE Filter = MilesSampleAddFilterByName(Sample, MilesFilterNameLowPass);
    MilesFilterSetPropertyValueByName(Filter, "Cutoff", 800);

    short last_max_recv_signal = 0;

    bool has_started = false;
    for (;;)
    {
        for (;;)
        {
            HMILESBUFFERREFERENCE Buffer = MilesCaptureDequeue(Capture);
            if (Buffer)
            {
                MilesLock();
                enqueue_buffer(&queue_count, &queue_alloc, &queue_array, Buffer);

                U32 ByteCount = 0;
                void* Bytes = MilesBufferReferenceGetData(Buffer, &ByteCount);

                short MaxSignal = 0;
                short* Samples = (short*)Bytes;
                for (U32 i = 0; i < ByteCount/sizeof(short); i++)
                {
                    if (abs(Samples[i]) > MaxSignal)
                        MaxSignal = abs(Samples[i]);
                }
                last_max_recv_signal = MaxSignal;

                
                // Wait to start the sample until we have a backlog to cover any
                // drops or batching.            
                if (has_started == false && queue_count >= backlog)
                {
                    has_started = true;
                    FillSample(Sample);
                    MilesSamplePlay(Sample);
                }
                MilesUnlock();
            }
            else
                break;
        }

        if (has_started && MilesSampleGetBufferCount(Sample) == 0)
        {
            // We've starved - restart when buffers are available.
            MilesLock();         
            if (queue_count >= backlog)
            {
                FillSample(Sample);
                MilesSamplePlay(Sample);
            }
            MilesUnlock();
        }

        MilesSleep(10);

        MilesConsoleHandleEvents();

        {
            //
            // A bunch of code to print out the signal levels in a fancy way.
            //
            // The 'Capture' level is the signal we just got from Miles.
            // The 'Playback' level is the signal we just sent to the sample
            //
            // These are separated by latency based on the backlog we started with
            //
            float level_playback = MilesLevelToLinear(last_max_signal / 32767.0f);
            float level_capture = MilesLevelToLinear(last_max_recv_signal / 32767.0f);

            static char meter[] = "-----++++#";
            char meter_playback[sizeof(meter)];
            char meter_capture[sizeof(meter)];

            if (level_playback <= 1)
            {
                int l = (int)(level_playback * (sizeof(meter) - 1) + .5f);
                for (int i = 0; i < sizeof(meter) - 1; i++)
                {
                    if (i < l)
                        meter_playback[i] = meter[i];
                    else
                        meter_playback[i] = ' ';
                }
                meter_playback[sizeof(meter) - 1] = 0;
            }
            if (level_capture <= 1)
            {
                int l = (int)(level_capture * (sizeof(meter) - 1) + .5f);
                for (int i = 0; i < sizeof(meter) - 1; i++)
                {
                    if (i < l)
                        meter_capture[i] = meter[i];
                    else
                        meter_capture[i] = ' ';
                }
                meter_capture[sizeof(meter) - 1] = 0;
            }
            
            printf("Signals: Capture: [%s] Playback [%s]\r", meter_capture, meter_playback);
        }

        if (MilesCaptureGetStatus(Capture) == MilesCaptureStatusBroken)
            break;

        if (_kbhit())
        {
            int key = _getch();
            if (key == 27) // escape
                break;
            else if (key == ' ')
            {
                U32 IsPaused = MilesCaptureGetPaused(Capture);
                if (IsPaused)
                    has_started = false; // refill the sample
                MilesCaptureSetPaused(Capture, !IsPaused);
            }
        }
    }

    // Free everything
    MilesFree(queue_array);
    MilesFree(outstanding_buffers_array);
    MilesShutdown();
    return 0;
}


// @cdep pre $Defaults
// @cdep post $Build
