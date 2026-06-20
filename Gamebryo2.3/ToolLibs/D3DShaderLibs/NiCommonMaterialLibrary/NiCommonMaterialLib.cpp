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
//---------------------------------------------------------------------------
// Precompiled Header
#include "NiCommonMaterialLibPCH.h"

#include "NiCommonMaterialLib.h"
#include "NiCommonMaterialLibrary.h"
#include "NiCommonMaterialSDM.h"

#if defined(_USRDLL)
#include "NiMaterialLibraryInterface.h"
#endif

#include <NiShaderAttributeDesc.h>
#include <NiShaderDesc.h>
#include <NiShaderLibraryDesc.h>

//---------------------------------------------------------------------------
#if defined(_USRDLL)
BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved)
{
    OutputDebugString("NiCommonMaterialLib> DLLMain CALL - ");

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        {
            //  Initialize anything needed here
            //  If failed, return FALSE
            OutputDebugString("PROCESS ATTACH!\n");
            NiStaticDataManager::ProcessAccumulatedLibraries();
        }
        break;
    case DLL_THREAD_ATTACH:
        {
            OutputDebugString("THREAD ATTACH!\n");
        }
        break;
    case DLL_PROCESS_DETACH:
        {
            //  Shutdown anything needed here
            OutputDebugString("PROCESS DETACH!\n");
            
            NiStaticDataManager::RemoveLibrary(NiCommonMaterialSDM::Init, 
                NiCommonMaterialSDM::Shutdown);
        }
        break;
    case DLL_THREAD_DETACH:
        {
            OutputDebugString("THREAD DETACH!\n");
        }
        break;
    }

    return (TRUE);
}
//---------------------------------------------------------------------------
NICOMMONMATERIAL_ENTRY bool LoadMaterialLibrary(
    NiRenderer* pkRenderer, NiMaterialLibrary*& pkLibrary, 
    unsigned int uiWhichLibrary)
{
    if (uiWhichLibrary == 0)
    {
        pkLibrary = NiCommonMaterialLibrary::Create();
        return pkLibrary != NULL;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
NICOMMONMATERIAL_ENTRY unsigned int GetMaterialLibraryCount()
{
    return 1;
}
//---------------------------------------------------------------------------
NICOMMONMATERIAL_ENTRY unsigned int GetCompilerVersion()
{
     return (_MSC_VER);
}
//---------------------------------------------------------------------------
#endif
