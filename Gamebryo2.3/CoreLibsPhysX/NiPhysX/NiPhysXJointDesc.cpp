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

#include "NiPhysXPCH.h"

#include "NiPhysX.h"

#include <NxPhysics.h>

NiImplementRTTI(NiPhysXJointDesc, NiObject);

//---------------------------------------------------------------------------
NiPhysXJointDesc::NiPhysXJointDesc() : m_kJointName(NULL)
{
    m_eType = NX_JOINT_COUNT;
    m_aspActors[0] = 0;
    m_aspActors[1] = 0;
    m_akLocalNormal[0].set(1.0f, 0.0f, 0.0f);
    m_akLocalNormal[1].set(1.0f, 0.0f, 0.0f);
    m_akLocalAxis[0].set(0.0f, 0.0f, 1.0f);
    m_akLocalAxis[1].set(0.0f, 0.0f, 1.0f);
    m_akLocalAnchor[0].zero();
    m_akLocalAnchor[1].zero();
    m_fMaxForce = NX_MAX_REAL;
    m_fMaxTorque = NX_MAX_REAL;
    m_uiJointFlags = NX_JF_VISUALIZATION;
    
    m_kLimitPoint.zero();
    m_uiNumLimits = 0;
    m_pkLimitPlaneNorms = 0;
    m_pkLimitPlaneDs = 0;
}
//---------------------------------------------------------------------------
NiPhysXJointDesc::~NiPhysXJointDesc()
{
    m_aspActors[0] = 0;
    m_aspActors[1] = 0;
    
    NiFree(m_pkLimitPlaneNorms);
    NiFree(m_pkLimitPlaneDs);
}
//---------------------------------------------------------------------------
NiPhysXJointDesc* NiPhysXJointDesc::CreateFromJoint(NxJoint* pkJoint,
    NiTMap<NxActor*, NiPhysXActorDescPtr>& kActorMap, const NxMat34& kXform)
{
    NxD6Joint* pkD6Joint = pkJoint->isD6Joint();
    if (!pkD6Joint)
    {
        NiOutputDebugString(
            "NiPhysXJointDesc::CreateFromJoint only "
            "support D6 type joints.\n");
        return 0;
    }
    
    NiPhysXD6JointDesc* pkD6JointDesc = NiNew NiPhysXD6JointDesc;
    pkD6JointDesc->FromJoint(pkD6Joint, kActorMap, kXform);
    
    return pkD6JointDesc;
}
//---------------------------------------------------------------------------
NiPhysXActorDescPtr NiPhysXJointDesc::GetActor(const unsigned int uiIndex)
{
    return m_aspActors[uiIndex];
}
//---------------------------------------------------------------------------
void NiPhysXJointDesc::SetActor(const unsigned int uiIndex,
    NiPhysXActorDesc* kActorDesc)
{
    m_aspActors[uiIndex] = kActorDesc;
}
//---------------------------------------------------------------------------
NxVec3 NiPhysXJointDesc::GetLimitPoint() const
{
    return m_kLimitPoint;
}
//---------------------------------------------------------------------------
void NiPhysXJointDesc::SetLimitPoint(const NxVec3& kLimitPt)
{
    m_kLimitPoint = kLimitPt;
}
//---------------------------------------------------------------------------
unsigned int NiPhysXJointDesc::GetNumLimits() const
{
    return m_uiNumLimits;
}
//---------------------------------------------------------------------------
void NiPhysXJointDesc::GetLimitPlane(const unsigned int uiIndex,
    NxVec3& kNorm, NxReal& kD)
{
    kNorm = m_pkLimitPlaneNorms[uiIndex];
    kD = m_pkLimitPlaneDs[uiIndex];
}
//---------------------------------------------------------------------------
void NiPhysXJointDesc::SetLimitPlane(const unsigned int uiIndex,
    const NxVec3& kNorm, const NxReal& kD)
{
    if (m_uiNumLimits <= uiIndex)
    {
        NxVec3* akNewNorms = NiAlloc(NxVec3, uiIndex + 1);
        NxReal* akNewDs = NiAlloc(NxReal, uiIndex + 1);
        for (unsigned int ui = 0; ui < m_uiNumLimits; ui++)
        {
            akNewNorms[ui] = m_pkLimitPlaneNorms[ui];
            akNewDs[ui] = m_pkLimitPlaneDs[ui];
        }
        NiFree(m_pkLimitPlaneNorms);
        NiFree(m_pkLimitPlaneDs);
        m_pkLimitPlaneNorms = akNewNorms;
        m_pkLimitPlaneDs = akNewDs;
        m_uiNumLimits = uiIndex + 1;
    }

    m_pkLimitPlaneNorms[uiIndex] = kNorm;
    m_pkLimitPlaneDs[uiIndex] = kD;
}
//---------------------------------------------------------------------------
void NiPhysXJointDesc::SetJointDesc(const NxJointDesc& kJointDesc)
{
    m_eType = kJointDesc.getType();

    if (kJointDesc.name)
        m_kJointName = NiFixedString(kJointDesc.name);
    else
        m_kJointName = NiFixedString(NULL);
        
    for (unsigned int i = 0; i < 2; i++)
    {
        m_akLocalNormal[i] = kJointDesc.localNormal[i];
        m_akLocalAxis[i] = kJointDesc.localAxis[i];
        m_akLocalAnchor[i] = kJointDesc.localAnchor[i];
    }

    m_fMaxForce = kJointDesc.maxForce;
    m_fMaxTorque = kJointDesc.maxTorque;
    m_uiJointFlags = kJointDesc.jointFlags; 
}
//---------------------------------------------------------------------------
void NiPhysXJointDesc::GetJointDesc(NxJointDesc& kJointDesc,
        const NxMat34& kXform) const
{
    kJointDesc.name = m_kJointName;
        
    for (unsigned int i = 0; i < 2; i++)
    {
        if (m_aspActors[i])
        {
            kJointDesc.actor[i] = m_aspActors[i]->GetActor();
            kJointDesc.localNormal[i] = m_akLocalNormal[i];
            kJointDesc.localAxis[i] = m_akLocalAxis[i];
            kJointDesc.localAnchor[i] = m_akLocalAnchor[i];
        }
        else
        {
            kJointDesc.actor[i] = NULL;
            kJointDesc.localNormal[i] = kXform.M * m_akLocalNormal[i];
            kJointDesc.localAxis[i] = kXform.M * m_akLocalAxis[i];
            kJointDesc.localAnchor[i] = kXform * m_akLocalAnchor[i];
        }
    }

    kJointDesc.maxForce = m_fMaxForce;
    kJointDesc.maxTorque = m_fMaxTorque;
    kJointDesc.jointFlags = m_uiJointFlags; 
}
//---------------------------------------------------------------------------
void NiPhysXJointDesc::FromJoint(NxJoint* pkJoint)
{
    // Can't get descriptor from here, so caller must ensure descriptor is
    // extracted.
    
    pkJoint->getLimitPoint(m_kLimitPoint);
    
    NiFree(m_pkLimitPlaneNorms);
    m_pkLimitPlaneNorms = 0;
    NiFree(m_pkLimitPlaneDs);
    m_pkLimitPlaneDs = 0;
    m_uiNumLimits = 0;
    pkJoint->resetLimitPlaneIterator();
    while (pkJoint->hasMoreLimitPlanes())
    {
        NxVec3 kDummy;
        NxReal fDummy;
        pkJoint->getNextLimitPlane(kDummy, fDummy);
        m_uiNumLimits++;
    }
    m_pkLimitPlaneNorms = NiAlloc(NxVec3, m_uiNumLimits);
    m_pkLimitPlaneDs = NiAlloc(NxReal, m_uiNumLimits);
    m_uiNumLimits = 0;
    pkJoint->resetLimitPlaneIterator();
    while (pkJoint->hasMoreLimitPlanes())
    {
        pkJoint->getNextLimitPlane(m_pkLimitPlaneNorms[m_uiNumLimits],
            m_pkLimitPlaneDs[m_uiNumLimits]);
        m_uiNumLimits++;
    }
}
//---------------------------------------------------------------------------
void NiPhysXJointDesc::ToJoint(NxJoint* pkJoint)
{
    // Always assume point is on actor 1. We have no way of finding otherwise.
    pkJoint->setLimitPoint(m_kLimitPoint);
    
    for (unsigned int i = 0; i < m_uiNumLimits; i++)
    {
        NxVec3 kPtOnPlane = -m_pkLimitPlaneDs[i] * m_pkLimitPlaneNorms[i];
        pkJoint->addLimitPlane(m_pkLimitPlaneNorms[i], kPtOnPlane);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXJointDesc);
//---------------------------------------------------------------------------
void NiPhysXJointDesc::CopyMembers(NiPhysXJointDesc* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    pkDest->m_eType = m_eType;

    // copy object names
    if (kCloning.m_eCopyType == NiObjectNET::COPY_EXACT)
    {
        pkDest->m_kJointName = m_kJointName;
    }
    else if (kCloning.m_eCopyType == NiObjectNET::COPY_UNIQUE)
    {
        if (m_kJointName)
        {
            int iQuantity = strlen(m_kJointName) + 2;
            char* pcNamePlus = NiAlloc(char, iQuantity);
            NIASSERT(pcNamePlus);
            NiStrcpy(pcNamePlus, iQuantity, m_kJointName);
            pcNamePlus[iQuantity - 2] = kCloning.m_cAppendChar;
            pcNamePlus[iQuantity - 1] = 0;
            pkDest->m_kJointName = pcNamePlus;
            NiFree(pcNamePlus);
        }
    }
    
    for (unsigned int ui = 0; ui < 2; ui++)
    {
        pkDest->m_aspActors[ui] = m_aspActors[ui];
        pkDest->m_akLocalNormal[ui] = m_akLocalNormal[ui];
        pkDest->m_akLocalAxis[ui] = m_akLocalAxis[ui];
        pkDest->m_akLocalAnchor[ui] = m_akLocalAnchor[ui];
    }
    
    pkDest->m_fMaxForce = m_fMaxForce;
    pkDest->m_fMaxTorque = m_fMaxTorque;
    pkDest->m_uiJointFlags = m_uiJointFlags;
    
    pkDest->m_kLimitPoint = m_kLimitPoint;
    pkDest->m_uiNumLimits = m_uiNumLimits;
    
    pkDest->m_pkLimitPlaneNorms = NiAlloc(NxVec3, m_uiNumLimits);
    pkDest->m_pkLimitPlaneDs = NiAlloc(NxReal, m_uiNumLimits);
    for (unsigned int ui = 0; ui < m_uiNumLimits; ui++)
    {
        pkDest->m_pkLimitPlaneNorms[ui] = m_pkLimitPlaneNorms[ui];
        pkDest->m_pkLimitPlaneDs[ui] = m_pkLimitPlaneDs[ui];
    }
}
//---------------------------------------------------------------------------
void NiPhysXJointDesc::ProcessClone(NiCloningProcess& kCloning)
{
    NiObject::ProcessClone(kCloning);
    
    bool bCloned;
    NiObject* pkCloneObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(this, pkCloneObj);
    if (!bCloned)
        return;
    NiPhysXJointDesc* pkClone = (NiPhysXJointDesc*) pkCloneObj;
            
    NiObject* pkClonedActorObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(m_aspActors[0], pkClonedActorObj);
    NiPhysXActorDesc* pkClonedActor = (NiPhysXActorDesc*) pkClonedActorObj;
    if (bCloned)
    {
       pkClone->m_aspActors[0] = pkClonedActor;
    }

    bCloned = kCloning.m_pkCloneMap->
        GetAt(m_aspActors[1], pkClonedActorObj);
    pkClonedActor = (NiPhysXActorDesc*) pkClonedActorObj;
    if (bCloned)
    {
       pkClone->m_aspActors[1] = pkClonedActor;
    }
    
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXJointDesc::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;
    
    kStream.RegisterFixedString(m_kJointName);

    if (m_aspActors[0])
        m_aspActors[0]->RegisterStreamables(kStream);
    if (m_aspActors[1])
        m_aspActors[1]->RegisterStreamables(kStream);
    
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXJointDesc::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXJointDesc::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    NiStreamSaveEnum(kStream, m_eType);
    kStream.SaveFixedString(m_kJointName);

    for (unsigned int i = 0; i < 2; i++)
    {
        kStream.SaveLinkID(m_aspActors[i]);
        NiPhysXTypes::NxVec3SaveBinary(kStream, m_akLocalNormal[i]);
        NiPhysXTypes::NxVec3SaveBinary(kStream, m_akLocalAxis[i]);
        NiPhysXTypes::NxVec3SaveBinary(kStream, m_akLocalAnchor[i]);
    }

    NiStreamSaveBinary(kStream, m_fMaxForce);
    NiStreamSaveBinary(kStream, m_fMaxTorque);
    NiStreamSaveBinary(kStream, m_uiJointFlags);

    NiPhysXTypes::NxVec3SaveBinary(kStream, m_kLimitPoint);
    NiStreamSaveBinary(kStream, m_uiNumLimits);
    for (unsigned int i = 0; i < m_uiNumLimits; i++)
    {
        NiPhysXTypes::NxVec3SaveBinary(kStream, m_pkLimitPlaneNorms[i]);
        NiStreamSaveBinary(kStream, m_pkLimitPlaneDs[i]);
    }
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXJointDesc);
//---------------------------------------------------------------------------
void NiPhysXJointDesc::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    NiStreamLoadEnum(kStream, m_eType);
    kStream.LoadFixedString(m_kJointName);

    for (unsigned int i = 0; i < 2; i++)
    {
        m_aspActors[i] = (NiPhysXActorDesc*)kStream.ResolveLinkID();
        
        NiPhysXTypes::NxVec3LoadBinary(kStream, m_akLocalNormal[i]);
        NiPhysXTypes::NxVec3LoadBinary(kStream, m_akLocalAxis[i]);
        NiPhysXTypes::NxVec3LoadBinary(kStream, m_akLocalAnchor[i]);
    }

    NiStreamLoadBinary(kStream, m_fMaxForce);
    NiStreamLoadBinary(kStream, m_fMaxTorque);
    NiStreamLoadBinary(kStream, m_uiJointFlags);
    
    NiPhysXTypes::NxVec3LoadBinary(kStream, m_kLimitPoint);
    NiStreamLoadBinary(kStream, m_uiNumLimits);
    for (unsigned int i = 0; i < m_uiNumLimits; i++)
    {
        NiPhysXTypes::NxVec3LoadBinary(kStream, m_pkLimitPlaneNorms[i]);
        NiStreamLoadBinary(kStream, m_pkLimitPlaneDs[i]);
    }
}
//---------------------------------------------------------------------------
void NiPhysXJointDesc::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXJointDesc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXJointDesc, pkObject));
    if(!NiObject::IsEqual(pkObject))
        return false;
        
    NiPhysXJointDesc* pkNewObject = (NiPhysXJointDesc*)pkObject;
    
    if (m_eType != pkNewObject->m_eType ||
        m_kJointName != pkNewObject->m_kJointName)
        return false;

    NiPoint3 kNewP;
    NiPoint3 kThisP;
    for (unsigned int i = 0; i < 2; i++)
    {
        if ((m_aspActors[i] && !pkNewObject->m_aspActors[i]) ||
            (!m_aspActors[i] && pkNewObject->m_aspActors[i]))
            return false;
            
        if (m_aspActors[i] &&
            (!m_aspActors[i]->IsEqual(pkNewObject->m_aspActors[i])))
        {
            return false;
        }
 
        NiPhysXTypes::NxVec3ToNiPoint3(pkNewObject->m_akLocalNormal[i], kNewP);
        NiPhysXTypes::NxVec3ToNiPoint3(m_akLocalNormal[i], kThisP);
        if (kNewP != kThisP)
            return false;

        NiPhysXTypes::NxVec3ToNiPoint3(pkNewObject->m_akLocalAxis[i], kNewP);
        NiPhysXTypes::NxVec3ToNiPoint3(m_akLocalAxis[i], kThisP);
        if (kNewP != kThisP)
            return false;

        NiPhysXTypes::NxVec3ToNiPoint3(pkNewObject->m_akLocalAnchor[i], kNewP);
        NiPhysXTypes::NxVec3ToNiPoint3(m_akLocalAnchor[i], kThisP);
        if (kNewP != kThisP)
            return false;
    }
       
    if (pkNewObject->m_fMaxForce != m_fMaxForce  ||
        pkNewObject->m_fMaxTorque != m_fMaxTorque ||
        pkNewObject->m_uiJointFlags != m_uiJointFlags)
        return false;

    NiPhysXTypes::NxVec3ToNiPoint3(pkNewObject->m_kLimitPoint, kNewP);
    NiPhysXTypes::NxVec3ToNiPoint3(m_kLimitPoint, kThisP);
    if (kNewP != kThisP)
        return false;
    
    if (m_uiNumLimits != pkNewObject->m_uiNumLimits)
        return false;

    for (unsigned int i = 0; i < m_uiNumLimits; i++)
    {
        NiPhysXTypes::NxVec3ToNiPoint3(pkNewObject->m_pkLimitPlaneNorms[i],
            kNewP);
        NiPhysXTypes::NxVec3ToNiPoint3(m_pkLimitPlaneNorms[i], kThisP);
        if (kNewP != kThisP)
            return false;
        if (m_pkLimitPlaneDs[i] != pkNewObject->m_pkLimitPlaneDs[i])
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
