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

#ifndef NIFXLINCLUDE_H
#define NIFXLINCLUDE_H

#include "NiFXLShaderLibType.h"

class NIFXLSHADER_ENTRY NiFXLInclude : public ID3DXInclude
{
public:
    NiFXLInclude(const char* pcBasePath);

    HRESULT Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName,
        LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes, LPSTR pFullPath,
        DWORD cbFullPath);
    HRESULT Close(LPCVOID pData);

protected:
    char m_acBasePath[NI_MAX_PATH];
};

#endif  // #ifndef NIFXLINCLUDE_H
