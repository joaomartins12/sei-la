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

#include "NiAnimationMetrics.h"
#include <NiTriShape.h>

#include "NiBlendFloatInterpolator.h"
#include "NiFloatInterpolator.h"
#include "NiFloatKey.h"
#include "NiGeomMorpherController.h"
#include "NiGeomMorpherUpdateTask.h"
#include "NiInterpolator.h"
#include "NiParallelUpdateTaskManager.h"
#include "NiSpline.h"

NiImplementRTTI(NiGeomMorpherController, NiInterpController);

bool NiGeomMorpherController::ms_bAlwaysUpdateClone = false;

bool NiGeomMorpherController::ms_bParallelUpdateEnabled = false;
NiTaskManager::TaskPriority 
    NiGeomMorpherController::ms_eParallelUpdatePriority = 
    NiTaskManager::MEDIUM;
//---------------------------------------------------------------------------
NiGeomMorpherController::NiGeomMorpherController(NiMorphData* pkData)
    :
    m_spMorphData(pkData), m_pkInterpItems(NULL)
{
    m_uFlags = 0;
    if (pkData)
    {
        AllocateInterpArray();
    }

    m_bNeedPreDisplay = false;
    m_bIgnoreBaseTarget = false;
    m_bAlwaysUpdate = false;
    m_bBoundCalculated = false;
}
//---------------------------------------------------------------------------
NiGeomMorpherController::~NiGeomMorpherController()
{
    m_spMorphData = 0;
    NiDelete[] m_pkInterpItems;
}
//---------------------------------------------------------------------------
void NiGeomMorpherController::ReplaceTargets(
    NiMorphData::MorphTarget* pkMorphTargets, unsigned int uiNumTargets, 
    unsigned int uiNumVertsPerTarget, bool bUpdate)
{
    if (!m_spMorphData)
        m_spMorphData = NiNew NiMorphData;

    m_spMorphData->ReplaceTargets(pkMorphTargets, uiNumTargets, 
        uiNumVertsPerTarget);

    AllocateInterpArray();

    if (bUpdate && m_pkTarget)
    {
        CalculateMorphBound();
    }
}
//---------------------------------------------------------------------------
void NiGeomMorpherController::ResetTimeExtrema()
{
    if (GetManagerControlled())
    {
        // Do nothing if this controller is being controlled by a manager.
        return;
    }

    unsigned int uiNumTargets = GetNumTargets();

    m_fLoKeyTime = NI_INFINITY;
    m_fHiKeyTime = -NI_INFINITY;

    for (unsigned int uiI = 0; uiI < uiNumTargets; uiI++)
    {
        NiInterpolator* pkInterp = GetInterpolator(uiI);
        if (pkInterp)
        {
            float fHi = -NI_INFINITY;
            float fLo = NI_INFINITY;
            pkInterp->GetActiveTimeRange(fLo, fHi);
            
            if (fLo < m_fLoKeyTime)
                m_fLoKeyTime = fLo;
            if (fHi > m_fHiKeyTime)
                m_fHiKeyTime = fHi;
        }
    }
}
//---------------------------------------------------------------------------
void NiGeomMorpherController::Update(float fTime) 
{
    if (!m_spMorphData)
    {
        return;
    }

    if (GetManagerControlled())
    {
        m_bNeedPreDisplay = true;
        m_fScaledTime = INVALID_TIME;
    }
    else if(!DontDoUpdate(fTime) || m_bAlwaysUpdate)
    {
        m_bNeedPreDisplay = true;
    }

    if (m_bNeedPreDisplay)
    {
        // If the manager is active and the object has a ref count of 1, we
        // will update in the background. The ref count of 1 check is an
        // over-conservative check to see if the object is cloned or not.
        if (GetParallelUpdateEnabled() &&
            NiParallelUpdateTaskManager::IsActive() &&
            ((NiGeometry*)m_pkTarget)->GetModelData()->GetRefCount() == 1)
        {
            NiGeomMorpherUpdateTask* pkTask = 
                NiGeomMorpherUpdateTask::GetFreeObject();

            if (pkTask)
            {
                pkTask->SetController(this);
                if (NiParallelUpdateTaskManager::Get()->AddTask(pkTask,
                    ms_eParallelUpdatePriority))
                {
                    return;
                }

                // Failed to add task, clean up task and fall through
                pkTask->Clear();
            }
        }
    }

}
//---------------------------------------------------------------------------
void NiGeomMorpherController::GenMorphInterp(float fTime)
{
    NIASSERT(m_pkTarget);

    NiPoint3* pkVerts = GetVertexData();

    unsigned int uiNumTargets = m_spMorphData->GetNumTargets();
    int iVertexDataArraySize = ((NiGeometry*)m_pkTarget)->GetVertexCount();

    unsigned int uiI;
    for (uiI = 0; uiI < uiNumTargets; uiI++)
    {
        NiInterpolator* pkInterp = GetInterpolator(uiI);
        
        float fFrac = GetWeight(uiI);
        if (uiI == 0 && m_spMorphData->GetRelativeTargets())
        {
            fFrac = (m_bIgnoreBaseTarget ? 0.0f : 1.0f);
        }
        else if (pkInterp)
        {
            if (!pkInterp->Update(m_fScaledTime, m_pkTarget, fFrac))
            {
                // If the interpolator update failed for whatever reason,
                // leave the target weight alone.
                continue;
            }
        }
        else
        {
            continue;
        }

        SetWeight(uiI, fFrac);
    }

    if (!m_bIgnoreBaseTarget)
    {
        memset(pkVerts, 0, iVertexDataArraySize*sizeof(NiPoint3));
    }

    for (uiI = 0; uiI < uiNumTargets; uiI++)
    {
        float fFrac = GetWeight(uiI);
        if (fFrac >= 0.001f || fFrac <= -0.001f)
        {
            NiPoint3* pkDeltaVerts = m_spMorphData->GetTargetVerts(uiI);
            NiPoint3::PointsPlusEqualFloatTimesPoints(pkVerts, fFrac,
                pkDeltaVerts, iVertexDataArraySize);
        }
    }
}
//---------------------------------------------------------------------------
void NiGeomMorpherController::SetTarget(NiObjectNET* pkTarget)
{
    if (NiIsKindOf(NiGeometry, pkTarget))
    {
        NiTimeController::SetTarget(pkTarget);
        if (m_spMorphData)
        {
            CalculateMorphBound();
            m_bBoundCalculated = true;
        }
    }
    else
    {
        NiTimeController::SetTarget(NULL);
    }
}
//---------------------------------------------------------------------------
void NiGeomMorpherController::OnPreDisplay()
{
    NIMETRICS_ANIMATION_SCOPETIMER(MORPH_TIME);

    if (m_pkTarget && m_bNeedPreDisplay)
    {
        GenMorphInterp(m_fScaledTime);

        NiGeometryData* pkData = ((NiGeometry*) m_pkTarget)->GetModelData();
        pkData->MarkAsChanged(NiGeometryData::VERTEX_MASK);
        
        // If the base has normals and the application wants them 
        // updated, do so.

        if (GetUpdateNormals() && pkData->GetNormals())
            pkData->CalculateNormals();
            
        m_bNeedPreDisplay = false;
    }
}
//---------------------------------------------------------------------------
void NiGeomMorpherController::CalculateMorphBound()
{
    // Calculate morph bound by combining bounds of all targets
    // Assumes no target will have a weight of greater than 1.0f or less
    //   than 0.0f, and that the sum of weights will be 1.0f, or the bound 
    //   may be invalid.

    NIASSERT(m_pkTarget);

    NiBound kBound;
    unsigned int uiNumTargets = m_spMorphData->GetNumTargets();
    if (uiNumTargets)
    {
        NiMorphData::MorphTarget* pkTarget = m_spMorphData->GetTarget(0);
        kBound.ComputeFromData(m_spMorphData->GetNumVertsPerTarget(),
            pkTarget->GetTargetVerts());

        for (unsigned int i = 1; i < uiNumTargets; i++)
        {
            pkTarget = m_spMorphData->GetTarget(i);
            NiBound kTargetBound;
            kTargetBound.ComputeFromData(
                m_spMorphData->GetNumVertsPerTarget(),
                pkTarget->GetTargetVerts());
            kBound.Merge(&kTargetBound);
        }
    }
    else
    {
        kBound.SetCenterAndRadius(NiPoint3::ZERO, 0.0f);
    }

    // Set the target's bound
    ((NiGeometry*)m_pkTarget)->SetModelBound(kBound);
}
//---------------------------------------------------------------------------
void NiGeomMorpherController::GuaranteeTimeRange(float fStartTime,
    float fEndTime)
{
    for (unsigned int uiTarget = 0; uiTarget < 
         m_spMorphData->GetNumTargets();  uiTarget++)
    {
        NiInterpolator* pkInterp = GetInterpolator(uiTarget);
        if (pkInterp)
            pkInterp->GuaranteeTimeRange(fStartTime, fEndTime);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiGeomMorpherController);
//---------------------------------------------------------------------------
void NiGeomMorpherController::CopyMembers(NiGeomMorpherController* pkDest,
    NiCloningProcess& kCloning)
{
    NiInterpController::CopyMembers(pkDest, kCloning);
    pkDest->m_uFlags = m_uFlags;

    if (m_spMorphData)
    {
        pkDest->SetMorphData(m_spMorphData, pkDest->m_pkTarget != NULL);
    }

    if (ms_bAlwaysUpdateClone)
    {
        m_bAlwaysUpdate = true;
        pkDest->SetAlwaysUpdate(true);
    }

    for (unsigned short us = 0; us < GetNumTargets(); us++)
    {
        InterpItem& kInterpItem = m_pkInterpItems[us];
        if (kInterpItem.m_spInterpolator)
        {
            pkDest->m_pkInterpItems[us].m_spInterpolator = (NiInterpolator*)
                kInterpItem.m_spInterpolator->CreateSharedClone(kCloning);
        }
        pkDest->m_pkInterpItems[us].m_fWeight = kInterpItem.m_fWeight;
    }
}
//---------------------------------------------------------------------------
void NiGeomMorpherController::ProcessClone(NiCloningProcess& kCloning)
{
    NiInterpController::ProcessClone(kCloning);

    for (unsigned short us = 0; us < GetNumTargets(); us++)
    {
        if (m_pkInterpItems[us].m_spInterpolator)
        {
            m_pkInterpItems[us].m_spInterpolator->ProcessClone(kCloning);
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiGeomMorpherController);
//---------------------------------------------------------------------------
void NiGeomMorpherController::LoadBinary(NiStream& kStream)
{
    NiInterpController::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);

    kStream.ReadLinkID();   // m_spMorphData

    NiBool bAlwaysUpdate;
    NiStreamLoadBinary(kStream, bAlwaysUpdate);
    m_bAlwaysUpdate = (bAlwaysUpdate != 0);
    
    if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 1, 0, 104))
    {
        if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 3))
        {
            kStream.ReadMultipleLinkIDs(); // m_aspInterpolators
        }
        else
        {
            unsigned int uiArraySize;
            NiStreamLoadBinary(kStream, uiArraySize);
            m_pkInterpItems = NiNew InterpItem[uiArraySize];
            for (unsigned short ui = 0; ui < uiArraySize; ui++)
            {
                m_pkInterpItems[ui].m_spInterpolator =
                    (NiInterpolator*) kStream.ResolveLinkID();
                NiStreamLoadBinary(kStream, m_pkInterpItems[ui].m_fWeight);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiGeomMorpherController::LinkObject(NiStream& kStream)
{
    NiInterpController::LinkObject(kStream);

    // link the shared data
    m_spMorphData = (NiMorphData*) kStream.GetObjectFromLinkID();
    if (m_pkTarget)
    {
        CalculateMorphBound();
        m_bBoundCalculated = true;
    }

    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 3))
    {
        // Allocate interp items array and load weights.
        AllocateInterpArray();
        for (unsigned int ui = 0; ui < GetNumTargets(); ui++)
        {
            m_pkInterpItems[ui].m_fWeight =
                m_spMorphData->GetTarget(ui)->GetLegacyWeight();
        }
    }

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104)
        && m_spMorphData)
    {
        // we do not have to allocate the interpolator pointer array,
        // as it has already been allocated above
        unsigned int uiNumTargets = m_spMorphData->GetNumTargets();
        for (unsigned int ui = 0; ui < uiNumTargets; ui++)
        {
            NiMorphData::MorphTarget* pkTarget = 
                m_spMorphData->GetTarget(ui);
            NIASSERT(pkTarget != NULL);
            NiInterpolator* pkInterp = pkTarget->GetLegacyInterpolator();
            SetInterpolator(pkInterp, ui);
        }
    }
    else if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 3))
    {
        // link interpolators
        // we do not have to allocate the interpolator pointer array,
        // as it has already been allocated above
        unsigned int uiSize = kStream.GetNumberOfLinkIDs();
        if (uiSize)
        {
            NIASSERT(uiSize == GetNumTargets());
            NIASSERT(uiSize == GetInterpolatorCount());
            for (unsigned int ui = 0; ui < uiSize; ui++)
            {
                NiInterpolator* pkInterp = 
                    (NiInterpolator*) kStream.GetObjectFromLinkID();
                SetInterpolator(pkInterp, ui);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiGeomMorpherController::RegisterStreamables(NiStream& kStream)
{
    if (!NiInterpController::RegisterStreamables(kStream))
        return false;

    if (m_spMorphData)
        m_spMorphData->RegisterStreamables(kStream);

    for (unsigned short us = 0; us < GetInterpolatorCount(); us++)
    {
        NiInterpolator* pkInterp = GetInterpolator(us);
        if(pkInterp)
        {
            pkInterp->RegisterStreamables(kStream);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiGeomMorpherController::SaveBinary(NiStream& kStream)
{
    NiInterpController::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uFlags);

    kStream.SaveLinkID(m_spMorphData);

    NiBool bAlwaysUpdate = m_bAlwaysUpdate;
    NiStreamSaveBinary(kStream, bAlwaysUpdate);

    unsigned int uiArraySize = GetNumTargets();
    NiStreamSaveBinary(kStream, uiArraySize);
    for (unsigned int ui = 0; ui < uiArraySize; ui++)
    {
        kStream.SaveLinkID(m_pkInterpItems[ui].m_spInterpolator);
        NiStreamSaveBinary(kStream, m_pkInterpItems[ui].m_fWeight);
    }
}
//---------------------------------------------------------------------------
bool NiGeomMorpherController::IsEqual(NiObject* pkObject)
{
    if (!NiInterpController::IsEqual(pkObject))
        return false;

    NiGeomMorpherController* pkMorph = (NiGeomMorpherController*) pkObject;

    if (!m_spMorphData->IsEqual(pkMorph->m_spMorphData))
        return false;

    for (unsigned int ui = 0; ui < GetNumTargets(); ui++)
    {
        InterpItem& kCompareItem = pkMorph->m_pkInterpItems[ui];
        if ((kCompareItem.m_spInterpolator &&
                !m_pkInterpItems[ui].m_spInterpolator) ||
            (!kCompareItem.m_spInterpolator &&
                m_pkInterpItems[ui].m_spInterpolator) ||
            (kCompareItem.m_spInterpolator &&
                !kCompareItem.m_spInterpolator->IsEqual(
                m_pkInterpItems[ui].m_spInterpolator)))
        {
            return false;
        }
        
        if (kCompareItem.m_fWeight != m_pkInterpItems[ui].m_fWeight)
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiInterpController methods
//---------------------------------------------------------------------------
unsigned short NiGeomMorpherController::GetInterpolatorCount() const
{
    return (unsigned short) GetNumTargets();
}
//---------------------------------------------------------------------------
const char* NiGeomMorpherController::GetInterpolatorID(unsigned short 
    usIndex) 
{
    NIASSERT(usIndex < GetInterpolatorCount());
    NiMorphData::MorphTarget* pkTarget = m_spMorphData->GetTarget(usIndex);
    NIASSERT(pkTarget);
    return pkTarget->GetName();
}
//---------------------------------------------------------------------------
unsigned short NiGeomMorpherController::GetInterpolatorIndex(const char* pcID) 
    const
{
    if (pcID == NULL)
        return INVALID_INDEX;

    for (unsigned short us = 0; us < GetNumTargets(); us++)
    {
        NiMorphData::MorphTarget* pkTarget = 
            m_spMorphData->GetTarget(us);
        NIASSERT(pkTarget);
        if (NiStricmp(pkTarget->GetName(), pcID) == 0)
            return us;
    }

    return INVALID_INDEX;
}
//---------------------------------------------------------------------------
NiInterpolator* NiGeomMorpherController::GetInterpolator(
    unsigned short usIndex) const
{
    NIASSERT(m_pkInterpItems != NULL);
    NIASSERT(usIndex < GetInterpolatorCount());
    return m_pkInterpItems[usIndex].m_spInterpolator;
}
//---------------------------------------------------------------------------
void NiGeomMorpherController::SetInterpolator(NiInterpolator* pkInterpolator,
    unsigned short usIndex)
{
    NIASSERT(m_pkInterpItems != NULL);
    NIASSERT(usIndex < GetInterpolatorCount());
    NIASSERT(!pkInterpolator || InterpolatorIsCorrectType(pkInterpolator,
        usIndex));
    m_pkInterpItems[usIndex].m_spInterpolator = pkInterpolator;
}
//---------------------------------------------------------------------------
NiInterpolator* NiGeomMorpherController::CreatePoseInterpolator(
    unsigned short usIndex)
{
    float fValue = GetWeight(usIndex);

    NiFloatInterpolator* pkPoseInterp = 
        NiNew NiFloatInterpolator(fValue);
    return pkPoseInterp;
}
//---------------------------------------------------------------------------
void NiGeomMorpherController::SynchronizePoseInterpolator(
    NiInterpolator* pkInterp, unsigned short usIndex)
{
    float fValue = GetWeight(usIndex);

    NiFloatInterpolator* pkPoseInterp = 
        NiDynamicCast(NiFloatInterpolator, pkInterp);
    NIASSERT(pkPoseInterp);
    pkPoseInterp->SetPoseValue(fValue);
}
//---------------------------------------------------------------------------
NiBlendInterpolator* NiGeomMorpherController::CreateBlendInterpolator(
    unsigned short usIndex, bool bManagerControlled,
    bool bAccumulateAnimations, float fWeightThreshold,
    unsigned char ucArraySize) const
{
    return NiNew NiBlendFloatInterpolator(bManagerControlled, fWeightThreshold,
        ucArraySize);
}
//---------------------------------------------------------------------------
bool NiGeomMorpherController::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator, unsigned short usIndex) const
{
    NIASSERT(pkInterpolator);
    return pkInterpolator->IsFloatValueSupported();
}
//---------------------------------------------------------------------------
