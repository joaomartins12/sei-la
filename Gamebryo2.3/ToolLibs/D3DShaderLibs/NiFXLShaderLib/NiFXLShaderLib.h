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

#ifndef NIFXLSHADERLIB_H
#define NIFXLSHADERLIB_H

// DLL support
#include "NiFXLShaderLibType.h"
#include <NiD3DShaderLibrary.h>

#if defined(_LIB)
bool NiFXLShaderLib_LoadShaderLibrary(
    NiRenderer* pkRenderer, int iDirectoryCount, char* apcDirectories[], 
    bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary);
#endif  //#if defined(_LIB)

unsigned int NiFXLShaderLib_GetFXLCreationFlags();
void NiFXLShaderLib_SetFXLCreationFlags(unsigned int uiFlags);

#endif  //NIFXLSHADERLIB_H
