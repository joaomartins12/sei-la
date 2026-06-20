//-----------------------------------------------------------------------------
//
// Miles Sound System
//
// Copyright (C) 2014-2018, RAD Game Tools, Inc.
//
// Technical Support at miles3@radgametools.com
//
//-----------------------------------------------------------------------------
#define IDOC

#ifndef MilesVersionMajor
#   define MilesVersionMajor        10
#   define MilesVersionMinor        0               // Incremented with feature releases
#   define MilesVersionRelease      33              // Incremented with bug and minor features
#   define MilesVersionPatch        8               // Incremented with bug fixes to previous releases
#   define MilesVersionString       "10.0.33.8"      IDOC
#   define MilesCopyright           "Copyright (C) 2014-2018, RAD Game Tools, Inc." IDOC
#   define MilesVersionDate         "Dec 2, 2018"
#endif

// Setup structure alignment macros, since not in radtypes.h
#if defined(__GNUC__)
#define RR_ALIGNED_STRUCT(bytes) __attribute__ ((aligned(bytes)))
#else
// clang and vc are the same..
#define RR_ALIGNED_STRUCT(bytes) __declspec(align(bytes))
#endif

#ifndef __MILES_H__
#define __MILES_H__

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef __RADTYPESH__
    #include "radtypes.h"
#endif

#define MILES_NO_LOCK
#define MILES_NO_THUNK

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//idoc(parent, SampleAPI)
IDOC typedef enum MilesSampleStatus
{
    MilesSampleStatusCreated,    /*
        Sample has been created, but no source has been set and metadata hasn't been
        discovered.
    */

    MilesSampleStatusPlaying,    /*
        Sample is currently supplying samples to all outputs.
    */

    MilesSampleStatusPaused,    /*
        Sample has a source set, and all metadata has been parsed, but the sample
        is currently not supplying samples to outputs.
    */

    MilesSampleStatusDone,    /*
        Sample's sources have exhausted and all loops are complete.
    */

    MilesSampleStatusErrorMask = 0x80,    /*
        If the sample has encountered an error and must be recreated, the
        return from $MilesSampleGetStatus will return a value with this bit
        set. Generally, the payload of the error is irrelevant as the code
        is not capable of recovering dynamically.

        (These error codes MUST fit in a U8)
    */

    MilesSampleStatusErrorGeneric = 0x80, /*
        The error used when there is no useful payload for the error. Check
        the report function for details on the error during development. In retail
        builds this is generally a fatal error.
    */

    MilesSampleStatusErrorStreamFileNotFound = 0x81, /*
        If a stream is opened and the file fails to open, this will be the return
        from $MilesSampleGetStatus. 
    */

    MilesSampleStatusFORCE32 = 0x80000000
} MilesSampleStatus;
RR_COMPILER_ASSERT(sizeof(MilesSampleStatus) == 4);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef enum MilesSampleSourceType
{
    MilesSampleSourceSingleBuffer,  // The sample's source data is entirely in a single chunk of memory.
    MilesSampleSourceMultiBuffer,   /* 
        The sample's source data is spread across multiple buffers, but each buffer
        remains resident and can be seeked to and replayed.
    */

    MilesSampleSourceDiscards,       /*
        The sample's source data is supplied externally, and not necessarily resident
        once played. Miles will discard any buffers once completed, and any
        looping or seeking requires an additional callback to the game.
    */

    MilesSampleSourceStreamed,      /*
        The samples source is a filename that Miles will on-demand stream data packets from.
        Very similar to MilesSampleSourceDiscard, except Miles is also the data
        supplier.
    */

    MilesSampleSourceDemand,        /*
        The source data is on-demand via the high level event system.
    */
    MilesSampleSourceFORCE32 = 0x80000000
} MilesSampleSourceType;
RR_COMPILER_ASSERT(sizeof(MilesSampleSourceType) == 4);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef enum MilesSampleDataType
{
    MilesSampleDataUncompressed,    // The sample's source data is raw PCM.
    MilesSampleDataAdpcm,           // The sample's source data is IMA ADPCM.
    MilesSampleDataEncoded,         // The sample's source data is encdoded and requires a decoder
    MilesSampleDataFORCE32 = 0x80000000
} MilesSampleDataType;
RR_COMPILER_ASSERT(sizeof(MilesSampleDataType) == 4);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IDOC typedef enum MilesFileType
{
    MilesFileTypeRaw,
    MilesFileTypeWav,

    // order matters here - matches list of decoders in driver and miles studio compressor
    MilesFileTypeBink,
    MilesFileTypeMP3,
    MilesFileTypeOGG,
    MilesFileTypeAuto = 0x40,
    MilesFileTypeFORCE32 = 0x80000000
} MilesFileType;
RR_COMPILER_ASSERT(sizeof(MilesFileType) == 4);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//idoc(parent, CoreAPI)

// Tangent types for graphs. The values must not exceed 255.
// Adding to this requires updating string table in Tab_Event.cpp:GuiMCurveCombo
#define MilesTangentLinear          0
#define MilesTangentCurved          1
#define MilesTangentStep            2
#define MilesTangentQuadratic       3
#define MilesTangentInvQuadratic    (3 | 0x10)
#define MilesTangentFadeIn          4
#define MilesTangentFadeOut         (4 | 0x10)
#define MilesTangentCrossFadeIn     5
#define MilesTangentCrossFadeOut    (5 | 0x10)
#define MilesTangentCubic           6
#define MilesTangentInvCubic        (6 | 0x10)


#define MilesNumDecoders            3
#define MilesMaxOutputChannels      8      // The maximum number of channels the output hardware can have.
#define MilesMaxLineChannels        8      // The maximum number of channels a bus can have.
#define MilesMaxListeners           32      IDOC // The maximum number of listeners supported by Miles.
#define MilesMaxSampleBuffers       4
#define MilesMaxConvolutions        32
#define MilesFloatMixCount          64
#define MilesDenormalMin            ( 1.192092896e-06F * 2.0f )
#define MilesDecoderPassBlockCount  4
#define MilesFilePassBlockCount     4

#define MilesFilterTypePipe         1
#define MilesFilterTypeMatrix       2
#define MilesFilterCurrentVersion   3

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef enum MilesSpeaker
{
    MilesSpeakerFrontLeft           = 0,
    MilesSpeakerFrontRight          = 1,
    MilesSpeakerFrontCenter         = 2,     // Also see microsoft.com/whdc/device/audio/multichaud.mspx
    MilesSpeakerLowFrequency        = 3,
    MilesSpeakerBackLeft            = 4,
    MilesSpeakerBackRight           = 5,
    MilesSpeakerFrontLeftOfCenter   = 6,
    MilesSpeakerFrontRightOfCenter  = 7,
    MilesSpeakerBackCenter          = 8,
    MilesSpeakerSideLeft            = 9,
    MilesSpeakerSideRight           = 10,
    MilesSpeakerTopCenter           = 11,
    MilesSpeakerTopFrontLeft        = 12,
    MilesSpeakerTopFrontCenter      = 13,
    MilesSpeakerTopFrontRight       = 14,
    MilesSpeakerTopBackLeft         = 15,
    MilesSpeakerTopBackCenter       = 16,
    MilesSpeakerTopBackRight        = 17,
    MilesSpeakerMAXINDEX            = 17,
    MilesSpeakerFORCE32             = 0x80000000
} MilesSpeaker;
RR_COMPILER_ASSERT(sizeof(MilesSpeaker) == 4);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef enum MilesChannelSpec
{
    MilesChannelSpecUnknown         = 0,        // Returned from system query functions on error.

    // Normal usage, channel count is a direct mapping.
    MilesChannelSpecMono            = 1,
    MilesChannelSpecStereo          = 2,
    MilesChannelSpec40Discrete      = 4,
    MilesChannelSpec51Discrete      = 6,
    MilesChannelSpec71Discrete      = 8,
    MilesChannelSpecCOUNT           = 9,

    MilesChannelSpecFORCE32         = 0x80000000
} MilesChannelSpec;
RR_COMPILER_ASSERT(sizeof(enum MilesChannelSpec) == 4);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IDOC typedef enum MilesOutputProperty
{
    MilesOutputPropertyWASAPIThreadHandle, /*
        Returns the windows HANDLE for the thread that services the WASAPI 
        device.

        PointerToResult should be a pointer to a HANDLE.
    */
    
    MilesOutputPropertyFORCE32 = 0x80000000
} MilesOutputProperty;
RR_COMPILER_ASSERT(sizeof(MilesOutputProperty) == 4);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef enum MilesAllocFlags
{
    MilesAllocFlagNone =        0x0,
    MilesAllocFlagZeroed =      0x1,
    MilesAllocFlagFORCE32 = 0x80000000
} MilesAllocFlags;
RR_COMPILER_ASSERT(sizeof(MilesAllocFlags) == 4);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef enum MilesReportType
{
    MilesReportTypeOutOfMemory,
    MilesReportTypeContent,
    MilesReportTypeUsage,
    MilesReportTypeSystem,
    MilesReportTypeWarning,
    MilesReportTypeAudition,
    MilesReportFORCE32 = 0x80000000
} MilesReportType;
RR_COMPILER_ASSERT(sizeof(MilesReportType) == 4);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef enum MilesProfilingPostType
{
    MilesProfilingPostAsyncStart,
    MilesProfilingPostAsyncEnd,
    MilesProfilingPostEventStart,
    MilesProfilingPostEventEnd,
    MilesProfilingPostMixStart,
    MilesProfilingPostMixEnd,
    MilesProfilingPostMixerThreadStart,
    MilesProfilingPostMixerThreadEnd,
} MilesProfilingPostType;
RR_COMPILER_ASSERT(sizeof(MilesProfilingPostType) == 4);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef enum MilesDecoderOpenReturnType
{
    MilesDecoderOpenReturnFail,
    MilesDecoderOpenReturnSuccessNoSeekTable,
    MilesDecoderOpenReturnSuccessWithSeekTable
} MilesDecoderOpenReturnType;
RR_COMPILER_ASSERT(sizeof(MilesDecoderOpenReturnType) == 4);

struct MilesSpatializationInfo;
struct MilesListener;

// type definitions.
typedef struct MilesSample*         HMILESSAMPLE;
typedef struct MilesDriver*         HMILESDRIVER;
typedef struct MilesOutput*         HMILESOUTPUT;
typedef struct MilesBus*            HMILESBUS;
typedef struct MilesRoute*          HMILESROUTE;
typedef struct MilesFilterStage*    HMILESFILTERSTAGE;
typedef struct MilesBank*           HMILESBANK;
typedef struct MilesFilter const*   HMILESFILTER;
typedef struct MilesFilterProperty const* HMILESFILTERPROPERTY;
typedef UINTa                       HMILESQUEUE;
typedef UINTa                       HMILESHARDWAREOUTPUT;
typedef UINTa                       HMILESPOSTBOX;

#ifdef __cplusplus
    // type used for ensuring type safety in functions that can take either a output or a bus (or null). 
    struct HMILESOUTPUTORBUS
    {
        UINTa output_or_bus;
        HMILESOUTPUTORBUS(HMILESOUTPUT output) { output_or_bus = (UINTa)output; }
        HMILESOUTPUTORBUS(HMILESBUS bus) { output_or_bus = (UINTa)bus; }
        HMILESOUTPUTORBUS(int) { output_or_bus = 0; } // for passing zero
    };

    // type used for ensuring type safety in functions that can take either a sample or a bus.
    struct HMILESSAMPLEORBUS
    {
        UINTa sample_or_bus;
        HMILESSAMPLEORBUS(HMILESSAMPLE sample) { sample_or_bus = (UINTa)sample; }
        HMILESSAMPLEORBUS(HMILESBUS bus) { sample_or_bus = (UINTa)bus; }
    };
#else // cplusplus

    typedef UINTa HMILESOUTPUTORBUS;
    typedef UINTa HMILESSAMPLEORBUS;

#endif // cplusplus

// callback types...
IDOC typedef void* RADLINK MilesAllocFnType(UINTa ByteCount, char const* AllocationFile, U32 AllocationLine);
/*
    Called when Miles needs a new memory buffer. The resulting buffer
    must have at least ByteCount bytes available, and must be at
    least 16 byte aligned. Miles internally zeroes most allocations,
    so the buffer contents can be undefined.

    See also $MilesSetAllocFns.
*/

IDOC typedef void RADLINK MilesFreeFnType(void* Ptr, UINTa ByteCount, char const* FreeFile, U32 FreeLine);
/*
    Called when Miles is done using a buffer previously returned
    from $MilesAllocFnType.

	ByteCount is the same value passed to the allocation function for the given memory.

    See also $MilesSetAllocFns.
*/

//idoc(parent, CoreAPI)
IDOC typedef void RADLINK MilesReportFnType(MilesReportType Type, char const* Message);
/*
    The function signature for receiving error and warning information from Miles.

    $:Type The classification of error or warning.
    $:Message Any additional information regarding the report. For example,
        an out of memory report will post the system that failed to allocate.
        This string may be on the stack and the pointer should not be retained
        after control returns to the caller.
*/

//idoc(parent, CoreAPI)
IDOC typedef void RADLINK MilesProfilingPostFnType(MilesProfilingPostType Type);
/*
    The function signature for receiving a signal for when a task starts and ends.

    This is used for hooking Miles in to a game's profiling system.
*/

typedef void            RADLINK MilesFilterProcessFnType(void* state_buffer, S32 in_chan_or_chan_count, F32* buffer, F32* raw_buffer, F32 wet, F32 dry);
typedef S32             RADLINK MilesFilterOpenFnType(HMILESDRIVER driver, F32 rate, S32 in_chans, void* state_buffer);
typedef UINTa           RADLINK MilesFilterGetValueFnType(F32 rate, S32 in_chans, void* state_buffer, S32 value_type);
typedef S32             RADLINK MilesFilterSetPropertyFnType(void* state_buffer, struct MilesFilterProperty const* prop, F32 value);
typedef F32             RADLINK MilesFilterGetPropertyFnType(void* state_buffer, struct MilesFilterProperty const* prop);
typedef void            RADLINK MilesFilterCloseFnType(void* state_buffer);

typedef U32             RADLINK MilesDecoderFetchFnType(void* dest_buffer, U32 bytes_needed, void* fetch_user);
typedef U32             RADLINK MilesDecoderProcessFnType(void* state_buffer, float* dest_buffer, U32 dest_buffer_stride, U32 samples_needed, MilesDecoderFetchFnType* fetch_fn, void* fetch_user);
typedef MilesDecoderOpenReturnType
                        RADLINK MilesDecoderOpenFnType(void* state_buffer, SINTa passblock[MilesDecoderPassBlockCount], MilesDecoderFetchFnType* fetch_fn, void* fetch_param);
typedef S32             RADLINK MilesDecoderMetadataFnType(void const* data, U32 datalen, U16* out_chans, U32* out_rate, U32* out_samples, U32* out_statesize, SINTa out_passblock[MilesDecoderPassBlockCount]);
typedef void            RADLINK MilesDecoderSeekFnType(void* state_buffer);
typedef S32             RADLINK MilesDecoderSeekTableFnType(void* state_buffer, U32 sample_count, U32* actual_sample_count, S32* block_size);
typedef S32             RADLINK MilesDecoderRawSeekTableFnType(void* data, S32 datalen, U32 sample_count, U32* actual_sample_count, S32* block_size);

typedef void            RADLINK MilesSampleEOBFnType(HMILESSAMPLE sample, void* buffer_ptr, void* user);

typedef void            RADLINK MilesSample3DSpatializationFnType(HMILESSAMPLE sample, struct MilesRoute const* route, struct MilesListener const* listener, struct MilesSpatializationInfo* sinfo);

//idoc(parent, OutputAPI)
IDOC typedef void RADLINK MilesCaptureFnType(HMILESOUTPUT Output, F32* Samples, void* User);
/*
    Provides the samples that are about to be written to the output's speakers.

    Samples points to an array of floating point samples, nominally in the 
    -1 to 1 range. The size of the array is MilesFloatMixCount * $MilesOutputGetInputChannelCount,
    and the channels are *not* interleaved:

    \<pre\>

    static void RADLINK MyCaptureFn(HMILESOUTPUT Output, F32* Samples, void* User)
    {
        // this example is for a stereo output.
        assert($MilesOutputGetInputChannelCount() == 2); 
        F32* LeftChannel = Samples;
        F32* RightChannel = Samples + MilesFloatMixCount;

        // Do whatever with the samples.
    }

    \</pre\>


    When the output gets destroyed, this function will be called with
    Samples set to zero to inform the capturing code that output is
    complete.

    This function is called from the mixer thread.
*/

//idoc(parent, CoreAPI)
IDOC typedef void RADLINK MilesAllocDumpFnType(char const* AllocFile, U32 AllocLine, U32 AllocBytes, U32 AllocIndex);
/*
    Passes information about an allocation within miles.

    See also $MilesAllocDump.
*/

typedef void            RADLINK MilesExternalSoundFnType();

//idoc(parent, EventAPI)
IDOC typedef void RADLINK MilesQueueSaveFnType(void* Bytes, U32 ByteCount);
/*
    Callback used by $MilesProjectSetQueueSaveFn to save queues that
    are submitted to Miles.
*/

//idoc(parent, CoreAPI)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IDOC typedef struct MilesStartupParamaters
{
    U16 DisableMixerThread; /*
        If set, Miles will *not* create a thread to service its drivers and
        output to hardware. In this case, the game *must* call $MilesServiceDrivers
        regularly in order to maintain buffer levels. This also potentially breaks
        functions that wait for async loads (eg $MilesProjectWaitForLoad), as
        there is no mixer thread to service the load.
    */

    U16 DisableAutomaticProfileConversion; /*
        Internal Use - prevents miles from converting the tick counts to a
        reportable format. (Leave 0)
    */
} MilesStartupParameters;
/*
    Provides optional parameters for initializing Miles. Set via $MilesSetStartupParameters
*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IDOC typedef struct MilesDriverParameters
{
    HMILESHARDWAREOUTPUT HardwareOutput; /*
        If nonzero, specifies the hardware output api to use.
    */

    UINTa HardwareOutputParam; /*
        Provides a per-api interpreted parameters to the output api. See
        the output api header file for use (eg milesoutputxaudio2.h). Default
        to zero.
    */

    MilesChannelSpec ChannelSpec; /*
        If nonzero, sets the output channel specification for the driver.

        If the requested channel specification doesn't match the hardware channel
        count, Miles will do a remix to try and make things sound as good as
        possible. In general it's best to run at the hardware's channel count.
    */

    U32 SampleRate; /*
        If nonzero, sets the sample rate to run the driver at. This should rarely
        be used - performance and quality is best when Miles is allowed to run
        at the sample rate the device expects. 
    */

    U16 DirectRemixesOnly; /*
        FOR AUDITIONING USE - if non zero, Miles will not do any clever remixing
        of audio if the hardware channel count is different than the channel
        count the driver is created with. For example, if the driver is opened
        in stereo, and the Windows device presents 5.1, only the front speakers
        will have audio. Similarly, if Miles is opened it 5.1 and Windows provides
        7.1, the side speakers will never produce audio.

        This flag is provided for mastering purposes, and is not intended to
        be used by consumers.
    */

    U16 EventSystemReadLatencyMs; /*
        This causes reads by the event system for streaming in audio to take an
        additional amount of time, for stress testing.
    */

    void* HRTFDatabase; /*
        If nonzero, points to a loaded Miles HRTF sample database for use in
        spatializing stereo drivers.
    */
} MilesDriverParameters;
/*
    Provides optional parameters to $MilesDriverCreate. Initialize to zero before
    use.
*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef struct MilesMemoryWriterParameters
{
    void* OutputBuffer;
    UINTa OutputBufferSize;
    U32 Done;
} MilesMemoryWriterParameters;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef struct MilesAdpcmData
{
    U32 blocksize;
    U32 extrasamples;
    U32 blockleft;
    U32 step;
    U32 sample;
    U32 samplesL;
    U32 samplesR;
    U32 is_stereo;
    char input_buffer[36];
    U32 input_buffer_count;
} MilesAdpcmData;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef struct MilesVector
{
    F32 x, y, z;
} MilesVector;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef struct MilesSpatializationInfo
{
    MilesVector normalized_direction;
    U16 coincident; // if nonzero, the distance is zero and the vector is indet.
    U8 disable_spatialization; // if nonzero, the default conversion matrix is applied and scaled by the 3d volume.
    U8 planar_spatialized;
    F32 volume; // Volume scale to apply to the sample as a result of spatialization.
    F32 spread; // If mono, amount to "bleed" the spatialization to other channels.
    F32 distance; // The distance to the sample.
    F32 lowpass_hz; // the lowpass cutoff to apply to the signal due to spatialization.
    F32 pan_radians_per_channel; // if multichannel, the pan amt to use when close.
    F32 pan_fold_distance; // if multichannel, the distance to fold the pan to zero.
    U32 cached_lts_change_mix; // if this is a cache, this is the last lister-to-speaker update we saw
} MilesSpatializationInfo;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef struct MilesLevels
{
    U32 last_filled_mix;
    U32 last_polled_mix;
    float levels[1];
} MilesLevels;

//-----------------------------------------------------------------------------
//
// Represents one filter active in a sample or bus pipeline.
//
//-----------------------------------------------------------------------------
typedef struct MilesFilterStage
{
    struct MilesMixerControl* control;
    struct MilesFilterStage* Next;
    struct MilesFilterStageProp* prepped_properties;
    struct MilesFilter const* filter;
    MilesFilterProcessFnType* ProcessFn;
    MilesFilterGetValueFnType* ValueFn;
    MilesFilterSetPropertyFnType* SetPropFn;
    void* Memory;

    // If we were set before the sample had metadata, we won't be initialized yet.
    U8 initialized;
    U8 memory_is_allocd;
    U8 stage_is_allocd;
    U8 joined_and_bypass; // if nonzero then we run on all channels at once.
    F32 WetLevel, DryLevel;
} MilesFilterStage;

//-----------------------------------------------------------------------------
//
// Used for on demand buffer assignment for buses.
//
//-----------------------------------------------------------------------------
typedef struct MilesBusBuffer
{
    struct MilesBusBuffer* next; // next ptr for the free list.
    struct MilesBusBuffer* next_alloc_list; // next ptr for the alloc list
    void* this_ptr; // unaligned allocated ptr
    U32 last_used_mix_index;
    S32 channel_count;
    float* buffer;
} MilesBusBuffer;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef struct MilesCachedSpeakerRotation
{
    U16 channel_count;
    U16 listener_index;
    MilesVector listener_to_speaker[1]; // [channel count]
} MilesCachedSpeakerRotation;

//-----------------------------------------------------------------------------
//
// Represents a physical output device
//
//-----------------------------------------------------------------------------
typedef struct MilesOutput
{
    U32 tag;     // HOUT
    struct MilesDriver* owner;

    U8 input_channel_count;
    U8 output_channel_count;
    U16 is_radshal;
    
    F32 master_volume_level;
    U32 sample_rate;
    MilesChannelSpec channel_spec;

    void* ss_ptr, *ss_heap;
    

    MilesCaptureFnType* capture_fn;
    void* capture_user_data;

    float* output_buffer; // this is assigned each frame

    struct MilesOutput* next;
} MilesOutput;


//-----------------------------------------------------------------------------
//
// Represents a route between two representation of sound and downmixing
// needed, eg 6 channel sample -> 2 channel bus.
//
//-----------------------------------------------------------------------------
typedef struct MilesRoute
{
    struct MilesRoute* next;
    union
    {
        // sample or bus that we are routing *from*.
        struct MilesBus* bus_parent;
        struct MilesSample* sample_parent;
    };
    // where we route *to*
    union
    {
        struct MilesBus* bus_output;
        struct MilesOutput* dig_output;
    };

    MilesLevels* output_signal_levels;
    float* delay_buffer; // [input_channel_count]
    U16 delay_current_sample;
    U16 delay_sample_count;
    F32 volume_level;
    F32 last_general_level; // used only to check if we can skip mixing this route.
    F32 lfe_level;
    U8 type;
    U8 flags;
    U8 route_id; // unassigned by low level

    // These values are copied from the actual output device, and should be constant across all calls.
    U8 output_channel_count;

    union
    {
        //
        // The last_volumes ptr must be first in the union.
        //
        struct
        {
            F32 last_direct_volumes[MilesMaxLineChannels]; // [output_channel_count]
        } direct;
        struct
        {
            // Volumes are a function of the input's fade/pan setting,
            // and results in a 1D volume array.
            F32 last_panned_volumes[MilesMaxLineChannels]; // [output_channel_count]
        } panned;
        struct
        {
            // Volumes are a function of the input channel count and the
            // output channel count, and are likely a system default.

            F32* last_conversion_volumes; // indexed by output_channel + output_channel_count * input_channel.

            // If nonzero, the levels were customized by the game.
            F32* conversion_levels; // indexed by output_channel + output_channel_count * input_channel.
        } convert;
        struct
        {
            // Volumes are a function of output channel count, the source position/orient,
            // input channel count, and the listener position/orient. 2D volume array.
            F32* last_spatialized_volumes; // [output_channel + output_channel_count * input_channel]
            struct MilesLowPassInfo* spatialization_lp; // [input_channel]
            struct MilesSpatializationInfo* cached_sinfo;
            F32* cached_3d_levels; // [output_channel + output_channel_count * input_channel]

            struct MilesHRTFInfo* hrtf; // only if hrtf enabled on a stereo output. [input_channel]
        } spatialized;
    };
} MilesRoute;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef struct MilesGraphPoint
{
    // must match CompiledGraphPoint, and GraphPoint in miles studio.
    F32 X, Y, ITX, ITY, OTX, OTY; // Point & tangents.
    U8 IType, OType;
    U8 PADDING[2];
} MilesGraphPoint;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef struct MilesConeParams
{
    F32 inner_angle_radians;
    F32 outer_angle_radians;
    F32 outer_volume_level;
} MilesConeParams;

//-----------------------------------------------------------------------------
//
// Falloff and location information for a sample
//
//-----------------------------------------------------------------------------
typedef struct MilesSample3D
{
    MilesSample3DSpatializationFnType* spatialization_fn;

    MilesGraphPoint* volgraph;
    MilesGraphPoint* lpgraph;
    MilesGraphPoint* spreadgraph;

    MilesVector position;         // 3D position
    MilesVector face;             // 3D orientation
    MilesVector up;               // 3D up-vector

    U8 volgraphcnt;
    U8 lpgraphcnt;
    U8 spreadgraphcnt;
    U8 PADDING;

    // Cone attenuation parameters
    MilesConeParams cone_params;
    S32 cone_enabled;
    
    F32 spread;
    F32 auto_spread_distance;
    // 16x4

    F32 multi_channel_pan_radians;
    F32 multi_channel_pan_fold_distance;

} MilesSample3D;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef struct MilesFilter
{
    U32 version;
    U32 type;
    char const* name;
    void* fns;
} MilesFilter;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef struct MilesDecoder
{
    S32 version;
    S32 type;
    MilesDecoderMetadataFnType* MetadataFn;
    MilesDecoderOpenFnType* OpenFn;
    MilesDecoderProcessFnType* ProcessFn;
    MilesDecoderSeekFnType* SeekFn;
    MilesDecoderSeekTableFnType* SeekTableFn;
    MilesDecoderRawSeekTableFnType* RawSeekTableFn;
} MilesDecoder;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef struct MilesRegisteredFilter
{
    MilesFilter const* filter_def;
    struct MilesRegisteredFilter* next;
} MilesRegisteredFilter;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef struct MilesListener
{
    U8 index; // 0...MilesMaxListeners-1
    U8 cone_enabled;
    U8 cached_speaker_count;
    U8 dirty;
    U32 last_updated_mix;
    MilesVector listen_position;
    MilesVector listen_face;
    MilesVector listen_up;
    MilesVector listen_cross;
    MilesConeParams cone_params;
    MilesCachedSpeakerRotation** speaker_rotations;
} MilesListener;

//-----------------------------------------------------------------------------
//
// Management structure for samples, outputs, and buses.
//
//-----------------------------------------------------------------------------
typedef struct MilesDriver
{
    // HDIG
    U32 tag;

    // incrementing number of mixes
    U32 current_mix_index;

    // Master volume applied to all samples 0-1.0
    F32 master_volume_level;

    // Copy of the master volume when set by the foreground thread so we can commit before mixes.
    volatile U32 shadow_master_volume_level;

    // # of samples being played
    volatile U32 playing_sample_count;

    // # of buses active last mix.
    volatile U32 active_bus_count;

	// # of active/muted voices and eventsn in the high level last mix.
	volatile U64 high_level_counts;

    // rate at which all of the buses mix in to == rate of initial MilesOutput.
    F32 master_rate;

    // # of ms mixed each call to the mixer.
    F32 ms_per_mix;

    // Linked list of outputs managed by this driver.
    MilesOutput output_list;

    // List of listeners and their properties
    MilesListener* listeners;

    // List of allocated samples - owns memory.
    struct MilesSample* sample_list;

    // List of samples that are streamed sources and need updating - doesn't own memory.
    struct MilesSample* stream_list;

    // List of allocated buses - owns memory.
    struct MilesBus* bus_list;

    // List of buses that are force activated due to decay - doesn't own memory
    struct MilesBus* bus_decay_list;

    // pointer the high level state hangs off of. Allocated with the driver, so never freed.
    struct MilesEventSystem* event_system;

    // pointer to an allocated space for tracking the output levels.
    float* signal_output_pointer;
    S32 signal_output_retrieved;

    MilesDecoder const* decoders[MilesNumDecoders];

    // List of filters that can be assigned to a filter stage.
    MilesRegisteredFilter* filter_list;

    // Buffers for active buses.
    MilesBusBuffer* bus_buffers_free[MilesMaxLineChannels]; // free list - doesn't own memory ever.
    MilesBusBuffer* bus_buffers_alloc_list; // allocated bus buffers list - used for tracking for frees.

    // List of asyncs we need to close.
    struct MilesStreamData* stream_close_list;

    // Playback active if non-zero
    U8 playing;
    U8 has_played;
    U8 mastering_remix_on;

    U8 listener_count; // [1, MilesMaxListeners]

    // # of samples that are waiting to be started at an exact time.
    S32 scheduled_sample_count;

    // Profiling
    U64 driver_profile_ticks;
    U64 decoder_profile_ticks;
    U64 mix_profile_ticks;
    U64 event_profile_ticks;
	U64 filter_profile_ticks;
	U64 prepare_profile_ticks;

    U64 last_profile_ticks;
    U32 last_report_mix;

    // these are floats stored as int
    volatile U32 last_profile_percent_reported;
    volatile U32 last_decoder_reported;
    volatile U32 last_mix_reported;
    volatile U32 last_event_reported;
	volatile U32 last_filter_reported;
	volatile U32 last_prepare_reported;

    volatile U32 skips_since_last_report;

    U32 mix_count;
    U32 mix_type_count[4];

    U16 downmix_cache_count;
    U16 downmix_cache_alloc_count;
    void* downmix_cache;
    float* downmix_defaults;

    UINTa memory_allocated;

    U8 global_mute;
    U8 last_global_mute;

    U32 stream_read_error_latch;

    void* convolution_impulse_responses[MilesMaxConvolutions - 1];
    U32 convolution_impulse_response_allocated;

    U32 solo_bus_count;

    struct MilesHRTFDB* hrtf_db;
    struct MilesDriver* next_driver;
} MilesDriver;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef struct MilesSampleBuffer
{
    void const* start;          // Sample buffer address
    U32 len;                    // Sample buffer size in bytes
    U32 pos;                    // Index to next byte to use
} MilesSampleBuffer;

//-----------------------------------------------------------------------------
//
// Holds all off the state required to advance the mixer for a sample.
//
//-----------------------------------------------------------------------------
typedef struct MilesSampleMixerState
{
    // # of samples requested last prepare
    U16 prepare_sample_count;

    // # of resample samples valid.
    U8 resample_remns_valid;

    // nonzero means our loop boundaries are managed at the sample level, not the source(byte) level
    S8 loop_is_sample_level;

    // nonzero means the mixer specification has completed providing samples.
    U8 mixer_is_exhausted;

    // nonzero means must be freed with MilesDriverFree
    U8 is_allocated;

    // nonzero means mute whatever samples this provides.
    U8 mute;

    // nonzero if the mixer reached the end of a loop and migrated to loop_start
    U8 looped_this_mix;

    // nonzero if the mixer should copy over another loop end when a loop occurs
    U8 loop_end_update; 

    // fractional component of resampling remaining.
    U32 resample_start_frac;

    // the number of loops to execute - 1 is play once, 0 is inf.
    S32 play_count;

    // the source sample position loops end at.
    S32 loop_end_samples;

    // if loop_end_update, set loop_end_samples to this after a loop
    S32 loop_after_end_samples;
    S32 loop_after_end_bytes;

    // the source sample position loops start at.
    S32 loop_start_samples;

    // the # of samples we have to cull after looping a compressed source.
    S32 loop_start_cull_samples;

    // the byte offset to return to when we sample level loop.
    U32 loop_start_byte_position;

    // the maximum byte offset we need to read to complete the sample level loop (ie finish the
    // block containing our last samples).
    U32 loop_end_byte_position;

    // our current sample position in our source - invalid after seek if the source doesn't have a seek table.
    U32 current_sample_position;

    // number of samples that need to be culled from the source before using
    // this is for block based compression and sample accurate looping/seeking.
    U32 cull_sample_count;

    // rate set by game for pitch effects.
    F32 playback_rate_factor;

    // ratio of our data source rate to the master output rate.
    F32 driver_rate_factor;

    // rate set by game for doppler effects.
    F32 doppler_rate_factor;

    // volume for this layer.
    F32 mixer_volume_level;
    F32 last_mixer_volume_level;

    // samples to delay before starting to mix this sample.
    U32 mix_delay_samples;

    struct MilesSampleMixerState* next, *prev;

    // sample bleed over from resampling.
    F32 resample_remns_1[MilesMaxLineChannels];
    F32 resample_remns_2[MilesMaxLineChannels];
} MilesSampleMixerState;

//-----------------------------------------------------------------------------
//
// Shared state across buses and samples for controlling the mix.
//
//-----------------------------------------------------------------------------
typedef struct MilesMixerControl
{
    struct MilesFilterStage* filter_chain;
    struct MilesRoute* output_chain;
    F32 panning_control_levels[5];
    F32 lfe_level;
    F32 volume_level;
    U8 channel_count;
    U8 filter_uses_source;
    U16 route_max_delay_samples;
} MilesMixerControl;


typedef struct MilesDataDecoder
{
    MilesDecoder const* decoder;
    void* decoder_state_buffer_aligned;

    MilesSampleDataType data_type;

    MilesAdpcmData adpcm;

    U32 output_rate;
    U32 total_sample_count;

    // wav seek helper data if we are a wav file.
    U32 wav_data_offset;

    U8 decoder_has_seek_table;
    U8 output_channel_count;

    // decoder state for raw (wav) sources
    U8 uncompressed_data_is_float;
} MilesDataDecoder;


//-----------------------------------------------------------------------------
//
// Interface to the raw bytes supplying the decoder
//
//-----------------------------------------------------------------------------
typedef struct MilesDataSource
{    
    MilesSampleEOBFnType* eob_fn_callback;

    // user parameters for the data source.
    void* user;

    struct MilesSample* sample;
    struct MilesDataSource* next, *prev;

    struct MilesStreamData* stream_data;

    // Source data buffers
    MilesSampleBuffer buf[MilesMaxSampleBuffers];

    // the decoder we use to output samples.
    MilesDataDecoder decoder_state;

    MilesSampleSourceType source_type;

    U32 current_byte_position;
    U32 source_total_bytes;

    // the max read point in the source (current_byte_position <= this)
    U32 source_data_view_end;
    U32 source_data_view_start;

    // # of valid source buffers
    U8 n_buffers;
    U8 head;
    U8 tail;

    // Nonzero when a request was made to the data source and NOTHING was available to return
    // due to reaching the end of the view of the file.
    U8 source_is_exhausted;

    S32 loop_start_bytes;             // Starting offset of loop block (0=SOF)
    U32 loop_end_bytes;               // End offset of loop block (~0U=EOF)

    U8 is_allocated;
} MilesDataSource;

//-----------------------------------------------------------------------------
//
// A single playing voice in the system
//
//-----------------------------------------------------------------------------
typedef struct MilesSample
{
    // HSAM
    U32 tag;
    struct MilesDriver* driver;

    MilesMixerControl mixer_control;

    // management
    struct MilesSample* next_sample;
    struct MilesSample* next_stream_sample;

    // Playback rate of data supplied from the source.
    S32 original_playback_rate;

    MilesSampleStatus status;

    // max_volume of any speaker at last mix
    F32 max_output_mix_volume;

    // master mix samples to start the sample on.
    U64 mix_start_samples;

    // ID for grouped operations.
    U32 group_id;

    // number of samples we need to continue playing to allow filters to drain
    S32 decay_samples;

    // which listeners we operate on.
    U32 listener_mask;    
    
    //
    // [DataSource0] -> [Decoder0] -> [Resample0] \
    // [DataSource1] -> [Decoder1] -> [Resample1] |-- [Filtering] -- [Outputs]
    // [DataSource2] -> [Decoder2] -> [Resample2] /
    //

    MilesDataSource* source;

    // mixer advancement state
    MilesSampleMixerState* mixer;


    // built in low pass state.
    struct MilesLowPassInfo* lp;

    // Spatialization info - position, falloffs, etc.
    struct MilesSample3D S3D;
    
    UINTa user_data[2];

    // non zero to fade
    U8 fade_to_stop;

    U8 all_sources_were_exhausted;

    // if we are going to switch between >1 mixer on a sample and 1 mixer, then we need 
    // to ALWAYS do the mixer volume scale separately. If we don't, when we switch between them
    // the volume ramping in the core mixer loop gets a random scalar dropped in it, when
    // that volume had already been "baked in" by ramping where we accumulated teh separate
    // mixers. If the scalars are different this causes pops.
    U8 force_multi_mixer_mix;

    // if nonzero, the loop was set before we had any metadata to validate.
    U8 loop_is_pending;

    // if nonzero the position was set before metadata.
    U8 pending_position_valid; // 1 bytes 2 samples 3 ms

    U8 error_state;

    U8 is_allocated;

    // nonzero if we have been added to the stream update list.
    U8 is_stream_registered;

    U32 pending_position;
    U8 last_volumes_initialized;
    U8 want_output_max_level;

    U8 sample_allocation_failed;
    U16 sample_allocation_buffer_remain; // must always be set such that current offset is 16 byte aligned.
    U16 sample_allocation_buffer_max;
    char* sample_allocation_buffer;

} MilesSample;


//-----------------------------------------------------------------------------
//
// A grouping of inputs for processing and further routing.
//
//-----------------------------------------------------------------------------
typedef struct MilesBus
{
    U32 tag; // HBUS
    struct MilesDriver* driver;
    MilesMixerControl mixer_control;
    
    S32 decay_samples;

    F32 max_output_mix_volume;
    U32 last_mix_index;

    float* assigned_buffer;
    float* last_assigned_buffer; // forensics only - never read this!
    struct CompiledBus* event_system_bus_owner; // forensics only - never read this!
    char const* event_system_bus_name; // forensics only - never read this!
    struct MilesBus* next_used; // linkedlist of buses active this mix.
    struct MilesBus* next_mix_list; // ll of buses ordered for the mix.
    struct MilesBus* next_allocated; // ll of buses created on the driver.
    struct MilesBus* next_decay_list; // ll of buses that have active decay samples.

    // if we are soloing, we copy our output to this buffer so we can copy it
    // to the output once the mixes are done.
    float* solo_accumulation_buffer;

    MilesLevels* input_signal_levels;
    U8 on_decay; // whether we've been added to the decay list.
    U8 filter_decays; // nonzero if an installed filter requires us to decay.
    U8 mixed; // whether we've been mixed yet.
    U8 muted;
    U8 want_output_max_level;
} MilesBus;


//-----------------------------------------------------------------------------
//
// Driver Servicing API
//
//-----------------------------------------------------------------------------
//idoc(parent, TimerAPI)
IDOC RADEXPFUNC F32 MILES_NO_THUNK RADEXPLINK MilesServiceGetHighestDelayMs();
/*
    Returns the service timer delay since the last call to $MilesServiceGetHighestDelayMs

    $:return The delay, in milliseconds.

    Each call resets the highest delay tracking, so this function should only
    be called periodically, on the order of a second. In general this is just
    used for diagnosing platform issues and skips, as a high delay
    on the mixer thread can prevent the audio hardware from getting
    serviced reliably. If the platform doesn't service the mixer thread reliably,
    then a higher mixahead is necessary in order to prevent skips.

    This function returns 0 under manual driver servicing via $MilesServiceDrivers.

    If the platform (or CPU load) prevents reliable servicing, the mixahead 
    should be increased to prevent audio dropouts via $MilesDriverSetMixahead.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

RADEXPFUNC void RADEXPLINK MilesServiceDriverStartup(HMILESDRIVER Driver);

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesServiceDrivers();
/*
    Provides a way to manually service the Miles drivers under advanced configurations.

    $:return The number of additional services Miles requires to be considered "up to date".

    Generally, Miles will create a thread and service its drivers on its own.
    If the game has disabled this via the $MilesStartupParameters call, then
    the game must regularly service the drivers.

    If the return for this function is nonzero, call it again as soon as possible.

    Note that for the event system async loading (eg banks and projects), the loading
    logic takes place during the driver servicing - so even though no samples are
    getting played, driver servicing should still take place. A corollary of this
    is the game must never call a waiting function (eg $MilesBankWaitForLoad) on
    the same thread that is servicing the drivers.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesServiceGetThreadHandle(void* PtrToPlatformHandle);
/*
    Access the platform handle for the thread managing the driver servicing.

    $:PtrToPlatformHandle A pointer to the platform thread handle type.

    Provides access to the platform handle for the service thread.

    The parameter should be a *pointer* to the platform type (which may also
    be a pointer!).

    \<pre\>

    // On windows.
    HANDLE ThreadHandle = 0;
    MilesServiceGetThreadHandle(&ThreadHandle);

    \</pre\>

    *Thread Safety* $(Threading:TS_Lock)
*/


//-----------------------------------------------------------------------------
//
// Math API
//
//-----------------------------------------------------------------------------
//idoc(parent, MathAPI)
IDOC RADEXPFUNC F32 MILES_NO_THUNK RADEXPLINK MilesDbToLinear(F32 VolumeDb);
/*
    Converts from decibels to a linear representation of loudness.

    See also $MilesVolumes.

    *Thread Safety* $(Threading:TS_Pure)
*/

IDOC RADEXPFUNC F32 MILES_NO_THUNK RADEXPLINK MilesDbToLevel(F32 VolumeDb);
/*
    Converts from decibels to a scalar representation of volume.

    See also $MilesVolumes.

    *Thread Safety* $(Threading:TS_Pure)
*/

IDOC RADEXPFUNC F32 MILES_NO_THUNK RADEXPLINK MilesLevelToLinear(F32 VolumeLevel);
/*
    Converts from a scalar representation of volume to a linear representation of loudness.

    See also $MilesVolumes.

    *Thread Safety* $(Threading:TS_Pure)
*/

IDOC RADEXPFUNC F32 MILES_NO_THUNK RADEXPLINK MilesLevelToDb(F32 VolumeLevel);
/*
    Converts from a scalar representation of volume to a decibels.

    See also $MilesVolumes.

    *Thread Safety* $(Threading:TS_Pure)
*/

IDOC RADEXPFUNC F32 MILES_NO_THUNK RADEXPLINK MilesLinearToDb(F32 VolumeLinear);
/*
    Converts from a linear representation of loudness to decibels.

    See also $MilesVolumes.

    *Thread Safety* $(Threading:TS_Pure)
*/

IDOC RADEXPFUNC F32 MILES_NO_THUNK RADEXPLINK MilesLinearToLevel(F32 VolumeLinear);
/*
    Converts from a linear representation of loudness to a scalar representation of volume.

    See also $MilesVolumes.

    *Thread Safety* $(Threading:TS_Pure)
*/

IDOC RADEXPFUNC F32 MILES_NO_THUNK RADEXPLINK MilesStToScalar(F32 PitchSt);
/*
    Converts from semitones to a playback rate scalar.

    *Thread Safety* $(Threading:TS_Pure)
*/

IDOC RADEXPFUNC F32 MILES_NO_THUNK RADEXPLINK MilesScalarToSt(F32 PitchScalar);
/*
    Converts from a playback rate scalar to semitones.

    *Thread Safety* $(Threading:TS_Pure)
*/

IDOC RADEXPFUNC F32 MILES_NO_LOCK RADEXPLINK MilesEvaluateGraph(MilesGraphPoint* GraphPoints, S32 GraphPointCount, F32 X);
/*
    Evaluates a function with tangent information at a given point.

    $:GraphPoints An array of points with position and tangent information.
    $:GraphPointCount The size of the GraphPoints array.
    $:X The position to evaluate the function at.

    This is the same function used internally to evaluate falloff graphs
    and controller transforms.

    Each point has must have a position (X, Y) set. However, whether the
    tangent vectors (ITX, ITY, OTX, OTY) are necessary depends on the tangent
    type, and some tangent types also overwrite the next tangents usage as
    well.

    $* MilesTangentLinear - The tangent is set to the vector from the
        point to the next/previous point. The given tangent vector is
        ignored.
    $* MilesTangentCurved - The given tangent vector is used to evaluate
        a curved path between the two points.
    $* MilesTangentStep - The tangent vectors are ignored, and the
        graph between the points remains at the previous value
        until the next point is reached.
    $* MilesTangentFalloff - The tangent vectors are ignored, and the
        graph between the points acts as an exponential falloff.
    $* MilesTangentInvFalloff - As MilesTangentFalloff, except the curve
        is reversed.
    $* MilesTangentFadeIn - The tangent vectors are ignored, and the
        graph between the points acts as a fade where the perceived loudness
        is increased linearly. See also $MilesVolumes.
    $* MilesTangentFadeOut - As MilesTangentFadeIn, but decreasing instead 
        of increasing.
    $* MilesTangentCrossFadeOut/MilesTangentCrossFadeIn - As MilesTangentFadeIn, 
        but a constant power crossfade.

    *Thread Safety* $(Threading:TS_Pure)
*/

//-----------------------------------------------------------------------------
//
// Core API
//
//-----------------------------------------------------------------------------
//idoc(parent, CoreAPI)
IDOC RADEXPFUNC U32 MILES_NO_LOCK RADEXPLINK MilesStartup(MilesReportFnType* ReportFn);
/*
    Initializes the Miles Sound System for use.

    $:ReportFn The function to call on an error or warning in Miles, or zero.
        It's always a good idea to supply this, as it will often help isolate
        API usage errors.
    $:return Zero if a global subsystem failed to initialize. If this happens
        it's almost certainly a support request.

    Prepares Miles for use by creating certain synchronization
    primitives and threads.

    Most Miles functions cannot be called prior to startup, with
    the exception of overrides such as allocators, file io, or
    async io.

    The report callback provides feedback as to why various functions
    fail, when returning an error code doesn't result in anything the
    code can do to recover. Most Miles functions will simply fail, or
    return a failure, without any additional information. This is because
    most errors are content or implementation errors and need to be corrected
    during development - knowing _why_ it failed doesn't matter once
    deployed.

    If the function's failure mode can lead to useful decisions from the
    caller, then an error code will be returned.
*/

IDOC RADEXPFUNC void MILES_NO_LOCK RADEXPLINK MilesShutdown();
/*
    Shuts down the Miles Sound System.

    After completion of this function, all Miles handles are invalid.
    All Miles objects are freed - there is no need to explicitly free 
    anything.

    Further Miles calls are not valid.
*/

IDOC RADEXPFUNC void MILES_NO_THUNK RADEXPLINK MilesSetStartupParameters(MilesStartupParameters* Parameters);
/*
    Sets the advanced customization parameters for Miles.

    Must be called before $MilesStartup.
*/

IDOC RADEXPFUNC char const* MILES_NO_THUNK RADEXPLINK MilesGetVersionString();
/*
    Returns the MilesVersionString that the binary was compiled with.

    This can be used to force compatibility between the header and the binary,
    however if present, it is highly advised to have a way to bypass this
    check via an environment variable or parameter, in order to facilitate
    drop-in binary fixes.

    This function does not provide any mechanisms for comparison other
    than exact equality via strcmp.

    The return from this function must not be deleted.

    *Thread Safety* $(Threading:TS_Pure)
*/

IDOC RADEXPFUNC void MILES_NO_LOCK RADEXPLINK MilesLock();
/*
    Locks the Miles API mutex.

    Currently, most access in the Miles low level is synchronized with
    the mixer thread. This is done automatically, and in general this
    is never used manually. However if the game requires that several 
    calls are not interrupted by a mix, you can wrap them with MilesLock
    and $MilesUnlock().
*/

IDOC RADEXPFUNC void MILES_NO_LOCK RADEXPLINK MilesUnlock();
/*
    Unlocks the Miles API mutex.

    See $MilesLock for details.
*/

IDOC RADEXPFUNC void MILES_NO_LOCK RADEXPLINK MilesSleep(U32 SleepMs);
/*
    Sleeps the current thread for the given number of milliseconds.
*/

IDOC RADEXPFUNC void MILES_NO_THUNK RADEXPLINK MilesUseTelemetry(void* TelemetryContext);
/*
    Sets the Telemetry context Miles will use to report profiling data.

    If using a build of Miles with telemetry enabled (ask support if desired),
    a telemetry 3 api pointer can be passed to this function to capture Miles
    specific profiling data.

    Generally, this function should be called with the TM_API_PTR define:

***

    // Initialize TM3 as your game requires...
    tmLoadLibrary(...);
    tmInitalize(...);
    tmOpen(...);

    // Pass the global TM3 API pointer to Miles for use.
    MilesUseTelemetry(TM_API_PTR);

***

*/

IDOC RADEXPFUNC void RADEXPLINK MilesSetProfilingCallback(MilesProfilingPostFnType* PostFn);
/*
    Sets the function callback to call for hooking coarse miles profiling 
    information in to custom tools.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U64 MILES_NO_THUNK RADEXPLINK MilesGetMsCount();
/*
    Returns the number of milliseconds since application startup.

    This utilizes the system clock, which can be a kernel call. As
    a result, it's not the fastest way to do unimportant timekeeping.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC void* MILES_NO_THUNK RADEXPLINK MilesAllocEx(UINTa ByteCount, MilesAllocFlags Flags, struct MilesDriver* Driver, char const* AllocFile, U32 AllocLine);
/*
    Allocates memory (generally use *MilesAlloc* wrapper instead).

    $:ByteCount The number of bytes to allocate.
    $:Flags Flags controlling the allocation.
    $:Driver The driver to assign the memory to for quota limits.
    $:AllocFile The source file for the allocation
    $:AllocLine The line number for the allocation
    $:return A 16-byte aligned pointer to the allocated memory.

    Generally, MilesAlloc is the allocator to use, as it automatically places
    the file and line information, and specifies that the memory be zeroed
    prior to return.

    If you have a bulk allocation that you don't want zeroed, use this instead.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC UINTa MILES_NO_THUNK RADEXPLINK MilesFreeEx(void* Ptr, struct MilesDriver* Driver, char const* FreeFile, U32 FreeLine);
/*
    Frees memory (generally use *MilesFree* instead)

    $:Ptr The pointer previously allocated via $MilesAllocEx.
    $:Driver The driver who owns the quota.
    $:FreeFile The source file where the free occurs (__FILE__)
    $:FreeLine The line number where the free occurs (__LINE__)
    $:return The number of bytes the original allocation was for

    See $MilesAllocEx.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSetAllocFns(MilesAllocFnType* AllocFn, MilesFreeFnType* FreeFn);
/*
    Sets the allocator for Miles to use. 

    $:AllocFn The function to call for allocating memory.
    $:FreeFn The function to call for freeing memory.
    
    Memory returned by the allocator *must* be 16 byte aligned.

    Passing zero as the parameters will reset them to Miles defaults.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC UINTa MILES_NO_THUNK RADEXPLINK MilesAllocGetCurrentTotal(UINTa* OutMaxSinceLastCall);
/*
    Returns the current number of bytes Miles has allocated.
*/

IDOC typedef enum MilesAllocTrackLevel
{
    MilesAllocTrackNone,
    /*
        Memory allocation tracking is disabled.
    */

    MilesAllocTrackAll,
    /*
        Miles retains allocation and free information for ALL allocations. This diagnostic
        information is not freed at shutdown, and since it tracks all allocations
        its memory use is unbounded. View the saved information with $MilesAllocDump.
    */

    MilesAllocTrackLarge,
    /*
        Miles only tracks allocations larger than 1 MB, and saved free information
        for the last 5 large allocations. 
    */

    MilesAllocTrackFORCE32 = 0x80000000
} MilesAllocTrackLevel;

IDOC RADEXPFUNC void MILES_NO_THUNK RADEXPLINK MilesAllocTrack(MilesAllocTrackLevel TrackLevel);
/*
    Instructs Miles to enable diagnostic allocation tracking through MilesAlloc and MilesFree.

    This MUST be called before the allocator is used, as well as before
    $MilesStartup.

    See Also $MilesAllocDump
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesAllocDump(MilesAllocDumpFnType* DumpFn, U32 OnlyActive);
/*
    Dumps all allocations to the given callback. If OnlyActive is nonzero, then only
    allocations that have not been freed will be dumped. Returns 1 if any were 
    printed, 0 otherwise.

    This function has no effect if $MilesAllocTrack hasn't been called.

    *Thread Safety* This function takes no lock as it should only be called after $MilesShutdown.
*/

// Common accessors for allocations.
#define MilesAlloc(bytes) MilesAllocEx(bytes, MilesAllocFlagZeroed, 0, __FILE__, __LINE__) IDOC // Wrapper to automatically pass __FILE__ and __LINE__
#define MilesFree(ptr) MilesFreeEx(ptr, 0, __FILE__, __LINE__) IDOC // Wrapper to automatically pass __FILE__ and __LINE__


//-----------------------------------------------------------------------------
//
// Driver API
//
//-----------------------------------------------------------------------------
//idoc(parent, DriverAPI)
IDOC RADEXPFUNC HMILESDRIVER RADEXPLINK MilesDriverCreate(MilesDriverParameters* CreateParameters);
/*
    Creates a driver to output audio on the default hardware output for the platform, with optional
    overrides.

    $:CreateParameters A pointer containing the optional creation parameters, or null to use all defaults.
        This object is copied internally and is not referenced once MilesDriverCreate completes.
    $:return A handle to the created driver, or zero on fail.

    See $MilesDriverParameters for override options.

    Creates a driver that has one hardware output. If using defaults, $MilesOutputGetDefault can
    be used to determine what API has been selected.

    Creating a driver is often the first step to getting audio to the speakers.
    From there the game can either use the low level interface by creating samples
    ($MilesSampleCreate) and buses ($MilesBusCreate) manually, or use the high 
    level by creating queues ($MilesQueueCreate) and loading a project ($MilesProjectLoad)
    with banks ($MilesBankLoad)

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesDriverDestroy(HMILESDRIVER Driver);
/*
    Destroys a given driver and all associated resources.

    $:Driver The driver to destroy.

    This frees all allocated resources under the given driver, including
    samples, projects, and banks.

    The driver handle is invalid upon return.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesDriverGetReadError(HMILESDRIVER Driver);
/*
    Returns nonzero when a background load (event system, stream) fails

    If any background load fails a demand load, then any further reads on
    the driver will stop under the assumption that the read failed due to 
    an ejected disc. Once the game has verified that the disc is accessible,
    then call $MilesDriverRestartAsyncs to resume reads.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock).
*/

IDOC RADEXPFUNC void RADEXPLINK MilesDriverRestartAsyncs(HMILESDRIVER Driver);
/*
    Restarts any failed background loads that have failed due to read errors.

    See also $MilesDriverGetReadError.

    *Thread Safety* $(Threading:TS_Lock).
*/

IDOC RADEXPFUNC U32 MILES_NO_LOCK RADEXPLINK MilesDriverSetMixahead(HMILESDRIVER Driver, U32 MixAheadMs);
/*
    Sets the number of milliseconds to queue on the hardware to prevent accidental dropouts due to timing and service delays.

    $:Driver The driver to set the mixahead for.
    $:MixAheadCount The number of milliseconds to ready.
    $:return The previous mixahead.

    NOTE - currently this is a global setting (it will be per driver eventually)

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC F32 MILES_NO_LOCK RADEXPLINK MilesDriverGetProfiling(HMILESDRIVER Driver, F32* OutOfWhichDecoders, F32* OutOfWhichMix, F32* OutOfWhichEvent, F32* OutOfWhichFilters, F32* OutOfWhichPrepare);
/*
    Returns the percentage of CPU time utilized by the given Miles driver since the last call.

    $:Driver The driver to profile.
    $:OutOfWhichDecoders The percentage of miles' execution that was spent in sound decoders, or zero if not needed.
    $:OutOfWhichMix The percentage of miles' execution that was spent in the mixers, or zero if not needed.
    $:OutOfWhichEvent The percentage of miles' execution that was spent in the high level event system, or zero if not 
        needed.
	$:OutOfWhichFilters The percentage of miles' execution that was spent running filters, or zero if
		not needed.
	$:OutOfWhichPrepare The percentage of miles' execution that was spent preparing source data for
		mixing (e.g. resampling)

    This function will update numbers at a max of once per 500 ms.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_LOCK RADEXPLINK MilesDriverGetDropoutCount(HMILESDRIVER Driver);
/*
    Returns the number of hardware dropouts that have occured since the last call.

    $:Driver The driver to check. All outputs on the driver are returned.

    This function returns the number of times the output layer needed
    to provide data to the operating system audio output API and no data was ready, or
    alternatively if the buffers waiting on the hardware ever drops to zero,
    depending on the low level API structure. 

    Hardware APIs can update pretty fast, so often this will jump to large values very
    quickly even though only a single dropout has occurred. Use this as a test
    that something _has_ happened, not necessarily the magnitude of the event.

    Often this is an indication that the Miles workload is too large, or Miles
    is getting starved of CPU time.

    *Thread Safety* $(Threading::TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC HMILESOUTPUT RADEXPLINK MilesDriverGetOutput(HMILESDRIVER Driver, U32 OutputIndex);
/*
    Returns a handle to the hardware output at the given index.

    $:Driver The driver to access outputs for.
    $:OutputIndex The output index, in order of creation.
    $:return A handle to the hardware output device, or zero on error.

    The 0th output is created when the driver is created, all others are
    added via $MilesOutputCreate.

    The output device handle is used by $MilesSampleCreate to control
    whether the sample is spatialized to that device's channel count.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U64 RADEXPLINK MilesDriverGetMixedSamples(HMILESDRIVER Driver);
/*
    Returns the number of audio samples the driver has mixed to its 
    outputs since output began.

    $:Driver The driver to poll.
    $:return The number of audio samples mixed since creation.

    The total time the driver has mixed can be calculated by 
    dividing by the sample rate of the primary output on the
    driver ($MilesDriverGetOutput(Driver, 0)).     

    *Thread Safety* $(Threading:TS_Lock)   
*/

IDOC RADEXPFUNC U32 MILES_NO_LOCK RADEXPLINK MilesDriverGetCurrentMixIndex(HMILESDRIVER Driver);
/*
    Returns the current mix index. Each mix generates MilesFloatMixCount samples for
    the hardware and processes events once.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC UINTa MILES_NO_THUNK RADEXPLINK MilesDriverGetCurrentAllocationBytes(HMILESDRIVER Driver, UINTa* OutEventSystemCacheBytes);
/*
    Returns the current amount of memory this driver has allocated.

    If desired, OutEventSystemCacheBytes will return the current streaming cache size
    in the event system.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_LOCK RADEXPLINK MilesDriverGetActiveBusCount(HMILESDRIVER Driver);
/*
    Returns the number of buses that were active last mix.

    $:Driver The driver to poll.
    $:return The number of buses that were active.

    A bus is active when either a playing sample outputs to it,
    another bus outputs to it, or it is active due to a filter
    decaying.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_LOCK RADEXPLINK MilesDriverGetActiveSampleCount(HMILESDRIVER Driver);
/*
    Returns the number of playing samples as of the last mix.

    Don't use this in place of checking for MilesSampleStatusDone on a
    desired sample, as this is more for informative reasons than for
    driving logic.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC void MILES_NO_LOCK RADEXPLINK MilesDriverSetMasterVolume(HMILESDRIVER Driver, F32 MasterVolumeLevel);
/*
    Sets the master volume scalar.

    $:Driver The driver to set the volume for.
    $:MasterVolumeLevel The volume level to set it to. See also $MilesVolumes.

    This volume scalar will be applied to all sound routed through the 
    driver.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC void MILES_NO_LOCK RADEXPLINK MilesDriverSetOptimizedMute(HMILESDRIVER Driver, U32 OptimizedMuteOn);
/*
    Enables a global mute.
    
    $:OptimizedMuteOn If nonzero, all samples will be muted.
    
    This silences all samples and enables certain fast-pass
    optimizations. Note that this is not a pause, and as such filters will decay
    normally, and sources will progress normally.

    This is intended for muting as a result of the app being placed in a "background" 
    state.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesDriverTrackOutputLevels(HMILESDRIVER Driver);
/*
    Marks that the driver's primary output levels are to be tracked for retrieval.

    $:Driver The driver to mark for tracking.

    Once marked, the maximum sample values delivered to the primary output
    for the driver will be tracked. Generally used for debugging and 
    design usage.

    See also $MilesDriverGetOutputLevels.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesDriverGetOutputLevels(HMILESDRIVER Driver, float OutputLevels[MilesMaxOutputChannels]);
/*
    Retrieves the most recent max sample values delivered to the primary output.

    $:Driver A driver with $MilesDriverTrackOutputLevels previously called.
    $:OutputLevels An array of levels to be filled by this function.
    $:return Nonzero if successfully filled.

    Each mix, the driver tracks the maximum sample values delivered to the output,
    independent of actual volume levels set. For instance, if a silent sample
    were played, the output levels would be zero no matter what the volume levels
    affecting the sample were set to.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesDriverGetOutputChannelCount(HMILESDRIVER Driver, U32 OutputIndex);
/*
    Returns the number of channels a given output expects to be delivered to it.

    $:Driver The driver on which the output exists.
    $:OutputIndex The index of the output on the given driver to inspect.

    The channel count for the output is the number of channels it receives from
    Miles, and while in the common case is the same as the number of channels
    the output device delivers to the speakers, if an output filter is installed
    (e.g. SRS Circle Surround), they can differ.
    
    This function is equivalent to calling $MilesOutputGetInputChannelCount($MilesDriverGetOutput(Driver, OutputIndex));

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesDriverGetSampleRate(HMILESDRIVER Driver);
/*
    Returns the primary sample rate.

    $:Driver The driver to poll.
    $:return The sample rate that is used for the primary output on the driver.

    The primary sample rate is the rate buses and filters are run at.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC S32 RADEXPLINK MilesDriverRegisterImpulseResponse(HMILESDRIVER Driver, U32 ConvolutionIndex, void const* BakedImpulseResponse);
/*
    Sets the impulse response for a given index.

    $:Driver The driver to register the impulse for
    $:ConvolutionIndex The index the impulse will be registered for (1 to MilesMaxConvolutions)
    $:BakedImpulseResponse The impulse to register (created using $MilesBakeImpulseResponse). This is copied internally.

    The registered impulse is immediately available for use by a convolution reverb. To use,
    add a convolution filter to a bus or sample, and set the "Impulse" parameter to the index
    in this function.

    The index 0 can never be set, as it always represents an identity convolution.

    Setting the impulse while samples are playing will swap out the convolution without
    any technical issue, however the tail of the previous reverb will be lost. 

    The data pointed at by BakedImpulseResponse is copied internally for aligment reasons.
*/

RADEXPFUNC S32 RADEXPLINK MilesDriverRegisterDecoder(HMILESDRIVER Driver, MilesDecoder const* Decoder);
/*
    Registers a decoder for use in Miles.

    Currently this function is only used internally. Use the direct functions
    to register the desired decoders.

    $MilesDriverRegisterBinkAudio,
    $MilesDriverRegisterMp3,
    $MilesDriverRegisterOgg,

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void MILES_NO_THUNK RADEXPLINK MilesDriverRegisterBinkAudio(HMILESDRIVER Driver);
/*
    Registers the Bink Audio decoder.

    $:Driver The driver on which to enable bink audio decoding.

    The Bink Audio library for the target platform will need to be linked. It is
    provided in the Miles SDK.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void MILES_NO_THUNK RADEXPLINK MilesDriverRegisterMp3(HMILESDRIVER Driver);
/*
    Registers the MP3 decoder.

    $:Driver The driver on which to enable mp3 decoding.

    The Mp3 library for the target platform will need to be linked. Depending
    on the license, this library exists in the Miles SDK.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void MILES_NO_THUNK RADEXPLINK MilesDriverRegisterOgg(HMILESDRIVER Driver);
/*
    Registers the Ogg Vorbis decoder.

    $:Driver The driver on which to enable ogg vorbis decoding.

    The Ogg library for the target platform will need to be linked. It is
    provided in the Miles SDK.

    *Thread Safety* $(Threading:TS_Lock)
*/

typedef struct MilesBufferReference* HMILESBUFFERREFERENCE;
typedef struct MilesCapture* HMILESCAPTURE;

typedef enum MilesCaptureStatus
{
    MilesCaptureStatusOk,
    /*
        The device is providing frames to Miles as expected.
        (This is independent of whether the capture is paused.)
    */

    MilesCaptureStatusBroken,
    /*
        The device has stopped providing frames, and Miles is instead delivering
        zeroes. The device can not be resumed and must be destroyed and recreated
        to actually get nonzero frames.
    */

    MilesCaptureStatusSuspended,
    /*
        The device has stopped providing frames, and Miles is instead delivering
        zeroes. The device may in the future resume providing frames.
    */

    MilesCaptureStatusInvalid,
    /*
        The pointer provided is not a valid capture device.
    */

    MilesCaptureStatusFORCE32 = 0x80000000
} MilesCaptureStatus;

RADEXPFUNC HMILESCAPTURE RADEXPLINK MilesCaptureCreate(void* DeviceSpecificiation);
/*
    *Thread Safety* $(Threading:TS_Lock)
*/

RADEXPFUNC void RADEXPLINK MilesCaptureDestroy(HMILESCAPTURE Capture);
/*
    *Thread Safety* $(Threading:TS_Lock)
*/

RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesCaptureGetFormat(HMILESCAPTURE Capture, U32* OutSampleRate, MilesChannelSpec* OutSpec, U8* OutIsFloat);
/*
    Returns the format of the opened capture device.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

RADEXPFUNC HMILESBUFFERREFERENCE MILES_NO_THUNK RADEXPLINK MilesCaptureDequeue(HMILESCAPTURE Capture);
/*
    If there is as buffer of captured frames available, adds a reference to
    the buffer and returns it. If no frames are available, return zero.

    More than one buffer may be ready at any given time, drain by calling
    multiple times.

    *Thread Safety* $(Threading::TS_ValidDriverNoLock)
*/

RADEXPFUNC MilesCaptureStatus MILES_NO_THUNK RADEXPLINK MilesCaptureGetStatus(HMILESCAPTURE Capture);
/*
    Returns whether the capture device has stopped providing frames
    or is otherwise no longer reliable and should be destroyed.

    *Thread Safety* $(Threading::TS_ValidDriverNoLock)
*/

RADEXPFUNC void MILES_NO_THUNK RADEXPLINK MilesCaptureSetPaused(HMILESCAPTURE Capture, U32 IsPaused);
/*
    Sets whether the given capture will provide frames via $MilesCaptureDequeue.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesCaptureGetPaused(HMILESCAPTURE Capture);
/*
    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

RADEXPFUNC void* MILES_NO_THUNK RADEXPLINK MilesBufferReferenceGetData(HMILESBUFFERREFERENCE Buffer, U32* OutByteCount);
/*
    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

RADEXPFUNC void MILES_NO_THUNK RADEXPLINK MilesBufferReferenceRelease(HMILESBUFFERREFERENCE Buffer);
/*
    Releases the buffer reference. If no references remain, the memory is freed.

    *Thread Safety* $(Threading::TS_ValidDriverNoLock)
*/

//-----------------------------------------------------------------------------
//
// Listener API
//
//-----------------------------------------------------------------------------
IDOC RADEXPFUNC void RADEXPLINK MilesListenerSet3DPosition(HMILESDRIVER Driver, U16 ListenerIndex, F32 X, F32 Y, F32 Z);
/*
    Sets the 3D position of the listener represented by the given listener index.

    $:ListenerIndex An index between 0 and MilesMaxListeners.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesListenerGet3DPosition(HMILESDRIVER Driver, U16 ListenerIndex, F32* X, F32* Y, F32* Z);
/*
    Returns the 3D position of the listener represented by the given listener index.

    $:ListenerIndex An index between 0 and MilesMaxListeners.

    Zero can be passed for any element not needed.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesListenerSet3DOrientation(HMILESDRIVER Driver, U16 ListenerIndex, F32 FaceX, F32 FaceY, F32 FaceZ, F32 UpX, F32 UpY, F32 UpZ);
/*
    Sets the 3D orientation of the listener represented by the given listener index, specified
    via the forward facing vector and the upward facing vector.

    $:ListenerIndex An index between 0 and MilesMaxListeners.

    The default orientation is looking down the position z axis:
    face: (0, 0, 1)
    up: (0, 1, 0);

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesListenerGet3DOrientation(HMILESDRIVER Driver, U16 ListenerIndex, F32* FaceX, F32* FaceY, F32* FaceZ, F32* UpX, F32* UpY, F32* UpZ);
/*
    Returns the 3D orientation of the listener represented by the given listener index.

    $:ListenerIndex An index between 0 and MilesMaxListeners.

    Zero can be passed for any element not needed.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesListenerSet3DVolumeCone(HMILESDRIVER Driver, U16 ListenerIndex, S32 ConeEnabled, F32 InnerAngleDegrees, F32 OuterAngleDegrees, F32 OuterVolumeLevel);
/*
    Sets the 3D cone to use for attenuation as a function of listener angle.

    $:ListenerIndex An index between 0 and MilesMaxListeners.

    If ConeEnabled is zero, the cone parameters will have no effect on sample attenuation.

    See also $MilesSampleSet3DVolumeCone

    *Thread Safety* $(Threading:TS_Lock)
*/

//-----------------------------------------------------------------------------
//
// Output API
//
//-----------------------------------------------------------------------------
//idoc(parent, OutputAPI)
IDOC RADEXPFUNC HMILESOUTPUT RADEXPLINK MilesOutputCreate(HMILESDRIVER Driver, U32 SampleRate, MilesChannelSpec ChannelSpec, HMILESHARDWAREOUTPUT HardwareOutput, UINTa HardwareOutputParam);
/*
    Creates an additional speaker output on the given driver.

    $:Driver The driver to create the output on.
    $:SampleRate The frequency at which samples are delivered to the hardware.
    $:ChannelSpec The number of channel to output, and their configuration.
    $:HardwareOutput The hardware device to output to, or zero for the default 
        device for the platform.
    $:HardwareOutputParam An optional parameter to pass to hardware device creation.
    $:return A handle to the new output device.

    The new output is appended to the list of outputs on the driver. Once created,
    buses and samples can use the return from this function to route audio to the
    represented hardware device. This allows for samples and buses to output to
    more than one 

    Some platforms or devices may have channel count or sample rate limitations
    (or are limited to only one output!). 

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void* RADEXPLINK MilesOutputDestroy(HMILESOUTPUT Output);
/*
    Removes the given output from the driver and frees associated resources.

    $:Output The output to destroy.
    $:return A pointer to the resource still holding a reference to the output,
        or zero if successfully destroyed.

    *Generally, this function is never necessary to call*. Destroying the containing
    driver via $MilesDriverDestroy will also destroy the output, removing any
    concern about potential "gotchas".

    This function WILL FAIL if the output is still part of the overall 
    audio graph. Prior to calling, there must be no buses and no samples
    directed to the given output.

    Calling this function on the primary output for a driver is undefined
    and will likely cause crashes during driver destruction.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC char const* RADEXPLINK MilesOutputGetName(HMILESOUTPUT Output);
/*
    Returns a human readable string identifying the output API in use.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesOutputGetProperty(HMILESOUTPUT Output, MilesOutputProperty Property, void* PointerToResult);
/*
    Provides access to low level properties of the hardware output API.

    See $MilesOutputProperty for what to provide for PointerToResult.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC F32 RADEXPLINK MilesOutputGetLatency(HMILESOUTPUT Output);
/*
    Returns the number of milliseconds Miles queues on to the output.

    This function currently does not include any latency caused by the 
    operating system or the underlying hardware output API.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesOutputSetCaptureFn(HMILESOUTPUT Output, MilesCaptureFnType* CaptureFn, void* CaptureFnUser);
/*
    Sets the function to call for capturing all audio delivered to the output.

    $:Output The output for which to capture audio.
    $:CaptureFn The function to call with the audio each mix, or zero to remove.
    $:CaptureFnUse A user pointer to pass to the capture function.

    Each mix, if a capture function is registered on the output, the function
    is called with a pointer to the audio data about to be passed to the 
    hardware output API. See also $MilesCaptureFnType for size details.

    The function is called from the mixer thread.

    When the output is closed, the function is called with a null pointer
    for the audio buffer.

    *Thread Safety* $(Threading:TS_Lock)
*/


IDOC RADEXPFUNC void RADEXPLINK MilesOutputSetMasterVolume(HMILESOUTPUT Output, F32 MasterVolumeLevel);
/*
    Sets the master volume level for all audio passing to this output.

    While similar to the driver master volume ($MilesDriverSetMasterVolume), this is
    only applied to audio on the given output. Since most applications will only
    have one output, there's generally no reason to use this over the driver
    master volume.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC MilesChannelSpec RADEXPLINK MilesOutputGetChannelSpec(HMILESOUTPUT Output);
/*
    Returns the channel specification the output was opened with.

    Returns the "ChannelSpec" parameter passed to $MilesOutputCreate or
    $MilesDriverCreate.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesOutputGetInputChannelCount(HMILESOUTPUT Output);
/*
    Returns the number of channels the output device receives.

    Generally, an output's input and output channels are the same. However
    if an output filter is installed, then it is possible for the output to
    expect a different number of channels to be delivered to it, when compared
    to the number of channels it delivers to the speakers.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesOutputGetOutputChannelCount(HMILESOUTPUT Output);
/*
    Returns the number of channels the output device delivers to speakers.

    Generally, an output's input and output channels are the same. However
    if an output filter is installed, then it is possible for the output to
    expect a different number of channels to be delivered to it, when compared
    to the number of channels it delivers to the speakers.

    *Thread Safety* $(Threading:TS_Lock)
*/

//-----------------------------------------------------------------------------
//
// Bus API
//
//-----------------------------------------------------------------------------
//idoc(parent, BusAPI)
IDOC RADEXPFUNC HMILESBUS RADEXPLINK MilesBusCreate(HMILESDRIVER Driver, U16 ChannelCount, HMILESOUTPUTORBUS PrimaryOutput, F32 VolumeLevel);
/*
    Creates a bus which can be used to manipulate audio passing through it.

    $:ChannelCount The number of channels the bus supports.
    $:PrimaryOutput The default output for the bus. Must be either another bus, or a hardware output. 
        Zero will use the driver's default hardware output.
    $:VolumeLevel The volume level to apply to all channels passed through the bus.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesBusDestroy(HMILESBUS Bus);
/*
    Destroys the given bus.

    Destroying a bus that is still referenced by a sample or another bus 
    will fail.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC HMILESROUTE RADEXPLINK MilesBusGetRoute(HMILESBUS Bus, U32 RouteIndex);
/*
    Returns the output route for the given index on given bus.

    $:Bus The bus to inspect.
    $:RouteIndex The index of the route to retrieve.
    $:return The requested route, or zero if the bus or route doesn't exist.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesBusGetRouteCount(HMILESBUS Bus);
/*
    Returns the number of output routes on the given bus.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesBusSetVolumeLevel(HMILESBUS Bus, F32 VolumeLevel);
/*
    Sets the volume level to apply to all channels that pass through this bus.

    The volume level defaults to 1 (0 dB).

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC F32 RADEXPLINK MilesBusGetVolumeLevel(HMILESBUS Bus);
/*
    Returns the volume level previously set via $MilesBusSetVolumeLevel.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesBusSetLFEVolumeLevel(HMILESBUS Bus, F32 LFEVolumeLevel);
/*
    Sets the volume level to apply to the lfe channel, if one exists.

    The LFE volume level defaults to 1 (0 dB).

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC F32 RADEXPLINK MilesBusGetLFEVolumeLevel(HMILESBUS Bus);
/*
    Returns the volume level previously set via $MilesBusSetLFEVolumeLevel.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesBusSetPanLevels(HMILESBUS Bus, F32 FrontLevel, F32 BackLevel, F32 LeftLevel, F32 RightLevel, F32 CenterLevel);
/*
    Sets the panning control levels to use on panned outputs.

    $:Sample The sample to set the controls for.
    $:FrontLevel The gain to apply to front speakers.
    $:BackLevel The gain to apply to back speakers.
    $:LeftLevel The gain to apply to left speakers.
    $:RightLevel The gain to apply to right speakers.
    $:CenterLevel The gain to apply to center speakers.

    The panning scalars are only used on routes that are designated as panned. By default,
    only routes that have a mono source and a non-mono destination are marked as panned.
    See also $MilesSampleSetPanLevels.

    If more than one of the levels passed to this function applies to a speaker,
    then they will be accumulated. For instance, the front left speaker will use
    the level FrontLevel * LeftLevel.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesBusSetPanLeftRight(HMILESBUS Bus, F32 PanPot);
/*
    Sets the left and right panning levels based on a constant power (-3db law) pan.

    $:Bus The bus to set the pan controls for.
    $:PanPot The pan control value, between -1 and 1, with -1 being hard left pan.

    The panning scalars are only used on routes that are designated as panned.
    See also $MilesRouteSetPanned.

    The center channel, if present, is not affected by this pan.

    Setting a neutral pan will result in a -3db gain for all left and right speakers.
    This function can be combined with $MilesBusSetPanFrontBack.

    To return a bus to completely pass-through panning, use 
    $MilesBusSetPanLevels(Bus, 1, 1, 1, 1, 1);

    See also $MilesBusSetPanLevels.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesBusSetPanFrontBack(HMILESBUS Bus, F32 PanPot);
/*
    Sets the front and back panning levels based on a constant power (-3db law) pan.

    $:Bus The bus to set the pan controls for.
    $:PanPot The pan control value, between -1 and 1, with -1 being hard back pan.

    The panning scalars are only used on routes that are designated as panned.
    See also $MilesRouteSetPanned.

    The center channel, if present, is not affected by this pan.

    Setting a neutral pan will result in a -3db gain for all front and back speakers.
    This function can be combined with $MilesBusSetPanLeftRight.

    To return a bus to completely pass-through panning, use 
    $MilesBusSetPanLevels(Bus, 1, 1, 1, 1, 1);

    See also $MilesBusSetPanLevels.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesBusSetPan360(HMILESBUS Bus, F32 PanDegrees);
/*
    Sets the front, back, left, and right panning levels based on a 360 degree pan control.

    $:Bus The bus to set the pan controls for.
    $:PanDegrees The theta, in degrees, to represent the audio source. 0 degrees represents
        in front of the listener, 90 degrees represents to the right of the listener.

    The panning scalars are only used on routes that are designated as panned.
    See also $MilesRouteSetPanned.

    The center channel, if present, is not affected by this pan.

    To return a bus to completely pass-through panning, use 
    $MilesBusSetPanLevels(Bus, 1, 1, 1, 1, 1);

    See also $MilesBusSetPanLevels.    

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC HMILESROUTE RADEXPLINK MilesBusCreateRoute(HMILESBUS Bus, HMILESOUTPUTORBUS OutputOrBus, F32 VolumeLevel);
/*
    Adds another path for audio on to the given bus.

    $:Bus The bus to add the route to.
    $:OutputOrBus The bus or output to route audio to.
    $:VolumeLevel The amount to attenuate the audio on the route.

    On creation every bus has one route that passes audio to either a bus
    or a hardware output destination. This function adds another route. For
    the first additional route, this has the effect of doubling the audio
    passing through the bus.

    See also $MilesRouteCreate.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC HMILESFILTERSTAGE RADEXPLINK MilesBusAddFilterByName(HMILESBUS Bus, char const* FilterName);
/*
    Adds a filter to the bus filter chain.

    Filters are added with default values for all properties. Wet level defaults to
    1, and dry level defaults to zero.

    See also $MilesDefaultFilters for filter names.

    See also $MilesBusAddFilter.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC HMILESFILTERSTAGE RADEXPLINK MilesBusAddFilter(HMILESBUS Bus, HMILESFILTER Filter);
/*
    Adds a filter to the bus filter chain.

    Filters are added with default values for all properties. Wet level defaults to
    1, and dry level defaults to zero.

    See also $MilesFilterGetByName, $MilesBusAddFilterByName.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesBusGetChannelCount(HMILESBUS Bus);
/*
    Returns the number of channels the bus was created with.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC F32 RADEXPLINK MilesBusGetMaxSignalLevel(HMILESBUS Bus);
/*
    Returns the max signal level across all channels for the most recent mix.

    This value includes the strength of the audio source, and isn't just
    the volume for the bus. For example, all volumes applied to the audio
    could be at full strength, but if the audio passing through the bus
    is silent, this function will return 0.

    This function doesn't return a valid value until $MilesBusWantMaxSignalLevel
    has been called.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesBusWantMaxSignalLevel(HMILESBUS Bus);
/*
    Marks that the max signal level is desired for this bus.

    Once called, $MilesBusGetMaxSignalLevel will return a valid value.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesBusTrackInputLevels(HMILESBUS Bus);
/* 
    Marks that a bus should track its input levels.

    Used for informative/design purposes.

    See also $MilesBusGetInputLevels.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesBusGetInputLevels(HMILESBUS Bus, float* InputLevels);
/*
    Retrieves the input levels for a bus.

    $:Bus The bus for which $MilesBusTrackInputLevels has been called.
    $:InputLevels A pointer to an array of floats of length $MilesBusGetChannelCount.
    $:return A bitmask indicating whether the bus or driver has mixed.

    Generally used for auditioning and debugging.

    The return of this function represents what has happened to the InputLevels
    array.

    $* If neither the bus nor the driver have mixed, then this function
        returns zero, and the InputLevels array is not touched.
    $* If the driver has mixed, but the bus has not due to not being part of 
        the active audio graph, the function returns 2, and the InputLevels
        array is zeroed (as no audio passed through the bus).
    $* If both the driver and the bus have mixed, the InputLevels is filled
        and this function returns 3.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesBusSetMuted(HMILESBUS Bus, U32 IsMuted);
/*
    Sets whether a bus is muted.

    $:Bus The bus to mute.
    $:IsMuted Nonzero to mute the bus.

    Prevents all audio passing through this bus from reaching the speakers.
    Generally used for debugging and auditioning, acting like the "mute" 
    found on a channel strip.

    See Also $MilesBusSetSolo.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesBusSetSolo(HMILESBUS Bus, U32 IsSolo);
/*
    Sets whether a bus is solod.

    $:Bus The bus to mark as solo.
    $:IsSolo Nonzero to set the bus as solo.

    A solod bus routes its *primary output* to the hardware output. Sends
    are not audible. Multiple buses can be solod, and they will all be copied
    assuming they are not muted via $MilesBusSetMuted.

    The mixing route for solod data is not optimized and is designed for
    use during the production process.

    See Also $MilesBusSetMute.

    *Thread Safety* $(Threading:TS_Lock)
*/

   
//-----------------------------------------------------------------------------
//
// Route API
//
//-----------------------------------------------------------------------------
//idoc(parent, RouteAPI)
IDOC RADEXPFUNC HMILESROUTE RADEXPLINK MilesRouteCreate(HMILESSAMPLEORBUS FromSampleOrBus, HMILESOUTPUTORBUS ToOutputOrBus, F32 VolumeLevel);
/*
    Adds a new route for audio from a sample/bus to an output/bus.

    $:FromSampleOrBus A HMILESSAMPLE or HMILESBUS that acts as the audio source.
    $:ToOutputOrBus A HMILESBUS or HMILESOUTPUT to route the audio to.
    $:VolumeLevel The volume level to scale any audio on the route by.
        See also $MilesVolumes.
    $:return The handle to the route, or zero.

    Routes are pathways for audio to travel. The most basic route is from
    a sample to the default hardware output. This route is created implicitly
    when a new sample is created on the default output. However, more 
    advanced routing is possible, from having a sample only output one
    stream of audio through multiple buses before arriving at the default
    output, to having additional copies of the audio created at any point
    in the chain, ultimately arriving at one or more hardware outputs.

    Since the channel counts of the source and destination might not be
    the same, routes are also responsible for managing any downmixing that
    might be necessary. If the channel counts match, then all channels are
    passed through without gain, aside from the route's VolumeLevel.

    Routes have four different settings for managing audio that passes through it: 
    Direct, Panned, Spatialized, and Mixed.

    By default, the route is _Spatialized_ if the destination has a spatialization
    specification (set via $MilesBusCreate) and the source is a Sample, _Direct_ if 
    the source and destination have the same number of channels, _Panned_ if the 
    source is mono and the destination is not, and _Mixed_ otherwise.

    \<pre\>
    
    // Create a couple of buses to output to.
    HMILESBUS Bus1 = $MilesBusCreate(Driver, 0, 0, 1.0f);
    HMILESBUS Bus2 = $MilesBusCreate(Driver, 0, 0, 1.0f);

    // Create a sample, with an initial route to Bus1.
    HMILESSAMPLE Sample = $MilesSampleCreate(Driver, Bus1);

    // The route was created for us, since all samples and buses
    // must have an initial route. We can access it if desired:
    HMILESROUTE RouteToBus1 = $MilesSampleGetRoute(Driver, 0);

    // Add another route from the sample to the second bus.
    HMILESROUTE RouteToBus2 = $MilesRouteCreate(Sample, Bus2, 1.0f);

    \</pre\>

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesRouteSetVolumeLevel(HMILESROUTE Route, F32 VolumeLevel);
/*
    Sets the volume scalar for the given route.

    $:Route The route to update volume for.
    $:VolumeLevel The volume level to set the route to. See also $MilesVolumes.

    All audio that passes through this route gets scaled by the given volume.
    This is the same value passed in to $MilesRouteCreate.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesRouteSetDelaySamples(HMILESROUTE Route, U16 DelaySampleCount);
/*
    Sets the delay on the route.

    $:Route The route to delay.
    $:DelaySampleCount The number of samples to delay.

    The audio is delayed after filtering, but before any volume effects are applied -
    meaning if the volume of the sample is dramatically changed during the delay,
    the delayed audio will have the new volume, not the volume at the time of the delay.

    Note that delay buffers can get very big, very fast. Total byte count is
    4 * RouteInputChannels * DelaySampleCount. A stereo 100 ms delay clocks in
    around 38k, which adds up fast across dozens of samples with multiple delayed
    routes.

    Changing the delay amount during playback will lose the current delay buffer.
*/

IDOC RADEXPFUNC F32 RADEXPLINK MilesRouteGetVolumeLevel(HMILESROUTE Route);
/*
    Returns the volume scalar for the given route.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesRouteSetLFELevel(HMILESROUTE Route, F32 LFEVolumeLevel);
/*
    Sets the relative volume for the LFE channel on this route.

    $:Route The route to update volume for.
    $:LFEVolumeLevel The volume level to additionally scale the LFE channel by.
        See also $MilesVolumes.

    This exists as a convenience function to adjust the LFE volume independently.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesRouteGetOutputChannels(HMILESROUTE Route);
/*
    Returns the channel count of the destination for the route.

    $:Route The route for which to check the destination channel count.

    This is equivalent to calling the channel count accessor for whatever
    was passed in to $MilesRouteCreate. E.g. $MilesBusGetChannelCount or
    $MilesOutputGetInputChannelCount.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesRouteGetInputChannels(HMILESROUTE Route);
/*
    Returns the channel count of the source for the route.

    $:Route The route for which to check the source channel count.

    This is equivalent to calling the channel count accessor for whatever was
    passed in to $MilesRouteCreate. E.g. $MilesSampleGetChannelCount or
    $MilesBusGetChannelCount.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesRouteSetPanned(HMILESROUTE Route);
/*
    Sets the given route to pan audio passing through it using the source's panning controls.

    See $MilesRouteCreate for a list of the different route types.

    This function will convert the route to be _Panned_ if it isn't already.

    For a Panned route, each channel is treated as a mono source and mixed to 
    each destination channel based on the panning levels set on the audio's
    source. See also $MilesSampleSetPanLevels and $MilesBusSetPanLevels.

    See also $MilesRouteSetDirect, $MilesRouteSetSpatialized, and $MilesRouteSetMixed.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesRouteSetDirect(HMILESROUTE Route);
/*
    Sets the given route to be a direct mapping between source and destination.

    See $MilesRouteCreate for a list of the different route types.

    This function will have no effect if the source and destination channel counts
    do not match.

    A _Direct_ route scales each channel's volume by the relevant scalar and adds
    it only to the matching channel on the destination. As a result, a Direct route
    cannot add audio from the front left speaker to any other speaker.

    See also $MilesRouteSetPanned, $MilesRouteSetSpatialized, and $MilesRouteSetMixed.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesRouteSetSpatialized(HMILESROUTE Route, HMILESOUTPUT Output);
/*
    $:Output The output to spatialize for, or zero for the driver's default output.

    Sets the given route to spatialize the audio passing through it based
    on the destination's spatialization specification.

    See $MilesRouteCreate for a list of the different route types.

    This function will have no effect if the destination has no spatialization specification,
    or if the source is not a sample.

    A _Spatialized_ route uses the sample's position and listener specification
    as well as the destinations speaker specification to pan and attenuate the
    audio so that it has directionality. If the sample is multi channel, then
    each channel is panned per the sample's multi channel pan controls.

    See also $MilesRouteSetPanned, $MilesRouteSetDirect, and $MilesRouteSetMixed,
    $MilesSampleSet3DMultiChannelPan

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesRouteSetMixed(HMILESROUTE Route, F32* CustomMixLevels);
/*
    Sets the given route to down or up mix the source's channels to the destination's channels.

    $:CustomMixLevels An optional pointer to a matrix of volume levels defining how audio is
        to pass from each source channel to each destination channel, or zero to use the Miles
        default mix levels. The matrix index will be generated with [DestChannelIndex + DestChannelCount*SourceChannelIndex].

    See $MilesRouteCreate for a list of the different route types.

    This function allows for a completely custom routing of audio. Any channel can
    be sent to any other channel with any volume level. If, upon creation, the 
    route uses this type, Miles creates sane defaults that map speakers as best it
    can.

    Example - Sets the mix for a stereo source to a 5.1 destination to only
        place the audio in the front speakers.
    \<pre\>
    
        // A stereo source with a 5.1 destination means a 2x6 matrix.
        F32 Mix[2*6];

        // Default all routing to silence.
        memset(Mix, 0, sizeof(Mix));

        // Route the 0th source channel (front left) to the 
        // 0th destination channel (front left) at full volume.
        Mix[0 + 6*0] = 1.0f;

        // Route the 1st source channel (front right) to the
        // 1st destination channel (front right) at full volume.
        Mix[1 + 6*1] = 1.0f;

        // This mix only works for stereo to 5.1!
        assert($MilesRouteGetInputChannels(Route) == 2);
        assert($MilesRouteGetOutputChannels(Route) == 6);

        $MilesRouteSetMixed(Route, Mix);

    \</pre\>

    Example - Route the left channel to the front speakers and
        the right channel to the back speakers (stereo -> 5.1)
    \<pre\>
    
        // A stereo source with a 5.1 destination means a 2x6 matrix.
        F32 Mix[2*6];

        // Default all routing to silence.
        memset(Mix, 0, sizeof(Mix));

        // Route the 0th source channel (front right) to the
        // 0th dest channel (front left) and 1st dest channel
        // (front right)
        Mix[0 + 6*0] = 0.5f;
        Mix[1 + 6*0] = 0.5f;

        // Route the 1st source channel (front right) to the 
        // 4th channel (back left) and 5th channel (back right)
        Mix[4 + 6*1] = 0.5f;
        Mix[5 + 6*1] = 0.5f;

        // This mix only works for stereo to 5.1!
        assert($MilesRouteGetInputChannels(Route) == 2);
        assert($MilesRouteGetOutputChannels(Route) == 6);

        $MilesRouteSetMixed(Route, Mix);

    \</pre\>

    See also $MilesRouteSetPanned, $MilesRouteSetDirect, and $MilesRouteSetMixed.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesRouteRedirect(HMILESROUTE Route, HMILESOUTPUTORBUS NewOutput);
/*
    Redirects the given route such that it points to the new output. In effect,
    recreates the route. See $MilesRouteCreate for restriction on the output.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC S32 RADEXPLINK MilesRouteDestroy(HMILESROUTE Route);
/*
    Removes the given route from the source that uses it.

    The default route cannot be destroyed.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesRouteTrackOutputLevels(HMILESROUTE Route);
/*
    Marks that the given route should track its output levels.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesRouteGetOutputLevels(HMILESROUTE Route, F32* OutputLevels);
/*
    Returns the output levels for the given route.

    $:Route A route for which $MilesRouteTrackOutputLevels has been called.
    $:OutputLevels A pointer to an array of floats of length $MilesRouteGetOutputChannels.
    $:return Whether the route or driver has been mixed.

    This function acts per $MilesBusGetInputLevels, except for a route's
    output levels.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC HMILESROUTE RADEXPLINK MilesRouteGetNext(HMILESROUTE Route);
/*
    Returns the next route on the given route's source.

    Each sample or bus has a list of routes that it uses to output audio.
    This function will retrieve the route after the given one, in that list,
    or zero if there are no more routes. See also $MilesSampleGetRoute, 
    $MilesBusGetRoute.

    *Thread Safety* $(Threading:TS_Lock)
*/

//-----------------------------------------------------------------------------
//
// Sample API
//
//-----------------------------------------------------------------------------
//idoc(parent, SampleAPI)
IDOC RADEXPFUNC HMILESSAMPLE RADEXPLINK MilesSampleCreate(HMILESDRIVER Driver, HMILESOUTPUTORBUS PrimaryOutput, U8 PrimaryOutputIs3D);
/*
    Creates a blank sample with its primary output set to the given bus or hardware output.

    $:Driver The driver the sample will output to.
    $:PrimaryOutput The primary output audio will route to, or 
        zero for the driver's default hardware output.
    $:PrimaryOutputIs3D Audio output will be spatialized to the channel count of the destination.
    $:return a sample handle for use in generating audio within the Miles Sound System.

    Samples are the sources for audio data within Miles. From Samples, audio will
    travel along Routes to either Buses or Outputs. All samples have at least one
    Route, which directs audio to the output specified. Additional routes can
    be added via $MilesSampleCreateRoute, allowing a single sample to output
    its audio to multiple buses, or even multiple hardware endpoints.

    Once created, a sample needs to have a "source". In general, this will
    be a "buffered" source, where the audio data is loaded by the game code
    in to one or more buffers, and then provided to the sample. Streamed sources
    are also common, where Miles manages the buffering internally and streams
    directly from a file in to a small buffer.

    To set a buffered source, use $MilesSampleSetSource, and if the audio data
    is split across buffers, use $MilesSampleAddSource to append them. 

    To set a streamed source, use $MilesSampleSetSourceStream. Miles opens the
    file in the background, so results or metadata about the sample will not be
    available immediately.

    The usage pattern for a basic sound in a game is:

    $* $MilesSampleCreate to get a sample handle.
    $* $MilesSampleSetSource to tell Miles where to get
        the audio data.
    $* $MilesSampleAddFilter to add any desired effects.
    $* $MilesSampleSetLoopSamples / $MilesSampleSetLoopCount to set up looping.
    $* $MilesSamplePlay to start the sample.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleDestroy(HMILESSAMPLE Sample);
/*
    Destroys a sample and frees all associated resources.

    $:Sample The sample to destroy.

    Frees all filters and routes, as well as the sample itself. If the sample
    was playing, the output is not faded out and may clip if the last delivered
    audio sample was at a large DC offset.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC MilesSampleStatus RADEXPLINK MilesSampleGetStatus(HMILESSAMPLE Sample);
/*
    Returns the current sample status.

    $:Sample The sample to poll.
    $:return The current sample status.

    Generally used to determine when a sample has completed by checking for
    MilesSampleStatusDone. 

    If a sample has encountered an unrecoverable error, then the sample is
    latched to an error state and must be recreated. The reason for the
    error is output to the reporting function (see $MilesStartup) at
    the time of the error, and this function returns MilesSampleStatusError
    until the sample is destroyed.

    The only other potential gotcha is if the sample has a stream source
    set to it ($MilesSampleSetSourceStream), and the async open hasn't
    completed yet. During this time, if the sample is marked to play
    ($MilesSamplePlay), then the status will not return MilesSampleStatusPlaying,
    it will return MilesSampleStatusOpenThenPlay.

    *Thread Safety* $(Threading:TS_Lock)
*/

// Source
IDOC RADEXPFUNC S32 RADEXPLINK MilesSampleSetSource(HMILESSAMPLE Sample, void* Buffer, U32 BufferLen, MilesFileType Type);
/*
    Sets a sample to use a buffered source.

    $:Sample The sample to set. The sample must not already have a source.
    $:Buffer A pointer to the memory for the audio data.
    $:BufferLen The total bytes pointed at by "Buffer".
    $:Type The type of audio file pointed at by "Buffer". Generally MilesFileTypeAuto works.
    $:return Nonzero on success.

    Sources can only be set on samples that don't already have a source
    set.

    The memory provided must remain valid over the lifetime of the sample - 
    Miles does not copy the audio data.

    This function takes the provided buffer and attempts to parse metadata
    about the audio. If the file type is unrecognized or otherwise fails to
    parse, this function fails. On success, the sample is set up and ready
    to play whatever data is provided. If the entirety of the file is not
    provided, the sample will still start when $MilesSamplePlay is called, 
    expecting more data to be provided in time via $MilesSampleAddSource.

    If the data source is compressed, the decoder is opened and initialized.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC S32 RADEXPLINK MilesSampleSetSourceDiscards(HMILESSAMPLE Sample);
/*
    Marks that once a sample's buffers have been used, they are released.

    $:Sample A sample with a buffer source to mark as consuming buffers.
    $:return Nonzero on success.

    If the game is managing streaming of data itself, then in order
    to keep memory usage down, a sample can be marked as discarding
    a buffer once it's been decoded and played.

    However, in this case, if a sample is seeked, then Miles no longer
    has the ability to deliver the correct samples. As a result, 
    Miles has to ask the code responsible for managing the buffers
    to add a new buffer (via $MilesSampleAddSource) where the start
    of the buffer holds the data at the requested byte position in
    the source audio file.

    Note that no more than MilesMaxSampleBuffers can be queued at one
    time. Once this function successfully completes, you can stream
    more than that limit through the sample, as long as sufficient
    previous buffers have been completed.

    Streamed (or otherwise non buffered) samples cannot be used
    with this function. See also $MilesSampleSetSource.

    If the source is already set to consume, this function can be
    used to update the callback.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC S32 RADEXPLINK MilesSampleAddBuffer(HMILESSAMPLE Sample, void* Buffer, U32 BufferLen);
/*
    Adds another buffer for play on a sample with a buffered source.

    $:Sample A sample with a buffered source set via $MilesSampleSetSource, or a discarding source
        set via $MilesSampleSetSourceDiscards.
    $:Buffer A pointer to the memory containing the audio data.
    $:BufferLen The total bytes pointer at by "Buffer"
    $:return Nonzero on success.

    Samples can have a maximum of "MilesMaxSampleBuffers" set at once
    for play. For basic buffered samples, this is a hard limit. If 
    the sample "consumes" its buffers, the game can wait for a previous
    buffer to complete to add the next. See also $MilesSampleSetSourceDiscards.

    The memory pointed at by "bytes" must be valid over the course of 
    the lifetime of the sample.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC S32 RADEXPLINK MilesSampleSetSourceStream(HMILESSAMPLE Sample, char const* FileName, U32 StreamBufferSize, void* PreloadBuffer, U32 PreloadBufferLen);
/*
    Sets a sample's source to be a file streamed in over time.

    $:Sample A sample with no previous source set.
    $:FileName The file to stream in. The filename can adhere to the Miles 
        subfile format. See also $MilesFilenames.
    $:StreamBufferSize The total amount of memory to use for the stream, 
        or zero for platform default.
    $:PreloadBuffer A pointer to the first bytes of the file, if already 
        loaded via other means, or zero.
    $:PreloadBufferLen The total number of bytes pointed at by PreloadBuffer.
    $:return Nonzero on success.

    Starts an async operation to open the file to stream. Since the data is 
    not immediately available (even with a preload buffer), the sample's status
    is marked MilesSampleStatusOpening. If the game needs the sample to start
    playing as soon as the file is opened and ready, a $MilesSamplePlay will
    set the sample's status to MilesSampleStatusOpenThenPlay, where it will remain
    until the mixer thread processes the opening.

    In general it's fine to leave the stream buffer size as zero. The default size
    will be on the order of 32 kb of data. If skipping occurs, then increase it
    as necessary.

    Preloading exists to provide a way to start a streamed sample without having
    to wait for file io. If used, there should be enough data to supply demand
    for whatever the disc io latency is for the target platform.

    If a disc error occurs while the stream is running, the driver is placed in
    an error state and no further reads (from anywhere) will take place. See
    $MilesDriverGetReadError.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC S32 RADEXPLINK MilesSampleSetSourceRaw(HMILESSAMPLE Sample, void* Buffer, U32 BufferLen, U32 SampleRate, U16 ChannelCount);
/*
    Sets a sample's source to be a raw source of samples without any metadata.

    $:Sample A sample with no previous source set.
    $:Buffer A buffer of signed 16 bit samples.
    $:BufferLen The total number of *bytes* pointed at by Buffer.
    $:SampleRate The rate at which to play the samples.
    $:ChannelCount The number of channels to play.
    $:return Nonzero on success.

    Sets the sample to play a buffer of raw samples with no header data.
    The audio data must be a 16 bit signed PCM source, with packed channels.
    E.g. For a 6 channel source, the first 12 bytes are [short L, short R, short C, 
    short LFE, short BL, short BR].

    \<pre\>
    
        // Set a 4 sample source for a sample that generates silence.
        static U64 Silence = 0;
        $MilesSampleSetSourceRaw(Sample, &Silence, sizeof(Silence), 48000, 1);
        $MilesSampleSetLoopCount(Sample, 0); // output silence until stopped.

    \</pre\>

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSetEOBCallback(HMILESSAMPLE Sample, MilesSampleEOBFnType* EndOfBufferFn, void* User);
/*
    Sets the function to call when a buffer is completed.

    $:Sample The sample to set the callback for.
    $:EndOfBufferFn The function to call on buffer completion.
    $:User A pointer to pass to the end of buffer function.

    Once the last byte has been copied from a given buffer, if this callback is
    set, the function will be called with the buffer pointer originally
    passed to the function that set the Source.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesSampleGetBufferCount(HMILESSAMPLE Sample);
/*
    Returns the current number of buffers the sample has ready.

    $:Sample The sample to poll.
    $:return the buffer count.

    For non consuming samples, this will be equal to the number of buffers
    the game has added. If consuming, this value will represent the number
    of buffers that remain. If this goes to zero, the sample starves.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesSampleGetDurationSamples(HMILESSAMPLE Sample);
/*
    Returns the length of a sample with a set source in samples

    $:Sample The sample to inspect. Must have a valid source.
    $:return The sample count for the sample's source, or 0 if not available.

    This function only works if the source is a format that provides this information.
    For example, loading a standard MP3 will not provide this, as there is no seek
    information. When Miles Studio compresses to MP3, a seek table is added, 
    allowing for proper seeking when used in the high level. Currently, the only
    way to get such file is to compress them in Miles Studio and retrieve the 
    MP3 file from the audition temp directory.

    See also $MilesSampleGetDurationMs.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesSampleGetDurationMs(HMILESSAMPLE Sample);
/*
    Returns the length of a sample with a set source in milliseconds.

    See $MilesSampleGetDurationSamples for documentation.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC F32 RADEXPLINK MilesSampleGetSampleRate(HMILESSAMPLE Sample);
/*
    Returns the playback rate for a sample's source. Returns zero
    if no source has been set.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesSampleGetChannelCount(HMILESSAMPLE Sample);
/*
    Returns the channel count for a sample's source. Returns zero
    if no source has been set.

    *Thread Safety* $(Threading:TS_Lock)
*/

// Output Management
IDOC RADEXPFUNC HMILESROUTE RADEXPLINK MilesSampleCreateRoute(HMILESSAMPLE Sample, HMILESOUTPUTORBUS Output, F32 VolumeLevel, U8 RouteIs3D);
/*
    Adds another path for audio to the given sample.

    $:Sample The sample to add the route to.
    $:Output The bus or output to route audio to.
    $:VolumeLevel The amount to attenuate the audio on the route.
    $:RouteIs3D Audio output on this route will be spatialized to the channel count of
        the destination.

    On creation every sample has one route that passes audio to either a bus
    or a hardware output destination. This function adds another route. For
    the first additional route, this has the effect of doubling the audio
    the sample produces.

    See also $MilesRouteCreate.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC HMILESROUTE RADEXPLINK MilesSampleGetRoute(HMILESSAMPLE Sample, U32 RouteIndex);
/*
    Returns a route on a given sample.

    If the route index is beyond the count of routes on the sample ($MilesSampleGetRouteCount), or
    the sample is zero, this function returns zero.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesSampleGetRouteCount(HMILESSAMPLE Sample);
/*
    Returns the number of output routes on a given sample.

    *Thread Safety* $(Threading:TS_Lock)
*/

// Volumes
IDOC RADEXPFUNC void RADEXPLINK MilesSampleSetVolumeLevel(HMILESSAMPLE Sample, F32 VolumeLevel);
/*
    Sets the volume applied to all outputs and all channels for the sample.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC F32 RADEXPLINK MilesSampleGetVolumeLevel(HMILESSAMPLE Sample);
/*
    Returns the volume applied to all outputs and all channels for the sample.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSetLFEVolumeLevel(HMILESSAMPLE Sample, F32 LFEVolumeLevel);
/*
    Sets the volume applied to the LFE channel for all outputs on the sample.

    This volume is in addition to the volume set by $MilesSampleSetVolumeLevel.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC F32 RADEXPLINK MilesSampleGetLFEVolumeLevel(HMILESSAMPLE Sample);
/*
    Returns the volume applied to the LFE channel for all outputs on the sample.

    This volume is in addition to the volume set by $MilesSampleSetVolumeLevel.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSetPanLevels(HMILESSAMPLE Sample, F32 FrontLevel, F32 BackLevel, F32 LeftLevel, F32 RightLevel, F32 CenterLevel);
/*
    Sets the panning control levels to use on panned outputs.

    $:Sample The sample to set the controls for.
    $:FrontLevel The gain to apply to front speakers.
    $:BackLevel The gain to apply to back speakers.
    $:LeftLevel The gain to apply to left speakers.
    $:RightLevel The gain to apply to right speakers.
    $:CenterLevel The gain to apply to center speakers.

    The panning scalars are only used on routes that are designated as panned.
    See also $MilesRouteSetPanned.

    This functions provides a lower level of access compared to $MilesSampleSetPanLeftRight
    and $MilesSampleSetPanFrontBack, and doesn't do any power-pan computations. The
    given levels are used directly.

    If more than one of the levels passed to this function applies to a speaker,
    then they will be accumulated. For instance, the front left speaker will use
    the level FrontLevel * LeftLevel.

    Generally, this function will be used to set a zero gain pass through on a route
    by passing all ones.

    Note that the LFE level is unaffected by this pan. Use $MilesSampleSetLFEVolumeLevel to
    control the LFE.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSetPanLeftRight(HMILESSAMPLE Sample, F32 PanPot);
/*
    Sets the left and right panning levels based on a constant power (-3db law) pan.

    $:Sample The sample to set the pan controls for.
    $:PanPot The pan control value, between -1 and 1, with -1 being hard left pan.

    The panning scalars are only used on routes that are designated as panned.
    See also $MilesRouteSetPanned.

    The center channel, if present, is not affected by this pan.

    Setting a neutral pan will result in a -3db gain for all left and right speakers.
    This function can be combined with $MilesSampleSetPanFrontBack.

    To return a sample to completely pass-through panning, use 
    $MilesSampleSetPanLevels(Sample, 1, 1, 1, 1, 1);

    See also $MilesSampleSetPanLevels.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSetPanFrontBack(HMILESSAMPLE Sample, F32 PanPot);
/*
    Sets the front and back panning levels based on a constant power (-3db law) pan.

    $:Sample The sample to set the pan controls for.
    $:PanPot The pan control value, between -1 and 1, with -1 being hard back pan.

    The panning scalars are only used on routes that are designated as panned.
    See also $MilesRouteSetPanned.

    The center channel, if present, is not affected by this pan.

    Setting a neutral pan will result in a -3db gain for all front and back speakers.
    This function can be combined with $MilesSampleSetPanLeftRight.

    To return a sample to completely pass-through panning, use 
    $MilesSampleSetPanLevels(Sample, 1, 1, 1, 1, 1);

    See also $MilesSampleSetPanLevels.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSetPan360(HMILESSAMPLE Sample, F32 PanDegrees);
/*
    Sets the front, back, left, and right panning levels based on a 360 degree pan control.

    $:Sample The sample to set the pan controls for.
    $:PanDegrees The theta, in degrees, to represent the audio source. 0 degrees represents
        in front of the listener, 90 degrees represents to the right of the listener.

    The panning scalars are only used on routes that are designated as panned.
    See also $MilesRouteSetPanned.

    The center channel, if present, is not affected by this pan.

    To return a sample to completely pass-through panning, use 
    $MilesSampleSetPanLevels(Sample, 1, 1, 1, 1, 1);

    See also $MilesSampleSetPanLevels.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC F32 RADEXPLINK MilesSampleGetMaxSignalLevel(HMILESSAMPLE Sample);
/*
    Returns the max signal level across all channels for the most recent mix.

    This value includes the strength of the audio source, and isn't just
    the volume for the sample. For example, all volumes applied to the audio
    could be at full strength, but if the audio leaving the sample
    is silent, this function will return 0.

    This function will not return a valid value unless $MilesSampleWantMaxSignalLevel has
    been called.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleWantMaxSignalLevel(HMILESSAMPLE Sample);
/*
    Marks that the max signal level is desired for this sample.

    Once called, $MilesSampleGetMaxSignalLevel will return a valid value.

    *Thread Safety* $(Threading:TS_Lock)
*/

// Playhead
IDOC RADEXPFUNC void RADEXPLINK MilesSamplePlay(HMILESSAMPLE Sample);
/*
    Sets a sample to start supplying audio to its outputs.

    If the sample has completed ($MilesSampleStatus returns MilesSampleStatusDone),
    then the sample is rewound ($MilesSampleRewind) and started again. A sample
    that is currently playing will NOT be rewound.

    If the sample is currently undergoing an asynchronous opening operation,
    then it will queue the play command to occur once the opening has
    completed. (MilesSampleStatusOpening will become MilesSampleStatusOpenThenPlay)

    This function has no effect on a sample with no source set.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSamplePause(HMILESSAMPLE Sample);
/*
    Stops a sample from supplying audio to its outputs.

    A sample stopped this way could potentially clip if the last level
    output was fairly high. If the sample doesn't need to be recycled
    immediately, consider $MilesSamplePauseFade.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSamplePauseFade(HMILESSAMPLE Sample);
/*
    Stops a sample from supplying audio to its outputs, fading the 
    output over one mix to prevent clipping. The sample will still
    return MilesSampleStatusPlaying during the fade.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleRewind(HMILESSAMPLE Sample);
/*
    Moves the play cursor to the start of the sample's source.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSamplePlayScheduled(HMILESSAMPLE Sample, U64 MixSampleToStartAt);
/*
    Schedules the given miles sample to start on the given output audio sample.

    $:Sample The sample to schedule to start.
    $:MixSampleToStartAt The output sample to start adding this samples audio to.

    This function allows sample-accurate scheduling of samples. In order to 
    schedule a correct time, get the current mix time via $MilesDriverGetMixedSamples,
    add the desired samples to that, and pass the result as MixSampleToStartAt.
    Note that without $MilesLock and $MilesUnlock around both functions,
    it's possible for the thread to get preempted and the mixed sample count
    to increase between the two calls.

    If the desired time is in the past, the sample will be started on the
    next mix.

    The samples status will remain MilesSampleStatusPaused prior to start.

    *Thread Safety* $(Threading:TS_Lock)
*/

RADEXPFUNC void RADEXPLINK MilesSampleSetId(HMILESSAMPLE Sample, U32 Id);
/*
    Assigns this sample a group id for synchronized starting.

    For synchronized playhead control, all samples with the same id are
    managed together via $MilesSamplePlayGroup, $MilesSamplePauseGroup,
    $MilesSamplePauseFadeGroup, and $MilesSampleRewindGroup. This guarantees
    all of the samples are controlled together without requiring a lock
    across several individual calls.

    By default samples are assigned id 0.

    *Thread Safety* $(Threading:TS_Lock)
*/

RADEXPFUNC void RADEXPLINK MilesSamplePlayGroup(HMILESDRIVER Driver, U32 StartId, U32 SetId);
/*
    Calls $MilesSamplePlay on all samples on the driver that match StartId, and
    sets the sample's id to SetId.

    *Thread Safety* $(Threading:TS_Lock)
*/

RADEXPFUNC void RADEXPLINK MilesSamplePauseGroup(HMILESDRIVER Driver, U32 PauseId, U32 SetId);
/*
    Calls $MilesSamplePause on all samples on the driver that match PauseId, and
    sets the sample's id to SetId.

    *Thread Safety* $(Threading:TS_Lock)
*/

RADEXPFUNC void RADEXPLINK MilesSamplePauseFadeGroup(HMILESDRIVER Driver, U32 PauseId, U32 SetId);
/*
    Calls $MilesSamplePauseFade on all samples on the driver that match PauseId, and
    sets the sample's id to SetId.

    *Thread Safety* $(Threading:TS_Lock)
*/

RADEXPFUNC void RADEXPLINK MilesSampleRewindGroup(HMILESDRIVER Driver, U32 RewindId, U32 SetId);
/*
    Calls $MilesSampleRewind on all samples on the driver that match RewindId, and
    sets the sample's id to SetId.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSetPlaybackRateFactor(HMILESSAMPLE Sample, F32 RateFactor);
/*
    Sets the amount to scale the playback rate for this sample.

    The playback rate factor has the effect of increasing or decreasing the pitch
    of the sample, at the cost of increasing or decreasing the duration of 
    the sample. To convert from a semitone to a playback rate factor, use
    $MilesStToScalar.

    The rate factor affects all outputs for the sample.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC F32 RADEXPLINK MilesSampleGetPlaybackRateFactor(HMILESSAMPLE Sample);
/*
    Returns the current playback rate for a sample previously set via $MilesSampleSetPlaybackRateFactor.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleInsertSilence(HMILESSAMPLE Sample, U32 SamplesOfSilence);
/*
    Inserts silence for a number of audio samples in to the sample's playback.

    Generally used to delay the start of a sample to an exact time.

    Use the following to determine how many samples are needed for a given amount of milliseconds:

    \<pre\>

    U32 SilenceSampleCount = (U32)(SilenceInMilliseconds * 
        MilesSampleGetSampleRate(Sample) / 1000.0f);

    \</pre\>

    The sample must have an alternate source set - this function cannot be used
    generate silence in isolation. See $MilesSampleSetSourceRaw for an example of a sample
    that produces only silence.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSetLowPassCutoff(HMILESSAMPLE Sample, F32 CutoffHz);
/*
    Sets the low pass cutoff for the built-in low pass filter.

    All samples have a built in low pass cutoff filter that is disabled by default.
    Setting the cutoff for this filter to a value lower than 23khz will enable
    this filter.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSetDopplerFactor(HMILESSAMPLE Sample, F32 DopplerEffectiveness, F32 const* ListenerVelocity, F32 const* SampleVelocity, F32 MetersPerGameUnit);
/*
    Computes and sets the Doppler factor for a sample.

    $:Sample The sample to set the Doppler factor for.
    $:DopplerEffectiveness The amount that the Doppler shift should affect 
        this sample, between 0 and 1.
    $:ListenerVelocity The 3d velocity of the listener. Ignored if DopplerEffectiveness
        is zero. You can pass null in place of (0, 0, 0).
    $:SampleVelocity The 3d velocity of the actor. Ignored if DopplerEffectiveness
        is zero. You can pass null in place of (0, 0, 0).
    $:MetersPerGameUnit Scaling factor to convert distances and velocities 
        from game units to real world meters. Ignored if DopplerEffectiveness
        is zero.

    Doppler shift is the effect where a sound gets higher in pitch as
    it approaches, and lower in pitch as it departs. As a result, it's a function
    of the relative velocities of the sound and the listener. This function
    acts as a helper to compute the Doppler shift factor as well as set it for
    a sample.

    If the DopplerEffectiveness parameter is zero, the other math-relevant 
    parameters are ignored, so you don't need to come up with values if you 
    are just disabling Doppler.

    The underlying implementation for Doppler factor is as a playback rate
    adjustment, and as a result applies to all outputs for the sample.

    This should be updated every frame - Miles does no internal integration
    of position.

    This functions uses the sample's 3D position set via $MilesSampleSet3DPosition
    and the position of the listener assigned to the sample via $MilesListenerSet3DPosition.
    
    Currently, the first listener index is used if more than one is set via
    $MilesSampleSetListenerMask.

    *Thread Safety* $(Threading:TS_Lock)
*/


IDOC RADEXPFUNC void RADEXPLINK MilesSampleSetListenerMask(HMILESSAMPLE Sample, U32 ListenerMask);
/*
    Sets which listeners will be used when spatializing this sample.

    When outputting to a spatialized bus, the resulting per-speaker volumes are
    accumulated for each listener in the mask.

    If the sample does not have an output to a bus created as 3d, then this function
    has no effect.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSet3DPosition(HMILESSAMPLE Sample, F32 X, F32 Y, F32 Z);
/*
    Sets the position this sample will use for 3d spatialized audio.

    If the sample does not have an output to a bus created as 3d, then this function
    has no effect.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSet3DOrientation(HMILESSAMPLE Sample, F32 FaceX, F32 FaceY, F32 FaceZ, F32 UpX, F32 UpY, F32 UpZ);
/*
    Sets the orientation this sample will use for 3d spatialized audio.

    The orientation of a sample has no effect if there is no cone falloff set
    via $MilesSampleSet3DVolumeCone.

    If the sample does not have an output to a bus created as 3d, then this function
    has no effect.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSet3DAutoSpreadDistance(HMILESSAMPLE Sample, F32 AutoSpreadDistancePct);
/*
    Specifies at what distance to start automatically adjusting the 3d spread.

    When the sample approaches the listener, Miles automatically "spreads" the effect of 
    spatialization across adjacent speakers in order to prevent sudden ear switches 
    when the sample passes the listener (for mono sources)

    Automatic spread is disabled if a 3d spread graph is enabled, or if zero is passed
    to this function.

    If the sample does not have a spatialized output route, this function has no effect.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSet3DMultiChannelPan(HMILESSAMPLE Sample, F32 PanDegrees, F32 FoldDistance);
/*
    Specifies the amount, per channel pair, to pan each channel when the sample is within the given fold down distance.

    When a sample has more than one channel, the game can opt to pan each channel to the sides of the
    listener. By default this is zero, resulting in all channels being spatialized such that they sound 
    like they are all on top of each other, at the sample's 3d position. This function allows each
    channel to be panned to the left and right of the listener if the sample is close by.

    This works by taken each channel in sequence and alternating between adding/subtracting the 
    pan degrees from a baseline of zero, and then lerping the result towards zero based on how far the
    sample is from the listener, where a distance at or beyond FoldDistance is zero, and a distance
    of zero is the resulting pan amount from the original calculation.

    If the number of channels is odd, the remainder channel at the end has a pan of zero.

    In general, this is used for stereo sources, and PanDegrees will be something like 
    90. This results in a pan for the first channel (left) as 90, and the second channel
    (right) as -90. With a fold distance of 100, if the sample were at a distance of 10,
    for example, the pan would be 10 percent of the way to the fold distance 
    (10 distance / 100 fold distance), resulting in a pan of 81 degrees to the left
    and right (90 * (100 - 10 pct)).

    If there are more channel, this continues to add, so a quad channel could have
    PanDegrees be 60, and the resulting channels will be panned to 60, -60, 120, -120.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSet3DSpreadGraph(HMILESSAMPLE Sample, MilesGraphPoint* Points, S32 PointCount);
/*
    Sets the graph to evaluate to determine what 3d spread to use when spatializing the sample.

    $:Points A pointer to an array of points representing the spread.
    $:PointCount The number of points referenced by Points.

    This will copy the data from the array in to internal memory.

    The graph converts the distance the sample is at to a spread value to use
    between 0 and 1.

    Spread is a value representing how much to conceptually "spread" audio to
    adjacent speakers. A value of 1 effectively removes spatialization, as 
    the audio is spread to all speakers. It is useful when samples pass near the
    listener to prevent sudden jumps between speakers, or to increase the feeling
    of volume for the sound's source.

    The spread is evaluated separately for each output, as some outputs
    might not be spatialized.

    Spread is only applied for mono sources - multi channel pan is used otherwise.

    If the sample does not have a spatialized output, this function has no effect.

    See also $MilesSampleSet3DMultiChannelPan

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSet3DLowPassGraph(HMILESSAMPLE Sample, MilesGraphPoint* Points, S32 PointCount);
/*
    Sets the graph to evaluate to determine what low pass to use for the sample.

    $:Points A pointer to an array of points representing the low pass cutoff.
    $:PointCount The number of points referenced by Points.

    This will copy the data from the array in to internal memory.

    The graph converts the distance the sample is at to the low pass cutoff 
    value to use, in hertz: [10, 20000]. 

    The low pass is evaluated separately for each output, as some outputs
    might not be spatialized, and is in addition to the low pass set via
    $MilesSampleSetLowPassCutoff.

    If the sample does not have a spatialized output, this function has no effect.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSet3DVolumeGraph(HMILESSAMPLE Sample, MilesGraphPoint* Points, S32 PointCount);
/*
    Sets the graph to evaluate to determine what volume to use for the sample.

    $:Points A pointer to an array of points representing the volume.
    $:PointCount The number of points referenced by Points.

    This will copy the data from the array in to internal memory.

    The graph converts the distance the sample is at to the volume value 
    to use, in *decibels*: [-96, 12]. 

    The volume is evaluated separately for each output, as some outputs
    might not be spatialized.

    If the sample does not have a spatialized output, this function has no effect.

    This example will set the sample to fall off linearly in decibels over
    a distance of 1200 game units.

    \<pre\>

    // Set the sample's decibel volume as a linear function of distance.
    MilesGraphPoint linear_points[2] = {0};
    linear_points[0].X = 0;
    linear_points[0].Y = 0; // Full volume when adjacent.
    linear_points[0].OType = MilesTangentLinear;
    // ITX,ITY,OTX,OTY are ignored for linear tangents.

    linear_points[1].X = 1200;
    linear_points[1].Y = -96.0f; // Zero volume at 1200
    linear_points[1].IType = MilesTangentLinear;
    // ITX, ITY, OTX, OTY are ignored for linear tangents

    $MilesSampleSet3DVolumeGraph(Sample, linear_points, 2);

    \</pre\>

    This example will set the sample to fall off more naturally:
    \<pre\>

    // Use the Fade Out tangent, which also overrides 
    // the input tangent for the next point
    MilesGraphPoint fade_points[2] = {0};
    fade_points[0].X = 0;
    fade_points[0].Y = 0; // Full volume when adjacent.
    fade_points[0].OType = MilesTangentFadeOut;

    fade_points[1].X = 1200;
    fade_points[1].Y = -96.0f;

    $MilesSampleSet3DVolumeGraph(Sample, fade_points, 2);

    \</pre\>

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSet3DVolumeCone(HMILESSAMPLE Sample, S32 ConeEnabled, F32 InnerAngleDegrees, F32 OuterAngleDegrees, F32 OuterVolumeLevel);
/*
    Sets the cone attenuation for a given sample.

    $:Sample The sample to update
    $:ConeEnabled Whether to use the cone falloff for this sample. If zero, the other parameters are ignored.
    $:InnerAngleDegrees The angle to begin the falloff, from 0 to 180. 
    $:OuterAngleDegrees The angle to complete the falloff, from 0 to 180.
    $:OuterVolumeLevel The volume level to attenuate by at or beyond the outer angle.

    This function provides a way to attenuate the sample as a function of its facing direction.

    If the sample's orientation is such that it's face vector (see $MilesSampleSet3DOrientation)
    is pointed directly at the listener, the angle used for this calculation will be 0. If 
    it's pointed directly away, the result will be 180.

    If the above angle is less than InnerAngleDegrees, then the sample's 3d volume 
    (the result from evaluating $MilesSampleSet3DVolumeGraph) will be unaffected. Between 
    InnerAngleDegrees and OuterAngleDegrees, the 3d volume will be further attenuated
    based on where it is in that range. As the angle approaches OuterAngleDegrees, the
    volume approaches OuterVolumeLevel. Past the outer angle, the volume remains
    at OuterVolumeLevel.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSetLoopCount(HMILESSAMPLE Sample, S32 Count);
/*
    Sets the number of times the sample will play. Pass zero to loop forever.

    If the loop count is more than one, the sample's playhead will return to the loop start
    position when the loop end position is reached. The loop count is then decremented. The 
    sample is stopped when there are no loops remaining.

    See also $MilesSampleSetLoopSamples, $MilesSampleSetLoopBytes.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSetLoopSamples(HMILESSAMPLE Sample, S32 StartSample, S32 EndSample);
/*
    Sets the loop start and end positions for a sample, in audio samples.

    $:StartSample The position, in samples, to move the playhead to when EndSample is reached.
    $:EndSample The position, is samples, that will move the playhead to StartSample when reached.

    If the sample's source is compressed and does not have a seek table, this function will fail.
    At this time the Miles Ogg Vorbis support does not provide a seek table. MP3 compressed
    with Miles Studio will, but normal MP3s will not (See $MilesSampleGetDurationSamples)

    To remove sample loop boundaries, call $MilesSampleSetLoopBytes(Sample, 0, -1);

    Note that a sample that has its play cursor before the loop start will not jump
    to the loop start when the loop boundary is set - the position has to be set separately.

    See also $MilesSampleSetLoopCount.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSetLoopBytes(HMILESSAMPLE Sample, S32 StartBytes, S32 EndBytes);
/*
    Sets the loop start and end position for a sample, in bytes.

    $:StartBytes The byte position to start the loop at, or 0.
    $:EndBytes The byte position to end the loop at, or -1 for no loop endpoint.

    If the sample is compressed, this often will not have the desired effect as
    the byte position must be on a block boundary. If looping of a part of a file
    is desired, then a format with a seek table should be used with $MilesSampleSetLoopSamples,
    instead.

    Loop boundaries can be removed with this function via $MilesSampleSetLoopBytes(Sample, 0, -1);

    Note that a sample that has its play cursor before the loop start will not jump
    to the loop start when the loop boundary is set - the position has to be set separately.    

    See also $MilesSampleSetLoopCount.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSetPositionBytes(HMILESSAMPLE Sample, U32 BytePosition);
/*
    Sets the sample's play cursor to the given byte position.

    If the sample is compressed, this will likely not have the desired effect
    as the position must be on a block boundary. If accurate positioning is required
    of a compressed file, then a format with a seek table should be used, and 
    $MilesSampleSetPositionSamples called instead.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSetPositionSamples(HMILESSAMPLE Sample, U32 SamplePosition);
/*
    Sets the sample's play cursor to the given sample position.

    If the sample is compressed, the file must provide a seek table.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesSampleSetPositionMs(HMILESSAMPLE Sample, S32 MsPosition);
/*
    Sets the sample's play cursor to the given position, in milliseconds.

    The function has all of the same restrictions as $MilesSampleSetPositionSamples,
    as it simply computes the desired target sample using the sample's sample rate.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesSampleGetPositionMs(HMILESSAMPLE Sample);
/*
    Returns the current position of the sample, in milliseconds.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC HMILESFILTERSTAGE RADEXPLINK MilesSampleAddFilter(HMILESSAMPLE Sample, HMILESFILTER Filter);
/*
    Adds a filter to the sample's filter chain.

    Filters are added with default values for all properties. Wet level defaults to
    1, and dry level defaults to zero.

    See also $MilesFilterGetByName, $MilesSampleAddFilterByName.

    *Thread Safety* $(Threading:TS_Lock)
*/


IDOC RADEXPFUNC HMILESFILTERSTAGE RADEXPLINK MilesSampleAddFilterByName(HMILESSAMPLE Sample, char const* FilterName);
/*
    Adds a filter to the sample's filter chain.

    Filters are added with default values for all properties. Wet level defaults to
    1, and dry level defaults to zero.

    See also $MilesDefaultFilters for filter names.

    See also $MilesSampleAddFilter.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC HMILESFILTERSTAGE RADEXPLINK MilesSampleGetFilter(HMILESSAMPLE Sample, U32 FilterIndex);
/*
    Returns the filter stage installed at the given index.

    If there is no filter at the given index, returns 0.

    *Thread Safety* $(Threading:TS_Lock)
*/


//-----------------------------------------------------------------------------
//
// File API
//
//-----------------------------------------------------------------------------
//idoc(parent, FileAPI)
IDOC RADEXPFUNC void* MILES_NO_LOCK RADEXPLINK MilesFileReadEx(char const* FileName, void* DestBufferOrFlags, char const* SourceFile, U32 SourceLine);
/*
    Synchronously reads a file from disc - generally use the $MilesFileRead wrapper.

    $:FileName The file to read on disc. See also $MilesFilenames.
    $:DestBufferOrFlags A pointer to read the file in to, which must be large enough
        to contain the file's contents. Additionally, zero can be passed which will
        cause Miles to allocate a buffer of the necessary size, or MilesFileReadWithSize
        can be passed here - see discussion.
    $:SourceFile The file that requested the read.
    $:SourceLine The line that requested the read.
    $:return The buffer container the data, which if supplied by the caller, will equal
        DestBufferOrFlags. If allocated by Miles, this must be freed by MilesFree.

    Generally, $MilesFileRead should be used to avoid manually passing file and line information.

    The most common use for this function is to pass MilesFileReadWithSize as the
    DestBufferOrFlags parameter. This causes Miles to internally allocate
    a buffer sized to the file's size, with an extra 4 bytes to store the file size.

    As a result, if the file is larger than 4GB, this function will fail to open it,
    and MilesAsyncFileRead should be used instead.

    If the file is read successfully, then the first 4 bytes will contain the size,
    and the file data exists just past that.

    \<pre\>
    
    U32* DataWithSize = MilesFileRead("path/to/file", MilesFileReadWithSize);
    if (DataWithSize)
    {
        U32 Size = DataWithSize[0];
        void* FileData = (DataWithSize + 1);
    }
    
    \</pre\>

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

#define MilesFileRead(fn, buffer) MilesFileReadEx(fn, buffer, __FILE__, __LINE__) IDOC // Wrapper to automatically pass __FILE__ and __LINE__
#define MilesFileReadWithSize ((void*)(SINTa)-1) IDOC // Signals to $MilesFileReadEx to prepend the size of the file to the result data.

#define MilesIoFlagsDontCloseHandle             1
#define MilesIoFlagsQuerySizeOnly               2
#define MilesIoFlagsDontUseOffset               4
#define MilesIoFlagsQuerySize                   8
#define MilesIoFlagsSingleReadCall              16

IDOC typedef enum MilesIoStatus
{
    MilesIoStatusRunning = 0, /* 
        The io is still pending completion.
    */

    MilesIoStatusComplete = 0x0001, /*
        The io has completed successfully and the data is waiting in the buffer.
    */

    MilesIoStatusErrorMask = 0x1000,

    MilesIoStatusErrorFailedOpen = 0x1003, /*
        The io failed because the file could not be opened (did not exist).
    */
    MilesIoStatusErrorFailedRead = 0x1004, /*
        The io failed because a read failed after the file was successfully opened.
    */
    MilesIoStatusErrorShutdown = 0x1005, /*
        The io failed because the async system was shut down.
    */
    MilesIoStatusErrorCanceled = 0x1006, /*
        The io request was cancelled prior to completion.
    */
    MilesIoStatusErrorAllocFailed = 0x1007, /*
        The io request failed because the allocation for the output buffer failed.
    */

    MilesIoStatusFORCE32 = 0x80000000
} MilesIoStatus;

IDOC RADEXPFUNC char const* MILES_NO_THUNK RADEXPLINK MilesIoStatusToString(U32 IoStatus);
/*
    Returns a string representation of $MilesIoStatus errors.

    The returned string should not be deleted.

    *Thread Safety* $(Threading:TS_Pure)
*/

struct MilesAsyncRead;

// returns percent full (1.0 = 100%)
typedef F32             (RADLINK MilesAsyncGetStreamStatusFnType)(void* i_User);
typedef S32             (RADLINK MilesAsyncFileReadFnType)(struct MilesAsyncRead* i_Request);
typedef S32             (RADLINK MilesAsyncFileCancelFnType)(struct MilesAsyncRead* i_Request); // 1 if the request has completed, 0 otherwise. Use Wait if needed.
typedef U32             (RADLINK MilesAsyncFileStatusFnType)(struct MilesAsyncRead* i_Request, U32 i_MS); // 1 if complete, 0 if timeout exceeded.
typedef S32             (RADLINK MilesAsyncStartupFnType)();
typedef void            (RADLINK MilesAsyncShutdownFnType)();
typedef void            (RADLINK MilesAsyncSetPausedFnType)(S32 i_IsPaused);
typedef S32             (RADLINK MilesAsyncGetThreadHandleFnType)(void* out_handle);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef struct RR_ALIGNED_STRUCT(8) MilesAsyncRead
{
    char FileName[256];
    U64 Offset;
    U64 FileSize; // Only valid if MilesIoFlagQuerySize or MilesIoFlagQuerySizeOnly set.
    S64 Count;
    U64 volatile ReadAmt; // current read amt. Read only from caller!
    U64 Internal[4];
    UINTa FileHandle;
    void* Buffer;
    void* StreamUserData;
    struct MilesDriver* MemoryQuotaOwner;
    MilesAsyncGetStreamStatusFnType* StreamCB;
    char const* CallerFile;
    U32 CallerLine;    
    S32 Flags;
    S32 AdditionalBuffer;
    S32 volatile Status; // This is only valid after a call to MilesAsyncFileWait or MilesAsyncFileCancel has succeeded.
} MilesAsyncRead;

RADEXPFUNC S32   MILES_NO_LOCK        RADEXPLINK MilesAsyncFileRead(struct MilesAsyncRead* i_Request);
RADEXPFUNC S32   MILES_NO_LOCK        RADEXPLINK MilesAsyncFileCancel(struct MilesAsyncRead* i_Request); // 1 if the request has completed, 0 otherwise. Use Wait if needed.
RADEXPFUNC U32   MILES_NO_LOCK        RADEXPLINK MilesAsyncFileStatus(struct MilesAsyncRead* AsyncRead, U32 MsToWait);
RADEXPFUNC S32   MILES_NO_LOCK        RADEXPLINK MilesAsyncGetThreadHandle(void* out_handle);
RADEXPFUNC void  MILES_NO_LOCK        RADEXPLINK MilesAsyncSetPaused(S32 i_IsPaused);

IDOC RADEXPFUNC void MILES_NO_THUNK RADEXPLINK MilesAsyncGetProfiling(F32* OutCpuPercent, U64* OutBytesRead, U32* OutSeekCount);
/*
    Returns profiling information for the async io system since the last call.
    
    $:OutCpuPercent The percentage of the core the async io system is on it took. This includes time spent in synchronous IO.
    $:OutBytesRead The number of bytes that have been read.
    $:OutSeekCount The number of seeks (excluding file opens) that have occurred.
    
    Zero can be passed for any parameter not desired. All numbers are from the last call
    to MilesAsyncGetProfiling.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

RADEXPFUNC void  MILES_NO_LOCK        RADEXPLINK MilesAsyncSetCallbacks(
                            MilesAsyncFileReadFnType* read, MilesAsyncFileCancelFnType* cancel, MilesAsyncFileStatusFnType* status, MilesAsyncStartupFnType* startup,
                            MilesAsyncShutdownFnType* shutdown, MilesAsyncSetPausedFnType* setpaused, MilesAsyncGetThreadHandleFnType* threadhandle);

IDOC typedef U32 RADLINK MilesFileOpenFnType(char const* FileName, UINTa OutFileHandle[MilesFilePassBlockCount], U64* OutFileSize);
/*
    The synchronous open function signature for Miles.

    $:FileName The name of the file to open.
    $:OutFileHandle The handle block to fill out with any relevant handle data.
    $:OutFileSize The total size of the file. This must be filled out.
    $:return 1 if the file open succeeded, 0 on failure.

    The file handle data is there to place any data you need to track the file
    in your file system. For a basic C standard implementation, this could be
    as straightforward as the FILE* pointer.
*/

IDOC typedef void RADLINK MilesFileSeekFnType(UINTa FileHandle[MilesFilePassBlockCount], U64 OffsetFromStart);
/*
    The synchronous seek function signature for Miles.

    $:FileHandle The handle block filled out with a previous open call.
    $:OffsetFromStart The offset, from the start of the file, to set the file pointer to.

    Note that all seeks from Miles will be from the start of the file (like SEEK_SET in
    the c standard).
*/

IDOC typedef void RADLINK MilesFileCloseFnType(UINTa FileHandle[MilesFilePassBlockCount]);
/*
    The synchronous close function signature for Miles.

    $:FileHandle The handle block filled out with a previous open call.
*/

IDOC typedef U64 RADLINK MilesFileReadFnType(UINTa FileHandle[MilesFilePassBlockCount], void* DestBuffer, U64 BytesToRead);
/*
    The synchronous read function signature for Miles.

    $:FileHandle The handle block filled out with a previous open call.
    $:DestBuffer The buffer to read in to.
    $:BytesToRead The number of bytes requested.
    $:return The number of bytes read.
*/

IDOC RADEXPFUNC void MILES_NO_LOCK RADEXPLINK MilesFileGetCallbacks(MilesFileOpenFnType** OutOpenFn, MilesFileCloseFnType** OutCloseFn, MilesFileSeekFnType** OutSeekFn, MilesFileReadFnType** OutReadFn);
/*
    Returns the current set of functions used for serialized I/O.

    $:OutOpenFn Returns the current open function pointer.
    $:OutCloseFn Returns the current close function pointer.
    $:OutSeekFn Returns the current seek function pointer.
    $:OutReadFn Returns the current read function pointer.

    Generally just used for chaining file callbacks - see $MilesFileSetCallbacks.
*/

IDOC RADEXPFUNC void MILES_NO_LOCK RADEXPLINK MilesFileSetCallbacks(MilesFileOpenFnType* OpenFn, MilesFileCloseFnType* CloseFn, MilesFileSeekFnType* SeekFn, MilesFileReadFnType* ReadFn);
/*
    Sets the functions to use for serialized I/O.

    $:OpenFn The open function call.
    $:CloseFn The close function call.
    $:SeekFn The seek function call.
    $:ReadFn The read function call.

    In general, this is the function used for overriding the Miles file I/O.

    All Miles I/O is routed through the async I/O system, which eventually boils down
    to calls to these synchronous file calls on the async i/o thread. As a result, any
    calls to these functions MUST be thread-safe.

    The file handle blocks are meant to hold any file handle data. For a basic C io
    system, The FILE* pointer could simply be placed in the first entry (FileHandle[0])
    Miles provides some extra space if additional state is needed.

    If only a subset of files are handled, unhandled files can be passed to the
    previous callbacks, retrieved via $MilesFileGetCallbacks. However,
    be sure to filter in *all* calls, not just the open call.
*/


//-----------------------------------------------------------------------------
//
// Filter API
//
//-----------------------------------------------------------------------------
//idoc(parent, FilterAPI)
#define MilesFilterValueMemoryNeeded        0x1001
//#define MilesFilterValueOutputChannels      0x1002
//#define MilesFilterValueRequiresOutput      0x1003
#define MilesFilterValuePropertyCount       0x1004
#define MilesFilterValuePropertyPtr         0x1005
#define MilesFilterValueDecaySamples        0x1006
#define MilesFilterValueDecays              0x1007
#define MilesFilterValueUsesRawSource       0x1008
#define MilesFilterValueChannelsJoined      0x1009

typedef struct MilesFilterPipeFunctions
{
    MilesFilterProcessFnType* ProcessFn;
    MilesFilterOpenFnType* OpenSampleFn;
    MilesFilterGetValueFnType* GetValueFn;
    MilesFilterSetPropertyFnType* SetPropFn;
    MilesFilterGetPropertyFnType* GetPropFn;
    MilesFilterCloseFnType* CloseFn;
} MilesFilterPipeFunctions;

//
//typedef struct MilesFilterMatrixFunctions
//{
//    void* ProcessFn;
//    void* OpenOutputFn;
//    void* GetValue;
//    void* GetProperties;
//} MilesFilterMatrixFunctions;
//


RADEXPFUNC S32 RADEXPLINK MilesFilterRegister(HMILESDRIVER Driver, MilesFilter const* Filter);

IDOC RADEXPFUNC S32 RADEXPLINK MilesFilterEnum(HMILESDRIVER Driver, UINTa* Token, HMILESFILTER* Filter);
/*
    Enumerates the filters installed on the given driver.

    $:Token An enumeration token, initialized to zero for the first call.
    $:Filter A pointer to a filter handle that will be filled in on a successful call.
    $:return 1 if the current enumeration is valid and should continue.

    \<pre\>

    UINTa enum_token = 0;
    HMILESFILTER filter = 0;
    while (MilesFilterEnum(driver, &enum_token, &filter))
    {
        printf("%s\n", filter->name);
    }
    
    \</pre\>

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC HMILESFILTER RADEXPLINK MilesFilterGetByName(HMILESDRIVER Driver, char const* FilterName);
/*
    Returns the filter handle for a filter installed on the given driver with the given name,
    or zero on fail.

    *Thread Safety* $(Threading:TS_Lock)
*/

enum MilesFilterFlags
{
    MilesFilterFlagDb           = 0x1,
    MilesFilterFlagInteger      = 0x2
};

typedef struct MilesFilterProperty
{
    char const* name;
    char const* help;
    F32 def, min, max;
    U32 flags;
} MilesFilterProperty;

RADEXPFUNC S32 RADEXPLINK MilesFilterGetPropertyCount(HMILESFILTER Filter);
RADEXPFUNC HMILESFILTERPROPERTY RADEXPLINK MilesFilterGetProperties(HMILESFILTER Filter);

IDOC RADEXPFUNC HMILESFILTERPROPERTY RADEXPLINK MilesFilterGetPropertyByName(HMILESFILTER Filter, char const* PropertyName);
/*
    Returns the property matching the given name for the given filter.

    The result of this function can be used to set the value for the property
    via $MilesFilterSetPropertyValue. Returns zero if no property with the
    name exists for the filter.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC F32 RADEXPLINK MilesFilterGetPropertyValue(HMILESFILTERSTAGE FilterStage, HMILESFILTERPROPERTY Property);
/*
    Returns the current value for the property on the given filter stage.

    $:FilterStage The result from a previous call to $MilesSampleAddFilter or $MilesBusAddFilter.
    $:Property The result from a previous call to $MilesFilterGetPropertyByName.

    The wet and dry level for the filter stage are available separately, via $MilesFilterGetWetLevel
    and $MilesFilterGetDryLevel, as they are not managed by the filter.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC S32 RADEXPLINK MilesFilterSetPropertyValue(HMILESFILTERSTAGE FilterStage, HMILESFILTERPROPERTY Property, F32 Value);
/*
    Sets the value for the given property on the given filter stage.

    $:FilterStage The result from a previous call to $MilesSampleAddFilter or $MilesBusAddFilter.
    $:Property The result from a previous call to $MilesFilterGetPropertyByName.
    $:Value The new value to use for the property.
    $:return Zero if the property doesn't exist on the filter stage.

    The wet and dry level for the filter stage are set separately, via $MilesFilterSetWetDryLevels,
    as they are not managed by the filter.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC S32 RADEXPLINK MilesFilterSetPropertyValueByName(HMILESFILTERSTAGE FilterStage, char const* PropertyName, F32 Value);
/*
    A convenience function to set a filter's property by name. Equivalent to calling $MilesFilterSetPropertyValue
    with the return from $MilesFilterGetPropertyByName.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesFilterSetWetDryLevels(HMILESFILTERSTAGE FilterStage, F32 WetLevel, F32 DryLevel);
/*
    Sets the wet and dry levels for the given filter stage.

    The wet level controls the attenuation for the output from the filter,
    and the dry level controls the attenuation for the unfiltered path.

    By default, the dry level is set to zero, and the wet level is set to 1,
    resulting in a fully filtered signal. Setting both the dry and wet level
    to 1 will result in an overall gain in the signal.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC F32 RADEXPLINK MilesFilterGetWetLevel(HMILESFILTERSTAGE FilterStage);
/*
    Returns the current wet level for the given filter stage.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC F32 RADEXPLINK MilesFilterGetDryLevel(HMILESFILTERSTAGE FilterStage);
/*
    Returns the current dry level for the given filter stage.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesFilterRemove(HMILESFILTERSTAGE FilterStage);
/*
    Removes the filter stage from the filter chain containing it, freeing
    all associated memory.

    *Thread Safety* $(Threading:TS_Lock)
*/

//-----------------------------------------------------------------------------
//
// Built-in filter defines
//
//-----------------------------------------------------------------------------
#define MilesFilterNameReverb "Reverb"
#define MilesFilterNameChorus "Chorus"
#define MilesFilterNameFlanger "Flanger"
#define MilesFilterNameLowPass "Lowpass"
#define MilesFilterNameHighPass "Highpass"
#define MilesFilterNameBandPass "Bandpass"
#define MilesFilterNameGainBlock "GainBlock"
#define MilesFilterNameParamEQ "ParametricEQ"
#define MilesFilterNameVolumeRamp "VolRamp"
#define MilesFilterNameGate "Gate"
#define MilesFilterNameCompress "Compress"
#define MilesFilterNameDelay "Delay"
#define MilesFilterNameConvolution "Convolution"
#define MilesFilterNameGain "Gain"

//-----------------------------------------------------------------------------
//
// Platform Abstractions
//
//-----------------------------------------------------------------------------
//idoc(parent, OutputAPI)
IDOC RADEXPFUNC HMILESHARDWAREOUTPUT MILES_NO_THUNK RADEXPLINK MilesOutputGetDefault(U32* OutputSampleRate, MilesChannelSpec* OutputChannelSpec);
/*
    Returns the default output system for the current platform.

    $:OutputSampleRate The sample rate the output device expects.
    $:OutputChannelSpec The channel rate the output device expects.

    Many platforms have strong restrictions on what formats their audio drivers
    expect. If it can be determined, OutputSampleRate and OutputChannelSpec will
    hold the restricted values. If the platform has no restrictions, Miles
    makes its best guess. For example, under directsound on windows, Miles can get the
    channel count of the current device, but not the sample rate. A suitable
    default is returned in these cases.

    If your platform (or TCRs) have alternate options, feel free to ignore the OutputSampleRate
    or OutputChannelSpec results. However, $MilesDriverCreate or $MilesOutputCreate may
    fail if the underlying device doesn't support it.
*/

IDOC RADEXPFUNC HMILESHARDWAREOUTPUT MILES_NO_THUNK RADEXPLINK MilesOutputGetNull();
/*
    Returns the null output system.

    This output system consumes samples at a fixed rate determined by the
    sample rate specified at driver creation. This system allows for Miles to
    be loaded and used without any audio hardware.
*/


IDOC RADEXPFUNC HMILESHARDWAREOUTPUT MILES_NO_THUNK RADEXPLINK MilesOutputGetMemory();
/*
    Returns the memory output system.

    This output system renders to a supplied buffer as fast as possible. Note that
    if any IO is required, samples will almost certainly starve, so make sure
    data is available prior to the first sample playing. Once playing, the output
    will render until the supplied buffer has been filled. When filled, the 
    MilesMemoryWriterParameters::Done flag is set.

    ${
        MilesMemoryWriterParameters Params = {0};
        Params.OutputBuffer = MyBuffer;
        Params.OutputBufferSize = MyBufferByteCount;

        // Open a driver to render to memory.
        MilesDriver* Driver = MilesDriverCreateEx(
            48000, MilesChannelSpecStereo, 
            MilesOutputGetMemory(), (UINTa)&Params);

        // (Create and play a sample here)

        // Wait for the render to complete. If no sample is started, then 
        // this is a deadlock.
        for (;;)
        {
            MilesSleep(10);
            
            if (Params.Done)
                break;
        }

        // MyBuffer now has the rendered samples.
    $}
*/


#ifdef __RADWIN__

RADEXPFUNC MilesChannelSpec MILES_NO_THUNK RADEXPLINK MilesSystemSpeakerConfig(UINTa device_lpguid_or_zero);

#endif // __RADWIN__


//
// String Functions
//

// MS compilers use _strnicmp
#ifdef __RADWIN__
#   define MilesStrnicmp _strnicmp
#   define MilesStricmp _stricmp
#else
#   define MilesStrnicmp strncasecmp
#   define MilesStricmp strcasecmp
#endif

//idoc(parent, FileAPI)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
RADEXPFUNC S32 MILES_NO_THUNK RADEXPLINK MilesGetWavMetadata(void const* in_data, U32 in_len, U16* out_channels, U32* out_rate, U32* out_mask, U16* out_blocksize, U32* out_samples, U16* out_bits, U32* out_offset_to_samples, U32* out_sample_data_len);

#define MilesWavHeaderSize (11 * sizeof(U32))
IDOC RADEXPFUNC void MILES_NO_THUNK RADEXPLINK MilesFillWavHeader(char* OutWavHeaderBuffer, U32 SampleRate, U32 ChannelCount, U32 BufferSizeInBytes);
/*
    Generates a .wav header for the given audio format.

    OutWavHeaderBuffer must be of size MilesWavHeaderSize. Once filled,
    a wav file can be generated by writing the header followed by the 
    wav sample buffer to a file.

    *Thread Safety* $(Threading:TS_Pure)
*/

IDOC RADEXPFUNC S32 MILES_NO_THUNK RADEXPLINK MilesBakeImpulseResponse(S16* Samples, U32 SampleCount, U32 IsStereo, U32 StepdownAtBlock, U32 StepdownIs4, void** OutBakedImpulseResponse, U32* OutBakedImpulseResponseSize);
/*
    Bakes an impulse response in to a format for use by the convolution reverb.

    $:Samples The 16 bit source samples for the impulse response.
    $:SampleCount The number of samples pointed to by Samples.
    $:IsStereo If nonzero, Samples points to SampleCount number of sample pairs (left and right).
    $:StepdownAtBlock If nonzero, then the downsample optimization is enabled, and will
        start at the given offset. See discussion.
    $:StepdownIs4 If StepdownOffset is nonzero, then this sets whether the downsample is to 24 khz (2x) or 12 khz (4x)
    $:OutBakedImpulseResponse The address of a pointer that will recieve the baked output. The must be freed by MilesFree.
    $:OutBakedImpulseResponseBytes The number of bytes pointed to by OutBakedImpulseResponse.

    Only mono or stereo responses are supported. If applied to multichannel audio, center and lfe channels will
    use the average response.

    The samples provided are assumed to be at 48khz. If the original response was recorded or generated at
    another sample rate, it should be resampled before being provided to this function. (Failure to do this
    is not a critical failure, it just alters the effect of the response.)

    The output of this function can be saved across application runs.

    For optimization purposes, Miles provides a way to run the IR tails at a
    lower sample rate. Generally speaking, reverb tails are low frequency due
    to the nature of reflection attenuation. Since there aren't any high frequencies,
    we can take advantage of the sampling theorem and lower the sampling rate to
    2x the highest frequency in the signal and get a mathematically exact
    representation of the original signal. Miles currently supports a 2x and 4x
    downsample selection, so to take advantage of this optimization, analyze your
    IR and find where the frequency response drops below 12khz and 6khz. From that,
    use the block size below to determine where to start the downsample.

    StepdownIs4 != 0: Max frequency 6khz, block size 4096.
    StepdownIs4 == 0: Max frequency 12khz, block size 2048.

    So, if the IR falloff off such that the last sample above 6khz occurs at
    sample 18350, it would make sense to select StepdownIs4 != 0, and determine
    that the given sample is in block 4 (18350 / 4096 = 4.479). So start the
    stepdown at the next block:

    StepdownAtBlock = 5
    StepdownIs4 = 1

    This decreases both the memory usage and the number of convolutions, with
    the only potential artifacting being resampling aliasing on the incoming
    signal.
    
*/


//-----------------------------------------------------------------------------
//
// High Level API
//idoc(parent, HighLevelAPI)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define MilesVoiceStateActive                   0x1                                     IDOC
#define MilesVoiceStatePlaying                  0x2                                     IDOC

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define MilesProjectStatusLoading               0                                       IDOC
#define MilesProjectStatusLoaded                1                                       IDOC
#define MilesProjectStatusLoadError             0x80000001                              IDOC
#define MilesProjectStatusAuditioningFailed     ~0U                                     IDOC

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define MilesBankStatusLoading                  0                                       IDOC
#define MilesBankStatusLoaded                   1                                       IDOC
#define MilesBankStatusProjectError             0x80000001                              IDOC
#define MilesBankStatusLoadError                0x80000002                              IDOC

//idoc(parent, EventAPI)
IDOC RADEXPFUNC void RADEXPLINK MilesEventGetSoundMemoryUse(HMILESDRIVER Driver, U32* OutActiveBytes, U32* OutStaleBytes);
/*
    Returns the amount of sound data the event system has dynamically loaded.

    Zero can be passed for any output not desired.
    
    OutStaleBytes will contain the bytecount of data that is no longer on active samples,
    but is currently being retained. OutActiveBytes will contain the bytecount of data loaded
    and on active voices.

    *Thread Safety* $(Threading:TS_Lock).
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesEventGetStarvedVoiceCount(HMILESDRIVER Driver);
/*
    Returns the number of voices that outpaced the async read of their data.

    For voices that are streamed in as they play, high I/O contention situations
    could cause a starvation of the available data. In this case they will stop until
    more data arrives.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC void MILES_NO_THUNK RADEXPLINK MilesEventGetCounts(HMILESDRIVER Driver, U32* OutActiveVoiceCount, U32* OutMutedVoiceCount, U32* OutEventCount, U32* OutSleepingEventCount);
/*
    Returns the current voice and event counts for performance displays.

    $:OutActiveVoiceCount The number of voices that are allocated and currently have 
        a sample assigned to them for mixing. Includes paused voices.
    $:OutMutedVoiceCount The number of voices that are allocated but have no
        sample assigned for mixing and thus can not be heard. Includes paused voices.
    $:OutEventCount The number of events with allocated voices.
    
    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

//RADEXPFUNC void         RADEXPLINK MilesEventSetExternalSoundHandler(HMILESDRIVER driver, MilesExternalSoundFnType* fn);

IDOC RADEXPFUNC void RADEXPLINK MilesEventSetReadLimits(HMILESDRIVER Driver, U32 BaseChunkBytes, U32 LargeChunkBytes);
/*
    Sets the sizes of the reads the event system uses to stream in audio data.

    $:BaseChunkBytes The number of bytes that is read when multiple voices need data.
        Defaults to 32k. This is smaller to allow the system to more easily bounce
        between voices, preventing starvation.
    $:LargeChunkBytes The number of bytes that is read when no other voices need data.
        Defaults to 128k. This is larger so that in periods of no contention, the
        data can be slurped in as fast as possible.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC UINTa RADEXPLINK MilesEventSetStreamingCacheLimit(HMILESDRIVER Driver, UINTa StreamingMemoryLimit);
/*
    Sets the maximum amount of memory the event system will use for streamed in data.

    $:StreamingMemoryLimit The number of bytes to limit to, or zero to remove the limit.
    $:return The previous limit.

    *Thread Safety* $(Threading:TS_Lock)    
*/

//-----------------------------------------------------------------------------
//
// Project Management
//
//-----------------------------------------------------------------------------
IDOC RADEXPFUNC S32 RADEXPLINK MilesProjectLoad(HMILESDRIVER Driver, char const* ProjectFileName, char const* ProjectLanguageName, char const* ProjectAuditionName);
/*
    Starts a loads for a project in to the miles event runtime.

    $:Driver The driver to load the project in to.
    $:ProjectFileName The file name of the project to load.
    $:ProjectLanguageName The name of the language to localize for. Must match a language defined 
        in Miles Studio (Project->Settings).
    $:ProjectAuditionName The name of the project to load, if auditioning. If this is zero,
        Miles will use ProjectFileName, stripped of path and extension. For example,
        if ProjectFileName is "/path/to/myproject.mprj", ProjectAuditionName will
        default to "myproject". When auditioning, this must match the project name
        loaded in Miles Studio or an error will result.

    Kicks off an asynchronous load for the project.

    For non auditioned projects, the project file data is loaded on the file io thread,
    and deserialized on the mixer thread. To check whether the project load has completed,
    check $MilesProjectGetStatus. To wait until the project loads or an error occurs,
    use $MilesProjectWaitForLoad.

    Banks can be queued for load without waiting for the project to complete
    loading using $MilesBankLoad:

    \<pre\>

    // Load a project that has no localization
    $MilesProjectLoad(driver, path_and_project_filename, 0, 0);

    // The project is still loading, but we can just queue the bank to load.
    HMILESBANK bank = $MilesBankLoad(driver, path_and_bank_filename, 
        path_and_stream_filename, 0, 0);   

    // Wait for the bank to load, which will happen once the project finishes
    // loading.
    $MilesBankWaitForLoad(bank);

    // the bank and project have completed loading - check for errors.

    \</pre\>

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesProjectGetStatus(HMILESDRIVER Driver);
/*
    Returns the current project status. Undefined if $MilesProjectLoad has not
    been called.

    *Project States*
    $: $MilesProjectStatusLoading - the project is still loading file data off of
        disc or deserializing.
    $: $MilesProjectStatusLoaded - the project has successfully loaded.
    $: $MilesProjectStatusAuditioningFailed - the project was not able to load
        off of the network. 
    $: $MilesProjectStatusLoadError - the file isn't a project or is from a
        different version of Miles.

    If a file error occurs, the project status will be set to the io error result.
    See $MilesIoStatus for possible error codes.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesProjectWaitForLoad(HMILESDRIVER Driver);
/*
    Blocks until the project completes loading, and returns the result from
    $MilesProjectGetStatus.

    This will block forever if no project has been loaded on to the driver.

    *Thread Safety* $(Threading:TS_Wait)
*/

IDOC RADEXPFUNC char const* MILES_NO_THUNK RADEXPLINK MilesProjectStatusToString(U32 ProjectStatus);
/*
    Returns a string representation for the result of $MilesProjectGetStatus.

    *Thread Safety* $(Threading:TS_Pure)
*/                        

IDOC RADEXPFUNC S32 RADEXPLINK MilesProjectLoadData(HMILESDRIVER Driver, void* Buffer, char const* ProjectLanguageName);
/*
    Loads a project from the given memory buffer instead of from disc.

    The data is referenced directly and not copied - the buffers must be
    valid until the driver is closed.

    This function cannot be paired with $MilesBankLoad, but must be used with
    $MilesBankLoadData - all functions must be data oriented or not.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC HMILESBUS RADEXPLINK MilesProjectGetBus(HMILESDRIVER Driver, char const* BusName);
/*
    Returns the HMILESBUS associated with the named bus in the event runtime.

    BusName must be the name of a bus defined in Miles Studio. If the bus name is
    not found, zero will be returned.

    Use caution, as during auditioning the buses can (and likely will) be 
    reallocated due changes in Miles Studio. There is currently no reliable
    way to use this during auditioning.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesProjectGetBankCount(HMILESDRIVER Driver);
/*
    Returns the number of banks the project loaded on to the driver was rendered with.

    A utility function to determine how many banks the sound designer used. For
    use with $MilesProjectGetBankName.

    In general, it's expected that banks are broken out for a specific purpose,
    and as such the programmer should know the different banks (core, incidental,
    DLC, etc). However, due to the on-demand loading, it's rarely necessary to
    split out banks as there isn't a lot of memory wasted.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC char const* RADEXPLINK MilesProjectGetBankName(HMILESDRIVER Driver, U32 BankIndex);
/*
    Returns the name of a bank the project was rendered with.

    $:BankIndex The index of the bank to inspect, bounded by $MilesProjectGetBankCount. Note this is NOT bank ID.
    $:return A static string of the name of the bank. Do not delete!

    See $MilesProjectGetBankCount for comments.

    This name can be used to prepend the event name from $MilesBankGetEventName
    to get a complete event name:
    
    \<pre\>

    // Generates the event name for the first event in the first bank.
    // Assumes both exist.
    sprintf(complete_event_name, "%s/%s", $MilesProjectGetBankName(driver, 0), $MilesBankGetEventName(bank, 0));
    \</pre\>

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesProjectSetQueueSaveFn(HMILESDRIVER Driver, MilesQueueSaveFnType* SaveFn);
/*
    When set, this function will be called in order to allow for saving of any queues passed
    to the event system, for debugging purposes.

    This function will be called from the mixer thread!

    The queues saved by this system can be replayed using $MilesProjectSetSavedQueueBuffer,
    however this shouldn't be used in a shipping release - it's purpose is to provide
    reproduction steps to RAD for bugs or feature requests, or for regression testing.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesProjectSetSavedQueueBuffer(HMILESDRIVER Driver, void* QueueBuffer, U32 QueueBufferSize);
/*
    This function takes in a buffer saved by $MilesProjectSetQueueSaveFn 
    and replays it. It is designed for testing and bug reproduction.

    The project and bank loaded must be the same as loaded during the capture, 
    and the driver must be newly created.

    *Thread Safety* $(Threading:TS_Lock)
*/

#if defined(__RAD64__) && defined(__RADNT__)
IDOC RADEXPFUNC void RADEXPLINK MilesDecodeQueueBuffer(char const* OutputFileName, void* QueueBuffer, U32 QueueBufferSize);
/*

*/
#endif

IDOC RADEXPFUNC U32 RADEXPLINK MilesProjectLoadSavedQueueBufferSources(HMILESDRIVER Driver, void* QueueBuffer, U32 QueueBufferSize);
/*
    Similar to $MilesProjectSetSavedQueueBuffer, except instead of playing the
    buffer, it attempts to load all possible sources referenced by the queue,
    such that playing the buffer will require no I/O.

    This function does not return until the loads are complete.

    *Thread Safety* $(Threading:TS_Wait)
*/

//-----------------------------------------------------------------------------
//
// Bank Management
//
//-----------------------------------------------------------------------------
IDOC RADEXPFUNC HMILESBANK RADEXPLINK MilesBankLoad(HMILESDRIVER Driver, char const* BankFileName, char const* BankStreamFileName, char const* BankLocalizedStreamFileName, char const* BankAuditionName);
/*
    Starts to load a bank.

    $:Driver The driver the project for the bank is loaded.
    $:BankFileName The filename of the bank metadata to load.
    $:BankStreamFileName The filename of the bank streaming data to load.
    $:BankLocalizedStreamFileName If the project is localized, this is the language stream
        file to load. If $MilesProjectLoad was called with a nonzero ProjectLanguageName, 
        this parameter must be nonzero. Likewise, if ProjectLanguageName was zero,
        this must be zero. Supplying a localized stream for an incorrect language
        results in an error. Localization is ignored during auditioning.
    $:BankAuditionName If auditioning, this is the bank that will be loaded in Miles Studio.
        If not auditioning, ignored. If zero and auditioning, Miles will use BankFileName with
        path and extension removed. For example, if BankFileName were "/path/to/mybank.mbnk", 
        Miles will look for the "mybank" bank in Miles Studio when auditioning.

    Kicks off an asynchronous load for a bank.

    For non auditioned projects, the bank file data is loaded on the file io thread,
    and deserialized on the mixer thread. To check whether the bank load has completed,
    check $MilesBankGetStatus. To wait until the bank loads or an error occurs,
    use $MilesBankWaitForLoad.

    Banks can be queued for load without waiting for the project to complete:

    \<pre\>

    $MilesProjectLoad(driver, path_and_project_filename, 0, 0);

    // The project is still loading, but we can just queue the bank to load.
    HMILESBANK bank = $MilesBankLoad(driver, path_and_bank_filename, 
        path_and_stream_filename, 0);   

    // Here we have to know about our patch situation. If we already know
    // how many patches we have (e.g. 0), we can just queue the loads and wait:
    if (patch_count_known)
    {
        for (int i = 0; i < patch_count; i++)
            $MilesBankPatch(bank, patch_file_name[i], 0);

        // wait for everything to load (including the project)
        $MilesBankWaitForLoad(bank);
    }
    else
    {
        // If we don't know, we have to get the number from Miles. This
        // is stored in the bank metadata file (passed to MilesBankLoad),
        // so we have to wait until that has been loaded:
        U32 patch_count = ~0U;
        for (;;)
        {
            // give the IO some time to run. Alternatively, do
            // some other loading.
            MilesSleep(5);

            if ($MilesBankGetStatus(Bank, &patch_count) & 0x80000000)
            {
                // if the highest bit is set (i.e. is negative) then
                // we errord and will never get the patch count.
                return error;
            }

            // otherwise, MilesBankGetStatus will only touch patch_count
            // once it has the answer, so check if it's changed from
            // our known value.
            if (patch_count != ~0U)
                break; // got the patch count.
        }

        // patch - the filename must be discoverable/generated from the
        // patch index
        for (U32 i = 0; i < patch_count; i++)
            $MilesBankPatch(bank, create_patch_filename(i), 0);

        // Now we can wait for everything to finish.
        $MilesBankWaitForLoad(bank);
    }

    // the bank and project have completed loading - check for errors.

    \</pre\>

    *Bank Files*

    When rendering a project, each bank produces several files:
    $* Bank Metadata File (.mbnk) - This file contains the events and global sound 
        metadata.
    $* Bank Global Stream File (.mstr) - This file contains all audio data for
        sounds that will be played no matter the language.
    $* Bank Localized Stream Files (.mstr) - These files contain the audio data
        for sounds that will only be played for a specific language. One
        will be produced for each language defined in Miles Studio. If 
        the project is not localize, no file will be produced.

    *Patch Files*

    When a patch is created, the bank metadata file (.mbnk) is recreated in
    entirety - no older versions need to be retained on disc when deploying.

    However, new stream files are created containing audio data that
    has been added or changed. They are relative to any previously deployed
    stream files (.mstr), and as such the old files must be retained. This
    includes any previous patch stream files!

    *Memory Usage*

    The bank metadata file is loaded in its entirety. However, only the 
    "preload" sections of the stream files are loaded. These sections hold
    the data used by Miles to play audio while the remnants stream in. By
    default, this is 100ms of audio per source. This amount can be overridden
    in Miles Studio on a per platform or per source basis.

    To determine how much data was loaded in, call $MilesBankGetBaseMemoryUsage.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC HMILESBANK RADEXPLINK MilesBankLoadData(HMILESDRIVER Driver, void* BankBuffer, void* BankStream, void* BankLocalized);
/*
    Loads a bank from a provided buffer.

    $MilesBankLoadData cannot be used with $MilesProjectLoad - only $MilesProjectLoadData.
    The data and file functions cannot be mixed.

    The data pointer must contain the entire rendered bank file. As a result, 
    all of the advantages of dynamically streaming in sound data are lost,
    since all sound data is in memory at all times.

    The data pointer is NOT copied, and must be available until the bank is closed.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesBankGetStatus(HMILESBANK Bank, U32* OutPatchCount);
/*
    Returns the current status of a bank.

    *Bank States*
    $* $MilesBankStatusLoading - the bank is current still loading, or is waiting for the project to load.
    $* $MilesBankStatusLoaded - the bank has completed loading and is ready to play events.
    $* $MilesBankStatusProjectError - the bank failed to load due to the project failing to load. Check $MilesProjectGetStatus.
    $* $MilesBankStatusLoadError - the file provided was either not a bank, or a bank for a different version of Miles.
    $* IO Error - see $MilesIoStatus for an additional list of possible errors.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesBankWaitForLoad(HMILESBANK Bank);
/*
    Stalls the current thread until the bank completes loading.

    $:return Returns the bank's status as per $MilesBankGetStatus.
    
    This function returns once the bank loading completes, whether or not
    the load succeeds. To determine whether the load was successful check
    the return value or $MilesBankGetStatus.

    See $MilesBankLoad for usage example.

    *Thread Safety* $(Threading:TS_Wait)
*/

IDOC RADEXPFUNC S32 RADEXPLINK MilesBankPatch(HMILESBANK Bank, char const* BankPatchStreamFileName, char const* BankPatchLocalizedStreamFileName);
/*
    Adds a patch to the bank.

    $:BankPatchStreamFileName The filename of the global audio stream for the patch, if provided.
    $:BankPatchLocalizedStreamFileName The filename of the localized audio stream for the patch, if provided.

    It is possible that one or both (or none) are provided for a given patch, if for
    example the only data that changed was for German audio, and the game happens to
    be loading English. \<b\>In this case, the patch function still needs to be called
    with both parameters zero.\</b\>

    Once all patches have been provided, call $MilesBankCommitPatches.

    See also $patching

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC char const* MILES_NO_THUNK RADEXPLINK MilesBankStatusToString(U32 BankStatus);
/*
    Converts a bank status to a human readable string.

    $:BankStatus The return from either $MilesBankGetStatus or $MilesBankWaitForLoad.

    Used primarily in example and development code - the returned strings
    are not useful to end users.

    *Thread Safety* $(Threading:TS_Pure)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesBankGetBaseMemoryUsage(HMILESBANK Bank);
/*
    Returns the amount of memory used by a bank, excluding dynamically loaded sound.

    Banks don't read the complete file when loaded. Most actual sound data is 
    streamed in on demand. This returns the amount of memory actually used by
    a bank and all of its patches, *excluding* this streamed data.

    This function should not be used with banks loaded using $MilesBankLoadData
    as we do not allocate any internal memory for banks on that path.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesBankFree(HMILESBANK Bank);
/*
    Frees all memory associated with a bank.

    Any references in the runtime to data managed by the will be freed. 
    This includes any active playing voices!

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesBankGetEventCount(HMILESBANK Bank);
/*
    Returns the number of events in the given loaded bank.

    The number of events may change due to auditioning - this only returns
    the events in the bank at the time of load. Event updates from auditioning
    are pushed via the event info queue ($MilesEventInfoQueueEnable)

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC char const* RADEXPLINK MilesBankGetEventName(HMILESBANK Bank, U32 EventIndex);
/*
    Returns the name of the event at the given index.

    $:EventIndex An index in the range [0, $MilesBankGetEventCount).
    $:return A pointer to a static string representing the name of the event. Do not delete!

    The returned name of the event will NOT have the name of the bank prepended.

    The names will be sorted.

    As per $MilesBankGetEventCount, this only returns the names of events
    that existed in the bank at the time of load.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC typedef struct MilesEventTemplateId
{    
    U32 BankId;
    // The ID of the bank - this is a FOURCC code that is set in Miles Studio when selecting the bank.
    
    U32 Index;
    // The index of the asset in the bank (internally - highest bit is audition override assets, next highest is if its a source)
} MilesEventTemplateId;
/*
    An event template ID is a numerical ID used to identify an event (or source with
    a default event) in the project. It can be stored across executions provided the 
    project is not rendered again (including patching).
*/

IDOC RADEXPFUNC void MILES_NO_THUNK RADEXPLINK MilesBankGetEventTemplateId(HMILESBANK Bank, U32 EventIndex, MilesEventTemplateId* OutTemplateId);
/*
    Returns the Template Id for the given event in a bank.

    The Template Id can be stored across executions, and is unique to a given render of the bank.

    If the bank is patched, the template ids will be invalidated.

    *Thread Safety* $(Threading:TS_StableProject)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesEventResolveName(HMILESDRIVER Driver, char const* EventName, MilesEventTemplateId* OutTemplateId);
/*
    Resolves the name is the same fashion as $MilesQueueEventRun, returning the 
    resulting TemplateId for the event, if it exists.

    $:EventName The event name, in standard format (See $MilesQueueEventRun)
    $:OutTemplateId A pointer to the template id to fill out, or zero if not desired.
    $:return Zero if the event doesn't exist.

    If the bank is patched, the template ids will be invalidated.

    See also $MilesBankGetEventTemplateId.

    *Thread Safety* $(Threading:TS_StableProject)
*/

IDOC RADEXPFUNC char const* MILES_NO_THUNK RADEXPLINK MilesEventGetName(HMILESDRIVER Driver, MilesEventTemplateId const* TemplateId, char const** OutBankName);
/*
    Returns the event name for a given template id.

    DO NOT DELETE the returned name. If the template id is invalid, zero will be returned.

    *Thread Safety* $(Threading:TS_StableProject)    
*/

typedef struct MilesEventMetaContentEntry
{
    char const* Key;
    char const* Value;
    U32 ActionIndex;
} MilesEventMetaContentEntry;

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesEventGetMetaContent(HMILESDRIVER Driver, MilesEventTemplateId const* TemplateId, MilesEventMetaContentEntry* Entries, U32 EntryCount);
/*
    Returns the number of metadata content entries for the given event, or ~0U if not found.

    $:Driver The driver the event system is active on.
    $:TemplateId The template ID for the event. See also $MilesEventResolveName.
    $:Entries An array of entries passed to fill out with the user content.
    $:EntryCount The size of the Entries array.
    
    MetaContent is data specified by the designer in Miles Studio for access by the game.
    Every event action can have a separate value for a globally defined key (also
    specified in Miles Studio).

    If Entries is zero, the number of available entries is returned. If Entries
    is nonzero, up to EntryCount user content entries is filled out, with the
    number of entries used up returned. If the Entries array is not big enough,
    it will be filled up to EntryCount, with the total number needed returned.

    
    *Thread Safety* $(Threading:TS_StableProject)
*/

IDOC typedef enum MilesEventDetailsFlag
{
    MilesEventDetailsFlagNone = 0,

    MilesEventDetailsFlagControl = 0x1,
    /*
        If set, the event contains an action other than play, and shouldn't
        be high level culled.
    */

    MilesEventDetailsFlagZeroPriority = 0x2,
    /*
        If set, the event contains a play action with a zero priority.
        Zero priority is used by the sound designers to prevent a sound
        from ever getting evicted. If $MilesEventGetDetails is being
        used for high level event culling, existence of this flag
        should prevent such culling on the event.
    */

    MilesEventDetailsFlagMetadata = 0x4,
    /*
        If set, the event has metadata that can be retrieved via MilesEventGetMetaContent.
    */

    MilesEventDetailsFlagPosition = 0x8,
    /*
        If set, the event is 3D or otherwise has a controller that requires distance from
        a listener.
    */

    MilesEventDetailsFlagOrientationContinual = 0x10,
    /*
        If set, the event uses an angle that requires the event's orientation to
        be valid, and the controller will continue to poll the orientation over the
        life of the event.
    */

    MilesEventDetailsFlagOrientationOnce = 0x20,
    /*
        If set, the event uses an angle that requires the event's orientation to
        be valid, but the controller only grabs the value once.
    */

    MilesEventDetailsFlagSleepable = 0x40,
    /*
        If set, the event has been marked in Miles Studio as allowing sleep via
        MilesQueueEventSleeping and MilesQueueEventUpdateSleeping.
    */

    MilesEventDetailsFlagControllerCountMask = 0xff000000,
    /*
        The number of GAME controllers the event references is stored in the high
        byte of the details flag. These are controllers that are expected to 
        be set by MilesQueueControllerValue (for global controllers) or 
        MilesQueueEventControllerValue (for event local controllers). The event
        may reference other controllers that are implictly set - for example
        an angle controller that requires the event's position or orientation
        to be set.

        Use MILES_EVENT_DETAILS_CONTROLLER_COUNT() to retrieve, and
        MilesEventGetControllerNames to get the actual controllers referenced.

        Note that this does NOT include any controllers that are on buses on
        the event's output routing.
    */
    MilesEventDetailsFlagFORCE32 = 0x80000000
} MilesEventDetailsFlag;

#define MILES_EVENT_DETAILS_CONTROLLER_COUNT(DetailsFlags) ( ((U32)DetailsFlags) >> 24 )

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesEventGetDetails(HMILESDRIVER Driver, MilesEventTemplateId const* TemplateId, char const** OutEventName, MilesEventDetailsFlag* OutEventFlags, F32* OutRadius, F32* OutLongestDurationOfFirstSourceSeconds);
/*
    Returns basic information about an event for high level filtering.

    Any output parameter can be zero if it is not desired.

    To convert an event name to a template id, use MilesEventResolveName.
    
    Returns:
    $* OutEventName - event name, excluding bank. Do not delete.
    $* OutEventFlags - flags describing properties about the event, as well as the number of
        game controllers the event requires.
    $* OutRadius - if the event is 3D, then this is the estimated distance the distance component 
        will fall below the level specified by RadiusCutoffDb. If the falloff is not monotonically 
        decreasing as a function of distance, the estimate could easily be off. If the Input Gain
        is bound to a Distance controller, then that will be used as the falloff, otherwise the
        3D falloff will be used. If the event is not 3D or does not ever fall below the falloff,
        OutRadius will be -1.
    $* OutLongestDurationOfFirstSourceSeconds - The estimated duration of the event, taken by evaluating
        the first source available for each start action, scaling by the average pitch, multiplied
        by the play count. If the play count is infinite, or there are no start actions, the duration
        is -1.

    *Thread Safety* $(Threading:TS_StableProject)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesEventActionGetMetaContent(HMILESDRIVER Driver, MilesEventTemplateId const* TemplateId, U32 ActionIndex, MilesEventMetaContentEntry* Entries, U32 EntryCount);
/*
    As per $MilesEventGetMetaContent, but for a specific action index instead 
    of all metadata in the event.

    *Thread Safety* $(Threading:TS_StableProject)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesEventControllerNames(HMILESDRIVER Driver, MilesEventTemplateId const* TemplateId, char const** OutControllerNames, U32 Count);
/*
    Provides access to the names of Game controllers that the given event references.

    Returns the number of game controller names on the event, or zero if the event doesn't exist.
    Pass OutControllerNames == 0 to retrieve the number of controllers, or use $MilesEventGetDetails.

    OutControllerNames should be an array of char const* of size Count.

    DO NOT delete the pointers provided.

    *Thread Safety* $(Threading:TS_StableProject)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesEventReferencesController(HMILESDRIVER Driver, MilesEventTemplateId const* TemplateId, char const* ControllerName);
/*
    Returns 1 if the controller is used by the given event, or 0 on error or not used.

    Currently only check Play actions. References to controllers in other
    types of actions will currently be missed.

    *Thread Safety* $(Threading:TS_StableProject)
*/

IDOC RADEXPFUNC U32 RADEXPLINK MilesBankGetSourceCount(HMILESBANK Bank);
/*
    Returns the number of sources in the given loaded bank.

    As per $MilesBankGetEventCount.    

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesBankGetSourceTemplateId(HMILESBANK Bank, U32 SourceIndex, MilesEventTemplateId* OutTemplateId);
/*
    Provides the template id for sources that have default events.

    As per $MilesBankGetEventTemplateId

    Only sources with Default Events have a template id. Other sources
    will return zero.

    *Thread Safety* $(Threading:TS_StableProject)
*/

IDOC RADEXPFUNC char const* RADEXPLINK MilesBankGetSourceName(HMILESBANK Bank, U32 SourceIndex);
/*
    Returns the name of the source at the given index.

    As per $MilesBankGetEventName.

    *Thread Safety* $(Threading:TS_Lock).
*/

//-----------------------------------------------------------------------------
//
// Queue Management
//
//-----------------------------------------------------------------------------
//idoc(parent, QueueAPI)
IDOC RADEXPFUNC HMILESQUEUE RADEXPLINK MilesQueueCreate(HMILESDRIVER Driver);
/*
    Creates a queue to submit high level commands to the miles runtime.

    $:Driver The HMILESDRIVER the queue will submit to.
    $:return a handle to a queue to use with all MilesQueue* functions.

    In order to facilitate a lock-free frame, Miles uses a system
    similar in design to graphics command buffer systems. Briefly, a set 
    of commands are added to a queue over the course of a frame, and
    then is submitted when ready. The queue gets processed by the
    mixer thread prior to the next mix. Virtually all commands 
    related to the high level event system will pass through one
    of these queues. In general, limit the number of submissions
    per frame and instead add many events in to one queue.

    \<pre\>

    // Create a queue to submit commands.
    HMILESQUEUE queue = $MilesQueueCreate(driver);

    // Add a command to run an event.
    $MilesQueueEventRun(queue, "mybank/myevent");

    // Submit the queue to the mixer thread.
    $MilesQueueSubmit(queue);

    // The queue is ready for use again immediately, even
    // though the previous commands are not necessarily
    // executed yet.
    \</pre\>

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC void RADEXPLINK MilesQueueDestroy(HMILESQUEUE Queue);
/*
    Frees memory associated with a Miles queue.

    Any commands that have not yet been submitted will not be executed.
    Submitted commands that are pending will still be executed. For example
    the following code is safe, _though not recommended every frame_.

    \<pre\>

    // Create a queue
    HMILESQUEUE queue = $MilesQueueCreate();

    // Add a command to run an event, then submit it.
    $MilesQueueEventRun(queue, "mybank/myevent");
    $MilesQueueSubmit(queue);

    // Destroy the queue. If the event hasn't already been run, it still will be.
    $MilesQueueDestroy(queue);
    \</pre\>

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueSubmit(HMILESQUEUE Queue);
/*
    Submits a queue's commands for processing on the mixer thread.

    $:return The ID for the submission. This ID is specific to the queue.
        See also $MilesQueueGetIsRetired.

    This function is lock free most of the time. If many queues are waiting
    on processing by the mixer thread (16 as of this writing), then the
    calling thread will wait for room.

    Once the commands have been submitted, there is a period of time
    where the commands are pending on the mixer thread and have not
    been processed yet. Be careful not to assume any voices are created
    immediately after MilesQueueSubmit returns.

    Queues are technically processed in the order they are received, 
    however care must be taken if submitting from several threads, as
    the ordering is arbitrary due to the nature of multi threaded code.

    \<pre\>

    // Create a queue
    HMILESQUEUE queue = $MilesQueueCreate();

    // Add a command to run an event, then submit it.
    $MilesQueueEventRun(queue, "mybank/myevent_one");
    $MilesQueueSubmit(queue);

    // Add another command to run an event.
    $MilesQueueEventRun(queue, "mybank/myevent_two");
    $MilesQueueSubmit(queue);

    // myevent_one will be run before myevent_two.
    \</pre\>

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/                        

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueGetIsRetired(HMILESQUEUE Queue, U32 QueueSubmitId);
/*
    Returns whether a given queue submission has been processed by the mixer thread.

    $:QueueSubmitId The ID of the submission to check, or ~0U to check all submissions.

    A queue's submission is considered "retired" once all of its commands
    have been processed and run. 

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

// Event Management
IDOC RADEXPFUNC U64 MILES_NO_THUNK RADEXPLINK MilesQueueEventRun(HMILESQUEUE Queue, char const* EventName);
/*
    Adds a command to a queue to run an event.

    $:EventName The name of the event (See $EventNames) or an intrinsic.
    $:return The global ID of the event in the system, or zero on failure.

    The given event name will be looked up in any currently loaded banks. If
    the event exists, the actions contained within will be executed in order.

    The event will inherit the currently set event state including
    filter, 3d position and orientation, and others as set by the
    MilesQueueEvent* functions.

    The event state is cleared after every event run command.

    Event names are generally of the form "bank/event", but can be more
    complicated. See $EventNames for details.

    Several intrinsic event names can also be used:
    $* "STOP" - Stops all voices that match the current filter (MilesQueueEventFilter*).
    $* "STOPNOW" - As per STOP, except fades are disallowed.
    $* "PAUSE" - Pauses all sounds that match the filter.
    $* "RESUME" - Resumes all sounds paused by a previous PAUSE event id.

    These incrinsics are demonstrated in the hl_basic_filtering.cpp
    example.

    \<pre\>

    // Create a queue
    HMILESQUEUE queue = $MilesQueueCreate();

    // Add a command to run an event, then submit it.
    $MilesQueueEventRun(queue, "mybank/myevent");
    $MilesQueueSubmit(queue);

    // On the next mix, "mybank/myevent" will be looked up and 
    // run.
    \</pre\>

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U64 MILES_NO_THUNK RADEXPLINK MilesQueueEventRunByTemplateId(HMILESQUEUE Queue, MilesEventTemplateId const* TemplateId);
/*
    As per $MilesQueueEventRun, except the event is looked up by template id instead of by name.

    The template ID for an event can be found by using $MilesEventResolveName, $MilesBankGetEventTemplateId, or
    $MilesBankGetSourceTemplateId.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/


IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueEventInfoMask(HMILESQUEUE Queue, U32 ReportMask);
/*
    Marks that the next event should report information to the report queue to be
    retrieved via $MilesEventInfoQueueEnum.

    This function has no effect if $MilesEventInfoQueueEnable has not been called.

    The ReportMask should be a logical or of the MilesEventInfoRequest enumerations.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueEventPositionPostbox(HMILESQUEUE Queue, HMILESPOSTBOX Postbox);
/*
    Adds a command to the queue to set the next event's position postbox.

    The position postbox will receive the millisecond position of the first
    voice that the event starts every mix.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueEvent3DOrientation(HMILESQUEUE Queue, F32 FaceX, F32 FaceY, F32 FaceZ, F32 UpX, F32 UpY, F32 UpZ);
/*
    Adds a command to the queue to set the next event's initial 3d orientation.

    All voices started by this event will use this orientation.

    Use $MilesQueueUpdateEvent3DOrientation to update the orientation over time.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueEvent3DPosition(HMILESQUEUE Queue, F32 X, F32 Y, F32 Z);
/*
    Adds a command to a queue to set the next event's initial 3d position.

***

    // Create a queue
    HMILESQUEUE queue = $MilesQueueCreate();

    // Add a command to run an event at a position
    $MilesQueueEvent3DPosition(queue, 10, 0, 10);
    $MilesQueueEventRun(queue, "mybank/myevent");
    $MilesQueueSubmit(queue);

    // On the next mix, "mybank/myevent" will be looked up and 
    // run, and voices started by the event will be placed at
    // position (10, 0, 10).

***

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueEventListenerMask(HMILESQUEUE Queue, U32 ListenerMask);
/*
    Adds a command to a queue to set the next event's listener mask.

***

    // Create a queue
    HMILESQUEUE queue = $MilesQueueCreate();

    // Add a command to run an event that spatializes to multiple listeners.
    $MilesQueueEvent3DPosition(queue, 10, 0, 10);

    // Spatialize against the first and second listeners.
    $MilesQueueEventListenerMask(queue, 0x3);
    $MilesQueueEventRun(queue, "mybank/myevent");
    $MilesQueueSubmit(queue);

    // On the next mix, "mybank/myevent" will be looked up and 
    // run, and voices started by the event will be placed at
    // position (10, 0, 10). Voices will be spatialized
    // against the position of both the first and second
    // listener.

***

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueEventPriorityAdjust(HMILESQUEUE Queue, S8 PriorityAdjustment);
/*
    Adds a command to the queue to set the priority adjustment for voices started by the next event.

    This value will be added to any priority calculation done for voices
    started by the next event, allowing the same event to be considered
    more or less important based on game state.

    See also $adv_limits.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueEventDelayVoicesByTime(HMILESQUEUE Queue, F32 DelayMs);
/*
    Adds a command to the queue to delay voices by the given number of milliseconds for the
    next event.

    This is in addition to the delay specified by the event in Miles Studio. The time is relative to
    when the event is processed (i.e. the queue is processed) not when this function is called
    or when the queue is submitted. The delay is sample accurate (a time that falls during a mix
    will be handled correctly).

    Note that this doesn't affect anything other that voices. If the event contains stops or actions
    other than starts, they will happen without any delay.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueEventVolume(HMILESQUEUE Queue, F32 VolumeLevel);
/*
    Adds a command to the queue to set the code based volume of the next event.

    The volume is a DAC level - use MilesDbToLevel or MilesLinearToLevel to convert if desired.

    This gain accumulates with any gain set by the designers on the event.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueEventFadeOutTime(HMILESQUEUE Queue, F32 FadeOutTimeMs);
/*
    Adds a command to the queue to set the fade out time for events stopped _by code_.

    Used for stopping voices with a fade, overriding the fade out specified by Miles Studio. Only
    used by "STOP" code events.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/


IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueEventStartTime(HMILESQUEUE Queue, F32 StartTimeMs);
/*
    Adds a command to the queue to set the start position in milliseconds of voices started
    by the next event.

    This is the position of the voice in its sources - eg if the source for the event is two
    seconds long, and 1000 were passed to this function, the voice would start halfway 
    through the source.
    
    This is in addition to the start position specified by the event in Miles Studio.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueEventDopplerFactor(HMILESQUEUE Queue, F32 DopplerEffectiveness, F32 const* ListenerVelocity, F32 const* EventVelocity, F32 MetersPerGameUnit);
/*
    Adds a command to the queue to set the doppler factor for the next event.

    For the parameters and example, refer to $MilesQueueUpdateEventDopplerFactor.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueEventRateFactor(HMILESQUEUE Queue, F32 PlaybackRateFactor);
/*
    Adds a command to the queue to set a playback rate factor for the next event.

    This can not be used with $MilesQueueEventDopplerFactor

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueEventControllerValue(HMILESQUEUE Queue, char const* ControllerName, F32 ControllerValue);
/*
    Adds a command to the queue to set a controller value that is visible only to the next event.

    The next event will use this value instead of the global value set
    by $MilesQueueControllerValue for any controller references. Each
    event can maintain a distinct value for each controller.

    To update a controller's value on a running event, use 
    $MilesQueueUpdateEventControllerValue.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueEventFilter3DPosition(HMILESQUEUE Queue, F32 X, F32 Y, F32 Z);
/*
    Adds a command to a queue to set the next event's filter 3d position.

    The next event run via $MilesQueueEventRun will only apply to voices
    that have the given position. This only has effect on control-type
    events (stop, pause, etc). 

    Note that the position is tested *exactly*, so if the position is calculated
    differently then it is likely that it will fail - make sure the
    exact same floats are passed to $MilesQueueEvent3DPosition and 
    $MilesQueueEventFilter3DPosition.

    See also $MilesQueueEventFilterId, $MilesQueueEventFilterName.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueEventFilterName(HMILESQUEUE Queue, char const* EventName);
/*
    Adds a command to a queue to set the next event's filter name.

    The next event run via $MilesQueueEventRun will only apply to voices
    that were started by events with this name. This only has effect on control-type
    events (stop, pause, etc). The name must be a fully qualified event name -
    context sensitive names will not match anything.

    See also $MilesQueueEventFilterId, $MilesQueueEventFilter3DPosition.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueEventFilterTemplateId(HMILESQUEUE Queue, MilesEventTemplateId const* TemplateId);
/*
    Adds a command to a queue to set the next event's filter by event template.

    The next event run via $MilesQueueEventRun will only apply to voices
    that were started by events with this index. This only has effect on control-type
    events (stop, pause, etc).    

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueEventFilterId(HMILESQUEUE Queue, U64 Filter);
/*
    Adds a command to a queue to set the next event's filter ID.

    $:Filter The event ID to filter the next event to.

    The next event run via $MilesQueueEventRun will use this filter.

    An event filter prevents any control-type actions from affecting any
    voices other than ones created by the event referenced by the
    given event ID.

    This is how you single out specific sounds to stop or alter via code.

    See also $MilesQueueEventFilterName, $MilesQueueEventFilter3DPosition.

    \<pre\>

    // Create a queue
    HMILESQUEUE queue = $MilesQueueCreate();

    // Start a sound with an event
    U64 SoundEventID = $MilesQueueEventRun(queue, "mybank/start_sound");
    $MilesQueueSubmit(queue);

    // Stop that specific sound. "mybank/stop_sounds" must also affect the sound,
    // meaning if it were stopping sounds on a different bus, it won't have
    // the desired effect.
    $MilesQueueEventFilterId(queue, SoundEventID);
    $MilesQueueEventRun(queue, "mybank/stop_sounds");
    $MilesQueueSubmit(queue);
    \</pre\>

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueEventRandomSeed(HMILESQUEUE Queue, U32 Seed);
/*
    Adds a command to the queue to set the next event's random seed.

    $:Seed A non-zero seed to use for all randomization of the next event.

    Each event carries its own copy of a LCG randomizer. By default, the event
    gets a random seed, however the seed can be manually specified using this
    function.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueEventSleeping(HMILESQUEUE Queue);
/*
    Adds a command to the queue to put the next event immediately to sleep.

    When an event starts in the sleeping state, play actions are not evaluated - so
    any source selection that relies on current global state will not occur until
    the event is woken up with $MilesQueueUpdateEventSleeping.
*/

// Global State
IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueControllerValue(HMILESQUEUE Queue, char const* ControllerName, F32 Value);
/*
    Adds a command to the queue to set a controller's value.

    $:ControllerName The name of the controller. This must match a name defined in Miles Studio.

    Controllers are the primary interface for dynamically responding voices
    or buses. Any voice or bus that references the named controller will use
    this value (potentially transformed per the controller definition) for
    the property referencing it.

    If the controller is not defined, this will have no effect.

    The value will be clamped to the controller's min and max range as set by 
    Miles Studio. 

    Controllers can be set on a global or an event-local level. This function
    sets the global value. If an event has a controller set via $MilesQueueEventControllerValue,
    then that value takes precedence.

    \<pre\>

    HMILESQUEUE queue = $MilesQueueCreate();
    $MilesQueueControllerValue(queue, "my_controller", .75f);
    $MilesQueueSubmit(queue);

    // On the next mix, the controller's value will be set to .75f.
    \</pre\>

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueListener3DPosition(HMILESQUEUE Queue, U32 ListenerIndex, F32 X, F32 Y, F32 Z);
/*
    Adds a command to the queue to set the listener's 3d position.

    $:queue The queue to add the command to.
    $:x x position
    $:y y position
    $:z z position

    The listener's position (and orientation) are used for determining 3D
    falloff and directionality for voices that are spatialized, as well
    as determining the value for the auto-controller "AutoDistance".

    \<pre\>

    HMILESQUEUE queue = $MilesQueueCreate();
    $MilesQueueListener3DPosition(queue, 10, 0, 10);
    $MilesQueueSubmit(queue);

    // On the next mix, the listener's position will be (10, 0, 10);
    \</pre\>      

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/  

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueListener3DOrientation(HMILESQUEUE Queue, U32 ListenerIndex, F32 FaceX, F32 FaceY, F32 FaceZ, F32 UpX, F32 UpY, F32 UpV);
/*
    Adds a command to the queue to set the listener's 3d orientation, specified
    via the forward facing vector and the upward facing vector.

    The listener's orientation (and position) are used for determining
    3D falloff and directionality for voices that are spatialized, as well
    as determining the value for the auto-controller "AutoDistance".

    The default orientation is looking down the position z axis:
    face: (0, 0, 1)
    up: (0, 1, 0);
    \<pre\>

    HMILESQUEUE queue = $MilesQueueCreate();
    $MilesQueueListener3DOrientation(queue, 1, 0, 0, 0, 1, 0);
    $MilesQueueSubmit(queue);

    // On the next mix, the listener's 3d orientation will be facing
    // (1, 0, 0) and up will be (0, 1, 0)
    \</pre\>  

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueListener3DVolumeCone(HMILESQUEUE Queue, U32 ListenerIndex, U32 ConeEnabled, F32 ConeInnerDegrees, F32 ConeOuterDegrees, F32 ConeOuterVolumeLevel);
/*
    Adds a command to the queue to set the listener's 3d cone.

    The mix after submission of the queue containing the command will
    utilize the cone settings.

    See also $MilesListenerSet3DVolumeCone.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueSetVoiceLimit(HMILESQUEUE Queue, char const* LimitName);
/*
    Adds a command to the queue to set the currently active voice limit set by name.

    The limit set must have been created in Miles Studio. If the name doesn't exist,
    then an error will be posted to the report function.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

// Updating active state
IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueUpdateEvent3DPosition(HMILESQUEUE Queue, U64 EventId, F32 X, F32 Y, F32 Z);
/*
    Adds a command to the queue to update the event's 3d position.

    $:Queue The queue to add the command to.
    $:EventId The event id as returned by $MilesQueueEventRun.
    $:X x position
    $:Y y position
    $:Z z position

    This will update all voices that were started by the given event to be placed
    at X, Y, Z. 

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueUpdateEvent3DOrientation(HMILESQUEUE Queue, U64 EventId, F32 FaceX, F32 FaceY, F32 FaceZ, F32 UpX, F32 UpY, F32 UpZ);
/*
    Adds a command to the queue to update the event's 3d orientation.

    This will update all voices that were started by the given event.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueUpdateEventControllerValue(HMILESQUEUE Queue, U64 EventId, char const* ControllerName, F32 Value);
/*
    Adds a command to update the controller value on a running event. The controller
    must have been initialized with a $MilesQueueEventControllerValue prior
    to the $MilesQueueEventRun associated with the given EventId.

    Controllers set this way are only visible to the given event. I.e. each
    event can have a distinct value for the controller.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueUpdateEventControllerValueReset(HMILESQUEUE Queue, U64 EventId, char const* ControllerName, F32 Value);
/*
    Adds a command to update the controller value on a running event. The controller
    must have been initialized with a $MilesQueueEventControllerValue prior
    to the $MilesQueueEventRun associated with the given EventId.

    Controllers set this way are only visible to the given event. I.e. each
    event can have a distinct value for the controller.

    This function differs from $MilesQueueUpdateEventControllerValue in that 
    any controller smoothing specified by the designer is ignored. In addition,
    any controllers that "Hold First Value" will use this value instead of the
    original value.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueUpdateEventDopplerFactor(HMILESQUEUE Queue, U64 EventId, F32 DopplerEffectiveness, F32 const* ListenerVelocity, F32 const* EventVelocity, F32 MetersPerGameUnit);
/*
    Adds a command to the queue to update the doppler factor for all voices started by the given event.

    $:EventId The event id, returned from $MilesQueueEventRun, to affect.
    $:DopplerEffectiveness The amount that the doppler shift should affect 
        the voices, between 0 and 1.
    $:ListenerVelocity The 3d velocity of the listener. Ignored if DopplerEffectiveness
        is zero. You can pass null in place of (0, 0, 0).
    $:EventVelocity The 3d velocity of the voices. Ignored if DopplerEffectiveness
        is zero. You can pass null in place of (0, 0, 0).
    $:MetersPerGameUnit Scaling factor to convert distances and velocities 
        from game units to real world meters. Ignored if DopplerEffectiveness
        is zero.

    Doppler shift is the effect where a sound gets higher in pitch as
    it approaches, and lower in pitch as it leaves. As a result, it's a function
    of the relative velocities of the sound and the listener. This function
    acts as a helper to compute the doppler shift factor as well as set it for
    all voices started by an event.

    If the DopplerEffectiveness parameter is zero, the other math-relevant 
    parameters are ignored.

    The underlying implementation for doppler factor is as a playback rate
    adjustment.

    This should be updated every frame - Miles does no internal integration
    of position.

***

    // Assume the listener is in a jet and moving fast in one direction.
    F32 player_velocity[] = {1000, 0, 0};

    HMILESQUEUE queue = $MilesQueueCreate();

    // Start the event somewhere in the world.
    $MilesQueueEvent3DPosition(queue, 0, 100, 0);

    // Set the initial doppler for the event - full effectiveness, 
    // but the event isn't moving, and our game units are in meters (so 1.0f translation)
    $MilesQueueEventDopplerFactor(queue, 1.0f, player_velocity, 0, 1.0f);

    // Start a sound that will be affected.
    U64 event_id = $MilesQueueEventRun(queue, "mybank/start_sound");

    $MilesQueueSubmit(queue);

    // ... time passes, we want to update the doppler factor due to a change in
    // position/velocity. For this example, lets say the position has moved
    // away from the origin due to the high velocity.
    F32 player_position[] = {500, 0, 0};
    $MilesQueueUpdateEvent3DPosition(queue, event_id, player_position);

    // Even though the parameters are the same as above, due to the position
    // change, the doppler factor can change so we need to update it.
    $MilesQueueUpdateEventDopplerFactor(queue, 1.0f, player_velocity, 0, 1.0f);

    $MilesQueueSubmit(queue);

***

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueUpdateEventRateFactor(HMILESQUEUE Queue, U64 EventId, F32 PlaybackRateFactor);
/*
    Adds a command to the queue to update the playback rate factor for a given event.

    This can not be used with $MilesQueueUpdateEventDopplerFactor

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueUpdateEventVolume(HMILESQUEUE Queue, U64 EventId, F32 VolumeLevel);
/*
    Adds a command to the queue to update the code driver volume for a given event.

    See also $MilesQueueEventVolume.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueUpdateEventSleeping(HMILESQUEUE Queue, U64 EventId, U32 IsSleeping);
/*
    Adds a command to the queue to update the event's current sleep state.

    If IsSleeping is nonzero, then the event will be put to sleep if it isn't already asleep.
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueueEventSignal(HMILESQUEUE Queue, U64 EventId, char const* Signal);
/*
    Adds a command to the queue to send a signal to an event.

    This function is used to activate signal transitions in state based events.
*/

//-----------------------------------------------------------------------------
//
// Event Information Queue Management
//
//-----------------------------------------------------------------------------
IDOC typedef enum MilesEventInfoRequest
{
    MilesEventInfoRequestEventInfo =        0x1, /*
        Posts EventDispatchStart, EventDispatchEnd,
        and EventVoicesComplete.
        */

    MilesEventInfoRequestVoiceInfo =        0x2, /*
        Posts VoiceStart and VoiceDestroy.
        */

    MilesEventInfoRequestVoiceSource  =     0x4, /*
        Posts VoiceSource.
        */

} MilesEventInfoRequest;

typedef enum MilesEventInfoType
{
    MilesEventInfoTypeEventDispatchStart = 1,

    MilesEventInfoTypeEventDispatchEnd,
    /*
        Only header - no payload - the event that ended was the one
        just before
    */

    MilesEventInfoTypeEventVoicesComplete,
    MilesEventInfoTypeVoiceStart,
    MilesEventInfoTypeVoiceDestroy,
    MilesEventInfoTypeVoiceSource,
    MilesEventInfoTypeDebugQueueComplete,
    MilesEventInfoTypeAuditionPost,
    MilesEventInfoTypeAuditionRecreate,
} MilesEventInfoType;
RR_COMPILER_ASSERT(sizeof(MilesEventInfoType) == 4);

typedef struct MilesEventInfoHeader
{
    U16 Type;
    U16 SizeInDwords;
} MilesEventInfoHeader;

typedef struct MilesEventInfoEventDispatchStart
{
    U16 Type;        // = MilesEventInfoTypeEventDispatchStart
    U16 SizeInDwords;
    U32 ExpectedVoiceCount;

    // Use MilesAssemble64BitId(EventIdLow, EventIdHigh) to create the full U64 id.
    U32 EventIdLow;
    U32 EventIdHigh;
} MilesEventInfoEventDispatchStart;

typedef struct MilesEventInfoEventVoicesComplete
{
    U16 Type;        // = MilesEventInfoTypeEventVoicesComplete
    U16 SizeInDwords;
    U32 EventIdLow;
    U32 EventIdHigh;
} MilesEventInfoEventVoicesComplete;

typedef struct MilesEventInfoVoiceStart
{
    U16 Type;        // = MilesEventInfoTypeVoiceStart
    U16 SizeInDwords;
    U32 VoiceId;
    U32 ActionIndex;
    F32 NominalVoiceLengthSec; // If -1, the voice loops infinitely. This doesn't take in to account playback rate shifts.
    F32 InitialGain; // Input Gain + 3D Falloff (if any)
} MilesEventInfoVoiceStarted;

typedef struct MilesEventInfoVoiceDestroy
{
    U16 Type;        // = MilesEventInfoTypeVoiceDestroy
    U16 SizeInDwords;
    U32 VoiceId;
} MilesEventInfoVoicedestroy;

typedef struct MilesEventInfoVoiceSource
{
    U16 Type;        // = MilesEventInfoTypeVoiceSource
    U16 SizeInDwords;
    U32 VoiceId;
    char SourceName[4];
} MilesEventInfoVoiceSource;

typedef struct MilesEventInfoAuditionPost
{
    U16 Type;       // = MilesEventInfoTypeAuditionPost
    U16 SizeInDwords;
    MilesEventTemplateId TemplateId;
    U16 IsSource;
    U16 SourceHasDefaultEvent;
    char NameWithBank[4]; // BankName/AssetNameWithPath
} MilesEventInfoAuditionPost;

#define MilesAssemble64BitId(low, high) ((((U64)(high)) << 32) | (U64)(low))

IDOC RADEXPFUNC void RADEXPLINK MilesEventInfoQueueEnable(HMILESDRIVER Driver);
/*
    Causes the event system to start queuing basic information about
    events for the foreground to consume. If consumption stops for
    an extended period of time (i.e. the post queue gets full), the 
    events will stack begin stacking up in memory and warnings
    will be passed to the error function. To prevent this situation,
    drain the info queue using $MilesEventInfoQueueEnum periodically.

    This function should be called once on startup if the event info
    queue is to be used.

    *Thread Safety* $(Threading:TS_Lock)
*/


IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesEventInfoQueueEnum(HMILESDRIVER Driver, MilesEventInfoHeader** Header);
/*
    Enumerates the current information available. Once this function returns zero
    there is no further information available. The amount of information can
    update at any time from the mixer thread.

    Usage:

    ${
        MilesEventInfoHeader* Header = 0;
        while (MilesEventInfoQueueEnum(gDriver, &Header))
        {
            // Switch on the header type and cast to the respective structs.
            switch (Header->Type)
            {
            case MilesEventInfoTypeEventVoicesComplete: break;
            }
        }
    $}


    This function is thread safe with respect to the mixer thread but not
    with itself or shutdown, and should only be called from one thread when
    it is known that Miles will continue running.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC HMILESPOSTBOX MILES_NO_THUNK RADEXPLINK MilesPostboxCreate(HMILESDRIVER Driver);
/*
    Creates a postbox for receiving async realtime values back from the runtime.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC F32 MILES_NO_THUNK RADEXPLINK MilesPostboxGetValue(HMILESPOSTBOX Postbox);
/*
    Retrieves the current value stored in the postbox.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesQueuePostboxDestroy(HMILESQUEUE Queue, HMILESPOSTBOX Postbox);
/*
    Adds a command to the queue to destroy the given postbox.

    The postbox is not destroyed until the containing queue is submitted, however
    postboxes should be considered invalid after this call, and should not 
    be passed to any function.

    *Thread Safety* $(Threading:TS_ValidDriverNoLock)
*/

//-----------------------------------------------------------------------------
//
// Audition API
//
//-----------------------------------------------------------------------------
//idoc(parent, AuditionAPI)
IDOC typedef enum MilesAuditionType
{
    MilesAuditionTypeProfile = 1,       // Specifies that auditioning only send profiling and replay information to Miles Studio
    MilesAuditionTypeDownload = 2,      // Specifies that all assets (project and banks) will be downloaded from Miles Studio at runtime.
    MilesAuditionType_FORCE32 = 0x80000000
} MilesAuditionType;

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesAuditionStartup(HMILESDRIVER Driver, MilesAuditionType AuditionType);
/*
    Initializes and prepares Miles to connect to a running instance of Miles Studio.

    $:Driver The driver that will connect to Miles Studio.
    $:AuditionType Controls whether all assets will be downloaded from Miles Studio, or if 
        only profiling information will be sent.

    *Thread Safety* $(Threading:TS_Lock)
*/

IDOC RADEXPFUNC U32 MILES_NO_THUNK RADEXPLINK MilesAuditionConnect(char const* Address);
/*
    Connects the Miles Runtime to a running instance of Miles Studio. 

    $:Address The IP address of the machine running Miles Studio.

    Miles Studio will need to have a project opened of the same name, with the
    needed banks opened and available. The names expected are the names passed
    via $MilesProjectLoad and $MilesBankLoad.

    *Thread Safety* $(Threading:TS_Lock)
*/

RADEXPFUNC void MILES_NO_THUNK RADEXPLINK MilesAuditionCounts(U32* event_count, U32* source_count, U32* resource_count);
/*
    Basic profiling information for the audition system.

    *Thread Safety* $(Threading:TS_Pure)
*/
#endif // __MILES_H__
