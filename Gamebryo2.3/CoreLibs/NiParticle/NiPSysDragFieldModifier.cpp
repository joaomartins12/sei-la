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

#include "NiPSysDragFieldModifier.h"
#include "NiParticleSystem.h"
#include <NiCloningProcess.h>

NiImplementRTTI(NiPSysDragFieldModifier, NiPSysFieldModifier);
//---------------------------------------------------------------------------
NiPSysDragFieldModifier::NiPSysDragFieldModifier(const char* pcName, 
    NiAVObject* pkFieldObj, float fMagnitude, float fAttenuation,
    bool bUseMaxDistance, float fMaxDistance, bool bUseDirection, 
    NiPoint3 kDirection) : 
    NiPSysFieldModifier(pcName, pkFieldObj, fMagnitude, fAttenuation, 
    bUseMaxDistance, fMaxDistance), m_bUseDirection(bUseDirection), 
    m_kDirection(kDirection)
{
}
//---------------------------------------------------------------------------
NiPSysDragFieldModifier::NiPSysDragFieldModifier() : NiPSysFieldModifier(),
    m_bUseDirection(false), m_kDirection(NiPoint3::ZERO)
{
}
//---------------------------------------------------------------------------
void NiPSysDragFieldModifier::Update(float fTime, NiPSysData* pkData)
{

    // Check for a zero Magnitude, No Particles, or No Drag Object
    if ((m_fMagnitude == 0.0f) || (pkData->GetNumParticles() == 0) ||
        !m_pkFieldObj)
    {
        return;
    }


    // Check for optimized update path
    if (!m_bUseDirection && (m_fAttenuation == 0.0f))
    {
        if (m_bUseMaxDistance)
        {
            UpdateNoAttenuationNoDir(fTime, pkData);
        }
        else
        {
            UpdateNoAttenuationNoDirNoDist(fTime, pkData);
        }
        return;
    }


    // Find the transform from Drag Space to Particle Space
    NiTransform kDrag = m_pkFieldObj->GetWorldTransform();
    NiTransform kPSys = m_pkTarget->GetWorldTransform();
    NiTransform kInvPSys;
    kPSys.Invert(kInvPSys);
    NiTransform kDragToPSys = kInvPSys * kDrag;


    NiPoint3 kPosition = kDragToPSys.m_Translate;
    NiPoint3 kDragAxis;

    // Compute the drag direction
    if (m_bUseDirection && (m_kDirection != NiPoint3::ZERO))
    {
        kDragAxis = kDragToPSys.m_Rotate * m_kDirection;
        kDragAxis.Unitize();
    }

    //
    //  (Magnitude * Delta Time) / (1.0 + (Distance * Attenuation))
    //

    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        NiParticleInfo* pkCurrentParticle = &pkData
            ->GetParticleInfo()[us];
        float fDeltaT = (fTime - pkCurrentParticle->m_fLastUpdate);

        
        if (fDeltaT != 0.0f)
        {
            NiPoint3 kParticlePosition = pkData->GetVertices()[us];
            float fDistance = (kParticlePosition - kPosition).Length();
        
            if (!m_bUseMaxDistance || (fDistance <= m_fMaxDistance))
            {
        
                // Project the drag axis onto the velocity vector
                float fProj = 1.0f;
                
                if (m_bUseDirection)
                {
                    NiPoint3 kParticleDirection =
                        pkCurrentParticle->m_kVelocity;
                    kParticleDirection.Unitize();

                    fProj = kParticleDirection.Dot(kDragAxis);
                }

                float fReduction = (m_fMagnitude * fDeltaT * fProj / (1.0f + 
                    fDistance * m_fAttenuation));

                // Ensure we never decelerate so much we go backwards
                if (fReduction < 1.0f)
                {
                    pkCurrentParticle->m_kVelocity -= fReduction * 
                        pkCurrentParticle->m_kVelocity;
                }
                else
                {
                    pkCurrentParticle->m_kVelocity = NiPoint3::ZERO;
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPSysDragFieldModifier::UpdateNoAttenuationNoDirNoDist(float fTime, 
    NiPSysData* pkData)
{
    float fReduction;
    NiParticleInfo* pkCurrentParticle;

    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        // Get the Current Particle
        pkCurrentParticle = &pkData->GetParticleInfo()[us];

        fReduction = m_fMagnitude * (fTime - 
            pkCurrentParticle->m_fLastUpdate);

        // Ensure we never decelerate so much we go backwards
        if (fReduction < 1.0f)
        {
            // Compute the change in particle velocity
            pkCurrentParticle->m_kVelocity *= 1.0f - fReduction;
        }
        else
        {
            pkCurrentParticle->m_kVelocity = NiPoint3::ZERO;
        }
    }
}
//---------------------------------------------------------------------------
void NiPSysDragFieldModifier::UpdateNoAttenuationNoDir(float fTime, 
    NiPSysData* pkData)
{

    // Find the transform from Drag Space to Particle Space
    NiTransform kDrag = m_pkFieldObj->GetWorldTransform();
    NiTransform kPSys = m_pkTarget->GetWorldTransform();
    NiTransform kInvPSys;
    kPSys.Invert(kInvPSys);
    NiTransform kDragToPSys = kInvPSys * kDrag;

    NiPoint3 kPosition = kDragToPSys.m_Translate;
    float fReduction;
    NiParticleInfo* pkCurrentParticle;
    NiPoint3 kParticlePosition;

    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        // Get the Current Particle
        pkCurrentParticle = &pkData->GetParticleInfo()[us];

        // Compute the Reduction
        fReduction = m_fMagnitude * (fTime -
            pkCurrentParticle->m_fLastUpdate);

        if (fReduction != 0.0f)
        {
            kParticlePosition = pkData->GetVertices()[us];

            if ((kParticlePosition - kPosition).SqrLength() <= 
                m_fMaxDistanceSqr)
            {

                // Ensure we never decelerate so much we go backwards
                if (fReduction < 1.0f)
                {
                    // Compute the change in particle velocity
                    pkCurrentParticle->m_kVelocity *= 1.0f - fReduction;
                }
                else
                {
                    pkCurrentParticle->m_kVelocity = NiPoint3::ZERO;
                }
            }
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysDragFieldModifier);
//---------------------------------------------------------------------------
void NiPSysDragFieldModifier::CopyMembers(NiPSysDragFieldModifier* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysFieldModifier::CopyMembers(pkDest, kCloning);

    pkDest->m_bUseDirection = m_bUseDirection;
    pkDest->m_kDirection = m_kDirection;
}
//---------------------------------------------------------------------------
void NiPSysDragFieldModifier::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysModifier::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);
    NiPSysDragFieldModifier* pkDest = (NiPSysDragFieldModifier*) pkClone;

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
NiImplementCreateObject(NiPSysDragFieldModifier);
//---------------------------------------------------------------------------
void NiPSysDragFieldModifier::LoadBinary(NiStream& kStream)
{
    NiPSysFieldModifier::LoadBinary(kStream);

    // Load Use Direction
    NiBool bBool;
    NiStreamLoadBinary(kStream, bBool);
    m_bUseDirection = (bBool != 0);
    
    // Load Direction
    m_kDirection.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysDragFieldModifier::LinkObject(NiStream& kStream)
{
    NiPSysFieldModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysDragFieldModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysFieldModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysDragFieldModifier::SaveBinary(NiStream& kStream)
{
    NiPSysFieldModifier::SaveBinary(kStream);

    // Save Use Direction
    NiBool bBool = m_bUseDirection;
    NiStreamSaveBinary(kStream, bBool);

    // Save Direction
    m_kDirection.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysDragFieldModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysFieldModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysDragFieldModifier* pkDest = (NiPSysDragFieldModifier*) pkObject;

    if (pkDest->m_bUseDirection != m_bUseDirection ||
        pkDest->m_kDirection != m_kDirection)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysDragFieldModifier::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysFieldModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiPSysDragFieldModifier::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("Use Direction", m_bUseDirection));
    pkStrings->Add(m_kDirection.GetViewerString("Direction"));
}
//---------------------------------------------------------------------------
