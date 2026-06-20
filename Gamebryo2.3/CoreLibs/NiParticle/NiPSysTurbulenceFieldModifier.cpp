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

#include "NiPSysTurbulenceFieldModifier.h"
#include "NiParticleSystem.h"
#include <NiCloningProcess.h>

NiImplementRTTI(NiPSysTurbulenceFieldModifier, NiPSysFieldModifier);
//---------------------------------------------------------------------------
NiPSysTurbulenceFieldModifier::NiPSysTurbulenceFieldModifier(
    const char* pcName, NiAVObject* pkFieldObj, float fMagnitude,
    float fAttenuation, bool bUseMaxDistance, float fMaxDistance,
    float fFrequency) :
    NiPSysFieldModifier(pcName, pkFieldObj, fMagnitude, fAttenuation, 
    bUseMaxDistance, fMaxDistance), m_fFrequency(fFrequency)
{
    m_fLastUpdateTime = -NI_INFINITY;

    // Check for near zero frequency
    if (m_fFrequency < 0.0001f)
    {
        m_fFrequencyUpdateTime = 1.0f / 0.0001f;
    }
    else
    {
        m_fFrequencyUpdateTime = 1.0f / m_fFrequency;
    }
}
//---------------------------------------------------------------------------
NiPSysTurbulenceFieldModifier::NiPSysTurbulenceFieldModifier() :
    NiPSysFieldModifier(),
    m_fFrequency(0.0f)
{
    m_fLastUpdateTime = -NI_INFINITY;

    // Near Zero Frequency
    m_fFrequencyUpdateTime = 1.0f / 0.0001f;

}
//---------------------------------------------------------------------------
void NiPSysTurbulenceFieldModifier::Update(float fTime, NiPSysData* pkData)
{

    // Check for the Next Update Time
    if ((fTime >= m_fLastUpdateTime) &&
        (fTime < (m_fLastUpdateTime + m_fFrequencyUpdateTime)))
        return;

    // Update the last time
    m_fLastUpdateTime = fTime;

    // Check for a zero Magnitude, No Particles, or No Turbulence Object
    if ((m_fMagnitude == 0.0f) || (pkData->GetNumParticles() == 0) ||
        !m_pkFieldObj)
    {
        return;
    }

    // Check for optimized update conditions
    if (m_fAttenuation == 0.0f)
    {
        if (m_bUseMaxDistance)
        {
            UpateNoAttenuation(fTime, pkData);
        }
        else
        {
            UpateNoAttenuationNoDistance(fTime, pkData);
        }

        return;
    }

    // Find the transform from Turbulence Space to Particle Space
    NiTransform kTurbulence = m_pkFieldObj->GetWorldTransform();
    NiTransform kPSys = m_pkTarget->GetWorldTransform();
    NiTransform kInvPSys;
    kPSys.Invert(kInvPSys);
    NiTransform kTurbulenceToPSys = kInvPSys * kTurbulence;


    NiPoint3 kPosition = kTurbulenceToPSys.m_Translate;
    NiPoint3 kParticlePosition;
    NiPoint3 kDeltaV;
    float fDistance;

    //
    //  Magnitude / (1.0 + (Distance * Attenuation))
    //

    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        NiParticleInfo* pkCurrentParticle = &pkData->GetParticleInfo()[us];
        
        kParticlePosition = pkData->GetVertices()[us];
        fDistance = (kParticlePosition - kPosition).Length();
        
        if (!m_bUseMaxDistance || (fDistance <= m_fMaxDistance))
        {
            kDeltaV.x = NiSymmetricRandom();
            kDeltaV.y = NiSymmetricRandom();
            kDeltaV.z = NiSymmetricRandom();

            kDeltaV *= m_fMagnitude / (1.0f + fDistance * m_fAttenuation);
            
            pkCurrentParticle->m_kVelocity += kDeltaV;
        }
    }
}
//---------------------------------------------------------------------------
void NiPSysTurbulenceFieldModifier::UpateNoAttenuationNoDistance(float fTime, 
    NiPSysData* pkData)
{
    NiPoint3 kDeltaV;

    // Check each Particle
    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        // Get the current particle
        NiParticleInfo* pkCurrentParticle = &pkData->GetParticleInfo()[us];

        // Create a random turbulence
        kDeltaV.x = NiSymmetricRandom();
        kDeltaV.y = NiSymmetricRandom();
        kDeltaV.z = NiSymmetricRandom();
        
        // Apply the random turbulence by the magnitude
        pkCurrentParticle->m_kVelocity += kDeltaV * m_fMagnitude;
    }
}
//---------------------------------------------------------------------------
void NiPSysTurbulenceFieldModifier::UpateNoAttenuation(float fTime, 
    NiPSysData* pkData)
{
    // Find the transform from Turbulence Space to Particle Space
    NiTransform kTurbulence = m_pkFieldObj->GetWorldTransform();
    NiTransform kPSys = m_pkTarget->GetWorldTransform();
    NiTransform kInvPSys;
    kPSys.Invert(kInvPSys);
    NiTransform kTurbulenceToPSys = kInvPSys * kTurbulence;


    NiPoint3 kPosition = kTurbulenceToPSys.m_Translate;
    NiPoint3 kParticlePosition;
    NiPoint3 kDeltaV;

    // Check each Particle
    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        // Get the current particle
        NiParticleInfo* pkCurrentParticle = &pkData->GetParticleInfo()[us];
        kParticlePosition = pkData->GetVertices()[us];
        
        if ((kParticlePosition - kPosition).SqrLength() <= m_fMaxDistanceSqr)
        {
            // Create a random turbulence
            kDeltaV.x = NiSymmetricRandom();
            kDeltaV.y = NiSymmetricRandom();
            kDeltaV.z = NiSymmetricRandom();
            
            // Apply the random turbulence by the magnitude
            pkCurrentParticle->m_kVelocity += kDeltaV * m_fMagnitude;
        }
    }
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysTurbulenceFieldModifier);
//---------------------------------------------------------------------------
void NiPSysTurbulenceFieldModifier::CopyMembers(
    NiPSysTurbulenceFieldModifier* pkDest, NiCloningProcess& kCloning)
{
    NiPSysFieldModifier::CopyMembers(pkDest, kCloning);

    pkDest->SetFrequency(m_fFrequency);
}
//---------------------------------------------------------------------------
void NiPSysTurbulenceFieldModifier::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysModifier::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);
    NiPSysTurbulenceFieldModifier* pkDest = 
        (NiPSysTurbulenceFieldModifier*) pkClone;

    if (m_pkFieldObj)
    {
        bCloned = kCloning.m_pkCloneMap->GetAt(m_pkFieldObj, pkClone);
        if (bCloned)
        {
            NIASSERT(m_pkFieldObj->GetRTTI() == pkClone->GetRTTI());
            pkDest->m_pkFieldObj = (NiAVObject*) pkClone;
        }
        else
        {
            pkDest->m_pkFieldObj = m_pkFieldObj;
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysTurbulenceFieldModifier);
//---------------------------------------------------------------------------
void NiPSysTurbulenceFieldModifier::LoadBinary(NiStream& kStream)
{
    NiPSysFieldModifier::LoadBinary(kStream);

    // Load the Frequency
    NiStreamLoadBinary(kStream, m_fFrequency);

    // Set the Frequency Update Time
    // Check for near zero frequency
    if (m_fFrequency < 0.0001f)
    {
        m_fFrequencyUpdateTime = 1.0f / 0.0001f;
    }
    else
    {
        m_fFrequencyUpdateTime = 1.0f / m_fFrequency;
    }
}
//---------------------------------------------------------------------------
void NiPSysTurbulenceFieldModifier::LinkObject(NiStream& kStream)
{
    NiPSysFieldModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysTurbulenceFieldModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysFieldModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysTurbulenceFieldModifier::SaveBinary(NiStream& kStream)
{
    NiPSysFieldModifier::SaveBinary(kStream);

    // Save Spread
    NiStreamSaveBinary(kStream, m_fFrequency);
}
//---------------------------------------------------------------------------
bool NiPSysTurbulenceFieldModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysFieldModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysTurbulenceFieldModifier* pkDest =
        (NiPSysTurbulenceFieldModifier*) pkObject;

    if (pkDest->m_fFrequency != m_fFrequency)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysTurbulenceFieldModifier::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSysFieldModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiPSysTurbulenceFieldModifier::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("Frequency", m_fFrequency));

}
//---------------------------------------------------------------------------
