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

#include "NiPSysVortexFieldModifier.h"
#include "NiParticleSystem.h"
#include <NiCloningProcess.h>

NiImplementRTTI(NiPSysVortexFieldModifier, NiPSysFieldModifier);

//---------------------------------------------------------------------------
NiPSysVortexFieldModifier::NiPSysVortexFieldModifier(const char* pcName, 
    NiAVObject* pkFieldObj, float fMagnitude, float fAttenuation,
    bool bUseMaxDistance, float fMaxDistance, NiPoint3 kDirection) : 
    NiPSysFieldModifier(pcName, pkFieldObj, fMagnitude, fAttenuation, 
    bUseMaxDistance, fMaxDistance), m_kDirection(kDirection)
{
}
//---------------------------------------------------------------------------
NiPSysVortexFieldModifier::NiPSysVortexFieldModifier() :
    NiPSysFieldModifier(),
    m_kDirection(NiPoint3::ZERO)
{
}
//---------------------------------------------------------------------------
void NiPSysVortexFieldModifier::Update(float fTime, NiPSysData* pkData)
{

    // Check for a zero Magnitude, No Particles, or No Vortex Object
    if ((m_fMagnitude == 0.0f) || (pkData->GetNumParticles() == 0) ||
        (m_kDirection == NiPoint3::ZERO) || !m_pkFieldObj)
    {
        return;
    }

    // Check for optimized update path
    if (m_fAttenuation == 0.0f)
    {
        if(m_bUseMaxDistance)
            UpdateNoAttenuation(fTime, pkData);
        else
            UpdateNoAttenuationNoDist(fTime, pkData);
        return;
    }


    // Find the transform from Vortex Space to Particle Space
    NiTransform kVortex = m_pkFieldObj->GetWorldTransform();
    NiTransform kPSys = m_pkTarget->GetWorldTransform();
    NiTransform kInvPSys;
    kPSys.Invert(kInvPSys);
    NiTransform kVortexToPSys = kInvPSys * kVortex;


    NiPoint3 kPosition = kVortexToPSys.m_Translate;

    // Compute the Vortex direction
    NiPoint3 kDirection = kVortexToPSys.m_Rotate * m_kDirection;
    kDirection.Unitize();


    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        NiParticleInfo* pkCurrentParticle = &pkData
            ->GetParticleInfo()[us];
        float fDeltaT = (fTime - pkCurrentParticle->m_fLastUpdate);

        
        if (fDeltaT != 0.0f)
        {
            NiPoint3 kParticlePosition = pkData->GetVertices()[us];
            float fDistance = (kParticlePosition - kPosition).Length();
        
            if ((fDistance != 0.0f) &&
               (!m_bUseMaxDistance || (fDistance <= m_fMaxDistance)))
            {
                NiPoint3 kDeltaV = VortexDirection(kPosition, kDirection, 
                    kParticlePosition);

                if (m_fAttenuation == 0.0f)
                {
                    kDeltaV *= m_fMagnitude * fDeltaT;
                }
                else
                {
                    kDeltaV *= m_fMagnitude * fDeltaT / 
                        NiPow(fDistance, m_fAttenuation);
                }

                pkCurrentParticle->m_kVelocity += kDeltaV;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPSysVortexFieldModifier::UpdateNoAttenuation(float fTime, 
    NiPSysData* pkData)
{
    // Find the transform from Vortex Space to Particle Space
    NiTransform kVortex = m_pkFieldObj->GetWorldTransform();
    NiTransform kPSys = m_pkTarget->GetWorldTransform();
    NiTransform kInvPSys;
    kPSys.Invert(kInvPSys);
    NiTransform kVortexToPSys = kInvPSys * kVortex;


    NiPoint3 kPosition = kVortexToPSys.m_Translate;

    // Compute the Vortex direction
    NiPoint3 kDirection = kVortexToPSys.m_Rotate * m_kDirection;
    kDirection.Unitize();

    float fMaxDistanceSqr = m_fMaxDistance * m_fMaxDistance;

    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        NiParticleInfo* pkCurrentParticle = &pkData
            ->GetParticleInfo()[us];
        float fDeltaT = (fTime - pkCurrentParticle->m_fLastUpdate);

        
        if (fDeltaT != 0.0f)
        {
            NiPoint3 kParticlePosition = pkData->GetVertices()[us];
            float fDistanceSqr = (kParticlePosition - kPosition).Length();
        
            if ((fDistanceSqr != 0.0f) &&
               (fDistanceSqr <= fMaxDistanceSqr))
            {
                NiPoint3 kDeltaV = VortexDirection(kPosition, kDirection, 
                    kParticlePosition);

                kDeltaV *= m_fMagnitude * fDeltaT;

                pkCurrentParticle->m_kVelocity += kDeltaV;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPSysVortexFieldModifier::UpdateNoAttenuationNoDist(float fTime, 
    NiPSysData* pkData)
{
    // Find the transform from Vortex Space to Particle Space
    NiTransform kVortex = m_pkFieldObj->GetWorldTransform();
    NiTransform kPSys = m_pkTarget->GetWorldTransform();
    NiTransform kInvPSys;
    kPSys.Invert(kInvPSys);
    NiTransform kVortexToPSys = kInvPSys * kVortex;


    NiPoint3 kPosition = kVortexToPSys.m_Translate;

    // Compute the Vortex direction
    NiPoint3 kDirection = kVortexToPSys.m_Rotate * m_kDirection;
    kDirection.Unitize();


    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        NiParticleInfo* pkCurrentParticle = &pkData
            ->GetParticleInfo()[us];
        float fDeltaT = (fTime - pkCurrentParticle->m_fLastUpdate);

        
        if (fDeltaT != 0.0f)
        {
            NiPoint3 kParticlePosition = pkData->GetVertices()[us];

            NiPoint3 kDeltaV = VortexDirection(kPosition, kDirection, 
                kParticlePosition);

            kDeltaV *= m_fMagnitude * fDeltaT;

            pkCurrentParticle->m_kVelocity += kDeltaV;
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysVortexFieldModifier);
//---------------------------------------------------------------------------
void NiPSysVortexFieldModifier::CopyMembers(
    NiPSysVortexFieldModifier* pkDest, NiCloningProcess& kCloning)
{
    NiPSysFieldModifier::CopyMembers(pkDest, kCloning);

    pkDest->m_kDirection = m_kDirection;
}
//---------------------------------------------------------------------------
void NiPSysVortexFieldModifier::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysModifier::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);
    NiPSysVortexFieldModifier* pkDest = (NiPSysVortexFieldModifier*) pkClone;

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
NiImplementCreateObject(NiPSysVortexFieldModifier);
//---------------------------------------------------------------------------
void NiPSysVortexFieldModifier::LoadBinary(NiStream& kStream)
{
    NiPSysFieldModifier::LoadBinary(kStream);

    // Load Direction
    m_kDirection.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysVortexFieldModifier::LinkObject(NiStream& kStream)
{
    NiPSysFieldModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysVortexFieldModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysFieldModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysVortexFieldModifier::SaveBinary(NiStream& kStream)
{
    NiPSysFieldModifier::SaveBinary(kStream);

    // Save Direction
    m_kDirection.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysVortexFieldModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysFieldModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysVortexFieldModifier* pkDest = (NiPSysVortexFieldModifier*) pkObject;

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
void NiPSysVortexFieldModifier::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiPSysFieldModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiPSysVortexFieldModifier::ms_RTTI.GetName()));

    pkStrings->Add(m_kDirection.GetViewerString("Direction"));
}
//---------------------------------------------------------------------------
