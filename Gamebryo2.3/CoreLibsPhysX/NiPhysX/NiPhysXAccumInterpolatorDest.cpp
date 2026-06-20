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

NiImplementRTTI(NiPhysXAccumInterpolatorDest, NiPhysXAccumRigidBodyDest);

//---------------------------------------------------------------------------
NiPhysXAccumInterpolatorDest::NiPhysXAccumInterpolatorDest()
{
    m_pkAccumTarget = 0;
    m_pkNonAccumTarget = 0;
    m_pkSceneParent = 0;
}
//---------------------------------------------------------------------------
NiPhysXAccumInterpolatorDest::NiPhysXAccumInterpolatorDest(
    NiInterpolator* pkAccumTarget, NiInterpolator* pkNonAccumTarget,
    NiBlendAccumTransformInterpolator* pkBlendInterplator,
    NiNode* pkSceneParent, NxActor *pkActor, NxActor* pkActorParent)
    : NiPhysXAccumRigidBodyDest(pkActor, pkActorParent)
{
    m_pkAccumTarget = pkAccumTarget;
    m_pkNonAccumTarget = pkNonAccumTarget;
    m_pkBlendInterp = pkBlendInterplator;
    m_pkSceneParent = pkSceneParent;
}
//---------------------------------------------------------------------------
NiPhysXAccumInterpolatorDest::~NiPhysXAccumInterpolatorDest()
{
    m_pkAccumTarget = 0;
    m_pkNonAccumTarget = 0;
    m_pkBlendInterp = 0;
    m_pkSceneParent = 0;
}
//---------------------------------------------------------------------------
void NiPhysXAccumInterpolatorDest::UpdateSceneGraph(const float fT,
    const NiTransform& kRootTransform, const float fScalePToW,
    const bool bForce)
{
    if (!m_bActive)
        return;
    
    NiQuatTransform m_kInverseAccum;
    if (m_pkBlendInterp)
    {
        NiQuatTransform kAccumTransform =
            m_pkBlendInterp->GetAccumulatedTransformValue();
        if (!kAccumTransform.IsTransformInvalid())
        {
            kAccumTransform.HierInvert(m_kInverseAccum);
        }
        else
        {
            m_kInverseAccum.SetTranslate(NiPoint3::ZERO);
            m_kInverseAccum.SetRotate(NiQuaternion::IDENTITY);
            m_kInverseAccum.SetScale(1.0f);
        }
    }
    else
    {
        m_kInverseAccum.SetTranslate(NiPoint3::ZERO);
        m_kInverseAccum.SetRotate(NiQuaternion::IDENTITY);
        m_kInverseAccum.SetScale(1.0f);
    }

    NiQuaternion kAccumRotate;
    NiPoint3 kAccumTranslate;
    NiQuaternion kNonAccumRotate;
    NiPoint3 kNonAccumTranslate;
    GetAccumTransforms(fT, kAccumRotate, kAccumTranslate, kNonAccumRotate,
        kNonAccumTranslate, kRootTransform);

    NiQuatTransform kAccumLocal(kAccumTranslate, kAccumRotate, 1.0f);
    kAccumLocal = m_kInverseAccum.HierApply(kAccumLocal);
        
    if (NiIsKindOf(NiTransformInterpolator, m_pkAccumTarget))
    {
        NiTransformInterpolator* pkTarget =
            (NiTransformInterpolator*)m_pkAccumTarget;
        pkTarget->SetPoseRotate(kAccumLocal.GetRotate());
        pkTarget->SetPoseTranslate(kAccumLocal.GetTranslate());
    }
    else if (NiIsKindOf(NiBSplineTransformInterpolator, m_pkAccumTarget))
    {
        NiBSplineTransformInterpolator* pkTarget =
            (NiBSplineTransformInterpolator*)m_pkAccumTarget;
        pkTarget->SetPoseRotate(kAccumLocal.GetRotate());
        pkTarget->SetPoseTranslate(kAccumLocal.GetTranslate());
    }
    else
    {
        // You hit this if you passed in an interpolator that does not
        // support SetPose functions.
        NIASSERT(false && "Invalid interpolator type");
    }

    if (NiIsKindOf(NiTransformInterpolator, m_pkNonAccumTarget))
    {
        NiTransformInterpolator* pkTarget =
            (NiTransformInterpolator*)m_pkNonAccumTarget;
        pkTarget->SetPoseRotate(kNonAccumRotate);
        pkTarget->SetPoseTranslate(kNonAccumTranslate);
    }
    else if (NiIsKindOf(NiBSplineTransformInterpolator,
        m_pkNonAccumTarget))
    {
        NiBSplineTransformInterpolator* pkTarget =
            (NiBSplineTransformInterpolator*)m_pkNonAccumTarget;
        pkTarget->SetPoseRotate(kNonAccumRotate);
        pkTarget->SetPoseTranslate(kNonAccumTranslate);
    }
    else
    {
        // You hit this if you passed in an interpolator that does not
        // support SetPose functions.
        NIASSERT(false && "Invalid interpolator type");
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXAccumInterpolatorDest);
//---------------------------------------------------------------------------
void NiPhysXAccumInterpolatorDest::CopyMembers(
    NiPhysXAccumInterpolatorDest* pkDest, NiCloningProcess& kCloning)
{
    NiPhysXAccumRigidBodyDest::CopyMembers(pkDest, kCloning);

    pkDest->m_pkAccumTarget = m_pkAccumTarget;
    pkDest->m_pkNonAccumTarget = m_pkNonAccumTarget;
    pkDest->m_pkBlendInterp = m_pkBlendInterp;
    pkDest->m_pkSceneParent = m_pkSceneParent;
}
//---------------------------------------------------------------------------
void NiPhysXAccumInterpolatorDest::ProcessClone(NiCloningProcess& kCloning)
{
    NiPhysXAccumRigidBodyDest::ProcessClone(kCloning);
    
    bool bCloned;
    NiObject* pkCloneObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(this, pkCloneObj);
    if (!bCloned)
        return;
    NiPhysXAccumInterpolatorDest* pkClone =
        (NiPhysXAccumInterpolatorDest*)pkCloneObj;

    NiObject* pkClonedTargetObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(m_pkAccumTarget, pkClonedTargetObj);
    NiInterpolator* pkClonedTarget = (NiInterpolator*)pkClonedTargetObj;
    if (bCloned)
    {
        pkClone->m_pkAccumTarget = pkClonedTarget;
    }

    pkClonedTargetObj = 0;
    bCloned =
        kCloning.m_pkCloneMap->GetAt(m_pkNonAccumTarget, pkClonedTargetObj);
    pkClonedTarget = (NiInterpolator*)pkClonedTargetObj;
    if (bCloned)
    {
        pkClone->m_pkNonAccumTarget = pkClonedTarget;
    }

    pkClonedTargetObj = 0;
    bCloned =
        kCloning.m_pkCloneMap->GetAt(m_pkBlendInterp, pkClonedTargetObj);
    NiBlendAccumTransformInterpolator* pkClonedBlend =
        (NiBlendAccumTransformInterpolator*)pkClonedTargetObj;
    if (bCloned)
    {
        pkClone->m_pkBlendInterp = pkClonedBlend;
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
bool NiPhysXAccumInterpolatorDest::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXAccumRigidBodyDest::RegisterStreamables(kStream))
        return false;

    m_pkAccumTarget->RegisterStreamables(kStream);
    m_pkNonAccumTarget->RegisterStreamables(kStream);
    m_pkBlendInterp->RegisterStreamables(kStream);
    m_pkSceneParent->RegisterStreamables(kStream);
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXAccumInterpolatorDest::SaveBinary(NiStream& kStream)
{
    NiPhysXAccumRigidBodyDest::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkAccumTarget);
    kStream.SaveLinkID(m_pkNonAccumTarget);
    kStream.SaveLinkID(m_pkBlendInterp);
    kStream.SaveLinkID(m_pkSceneParent);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXAccumInterpolatorDest);
//---------------------------------------------------------------------------
void NiPhysXAccumInterpolatorDest::LoadBinary(NiStream& kStream)
{
    NiPhysXAccumRigidBodyDest::LoadBinary(kStream);

    m_pkAccumTarget = (NiInterpolator*)kStream.ResolveLinkID();
    m_pkNonAccumTarget = (NiInterpolator*)kStream.ResolveLinkID();
    m_pkBlendInterp =
        (NiBlendAccumTransformInterpolator*)kStream.ResolveLinkID();
    m_pkSceneParent = (NiNode*)kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPhysXAccumInterpolatorDest::LinkObject(NiStream& kStream)
{
    NiPhysXAccumRigidBodyDest::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXAccumInterpolatorDest::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXAccumInterpolatorDest, pkObject));
    if(!NiPhysXAccumRigidBodyDest::IsEqual(pkObject))
        return false;
        
    NiPhysXAccumInterpolatorDest* pkNewObject =
        (NiPhysXAccumInterpolatorDest*)pkObject;

    if (!m_pkAccumTarget->IsEqual(pkNewObject->m_pkAccumTarget))
        return false;
    
    if (!m_pkNonAccumTarget->IsEqual(pkNewObject->m_pkNonAccumTarget))
        return false;
    
    if (!m_pkBlendInterp->IsEqual(pkNewObject->m_pkBlendInterp))
        return false;
    
    if (!m_pkSceneParent->IsEqual(pkNewObject->m_pkSceneParent))
        return false;

    return true;
}
//---------------------------------------------------------------------------
