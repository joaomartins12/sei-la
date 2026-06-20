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

#include "NiPSysAirFieldModifier.h"
#include "NiParticleSystem.h"
#include <NiCloningProcess.h>

NiImplementRTTI(NiPSysAirFieldModifier, NiPSysFieldModifier);

NiPoint3 NiPSysAirFieldModifier::m_ckUninitializePosition(FLT_MAX, FLT_MAX,
    FLT_MAX);

//---------------------------------------------------------------------------
NiPSysAirFieldModifier::NiPSysAirFieldModifier(const char* pcName, 
    NiAVObject* pkFieldObj, float fMagnitude, float fAttenuation,
    bool bUseMaxDistance, float fMaxDistance, NiPoint3 kDirection, 
    float fAirFriction, float fInheritVelocity, bool bInheritRotation, 
    bool bComponentOnly, bool bEnableSpread, float fSpread) : 
    NiPSysFieldModifier(pcName, pkFieldObj, fMagnitude, fAttenuation, 
    bUseMaxDistance, fMaxDistance), m_bInheritRotation(bInheritRotation), 
    m_bComponentOnly(bComponentOnly), m_bEnableSpread(bEnableSpread)
{
    SetDirection(kDirection);
    SetAirFriction(fAirFriction);
    SetInheritVelocity(fInheritVelocity);
    SetSpread(fSpread);

    m_kLastFieldPosition = m_ckUninitializePosition;
    m_fLastUpdateTime = FLT_MAX;
}
//---------------------------------------------------------------------------
NiPSysAirFieldModifier::NiPSysAirFieldModifier() : NiPSysFieldModifier()
{
    SetDirection(-NiPoint3::UNIT_X);
    m_kLastFieldPosition = m_ckUninitializePosition;
    m_fLastUpdateTime = FLT_MAX;
}
//---------------------------------------------------------------------------
void NiPSysAirFieldModifier::Update(float fTime, NiPSysData* pkData)
{

    // Check for No Particles, or No Air Object
    if ((pkData->GetNumParticles() == 0) || !m_pkFieldObj)
    {
        return;
    }

    // Check for Fast Path Solutions
    if ((m_fAttenuation == 0.0f) && (m_fInheritVelocity == 0.0f) &&
        !m_bEnableSpread)
    {
        if (m_bUseMaxDistance)
            UpdateNoAttenuationNoInheritNoSpread(fTime, pkData);
        else
            UpdateNoAttenuationNoInheritNoSpreadNoMaxDistance(fTime, pkData);
        return;
    }


    // Find the transform from Air Space to Particle Space
    NiTransform kAir = m_pkFieldObj->GetWorldTransform();
    NiTransform kPSys = m_pkTarget->GetWorldTransform();
    NiTransform kInvPSys;
    kPSys.Invert(kInvPSys);
    NiTransform kAirToPSys = kInvPSys * kAir;


    NiPoint3 kPosition = kAirToPSys.m_Translate;


    // Compute the Air direction
    NiPoint3 kDirection;
    if (m_bInheritRotation)
        kDirection = kAirToPSys.m_Rotate * m_kUnitDirection;
    else
        kDirection = m_kUnitDirection;

    // Compute the Delta Air Position
    NiPoint3 kAirFieldVelocity = NiPoint3::ZERO;
    if ((m_kLastFieldPosition != m_ckUninitializePosition) &&
        (m_fLastUpdateTime != FLT_MAX) &&
        (m_fLastUpdateTime != fTime))
    {
        kAirFieldVelocity = (kPosition - m_kLastFieldPosition) *
            m_fInheritVelocity / (fTime - m_fLastUpdateTime);
    }

    // Update the Last Field Position
    m_kLastFieldPosition = kPosition;
    m_fLastUpdateTime = fTime;
        
    // Apply the Magnitude to the Air Velocity
    kAirFieldVelocity += kDirection * m_fMagnitude;

    // Check for No air
    if (kAirFieldVelocity.SqrLength() == 0.0f)
        return;

    NiPoint3 kUnitAirFieldVelocity = kAirFieldVelocity;
    float fAirFieldVelocityLength = kUnitAirFieldVelocity.Unitize();

    // Create the CosSpread if we are using spread
    const float fCosSpread =
        m_bEnableSpread ? NiCos(NI_HALF_PI * m_fSpread) : 0.0f;


    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        NiParticleInfo* pkCurrentParticle = &pkData
            ->GetParticleInfo()[us];
        float fDeltaT = (fTime - pkCurrentParticle->m_fLastUpdate);


        if (fDeltaT != 0.0f)
        {
            NiPoint3 kParticlePosition = pkData->GetVertices()[us];
            NiPoint3 kParticleOffset = kParticlePosition - kPosition;
            float fDistanceSqr = kParticleOffset.SqrLength();

            if (!m_bUseMaxDistance || (fDistanceSqr <= m_fMaxDistanceSqr))
            {
                NiPoint3 kAirVelocity = kAirFieldVelocity;

                // Apply Spread
                if (m_bEnableSpread )
                {
                    kAirVelocity = kParticleOffset;
                    kAirVelocity.Unitize();

                    // if outside the cone they don't affect this particle
                    if (kUnitAirFieldVelocity.Dot(kAirVelocity) < fCosSpread)
                        continue;

                    kAirVelocity *= fAirFieldVelocityLength;
                }

                NiPoint3 kPartVel = pkCurrentParticle->m_kVelocity;

                // Don't go faster then the air
                if (kAirVelocity.Dot(kPartVel - kAirVelocity) < 0.0f)
                {
                    float fAttenuationWithTime = fDeltaT;

                    // Apply Attenuation
                    if (m_bUseMaxDistance && (m_fAttenuation != 0.0f))
                    {
                        fAttenuationWithTime *= NiPow(1.0f -
                            (NiSqrt(fDistanceSqr)/m_fMaxDistance),
                            m_fAttenuation);
                    }

                    // Apply Air to the particle velocity
                    kPartVel += kAirVelocity * m_fAirFriction *
                        fAttenuationWithTime;

                    // Apply the computed velocity
                    pkCurrentParticle->m_kVelocity = kPartVel;
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPSysAirFieldModifier::UpdateNoAttenuationNoInheritNoSpread(
    float fTime, NiPSysData* pkData)
{
    // Check for no air flow
    if (m_fMagnitude == 0.0f)
        return;

    // Find the transform from Air Space to Particle Space
    NiTransform kAir = m_pkFieldObj->GetWorldTransform();
    NiTransform kPSys = m_pkTarget->GetWorldTransform();
    NiTransform kInvPSys;
    kPSys.Invert(kInvPSys);
    NiTransform kAirToPSys = kInvPSys * kAir;

    NiPoint3 kPosition = kAirToPSys.m_Translate;

    // Update the Last Field Position
    m_kLastFieldPosition = kPosition;
    m_fLastUpdateTime = fTime;


    // Compute the Air direction
    NiPoint3 kDirection;
    if (m_bInheritRotation)
        kDirection = kAirToPSys.m_Rotate * m_kUnitDirection;
    else
        kDirection = m_kUnitDirection;

    NiPoint3 kAirFieldVelocity = kDirection * m_fMagnitude;

    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        NiParticleInfo* pkCurrentParticle = &pkData
            ->GetParticleInfo()[us];
        float fDeltaT = (fTime - pkCurrentParticle->m_fLastUpdate);


        if (fDeltaT != 0.0f)
        {
            NiPoint3 kParticlePosition = pkData->GetVertices()[us];
            NiPoint3 kParticleOffset = kParticlePosition - kPosition;

            // Check for Max Distance
            if (kParticleOffset.SqrLength() <= m_fMaxDistanceSqr)
            {
                // Don't go faster then the air
                if (kAirFieldVelocity.Dot(pkCurrentParticle->m_kVelocity - 
                    kAirFieldVelocity) < 0.0f)
                {
                    // Apply Air to the particle velocity
                    pkCurrentParticle->m_kVelocity += kAirFieldVelocity * 
                        m_fAirFriction * fDeltaT;
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void
NiPSysAirFieldModifier::UpdateNoAttenuationNoInheritNoSpreadNoMaxDistance(
    float fTime, NiPSysData* pkData)
{
    // Check for no air flow
    if (m_fMagnitude == 0.0f)
        return;

    // Find the transform from Air Space to Particle Space
    NiTransform kAir = m_pkFieldObj->GetWorldTransform();
    NiTransform kPSys = m_pkTarget->GetWorldTransform();
    NiTransform kInvPSys;
    kPSys.Invert(kInvPSys);
    NiTransform kAirToPSys = kInvPSys * kAir;

    NiPoint3 kPosition = kAirToPSys.m_Translate;

    // Update the Last Field Position
    m_kLastFieldPosition = kPosition;
    m_fLastUpdateTime = fTime;


    // Compute the Air direction
    NiPoint3 kDirection;
    if (m_bInheritRotation)
        kDirection = kAirToPSys.m_Rotate * m_kUnitDirection;
    else
        kDirection = m_kUnitDirection;

    NiPoint3 kAirFieldVelocity = kDirection * m_fMagnitude;

    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        NiParticleInfo* pkCurrentParticle = &pkData
            ->GetParticleInfo()[us];
        float fDeltaT = (fTime - pkCurrentParticle->m_fLastUpdate);


        // Don't go faster then the air
        if ((fDeltaT != 0.0f) &&
            (kAirFieldVelocity.Dot(pkCurrentParticle->m_kVelocity - 
             kAirFieldVelocity) < 0.0f))
        {
            // Apply Air to the particle velocity
            pkCurrentParticle->m_kVelocity += kAirFieldVelocity * 
                m_fAirFriction * fDeltaT;
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysAirFieldModifier);
//---------------------------------------------------------------------------
void NiPSysAirFieldModifier::CopyMembers(NiPSysAirFieldModifier* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysFieldModifier::CopyMembers(pkDest, kCloning);

    pkDest->SetDirection(m_kDirection);
}
//---------------------------------------------------------------------------
void NiPSysAirFieldModifier::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysModifier::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);
    NiPSysAirFieldModifier* pkDest = (NiPSysAirFieldModifier*) pkClone;

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
NiImplementCreateObject(NiPSysAirFieldModifier);
//---------------------------------------------------------------------------
void NiPSysAirFieldModifier::LoadBinary(NiStream& kStream)
{
    NiPSysFieldModifier::LoadBinary(kStream);

    // Load Direction
    m_kDirection.LoadBinary(kStream);

    // Set Internal Variables
    SetDirection(m_kDirection);

    NiStreamLoadBinary(kStream, m_fAirFriction);

    NiStreamLoadBinary(kStream, m_fInheritVelocity);

    NiBool kBool;
    NiStreamLoadBinary(kStream, kBool);
    m_bInheritRotation = kBool ? true : false;

    NiStreamLoadBinary(kStream, kBool);
    m_bComponentOnly = kBool ? true : false;

    NiStreamLoadBinary(kStream, kBool);
    m_bEnableSpread = kBool ? true : false;

    NiStreamLoadBinary(kStream, m_fSpread);
}
//---------------------------------------------------------------------------
void NiPSysAirFieldModifier::LinkObject(NiStream& kStream)
{
    NiPSysFieldModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysAirFieldModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysFieldModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysAirFieldModifier::SaveBinary(NiStream& kStream)
{
    NiPSysFieldModifier::SaveBinary(kStream);

    // Save Direction
    m_kDirection.SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fAirFriction);

    NiStreamSaveBinary(kStream, m_fInheritVelocity);

    NiBool kBool = m_bInheritRotation;
    NiStreamSaveBinary(kStream, kBool);

    kBool = m_bComponentOnly;
    NiStreamSaveBinary(kStream, kBool);

    kBool = m_bEnableSpread;
    NiStreamSaveBinary(kStream, kBool);

    NiStreamSaveBinary(kStream, m_fSpread);
}
//---------------------------------------------------------------------------
bool NiPSysAirFieldModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysFieldModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysAirFieldModifier* pkDest = (NiPSysAirFieldModifier*) pkObject;

    if (pkDest->m_kDirection != m_kDirection)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysAirFieldModifier::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysFieldModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiPSysAirFieldModifier::ms_RTTI.GetName()));

    pkStrings->Add(m_kDirection.GetViewerString("Direction"));
    pkStrings->Add(m_kUnitDirection.GetViewerString("Unit Direction"));
    pkStrings->Add(NiGetViewerString("AirFriction", m_fAirFriction));
    pkStrings->Add(NiGetViewerString("Inherit Velocity", m_fInheritVelocity));
    pkStrings->Add(NiGetViewerString("Inherit Rotation", m_bInheritRotation));
    pkStrings->Add(NiGetViewerString("Component Only", m_bComponentOnly));
    pkStrings->Add(NiGetViewerString("Enable Spread", m_bEnableSpread));
    pkStrings->Add(NiGetViewerString("Spread", m_fSpread));
}
//---------------------------------------------------------------------------
