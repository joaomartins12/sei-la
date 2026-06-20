//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined(_SEKRIT) || defined(WINAPI_FAMILY)
    #include <windows.h>
    #include <xaudio2.h>
#elif _XBOX
    #define NOD3D
    #include <xtl.h>
    #include <xaudio2.h>
#elif defined(WIN32)
    #include <windows.h>
    #include <xaudio2.h>
#endif

#include <new>

#include "rrCore.h"
#define OutMilesMutex()
#define InMilesMutex()

#include "rrTime.h"

#define oal_memset   memset
#define oal_memcpy   memcpy

#include "radss.h"


//#define LOGFILE
//#define OAL_DEBUG

#ifdef OAL_DEBUG
#include <stdio.h>

static void Log(char* i_fmt, ...)
{
    va_list list;
    va_start(list, i_fmt);

    char output[1024];

    vsprintf(output, i_fmt, list);

    va_end(list);

    printf(output);
    //OutputDebugStringA(output);
}

#define LOG Log
#else
#define LOG
#endif

extern "C"
{

#define RADSS_X2_MAXBUFFERS 24

struct CallerBacker : IXAudio2EngineCallback
{
    virtual void __stdcall OnCriticalError(HRESULT error);
    virtual void __stdcall OnProcessingPassEnd()     {}
    virtual void __stdcall OnProcessingPassStart()   {}
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct RadSS_X2Private
{
    IXAudio2* m_pAudio;
    IXAudio2SourceVoice* m_pSourceVoice;
    IXAudio2MasteringVoice* m_pMasterVoice;
    HMODULE m_DLLHandle;

    U32 m_BufferCount;
    U32 m_BufferSize;
    U32 m_MaxLockSize;
    U32 m_MinBufferSizeInMs;
    U32 m_BufferWriteCursor;
    U32 m_BufferIndex;
    U8 m_HasQueued;
    U8 m_NeedsRecreate;
    U64 m_LastNullTick;
    U32 m_NullConsumeTicks5ms;
    U32 m_LastLockSize;
    U32 m_NullQueued;

    CallerBacker cb;
};

void CallerBacker::OnCriticalError(HRESULT error)
{
    RadSS_X2Private* Driver = (RadSS_X2Private*)(((UINTa)this) - offsetof(RadSS_X2Private, cb));
    Driver->m_NeedsRecreate = 1;
}

// ensure our private struct fits in the space.
#ifdef _DEBUG
typedef char assert1[(sizeof(RadSS_X2Private) <= RADSS_PRIVATE_BYTE_COUNT) ? 1 : -1];
#endif

static S32 RADSS_X2CloseDriver(RadSoundSystem*);
static S32 RADSS_X2SetPaused(RadSoundSystem*, S32);
static S32 RADSS_X2SetOnOff(RadSoundSystem*, S32);
static S32 RADSS_X2GetReadyBytes(RadSoundSystem*);
static void RADSS_X2SetMem(RadSoundSystem*, void*);
static S32 RADSS_X2Lock(RadSoundSystem*, void**, U32);
static S32 RADSS_X2Unlock(RadSoundSystem*);
static UINTa RADSS_X2GetProperty(RadSoundSystem*, RADSS_PROPERTY, UINTa*);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static S32 RADSS_X2OpenDriver(RadSoundSystem* i_SoundSystem, U32 i_MinBufferSizeInMs, U32 i_Frequency,
    U32 i_ChannelCount, U32 i_MaxLockSize, UINTa 
    )
{
    //
    // Initialize the structure.
    //
    oal_memset(i_SoundSystem, 0, sizeof(RadSoundSystem));

    i_SoundSystem->Close = RADSS_X2CloseDriver;
    i_SoundSystem->GetReadyBytes = RADSS_X2GetReadyBytes;
    i_SoundSystem->SetMem = RADSS_X2SetMem;
    i_SoundSystem->SetOnOff = RADSS_X2SetOnOff;
    i_SoundSystem->SetPaused = RADSS_X2SetPaused;
    i_SoundSystem->Lock = RADSS_X2Lock;
    i_SoundSystem->Unlock = RADSS_X2Unlock;
    i_SoundSystem->GetSetProperty = RADSS_X2GetProperty;

    i_SoundSystem->m_ChannelCount = (U8)i_ChannelCount;
    i_SoundSystem->m_OnOff = 1;
    i_SoundSystem->m_OutputType = RADSS_OUTPUT_S16;
    i_SoundSystem->m_Paused = 1;
    i_SoundSystem->m_Skipped = 0;
    i_SoundSystem->m_Frequency = (U16)i_Frequency;

    RadSS_X2Private* pDriver =  (RadSS_X2Private*)i_SoundSystem->m_Private;
    pDriver->m_MaxLockSize = i_MaxLockSize;
    pDriver->m_HasQueued = 0;
    pDriver->m_MinBufferSizeInMs = i_MinBufferSizeInMs;

    U32 MsPerBuffer = i_MinBufferSizeInMs / RADSS_X2_MAXBUFFERS;
    if (MsPerBuffer * RADSS_X2_MAXBUFFERS < i_MinBufferSizeInMs) 
        MsPerBuffer++;
    if (MsPerBuffer < 8) 
        MsPerBuffer = 8;

    //
    // Figure out how big our heap buffer needs to be, based on the channel count,
    // and ms size for the buffers.
    //
    S32 BufferSize = (i_Frequency * MsPerBuffer) / 1000;

    // Align the buffer size, since we'll need to return pointers on size granularity.
    BufferSize = (BufferSize + 15) & (~15);

    // Ensure we are divisible by the bytes per channel.
    S32 BytesPerChannel = 2 * i_ChannelCount;
    BufferSize *= BytesPerChannel;

    pDriver->m_BufferSize = BufferSize;

    // Find out how many 16 ms buffers we need to allocate.
    S32 BufferCount = i_MinBufferSizeInMs / MsPerBuffer;
    if (BufferCount * MsPerBuffer < (S32)i_MinBufferSizeInMs) BufferCount++;

    // Cap at our buffer limit.
    if (BufferCount > RADSS_X2_MAXBUFFERS) 
    {
#ifdef _DEBUG
        RR_BREAK();
#endif
        BufferCount = RADSS_X2_MAXBUFFERS;
    }

    pDriver->m_BufferCount = BufferCount;

    // Request extra memory so we can always hold the lock size.
    i_SoundSystem->m_MemoryNeeded = BufferCount*BufferSize + i_MaxLockSize;

#ifdef __RADNT__
    CoInitializeEx(0, COINIT_MULTITHREADED);
#endif

    HRESULT hr = XAudio2Create(&pDriver->m_pAudio, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr))
    {
        i_SoundSystem->m_ErrorString = "Couldn't create XAudio2.";
        RADSS_X2CloseDriver(i_SoundSystem);
        return 0;
    }
    
    hr = pDriver->m_pAudio->CreateMasteringVoice(
        &pDriver->m_pMasterVoice, 
        i_ChannelCount,
        i_Frequency,
        0,
        0,
        0);

    if (FAILED(hr))
    {
        i_SoundSystem->m_ErrorString = "Couldn't create XAudio2 master voice.";
        RADSS_X2CloseDriver(i_SoundSystem);
        return 0;
    }

    WAVEFORMATEX fmt;
    fmt.cbSize = 0;
    fmt.nChannels = (WORD)i_ChannelCount;
    fmt.wBitsPerSample = 16;
    fmt.nSamplesPerSec = i_Frequency;
    fmt.nAvgBytesPerSec = i_Frequency * i_ChannelCount * 2;
    fmt.nBlockAlign = (WORD)(2 * i_ChannelCount);
    fmt.wFormatTag = WAVE_FORMAT_PCM;

    hr = pDriver->m_pAudio->CreateSourceVoice(
        &pDriver->m_pSourceVoice, 
        &fmt,
        XAUDIO2_VOICE_NOPITCH,
        1.0f,
        0,
        0,
        0);
    if (FAILED(hr))
    {
        i_SoundSystem->m_ErrorString = "Couldn't create XAudio2 source voice.";
        RADSS_X2CloseDriver(i_SoundSystem);
        return 0;
    }

#ifdef LOGFILE
    DeleteFile("d:\\oal.raw");
#endif

    // There's a bug on some versions of windows where the DLL gets
    // unloaded due to a reference counting bug in the system. So
    // we hold our own reference.
    // https://blogs.msdn.microsoft.com/chuckw/2015/10/09/known-issues-xaudio-2-7/
#ifdef __RADNT__    
    pDriver->m_DLLHandle = LoadLibraryEx("xaudio2_7.dll", 0,  0x00000800 /* LOAD_LIBRARY_SEARCH_SYSTEM32 */ );
#endif

    // Make sure we're told about any devices that go missing.
    new (&pDriver->cb) CallerBacker;
    pDriver->m_pAudio->RegisterForCallbacks(&pDriver->cb);

    return 1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void DoClose(RadSS_X2Private* Driver, bool IsRecreate)
{
    if (Driver->m_pSourceVoice)
        Driver->m_pSourceVoice->DestroyVoice();
    Driver->m_pSourceVoice = 0;
    if (Driver->m_pMasterVoice)
        Driver->m_pMasterVoice->DestroyVoice();
    Driver->m_pMasterVoice = 0;
#ifndef __RADWINRTAPI__
    if (IsRecreate == false)
        Sleep(10); // Sleep since xaudio apparently leaks occasionally if you release the audio too soon after the voice
#endif
    if (Driver->m_pAudio)
        Driver->m_pAudio->Release();
    Driver->m_pAudio = 0;

#ifdef __RADNT__
    if (Driver->m_DLLHandle)
        FreeLibrary(Driver->m_DLLHandle);
    Driver->m_DLLHandle = 0;
#endif

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static S32 RADSS_X2CloseDriver(RadSoundSystem* i_SoundSystem)
{
    RadSS_X2Private* pDriver = (RadSS_X2Private*)i_SoundSystem->m_Private;
    DoClose(pDriver, false);
    return 1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static S32 RADSS_X2Lock(RadSoundSystem* i_SoundSystem, void** io_ppBuffer, U32 i_SizeRequested)
{
    RadSS_X2Private* pDriver = (RadSS_X2Private*)i_SoundSystem->m_Private;

    if (i_SizeRequested > pDriver->m_MaxLockSize)
    {
        // Nope!
        i_SoundSystem->m_ErrorString = "Invalid lock size.";
#ifdef _DEBUG
        RR_BREAK();
#endif
        return 0;
    }

    if (pDriver->m_pAudio == 0)
    {
        // For null consumption, just always copy to the front.
        *io_ppBuffer = (U8*)i_SoundSystem->m_pMemory;
        return 1;
    }

    LOG("Locked offset %d - %d\n", pDriver->m_BufferWriteCursor, pDriver->m_BufferWriteCursor + i_SizeRequested);

    // Return the current buffer location.
    *io_ppBuffer = (U8*)i_SoundSystem->m_pMemory + pDriver->m_BufferWriteCursor;

    // Update the cursor to the post-lock location.
    pDriver->m_BufferWriteCursor += i_SizeRequested;
    pDriver->m_LastLockSize = i_SizeRequested;

    return 1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static S32 RADSS_X2Unlock(RadSoundSystem* i_SoundSystem)
{
    RadSS_X2Private* pDriver = (RadSS_X2Private*)i_SoundSystem->m_Private;
    if (pDriver->m_pAudio == 0)
    {
        // We are a null device and are just arbitrarily consuming.
        pDriver->m_NullQueued += pDriver->m_LastLockSize;
        pDriver->m_HasQueued = 1;
        return 1;
    }

    XAUDIO2_VOICE_STATE State;
    pDriver->m_pSourceVoice->GetState(&State);

    if (pDriver->m_HasQueued && State.BuffersQueued == 0)
    {
        i_SoundSystem->m_Skipped ++;
    }

    S32 FreeCount = pDriver->m_BufferCount - State.BuffersQueued;
    if (FreeCount == 0)
    {
        // No room on the hardware. If we made it here, they didn't listen to GetReadyBytes.
        i_SoundSystem->m_ErrorString = "Invalid lock - check GetReadyBytes().";
#ifdef _DEBUG
        RR_BREAK();
#endif
        return 0;
    }

    U32 ReadyBytes = pDriver->m_BufferWriteCursor - pDriver->m_BufferSize * pDriver->m_BufferIndex;
    LOG("%08d ReadyBytes %d...", pDriver->m_BufferWriteCursor, ReadyBytes);

    // See if we got enough to submit to the hw.
    if (ReadyBytes >= pDriver->m_BufferSize)
    {
        // Assert that the amount we have is mod samplesize.
        if (pDriver->m_BufferWriteCursor % (i_SoundSystem->m_ChannelCount * 2) != 0)
        {
            i_SoundSystem->m_ErrorString = "Invalid byte count submitted to hw layer.";
#ifdef _DEBUG
            RR_BREAK();
#endif
            return 0;
        }

        // The last buffer needs to submit the tail, everything else should cap on buffer lines.
        U32 SubmitBytes = ReadyBytes;
        if (pDriver->m_BufferIndex != pDriver->m_BufferCount - 1)
        {
            SubmitBytes = pDriver->m_BufferSize;
        }
            
        LOG("%d submitted.\n", SubmitBytes);

#ifdef LOGFILE
        HANDLE TestHandle = CreateFile("d:\\oal.raw", GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        SetFilePointer(TestHandle, 0, 0, FILE_END);

        DWORD WriteCount = 0;
        WriteFile(TestHandle, (U8*)i_SoundSystem->m_pMemory + pDriver->m_BufferIndex*pDriver->m_BufferSize, SubmitBytes, &WriteCount, 0);

        CloseHandle(TestHandle);
#endif

        // Submit the buffer to the card.
        XAUDIO2_BUFFER SubmitBuffer = {0};
        SubmitBuffer.AudioBytes = SubmitBytes;
        SubmitBuffer.pAudioData = (U8*)i_SoundSystem->m_pMemory + pDriver->m_BufferIndex*pDriver->m_BufferSize;

        HRESULT hr = pDriver->m_pSourceVoice->SubmitSourceBuffer(&SubmitBuffer);
        if (FAILED(hr))
        {
            i_SoundSystem->m_ErrorString = "Failed to submit to voice.";
        }

        // Increment our working buffer.
        pDriver->m_BufferIndex ++;
        if (pDriver->m_BufferIndex >= pDriver->m_BufferCount) 
        {
            pDriver->m_BufferIndex = 0;
            pDriver->m_BufferWriteCursor = 0;
        }

        pDriver->m_HasQueued = 1;
    }
    else
    {
        LOG("\n");
    }

    return 1;
} // end Unlock.

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static S32 RADSS_X2SetPaused(RadSoundSystem* i_SoundSystem, S32 i_Paused)
{
    RadSS_X2Private* pDriver = (RadSS_X2Private*)i_SoundSystem->m_Private;
    if (i_Paused != i_SoundSystem->m_Paused)
    {
        if (pDriver->m_pAudio)
        {
            if (i_Paused)
                pDriver->m_pSourceVoice->Stop(0, 0);
            else
                pDriver->m_pSourceVoice->Start(0, XAUDIO2_COMMIT_NOW);
        }
        i_SoundSystem->m_Paused = (U8)i_Paused;
    }
    return 1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static S32 RADSS_X2SetOnOff(RadSoundSystem* i_SoundSystem, S32 i_On)
{
    RadSS_X2Private* Driver = (RadSS_X2Private*)i_SoundSystem->m_Private;
    if (i_On == 0)
    {
        //
        // Free all of the openal stuff, but keep around enough info
        // to reinit the device if we have to.
        //
        DoClose(Driver, true);
    }
    else
    {
        void* pBuffer = i_SoundSystem->m_pMemory;
        S32 OpenSuccess = RADSS_X2OpenDriver(
            i_SoundSystem, 
            Driver->m_MinBufferSizeInMs,
            i_SoundSystem->m_Frequency,
            i_SoundSystem->m_ChannelCount,
            Driver->m_MaxLockSize,
            0);

        // Okay - this is before the failure because we need to keep
        // the buffer when we recreate due to a device disappearing.
        RADSS_X2SetMem(i_SoundSystem, pBuffer);
        if (!OpenSuccess) 
            return 0;        
    }

    return 1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static S32 RADSS_X2GetReadyBytes(RadSoundSystem* i_SoundSystem)
{
    RadSS_X2Private* pDriver = (RadSS_X2Private*)i_SoundSystem->m_Private;

    if (i_SoundSystem->m_OnOff == 0) 
        return 0;

    if (pDriver->m_NeedsRecreate)
    {
        RADSS_X2SetOnOff(i_SoundSystem, 0);
        if (RADSS_X2SetOnOff(i_SoundSystem, 1) == 0)
        {
            // We drop to a null consumer.
            pDriver->m_LastNullTick = rrGetTicks();
            pDriver->m_NullConsumeTicks5ms = (U32)rrMillisToTime(5);
        }
        RADSS_X2SetPaused(i_SoundSystem, 0);
    }

    if (pDriver->m_pAudio == 0)
    {
        // We are a null device. Determine how much we are supposed to have.
        U64 check_time = rrGetTime();
        if (check_time - pDriver->m_LastNullTick > pDriver->m_NullConsumeTicks5ms)
        {
            if (i_SoundSystem->m_Paused == 0)
            {
                // eat the amount for the time.
                double chunk_count = ((check_time - pDriver->m_LastNullTick) / (double)pDriver->m_NullConsumeTicks5ms);

                U32 frames_to_consume = (U32)((chunk_count * 5) * i_SoundSystem->m_Frequency / 1000.0f);
                U32 bytes_to_eat = sizeof(short) * i_SoundSystem->m_ChannelCount * frames_to_consume;

                if (pDriver->m_NullQueued < bytes_to_eat)
                {
                    i_SoundSystem->m_Skipped++;
                    pDriver->m_NullQueued = 0;
                }
                else
                    pDriver->m_NullQueued -= bytes_to_eat;
            }

            pDriver->m_LastNullTick = check_time;
        }

        return pDriver->m_BufferCount * pDriver->m_BufferSize - pDriver->m_NullQueued;
    }

    XAUDIO2_VOICE_STATE State = {0};

#ifndef WIN32
    pDriver->m_pSourceVoice->GetState(&State, XAUDIO2_VOICE_NOSAMPLESPLAYED);
#elif defined(WIN32)
    pDriver->m_pSourceVoice->GetState(&State);
#else
#error (missing case)
#endif

    return (pDriver->m_BufferCount - State.BuffersQueued) * pDriver->m_BufferSize;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void RADSS_X2SetMem(RadSoundSystem* i_SoundSystem, void* i_Buffer)
{
    i_SoundSystem->m_pMemory = i_Buffer;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static
UINTa
RADSS_X2GetProperty(RadSoundSystem* i_SoundSystem, RADSS_PROPERTY i_Property, UINTa* i_NewValue)
{
    RadSS_X2Private* pDriver = (RadSS_X2Private*)i_SoundSystem->m_Private;
    switch (i_Property)
    {
    case RADSS_PROP_TYPE: return RADSS_XAUDIO2;
    case RADSS_PROP_TYPESTRING: return (UINTa)"XAudio2";
    case RADSS_PROP_X2_IXAUDIO: return (UINTa)pDriver->m_pAudio;
    case RADSS_PROP_X2_MASTERVOICE: return (UINTa)pDriver->m_pMasterVoice;
    case RADSS_PROP_X2_SRCVOICE: return (UINTa)pDriver->m_pSourceVoice;
    }
    return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
RADDEFFUNC RADSS_OPEN_FUNC RADLINK RADSS_XAudio2InstallDriver(UINTa, UINTa)
{
    return RADSS_X2OpenDriver;
}

} // end extern "C"
