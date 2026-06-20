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

#include "FLTImportPlugin.h"

#include <NiOpenFlight.h>

//---------------------------------------------------------------------------
NIPLUGINDLL_ENTRY void GetPlugins(NiPluginPtrSet& kPluginArray)
{
    kPluginArray.Add(NiNew FLTImportPlugin);
}
//---------------------------------------------------------------------------
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ulReason, LPVOID lpReserved) 
{ 
    switch (ulReason) 
    { 
        case DLL_PROCESS_ATTACH: 
            NiOpenFlight::Init();
            break; 
        case DLL_PROCESS_DETACH: 
            NiOpenFlight::Shutdown(); 
            break; 
    } 
    return TRUE; 
}
//---------------------------------------------------------------------------
      