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

#include "NiPSysAgeDeathModifier.h"
#include "NiParticleSystem.h"
#include "NiPSysSpawnModifier.h"
#include <NiCloningProcess.h>

NiImplementRTTI(NiPSysAgeDeathModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysAgeDeathModifier::NiPSysAgeDeathModifier(const char* pcName,
    bool bSpawnOnDeath, NiPSysSpawnModifier* pkSpawnModifier) :
    NiPSysModifier(pcName, ORDER_KILLOLDPARTICLES),
    m_bSpawnOnDeath(bSpawnOnDeath), m_pkSpawnModifier(pkSpawnModifier)
{
}
//---------------------------------------------------------------------------
NiPSysAgeDeathModifier::NiPSysAgeDeathModifier() : m_bSpawnOnDeath(false),
    m_pkSpawnModifier(NULL)
{
}
//---------------------------------------------------------------------------
void NiPSysAgeDeathModifier::Update(float fTime, NiPSysData* pkData)
{
    // This code is very similar to that in NiPhysXPSySAgeDeathModifier
    // from the NiPhysXParticles library. Changes here may need to be
    // reflected there.

    // Handle spawning death.
    if (m_bSpawnOnDeath && m_pkSpawnModifier)
    {
        // Iterate over particles backwards to avoid removing already dead
        // particles.
        for (unsigned short us = pkData->GetNumParticles(); us > 0; us--)
        {
            unsigned short usParticleIdx = us - 1;

            // Get particle.
            NiParticleInfo* pkCurrentParticle =
                &pkData->GetParticleInfo()[usParticleIdx];

            // Update age.
            pkCurrentParticle->m_fAge += fTime -
                pkCurrentParticle->m_fLastUpdate;
            
            // Update death.
            if (pkCurrentParticle->m_fAge > pkCurrentParticle->m_fLifeSpan)
            {
                m_pkSpawnModifier->SpawnParticles(fTime,
                    fTime + pkCurrentParticle->m_fLifeSpan -
                    pkCurrentParticle->m_fAge, usParticleIdx, m_pkTarget);
                
                pkData->RemoveParticle(usParticleIdx);
            }
        }
    }
    else    // No Spawning Death
    {
        // Iterate over particles backwards to avoid removing already dead
        // particles.
        for (unsigned short us = pkData->GetNumParticles(); us > 0; us--)
        {
            unsigned short usParticleIdx = us - 1;

            NiParticleInfo* pkCurrentParticle =
                &pkData->GetParticleInfo()[usParticleIdx];

            // Update age.
            pkCurrentParticle->m_fAge += fTime -
                pkCurrentParticle->m_fLastUpdate;
            
            // Update death.
            if (pkCurrentParticle->m_fAge > pkCurrentParticle->m_fLifeSpan)
            {
                pkData->RemoveParticle(usParticleIdx);
            }
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysAgeDeathModifier);
//---------------------------------------------------------------------------
void NiPSysAgeDeathModifier::CopyMembers(NiPSysAgeDeathModifier* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifier::CopyMembers(pkDest, kCloning);

    pkDest->m_bSpawnOnDeath = m_bSpawnOnDeath;
}
//---------------------------------------------------------------------------
void NiPSysAgeDeathModifier::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysModifier::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);
    NiPSysAgeDeathModifier* pkDest = (NiPSysAgeDeathModifier*) pkClone;

    if (m_pkSpawnModifier)
    {
        bCloned = kCloning.m_pkCloneMap->GetAt(m_pkSpawnModifier, pkClone);
        if (bCloned)
        {
            pkDest->m_pkSpawnModifier = (NiPSysSpawnModifier*) pkClone;
        }
        else
        {
            pkDest->m_pkSpawnModifier = m_pkSpawnModifier;
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysAgeDeathModifier);
//---------------------------------------------------------------------------
void NiPSysAgeDeathModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    NiBool bSpawnOnDeath;
    NiStreamLoadBinary(kStream, bSpawnOnDeath);
    m_bSpawnOnDeath = (bSpawnOnDeath == 0) ? false : true;
    kStream.ReadLinkID();   // m_pkSpawnModifier
}
//---------------------------------------------------------------------------
void NiPSysAgeDeathModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);

    m_pkSpawnModifier = (NiPSysSpawnModifier*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPSysAgeDeathModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysAgeDeathModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, NiBool(m_bSpawnOnDeath));
    kStream.SaveLinkID(m_pkSpawnModifier);
}
//---------------------------------------------------------------------------
bool NiPSysAgeDeathModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysAgeDeathModifier* pkDest = (NiPSysAgeDeathModifier*) pkObject;

    if (pkDest->m_bSpawnOnDeath != m_bSpawnOnDeath)
    {
        return false;
    }

    if ((pkDest->m_pkSpawnModifier && !m_pkSpawnModifier) ||
        (!pkDest->m_pkSpawnModifier && m_pkSpawnModifier) ||
        (pkDest->m_pkSpawnModifier && m_pkSpawnModifier &&
            !pkDest->m_pkSpawnModifier->IsEqual(m_pkSpawnModifier)))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysAgeDeathModifier::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysAgeDeathModifier::ms_RTTI
        .GetName()));

    pkStrings->Add(NiGetViewerString("Spawn on Death", m_bSpawnOnDeath));
    const char* pcSpawnModifierName;
    if (m_pkSpawnModifier)
    {
        pcSpawnModifierName = m_pkSpawnModifier->GetName();
    }
    else
    {
        pcSpawnModifierName = "None";
    }
    pkStrings->Add(NiGetViewerString("Spawn Modifier", pcSpawnModifierName));
}
//---------------------------------------------------------------------------
