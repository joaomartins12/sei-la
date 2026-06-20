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
#include "NiPhysXPCH.h"

#include "NiPhysX.h"

NiImplementRTTI(NiPhysXInterpolatorDest, NiPhysXRigidBodyDest);

//---------------------------------------------------------------------------
NiPhysXInterpolatorDest::NiPhysXInterpolatorDest()
{
    m_pkTarget = 0;
    m_pkSceneParent = 0;
}
//---------------------------------------------------------------------------
NiPhysXInterpolatorDest::NiPhysXInterpolatorDest(
    NiInterpolator* pkTarget, NiNode* pkSceneParent,
    NxActor *pkActor, NxActor* pkActorParent)
    : NiPhysXRigidBodyDest(pkActor, pkActorParent)
{
    m_pkTarget = pkTarget;
    m_pkSceneParent = pkSceneParent;
}
//---------------------------------------------------------------------------
NiPhysXInterpolatorDest::~NiPhysXInterpolatorDest()
{
    m_pkTarget = 0;
    m_pkSceneParent = 0;
}
//---------------------------------------------------------------------------
void NiPhysXInterpolatorDest::UpdateSceneGraph(const float fT,
    const NiTransform& kRootTransform, const float fScalePToW,
    const bool bForce)
{
    if (!m_bActive)
        return;
    
    NiMatrix3 kRotate;
    NiPoint3 kTranslate;
    GetTransforms(fT, kRotate, kTranslate, kRootTransform);
    NiQuaternion kRotateQuat;
    kRotateQuat.FromRotation(kRotate);

    if (NiIsKindOf(NiTransformInterpolator, m_pkTarget))
    {
        NiTransformInterpolator* pkTarget =
            (NiTransformInterpolator*)m_pkTarget;
        pkTarget->SetPoseRotate(kRotateQuat);
        pkTarget->SetPoseTranslate(kTranslate);
    }
    else if (NiIsKindOf(NiBSplineTransformInterpolator, m_pkTarget))
    {
        NiBSplineTransformInterpolator* pkTarget =
            (NiBSplineTransformInterpolator*)m_pkTarget;
        pkTarget->SetPoseRotate(kRotateQuat);
        pkTarget->SetPoseTranslate(kTranslate);
    }
    else
    {
        // You hit this if you passed in aninterpolator that does not
        // support SetPose functions.
        NIASSERT(false && "Invalid interpolator type");
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXInterpolatorDest);
//---------------------------------------------------------------------------
void NiPhysXInterpolatorDest::CopyMembers(NiPhysXInterpolatorDest* pkDest,
    NiCloningProcess& kCloning)
{
    NiPhysXRigidBodyDest::CopyMembers(pkDest, kCloning);

    pkDest->m_pkTarget = m_pkTarget;
    pkDest->m_pkSceneParent = m_pkSceneParent;
}
//---------------------------------------------------------------------------
void NiPhysXInterpolatorDest::ProcessClone(NiCloningProcess& kCloning)
{
    NiPhysXRigidBodyDest::ProcessClone(kCloning);
    
    bool bCloned;
    NiObject* pkCloneObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(this, pkCloneObj);
    if (!bCloned)
        return;
    NiPhysXInterpolatorDest* pkClone = (NiPhysXInterpolatorDest*)pkCloneObj;

    NiObject* pkClonedTargetObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(m_pkTarget, pkClonedTargetObj);
    NiInterpolator* pkClonedTarget = (NiInterpolator*)pkClonedTargetObj;
    if (bCloned)
    {
        pkClone->m_pkTarget = pkClonedTarget;
    }

    NiObject* pkClonedParentObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(m_pkSceneParent, pkClonedParentObj);
    NiNode* pkClonedParent = (NiNode*)pkClonedParentObj;
    if (bCloned)
    {
        pkClone->m_pkSceneParent = pkClonedParent;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXInterpolatorDest::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXRigidBodyDest::RegisterStreamables(kStream))
        return false;

    m_pkTarget->RegisterStreamables(kStream);
    m_pkSceneParent->RegisterStreamables(kStream);
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXInterpolatorDest::SaveBinary(NiStream& kStream)
{
    NiPhysXRigidBodyDest::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkTarget);
    kStream.SaveLinkID(m_pkSceneParent);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXInterpolatorDest);
//---------------------------------------------------------------------------
void NiPhysXInterpolatorDest::LoadBinary(NiStream& kStream)
{
    NiPhysXRigidBodyDest::LoadBinary(kStream);

    m_pkTarget = (NiInterpolator*)kStream.ResolveLinkID();
    m_pkSceneParent = (NiNode*)kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPhysXInterpolatorDest::LinkObject(NiStream& kStream)
{
    NiPhysXRigidBodyDest::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXInterpolatorDest::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXInterpolatorDest, pkObject));
    if(!NiPhysXRigidBodyDest::IsEqual(pkObject))
        return false;
        
    NiPhysXInterpolatorDest* pkNewObject = (NiPhysXInterpolatorDest*)pkObject;

    if (!m_pkTarget->IsEqual(pkNewObject->m_pkTarget))
        return false;
    
    if (!m_pkSceneParent->IsEqual(pkNewObject->m_pkSceneParent))
        return false;

    return true;
}
//---------------------------------------------------------------------------
