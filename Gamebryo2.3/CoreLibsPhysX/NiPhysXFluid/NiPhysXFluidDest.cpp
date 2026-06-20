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
#include "NiPhysXFluidPCH.h"

NiImplementRTTI(NiPhysXFluidDest, NiPhysXDest);

//---------------------------------------------------------------------------
NiPhysXFluidDest::NiPhysXFluidDest(NiParticleSystem* pkTarget)
    : m_pkTarget(pkTarget)
{
    m_kPhysXToFluid.MakeIdentity();
}
//---------------------------------------------------------------------------
NiPhysXFluidDest::NiPhysXFluidDest() : m_pkTarget(0)
{
    m_kPhysXToFluid.MakeIdentity();
}
//---------------------------------------------------------------------------
NiPhysXFluidDest::~NiPhysXFluidDest()
{
}
//---------------------------------------------------------------------------
void NiPhysXFluidDest::UpdateFromActors(const float fT,
    const NiTransform& kRootTransform, const float fScalePToW,
    const bool bForce)
{
    if (!m_bActive)
        return;
    
    NiPSysData* pkData = (NiPSysData*)m_pkTarget->GetModelData();

    NxParticleData kFluidData; 
    bool bPhysXSpace;
    if (NiIsKindOf(NiPhysXFluidData, pkData))
    {
        kFluidData = ((NiPhysXFluidData*)pkData)->GetFluidData();
        bPhysXSpace = ((NiPhysXFluidSystem*)m_pkTarget)->GetPhysXSpace();
    }
    else if (NiIsKindOf(NiPhysXMeshFluidData, pkData))
    {
        kFluidData = ((NiPhysXMeshFluidData*)pkData)->GetFluidData();
        bPhysXSpace = ((NiPhysXMeshFluidSystem*)m_pkTarget)->GetPhysXSpace();
    }
    else
    {
        NIASSERT(false && "NiPhysXFluidUpdateModifier: Wrong type\n");
    }

    if (!kFluidData.bufferPos)
        return;
     
    // We could have more or less fluid particles than active in the
    // particle system. Just work with what we have for now and leave
    // additions or deletions to the particle system update phase.   
    unsigned short usNumParticles =
        NiMin(pkData->GetNumParticles(), *(kFluidData.numParticlesPtr));

    NxF32* pfPosns = kFluidData.bufferPos;
    unsigned int uiPosStride = kFluidData.bufferPosByteStride / sizeof(NxF32);
    NxF32* pfVelocities = kFluidData.bufferVel;
    unsigned int uiVelStride = kFluidData.bufferVelByteStride / sizeof(NxF32);

    if (bPhysXSpace)
    {
        // Optimize for case where the particle system and PhysX root are
        // in the same coordinate system.
        unsigned short us;
        unsigned int ui;
        for (us = 0, ui = 0; us < usNumParticles; us++, ui += uiPosStride)
        {
            NiPoint3 kPosn;
            kPosn.x = pfPosns[ui];
            kPosn.y = pfPosns[ui+1];
            kPosn.z = pfPosns[ui + 2];
            kPosn *= fScalePToW;
            pkData->GetVertices()[us] = kPosn;

            if (pfVelocities)
            {
                NiPoint3 kVelocity;
                kVelocity.x = *pfVelocities;
                kVelocity.y = *(pfVelocities + 1);
                kVelocity.z = *(pfVelocities + 2);
                pfVelocities += uiVelStride;
                kVelocity *= fScalePToW;
                
                pkData->GetParticleInfo()[us].m_kVelocity = kVelocity;
            }
        }
        return;
    }
    
    NiTransform kFluid = m_pkTarget->GetWorldTransform();
    NiTransform kInvFluid;
    kFluid.Invert(kInvFluid);
    
    m_kPhysXToFluid = kInvFluid * kRootTransform;

    for (unsigned short us = 0; us < usNumParticles; us++)
    {
        NiPoint3 kPosn;
        kPosn.x = *pfPosns;
        kPosn.y = *(pfPosns + 1);
        kPosn.z = *(pfPosns + 2);
        pfPosns += uiPosStride;
        kPosn *= fScalePToW;

        pkData->GetVertices()[us] = m_kPhysXToFluid * kPosn;              

        if (pfVelocities)
        {
            NiPoint3 kVelocity;
            kVelocity.x = *pfVelocities;
            kVelocity.y = *(pfVelocities + 1);
            kVelocity.z = *(pfVelocities + 2);
            pfVelocities += uiVelStride;
            kVelocity *= fScalePToW;
        
            pkData->GetParticleInfo()[us].m_kVelocity =
                m_kPhysXToFluid.m_Rotate * kVelocity;
        }
    }
}
//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXFluidDest::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXDest::RegisterStreamables(kStream))
        return false;

    m_pkTarget->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXFluidDest::SaveBinary(NiStream& kStream)
{
    NiPhysXDest::SaveBinary(kStream);
    
    kStream.SaveLinkID(m_pkTarget);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXFluidDest);
//---------------------------------------------------------------------------
bool NiPhysXFluidDest::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXFluidDest::LoadBinary(NiStream& kStream)
{
    NiPhysXDest::LoadBinary(kStream);

    m_pkTarget = (NiPhysXFluidSystem*)kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPhysXFluidDest::LinkObject(NiStream& kStream)
{
    NiPhysXDest::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXFluidDest::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXFluidDest, pkObject));
    if(!NiPhysXDest::IsEqual(pkObject))
        return false;

    NiPhysXFluidDest* pkNewObject = (NiPhysXFluidDest*)pkObject;

    if (!m_pkTarget->IsEqual(pkNewObject->m_pkTarget))
        return false;

    return true;
}
//---------------------------------------------------------------------------
