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

#include "NiMultiTargetTransformController.h"
#include "NiControllerSequence.h"
#include "NiTransformInterpolator.h"
#include <NiNode.h>
#include <NiTransformController.h>
#include <NiCloningProcess.h>

NiImplementRTTI(NiMultiTargetTransformController, NiInterpController);

//---------------------------------------------------------------------------
NiMultiTargetTransformController::~NiMultiTargetTransformController()
{
#ifdef _DEBUG
    for (unsigned short us = 0; us < m_usNumInterps; us++)
    {
        // Ensure that we are the only object referencing each blend
        // interpolator before deleting the array.
        NIASSERT(m_pkBlendInterps[us].GetRefCount() == 1);
    }
#endif // _DEBUG

   NiDelete[] m_pkBlendInterps;
   NiFree(m_ppkTargets);
}
//---------------------------------------------------------------------------
void NiMultiTargetTransformController::Update(float fTime)
{
    NiQuatTransform kTransform;
    for (unsigned short us = 0; us < m_usNumInterps; us++)
    {
        // We need to check the UpdateSelected flag before updating the
        // interpolator. For instance, BoneLOD might have turned off that
        // bone.
        if (m_ppkTargets[us] == NULL ||
            !m_ppkTargets[us]->GetSelectiveUpdate())
        {
            continue;
        }

        if (m_pkBlendInterps[us].Update(fTime, m_ppkTargets[us], kTransform))
        {
            if (kTransform.IsTranslateValid())
            {
                m_ppkTargets[us]->SetTranslate(kTransform.GetTranslate());
            }
            if (kTransform.IsRotateValid())
            {
                m_ppkTargets[us]->SetRotate(kTransform.GetRotate());
            }
            if (kTransform.IsScaleValid())
            {
                m_ppkTargets[us]->SetScale(kTransform.GetScale());
            }
        }
    }
}
//---------------------------------------------------------------------------
unsigned short NiMultiTargetTransformController::GetInterpolatorCount() const
{
    return m_usNumInterps;
}
//---------------------------------------------------------------------------
const char* NiMultiTargetTransformController::GetInterpolatorID(
    unsigned short usIndex)
{
    NIASSERT(usIndex < m_usNumInterps);
    return m_ppkTargets[usIndex]->GetName();
}
//---------------------------------------------------------------------------
unsigned short NiMultiTargetTransformController::GetInterpolatorIndex(
    const char* pcID) const
{
    if (pcID == NULL)
    {
        return INVALID_INDEX;
    }

    for (unsigned short us = 0; us < m_usNumInterps; us++)
    {
        if (m_ppkTargets[us] != NULL && 
            strcmp(m_ppkTargets[us]->GetName(), pcID) == 0)
        {
            return us;
        }
    }

    return INVALID_INDEX;
}
//---------------------------------------------------------------------------
NiInterpolator* NiMultiTargetTransformController::GetInterpolator(
    unsigned short usIndex) const
{
    return &m_pkBlendInterps[usIndex];
}
//---------------------------------------------------------------------------
void NiMultiTargetTransformController::SetInterpolator(
    NiInterpolator* pkInterpolator, unsigned short usIndex)
{
    // SetInterpolator has no meaning for NiMultiTargetTransformControllers.
    // Do nothing.
}
//---------------------------------------------------------------------------
void NiMultiTargetTransformController::ResetTimeExtrema()
{
    if (GetManagerControlled())
    {
        // Do nothing if this controller is being controlled by a manager.
        return;
    }

    m_fLoKeyTime = NI_INFINITY;
    m_fHiKeyTime = -NI_INFINITY;

    for (unsigned short us = 0; us < m_usNumInterps; us++)
    {
        float fHi = -NI_INFINITY;
        float fLo = NI_INFINITY;
        m_pkBlendInterps[us].GetActiveTimeRange(fLo, fHi);

        if (fLo < m_fLoKeyTime)
        {
            m_fLoKeyTime = fLo;
        }
        if (fHi > m_fHiKeyTime)
        {
            m_fHiKeyTime = fHi;
        }
    }

    if (m_fLoKeyTime == NI_INFINITY && m_fHiKeyTime == -NI_INFINITY)
    {
        NiInterpController::ResetTimeExtrema();
    }
}
//---------------------------------------------------------------------------
void NiMultiTargetTransformController::AllocateStorage(unsigned short usCount)
{
   m_usNumInterps = usCount;
   if (usCount == 0)
   {
       return;
   }

   m_pkBlendInterps = NiNew NiBlendTransformInterpolator[usCount];
   for (unsigned short us = 0; us < usCount; us++)
   {
       // Increment the ref count here to prevent premature deletion.
        m_pkBlendInterps[us].IncRefCount();
   }

   m_ppkTargets = NiAlloc(NiAVObject*,usCount);
   memset(m_ppkTargets, 0, usCount * sizeof(NiAVObject*));
}
//---------------------------------------------------------------------------
NiInterpolator* NiMultiTargetTransformController::CreatePoseInterpolator(
    unsigned short usIndex)
{
    NIASSERT(usIndex < m_usNumInterps);
    NiAVObject* pkTarget = m_ppkTargets[usIndex];
    NiQuaternion kRotate;
    pkTarget->GetRotate(kRotate);
    NiQuatTransform kTransform(pkTarget->GetTranslate(), kRotate,
        pkTarget->GetScale());
    return NiNew NiTransformInterpolator(kTransform);
}
//---------------------------------------------------------------------------
void NiMultiTargetTransformController::SynchronizePoseInterpolator(
    NiInterpolator* pkMultiTransform, unsigned short usIndex)
{
    // SynchronizePoseInterpolator doesn't make sense for an
    // NiMultiTargetTransformController because it only holds
    // NiBlendTransformInterpolators, which cannot be posed. Do nothing.
}
//---------------------------------------------------------------------------
NiBlendInterpolator* NiMultiTargetTransformController::
    CreateBlendInterpolator(unsigned short usIndex, bool bManagerControlled, 
    bool bAccumulateAnimations, float fWeightThreshold,
    unsigned char ucArraySize) const
{
    return &m_pkBlendInterps[usIndex];
}
//---------------------------------------------------------------------------
void NiMultiTargetTransformController::GuaranteeTimeRange(float fStartTime,
    float fEndTime)
{
    for (unsigned short us = 0; us < m_usNumInterps; us++)
    {
        m_pkBlendInterps[us].GuaranteeTimeRange(fStartTime, fEndTime);
    }
}
//---------------------------------------------------------------------------
bool NiMultiTargetTransformController::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator, unsigned short usIndex) const
{
    return pkInterpolator->IsTransformValueSupported();
}
//---------------------------------------------------------------------------
bool NiMultiTargetTransformController::TargetIsRequiredType() const
{
    return NiIsKindOf(NiAVObject, m_pkTarget);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiMultiTargetTransformController);
//---------------------------------------------------------------------------
void NiMultiTargetTransformController::CopyMembers(
    NiMultiTargetTransformController* pkDest, NiCloningProcess& kCloning)
{
    NiInterpController::CopyMembers(pkDest, kCloning);

    pkDest->AllocateStorage(m_usNumInterps);
    for (unsigned short us = 0; us < m_usNumInterps; us++)
    {
        m_pkBlendInterps[us].CreateCloneInPlace(
            &pkDest->m_pkBlendInterps[us], kCloning);
    }
    
}
//---------------------------------------------------------------------------
void NiMultiTargetTransformController::ProcessClone(NiCloningProcess& 
    kCloning)
{
    NiInterpController::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);
    NiMultiTargetTransformController* pkDest = 
        (NiMultiTargetTransformController*) pkClone;

    for (unsigned short us = 0; us < m_usNumInterps; us++)
    {
        m_pkBlendInterps[us].ProcessClone(kCloning);

        if (m_ppkTargets[us] != NULL)
        {
            bCloned = kCloning.m_pkCloneMap->GetAt(m_ppkTargets[us], pkClone);
            NIASSERT(bCloned);
            if (bCloned)
                pkDest->m_ppkTargets[us] = (NiAVObject*) pkClone;
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiMultiTargetTransformController);
//---------------------------------------------------------------------------
void NiMultiTargetTransformController::LoadBinary(NiStream& kStream)
{
    NiInterpController::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_usNumInterps);
    AllocateStorage(m_usNumInterps);
    
    for (unsigned short us = 0; us < m_usNumInterps; us++)
    {
        m_ppkTargets[us] = (NiAVObject*) kStream.ResolveLinkID();
    }
}
//---------------------------------------------------------------------------
void NiMultiTargetTransformController::LinkObject(NiStream& kStream)
{
    NiInterpController::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiMultiTargetTransformController::RegisterStreamables(NiStream& kStream)
{
    return NiInterpController::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiMultiTargetTransformController::SaveBinary(NiStream& kStream)
{
    NiInterpController::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_usNumInterps);
    
    for (unsigned short us = 0; us < m_usNumInterps; us++)
    {
        kStream.SaveLinkID(m_ppkTargets[us]);
    }
}
//---------------------------------------------------------------------------
bool NiMultiTargetTransformController::IsEqual(NiObject* pkObject)
{
    if (!NiInterpController::IsEqual(pkObject))
    {
        return false;
    }

    NiMultiTargetTransformController* pkDest =
        (NiMultiTargetTransformController*) pkObject;

    if (pkDest->m_usNumInterps != m_usNumInterps)
    {
        return false;
    }

    for (unsigned short us = 0; us < m_usNumInterps; us++)
    {
        if ((m_ppkTargets[us] && !pkDest->m_ppkTargets[us]) ||
            (!m_ppkTargets[us] && pkDest->m_ppkTargets[us]) ||
            (m_ppkTargets[us] && !pkDest->m_ppkTargets[us]->IsEqual(
                m_ppkTargets[us])))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiMultiTargetTransformController::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiInterpController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiMultiTargetTransformController::ms_RTTI
        .GetName()));
    
    pkStrings->Add(NiGetViewerString("m_usNumInterps", m_usNumInterps));

    char acMsg[1024];
    for (unsigned short us = 0; us < m_usNumInterps; us++)
    {
        if (m_ppkTargets[us] != NULL)
        {
            NiSprintf(acMsg, 1024, "Target%d", us);
            pkStrings->Add(NiGetViewerString(acMsg, m_ppkTargets[us]));
        }
    }
}
//---------------------------------------------------------------------------
