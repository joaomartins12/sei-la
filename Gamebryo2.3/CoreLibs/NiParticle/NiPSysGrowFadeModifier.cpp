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

#include "NiPSysGrowFadeModifier.h"
#include "NiParticleSystem.h"

NiImplementRTTI(NiPSysGrowFadeModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysGrowFadeModifier::NiPSysGrowFadeModifier(const char* pcName,
    float fGrowTime, unsigned short usGrowGeneration, float fFadeTime,
    unsigned short usFadeGeneration) : NiPSysModifier(pcName, ORDER_GENERAL),
    m_fGrowTime(fGrowTime), m_usGrowGeneration(usGrowGeneration),
    m_fFadeTime(fFadeTime), m_usFadeGeneration(usFadeGeneration)
{
}
//---------------------------------------------------------------------------
NiPSysGrowFadeModifier::NiPSysGrowFadeModifier() : m_fGrowTime(0.0f),
    m_usGrowGeneration(0), m_fFadeTime(0.0f), m_usFadeGeneration(0)
{
}
//---------------------------------------------------------------------------
void NiPSysGrowFadeModifier::Update(float fTime, NiPSysData* pkData)
{
    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        NiParticleInfo* pkCurrentParticle =
            &pkData->GetParticleInfo()[us];

        float fGrow = 1.0f;
        if (pkCurrentParticle->m_usGeneration == m_usGrowGeneration &&
            pkCurrentParticle->m_fAge < m_fGrowTime &&
            m_fGrowTime != 0)
        {
            fGrow = pkCurrentParticle->m_fAge / m_fGrowTime;
        }

        float fFade = 1.0f;
        float fTimeLeft = pkCurrentParticle->m_fLifeSpan - 
            pkCurrentParticle->m_fAge;
        if (pkCurrentParticle->m_usGeneration == m_usFadeGeneration &&
            fTimeLeft < m_fFadeTime &&
            m_fFadeTime != 0)
        {
            fFade = fTimeLeft / m_fFadeTime;
        }

        float fScale = (fFade > fGrow ? fGrow : fFade);
        if (fScale < ms_fEpsilon)
        {
            fScale = ms_fEpsilon;
        }
        pkData->GetSizes()[us] = fScale;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysGrowFadeModifier);
//---------------------------------------------------------------------------
void NiPSysGrowFadeModifier::CopyMembers(NiPSysGrowFadeModifier* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifier::CopyMembers(pkDest, kCloning);

    pkDest->m_fGrowTime = m_fGrowTime;
    pkDest->m_usGrowGeneration = m_usGrowGeneration;

    pkDest->m_fFadeTime = m_fFadeTime;
    pkDest->m_usFadeGeneration = m_usFadeGeneration;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysGrowFadeModifier);
//---------------------------------------------------------------------------
void NiPSysGrowFadeModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fGrowTime);
    NiStreamLoadBinary(kStream, m_usGrowGeneration);

    NiStreamLoadBinary(kStream, m_fFadeTime);
    NiStreamLoadBinary(kStream, m_usFadeGeneration);
}
//---------------------------------------------------------------------------
void NiPSysGrowFadeModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysGrowFadeModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysGrowFadeModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fGrowTime);
    NiStreamSaveBinary(kStream, m_usGrowGeneration);

    NiStreamSaveBinary(kStream, m_fFadeTime);
    NiStreamSaveBinary(kStream, m_usFadeGeneration);
}
//---------------------------------------------------------------------------
bool NiPSysGrowFadeModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysGrowFadeModifier* pkDest = (NiPSysGrowFadeModifier*) pkObject;

    if (m_fGrowTime != pkDest->m_fGrowTime ||
        m_usGrowGeneration != pkDest->m_usGrowGeneration ||
        m_fFadeTime != pkDest->m_fFadeTime ||
        m_usFadeGeneration != pkDest->m_usFadeGeneration)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysGrowFadeModifier::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysGrowFadeModifier::ms_RTTI
        .GetName()));

    pkStrings->Add(NiGetViewerString("Grow Time", m_fGrowTime));
    pkStrings->Add(NiGetViewerString("Grow Generation", m_usGrowGeneration));
    pkStrings->Add(NiGetViewerString("Fade Time", m_fFadeTime));
    pkStrings->Add(NiGetViewerString("Fade Generation", m_usFadeGeneration));
}
//---------------------------------------------------------------------------
