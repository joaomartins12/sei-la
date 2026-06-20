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

#include "NiPSysGravityFieldModifier.h"
#include "NiParticleSystem.h"
#include <NiCloningProcess.h>

NiImplementRTTI(NiPSysGravityFieldModifier, NiPSysFieldModifier);

//---------------------------------------------------------------------------
NiPSysGravityFieldModifier::NiPSysGravityFieldModifier(const char* pcName, 
    NiAVObject* pkFieldObj, float fMagnitude, float fAttenuation,
    bool bUseMaxDistance, float fMaxDistance, NiPoint3 kDirection) : 
    NiPSysFieldModifier(pcName, pkFieldObj, fMagnitude, fAttenuation, 
    bUseMaxDistance, fMaxDistance)
{
    SetDirection(kDirection);
}
//---------------------------------------------------------------------------
NiPSysGravityFieldModifier::NiPSysGravityFieldModifier() :
    NiPSysFieldModifier()
{
    SetDirection(-NiPoint3::UNIT_Y);
}
//---------------------------------------------------------------------------
void NiPSysGravityFieldModifier::Update(float fTime, NiPSysData* pkData)
{

    // Check for a zero Magnitude, No Particles, or No Gravity Object
    if ((m_fMagnitude == 0.0f) || (pkData->GetNumParticles() == 0) ||
        !m_pkFieldObj)
    {
        return;
    }

    // Check for optimized update path
    if (!m_bUseMaxDistance && (m_fAttenuation == 0.0f) )
    {
        UpdateNoAttenuationNoDist(fTime, pkData);
        return;
    }


    // Find the transform from Gravity Space to Particle Space
    NiTransform kGravity = m_pkFieldObj->GetWorldTransform();
    NiTransform kPSys = m_pkTarget->GetWorldTransform();
    NiTransform kInvPSys;
    kPSys.Invert(kInvPSys);
    NiTransform kGravityToPSys = kInvPSys * kGravity;


    NiPoint3 kPosition = kGravityToPSys.m_Translate;
    float fHalfAttenuation = m_fAttenuation * 0.5f;


    // Compute the Gravity direction
    // If we are using direction kDirection will be constant 
    NiPoint3 kDirection = kGravityToPSys.m_Rotate * m_kUnitDirection;

    //
    //  kDirection * (m_fMagnitude * fDeltaT / NiPow(fDistance,
    //      m_fAttenuation));
    //

    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        NiParticleInfo* pkCurrentParticle = &pkData
            ->GetParticleInfo()[us];
        float fDeltaT = (fTime - pkCurrentParticle->m_fLastUpdate);


        if (fDeltaT != 0.0f)
        {
            NiPoint3 kParticlePosition = pkData->GetVertices()[us];
            float fDistanceSqr = (kParticlePosition - kPosition).SqrLength();

            if (!m_bUseMaxDistance || (fDistanceSqr <= m_fMaxDistanceSqr))
            {
                NiPoint3 kDeltaV;

                if ((m_fAttenuation == 0.0f) || (fDistanceSqr == 0.0f))
                {
                    kDeltaV = kDirection * (m_fMagnitude * fDeltaT);
                }
                else
                {
                    kDeltaV = kDirection * (m_fMagnitude * fDeltaT / 
                        NiMax(1.0f, NiPow(fDistanceSqr, fHalfAttenuation)));
                }

                pkCurrentParticle->m_kVelocity += kDeltaV;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPSysGravityFieldModifier::UpdateNoAttenuationNoDist(float fTime, 
    NiPSysData* pkData)
{
    // Find the transform from Gravity Space to Particle Space
    NiTransform kGravity = m_pkFieldObj->GetWorldTransform();
    NiTransform kPSys = m_pkTarget->GetWorldTransform();
    NiTransform kInvPSys;
    kPSys.Invert(kInvPSys);
    NiTransform kGravityToPSys = kInvPSys * kGravity;

    NiPoint3 kDirection = kGravityToPSys.m_Rotate * m_kDirection;
    kDirection.Unitize();

    // This is the general algorithm but because we know some values
    // The algorithm will simplify greatly.
    //
    //  kDirection * (m_fMagnitude * fDeltaT / NiPow(fDistance,
    //      m_fAttenuation));
    //

    // Premultipy the direction by the magnitude because it is 
    // constant for all particles
    kDirection *= m_fMagnitude;

    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        NiParticleInfo* pkCurrentParticle = &pkData
            ->GetParticleInfo()[us];
        float fDeltaT = (fTime - pkCurrentParticle->m_fLastUpdate);

        
        if (fDeltaT != 0.0f)
        {
            pkCurrentParticle->m_kVelocity += kDirection * fDeltaT;
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysGravityFieldModifier);
//---------------------------------------------------------------------------
void NiPSysGravityFieldModifier::CopyMembers(
    NiPSysGravityFieldModifier* pkDest, NiCloningProcess& kCloning)
{
    NiPSysFieldModifier::CopyMembers(pkDest, kCloning);

    pkDest->SetDirection(m_kDirection);
}
//---------------------------------------------------------------------------
void NiPSysGravityFieldModifier::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysModifier::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);
    NiPSysGravityFieldModifier* pkDest =
        (NiPSysGravityFieldModifier*) pkClone;

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
NiImplementCreateObject(NiPSysGravityFieldModifier);
//---------------------------------------------------------------------------
void NiPSysGravityFieldModifier::LoadBinary(NiStream& kStream)
{
    NiPSysFieldModifier::LoadBinary(kStream);

    // Load Direction
    m_kDirection.LoadBinary(kStream);

    // Set Internal Variables
    SetDirection(m_kDirection);
}
//---------------------------------------------------------------------------
void NiPSysGravityFieldModifier::LinkObject(NiStream& kStream)
{
    NiPSysFieldModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysGravityFieldModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysFieldModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysGravityFieldModifier::SaveBinary(NiStream& kStream)
{
    NiPSysFieldModifier::SaveBinary(kStream);

    // Save Direction
    m_kDirection.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysGravityFieldModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysFieldModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysGravityFieldModifier* pkDest =
        (NiPSysGravityFieldModifier*) pkObject;

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
void NiPSysGravityFieldModifier::GetViewerStrings(NiViewerStringsArray*
    pkStrings)
{
    NiPSysFieldModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiPSysGravityFieldModifier::ms_RTTI.GetName()));

    pkStrings->Add(m_kDirection.GetViewerString("Direction"));
}
//---------------------------------------------------------------------------
