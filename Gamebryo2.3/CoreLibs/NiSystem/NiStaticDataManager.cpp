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

#include "NiLog.h"
#include "NiStaticDataManager.h"
#include "NiSystemSDM.h"
#include "NiRTLib.h"
#include "NiInitOptions.h"

unsigned int NiStaticDataManager::ms_uiNumLibraries = 0;
unsigned int NiStaticDataManager::ms_uiNumInitializedLibraries = 0;
bool NiStaticDataManager::ms_bInitialized = false;
NiStaticDataManager::InitFunction
    NiStaticDataManager::ms_pfnRootInitFunction = NULL;
NiStaticDataManager::ShutdownFunction
    NiStaticDataManager::ms_pfnRootShutdownFunction = NULL;
NiStaticDataManager::InitFunction 
    NiStaticDataManager::ms_apfnInitFunctions[NI_NUM_LIBRARIES];
NiStaticDataManager::ShutdownFunction 
    NiStaticDataManager::ms_apfnShutdownFunctions[NI_NUM_LIBRARIES];

const NiInitOptions* NiStaticDataManager::ms_pkInitOptions = NULL;
bool NiStaticDataManager::ms_bAutoCreatedInitOptions = false;

//---------------------------------------------------------------------------
void NiStaticDataManager::Init(const NiInitOptions* pkOptions)
{
    // NiSystem is a required library, so its Init function is called
    // directly instead of being registered.
    if (pkOptions == NULL)
    {
        pkOptions = NiExternalNew NiInitOptions();
        ms_bAutoCreatedInitOptions = true;
    }
    else
    {   
        ms_bAutoCreatedInitOptions = false;
    }
    NIASSERT(pkOptions);
    ms_pkInitOptions = pkOptions;

    NiSystemSDM::Init();
    if (ms_pfnRootInitFunction)
        ms_pfnRootInitFunction();

    ms_bInitialized = true;
    ProcessAccumulatedLibraries();
    
    NiOutputDebugString("NiStaticDataManager Initialized\n");
}
//---------------------------------------------------------------------------
void NiStaticDataManager::Shutdown()
{
    // NiSystem is a required library, so its Shutdown functions is called
    // directly instead of being registered.    
    
    // Shutdown in reverse order of initialization
    for (int i = ms_uiNumLibraries-1; i >= 0 ; i--)
    {
        (*ms_apfnShutdownFunctions[i])();
    }
    ms_uiNumInitializedLibraries = 0;

    if (ms_pfnRootShutdownFunction)
        ms_pfnRootShutdownFunction();
    NiSystemSDM::Shutdown();

    if (ms_bAutoCreatedInitOptions)
        NiExternalDelete ms_pkInitOptions;

    ms_bInitialized = false;

    NiOutputDebugString("NiStaticDataManager Shutdown\n");
}
//---------------------------------------------------------------------------
void NiStaticDataManager::SetRootLibrary(InitFunction pfnInit,
    ShutdownFunction pfnShutdown)
{
    ms_pfnRootInitFunction = pfnInit;
    ms_pfnRootShutdownFunction = pfnShutdown;
}
//---------------------------------------------------------------------------
void NiStaticDataManager::AddLibrary(InitFunction pfnInit, 
    ShutdownFunction pfnShutdown)
{
    NIASSERT(ms_uiNumLibraries < NI_NUM_LIBRARIES); 
    ms_apfnInitFunctions[ms_uiNumLibraries] = pfnInit; 
    ms_apfnShutdownFunctions[ms_uiNumLibraries] = pfnShutdown; 
    ms_uiNumLibraries++; 
}
//---------------------------------------------------------------------------
void NiStaticDataManager::RemoveLibrary(InitFunction pfnInit,
    ShutdownFunction pfnShutdown)
{
    unsigned int uiRemove;
    for (uiRemove = 0; uiRemove < ms_uiNumLibraries; uiRemove++)
    {
        if (ms_apfnInitFunctions[uiRemove] == pfnInit &&
            ms_apfnShutdownFunctions[uiRemove] == pfnShutdown)
        {
            break;
        }
    }

    NIASSERT(uiRemove != ms_uiNumLibraries);

    if (uiRemove < ms_uiNumInitializedLibraries)
    {
        pfnShutdown();
        ms_uiNumInitializedLibraries--;
    }
   
    for (unsigned int ui = uiRemove; ui < ms_uiNumLibraries - 1; ui++)
    {
        ms_apfnInitFunctions[ui] = ms_apfnInitFunctions[ui + 1];
        ms_apfnShutdownFunctions[ui] = ms_apfnShutdownFunctions[ui + 1];
    }

    ms_uiNumLibraries--;
}
//---------------------------------------------------------------------------
void NiStaticDataManager::ProcessAccumulatedLibraries()
{
    if (ms_bInitialized)
    {
        for (unsigned int ui = ms_uiNumInitializedLibraries; 
            ui < ms_uiNumLibraries; ui++)
        {
            (*ms_apfnInitFunctions[ui])();
        }        

        ms_uiNumInitializedLibraries = ms_uiNumLibraries;
    }
}
//---------------------------------------------------------------------------
