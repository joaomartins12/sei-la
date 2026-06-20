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
#include "NiMainPCH.h"

#include "NiBool.h"
#include "NiFogProperty.h"

NiImplementRTTI(NiFogProperty, NiProperty);

NiFogPropertyPtr NiFogProperty::ms_spDefault;

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiFogProperty);
//---------------------------------------------------------------------------
void NiFogProperty::CopyMembers(NiFogProperty* pkDest,
    NiCloningProcess& kCloning)
{
    NiProperty::CopyMembers(pkDest, kCloning);

    pkDest->m_uFlags = m_uFlags;

    pkDest->m_fDepth = m_fDepth;
    pkDest->m_kColor = m_kColor;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiFogProperty);

//---------------------------------------------------------------------------
void NiFogProperty::LoadBinary(NiStream& kStream)
{
    NiProperty::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);

    // read native type members
    NiStreamLoadBinary(kStream, m_fDepth);
    m_kColor.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiFogProperty::LinkObject(NiStream& kStream)
{
    NiProperty::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiFogProperty::RegisterStreamables(NiStream& kStream)
{
    return NiProperty::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiFogProperty::SaveBinary(NiStream& kStream)
{
    NiProperty::SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_uFlags);
    NiStreamSaveBinary(kStream, m_fDepth);
    m_kColor.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiFogProperty::IsEqual(NiObject* pkObject)
{
    if (!NiProperty::IsEqual(pkObject))
        return false;

    NiFogProperty* pkFog = (NiFogProperty*) pkObject;

    if (GetFog() != pkFog->GetFog() ||
        m_fDepth != pkFog->m_fDepth ||
        GetFogFunction() != pkFog->GetFogFunction() ||
        m_kColor != pkFog->m_kColor)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiFogProperty::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiProperty::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("Enable", GetFog()));
    pkStrings->Add(NiGetViewerString("Depth", m_fDepth));
    pkStrings->Add(GetViewerString("Function", GetFogFunction()));
    pkStrings->Add(m_kColor.GetViewerString("Color"));
}
//---------------------------------------------------------------------------
char* NiFogProperty::GetViewerString(const char* pcPrefix, FogFunction eFunc)
{
    unsigned int uiLen = strlen(pcPrefix) + 20;
    char* pcString = NiAlloc(char, uiLen);

    switch (eFunc)
    {
    case FOG_Z_LINEAR:
        NiSprintf(pcString, uiLen, "%s = FOG_Z_LINEAR", pcPrefix);
        break;
    case FOG_RANGE_SQ:
        NiSprintf(pcString, uiLen, "%s = FOG_RANGE_SQ", pcPrefix);
        break;
    case FOG_VERTEX_ALPHA:
        NiSprintf(pcString, uiLen, "%s = FOG_VERTEX_ALPHA", pcPrefix);
        break;
    }

    return pcString;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiStaticDataManager
//---------------------------------------------------------------------------
void NiFogProperty::_SDMInit()
{
    ms_spDefault = NiNew NiFogProperty;
}
//---------------------------------------------------------------------------
    
void NiFogProperty::_SDMShutdown()
{
    NIASSERT(ms_spDefault->GetRefCount() == 1);
    ms_spDefault = NULL;
}
//---------------------------------------------------------------------------
