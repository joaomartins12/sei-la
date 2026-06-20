// Copyright 2008-2013 RAD Game Tools

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef RADSS_H
#define RADSS_H

#ifndef __RADRR_CORE2H__
    #include "rrCore.h"
#endif

struct _RadSoundSystem;

typedef S32 (*RADSS_OPEN_FUNC)(struct _RadSoundSystem* i_SoundSystem, U32 i_MinBufferSizeInMs, U32 i_Frequency, U32 i_ChannelCount, U32 i_MaxLockSize, UINTa i_FlagsOrParams);
typedef void (*RADSS_SETMEM_FUNC)(struct _RadSoundSystem* i_SoundSystem, void* i_Buffer);
typedef S32 (*RADSS_GETREADYBYTES_FUNC)(struct _RadSoundSystem* i_SoundSystem);
typedef S32 (*RADSS_SETPAUSE_FUNC)(struct _RadSoundSystem* i_SoundSystem, S32 i_Paused);
typedef S32 (*RADSS_SETONOFF_FUNC)(struct _RadSoundSystem* i_SoundSystem, S32 i_On);
typedef S32 (*RADSS_CLOSE_FUNC)(struct _RadSoundSystem* i_SoundSystem);
typedef S32 (*RADSS_LOCK_FUNC)(struct _RadSoundSystem* i_SoundSystem, void** io_ppBuffer, U32 i_Bytes);
typedef S32 (*RADSS_UNLOCK_FUNC)(struct _RadSoundSystem* i_SoundSystem);

//function is optional
typedef void (*RADSS_VOLUME_FUNC)(struct _RadSoundSystem* i_SoundSystem, F32 lvol, F32 rvol);

// i_ChannelVols: [ in_l_to_fl, in_r_to_fl, in_l_to_fr, in_r_to_fr, in_l_to_c, in_r_to_c, etc]
typedef void (*RADSS_CHANNELROUTE_FUNC)(struct _RadSoundSystem* i_SoundSystem, S16* i_ChannelVols, S32 Count);

typedef RADSS_OPEN_FUNC (RADLINK *RADSS_INSTALL_FUNC)(UINTa, UINTa);

//-----------------------------------------------------------------------------
typedef enum
{
    //! Returns the type of the underlying sound system (RADSS_TYPE), and sets the base of the enum.
    RADSS_PROP_TYPE = 5000,
    RADSS_PROP_TYPESTRING,

    // OAL properties.
    RADSS_PROP_OAL_DEVICE,
    RADSS_PROP_OAL_CONTEXT,

    // XAudio2 properties.
    RADSS_PROP_X2_IXAUDIO,
    RADSS_PROP_X2_SRCVOICE,
    RADSS_PROP_X2_MASTERVOICE,

    // DSound properties.
    RADSS_PROP_DS_DS,
    RADSS_PROP_DS_PRIMARYBUFFER,
    RADSS_PROP_DS_HWND,
    RADSS_PROP_DS_EMULATED,

    // PSP properties
    RADSS_PROP_PSP_SUBMITTHREAD,
    RADSS_PROP_PSP_AUDIOPORT,       // -1 for simple audio, >= 0 for libwave
    
    // PSP2 properties
    RADSS_PROP_PSP2_SUBMITTHREAD,
    RADSS_PROP_PSP2_AUDIOPORT,

    // PS4 properties
    RADSS_PROP_PS4_SUBMITTHREAD,

    // LibAudio properties.
    RADSS_PROP_LIBAUDIO_PORTNUM,

    // iOS audio session poke. Call when your Audio Session reports a session interruption has ended.
    RADSS_PROP_IOS_SESSION_RESUME,

    // Mss properties
    RADSS_PROP_MSS_SAMPLE,

    // Memory writer property - currently only in Miles10 as it wasn't written to handle bink usage.
    RADSS_PROP_MEMORY_CONSUMING,
} RADSS_PROPERTY;

//-----------------------------------------------------------------------------
//! Type defining the underlying implementation of the sound system.
typedef enum
{
    RADSS_OAL,
    RADSS_XAUDIO2,
    RADSS_WAVEOUT,
    RADSS_DSOUND,
    RADSS_PSP,
    RADSS_COREAUDIO,
    RADSS_LIBAUDIO,
    RADSS_3DS,
    RADSS_AX, // Only Wii now, or old WiiU API.
    RADSS_WIIU, // Update WiiU output, with DRC
    RADSS_PSP2,
    RADSS_MSS,
    RADSS_SLES,
    RADSS_PS4,
    RADSS_WASAPI,
    RADSS_NULL,
    RADSS_MEMORY,
    RADSS_NX
} RADSS_TYPE;

/*!
    Property types are defined on a per sound system basis.
*/
typedef UINTa (*RADSS_PROPERTY_FUNC)(struct _RadSoundSystem* i_SoundSystem, RADSS_PROPERTY i_Property, UINTa* i_NewValue);

//! List of possible datatypes the sound systems wants.
#define RADSS_OUTPUT_S16 0
#define RADSS_OUTPUT_FLOAT 1

//! Number of bytes the individual systems get for private data.
#define RADSS_PRIVATE_BYTE_COUNT 128

#ifdef __cplusplus
extern "C"
{
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef struct _RadSoundSystem
{
    //! Will be set to an error string if a function fails. Don't delete...
    const char* m_ErrorString;
    RADSS_SETMEM_FUNC SetMem;
    RADSS_GETREADYBYTES_FUNC GetReadyBytes;
    RADSS_SETPAUSE_FUNC SetPaused;

    RADSS_SETONOFF_FUNC SetOnOff;
    RADSS_CLOSE_FUNC Close;
    RADSS_LOCK_FUNC Lock;
    RADSS_UNLOCK_FUNC Unlock;

    RADSS_PROPERTY_FUNC GetSetProperty;
    RADSS_VOLUME_FUNC Volume;
    RADSS_CHANNELROUTE_FUNC ChannelRoute;

    //! Buffer set by caller via setmem() function, or zero.
    void* m_pMemory;

    //! Set to nonzero if the sound system needs heap memory allocated from the caller.
    U32 m_MemoryNeeded;

    //! RADSS_OUTPUT_FLOAT, RADSS_OUTPUT_S16...
    U8 m_OutputType;

    U8 m_ChannelCount;
    U8 m_Paused;
    U8 m_OnOff;
    U32 m_Frequency;

    S16 m_Skipped;

    //! Data for use by the individual sound systems.
    U64 m_Private[(RADSS_PRIVATE_BYTE_COUNT+7)/8]; // Enforce 64 bit alignment
} RadSoundSystem;

// This typedef assures we can fit in the BNKSND private space
#ifdef _DEBUG
typedef char assert_for_bink[(sizeof(RadSoundSystem) <= 256) ? 1 : -1];
#endif

#define RADSS_VERSION 1
#define RADSS_VER(name) RR_STRING_JOIN(name, RADSS_VERSION)

//
// The different output layers defined by radss.
//
// The relevant source files will need to be added.
//
#if defined(__RADNT__)

    //
    // DirectSound driver can take in an lpguid for a specific
    // device to use, or an HWND.
    //
    // Alternatively, the open() fn can take as its params/flags a UINTa[2] 
    // containing the lpguid and hwnd to use.
    //

    #define RADSS_DSInstallDriver RADSS_VER(RADSS_DSInstallDriver)
    RADDEFFUNC RADSS_OPEN_FUNC RADLINK RADSS_DSInstallDriver(UINTa optional_device_lpguid, UINTa optional_hwnd);

    #define RADSS_WOInstallDriver RADSS_VER(RADSS_WOInstallDriver)
    RADDEFFUNC RADSS_OPEN_FUNC RADLINK RADSS_WOInstallDriver(UINTa, UINTa);

    #define RADSS_HAS_XAUDIO2

#elif defined(__RADANDROID__)

    //
    // Can take in an object pointer for the engine and the output mix to use.
    //
    #define RADSS_SLESInstallDriver RADSS_VER(RADSS_SLESInstallDriver)
    RADDEFFUNC RADSS_OPEN_FUNC RADLINK RADSS_SLESInstallDriver(UINTa optional_engine_ptr, UINTa optional_mix_ptr);

#elif defined(__RADPSP2__)

    #define RADSS_PSP2InstallDriver RADSS_VER(RADSS_PSP2InstallDriver)
    RADDEFFUNC RADSS_OPEN_FUNC RADLINK RADSS_PSP2InstallDriver(UINTa, UINTa);

#elif defined(__RADPS3__)

    #define RADSS_LibAudioInstallDriver RADSS_VER(RADSS_LibAudioInstallDriver)
    RADEXPFUNC RADSS_OPEN_FUNC RADEXPLINK RADSS_LibAudioInstallDriver(UINTa, UINTa);

#elif defined(__RADPS4__)

    #define RADSS_SonyInstallDriver RADSS_VER(RADSS_SonyInstallDriver)
    RADDEFFUNC RADSS_OPEN_FUNC RADLINK RADSS_SonyInstallDriver(UINTa, UINTa);

#elif defined(__RADXENON__) || defined(__RADWINRTAPI__)

    #define RADSS_HAS_XAUDIO2

#elif defined(__RADNX__)

    #define RADSS_NXInstallDriver RADSS_VER(RADSS_NXInstallDriver)
    RADDEFFUNC RADSS_OPEN_FUNC RADLINK RADSS_NXInstallDriver(UINTa, UINTa);

#elif defined(__RADWIIU__)

    //
    // The WiiU device can open in a couple modes:
    // 

    #define AX_INIT_MODE_TV 0  
        // Standard output to the TV

    #define AX_INIT_MODE_DRC 1  
        // Output to the DRC speakers, must be stereo/mono

    #define AX_INIT_MODE_REMOTE 2 
        // Haven't actually tested this - to the little remote thing.

    #define AX_INIT_MODE_DRC_TV 3 
        // Output to both the TV and the DRC. The last 2 channels will be sent to the DRC.
        // IIRC this can only be 4 or 8 channels (stereo + DRC or 5.1 + DRC)

    // The mode defaults to AX_INIT_MODE_TV, and can be specified via the Install function
    // or the open() flags/params.
    #define RADSS_AXInstallDriver RADSS_VER(RADSS_AXInstallDriver)
    RADDEFFUNC RADSS_OPEN_FUNC RADLINK RADSS_AXInstallDriver(UINTa optional_mode, UINTa);

#elif defined(__RADMAC__) || defined(__RADIPHONE__)

    #define RADSS_CAInstallDriver RADSS_VER(RADSS_CAInstallDriver)
    RADDEFFUNC RADSS_OPEN_FUNC RADLINK RADSS_CAInstallDriver(UINTa, UINTa);

#elif defined(__RADLINUX__)

    #define RADSS_HAS_OPENAL

#else
    #error No outputs defined for this platform!
#endif

#if defined(RADSS_HAS_OPENAL)
    // OpenAL driver can optionally take in a ALCContext* to use as the first param.
    #define RADSS_OalInstallDriver RADSS_VER(RADSS_OalInstallDriver)
    RADDEFFUNC RADSS_OPEN_FUNC RADLINK RADSS_OalInstallDriver(UINTa optional_shared_context, UINTa);
#endif


#if defined(RADSS_HAS_XAUDIO2)

    #define RADSS_XAudio2InstallDriver RADSS_VER(RADSS_XAudio2InstallDriver)
    RADDEFFUNC RADSS_OPEN_FUNC RADLINK RADSS_XAudio2InstallDriver(UINTa, UINTa);

#endif


#if defined(__RADWIN__)

    #define RADSS_WASAPIInstallDriver RADSS_VER(RADSS_WASAPIInstallDriver)
    RADDEFFUNC RADSS_OPEN_FUNC RADLINK RADSS_WASAPIInstallDriver(UINTa, UINTa);

#endif

//
// Layer for outputing to Miles 9
//
// Install takes in the Miles 9 HDIGDRIVER to output to.
#define RADSS_BusInstallDriver RADSS_VER(RADSS_BusInstallDriver)
RADDEFFUNC RADSS_OPEN_FUNC RADLINK RADSS_BusInstallDriver(UINTa hdigdriver, UINTa);


//
// Layer for outputing to null
//
#define RADSS_NullInstallDriver RADSS_VER(RADSS_NullInstallDriver)
RADDEFFUNC RADSS_OPEN_FUNC RADLINK RADSS_NullInstallDriver(UINTa, UINTa);


#ifdef __cplusplus
} // end extern c
#endif
#endif // end ifn RADSS_H
