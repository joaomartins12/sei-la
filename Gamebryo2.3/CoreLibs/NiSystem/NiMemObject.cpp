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

#include "NiMemObject.h"
#include "NiMemManager.h"
#include <exception> // for std::bad_alloc
#include<new>


//---------------------------------------------------------------------------
#ifdef NI_MEMORY_DEBUGGER
    void* NiMemObject::operator new(size_t stSize)
    {
        throw std::bad_alloc();
    }

    void* NiMemObject::operator new[](size_t stSize)
    {
        throw std::bad_alloc();
    }

    void* NiMemObject::operator new(
        size_t stSizeInBytes, 
        const char* pcSourceFile, 
        int iSourceLine, 
        const char* pcFunction)
    {
        NIASSERT(NiMemManager::IsInitialized());
        if (stSizeInBytes == 0)
            stSizeInBytes = 1;

        void* p = NiMemManager::Get().Allocate(stSizeInBytes, 
            NI_MEM_ALIGNMENT_DEFAULT, NI_OPER_NEW, true, pcSourceFile, 
            iSourceLine, pcFunction);

        if (p == 0)
            throw std::bad_alloc();

        return p;
    }

    void* NiMemObject::operator new[](
        size_t stSizeInBytes, 
        const char* pcSourceFile, 
        int iSourceLine, 
        const char* pcFunction)
    {
       
        NIASSERT(NiMemManager::IsInitialized());
        if (stSizeInBytes == 0)
            stSizeInBytes = 1;

#ifdef _PS3
        // On GCC the allocated size is passed into operator delete[] 
        // so there is no need for the allocator to save the size of
        // the allocation.  
        void* p =  NiMemManager::Get().Allocate(stSizeInBytes, 
            NI_MEM_ALIGNMENT_DEFAULT, NI_OPER_NEW_ARRAY, true,
            pcSourceFile, iSourceLine, pcFunction);        
#else
        void* p =  NiMemManager::Get().Allocate(stSizeInBytes, 
            NI_MEM_ALIGNMENT_DEFAULT, NI_OPER_NEW_ARRAY, false,
            pcSourceFile, iSourceLine, pcFunction);        
#endif
        
        if (p == 0)
            throw std::bad_alloc();

        return p;
    }
#else
    void* NiMemObject::operator new(size_t stSizeInBytes)
    {
        NIASSERT(NiMemManager::IsInitialized());
        if (stSizeInBytes == 0)
            stSizeInBytes = 1;
        
        void* p =  NiMemManager::Get().Allocate(stSizeInBytes, 
            NI_MEM_ALIGNMENT_DEFAULT, NI_OPER_NEW, true);
        
        if (p == 0)
            throw std::bad_alloc();

        return p;
    }
    void* NiMemObject::operator new[](size_t stSizeInBytes)
    {
        NIASSERT(NiMemManager::IsInitialized());
        if (stSizeInBytes == 0)
            stSizeInBytes = 1;

#ifdef _PS3
        // On GCC the allocated size is passed into operator delete[] 
        // so there is no need for the allocator to save the size of
        // the allocation.
        void* p =  NiMemManager::Get().Allocate(stSizeInBytes, 
            NI_MEM_ALIGNMENT_DEFAULT, NI_OPER_NEW_ARRAY, true);
#else
        void* p =  NiMemManager::Get().Allocate(stSizeInBytes, 
            NI_MEM_ALIGNMENT_DEFAULT, NI_OPER_NEW_ARRAY, false);
#endif
        
        if (p == 0)
            throw std::bad_alloc();

        return p;
    }
#endif
//---------------------------------------------------------------------------
void NiMemObject::operator delete(void* pvMem, size_t stElementSize)
{
    if (pvMem)
    {
        NIASSERT(NiMemManager::IsInitialized());
        NiMemManager::Get().Deallocate(pvMem, NI_OPER_DELETE, 
            stElementSize);
    }
}
//---------------------------------------------------------------------------
void NiMemObject::operator delete[](void* pvMem, size_t stElementSize)
{
    if (pvMem)
    {
        NIASSERT(NiMemManager::IsInitialized());       
#ifdef _PS3
        // On GCC the allocated size is passed into operator delete[] 
        // so there is no need for the allocator to save the size of
        // the allocation.  
        NiMemManager::Get().Deallocate(pvMem, NI_OPER_DELETE_ARRAY, 
            stElementSize);
#else
        NiMemManager::Get().Deallocate(pvMem, NI_OPER_DELETE_ARRAY);
#endif
    } 
}
//---------------------------------------------------------------------------
