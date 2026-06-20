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

#include "NiPSysRadialFieldModifier.h"
#include "NiParticleSystem.h"
#include <NiCloningProcess.h>

NiImplementRTTI(NiPSysRadialFieldModifier, NiPSysFieldModifier);

//---------------------------------------------------------------------------
NiPSysRadialFieldModifier::NiPSysRadialFieldModifier(const char* pcName, 
    NiAVObject* pkFieldObj, float fMagnitude, float fAttenuation,
    bool bUseMaxDistance, float fMaxDistance, float fRadialType) : 
    NiPSysFieldModifier(pcName, pkFieldObj, fMagnitude, fAttenuation, 
    bUseMaxDistance, fMaxDistance)
{
    SetRadialType(fRadialType);
}
//---------------------------------------------------------------------------
NiPSysRadialFieldModifier::NiPSysRadialFieldModifier() :
    NiPSysFieldModifier()
{
    m_fRadialType = 0.0f;
}
//---------------------------------------------------------------------------
void NiPSysRadialFieldModifier::Update(float fTime, NiPSysData* pkData)
{

    // Check for a zero Magnitude, No Particles, or No Radial Object
    if ((m_fMagnitude == 0.0f) || (pkData->GetNumParticles() == 0) ||
        !m_pkFieldObj)
    {
        return;
    }


    // Check for optimized update path
    if (!m_bUseMaxDistance && 
        ((m_fAttenuation == 0.0f) || (m_fRadialType == 0.0f)))
    {
        UpdateNoAttenuationNoDirNoDist(fTime, pkData);
        return;
    }


    // Find the transform from Radial Space to Particle Space
    NiTransform kRadial = m_pkFieldObj->GetWorldTransform();
    NiTransform kPSys = m_pkTarget->GetWorldTransform();
    NiTransform kInvPSys;
    kPSys.Invert(kInvPSys);
    NiTransform kRadialToPSys = kInvPSys * kRadial;


    NiPoint3 kPosition = kRadialToPSys.m_Translate;

    // Compute the Half atttenuation modified by the Radial Type
    float fHalfAttenuation = m_fAttenuation * 0.5f;

        //
        //  kDirection * (m_fMagnitude * fDeltaT / NiPow(fDistance,
        //      m_fAttenuation));
        //

    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        NiParticleInfo* pkCurrentParticle = &pkData
            ->GetParticleInfo()[us];
        float fDeltaT = (fTime - pkCurrentParticle->m_fLastUpdate);

        /********

        //          The Algorithm according to Maya 

        if (fDeltaT != 0.0f)
        {
        NiPoint3 kParticlePosition = pkData->GetVertices()[us];

        NiPoint3 kDirection = kParticlePosition - kPosition;
        float fDistance = kDirection.Unitize();

        if (!m_bUseMaxDistance || (fDistance <= m_fMaxDistance))
        {
        NiPoint3 kDeltaV;

        if ((m_fAttenuation == 0.0f) || (fDistance == 0.0f))
        {
        kDeltaV = kDirection * (m_fMagnitude * fDeltaT);
        }
        else
        {

        // Since kDirection already has the distance we don't need to
        // determine and multipy by the distance.
        kDeltaV = kDirection * (-m_fMagnitude * fDeltaT * 
        NiPow(1 - fDistance / m_fMaxDistance , m_fAttenuation));
        }

        pkCurrentParticle->m_kVelocity += kDeltaV;
        }
        }
        }
        ********/

        // The Observed Algorthim
        if (fDeltaT != 0.0f)
        {
            NiPoint3 kParticlePosition = pkData->GetVertices()[us];

            NiPoint3 kDirection = kParticlePosition - kPosition;
            float fDistanceSqr = kDirection.SqrLength();

            if (!m_bUseMaxDistance || (fDistanceSqr <= m_fMaxDistanceSqr))
            {
                NiPoint3 kDeltaV;

                if ((m_fAttenuation == 0.0f) || (fDistanceSqr == 0.0f))
                {
                    kDeltaV = kDirection * (m_fMagnitude * fDeltaT);
                }
                else
                {
                    // Since kDirection already has the distance we don't
                    // need to determine and multipy by the distance.
                    kDeltaV = kDirection * (m_fMagnitude * fDeltaT / 
                        NiMax(1.0f, NiPow(fDistanceSqr, fHalfAttenuation) *
                        m_fRadialType));
                }

                pkCurrentParticle->m_kVelocity += kDeltaV;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPSysRadialFieldModifier::UpdateNoAttenuationNoDirNoDist(float fTime, 
    NiPSysData* pkData)
{
    // Find the transform from Radial Space to Particle Space
    NiTransform kRadial = m_pkFieldObj->GetWorldTransform();
    NiTransform kPSys = m_pkTarget->GetWorldTransform();
    NiTransform kInvPSys;
    kPSys.Invert(kInvPSys);
    NiTransform kRadialToPSys = kInvPSys * kRadial;


    NiPoint3 kPosition = kRadialToPSys.m_Translate;
    NiPoint3 kDirection;


    // This is the general algorithm but because we know some values
    // The algorithm will simplify greatly.
    //
    //
    //  kDirection * (m_fMagnitude * fDeltaT / NiPow(fDistance,
    //      m_fAttenuation));
    // 
    // Simplifying the Algorithm
    //
    // kDirection * (m_fMagnitude * fDeltaTime) * fDistance
    //

    float fMagnitude = m_fMagnitude;
    NiPoint3* pkParticlePosition = pkData->GetVertices();

    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        NiParticleInfo* pkCurrentParticle = &pkData
            ->GetParticleInfo()[us];
        float fDeltaT = (fTime - pkCurrentParticle->m_fLastUpdate);

        
        if (fDeltaT != 0.0f)
        {
            // Find the direction from the particle to the Radial
            kDirection = *pkParticlePosition++ - kPosition;

            // Since kDirection already has the distance we don't need to
            // determine and multipy by the distance.
            pkCurrentParticle->m_kVelocity += kDirection * (fMagnitude *
                fDeltaT);
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysRadialFieldModifier);
//---------------------------------------------------------------------------
void NiPSysRadialFieldModifier::CopyMembers(
    NiPSysRadialFieldModifier* pkDest, NiCloningProcess& kCloning)
{
    NiPSysFieldModifier::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
void NiPSysRadialFieldModifier::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysModifier::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);
    NiPSysRadialFieldModifier* pkDest = (NiPSysRadialFieldModifier*) pkClone;

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
NiImplementCreateObject(NiPSysRadialFieldModifier);
//---------------------------------------------------------------------------
void NiPSysRadialFieldModifier::LoadBinary(NiStream& kStream)
{
    NiPSysFieldModifier::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fRadialType);
}
//---------------------------------------------------------------------------
void NiPSysRadialFieldModifier::LinkObject(NiStream& kStream)
{
    NiPSysFieldModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysRadialFieldModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysFieldModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysRadialFieldModifier::SaveBinary(NiStream& kStream)
{
    NiPSysFieldModifier::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fRadialType);
}
//---------------------------------------------------------------------------
bool NiPSysRadialFieldModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysFieldModifier::IsEqual(pkObject))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysRadialFieldModifier::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiPSysFieldModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiPSysRadialFieldModifier::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("RadialType", m_fRadialType));

}
//---------------------------------------------------------------------------
