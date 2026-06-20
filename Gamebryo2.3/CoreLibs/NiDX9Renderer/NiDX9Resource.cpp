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
#include "NiD3DRendererPCH.h"

#include "NiDX9Resource.h"

//---------------------------------------------------------------------------
NiDX9Resource::NiDX9Resource()
{
    m_uiResID = 0xFFFFFFFF;
    m_eType = TYPE_INVALID;
    m_ePool = D3DPOOL_DEFAULT;
    m_pkD3DResource = NULL;
}
//---------------------------------------------------------------------------
NiDX9Resource::~NiDX9Resource()
{
    //  We are going under the assumption that any specific resource
    //  handling will occur first...
    if (m_pkD3DResource)
    {
        D3D_POINTER_RELEASE(m_pkD3DResource);
        m_pkD3DResource = NULL;
    }
}
//---------------------------------------------------------------------------
