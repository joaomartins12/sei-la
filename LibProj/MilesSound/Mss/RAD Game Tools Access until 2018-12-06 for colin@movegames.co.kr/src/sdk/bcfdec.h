//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#undef RAD_ALIGN
#include "mss.h"
#include "imssapi.h"

struct BCFHEADER
{
    U32 tag; // BCF1
    U8 version;
    U8 channels;
    U16 rate;
    U32 sample_count;
    U32 max_comp_space_needed;
    U32 output_file_size;
    U32 seek_table_entry_count;
};

typedef struct BINKAUDIODECOMP * HBINKAUDIODECOMP;

struct BcfStream
{
    //
    // SPU header for mixing (post decode)
    //

    // This is actually ignored in the BCF decoder.
    S64 OutputSampleCount; 
    char* output_reservoir;

    // originally mp3 samples pointer, doesn't appear to be 
    // used in the mixer, not sure what its supposed to be.
    S16* SeemsToBeIgnored; 
    U32 output_reservoir_valid_len; // Valid len in bytes from output_reservoir.
    U32 output_cursor; // Current output location to copy from in output_reservoir


    // file stuff
    UINTa user;
    VOIDFUNC* fetch_CB;
    MSS_FREE_TYPE * free_CB;
    U32 total_size;
    U32 current_position;
    U32 expected_seek; // current_position is valid, and we can use it to get to a frame boundary.
    U32 unexpected_seek; // current_position might not be valid, so we have to hunt.
    
    // codec stuff - some output stuff moved to the top of the stream for SPU reasons.
    BCFHEADER header;
    U32* seektable;
    S32 len_ms;
    U32 frames_per_seek_entry;
    char* input_reservoir;
    U32 input_reservoir_valid_len;
    U32 input_cursor;
    U32 input_reservoir_size;
    U32 next_input_size;
    U32 output_reservoir_size;
    U32 output_chunk_size; // amount output by one call to bink.
    U32 output_cull_count; // number of samples to ignore during next process()
    U32 loop_block_cull_samples; // number of samples to cull off the front on a loop.
    U32 loop_block_end_block; // position of the stream where the loop end block starts
    U32 loop_block_end_samples;  // number of samples to output out of the last block.
    U32 sample_output_cap; // cap on the number of samples to output before stopping (in this block)
    U32 header_size;

    U32 latch_output_cull_count;
    U32 latch_current_position;

    S32 drain_input_then_reset;

    S32 StreamCount;
    S32 ChannelsPerStream[4];
    S32 StreamSizes[4];
    HBINKAUDIODECOMP Decomp[4];
};

#define AT_LEAST_PER_CHANNEL_BYTES 1024

void AILCALL decode_bink_buffer( BcfStream* BcfStr );

