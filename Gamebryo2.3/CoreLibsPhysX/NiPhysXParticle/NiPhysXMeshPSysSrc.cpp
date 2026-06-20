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

NiImplementRTTI(NiPhysXMeshPSysSrc, NiPhysXPSysSrc);

//---------------------------------------------------------------------------
NiPhysXMeshPSysSrc::NiPhysXMeshPSysSrc(NiParticleSystem* pkSource) :
    NiPhysXPSysSrc(pkSource)
{
}
//---------------------------------------------------------------------------
NiPhysXMeshPSysSrc::NiPhysXMeshPSysSrc()
{
}
//---------------------------------------------------------------------------
NiPhysXMeshPSysSrc::~NiPhysXMeshPSysSrc()
{
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysSrc::UpdateToActors(const float fTBegin, const float fTEnd)
{
    if (!m_bActive)
        return;

    NiPhysXMeshParticleSystem* pkSource =
        (NiPhysXMeshParticleSystem*)m_pkSource;
    NiPhysXMeshPSysData* pkData =
        (NiPhysXMeshPSysData*)pkSource->GetModelData();
    
    NIASSERT(pkData->GetActorDescCount() > 0);
    
    NiPhysXParticleInfo* pkPhysXInfo = pkData->GetPhysXParticleInfo(); 
    NiParticleInfo* pkInfo = pkData->GetParticleInfo();
    
    float* pfRotationAngles = pkData->GetRotationAngles();
    NiPoint3* pkRotationAxes = pkData->GetRotationAxes();

    if (pkSource->GetPhysXSpace())
    {
        // Optimize for same coordinate system.
        for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
        {
            NxActor* pkActor = pkPhysXInfo[us].m_pkActor;
            
            if (!pkActor)
            {
                unsigned short usGen = pkInfo[us].m_usGeneration;
                if (usGen >= pkData->GetActorDescCount())
                {
                    usGen = pkData->GetActorDescCount() - 1;
                }

                // Try to pull from the pool
                pkActor = pkData->RemoveActorFromPool((unsigned int)usGen);

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
                    pkActor = pkData->GetActorDescAt(usGen)->
                        CreateActor(pkData->GetScene()->GetPhysXScene(),
                        pkCompartment, 0, NiPhysXTypes::NXMAT34_ID,
                        pkData->GetKeepsActorMeshes());
                    if (!pkActor)
                        continue;
                }
                pkPhysXInfo[us].m_pkActor = pkActor;

                NxVec3 kVec;            
                if (pfRotationAngles && pkRotationAxes)
                {
                    NiMatrix3 kRotate;
                    kRotate.MakeRotation(pfRotationAngles[us],
                        pkRotationAxes[us]);
                    NiPoint3 kPosn = pkData->GetVertices()[us] * m_fScaleWToP;
                    NxMat34 kPose;
                    NiPhysXTypes::NiTransformToNxMat34(kRotate, kPosn, kPose);
                    pkActor->setGlobalPose(kPose);
                }
                else
                {
                    NiPoint3 kPosn = pkData->GetVertices()[us] * m_fScaleWToP;
                    NiPhysXTypes::NiPoint3ToNxVec3(kPosn, kVec);
                    pkActor->setGlobalPosition(kVec);
                }        
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
                pkActor->addForce(kVec, NX_VELOCITY_CHANGE);
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
            unsigned short usGen = pkInfo[us].m_usGeneration;
            if (usGen >= pkData->GetActorDescCount())
            {
                usGen = pkData->GetActorDescCount() - 1;
            }

            // Try to pull from the pool
            pkActor = pkData->RemoveActorFromPool((unsigned int)usGen);

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
                pkActor = pkData->GetActorDescAt(usGen)->
                    CreateActor(pkData->GetScene()->GetPhysXScene(),
                    pkCompartment, 0, NiPhysXTypes::NXMAT34_ID,
                    pkData->GetKeepsActorMeshes());
                if (!pkActor)
                    continue;
            }
            pkPhysXInfo[us].m_pkActor = pkActor;

            NxVec3 kVec;            
            if (pfRotationAngles && pkRotationAxes)
            {
                NiMatrix3 kLocalRotate;
                kLocalRotate.MakeRotation(pfRotationAngles[us],
                    pkRotationAxes[us]);
                NiMatrix3 kRotate = kPSysToPhysX.m_Rotate * kLocalRotate;
                NiPoint3 kPosn = kPSysToPhysX * pkData->GetVertices()[us];
                kPosn *= m_fScaleWToP;
                NxMat34 kPose;
                NiPhysXTypes::NiTransformToNxMat34(kRotate, kPosn, kPose);
                pkActor->setGlobalPose(kPose);
            }
            else
            {
                NiPoint3 kPosn = kPSysToPhysX * pkData->GetVertices()[us];
                kPosn *= m_fScaleWToP;
                NiPhysXTypes::NiPoint3ToNxVec3(kPosn, kVec);
                pkActor->setGlobalPosition(kVec);
            }
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
bool NiPhysXMeshPSysSrc::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXPSysSrc::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysSrc::SaveBinary(NiStream& kStream)
{
    NiPhysXPSysSrc::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXMeshPSysSrc);
//---------------------------------------------------------------------------
bool NiPhysXMeshPSysSrc::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysSrc::LoadBinary(NiStream& kStream)
{
    NiPhysXPSysSrc::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysSrc::LinkObject(NiStream& kStream)
{
    NiPhysXPSysSrc::LinkObject(kStream);
}

//---------------------------------------------------------------------------
bool NiPhysXMeshPSysSrc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXMeshPSysSrc, pkObject));
    if(!NiPhysXPSysSrc::IsEqual(pkObject))
        return false;

    return true;
}
//---------------------------------------------------------------------------
