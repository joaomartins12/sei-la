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

NiImplementRTTI(NiPhysXPSysDest, NiPhysXDest);

//---------------------------------------------------------------------------
NiPhysXPSysDest::NiPhysXPSysDest(NiParticleSystem* pkTarget)
    : m_pkTarget(pkTarget)
{
}
//---------------------------------------------------------------------------
NiPhysXPSysDest::NiPhysXPSysDest() : m_pkTarget(0)
{
}
//---------------------------------------------------------------------------
NiPhysXPSysDest::~NiPhysXPSysDest()
{
}
//---------------------------------------------------------------------------
void NiPhysXPSysDest::UpdateFromActors(const float fT,
    const NiTransform& kRootTransform, const float fScalePToW,
    const bool bForce)
{
    if (!m_bActive)
        return;
    
    NiPSysData* pkData = (NiPSysData*)m_pkTarget->GetModelData();
    NiPhysXParticleInfo* pkInfo;
    bool bPhysXSpace;
    bool bUsesRotations;
    if (NiIsKindOf(NiPhysXPSysData, pkData))
    {
        pkInfo = ((NiPhysXPSysData*)pkData)->GetPhysXParticleInfo();
        bPhysXSpace = ((NiPhysXParticleSystem*)m_pkTarget)->GetPhysXSpace();
        bUsesRotations = false;
    }
    else
    {
        pkInfo = ((NiPhysXMeshPSysData*)pkData)->GetPhysXParticleInfo(); 
        bPhysXSpace = 
            ((NiPhysXMeshParticleSystem*)m_pkTarget)->GetPhysXSpace();
        bUsesRotations =
            ((NiPhysXMeshParticleSystem*)m_pkTarget)->GetUsesPhysXRotations();
    }
    
    float* pfRotationAngles = pkData->GetRotationAngles();
    NiPoint3* pkRotationAxes = pkData->GetRotationAxes();

    if (bPhysXSpace)
    {
        // Optimize for case where the particle system and PhysX root are
        // in the same coordinate system.
        for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
        {
            NxActor* pkActor = pkInfo[us].m_pkActor;
            
            if (!pkActor || pkActor->readActorFlag(NX_AF_DISABLE_COLLISION))
            {
                // This actor isn't initialized yet, so don't update it.
                // We need this because with fixed timing updates, this
                // function may get called before the source update function
                // is called, and hence before the newly added actors have
                // been initialized.
                continue;
            }

            NxVec3 kVec;
            if (bUsesRotations)
            {
                NxMat34 kPose;
                NiPoint3 kPosn;
                NiMatrix3 kRot;
                kPose = pkInfo[us].m_pkActor->getGlobalPose();
                NiPhysXTypes::NxMat34ToNiTransform(kPose, kRot, kPosn);

                pkData->GetVertices()[us] = kPosn * fScalePToW;

                kRot.ExtractAngleAndAxis(pfRotationAngles[us],
                    pkRotationAxes[us].x,
                    pkRotationAxes[us].y,
                    pkRotationAxes[us].z);
            }
            else
            {
                NiPoint3 kPosn;
                
                kVec = pkInfo[us].m_pkActor->getGlobalPosition();
                NiPhysXTypes::NxVec3ToNiPoint3(kVec, kPosn);
                pkData->GetVertices()[us] = kPosn * fScalePToW;
            }
            
            NiPoint3 kVelocity;
            kVec = pkInfo[us].m_pkActor->getLinearVelocity();
            NiPhysXTypes::NxVec3ToNiPoint3(kVec, kVelocity);
            kVelocity *= fScalePToW;
            pkData->GetParticleInfo()[us].m_kVelocity = kVelocity;
            pkInfo[us].m_kLastVelocity = kVelocity; 
        }
        return;
    }
    
    NiTransform kPSys = m_pkTarget->GetWorldTransform();
    NiTransform kInvPSys;
    kPSys.Invert(kInvPSys);
    NiTransform kPhysXToPSys = kInvPSys * kRootTransform;
    NiMatrix3 kPhysXToPSysRot = kPhysXToPSys.m_Rotate;

    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        NxActor* pkActor = pkInfo[us].m_pkActor;
        
        if (!pkActor)
        {
            // This actor isn't initialized yet, so don't update it.
            // We need this because with fixed timing updates, this
            // function may get called before the source update function
            // is called, and hence before the newly added actors have
            // been initialized.
            continue;
        }

        NxVec3 kVec;
        if (bUsesRotations)
        {
            NxMat34 kPose;
            NiPoint3 kPosn;
            NiMatrix3 kGlobalRot;
            kPose = pkInfo[us].m_pkActor->getGlobalPose();
            NiPhysXTypes::NxMat34ToNiTransform(kPose, kGlobalRot, kPosn);

            pkData->GetVertices()[us] = (kPhysXToPSys * kPosn) * fScalePToW;

            NiMatrix3 kRot = kPhysXToPSys.m_Rotate * kGlobalRot;
            kRot.ExtractAngleAndAxis(pfRotationAngles[us],
                pkRotationAxes[us].x,
                pkRotationAxes[us].y,
                pkRotationAxes[us].z);               
        }
        else
        {
            NiPoint3 kPosn;
            
            kVec = pkInfo[us].m_pkActor->getGlobalPosition();
            NiPhysXTypes::NxVec3ToNiPoint3(kVec, kPosn);
            pkData->GetVertices()[us] = (kPhysXToPSys * kPosn) * fScalePToW;
        }
        
        NiPoint3 kVelocity;
        kVec = pkInfo[us].m_pkActor->getLinearVelocity();
        NiPhysXTypes::NxVec3ToNiPoint3(kVec, kVelocity);
        kVelocity = (kPhysXToPSysRot * kVelocity) * fScalePToW;
        pkData->GetParticleInfo()[us].m_kVelocity = kVelocity;
        pkInfo[us].m_kLastVelocity = kVelocity; 
    }
}
//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXPSysDest::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXDest::RegisterStreamables(kStream))
        return false;

    m_pkTarget->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSysDest::SaveBinary(NiStream& kStream)
{
    NiPhysXDest::SaveBinary(kStream);
    
    kStream.SaveLinkID(m_pkTarget);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXPSysDest);
//---------------------------------------------------------------------------
bool NiPhysXPSysDest::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSysDest::LoadBinary(NiStream& kStream)
{
    NiPhysXDest::LoadBinary(kStream);

    m_pkTarget = (NiPhysXParticleSystem*)kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPhysXPSysDest::LinkObject(NiStream& kStream)
{
    NiPhysXDest::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXPSysDest::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXPSysDest, pkObject));
    if(!NiPhysXDest::IsEqual(pkObject))
        return false;

    NiPhysXPSysDest* pkNewObject = (NiPhysXPSysDest*)pkObject;

    if (!m_pkTarget->IsEqual(pkNewObject->m_pkTarget))
        return false;

    return true;
}
//---------------------------------------------------------------------------
