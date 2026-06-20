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
#include "NiFXLShaderLibPCH.h"

#include "NiFXLShaderLib.h"
#include "NiFXLShaderLibrary.h"
#include "NiFXLLoader.h"
#include "NiFXLFactory.h"
#include "NiFXLShaderSDM.h"

#include <NiD3DShaderLibraryInterface.h>
#include <NiShaderAttributeDesc.h>
#include <NiShaderDesc.h>
#include <NiShaderLibraryDesc.h>

//---------------------------------------------------------------------------
#if defined(_USRDLL)
BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved)
{
    OutputDebugString("NiFXLShaderLib> DLLMain CALL - ");

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
            NiStaticDataManager::RemoveLibrary(NiFXLShaderSDM::Init, 
                NiFXLShaderSDM::Shutdown);
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
NIFXLSHADER_ENTRY bool LoadShaderLibrary(NiRenderer* pkRenderer, 
    int iDirectoryCount, char* apcDirectories[], bool bRecurseSubFolders, 
    NiShaderLibrary** ppkLibrary)
{
    NiFXLShaderLibrary* pkLibrary = NiFXLShaderLibrary::Create(
        (NiD3DRenderer*)pkRenderer, iDirectoryCount, apcDirectories, 
        bRecurseSubFolders);

    *ppkLibrary = pkLibrary;
    return (pkLibrary != NULL);
}
//---------------------------------------------------------------------------
NIFXLSHADER_ENTRY unsigned int GetCompilerVersion(void)
{
    return (_MSC_VER);
}
#endif  //#if defined(_USRDLL)
//---------------------------------------------------------------------------
#if defined(_LIB)
bool NiFXLShaderLib_LoadShaderLibrary(NiRenderer* pkRenderer, 
    int iDirectoryCount, char* apcDirectories[], bool bRecurseSubFolders, 
    NiShaderLibrary** ppkLibrary)
{
    NiFXLShaderLibrary* pkLibrary = NiFXLShaderLibrary::Create(
        (NiD3DRenderer*)pkRenderer, iDirectoryCount, apcDirectories, 
        bRecurseSubFolders);

    *ppkLibrary = pkLibrary;
    return (pkLibrary != NULL);
}
#endif  //#if defined(_LIB)
//---------------------------------------------------------------------------
unsigned int NiFXLShaderLib_GetD3DXEffectCreationFlags()
{
    return NiFXLFactory::GetFXLCreationFlags();
}
//---------------------------------------------------------------------------
void NiFXLShaderLib_SetFXLCreationFlags(unsigned int uiFlags)
{
    NiFXLFactory::SetFXLCreationFlags(uiFlags);
}
//---------------------------------------------------------------------------
