//-----------------------------------------------------------------------------
//
// To Use:
//
// CaptureFile File = {0};
// File.FileName = "path/to/wav/file";
//
// MilesOutputSetCaptureFn(MilesDriverGetOutput(Driver, 0), CaptureToFile, &File);
//
//-----------------------------------------------------------------------------

#include <memory.h>
#include <stdio.h>

#ifndef CAPTURE_WAVOUT_DEFINED
#define CAPTURE_WAVOUT_DEFINED
typedef struct _WAVEOUT {
  U32 riffmark;
  U32 rifflen;
  U32 wavemark;
  U32 fmtmark;
  U32 fmtlen;
  U16 fmttag;
  U16 channels;
  U32 sampersec;
  U32 avepersec;
  U16 blockalign;
  U16 bitspersam;
  U32 datamark;
  U32 datalen;
} WAVEOUT;
#endif

#ifndef CAPTURE_SETUP_DEFINED
#define CAPTURE_SETUP_DEFINED
typedef struct _CaptureFile
{
    char const* FileName;
    U32 TotalBytes;
    U32 LimitToSamples;
    FILE* OutputFile;
    WAVEOUT wo;
} CaptureFile;

#endif

void RADLINK CaptureToFile(MilesOutput* output, float* samples, void* user)
{
    U32 i = 0;
    CaptureFile* state = (CaptureFile*)user;

    if (state->OutputFile == 0)
    {
        state->OutputFile = fopen(state->FileName, "wb");

        memcpy(&state->wo.riffmark,"RIFF",4);
        memcpy(&state->wo.wavemark,"WAVE",4);
        memcpy(&state->wo.fmtmark,"fmt ",4);
        state->wo.fmtlen=16;
        state->wo.fmttag=1;
        state->wo.channels = (U16)output->input_channel_count;

        state->wo.sampersec=output->sample_rate;
        state->wo.bitspersam=16;
        state->wo.blockalign=(S16)(((S32)state->wo.bitspersam*(S32)state->wo.channels) / 8);
        state->wo.avepersec=(state->wo.sampersec*(S32)state->wo.bitspersam*(S32)state->wo.channels) / 8;
        memcpy(&state->wo.datamark,"data",4);

        fwrite(&state->wo, sizeof(WAVEOUT), 1, state->OutputFile);
    }

    if (samples == 0)
    {
        // closing the output.
        fclose(state->OutputFile);
        state->OutputFile = 0;
        return;
    }


    {
        U16 buffer[MilesFloatMixCount * MilesMaxOutputChannels];
        U32 UseSampleCount = MilesFloatMixCount;
        U16* write_ptr = buffer;

        // Cap samples if needed.
        if (state->LimitToSamples == ~0U)
            UseSampleCount = 0; // already output everything.
        else if (state->LimitToSamples)
        {
            if (UseSampleCount > state->LimitToSamples)
                UseSampleCount = state->LimitToSamples;
            state->LimitToSamples -= UseSampleCount;
            if (state->LimitToSamples == 0)
                state->LimitToSamples = ~0U; // finished - disable.
        }

        // Cvt to 16 bit
        for (i = 0; i < UseSampleCount; i++)
        {
            S32 c = 0;
            for (c = 0; c < output->input_channel_count; c++)
            {
                float s = samples[i + c*MilesFloatMixCount];
                if (s < -1)
                    s = -1;
                if (s > 1)
                    s = 1;
                s *= 32767;

                *write_ptr = (S16)s;
                write_ptr++;
            }
        }

        if (UseSampleCount)
        {
            fwrite(buffer, UseSampleCount * output->input_channel_count * sizeof(S16), 1, state->OutputFile);
            state->TotalBytes += output->input_channel_count * sizeof(S16) * UseSampleCount;

            state->wo.rifflen = state->TotalBytes + sizeof(WAVEOUT) - 8;
            state->wo.datalen = state->TotalBytes;
            fseek(state->OutputFile, 0, SEEK_SET);
            fwrite(&state->wo, sizeof(WAVEOUT), 1, state->OutputFile);
            fseek(state->OutputFile, 0, SEEK_END);

            fflush(state->OutputFile);
        }
    }
}
