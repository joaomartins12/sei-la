// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2007 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net


// Precompiled Header
#include "NiSystemPCH.h"

#include "NiStandardAllocator.h"
#include "NiSmallObjectAllocator.h"

#ifdef NI_USE_SMALL_OBJECT_ALLOCATOR
NiSmallObjectAllocator* NiStandardAllocator::ms_pkSmallAlloc = NULL; 
#endif

#define IS_POWER_OF_TWO(x)   (((x)&(x-1)) == 0)

// In order to facilitate speedy deallocations, it is necessary to prepend
// the size of the allocation. This total size in bytes allows us to quickly
// determine how to handle the memory.
// 
// The code works by adding "NI_MEM_ALIGNMENT_DEFAULT" bytes
// to the size of the allocation. This value is expected to be a multiple
// of the compiler's default alignment. The size, in bytes, of the allocation
// is then set in this header. The memory address is advanced to the "real"
// address.
// 
// Upon deallocation, this value is retrieved by jumping back the header size,
// grabbing the allocation size and then forwarding the value onward into
// the memory management system.
//
// This design assumes that some allocator will be able to better handle
// knowing the full size, rather than inserting its own header. 
// 
// Note: Aligned allocation routines are a special case. Since they can
// be of any alignment, we forward them on without any additional tracking.

//---------------------------------------------------------------------------
void* NiStandardAllocator::Allocate(
    size_t& stSizeInBytes,
    size_t& stAlignment,
    NiMemEventType eEventType,
    bool bProvideAccurateSizeOnDeallocate,
    const char* pcFile,
    int iLine,
    const char* pcFunction)
{
    NIASSERT(IS_POWER_OF_TWO(stAlignment));
    
#ifdef NI_USE_SMALL_OBJECT_ALLOCATOR
    NIASSERT(ms_pkSmallAlloc != NULL);
    bool bPrependSize = false;
    void* pvMemory = NULL;

    if (stAlignment == NI_MEM_ALIGNMENT_DEFAULT && 
        bProvideAccurateSizeOnDeallocate == false &&
        eEventType != NI_ALIGNEDMALLOC &&
        eEventType != NI_ALIGNEDREALLOC)
    {
        stSizeInBytes += NI_MEM_ALIGNMENT_DEFAULT;
        bPrependSize = true;
    }

    if (eEventType != NI_ALIGNEDMALLOC &&
        eEventType != NI_ALIGNEDREALLOC && 
        stSizeInBytes <= MAX_SMALL_OBJECT_SIZE && 
        stAlignment == NI_MEM_ALIGNMENT_DEFAULT)
    {
        pvMemory = ms_pkSmallAlloc->Allocate(stSizeInBytes);
    }
    else
    {
        pvMemory = NiExternalAlignedMalloc(stSizeInBytes, stAlignment);
    }

    if (pvMemory && bPrependSize)
    {
        NIASSERT(NI_MEM_ALIGNMENT_DEFAULT >= sizeof(size_t));
        //*((size_t*) pvMemory) = stSizeInBytes;
        SetSizeToAddress(pvMemory, stSizeInBytes);
        pvMemory = ((char*)pvMemory) + NI_MEM_ALIGNMENT_DEFAULT;
    }

    return pvMemory;
#else
    return NiExternalAlignedMalloc(stSizeInBytes, stAlignment);
#endif
}
//---------------------------------------------------------------------------
void NiStandardAllocator::Deallocate(
    void* pvMemory, 
    NiMemEventType eEventType, 
    size_t stSizeInBytes)
{
    if (pvMemory == NULL)
        return;

#ifdef NI_USE_SMALL_OBJECT_ALLOCATOR
    NIASSERT(ms_pkSmallAlloc != NULL);

    if (eEventType != NI_ALIGNEDFREE && eEventType != NI_ALIGNEDREALLOC && 
        NI_MEM_DEALLOC_SIZE_DEFAULT == stSizeInBytes)
    {
        pvMemory = ((char*)pvMemory) - NI_MEM_ALIGNMENT_DEFAULT;
        NIASSERT(NI_MEM_ALIGNMENT_DEFAULT >= sizeof(size_t));
        //stSizeInBytes = *((size_t*) pvMemory); 
        stSizeInBytes = GetSizeFromAddress(pvMemory);
    }

    if (stSizeInBytes <= MAX_SMALL_OBJECT_SIZE)
    {
        ms_pkSmallAlloc->Deallocate(pvMemory, stSizeInBytes);
        return;
    }
#endif

    NiExternalAlignedFree(pvMemory);
}
//---------------------------------------------------------------------------
void* NiStandardAllocator::Reallocate(
    void* pvMemory,
    size_t& stSizeInBytes,
    size_t& stAlignment,
    NiMemEventType eEventType,
    bool bProvideAccurateSizeOnDeallocate,
    size_t stSizeCurrent,
    const char* pcFile,
    int iLine,
    const char* pcFunction)
{
    NIASSERT(IS_POWER_OF_TWO(stAlignment));

    // The deallocation case should have been caught by us before in
    // the allocation functions.
    NIASSERT(stSizeInBytes != 0);
    
    // The standard allocation case should have be caught by us before 
    // in the realloc functions.
    
#ifdef NI_USE_SMALL_OBJECT_ALLOCATOR
    // Determine the current size for the address
    if (eEventType != NI_ALIGNEDFREE && eEventType != NI_ALIGNEDREALLOC && 
        NI_MEM_DEALLOC_SIZE_DEFAULT == stSizeCurrent && 
        bProvideAccurateSizeOnDeallocate == false)
    {
        pvMemory = ((char*)pvMemory) - NI_MEM_ALIGNMENT_DEFAULT;
        NIASSERT(NI_MEM_ALIGNMENT_DEFAULT >= sizeof(size_t));
        //stSizeCurrent = *((size_t*) pvMemory); 
        stSizeCurrent = GetSizeFromAddress(pvMemory);
    }

    // pad the allocation for the new current size
    bool bPrependSize = false;
    if (stAlignment == NI_MEM_ALIGNMENT_DEFAULT && 
        bProvideAccurateSizeOnDeallocate == false &&
        eEventType != NI_ALIGNEDMALLOC &&
        eEventType != NI_ALIGNEDREALLOC)
    {
        stSizeInBytes += NI_MEM_ALIGNMENT_DEFAULT;
        bPrependSize = true;
    }

    if (stSizeCurrent != NI_MEM_DEALLOC_SIZE_DEFAULT && 
        stSizeInBytes <= stSizeCurrent && bPrependSize)
    {
        stSizeInBytes = stSizeCurrent;
        pvMemory = ((char*)pvMemory) + NI_MEM_ALIGNMENT_DEFAULT;
        return pvMemory;
    }

    void* pvNewMemory = NULL;

    NIASSERT(ms_pkSmallAlloc != NULL);
    if (eEventType != NI_ALIGNEDMALLOC &&
        eEventType != NI_ALIGNEDREALLOC &&
        (stSizeCurrent <= MAX_SMALL_OBJECT_SIZE || 
        stSizeInBytes <= MAX_SMALL_OBJECT_SIZE))
    {
        NIASSERT(stAlignment ==  NI_MEM_ALIGNMENT_DEFAULT);
        NIASSERT(stSizeCurrent != NI_MEM_DEALLOC_SIZE_DEFAULT);

        // Since the small object allocator does not support reallocation
        // we must manually reallocate.

        // Remove any header information that we have added
        // in this call.
        stSizeInBytes -= NI_MEM_ALIGNMENT_DEFAULT;

        // Allow the allocation call to do its job, which
        // should mean that the size will be embedded just before the
        // address that we receive.
        pvNewMemory = Allocate(stSizeInBytes, stAlignment, eEventType,
            bProvideAccurateSizeOnDeallocate, pcFile, iLine, pcFunction);
        bPrependSize = false;

        if (stSizeCurrent != NI_MEM_DEALLOC_SIZE_DEFAULT)
        {
            char* pvMemoryToCopy = ((char*)pvMemory) + 
                NI_MEM_ALIGNMENT_DEFAULT;
            size_t stSizeToCopy = stSizeCurrent - NI_MEM_ALIGNMENT_DEFAULT;

            NiMemcpy(pvNewMemory, stSizeInBytes, pvMemoryToCopy, stSizeToCopy);
            Deallocate(pvMemory, eEventType, stSizeCurrent);
        }
    }
    else
    {
        pvNewMemory = NiExternalAlignedRealloc(pvMemory, stSizeInBytes, 
            stAlignment);
    }

    if (pvNewMemory && bPrependSize)
    {
        NIASSERT(NI_MEM_ALIGNMENT_DEFAULT >= sizeof(size_t));
        //*((size_t*) pvNewMemory) = stSizeInBytes;
        SetSizeToAddress(pvNewMemory, stSizeInBytes);
        pvNewMemory = ((char*)pvNewMemory) + NI_MEM_ALIGNMENT_DEFAULT;
    }

    return pvNewMemory;
#else
    return NiExternalAlignedRealloc(pvMemory, stSizeInBytes, stAlignment);
#endif   

}
//---------------------------------------------------------------------------
bool NiStandardAllocator::TrackAllocate(
    const void* const pvMemory, 
    size_t stSizeInBytes, 
    NiMemEventType eEventType, 
    const char* pcFile, 
    int iLine,
    const char* pcFunction)
{
    return false;
}
//---------------------------------------------------------------------------
bool NiStandardAllocator::TrackDeallocate(
    const void* const pvMemory, 
    NiMemEventType eEventType)
{
    return false;
}
//---------------------------------------------------------------------------
void NiStandardAllocator::Initialize()
{
#ifdef NI_USE_SMALL_OBJECT_ALLOCATOR
    ms_pkSmallAlloc = NiExternalNew NiSmallObjectAllocator();
#endif
}
//---------------------------------------------------------------------------
void NiStandardAllocator::Shutdown()
{
#ifdef NI_USE_SMALL_OBJECT_ALLOCATOR
    NiExternalDelete ms_pkSmallAlloc;
    ms_pkSmallAlloc = NULL;
#endif
}
//---------------------------------------------------------------------------
bool NiStandardAllocator::VerifyAddress(const void* pvMemory)
{
    return true;
}
//---------------------------------------------------------------------------
#ifdef NI_USE_SMALL_OBJECT_ALLOCATOR
NiSmallObjectAllocator* NiStandardAllocator::GetSmallAllocator()
{
    return ms_pkSmallAlloc;
}
#endif
//---------------------------------------------------------------------------
size_t NiStandardAllocator::GetSizeFromAddress(void* pMemory)
{
    size_t stSize;
    char* pcMemory = (char*) pMemory;
    char* pcSize = (char*)&stSize;
    NIASSERT(sizeof(size_t) == 4);
    pcSize[0] = pcMemory[0];
    pcSize[1] = pcMemory[1];
    pcSize[2] = pcMemory[2];
    pcSize[3] = pcMemory[3];
    
    /*
    for (size_t i = 0; i < sizeof(size_t); i++)
    {
        pcSize[i] = pcMemory[i];
    }
    */

    return stSize;
}
//---------------------------------------------------------------------------
void NiStandardAllocator::SetSizeToAddress(void* pMemory, size_t stSize)
{
    char* pcMemory = (char*) pMemory;
    char* pcSize = (char*)&stSize;
    
    NIASSERT(sizeof(size_t) == 4);
    pcMemory[0] = pcSize[0];
    pcMemory[1] = pcSize[1];
    pcMemory[2] = pcSize[2];
    pcMemory[3] = pcSize[3];

    /*
    for (size_t i = 0; i < sizeof(size_t); i++)
    {
        pcMemory[i] = pcSize[i];
    }
    */
}
//---------------------------------------------------------------------------

