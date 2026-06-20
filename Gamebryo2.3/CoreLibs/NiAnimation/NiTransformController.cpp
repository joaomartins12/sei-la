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
#include "NiAnimationPCH.h"

#include "NiTransformController.h"
#include <NiAVObject.h>
#include "NiBlendTransformInterpolator.h"
#include "NiBlendAccumTransformInterpolator.h"
#include "NiTransformData.h"
#include "NiTransformInterpolator.h"

NiImplementRTTI(NiTransformController, NiSingleInterpController);

//---------------------------------------------------------------------------
NiTransformController::NiTransformController() 
{
}
//---------------------------------------------------------------------------
NiInterpolator* NiTransformController::CreatePoseInterpolator(
    unsigned short usIndex)
{
    NIASSERT(usIndex == 0);
    NiAVObject* pkTarget = (NiAVObject*) m_pkTarget;
    NiQuaternion kRotate;
    pkTarget->GetRotate(kRotate);
    NiQuatTransform kTransform(pkTarget->GetTranslate(), kRotate,
        pkTarget->GetScale());
    return NiNew NiTransformInterpolator(kTransform);
}
//---------------------------------------------------------------------------
void NiTransformController::SynchronizePoseInterpolator(NiInterpolator* 
    pkInterp, unsigned short usIndex)
{
    NiTransformInterpolator* pkTransformInterp = 
        NiDynamicCast(NiTransformInterpolator, pkInterp);

    NIASSERT(usIndex == 0);
    NIASSERT(pkTransformInterp);
    NiAVObject* pkTarget = (NiAVObject*) m_pkTarget;
    NiQuaternion kRotate;
    pkTarget->GetRotate(kRotate);
    NiQuatTransform kTransform(pkTarget->GetTranslate(), kRotate,
        pkTarget->GetScale());
    pkTransformInterp->SetPoseValue(kTransform);
}   
//---------------------------------------------------------------------------
NiBlendInterpolator* NiTransformController::CreateBlendInterpolator(
    unsigned short usIndex, bool bManagerControlled,
    bool bAccumulateAnimations, float fWeightThreshold,
    unsigned char ucArraySize) const
{
    NIASSERT(usIndex == 0);
    if (bAccumulateAnimations)
    {
        return NiNew NiBlendAccumTransformInterpolator(bManagerControlled, 
            fWeightThreshold, ucArraySize);
    }
    else
    {
        return NiNew NiBlendTransformInterpolator(bManagerControlled, 
            fWeightThreshold, ucArraySize);
    }
}
//---------------------------------------------------------------------------
void NiTransformController::Update(float fTime)
{
    if (GetManagerControlled())
    {
        m_fScaledTime = INVALID_TIME;
    }
    else if (DontDoUpdate(fTime) &&
        (!m_spInterpolator || !m_spInterpolator->AlwaysUpdate()))
    {
        return;
    }

    if (m_spInterpolator)
    {
        NiQuatTransform kTransform;
        if (m_spInterpolator->Update(m_fScaledTime, m_pkTarget, kTransform))
        {
            NiAVObject* pkTarget = (NiAVObject*) m_pkTarget;
            if (kTransform.IsTranslateValid())
            {
                pkTarget->SetTranslate(kTransform.GetTranslate());
            }
            if (kTransform.IsRotateValid())
            {
                pkTarget->SetRotate(kTransform.GetRotate());
            }
            if (kTransform.IsScaleValid())
            {
                pkTarget->SetScale(kTransform.GetScale());
            }
        }   
    }
}
//---------------------------------------------------------------------------
bool NiTransformController::InterpTargetIsCorrectType(NiObjectNET* pkTarget)
    const
{
    return NiIsKindOf(NiAVObject, pkTarget);
}
//---------------------------------------------------------------------------
bool NiTransformController::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator, unsigned short usIndex) const
{
    NIASSERT(usIndex == 0);
    return pkInterpolator->IsTransformValueSupported();
}
//---------------------------------------------------------------------------
bool NiTransformController::IsTransformController() const
{
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiTransformController);
//---------------------------------------------------------------------------
void NiTransformController::CopyMembers(NiTransformController* pkDest,
    NiCloningProcess& kCloning)
{
    NiSingleInterpController::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiTransformController);
//---------------------------------------------------------------------------
void NiTransformController::LoadBinary(NiStream& kStream)
{
    NiSingleInterpController::LoadBinary(kStream);

    // This code will replace the streaming code for NiKeyframeController.
    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        kStream.ReadLinkID();   // m_spKeyframeData
    }
}
//---------------------------------------------------------------------------
void NiTransformController::LinkObject(NiStream& kStream)
{
    NiSingleInterpController::LinkObject(kStream);

    // This code will replace the streaming code for NiKeyframeController.
    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        // link the shared data
        NiTransformData* pkKeyframeData = 
            (NiTransformData*) kStream.GetObjectFromLinkID();
        m_spInterpolator = NiNew NiTransformInterpolator(pkKeyframeData);
        m_spInterpolator->Collapse();
    }
}
//---------------------------------------------------------------------------
bool NiTransformController::RegisterStreamables(NiStream& kStream)
{
    return NiSingleInterpController::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiTransformController::SaveBinary(NiStream& kStream)
{
    NiSingleInterpController::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiTransformController::IsEqual(NiObject* pkObject)
{
    if (!NiSingleInterpController::IsEqual(pkObject))
        return false;

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiTransformController::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiSingleInterpController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiTransformController::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
