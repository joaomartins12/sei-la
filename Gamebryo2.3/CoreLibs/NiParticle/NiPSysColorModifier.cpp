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
#include "NiParticlePCH.h"

#include "NiPSysColorModifier.h"
#include "NiPSysData.h"

NiImplementRTTI(NiPSysColorModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysColorModifier::NiPSysColorModifier(const char* pcName,
    NiColorData* pkColorData) : NiPSysModifier(pcName, ORDER_GENERAL),
    m_fLoKeyTime(0.0f), m_fHiKeyTime(0.0f)
{
    SetColorData(pkColorData);
}
//---------------------------------------------------------------------------
NiPSysColorModifier::NiPSysColorModifier() : m_spColorData(NULL),
    m_fLoKeyTime(0.0f), m_fHiKeyTime(0.0f)
{
}
//---------------------------------------------------------------------------
void NiPSysColorModifier::Update(float fTime, NiPSysData* pkData)
{
    NiParticleInfo* pkCurrentParticle = pkData->GetParticleInfo();
    NiColorA* pkColor = pkData->GetColors();
    if (!pkColor)
        return;

    unsigned short usNumParticles = pkData->GetNumParticles();
    float fScaledAge;
    unsigned int uiNumKeys, uiLastIdx = 0;
    NiAnimationKey::KeyType eType;
    unsigned char ucSize;
    NiColorKey* pkKeys = m_spColorData->GetAnim(uiNumKeys,
        eType, ucSize);


    for (unsigned short us = 0; us < usNumParticles;
        us++, pkCurrentParticle++)
    {
        fScaledAge = pkCurrentParticle->m_fAge / 
            pkCurrentParticle->m_fLifeSpan;

        // clamp within keyframe range
        fScaledAge = NiMax(fScaledAge, m_fLoKeyTime);
        fScaledAge = NiMin(fScaledAge, m_fHiKeyTime);
        
        uiLastIdx = 0;
        NiColorA kColor = NiColorKey::GenInterp(fScaledAge, pkKeys, eType,
            uiNumKeys, uiLastIdx, ucSize);

        pkColor[us] = kColor;
    }

}
//---------------------------------------------------------------------------
void NiPSysColorModifier::ResetKeyTimeExtrema()
{
    m_fLoKeyTime = 0.0f;
    m_fHiKeyTime = 0.0f;

    if (m_spColorData)
    {
        unsigned int uiNumKeys;
        NiAnimationKey::KeyType eType;
        unsigned char ucSize;
        NiColorKey* pkKeys = m_spColorData->GetAnim(uiNumKeys, eType, 
            ucSize);

        if (uiNumKeys)
        {
            m_fLoKeyTime = pkKeys->GetKeyAt(0, ucSize)->GetTime();
            m_fHiKeyTime = pkKeys->GetKeyAt(uiNumKeys-1, ucSize)->GetTime();
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysColorModifier);
//---------------------------------------------------------------------------
void NiPSysColorModifier::CopyMembers(NiPSysColorModifier* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifier::CopyMembers(pkDest, kCloning);

    pkDest->SetColorData(m_spColorData);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysColorModifier);
//---------------------------------------------------------------------------
void NiPSysColorModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_spColorData
}
//---------------------------------------------------------------------------
void NiPSysColorModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);

    SetColorData((NiColorData*) kStream.GetObjectFromLinkID());
}
//---------------------------------------------------------------------------
bool NiPSysColorModifier::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSysModifier::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spColorData)
    {
        m_spColorData->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPSysColorModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    kStream.SaveLinkID(m_spColorData);
}
//---------------------------------------------------------------------------
bool NiPSysColorModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysColorModifier* pkDest = (NiPSysColorModifier*) pkObject;

    if (!m_spColorData->IsEqual(pkDest->m_spColorData))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysColorModifier::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysColorModifier::ms_RTTI.GetName()));
    m_spColorData->GetViewerStrings(pkStrings);
}
//---------------------------------------------------------------------------
