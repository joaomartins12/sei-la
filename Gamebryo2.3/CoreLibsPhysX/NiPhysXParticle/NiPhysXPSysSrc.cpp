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
#include "NiPhysXParticlePCH.h"

#include "NiPhysXParticle.h"

NiImplementRTTI(NiPhysXPSysSrc, NiPhysXSrc);

//---------------------------------------------------------------------------
NiPhysXPSysSrc::NiPhysXPSysSrc(NiParticleSystem* pkSource) :
    m_pkSource(pkSource)
{
}
//---------------------------------------------------------------------------
NiPhysXPSysSrc::NiPhysXPSysSrc() : m_pkSource(0)
{
}
//---------------------------------------------------------------------------
NiPhysXPSysSrc::~NiPhysXPSysSrc()
{
}
//---------------------------------------------------------------------------
void NiPhysXPSysSrc::UpdateToActors(const float fTBegin, const float fTEnd)
{
    if (!m_bActive)
        return;

    NiPhysXParticleSystem* pkSource = (NiPhysXParticleSystem*)m_pkSource;
    NiPhysXPSysData* pkData = (NiPhysXPSysData*)pkSource->GetModelData();
    
    NiPhysXParticleInfo* pkPhysXInfo = pkData->GetPhysXParticleInfo(); 
    NiParticleInfo* pkInfo = pkData->GetParticleInfo();

    if (pkSource->GetPhysXSpace())
    {
        // Optimize for same coordinate system.
        for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
        {
            NxActor* pkActor = pkPhysXInfo[us].m_pkActor;

            if (!pkActor)
            {
                // Try to pull from the pool
                pkActor = pkData->RemoveActorFromPool();
                
                if (pkActor)
                {
                    pkActor->clearActorFlag(NX_AF_DISABLE_COLLISION);
                }
                else
                {
                    NxCompartment* pkCompartment = 0;
                    unsigned int uiID = pkData->GetCompartmentID();
                    if (uiID)
                    {
                        pkCompartment =
                            pkData->GetScene()->GetCompartmentAt(uiID);
                    }
                    pkActor = pkData->GetActorDesc()->CreateActor(
                        pkData->GetScene()->GetPhysXScene(), pkCompartment, 0,
                        NiPhysXTypes::NXMAT34_ID,
                        pkData->GetKeepsActorMeshes());
                    if (!pkActor)
                        continue;
                }
                pkPhysXInfo[us].m_pkActor = pkActor;

                NxVec3 kVec;
                NiPoint3 kPosn = pkData->GetVertices()[us] * m_fScaleWToP;
                NiPhysXTypes::NiPoint3ToNxVec3(kPosn, kVec);
                pkActor->setGlobalPosition(kVec);     
                NiPhysXTypes::NiPoint3ToNxVec3(pkInfo[us].m_kVelocity, kVec);
                kVec *= m_fScaleWToP;
                pkActor->setLinearVelocity(kVec);
            }
            else
            {
                NiPoint3 kVelChange = (pkInfo[us].m_kVelocity -
                    pkPhysXInfo[us].m_kLastVelocity) * m_fScaleWToP;

                NxVec3 kVec;
                NiPhysXTypes::NiPoint3ToNxVec3(kVelChange, kVec);
                pkPhysXInfo[us].m_pkActor->addForce(kVec, NX_VELOCITY_CHANGE);
            }            
        }
        return;
    }

    // Compute transform from PSys coords to PhysX coords
    NiTransform kPSys = pkSource->GetWorldTransform();
    NiTransform kPSysToPhysX = m_kInvRootTransform * kPSys;

    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        NxActor* pkActor = pkPhysXInfo[us].m_pkActor;

        if (!pkActor)
        {
            // Try to pull from the pool
            pkActor = pkData->RemoveActorFromPool();
            
            if (pkActor)
            {
                pkActor->clearActorFlag(NX_AF_DISABLE_COLLISION);
            }
            else
            {
                NxCompartment* pkCompartment = 0;
                unsigned int uiID = pkData->GetCompartmentID();
                if (uiID)
                {
                    pkCompartment = pkData->GetScene()->GetCompartmentAt(uiID);
                }
                pkActor = pkData->GetActorDesc()->CreateActor(
                    pkData->GetScene()->GetPhysXScene(), pkCompartment, 0,
                    NiPhysXTypes::NXMAT34_ID, pkData->GetKeepsActorMeshes());
                if (!pkActor)
                    continue;
            }
            pkPhysXInfo[us].m_pkActor = pkActor;

            NxVec3 kVec;            
            NiPoint3 kPosn = kPSysToPhysX * pkData->GetVertices()[us];
            kPosn *= m_fScaleWToP;
            NiPhysXTypes::NiPoint3ToNxVec3(kPosn, kVec);
            pkActor->setGlobalPosition(kVec);
            NiPoint3 kVelocity = 
                kPSysToPhysX.m_Rotate * pkInfo[us].m_kVelocity;
            kVelocity *= m_fScaleWToP;
            NiPhysXTypes::NiPoint3ToNxVec3(kVelocity, kVec);
            pkActor->setLinearVelocity(kVec);
        }
        else
        {
            NiPoint3 kVelChange = (pkInfo[us].m_kVelocity -
                pkPhysXInfo[us].m_kLastVelocity) * m_fScaleWToP;

            kVelChange = kPSysToPhysX.m_Rotate * kVelChange;

            NxVec3 kVec;
            NiPhysXTypes::NiPoint3ToNxVec3(kVelChange, kVec);
            pkActor->addForce(kVec, NX_VELOCITY_CHANGE);
        }            
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXPSysSrc::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXSrc::RegisterStreamables(kStream))
        return false;

    m_pkSource->RegisterStreamables(kStream);
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSysSrc::SaveBinary(NiStream& kStream)
{
    NiPhysXSrc::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkSource);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXPSysSrc);
//---------------------------------------------------------------------------
bool NiPhysXPSysSrc::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSysSrc::LoadBinary(NiStream& kStream)
{
    NiPhysXSrc::LoadBinary(kStream);

    m_pkSource = (NiParticleSystem*)kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPhysXPSysSrc::LinkObject(NiStream& kStream)
{
    NiPhysXSrc::LinkObject(kStream);
}

//---------------------------------------------------------------------------
bool NiPhysXPSysSrc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXPSysSrc, pkObject));
    if(!NiPhysXSrc::IsEqual(pkObject))
        return false;
        
    NiPhysXPSysSrc* pkNewObject = (NiPhysXPSysSrc*)pkObject;

    if (!m_pkSource->IsEqual(pkNewObject->m_pkSource))
        return false;

    return true;
}
//---------------------------------------------------------------------------
