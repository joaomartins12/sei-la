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
#include "NiD3D10BinaryShaderLibPCH.h"

#include "NiD3D10BinaryShaderLib.h"
#include "NiD3D10BinaryShaderLibLibType.h"
#include <NiSystem.h>

//---------------------------------------------------------------------------
#if defined(_USRDLL)
BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved)
{
    OutputDebugString("NiD3D10BinaryShaderLib> DLLMain CALL - ");

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        {
            //  Initialize anything needed here
            //  If failed, return FALSE
            OutputDebugString("PROCESS ATTACH!\n");
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
#endif  //#if defined(_USRDLL)
//---------------------------------------------------------------------------
