#include "mss.h"
#include "imssapi.h"

#include <string.h> // for memmove
#include <stdarg.h>
#include <stdio.h>

#include "mileseventhandle.h"
#include "stringbuilder.h"

//
// The handle system is a stack allocator that is capable
// of defragmenting when the client can assert that there
// are no outstanding pointers.
//
// We begin life with a heap, and all of our allocations
// occur within it.
//
// The handle table is stored at the end of the heap, and
// grows 'downward'. User data allocations are supplied
// from the bottom.
//
// User data allocations require a single 4 byte index
// applied before the return pointer that reference the
// handle table index that uses them (negative number).
//
// The handle index returned to the user is positive, and
// must be negated before use.
//
// The free list is stored as a delta-linked list within
// the handle table. if s_MilesEvFreeBase is nonzero,
// it is the index of the last free index. That indeces
// ptr value is the delta to the next free index. It is
// set up to return s_MilesEvFreeBase to zero if it is the
// last one.
//

//
// The defragmentation should be very fast, but it is linear in
// active allocation count if no work needs to be done, and linear
// in byte count when moving data.
//
// Since it only "scoots" allocations towards the bottom of the
// heap, it has the effect of packing long term allocations towards
// the bottom, so frequent small allocations cause minimal data
// movement as they will always be at the top of the stack.
//
// Freeing a long term allocation causes basically the entire
// heap to scooch.
//
// Note that for shutdown, the entire system can be reinitialized
// in only a few instructions, which may be preferrable to
// a huge number of frees followed by a defrag().
//

//#define DEBUG_CLEARS

#define DELETED_ENTRY 1

/* extern */S32 g_MilesEvValidHandleCount = 0;
/* extern */HandleEntry* g_MilesEvValidHandlePtr = 0;

static S32 s_MilesEvFreeBase = 0;
static S32 s_MilesEvLastMagic = 1;
static S32 s_break_on_memory_error = 0;

char* s_BasePointer = 0;
char* s_Current = 0;

// Pointers passed to handle_init
char* s_Heap = 0;
S32 s_HeapMax = 0;

// Flag to determine if we should try to defrag.
S32 s_ShouldDefrag = 0;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void Handle_Shutdown()
{
    s_Heap = 0;
    s_HeapMax = 0;
    s_BasePointer = 0;
    s_Current = 0;

    g_MilesEvValidHandleCount = 0;
    s_MilesEvFreeBase = 0;

    // We don't reinit magic.
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void Handle_Init(void* i_Memory, S32 i_MemorySize)
{
    Handle_Shutdown();

    //
    // Init the base pointer to the offset we need.
    //
    // We want allocations to be on 16 byte alignment, 
    // so start us off at 16 byte - 4 for the index.
    //
    s_Heap = (char*)i_Memory;
    s_HeapMax = i_MemorySize;

    s_BasePointer = (char*)( ((SINTa)s_Heap + 0xf) & ~0xf);

    if (s_BasePointer - 4 < s_Heap) s_BasePointer += 16;

    // Terminate our allocation list.
    *(S32*)(s_BasePointer - 4) = 0;

    s_Current = s_BasePointer;

    // Setup the handle table to point at the end of the heap.
    g_MilesEvValidHandlePtr = (HandleEntry*)(s_Heap + s_HeapMax);
    g_MilesEvValidHandleCount = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void Handle_Defrag()
{
    if (s_ShouldDefrag == 0) 
        return;

    //
    // Walk the heap...
    //
    S32* pCurrentIndex = (S32*)(s_BasePointer - 4);
    char* pWritePointer = s_BasePointer;

    while (*pCurrentIndex != 0)
    {
        S32 entry_size = 0;
        if (*pCurrentIndex == DELETED_ENTRY)
        {
            entry_size = *(pCurrentIndex + 1);
        }
        else
        {
            // Valid entry
            // Move to our current position.
            HandleEntry CurHandle = g_MilesEvValidHandlePtr[*pCurrentIndex];

            S32 cur_handle_spacing = CurHandle.bytes;
            cur_handle_spacing += 8;
            cur_handle_spacing = (cur_handle_spacing + 15) & ~15;

            // Copy the memory for this entry forward.
            if (CurHandle.ptr != pWritePointer)
            {
                S32 MovedIndex = *pCurrentIndex;
                memmove(pWritePointer-4, (char*)CurHandle.ptr-4, cur_handle_spacing);
                g_MilesEvValidHandlePtr[MovedIndex].ptr = pWritePointer;
            }
            pWritePointer += cur_handle_spacing;

            entry_size = cur_handle_spacing;
        }

        // Use the bytecount to advance to the next block.
        pCurrentIndex += (entry_size) >> 2;
    }

#ifdef DEBUG_CLEARS
    memset(pWritePointer, 0, (char*)g_MilesEvValidHandlePtr - g_MilesEvValidHandleCount * sizeof(HandleEntry) - pWritePointer);
#endif

    *(S32*)(pWritePointer - 4) = 0;
    s_Current = pWritePointer;
    
    s_ShouldDefrag = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Handle Handle_AllocateInfo(S32 i_ByteCount, char const* file, S32 line)
{
    // Check for room.
    //
    // s_Current is 16 byte aligned.
    // s_Current[-4] is going to hold our result index.
    // s_Current[OrigRequest] is our border detect.
    //
    // We need (i_ByteCount+8) to be divisible by 16.
    //
    S32 OrigRequest = i_ByteCount;

    i_ByteCount += 8;
    i_ByteCount = (i_ByteCount + 15) & ~15;

    // In addition, we need to check for handle table space.
    S32 HandleSpaceNeeded = g_MilesEvValidHandleCount * sizeof(HandleEntry);

    // if we have to allocate a new handle entry, the space needed goes up.
    if (s_MilesEvFreeBase == 0)
        HandleSpaceNeeded += sizeof(HandleEntry);

    S32 AvailMem = (S32)(((char*)g_MilesEvValidHandlePtr - s_Current) - HandleSpaceNeeded);
    if (AvailMem < i_ByteCount)
        return Handle_Clear();

    //
    // Find the handle entry we are going to use
    //
    S32 ResultIndex = 0;

    if (s_MilesEvFreeBase)
    {
        // There are entries in the free list.
        ResultIndex = -s_MilesEvFreeBase;
        s_MilesEvFreeBase = s_MilesEvFreeBase + (S32)(SINTa)g_MilesEvValidHandlePtr[s_MilesEvFreeBase].ptr;
    }
    else
    {
        // We are using the next open entry.
        g_MilesEvValidHandleCount++;
        ResultIndex = g_MilesEvValidHandleCount;
    }


    // We return s_Current
    void* mem = s_Current;

    // Clear to zero
    memset(mem, 0, OrigRequest);
    
    // set end marker
    *((U32*)((char*)mem + OrigRequest)) = 0xdeadbeef;

    // Handle index for this allocation is the 4 bytes before the actual mem.
    S32* pHandleIndex = (S32*)((char*)mem - 4);

    // Move s_Current to the next 16 byte aligned allocation point.
    s_Current += i_ByteCount; 

    // Terminate the index chain.
    *(S32*)(s_Current - 4) = 0;

    // Setup return
    Handle Result;
    Result.index = ResultIndex;
    Result.magic = s_MilesEvLastMagic++;
    if (s_MilesEvLastMagic == 0) 
        s_MilesEvLastMagic = 1; // handle wrap.


    // Update state.
    HandleEntry* entry = &g_MilesEvValidHandlePtr[-ResultIndex];

#ifdef DEBUG_HANDLE_ALLOCS
    entry->allocated_file = file;
    entry->allocated_line = line;
#endif    
    entry->bytes = OrigRequest;
    entry->magic = Result.magic;
    entry->ptr = mem;
    *pHandleIndex = -ResultIndex;
    
    return Result;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void Handle_Free(Handle handle)
{
    if (handle.index == 0)
        return; // cleared handle - not an error to clear this.

    if ((U32)handle.index > (U32)g_MilesEvValidHandleCount) 
    {
        if (s_break_on_memory_error)
            RR_BREAK();

        RR_ASSERT_LITE_RELEASE(0);
        return; // trash handle (maybe double release)
    }

    HandleEntry HE = g_MilesEvValidHandlePtr[-handle.index];
    if (HE.magic != handle.magic) 
    {
        if (s_break_on_memory_error)
            RR_BREAK();

        RR_ASSERT_LITE_RELEASE(0);
        return; // double release
    }

    S32 ActualIndex = -handle.index;
    
    // make sure we didn't go over the end.
    if ( *(U32*)((char*)HE.ptr + HE.bytes) != 0xdeadbeef)
    {
        if (s_break_on_memory_error)
            RR_BREAK();

        RR_ASSERT_LITE_RELEASE(0);
    }

    // Recompute the amount we allocated from the amount requested.
    S32 entry_spacing = HE.bytes;
    entry_spacing += 8;
    entry_spacing = (entry_spacing + 15) & ~15;
    
    // invalidate the entry.
    g_MilesEvValidHandlePtr[ActualIndex].magic = 0;

    // Mark the entry in the index chain as deleted.
    *((S32*)g_MilesEvValidHandlePtr[ActualIndex].ptr - 1) = DELETED_ENTRY;

    // Store the spacing to the next entry where the data used to be, so we can skip this entry during defrag
    *((S32*)g_MilesEvValidHandlePtr[ActualIndex].ptr) = entry_spacing;

    // If we happened to free the last handle, just pop it.
    if (handle.index == g_MilesEvValidHandleCount)
    {
        g_MilesEvValidHandleCount--;
        s_ShouldDefrag = 1;
        return;
    }

    //
    // Update the free list.
    //
    if (s_MilesEvFreeBase)
    {
        S32 Diff = s_MilesEvFreeBase - ActualIndex;
        g_MilesEvValidHandlePtr[ActualIndex].ptr = (void*)(SINTa)Diff;
        s_MilesEvFreeBase = ActualIndex;
    }
    else
    {
        g_MilesEvValidHandlePtr[ActualIndex].ptr = (void*)(SINTa)(-ActualIndex);
        s_MilesEvFreeBase = ActualIndex;
    }

    s_ShouldDefrag = 1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void Handle_MemStats(S32* o_TotalSize, S32* o_RemainingSize)
{
    if (o_RemainingSize == 0 || o_TotalSize == 0) return;

    *o_TotalSize = s_HeapMax;

    S32 HandleSpaceNeeded = g_MilesEvValidHandleCount * sizeof(HandleEntry);
    S32 AvailMem = (S32)(((char*)g_MilesEvValidHandlePtr - s_Current) - HandleSpaceNeeded);
    *o_RemainingSize = AvailMem;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DXDEF void AILEXPORT AIL_event_system_break_on_mem_error(S32 on_off)
{
    s_break_on_memory_error = on_off;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DXDEF char* AILEXPORT AIL_event_system_dump_handles()
{
    StringBuilder SB;
    SB.Append("index,offset,bytes,file,line\r\n");

    S32* pCurrentIndex = (S32*)(s_BasePointer - 4);

    while (*pCurrentIndex != 0)
    {
        S32 entry_size = 0;
        if (*pCurrentIndex == DELETED_ENTRY)
        {
            entry_size = *(pCurrentIndex + 1);
        }
        else
        {
            // Valid entry
            // Move to our current position.
            HandleEntry CurHandle = g_MilesEvValidHandlePtr[*pCurrentIndex];

#ifdef DEBUG_HANDLE_ALLOCS
            SB.AppendFormat("%d,%d,%d,%s,%d\r\n", *pCurrentIndex, AIL_ptr_dif(CurHandle.ptr, s_BasePointer), CurHandle.bytes, CurHandle.allocated_file, CurHandle.allocated_line);
#else
            SB.AppendFormat("%d,%d,%d,%s,%d\r\n", *pCurrentIndex, AIL_ptr_dif(CurHandle.ptr, s_BasePointer), CurHandle.bytes, "<debug info missing>", 0);
#endif            
            S32 cur_handle_spacing = CurHandle.bytes;
            cur_handle_spacing += 8;
            cur_handle_spacing = (cur_handle_spacing + 15) & ~15;

            entry_size = cur_handle_spacing;
        }

        // Use the bytecount to advance to the next block.
        pCurrentIndex += (entry_size) >> 2;
    }

    char* ret = SB.Str();
    SB.Relinquish();
    return ret;
}
