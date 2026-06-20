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

#include "NiControllerManager.h"
#include <NiCloningProcess.h>
#include <NiNode.h>
#include <NiSystem.h>
#include <NiDefaultAVObjectPalette.h>
#include "NiTransformController.h"
#include "NiBlendAccumTransformInterpolator.h"

NiImplementRTTI(NiControllerManager, NiTimeController);

//---------------------------------------------------------------------------
NiControllerManager::NiControllerManager() : 
    m_kIndexMap(37, false),
    m_bCumulative(false)       
{
}
//---------------------------------------------------------------------------
NiControllerManager::NiControllerManager(NiAVObject* pkTarget,
    bool bCumulativeAnimations) : 
    m_kIndexMap(37, false),
    m_bCumulative(bCumulativeAnimations)
{
    NIASSERT(pkTarget);

    NiTimeController::SetTarget(pkTarget);
    m_spObjectPalette = NiNew NiDefaultAVObjectPalette(pkTarget);
}
//---------------------------------------------------------------------------
NiControllerManager::~NiControllerManager()
{
    m_kSequenceArray.RemoveAll();
    m_kTempBlendSeqs.RemoveAll();
}
//---------------------------------------------------------------------------
void NiControllerManager::Update(float fTime)
{
    NIMETRICS_ANIMATION_SCOPETIMER(CM_UPDATE_TIME);

    unsigned int uiMetricsUpdatedSequences = 0;

    if (!GetActive())
    {
        return;
    }

    for (unsigned int ui = 0; ui < m_kSequenceArray.GetSize(); ui++)
    {
        NiControllerSequence* pkSequence = m_kSequenceArray.GetAt(ui);
        if (pkSequence)
        {
            pkSequence->Update(fTime);
            uiMetricsUpdatedSequences++;
        }
    }

    NIMETRICS_ANIMATION_ADDVALUE(UPDATED_SEQUENCES, uiMetricsUpdatedSequences);
}
//---------------------------------------------------------------------------
NiControllerSequence* NiControllerManager::AddSequenceFromFile(
    const char* pcFilename, unsigned int uiIndex, const NiFixedString& kName)
{
    // Load the sequence from file.
    NiControllerSequencePtr spSequence =
        NiControllerSequence::CreateSequenceFromFile(pcFilename, uiIndex);
    if (!spSequence)
    {
        return NULL;
    }

    // Set the accum root name if not set (to support loading older KF files).
    if (!spSequence->GetAccumRootName().Exists())
    {
        spSequence->SetAccumRootName(m_pkTarget->GetName());
    }

    // Add the sequence to the manager.
    if (!AddSequence(spSequence, kName))
    {
        return NULL;
    }

    return spSequence;
}
//---------------------------------------------------------------------------
NiControllerSequence* NiControllerManager::AddSequenceFromFile(
    const NiStream& kStream, unsigned int uiIndex, const NiFixedString& kName)
{
    // Load the sequence from file.
    NiControllerSequencePtr spSequence = NiControllerSequence::
        CreateSequenceFromFile(kStream, uiIndex);
    if (!spSequence)
    {
        return NULL;
    }

    // Set the accum root name if not set (to support loading older KF files).
    if (!spSequence->GetAccumRootName())
    {
        spSequence->SetAccumRootName(m_pkTarget->GetName());
    }

    // Add the sequence to the manager.
    if (!AddSequence(spSequence, kName))
    {
        return NULL;
    }

    return spSequence;
}
//---------------------------------------------------------------------------
bool NiControllerManager::AddAllSequencesFromFile(const char* pcFilename)
{
    // Load the sequences from file.
    NiControllerSequencePointerArray kSequences;
    if (!NiControllerSequence::CreateAllSequencesFromFile(pcFilename,
        kSequences))
    {
        return false;
    }

    // Add each sequence to the manager.
    bool bSuccess = true;
    m_kSequenceArray.SetSize(m_kSequenceArray.GetSize() + 
        kSequences.GetSize());
    for (unsigned int ui = 0; ui < kSequences.GetSize(); ui++)
    {
        NiControllerSequence* pkSequence = kSequences.GetAt(ui);
        if (pkSequence)
        {
            // Set the accum root name if not set (to support loading older
            // KF files).
            if (!pkSequence->GetAccumRootName())
            {
                pkSequence->SetAccumRootName(m_pkTarget->GetName());
            }

            // Add the sequence to the manager.
            bSuccess = bSuccess && AddSequence(pkSequence);
        }
    }
    kSequences.RemoveAll();

    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiControllerManager::AddSequence(NiControllerSequence* pkSequence,
    const NiFixedString& kName, bool bStoreTargets)
{
    NIASSERT(pkSequence);

    if (!pkSequence->SetOwner(this))
    {
        NiOutputDebugString("Error adding sequence: Sequence already has "
            "an owner.\n");
        return false;
    }

    // Set the accum root name if not set (to support loading older
    // KF files).
    if (!pkSequence->GetAccumRootName())
    {
        pkSequence->SetAccumRootName(m_pkTarget->GetName());
    }

    if (bStoreTargets)
    {
        NIASSERT(m_pkTarget && NiIsKindOf(NiAVObject, m_pkTarget));
        if (!pkSequence->StoreTargets((NiAVObject*) m_pkTarget))
        {
            pkSequence->SetOwner(NULL);
            return false;
        }
    }

    if (kName.Exists())
    {
        pkSequence->SetName(kName);
    }

    m_kIndexMap.SetAt(pkSequence->GetName(), 
        m_kSequenceArray.AddFirstEmpty(pkSequence));

    return true;
}
//---------------------------------------------------------------------------
NiControllerSequencePtr NiControllerManager::RemoveSequence(
    const NiFixedString& kName)
{
    NIASSERT(kName.Exists());

    unsigned int uiIndex;
    if (m_kIndexMap.GetAt((const char*)kName, uiIndex))
    {
        NiControllerSequencePtr spSequence;
        spSequence = m_kSequenceArray.GetAt(uiIndex);
        if (spSequence)
        {
            spSequence->ClearTargets();
            spSequence->SetOwner(NULL);

            m_kIndexMap.RemoveAt(kName);
            m_kSequenceArray.RemoveAt(uiIndex);
        }
        return spSequence;
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiControllerSequencePtr NiControllerManager::RemoveSequence(
    NiControllerSequence* pkSequence)
{
    NIASSERT(pkSequence);

    for (unsigned int ui = 0; ui < m_kSequenceArray.GetSize(); ui++)
    {
        NiControllerSequencePtr spSequence = m_kSequenceArray.GetAt(ui);
        if (spSequence == pkSequence)
        {
            m_kSequenceArray.RemoveAt(ui);

            NiTMapIterator kIter = m_kIndexMap.GetFirstPos();
            while (kIter)
            {
                const char* pcName;
                unsigned int uiIndex;
                m_kIndexMap.GetNext(kIter, pcName, uiIndex);
                if (uiIndex == ui)
                {
                    m_kIndexMap.RemoveAt(pcName);
                    break;
                }
            }

            spSequence->ClearTargets();
            spSequence->SetOwner(NULL);
            return spSequence;
        }
    }
        
    return NULL;
}
//---------------------------------------------------------------------------
void NiControllerManager::RemoveAllSequences()
{
    for (unsigned int ui = 0; ui < m_kSequenceArray.GetSize(); ui++)
    {
        NiControllerSequence* pkSequence = m_kSequenceArray.GetAt(ui);
        if (pkSequence)
        {
            pkSequence->ClearTargets();
            pkSequence->SetOwner(NULL);
        }
    }
    m_kSequenceArray.RemoveAll();
    m_kIndexMap.RemoveAll();
}
//---------------------------------------------------------------------------
bool NiControllerManager::CrossFade(const NiFixedString& kSourceSequenceName,
    const NiFixedString& kDestSequenceName, float fDuration, int iPriority,
    bool bStartOver, float fWeight, const NiFixedString& kTimeSyncSeqName)
{
    NIASSERT(kSourceSequenceName.Exists() && kDestSequenceName.Exists());

    unsigned int uiSourceIndex, uiDestIndex;
    if (m_kIndexMap.GetAt(kSourceSequenceName, uiSourceIndex) &&
        m_kIndexMap.GetAt(kDestSequenceName, uiDestIndex))
    {
        unsigned int uiTimeSyncSeqIndex;
        if (kTimeSyncSeqName.Exists())
        {
            if (m_kIndexMap.GetAt(kTimeSyncSeqName, uiTimeSyncSeqIndex))
            {
                return CrossFade(m_kSequenceArray.GetAt(uiSourceIndex),
                    m_kSequenceArray.GetAt(uiDestIndex), fDuration, iPriority,
                    bStartOver, fWeight,
                    m_kSequenceArray.GetAt(uiTimeSyncSeqIndex));
            }
            else
            {
                return false;
            }
        }
        else
        {
            return CrossFade(m_kSequenceArray.GetAt(uiSourceIndex),
                m_kSequenceArray.GetAt(uiDestIndex), fDuration, iPriority,
                bStartOver, fWeight, NULL);
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiControllerManager::CrossFade(NiControllerSequence* pkSourceSequence,
    NiControllerSequence* pkDestSequence, float fDuration, int iPriority,
    bool bStartOver, float fWeight, NiControllerSequence* pkTimeSyncSeq)
{
    NIASSERT(pkSourceSequence && pkSourceSequence->GetOwner() == this);
    NIASSERT(pkDestSequence && pkDestSequence->GetOwner() == this);

    if (pkSourceSequence->GetState() == NiControllerSequence::INACTIVE ||
        pkDestSequence->GetState() != NiControllerSequence::INACTIVE)
    {
        return false;
    }

    pkSourceSequence->Deactivate(fDuration, true);
    return pkDestSequence->Activate(iPriority, bStartOver, fWeight, fDuration,
        pkTimeSyncSeq, true);
}
//---------------------------------------------------------------------------
bool NiControllerManager::BlendFromSequence(
    const NiFixedString& kSourceSequenceName,
    const NiFixedString& kDestSequenceName, float fDuration,
    const NiFixedString& kDestTextKey, int iPriority, float fSourceWeight,
    float fDestWeight, const NiFixedString& kTimeSyncSeqName)
{
    NIASSERT(kSourceSequenceName.Exists() && kDestSequenceName.Exists());

    unsigned int uiSourceIndex, uiDestIndex;
    if (m_kIndexMap.GetAt(kSourceSequenceName, uiSourceIndex) &&
        m_kIndexMap.GetAt(kDestSequenceName, uiDestIndex))
    {
        NiControllerSequence* pkSourceSequence = m_kSequenceArray.GetAt(
            uiSourceIndex);
        NiControllerSequence* pkDestSequence = m_kSequenceArray.GetAt(
            uiDestIndex);
        NIASSERT(pkSourceSequence && pkDestSequence);

        NiControllerSequence* pkTimeSyncSeq = NULL;
        unsigned int uiTimeSyncIndex;
        if (kTimeSyncSeqName.Exists())
        {
            if (m_kIndexMap.GetAt(kTimeSyncSeqName, uiTimeSyncIndex))
            {
                pkTimeSyncSeq = m_kSequenceArray.GetAt(uiTimeSyncIndex);
                NIASSERT(pkTimeSyncSeq);
            }
            else
            {
                return false;
            }
        }

        return BlendFromSequence(pkSourceSequence, pkDestSequence, fDuration,
            kDestTextKey, iPriority, fSourceWeight, fDestWeight,
            pkTimeSyncSeq);
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiControllerManager::BlendFromSequence(
    const NiFixedString& kSourceSequenceName,
    const NiFixedString& kDestSequenceName, float fDuration, float fDestFrame,
    int iPriority, float fSourceWeight, float fDestWeight,
    const NiFixedString& kTimeSyncSeqName)
{
    NIASSERT(kSourceSequenceName.Exists() && kDestSequenceName.Exists());

    unsigned int uiSourceIndex, uiDestIndex;
    if (m_kIndexMap.GetAt(kSourceSequenceName, uiSourceIndex) &&
        m_kIndexMap.GetAt(kDestSequenceName, uiDestIndex))
    {
        NiControllerSequence* pkSourceSequence = m_kSequenceArray.GetAt(
            uiSourceIndex);
        NiControllerSequence* pkDestSequence = m_kSequenceArray.GetAt(
            uiDestIndex);
        NIASSERT(pkSourceSequence && pkDestSequence);

        NiControllerSequence* pkTimeSyncSeq = NULL;
        unsigned int uiTimeSyncIndex;
        if (kTimeSyncSeqName.Exists())
        {
            if (m_kIndexMap.GetAt(kTimeSyncSeqName, uiTimeSyncIndex))
            {
                pkTimeSyncSeq = m_kSequenceArray.GetAt(uiTimeSyncIndex);
                NIASSERT(pkTimeSyncSeq);
            }
            else
            {
                return false;
            }
        }

        return BlendFromSequence(pkSourceSequence, pkDestSequence, fDuration,
            fDestFrame, iPriority, fSourceWeight, fDestWeight, pkTimeSyncSeq);
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiControllerManager::StopBlendFromSequence(
    const NiFixedString& kSourceSequenceName, 
    const NiFixedString& kDestSequenceName,
    float fEaseOutTime)
{
    NIASSERT(kSourceSequenceName.Exists() && kDestSequenceName.Exists());

    unsigned int uiSourceIndex, uiDestIndex;
    if (m_kIndexMap.GetAt(kSourceSequenceName, uiSourceIndex) &&
        m_kIndexMap.GetAt(kDestSequenceName, uiDestIndex))
    {
        return StopBlendFromSequence(m_kSequenceArray.GetAt(uiSourceIndex), 
            m_kSequenceArray.GetAt(uiDestIndex), fEaseOutTime);
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiControllerManager::StopBlendFromSequence(
    NiControllerSequence* pkSourceSequence,
    NiControllerSequence* pkDestSequence, float fEaseOutTime)
{
    NIASSERT(pkSourceSequence && pkSourceSequence->GetOwner() == this &&
        pkDestSequence && pkDestSequence->GetOwner() == this);

    pkSourceSequence->Deactivate(fEaseOutTime, true);
    pkDestSequence->Deactivate(fEaseOutTime, true);

    return true;
}
//---------------------------------------------------------------------------
bool NiControllerManager::Morph(const NiFixedString& kSourceSequenceName,
    const NiFixedString& kDestSequenceName, float fDuration, int iPriority,
    float fSourceWeight, float fDestWeight)
{
    NIASSERT(kSourceSequenceName.Exists() && kDestSequenceName.Exists());

    unsigned int uiSourceIndex, uiDestIndex;
    if (m_kIndexMap.GetAt(kSourceSequenceName, uiSourceIndex) &&
        m_kIndexMap.GetAt(kDestSequenceName, uiDestIndex))
    {
        return Morph(m_kSequenceArray.GetAt(uiSourceIndex), 
            m_kSequenceArray.GetAt(uiDestIndex), fDuration, iPriority,
            fSourceWeight, fDestWeight);
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiControllerManager::Morph(NiControllerSequence* pkSourceSequence,
    NiControllerSequence* pkDestSequence, float fDuration, int iPriority,
    float fSourceWeight, float fDestWeight)
{
    NIASSERT(pkSourceSequence && pkSourceSequence->GetOwner() == this &&
        pkDestSequence && pkDestSequence->GetOwner() == this);

    return pkSourceSequence->StartMorph(pkDestSequence, fDuration, iPriority,
        fSourceWeight, fDestWeight);
}
//---------------------------------------------------------------------------
bool NiControllerManager::StopMorph(const NiFixedString& kSourceSequenceName,
    const NiFixedString& kDestSequenceName, float fEaseOutTime)
{
    NIASSERT(kSourceSequenceName.Exists() && kDestSequenceName.Exists());

    unsigned int uiSourceIndex, uiDestIndex;
    if (m_kIndexMap.GetAt(kSourceSequenceName, uiSourceIndex) &&
        m_kIndexMap.GetAt(kDestSequenceName, uiDestIndex))
    {
        return StopMorph(m_kSequenceArray.GetAt(uiSourceIndex), 
            m_kSequenceArray.GetAt(uiDestIndex), fEaseOutTime);
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiControllerManager::StopMorph(NiControllerSequence* pkSourceSequence,
    NiControllerSequence* pkDestSequence, float fEaseOutTime)
{
    NIASSERT(pkSourceSequence && pkSourceSequence->GetOwner() == this &&
        pkDestSequence && pkDestSequence->GetOwner() == this);

    pkSourceSequence->Deactivate(fEaseOutTime, true);
    pkDestSequence->Deactivate(fEaseOutTime, true);

    return true;
}
//---------------------------------------------------------------------------
void NiControllerManager::ClearCumulativeAnimations()
{
    if (!m_bCumulative)
    {
        return;
    }

    NiAVObject* pkAccumRoot = GetAccumRoot();
    if (pkAccumRoot)
    {
        NiTransformController* pkTransformCtlr = NiGetController(
            NiTransformController, pkAccumRoot);
        if (pkTransformCtlr)
        {
            NiBlendAccumTransformInterpolator* pkAccumInterp = NiDynamicCast(
                NiBlendAccumTransformInterpolator,
                pkTransformCtlr->GetInterpolator());
            if (pkAccumInterp)
            {
                pkAccumInterp->ClearAccumulatedTransform();
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiControllerManager::SetAccumulatedTransform(
    const NiQuatTransform& kTransform)
{
    if (!m_bCumulative)
    {
        return;
    }

    NiAVObject* pkAccumRoot = GetAccumRoot();
    if (pkAccumRoot)
    {
        NiTransformController* pkTransformCtlr = NiGetController(
            NiTransformController, pkAccumRoot);
        if (pkTransformCtlr)
        {
            NiBlendAccumTransformInterpolator* pkAccumInterp = NiDynamicCast(
                NiBlendAccumTransformInterpolator,
                pkTransformCtlr->GetInterpolator());
            if (pkAccumInterp)
            {
                pkAccumInterp->SetAccumulatedTransform(kTransform);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiControllerManager::GetAccumulatedTransform(NiQuatTransform& kTransform)
    const
{
    if (!m_bCumulative)
    {
        return false;
    }

    NiAVObject* pkAccumRoot = GetAccumRoot();
    if (pkAccumRoot)
    {
        NiTransformController* pkTransformCtlr = NiGetController(
            NiTransformController, pkAccumRoot);
        if (pkTransformCtlr)
        {
            NiBlendAccumTransformInterpolator* pkAccumInterp = NiDynamicCast(
                NiBlendAccumTransformInterpolator,
                pkTransformCtlr->GetInterpolator());
            if (pkAccumInterp)
            {
                return pkAccumInterp->GetValue(kTransform);
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiControllerManager::BlendFromPose(const NiFixedString& kSequenceName,
    const NiFixedString& kDestTextKey, float fDuration, int iPriority,
    const NiFixedString& kSequenceToSynchronize)
{
    // This function is deprecated. Use BlendFromSequence instead.

    NIASSERT(kSequenceName.Exists());

    unsigned int uiIndex;
    if (m_kIndexMap.GetAt(kSequenceName, uiIndex))
    {
        NiControllerSequence* pkSequence = m_kSequenceArray.GetAt(uiIndex);
        NIASSERT(pkSequence);

        NiControllerSequence* pkSequenceToSynchronize = NULL;
        unsigned int uiSyncIndex;
        if (kSequenceToSynchronize.Exists())
        {
            if (m_kIndexMap.GetAt(kSequenceToSynchronize, uiSyncIndex))
            {
                pkSequenceToSynchronize = m_kSequenceArray.GetAt(uiSyncIndex);
            }
            else
            {
                return false;
            }
        }

        return BlendFromPose(pkSequence, kDestTextKey, fDuration, iPriority,
            pkSequenceToSynchronize);
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiControllerManager::BlendFromPose(const NiFixedString& kSequenceName,
    float fDestFrame, float fDuration, int iPriority,
    const NiFixedString& kSequenceToSynchronize)
{
    // This function is deprecated. Use BlendFromSequence instead.

    NIASSERT(kSequenceName.Exists());

    unsigned int uiIndex;
    if (m_kIndexMap.GetAt(kSequenceName, uiIndex))
    {
        NiControllerSequence* pkSequence = m_kSequenceArray.GetAt(uiIndex);
        NIASSERT(pkSequence);

        NiControllerSequence* pkSequenceToSynchronize = NULL;
        unsigned int uiSyncIndex;
        if (kSequenceToSynchronize.Exists())
        {
            if (m_kIndexMap.GetAt(kSequenceToSynchronize, uiSyncIndex))
            {
                pkSequenceToSynchronize = m_kSequenceArray.GetAt(uiSyncIndex);
            }
            else
            {
                return false;
            }
        }

        return BlendFromPose(pkSequence, fDestFrame, fDuration, iPriority,
            pkSequenceToSynchronize);
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiControllerManager::BlendFromPose(NiControllerSequence* pkSequence,
    const NiFixedString& kDestTextKey, float fDuration, int iPriority,
    NiControllerSequence* pkSequenceToSynchronize)
{
    // This function is deprecated. Use BlendFromSequence instead.

    NIASSERT(pkSequence && pkSequence->GetOwner() == this &&
        (!pkSequenceToSynchronize || pkSequenceToSynchronize->GetOwner() ==
        this));

    NiControllerSequence* pkTempSequence = CreateTempBlendSequence(pkSequence,
        pkSequenceToSynchronize);
    return pkTempSequence->StartBlend(pkSequence, fDuration, kDestTextKey,
        iPriority, 1.0f, 1.0f, NULL);
}
//---------------------------------------------------------------------------
bool NiControllerManager::BlendFromPose(NiControllerSequence* pkSequence,
    float fDestFrame, float fDuration, int iPriority,
    NiControllerSequence* pkSequenceToSynchronize)
{
    // This function is deprecated. Use BlendFromSequence instead.

    NIASSERT(pkSequence && pkSequence->GetOwner() == this &&
        (!pkSequenceToSynchronize || pkSequenceToSynchronize->GetOwner() ==
        this));

    NiControllerSequence* pkTempSequence = CreateTempBlendSequence(pkSequence,
        pkSequenceToSynchronize);
    return pkTempSequence->StartBlend(pkSequence, fDuration, fDestFrame,
        iPriority, 1.0f, 1.0f, NULL);
}
//---------------------------------------------------------------------------
bool NiControllerManager::StopBlendFromPose(const NiFixedString& kSequenceName)
{
    // This function is deprecated. Use StopBlendFromSequence instead.

    NIASSERT(kSequenceName.Exists());

    unsigned int uiIndex;
    if (m_kIndexMap.GetAt(kSequenceName, uiIndex))
    {
        NiControllerSequence* pkSequence = m_kSequenceArray.GetAt(uiIndex);
        return StopBlendFromPose(pkSequence);
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiControllerManager::StopBlendFromPose(NiControllerSequence* pkSequence)
{
    // This function is deprecated. Use StopBlendFromSequence instead.

    NIASSERT(false);
    return false;
}
//---------------------------------------------------------------------------
bool NiControllerManager::Sum(const NiFixedString& kSourceSequenceName,
    const NiFixedString& kDestSequenceName, float fWeight, int iPriority,
    float fEaseInTime)
{
    // This function is deprecated. ActivateSequence should instead be called
    // on both sequences with the appropriate weight values.

    NIASSERT(kSourceSequenceName.Exists() && kDestSequenceName.Exists());

    unsigned int uiSourceIndex, uiDestIndex;
    if (m_kIndexMap.GetAt(kSourceSequenceName, uiSourceIndex) &&
        m_kIndexMap.GetAt(kDestSequenceName, uiDestIndex))
    {
        return Sum(m_kSequenceArray.GetAt(uiSourceIndex), 
            m_kSequenceArray.GetAt(uiDestIndex), fWeight, iPriority,
            fEaseInTime);
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiControllerManager::Sum(NiControllerSequence* pkSourceSequence,
    NiControllerSequence* pkDestSequence, float fWeight, int iPriority,
    float fEaseInTime)
{
    // This function is deprecated. ActivateSequence should instead be called
    // on both sequences with the appropriate weight values.

    NIASSERT(pkSourceSequence && pkSourceSequence->GetOwner() == this &&
        pkDestSequence && pkDestSequence->GetOwner() == this);
    NIASSERT(fWeight >= 0.0f && fWeight <= 1.0f);

    if (!pkSourceSequence->Activate(iPriority, false, fWeight, fEaseInTime,
        pkDestSequence, false) || !pkDestSequence->Activate(iPriority, true,
        1.0f - fWeight, fEaseInTime, NULL, false))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiControllerManager::SetSumWeight(
    const NiFixedString& kSourceSequenceName,
    const NiFixedString& kDestSequenceName, float fWeight)
{
    // This function is deprecated. SetSequenceWeight should instead be called
    // on both sequences with the appropriate weight values.

    NIASSERT(kSourceSequenceName.Exists() && kDestSequenceName.Exists());

    unsigned int uiSourceIndex, uiDestIndex;
    if (m_kIndexMap.GetAt(kSourceSequenceName, uiSourceIndex) &&
        m_kIndexMap.GetAt(kDestSequenceName, uiDestIndex))
    {
        return SetSumWeight(m_kSequenceArray.GetAt(uiSourceIndex), 
            m_kSequenceArray.GetAt(uiDestIndex), fWeight);
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiControllerManager::SetSumWeight(NiControllerSequence* pkSourceSequence,
    NiControllerSequence* pkDestSequence, float fWeight)
{
    // This function is deprecated. SetSequenceWeight should instead be called
    // on both sequences with the appropriate weight values.

    NIASSERT(pkSourceSequence && pkSourceSequence->GetOwner() == this &&
        pkDestSequence && pkDestSequence->GetOwner() == this);
    NIASSERT(fWeight >= 0.0f && fWeight <= 1.0f);
    NIASSERT(pkSourceSequence->GetState() != NiControllerSequence::INACTIVE &&
        pkDestSequence->GetState() != NiControllerSequence::INACTIVE);

    pkSourceSequence->SetSequenceWeight(fWeight);
    pkDestSequence->SetSequenceWeight(1.0f - fWeight);

    return true;
}
//---------------------------------------------------------------------------
bool NiControllerManager::StopSum(const NiFixedString& kSourceSequenceName,
    const NiFixedString& kDestSequenceName, float fEaseOutTime)
{
    // This function is deprecated. DeactivateSequence should instead be
    // called on both sequences.

    NIASSERT(kSourceSequenceName.Exists() && kDestSequenceName.Exists());

    unsigned int uiSourceIndex, uiDestIndex;
    if (m_kIndexMap.GetAt(kSourceSequenceName, uiSourceIndex) &&
        m_kIndexMap.GetAt(kDestSequenceName, uiDestIndex))
    {
        return StopSum(m_kSequenceArray.GetAt(uiSourceIndex), 
            m_kSequenceArray.GetAt(uiDestIndex), fEaseOutTime);
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiControllerManager::StopSum(NiControllerSequence* pkSourceSequence,
    NiControllerSequence* pkDestSequence, float fEaseOutTime)
{
    // This function is deprecated. DeactivateSequence should instead be
    // called on both sequences.

    NIASSERT(pkSourceSequence && pkSourceSequence->GetOwner() == this &&
        pkDestSequence && pkDestSequence->GetOwner() == this);
    NIASSERT(pkSourceSequence->GetState() != NiControllerSequence::INACTIVE &&
        pkDestSequence->GetState() != NiControllerSequence::INACTIVE);

    pkSourceSequence->Deactivate(fEaseOutTime, false);
    pkDestSequence->Deactivate(fEaseOutTime, false);

    return true;
}
//---------------------------------------------------------------------------
bool NiControllerManager::TargetIsRequiredType() const
{
    return NiIsKindOf(NiAVObject, m_pkTarget);
}
//---------------------------------------------------------------------------
NiControllerSequence* NiControllerManager::CreateTempBlendSequence(
    NiControllerSequence* pkSequence,
    NiControllerSequence* pkSequenceToSynchronize)
{
    // Determine temp sequence array size.
    unsigned int uiArraySize;
    if (pkSequenceToSynchronize)
    {
        uiArraySize = pkSequenceToSynchronize->GetArraySize();
    }
    else
    {
        uiArraySize = pkSequence->GetArraySize();
    }

    // Get/create temp sequence.
    NiControllerSequence* pkTempSequence = NULL;
    unsigned int ui;
    for (ui = 0; ui < m_kTempBlendSeqs.GetSize(); ui++)
    {
        NiControllerSequence* pkSeq = m_kTempBlendSeqs.GetAt(ui);
        if (pkSeq->GetState() == NiControllerSequence::INACTIVE)
        {
            if (pkSeq->GetArraySize() < uiArraySize)
            {
                RemoveSequence(pkSeq);
                m_kTempBlendSeqs.RemoveAt(ui--);
                continue;
            }
            else
            {
                pkTempSequence = pkSeq;
                break;
            }
        }
    }
    if (!pkTempSequence)
    {
        pkTempSequence = NiNew NiControllerSequence("__TempBlendSequence__",
            uiArraySize);
        m_kTempBlendSeqs.Add(pkTempSequence);
    }
    else
    {
        RemoveSequence(pkTempSequence);
    }

    pkSequence->PopulatePoseSequence(pkTempSequence, uiArraySize,
        pkSequenceToSynchronize);

    AddSequence(pkTempSequence, NULL, false);

    return pkTempSequence;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiControllerManager);
//---------------------------------------------------------------------------
void NiControllerManager::CopyMembers(NiControllerManager* pkDest,
    NiCloningProcess& kCloning)
{
    NiTimeController::CopyMembers(pkDest, kCloning);

    pkDest->m_bCumulative = m_bCumulative;

    for (unsigned int ui = 0; ui < m_kSequenceArray.GetSize(); ui++)
    {
        NiControllerSequence* pkSequence = m_kSequenceArray.GetAt(ui);
        if (pkSequence)
        {
            NiControllerSequence* pkNewSequence = (NiControllerSequence*)
                pkSequence->CreateClone(kCloning);
            NIVERIFY(pkDest->AddSequence(pkNewSequence, NULL, false));
        }
    }

    if (m_spObjectPalette)
    {
        pkDest->m_spObjectPalette = (NiAVObjectPalette*) m_spObjectPalette
            ->CreateClone(kCloning);
    }
}
//---------------------------------------------------------------------------
void NiControllerManager::ProcessClone(NiCloningProcess& kCloning)
{
    NiTimeController::ProcessClone(kCloning);

    unsigned int ui;
    for (ui = 0; ui < m_kSequenceArray.GetSize(); ui++)
    {
        NiControllerSequence* pkSequence = m_kSequenceArray.GetAt(ui);
        if (pkSequence)
        {
            pkSequence->ProcessClone(kCloning);
            pkSequence->StoreTargets((NiAVObject*) m_pkTarget);
        }
    }

    NiObject* pkClone = 0;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(pkClone);
    NiControllerManager* pkDest = (NiControllerManager*) pkClone;

    for (ui = 0; ui < m_kTempBlendSeqs.GetSize(); ui++)
    {
        bCloned = kCloning.m_pkCloneMap->GetAt(m_kTempBlendSeqs.GetAt(ui),
            pkClone);
        if (bCloned)
        {
            pkDest->m_kTempBlendSeqs.Add((NiControllerSequence*) pkClone);
        }
        else
        {
            pkDest->m_kTempBlendSeqs.Add(m_kTempBlendSeqs.GetAt(ui));
        }
    }

    if (m_spObjectPalette)
    {
        m_spObjectPalette->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
bool NiControllerManager::RegisterStreamables(NiStream& kStream)
{
    if (!NiTimeController::RegisterStreamables(kStream))
    {
        return false;
    }

    for (unsigned int ui = 0; ui < m_kSequenceArray.GetSize(); ui++)
    {
        NiControllerSequence* pkSequence = m_kSequenceArray.GetAt(ui);
        if (pkSequence)
            pkSequence->RegisterStreamables(kStream);
    }

    if (m_spObjectPalette)
    {
        m_spObjectPalette->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiControllerManager::SaveBinary(NiStream& kStream)
{
    NiTimeController::SaveBinary(kStream);

    NiBool bCumulative = m_bCumulative;
    NiStreamSaveBinary(kStream, bCumulative);

    unsigned int uiNumSequences = m_kSequenceArray.GetSize();
    NiStreamSaveBinary(kStream, uiNumSequences);

    for (unsigned int ui = 0; ui < uiNumSequences; ui++)
    {
        NiControllerSequence* pkSequence = m_kSequenceArray.GetAt(ui);
        if (pkSequence)
            kStream.SaveLinkID(pkSequence);
    }

    kStream.SaveLinkID(m_spObjectPalette);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiControllerManager);

//---------------------------------------------------------------------------
void NiControllerManager::LoadBinary(NiStream& kStream)
{
    NiTimeController::LoadBinary(kStream);

    NiBool bCumulative;
    NiStreamLoadBinary(kStream, bCumulative);
    m_bCumulative = (bCumulative != 0);

    kStream.ReadMultipleLinkIDs(); // m_kSequenceArray

    if (kStream.GetFileVersion() >= kStream.GetVersion(10, 1, 0, 104))
    {
        m_spObjectPalette = (NiDefaultAVObjectPalette*)
            kStream.ResolveLinkID();
    }
}
//---------------------------------------------------------------------------
void NiControllerManager::LinkObject(NiStream& kStream)
{
    NiTimeController::LinkObject(kStream);
    
    unsigned int uiNumSequences = kStream.GetNumberOfLinkIDs();
    m_kSequenceArray.SetSize(uiNumSequences);
    for (unsigned int ui = 0; ui < uiNumSequences; ui++)
    {
        NiControllerSequence* pkSequence = (NiControllerSequence*)
            kStream.GetObjectFromLinkID();
        NIVERIFY(AddSequence(pkSequence, NULL, false));
    }
}
//---------------------------------------------------------------------------
void NiControllerManager::PostLinkObject(NiStream& kStream)
{
    NiTimeController::PostLinkObject(kStream);

    NIASSERT(m_pkTarget && NiIsKindOf(NiAVObject, m_pkTarget));
    NiAVObject* pkRoot = (NiAVObject*) m_pkTarget;
    
    for (unsigned int ui = 0; ui < m_kSequenceArray.GetSize(); ui++)
    {
        NiControllerSequence* pkSequence = m_kSequenceArray.GetAt(ui);
        if (pkSequence)
            pkSequence->StoreTargets(pkRoot);

    }
}
//---------------------------------------------------------------------------
bool NiControllerManager::IsEqual(NiObject* pkObject)
{
    if (!NiTimeController::IsEqual(pkObject))
    {
        return false;
    }

    NiControllerManager* pkManager = (NiControllerManager*) pkObject;

    if (m_kSequenceArray.GetSize() != pkManager->m_kSequenceArray.GetSize())
    {
        return false;
    }


    NiTMapIterator kIter = m_kIndexMap.GetFirstPos();
    while (kIter)
    {
        const char* pcName;
        unsigned int uiIndex0;
        m_kIndexMap.GetNext(kIter, pcName, uiIndex0);

        unsigned int uiIndex1;
        if (!pkManager->m_kIndexMap.GetAt(pcName, uiIndex1))
        {
            return false;
        }
        if (uiIndex0 != uiIndex0)
        {
            return false;
        }
    }

    for (unsigned int ui = 0; ui < m_kSequenceArray.GetSize(); ui++)
    {
        NiControllerSequence* pkSequence0 = m_kSequenceArray.GetAt(ui);
        NiControllerSequence* pkSequence1 = 
            pkManager->m_kSequenceArray.GetAt(ui);
        if(pkSequence0 && pkSequence1)
        {
            if (!pkSequence0->IsEqual(pkSequence1))
                return false;
        }
        else if (pkSequence0 != pkSequence1)
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
