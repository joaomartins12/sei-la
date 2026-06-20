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

#include "NiActorManager.h"
#include "NiAnimationMetrics.h"
#include "NiCloningProcess.h"
#include <NiFilename.h>

const NiActorManager::SequenceID NiActorManager::ANY_SEQUENCE_ID =
    (unsigned int) -1;
const NiActorManager::SequenceID NiActorManager::INVALID_SEQUENCE_ID =
    (unsigned int) -2;
const float NiActorManager::INVALID_TIME = -FLT_MAX;

const NiActorManager::SequenceID NiActorManager::ANY_EVENT_CODE =
    NiActorManager::ANY_SEQUENCE_ID;
const NiActorManager::SequenceID NiActorManager::INVALID_EVENT_CODE =
    NiActorManager::INVALID_SEQUENCE_ID;

NiFixedString NiActorManager::ms_kEndTextKey;
NiFixedString NiActorManager::ms_kStartTextKey;

NiAllocatorDeclareStatics(NiActorManager::TimelineData, 256)
//---------------------------------------------------------------------------
void NiActorManager::_SDMInit()
{
    ms_kEndTextKey = "end";
    ms_kStartTextKey = "start";
}
//---------------------------------------------------------------------------
void NiActorManager::_SDMShutdown()
{
    ms_kEndTextKey = NULL;
    ms_kStartTextKey = NULL;
}
//---------------------------------------------------------------------------
NiActorManager::NiActorManager(NiKFMTool* pkKFMTool,
    bool bCumulativeAnimations) : 
    m_spKFMTool(pkKFMTool),
    m_pkCallbackObject(NULL),
    m_bCumulativeAnimations(bCumulativeAnimations)
{
    Reset();
    m_kCallbacks.SetSize(3);
    m_kCallbacks.SetGrowBy(3);
}
//---------------------------------------------------------------------------
NiActorManager::~NiActorManager()
{
}
//---------------------------------------------------------------------------
NiActorManager* NiActorManager::Create(const char* pcKFMFilename,
    bool bCumulativeAnimations, bool bLoadFilesFromDisk, NiStream* pkStream)
{
    NIASSERT(pcKFMFilename);

    NiKFMTool::KFM_RC eRC, eSuccess = NiKFMTool::KFM_SUCCESS;

    // Create KFM tool and load KFM file.
    NiKFMToolPtr spKFMTool = NiNew NiKFMTool;
    eRC = spKFMTool->LoadFile(pcKFMFilename);
    if (eRC != eSuccess)
    {
        return NULL;
    }

    return Create(spKFMTool, pcKFMFilename, bCumulativeAnimations,
        bLoadFilesFromDisk, pkStream);
}
//---------------------------------------------------------------------------
NiActorManager* NiActorManager::Create(NiKFMTool* pkKFMTool,
    const char* pcKFMFilePath, bool bCumulativeAnimations,
    bool bLoadFilesFromDisk, NiStream* pkStream)
{
    NIASSERT(pkKFMTool && pcKFMFilePath);

    // Build the KFM path.
    NiFilename kFilename(pcKFMFilePath);
    char acKFMPath[NI_MAX_PATH];
    NiSprintf(acKFMPath, NI_MAX_PATH, "%s%s", kFilename.GetDrive(), 
        kFilename.GetDir());
    pkKFMTool->SetBaseKFMPath(acKFMPath);

    // Create actor manager.
    NiActorManager* pkActorManager = NiNew NiActorManager(pkKFMTool,
        bCumulativeAnimations);

    // Initialize actor manager.
    if (bLoadFilesFromDisk)
    {
        // Create the stream if not provided.
        bool bDeleteStream = false;
        if (!pkStream)
        {
            pkStream = NiNew NiStream;
            bDeleteStream = true;
        }

        // Initialize the actor manager and load all files.
        if (!pkActorManager->Initialize(*pkStream))
        {
            NiDelete pkActorManager;
            return NULL;
        }

        // Delete stream if created earlier.
        if (bDeleteStream)
        {
            NiDelete pkStream;
        }
    }

    return pkActorManager;
}
//---------------------------------------------------------------------------
bool NiActorManager::Initialize(NiStream& kStream)
{
    if (!LoadNIFFile(kStream, true))
    {
        return false;
    }
    LoadSequences(&kStream);

    return true;
}
//---------------------------------------------------------------------------
bool NiActorManager::LoadNIFFile(NiStream& kStream, bool bLoadNIFFile)
{
    // Get model path.
    const char* pcModelPath = m_spKFMTool->GetModelPath();
    if (!pcModelPath)
    {
        return false;
    }

    // Load NIF file, if indicated.
    if (bLoadNIFFile)
    {
        // Get NIF filename.
        const char* pcNIFFilename = m_spKFMTool->GetFullModelPath();
        NIASSERT(pcNIFFilename);

        // Load NIF file.
        bool bSuccess = kStream.Load(pcNIFFilename);
        if (!bSuccess)
        {
            return false;
        }
    }

    // Get NIF root.
    NiAVObject* pkNIFRoot = NiDynamicCast(NiAVObject,
        kStream.GetObjectAt(0));
    if (!pkNIFRoot)
    {
        return false;
    }

    return ChangeNIFRoot(pkNIFRoot);
}
//---------------------------------------------------------------------------
bool NiActorManager::LoadSequences(NiStream* pkStream)
{
    bool bDeleteStream = false;
    if (!pkStream)
    {
        pkStream = NiNew NiStream;
        bDeleteStream = true;
    }

    // Iterate through entire array of sequence IDs.
    bool bAllSuccessful = true;
    SequenceID* pSequenceIDs;
    unsigned int uiNumIDs;
    m_spKFMTool->GetSequenceIDs(pSequenceIDs, uiNumIDs);
    char acLastKFFilename[NI_MAX_PATH];
    acLastKFFilename[0] = '\0';
    for (unsigned int ui = 0; ui < uiNumIDs; ui++)
    {
        SequenceID eSequenceID = pSequenceIDs[ui];

        // Get KF filename.
        NiFixedString kKFFilename = m_spKFMTool->GetFullKFFilename(
            eSequenceID);
        NIASSERT(kKFFilename.Exists());

        // Determine whether or not to load file.
        bool bLoadKFFile = false;
        if (strcmp(acLastKFFilename, kKFFilename) != 0)
        {
            bLoadKFFile = true;
            NiStrcpy(acLastKFFilename, NI_MAX_PATH, kKFFilename);
        }

        if (!AddSequence(eSequenceID, *pkStream, bLoadKFFile))
        {
            bAllSuccessful = false;
        }
    }
    NiFree(pSequenceIDs);
    
    if (bDeleteStream)
    {
        NiDelete pkStream;
    }
    
    return bAllSuccessful;
}
//---------------------------------------------------------------------------
bool NiActorManager::AddSequence(SequenceID eSequenceID, NiStream& kStream,
    bool bLoadKFFile)
{
    if (!m_spManager)
    {
        return false;
    }

    // Get sequence information.
    NiKFMTool::Sequence* pkKFMSequence = m_spKFMTool->GetSequence(
        eSequenceID);
    if (!pkKFMSequence)
    {
        return false;
    }

    // Get KF filename.
    NiFixedString kKFFilename = m_spKFMTool->GetFullKFFilename(eSequenceID);
    NIASSERT(kKFFilename.Exists());

    // Load file, if specified.
    if (bLoadKFFile)
    {
        bool bSuccess = kStream.Load(kKFFilename);
        if (!bSuccess)
        {
            NILOG("NiActorManager: Failed to load "
                "KF File: %s\n", (const char*) kKFFilename);
            return false;
        }
    }

    int iAnimIndex = pkKFMSequence->GetAnimIndex();
    NiControllerSequencePtr spSequence =
        NiControllerSequence::CreateSequenceFromFile(kStream, iAnimIndex);
    if (!spSequence)
    {
        NILOG("NiActorManager: Failed to add "
            "sequence at index %d in %s\n", iAnimIndex, 
            (const char*) kKFFilename);
        return false;
    }

    return ChangeSequence(eSequenceID, spSequence);
}
//---------------------------------------------------------------------------
NiActorManager* NiActorManager::Clone(NiCloningProcess* pkCloningProcess)
{
    // Clone the NIF root.
    bool bDeleteCloningProcess = false;
    if (!pkCloningProcess)
    {
        pkCloningProcess = NiNew NiCloningProcess();
        bDeleteCloningProcess = true;
    }
    pkCloningProcess->m_eCopyType = NiObjectNET::COPY_EXACT;
    NiAVObject* pkNIFRoot = (NiAVObject*) m_spNIFRoot->Clone(
        *pkCloningProcess);
    if (bDeleteCloningProcess)
    {
        NiDelete pkCloningProcess;
    }

    // Get the controller manager from the clone.
    NiControllerManager* pkManager = FindControllerManager(pkNIFRoot);
    NIASSERT(pkManager);

    // Create new actor manager.
    NiActorManager* pkActorManager = NiNew NiActorManager(m_spKFMTool,
        m_bCumulativeAnimations);
    pkActorManager->m_spNIFRoot = pkNIFRoot;
    pkActorManager->m_spManager = pkManager;

    pkActorManager->m_spBoneLOD = FindBoneLODController(
        pkActorManager->m_spNIFRoot);

    // Copy member data.
    pkActorManager->m_pkCallbackObject = m_pkCallbackObject;
    NiTMapIterator kIter = m_kSequenceMap.GetFirstPos();
    while (kIter)
    {
        SequenceID eSequenceID;
        NiControllerSequence* pkOrigSequence;
        m_kSequenceMap.GetNext(kIter, eSequenceID, pkOrigSequence);

        NiControllerSequence* pkNewSequence = pkActorManager->m_spManager
            ->GetSequenceByName(pkOrigSequence->GetName());
        pkActorManager->m_kSequenceMap.SetAt(eSequenceID, pkNewSequence);
    }
    for (unsigned int ui = 0; ui < m_kCallbacks.GetSize(); ui++)
    {
        CallbackData* pkData = m_kCallbacks.GetAt(ui);
        if (pkData)
        {
            pkActorManager->m_kCallbacks.AddFirstEmpty(pkData);
        }
    }

    return pkActorManager;
}
//---------------------------------------------------------------------------
NiControllerManager* NiActorManager::FindControllerManager(
    NiAVObject* pkObject)
{
    NiControllerManager* pkManager = NiGetController(NiControllerManager,
        pkObject);
    if (pkManager)
    {
        return pkManager;
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                pkManager = FindControllerManager(pkChild);
                if (pkManager)
                {
                    return pkManager;
                }
            }
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiBoneLODController* NiActorManager::FindBoneLODController(
    NiAVObject* pkObject)
{
    NiBoneLODController* pkBoneLOD = NiGetController(NiBoneLODController,
        pkObject);
    if (pkBoneLOD)
    {
        return pkBoneLOD;
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                pkBoneLOD = FindBoneLODController(pkChild);
                if (pkBoneLOD)
                {
                    return pkBoneLOD;
                }
            }
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
void NiActorManager::Update(float fTime)
{
    NIMETRICS_ANIMATION_SCOPETIMER(AM_UPDATE_TIME);

    NiKFMTool::KFM_RC eRC;

    m_fTime = fTime;

    ProcessCallbacks();

    if (m_eTransitionState != NO_TRANSITION)
    {
        if (m_bStartTransition && m_fTime >= m_fTransStartTime)
        {
            if (m_eTransitionState == BLENDING)
            {
                float fDuration = m_fTransEndTime - m_fTransStartTime;
                NIVERIFY(m_spManager->BlendFromSequence(
                    m_pkCurSequence, m_pkNextSequence, fDuration,
                    m_pcTargetKey));
                m_fTransStartTime = m_fTime;
                m_fTransEndTime = m_fTransStartTime + fDuration;
            }
            m_bStartTransition = false;
        }

        if (m_fTime > m_fTransEndTime)
        {
            m_eTransitionState = NO_TRANSITION;

            m_eCurID = m_eNextID;
            m_pkCurSequence = m_pkNextSequence;
            m_eNextID = INVALID_SEQUENCE_ID;
            m_pkNextSequence = NULL;
            m_bBuildTimeline = true;
        }

        return;
    }

    if (m_fTime > m_fTimelineRefreshTime)
    {
        m_bBuildTimeline = false;
        BuildTimeline();
    }

    SequenceID eDesiredID;

    // Handle animation chains.
    if (m_bChainActive)
    {
        if (m_fNextChainTime == INVALID_TIME)
        {
            NIASSERT(!m_kChainIDs.IsEmpty() && !m_kChainDurations.IsEmpty());
            eDesiredID = m_kChainIDs.RemoveHead();
            float fDuration = m_kChainDurations.RemoveHead();
            if (!m_kChainIDs.IsEmpty())
            {
                if (fDuration == NiKFMTool::MAX_DURATION)
                {
                    NiControllerSequence* pkSequence = GetSequence(
                        eDesiredID);
                    NIASSERT(pkSequence);

                    // Get the time until the end of the sequence.
                    NIASSERT(eDesiredID == m_eCurID);
                    fDuration = pkSequence->GetTimeAt(GetDefaultEndTextKey(),
                        m_fTime) - m_fTime;

                    // This is to compensate for the fact that there
                    // is a frame delay. We assume no worse than 30fps.
                    fDuration -= 0.0333f;
                    if (fDuration < 0.0f)
                        fDuration = 0.0f;
                }
                m_fNextChainTime = m_fTime + fDuration;
                return;
            }
            else
            {
                NIASSERT(m_kChainDurations.IsEmpty());
                m_fNextChainTime = INVALID_TIME;
                m_bChainActive = false;
            }
        }
        else if (m_fTime > m_fNextChainTime)
        {
            NIASSERT(!m_kChainIDs.IsEmpty() && !m_kChainDurations.IsEmpty());
            eDesiredID = m_kChainIDs.GetHead();
            m_fNextChainTime = INVALID_TIME;
        }
        else
        {
            return;
        }
    }
    else
    {
        eDesiredID = m_eTargetID;
    }

    // If desired sequence is already animating, don't do anything.
    if (eDesiredID == m_eCurID)
    {
        if (m_bBuildTimeline)
        {
            m_bBuildTimeline = false;
            BuildTimeline();
        }
        return;
    }

    // If target sequence is invalid, deactivate all sequences.
    if (eDesiredID == INVALID_SEQUENCE_ID)
    {
        RaiseAnimDeactivatedEvents(m_eCurID);

        m_eCurID = INVALID_SEQUENCE_ID;
        if (m_pkCurSequence)
        {
            m_spManager->DeactivateSequence(m_pkCurSequence);
            m_pkCurSequence = NULL;
        }
        return;
    }

    // If there is no current sequence, activate the desired one and return.
    if (m_eCurID == INVALID_SEQUENCE_ID)
    {
        NiControllerSequence* pkDesiredSequence = GetSequence(eDesiredID);
        NIASSERT(pkDesiredSequence);
        NIVERIFY(m_spManager->ActivateSequence(pkDesiredSequence, 0, true));

        m_eCurID = eDesiredID;
        m_pkCurSequence = pkDesiredSequence;

        RaiseAnimActivatedEvents(m_eCurID);

        m_spManager->Update(m_fTime);
        m_bBuildTimeline = false;
        BuildTimeline();
        ProcessCallbacks();

        return;
    }

    // Determine if transition is allowed.
    bool bTransAllowed = false;
    eRC = m_spKFMTool->IsTransitionAllowed(m_eCurID, eDesiredID,
        bTransAllowed);
    NIASSERT(eRC == NiKFMTool::KFM_SUCCESS);
    if (bTransAllowed)
    {
        // Get desired sequence.
        NiControllerSequence* pkDesiredSequence = GetSequence(eDesiredID);
        NIASSERT(pkDesiredSequence);

        // Retrieve the desired transition.
        NiKFMTool::Transition* pkTransition = m_spKFMTool->GetTransition(
            m_eCurID, eDesiredID);
        NIASSERT(pkTransition);

        // Store the chain animations.
        if (pkTransition->GetType() == NiKFMTool::TYPE_CHAIN)
        {
            // Assert that a chain is not already active. Nested chains are
            // not supported.
            NIASSERT(!m_bChainActive);

            // Assert that this transition does not contain an infinite chain.
            // This check can be expensive, so only do for debug builds.
            NIASSERT(m_spKFMTool->IsValidChainTransition(m_eCurID, eDesiredID,
                pkTransition));

            m_kChainIDs.RemoveAll();
            m_kChainDurations.RemoveAll();

            unsigned int uiNumChainAnims =
                pkTransition->GetChainInfo().GetSize();
            NIASSERT(uiNumChainAnims > 0);

            for (unsigned int ui = 0; ui < uiNumChainAnims; ui++)
            {
                unsigned int uiChainID = pkTransition->GetChainInfo()
                    .GetAt(ui).GetSequenceID();
                float fDuration = pkTransition->GetChainInfo().GetAt(ui)
                    .GetDuration();

                m_kChainIDs.AddTail(uiChainID);
                m_kChainDurations.AddTail(fDuration);
            }
            m_kChainIDs.AddTail(eDesiredID);
            m_kChainDurations.AddTail(NiKFMTool::MAX_DURATION);

            eDesiredID = m_kChainIDs.GetHead();
            pkDesiredSequence = GetSequence(eDesiredID);
            NIASSERT(pkDesiredSequence);
            pkTransition = m_spKFMTool->GetTransition(m_eCurID, eDesiredID);
            NIASSERT(pkTransition);
            m_fNextChainTime = INVALID_TIME;
            m_bChainActive = true;
        }

        // Determine start time of transition.
        m_fTransStartTime = m_fTime;
        m_pcTargetKey = NULL;
        if (pkTransition->GetType() == NiKFMTool::TYPE_BLEND)
        {
            if (pkTransition->GetBlendPairs().GetSize() > 0)
            {
                // This update is to ensure that GetTimeAt returns an
                // accurate time.
                m_pkCurSequence->Update(m_fTime, false);

                m_fTransStartTime = NI_INFINITY;
                for (unsigned int ui = 0;
                    ui < pkTransition->GetBlendPairs().GetSize(); ui++)
                {
                    NiKFMTool::Transition::BlendPair* pkPair =
                        pkTransition->GetBlendPairs().GetAt(ui);

                    if (!pkPair->GetStartKey())
                    {
                        m_fTransStartTime = m_fTime;
                        m_pcTargetKey = pkPair->GetTargetKey();
                        break;
                    }

                    float fTempTime = m_pkCurSequence->GetTimeAt(
                        pkPair->GetStartKey(), m_fTime);
                    if (fTempTime != NiControllerSequence::INVALID_TIME &&
                        fTempTime >= m_fTime &&
                        fTempTime < m_fTransStartTime)
                    {
                        m_fTransStartTime = fTempTime;
                        m_pcTargetKey = pkPair->GetTargetKey();
                    }
                }
            }
        }

        // Determine end time of transition.
        NIASSERT(pkTransition->GetDuration() > 0.0f);
        m_fTransEndTime = m_fTransStartTime + pkTransition->GetDuration();

        // Update the destination sequence to ensure that any sequences that
        // are easing out are given a chance to complete.
        pkDesiredSequence->Update(m_fTime, false);

        switch (pkTransition->GetType())
        {
            case NiKFMTool::TYPE_BLEND:
                m_fTransStartFrameTime = m_pkCurSequence->GetLastScaledTime();
                if (m_fTransStartTime == m_fTime)
                {
                    NIVERIFY(m_spManager->BlendFromSequence(
                        m_pkCurSequence, pkDesiredSequence,
                        pkTransition->GetDuration(), m_pcTargetKey));
                }
                else
                {
                    m_fTransStartFrameTime += m_pkCurSequence
                        ->ComputeScaledTime(m_fTransStartTime - m_fTime,
                        false);
                    m_bStartTransition = true;
                }
                m_eTransitionState = BLENDING;
                break;
            case NiKFMTool::TYPE_MORPH:
            {
                m_fTransStartFrameTime = m_pkCurSequence->GetLastScaledTime();
                NIVERIFY(m_spManager->Morph(m_pkCurSequence,
                    pkDesiredSequence, pkTransition->GetDuration()));
                m_eTransitionState = MORPHING;
                break;
            }
            case NiKFMTool::TYPE_CROSSFADE:
            {
                m_fTransStartFrameTime = m_pkCurSequence->GetLastScaledTime();
                NIVERIFY(m_spManager->CrossFade(m_pkCurSequence,
                    pkDesiredSequence, pkTransition->GetDuration(), 0, true));
                m_eTransitionState = CROSSFADING;
                break;
            }
            default:
                NIASSERT(false);
                break;
        }

        m_eNextID = eDesiredID;
        m_pkNextSequence = pkDesiredSequence;
        m_bBuildTimeline = true;
    }

    if (m_bBuildTimeline)
    {
        m_bBuildTimeline = false;
        BuildTimeline();
    }
}
//---------------------------------------------------------------------------
NiActorManager::SequenceID NiActorManager::FindSequenceID(
    const char* pcName) const
{
    // pcName must be non-null
    NIASSERT(pcName);

    NiTMapIterator kItr = m_kSequenceMap.GetFirstPos();
    while (kItr)
    {
        SequenceID eID;
        NiControllerSequence* pkCSeq;
        m_kSequenceMap.GetNext(kItr, eID, pkCSeq);
        if (pkCSeq->GetName() && !strcmp(pcName, pkCSeq->GetName()))
        {
            // found the first one, return it
            return eID;
        }
    }

    // didn't find any with that name
    return INVALID_SEQUENCE_ID;
}
//---------------------------------------------------------------------------
bool NiActorManager::RegisterCallback(EventType eEventType,
    SequenceID eSequenceID, const NiFixedString& kTextKey)
{
    NiTextKeyMatch* pkMatch = NULL;

    if (eEventType == TEXT_KEY_EVENT)
    {
        if (kTextKey.Exists())
            pkMatch = NiNew NiTextKeyMatch( kTextKey );
        else
            return false;
    }

    return RegisterCallback(eEventType, eSequenceID, pkMatch);
}
//---------------------------------------------------------------------------
void NiActorManager::UnregisterCallback(EventType eEventType,
    SequenceID eSequenceID, const NiFixedString& kTextKey)
{
    for (unsigned int ui = 0; ui < m_kCallbacks.GetSize(); ui++)
    {
        CallbackData* pkData = m_kCallbacks.GetAt(ui);

        if (pkData == NULL)
            continue;

        if (pkData->m_eEventType == eEventType &&
            pkData->m_eSequenceID == eSequenceID)
        {
            NiTextKeyMatch* pkCurrMatch = pkData->GetMatchObject();

            // pkCurrMatch will only exist if it is not a text key event,
            // in which case we do not need to check for a match.
            if (pkCurrMatch == NULL || 
                (pkCurrMatch != NULL && pkCurrMatch->IsKeyMatch(kTextKey)))
            {
                RemoveTimelineDataObjects(eEventType, eSequenceID,
                    pkCurrMatch);
                m_kCallbacks.RemoveAt(ui);
                break;
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiActorManager::RegisterCallback(EventType eEventType,
    SequenceID eSequenceID, NiTextKeyMatch* pkMatchObject)
{
    if (eEventType == TEXT_KEY_EVENT)
    {
        if (pkMatchObject == NULL)
            return false;
    }
    else
    {
        pkMatchObject = NULL;
    }

    UnregisterCallback(eEventType, eSequenceID, pkMatchObject);
    m_kCallbacks.AddFirstEmpty(NiNew CallbackData(eEventType,
        eSequenceID, pkMatchObject));
    m_bBuildTimeline = true;

    return true;
}
//---------------------------------------------------------------------------
void NiActorManager::UnregisterCallback(EventType eEventType,
    SequenceID eSequenceID, NiTextKeyMatch* pkMatchObject)
{
    if (eEventType != TEXT_KEY_EVENT)
    {
        pkMatchObject = NULL;
    }

    for (unsigned int ui = 0; ui < m_kCallbacks.GetSize(); ui++)
    {
        CallbackData* pkData = m_kCallbacks.GetAt(ui);
      
        if (pkData && pkData->m_eEventType == eEventType &&
            pkData->m_eSequenceID == eSequenceID &&
            pkData->GetMatchObject() == pkMatchObject)
        {
            RemoveTimelineDataObjects(eEventType, eSequenceID, pkMatchObject);
            m_kCallbacks.RemoveAt(ui);
            break;
        }
    }
}
//---------------------------------------------------------------------------
void NiActorManager::BuildTimeline()
{
    NIASSERT(m_fTime != INVALID_TIME);

    RemoveInactiveExtraSequences();

    m_kTimeline.RemoveAll();

    // Find next event time for each registered callback and add to timeline.
    for (unsigned int ui = 0; ui < m_kCallbacks.GetSize(); ui++)
    {
        CallbackData* pkCallbackData = m_kCallbacks.GetAt(ui);
        if (pkCallbackData)
        {
            SequenceID eSequenceID = pkCallbackData->m_eSequenceID;
            if (eSequenceID == ANY_SEQUENCE_ID)
            {
                NiTMapIterator spos = m_kSequenceMap.GetFirstPos();
                while (spos)
                {
                    NiControllerSequence* pkSequence;
                    m_kSequenceMap.GetNext(spos, eSequenceID, pkSequence);

                    AddTimelineDataObject(pkCallbackData->m_eEventType,
                        eSequenceID, pkCallbackData->GetMatchObject());
                }
            }
            else
            {
                AddTimelineDataObject(pkCallbackData->m_eEventType,
                    eSequenceID, pkCallbackData->GetMatchObject());
            }

        }
    }

    NiTListIterator kIter = m_kTimeline.GetTailPos();
    if (kIter)
    {
        m_fTimelineRefreshTime = m_kTimeline.GetTail()->m_fEventTime;
    }
    else
    {
        m_fTimelineRefreshTime = NI_INFINITY;
    }
}
//---------------------------------------------------------------------------
void NiActorManager::AddTimelineDataObject(EventType eEventType,
    SequenceID eSequenceID, NiTextKeyMatch* pkMatchObject )
{
    float fEventTime = 
        GetNextEventTime(eEventType, eSequenceID, pkMatchObject);

    if (fEventTime != INVALID_TIME)
    {
        bool bInsertedObject = false;
        TimelineData* pkNewTimelineData;
        NiTListIterator kIter = m_kTimeline.GetHeadPos();
        while (kIter)
        {
            TimelineData* pkOldTimelineData = m_kTimeline.Get(kIter);

            if (pkOldTimelineData->m_fEventTime > fEventTime)
            {
                pkNewTimelineData = 
                    m_kTimeline.InsertNewBefore(kIter);

                bInsertedObject = true;
                break;
            }
            kIter = m_kTimeline.GetNextPos(kIter);
        }
        if (!bInsertedObject)
        {
            pkNewTimelineData = m_kTimeline.AddNewTail();
        }

        const char* pcTextKeyMatched = NULL;

        if (pkMatchObject)
        {
            pcTextKeyMatched = pkMatchObject->GetLastMatchedKey();
        }

        pkNewTimelineData->SetValues(fEventTime, eEventType, eSequenceID,
            pkMatchObject, pcTextKeyMatched);
    }
}
//---------------------------------------------------------------------------
void NiActorManager::RemoveTimelineDataObjects(EventType eEventType,
    SequenceID eSequenceID, NiTextKeyMatch* pkMatchObject)
{
    if (eEventType != TEXT_KEY_EVENT)
    {
        pkMatchObject = NULL;
    }

    NiTListIterator kIter = m_kTimeline.GetHeadPos();
    while (kIter)
    {
        NiTListIterator kOldIter = kIter;
        TimelineData* pkData = m_kTimeline.GetNext(kIter);
        if (pkData->m_eEventType == eEventType &&
            (pkData->m_eSequenceID == eSequenceID ||
                eSequenceID == ANY_SEQUENCE_ID) &&
            (!pkMatchObject || pkData->m_spMatchObject == pkMatchObject))
        {
            m_kTimeline.RemovePos(kOldIter);
        }
    }
}
//---------------------------------------------------------------------------
void NiActorManager::ProcessCallbacks()
{
    if (!m_pkCallbackObject || m_kTimeline.IsEmpty())
    {
        return;
    }

    bool bMadeCallback = false;
    while (!m_kTimeline.IsEmpty())
    {
        TimelineData* pkTimelineData = m_kTimeline.GetHead();
        NiTListIterator posHead = m_kTimeline.GetHeadPos();
        if (m_fTime >= pkTimelineData->m_fEventTime)
        {
            switch (pkTimelineData->m_eEventType)
            {
            case ANIM_ACTIVATED:
                m_pkCallbackObject->AnimActivated(this,
                    pkTimelineData->m_eSequenceID, m_fTime,
                    pkTimelineData->m_fEventTime);
                break;
            case ANIM_DEACTIVATED:
                m_pkCallbackObject->AnimDeactivated(this,
                    pkTimelineData->m_eSequenceID, m_fTime,
                    pkTimelineData->m_fEventTime);
                break;
            case TEXT_KEY_EVENT:
                m_pkCallbackObject->TextKeyEvent(this,
                    pkTimelineData->m_eSequenceID,
                    pkTimelineData->m_kTextKeyMatched,
                    pkTimelineData->m_spMatchObject,
                    m_fTime, pkTimelineData->m_fEventTime);
                break;
            case END_OF_SEQUENCE:
                m_pkCallbackObject->EndOfSequence(this,
                    pkTimelineData->m_eSequenceID, m_fTime,
                    pkTimelineData->m_fEventTime);
                break;
            }
            // The head position is checked here because one of the callback
            // functions could have called UnregisterCallback, which would
            // result in items being removed from m_kTimeline. If the head
            // item has already been removed, removing the head again could
            // result in missed callbacks. Thus, we check here that the head
            // of the list is still what is was earlier before removing it.
            if (posHead == m_kTimeline.GetHeadPos())
            {
                m_kTimeline.RemoveHead();
            }
            bMadeCallback = true;
        }
        else
        {
            break;
        }
    }

    if (bMadeCallback)
    {
        m_bBuildTimeline = true;
    }
}
//---------------------------------------------------------------------------
bool NiActorManager::ActivateSequence(SequenceID eSequenceID, int iPriority,
    bool bStartOver, float fWeight, float fEaseInTime,
    SequenceID eTimeSyncSeqID)
{
    NiControllerSequence* pkSequence = GetSequence(eSequenceID);
    if (!pkSequence)
    {
        return false;
    }

    NiControllerSequence* pkTimeSyncSeq = NULL;
    if (eTimeSyncSeqID != NiKFMTool::SYNC_SEQUENCE_ID_NONE)
    {
        pkTimeSyncSeq = GetSequence(eTimeSyncSeqID);
        if (!pkTimeSyncSeq)
        {
            return false;
        }
    }

    // Update the sequence without updating interpolators to give it a chance
    // to finish easing out, if necessary. This will prevent failure to
    // activate due to the fact that the sequence will not actually be fully
    // inactive until the next Update.
    if (m_fTime != INVALID_TIME)
    {
        pkSequence->Update(m_fTime, false);
    }

    bool bSuccess = m_spManager->ActivateSequence(pkSequence, iPriority,
        bStartOver, fWeight, fEaseInTime, pkTimeSyncSeq);
    if (bSuccess)
    {
        m_bBuildTimeline = true;

        // Update the sequence here to activate it immediately. This is
        // necessary in order to catch events because the timeline is built
        // before the sequence will be next updated.
        if (m_fTime != INVALID_TIME)
        {
            pkSequence->Update(m_fTime, false);
        }

        // We need to use AddUnique here instead of Add because it is possible
        // that the sequence ID already exists in the set (if the sequence
        // was deactivated but not yet removed from the set).
        m_kExtraSequences.AddUnique(eSequenceID);

        if (fEaseInTime == 0.0f)
        {
            // If the ease-in time is 0.0, we need to immediately raise any
            // AnimActivated events for the sequence.
            RaiseAnimActivatedEvents(eSequenceID);
        }
    }

    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiActorManager::DeactivateSequence(SequenceID eSequenceID,
    float fEaseOutTime)
{
    NiControllerSequence* pkSequence = GetSequence(eSequenceID);
    if (!pkSequence)
    {
        return false;
    }

    if (pkSequence->GetState() == NiControllerSequence::EASEOUT ||
        pkSequence->GetState() == NiControllerSequence::EASEIN)
    {
        return false;
    }

    // Update the sequence without updating interpolators to give it a chance
    // to finish easing in, if necessary. This will prevent failure to
    // deactivate due to the fact that the sequence will not actually be fully
    // active until the next Update.
    if (m_fTime != INVALID_TIME)
    {
        pkSequence->Update(m_fTime, false);
    }

    bool bSuccess = m_spManager->DeactivateSequence(pkSequence, 
        fEaseOutTime);
    if (bSuccess)
    {
        m_bBuildTimeline = true;

        // Update the sequence here to deactivate it immediately. This is
        // necessary in order to catch events because the timeline is built
        // before the sequence will be next updated.
        if (m_fTime != INVALID_TIME)
        {
            pkSequence->Update(m_fTime, false);
        }

        if (fEaseOutTime == 0.0f)
        {
            // If the ease-out time is 0.0, we can immediately remove the
            // sequence from the extra sequences set.
            int iIndex = m_kExtraSequences.Find(eSequenceID);
            NIASSERT(iIndex != -1);
            if (iIndex != -1)
            {
                m_kExtraSequences.RemoveAt(iIndex);
            }

            // If the ease-out time is 0.0, we need to immediately raise any
            // AnimDeactivated events for the sequence.
            RaiseAnimDeactivatedEvents(eSequenceID);
        }
    }

    return bSuccess;
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiActorManager::ChangeSequenceID(SequenceID eOldID,
    SequenceID eNewID)
{
    // Update KFM tool.
    NiKFMTool::KFM_RC eRC = m_spKFMTool->UpdateSequenceID(eOldID, eNewID);

    // Update stored IDs.
    if (m_eTargetID == eOldID)
    {
        m_eTargetID = eNewID;
    }
    if (m_eCurID == eOldID)
    {
        m_eCurID = eNewID;
    }
    if (m_eNextID == eOldID)
    {
        m_eNextID = eNewID;
    }

    // Update chain IDs.
    NiTListIterator pos_chain = m_kChainIDs.GetHeadPos();
    while (pos_chain)
    {
        unsigned int uiSequenceID = m_kChainIDs.Get(pos_chain);
        if (uiSequenceID == eOldID)
        {
            NiTListIterator pos_current = m_kChainIDs.InsertAfter(pos_chain,
                eNewID);
            m_kChainIDs.RemovePos(pos_chain);
            pos_chain = pos_current;
        }
        pos_chain = m_kChainIDs.GetNextPos(pos_chain);
    }

    // Update sequence map.
    NiTMapIterator pos_seq = m_kSequenceMap.GetFirstPos();
    while (pos_seq)
    {
        SequenceID eSequenceID;
        NiControllerSequence* pkSequence;
        m_kSequenceMap.GetNext(pos_seq, eSequenceID, pkSequence);
        if (eSequenceID == eOldID)
        {
            m_kSequenceMap.RemoveAt(eSequenceID);
            m_kSequenceMap.SetAt(eNewID, pkSequence);
        }
    }

    // Update callback data.
    unsigned int ui;
    for (ui = 0; ui < m_kCallbacks.GetSize(); ui++)
    {
        CallbackData* pkCallbackData = m_kCallbacks.GetAt(ui);
        if (pkCallbackData && pkCallbackData->m_eSequenceID == eOldID)
        {
            pkCallbackData->m_eSequenceID = eNewID;
        }
    }

    // Update timeline data.
    while (!m_kTimeline.IsEmpty())
    {
        TimelineData* pkTimelineData = m_kTimeline.GetHead();
        if (pkTimelineData->m_eSequenceID == eOldID)
        {
            pkTimelineData->m_eSequenceID = eNewID;
        }
    }

    // Update extra sequences.
    for (ui = 0; ui < m_kExtraSequences.GetSize(); ui++)
    {
        if (m_kExtraSequences.GetAt(ui) == eOldID)
        {
            m_kExtraSequences.ReplaceAt(ui, eNewID);
        }
    }

    return eRC;
}
//---------------------------------------------------------------------------
bool NiActorManager::ReloadNIFFile(NiStream* pkStream, bool bLoadNIFFile)
{
    if (!bLoadNIFFile && !pkStream)
    {
        return false;
    }

    // Create stream if not provided.
    bool bDeleteStream = false;
    if (!pkStream)
    {
        pkStream = NiNew NiStream;
        bDeleteStream = true;
    }

    bool bSuccess = LoadNIFFile(*pkStream, bLoadNIFFile);

    // Delete stream if created earlier.
    if (bDeleteStream)
    {
        NiDelete pkStream;
    }

    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiActorManager::ChangeNIFRoot(NiAVObject* pkNIFRoot)
{
    if (!pkNIFRoot)
    {
        return false;
    }

    NIASSERT(m_spKFMTool);

    // Get model root name.
    const char* pcModelRoot = m_spKFMTool->GetModelRoot();
    if (!pcModelRoot)
    {
        return false;
    }

    // Find the actor root.
    NiAVObject* pkActorRoot = pkNIFRoot->GetObjectByName(pcModelRoot);
    if (!pkActorRoot)
    {
        return false;
    }

    // Turn off all sequences.
    Reset();

    // Create the controller manager.
    NiControllerManager* pkManager = NiNew NiControllerManager(pkActorRoot,
        m_bCumulativeAnimations);

    // Copy all sequences from one manager to another.
    if (m_spManager)
    {
        NiTObjectArray<NiInterpControllerPtr> kCtlrs;
        for (unsigned int ui = 0; ui < m_spManager->GetSequenceCount(); ui++)
        {
            // Get sequence.
            NiControllerSequencePtr spSequence =
                m_spManager->GetSequenceAt(ui);

            if (!spSequence)
                continue;

            // Save controllers in smart pointers. These controllers need
            // to be saved because RemoveSequence will clear them out. When
            // the sequence is added to the new manager, the new scene graph
            // may be an old NIF file that does not contain controllers, so
            // the controllers need to be preserved here and added back after
            // the sequence is removed from the old manager.
            unsigned int uiCtlrCount = spSequence->GetArraySize();
            kCtlrs.RemoveAll();
            if (uiCtlrCount > kCtlrs.GetAllocatedSize())
            {
                kCtlrs.SetSize(uiCtlrCount);
            }
            unsigned int uiCtlr;
            for (uiCtlr = 0; uiCtlr < uiCtlrCount; uiCtlr++)
            {
                // NULL controller entries are intentionally added to kCtlrs
                // here. If it is not done this way, the sequence's IDTag
                // array will become out of sync with its interp array.
                NiTimeController* pkCtlr = spSequence->GetControllerAt(
                    uiCtlr);
                NIASSERT(!pkCtlr || NiIsKindOf(NiInterpController, pkCtlr));
                kCtlrs.Add((NiInterpController*) pkCtlr);
            }

            // Remove sequence.
            m_spManager->RemoveSequence(spSequence);

            // Restore controllers.
            for (uiCtlr = 0; uiCtlr < uiCtlrCount; uiCtlr++)
            {
                spSequence->SetControllerAt(kCtlrs.GetAt(uiCtlr), uiCtlr);
            }

            // Add sequence.
            pkManager->AddSequence(spSequence);
        }
    }

    m_spNIFRoot = pkNIFRoot;
    m_spManager = pkManager;
    m_spBoneLOD = FindBoneLODController(m_spNIFRoot);

    return true;
}
//---------------------------------------------------------------------------
bool NiActorManager::LoadSequence(SequenceID eSequenceID, bool bLoadKFFile,
    NiStream* pkStream)
{
    if (!bLoadKFFile && !pkStream)
    {
        return false;
    }

    // Create stream if not provided.
    bool bDeleteStream = false;
    if (!pkStream)
    {
        pkStream = NiNew NiStream;
        bDeleteStream = true;
    }

    bool bSuccess = AddSequence(eSequenceID, *pkStream, bLoadKFFile);

    // Delete stream if created earlier.
    if (bDeleteStream)
    {
        NiDelete pkStream;
    }

    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiActorManager::ChangeSequence(SequenceID eSequenceID,
    NiControllerSequence* pkSequence)
{
    // Remove old sequence mapped to this ID, if it exists.
    UnloadSequence(eSequenceID);

    bool bSuccess = m_spManager->AddSequence(pkSequence);
    if (bSuccess)
    {
        m_kSequenceMap.SetAt(eSequenceID, pkSequence);
    }

    return bSuccess;
}
//---------------------------------------------------------------------------
void NiActorManager::UnloadSequence(SequenceID eSequenceID)
{
    NIASSERT(m_spManager);

    NiControllerSequence* pkSequence = GetSequence(eSequenceID);
    if (!pkSequence)
    {
        return;
    }

    int iIndex = m_kExtraSequences.Find(eSequenceID);

    if (eSequenceID == m_eCurID || eSequenceID == m_eNextID ||
        eSequenceID == m_eTargetID || iIndex > -1)
    {
        Reset();
    }

    m_spManager->RemoveSequence(pkSequence);
    m_kSequenceMap.RemoveAt(eSequenceID);

    // m_kExtraSequences is emptied by Reset, so the sequence need not be
    // removed from it here.
}
//---------------------------------------------------------------------------
bool NiActorManager::FindTimeForAnimationToCompleteTransition(
    unsigned int uiTransSrcID, unsigned int uiTransDesID,
    NiKFMTool::Transition* pkTransition, float fBeginFrame,
    float fDesiredTransitionFrame, NiActorManager::CompletionInfo* pkInfoOut)
{
    NIASSERT(pkInfoOut);

    NIASSERT(pkTransition);
    NiControllerSequence* pkSrcSequence = GetSequence(uiTransSrcID);
    NiControllerSequence* pkDesSequence = GetSequence(uiTransDesID);
    NIASSERT(pkSrcSequence && pkDesSequence);

    NIASSERT(fDesiredTransitionFrame >= 0.0);
    NIASSERT(fDesiredTransitionFrame >= fBeginFrame);

    switch (pkTransition->GetType())
    {
        case NiKFMTool::TYPE_BLEND:
        {
            bool bImmediateBlend;
            if (pkTransition->GetBlendPairs().GetSize() == 0)
            {
                bImmediateBlend = true;
            }
            else if (pkTransition->GetBlendPairs().GetSize() == 1 &&
                !pkTransition->GetBlendPairs().GetAt(0)->
                GetStartKey().Exists())
            {
                bImmediateBlend = true;
            }
            else
            {
                bImmediateBlend = false;
            }

            if (bImmediateBlend)
            {
                // Handle immediate blend.
                float fBeginToTransitionFrameTime = 
                    fDesiredTransitionFrame - fBeginFrame;

                const char* pcImmediateOffsetTextKey = "start";
                if (pkTransition->GetBlendPairs().GetSize() == 1)
                {
                    pcImmediateOffsetTextKey = pkTransition->GetBlendPairs()
                        .GetAt(0)->GetTargetKey();
                }

                pkInfoOut->m_fFrameInDestWhenTransitionCompletes =
                    pkDesSequence->GetKeyTimeAt(pcImmediateOffsetTextKey);

                // TimeToCompleteTransition is in RealTime
                pkInfoOut->m_fTimeToCompleteTransition =
                    pkSrcSequence->TimeDivFreq(fBeginToTransitionFrameTime) + 
                    pkTransition->GetDuration();

                pkInfoOut->m_fFrameTransitionOccursInSrc =
                    fDesiredTransitionFrame;
            }
            else
            {
                // Handle delayed blend.

                // Given the time and minimal step time, determine what the
                // next blend pair will be and the time it'll take to reach
                // it.
                float fActualTransitionUnboundedFrame = 0;
                NiKFMTool::Transition::BlendPair* pkBlendPair =
                    GetNextBlendPair(uiTransSrcID, uiTransDesID,
                    pkTransition, fDesiredTransitionFrame,
                    fActualTransitionUnboundedFrame);
                pkInfoOut->m_pkBlendPair = pkBlendPair;

                if (pkBlendPair == NULL)
                {
                    // This could happen if the transition time was beyond
                    // the length of the sequence and the sequence was
                    // CLAMPED.

                    pkInfoOut->m_fFrameTransitionOccursInSrc =
                        pkSrcSequence->GetEndKeyTime();

                    pkInfoOut->m_fFrameInDestWhenTransitionCompletes = 
                        pkDesSequence->GetBeginKeyTime();

                    pkInfoOut->m_fTimeToCompleteTransition =
                        pkSrcSequence->TimeDivFreq(
                        fActualTransitionUnboundedFrame - fBeginFrame) +
                        pkTransition->GetDuration();
                }
                else
                {
                    const char* pcStartKey = pkBlendPair->GetStartKey();
                    if (pcStartKey == NULL)
                    {
                        pcStartKey = "start";
                    }

                    float fActualTransitionFrame =
                        pkSrcSequence->GetKeyTimeAt(pcStartKey);
                    NIASSERT(fActualTransitionFrame !=
                        NiControllerSequence::INVALID_TIME);

                    pkInfoOut->m_fFrameTransitionOccursInSrc =
                        fActualTransitionFrame;

                    const char* pcTargetKey = pkBlendPair->GetTargetKey();
                    if (pcTargetKey == NULL)
                    {
                        pcTargetKey = "start";
                    }
                    pkInfoOut->m_fFrameInDestWhenTransitionCompletes = 
                        pkDesSequence->GetKeyTimeAt(pcTargetKey);

                    pkInfoOut->m_fTimeToCompleteTransition =
                        pkSrcSequence->TimeDivFreq(
                        fActualTransitionUnboundedFrame - fBeginFrame) +
                        pkTransition->GetDuration();
                }
            }

            break;
        }
        case NiKFMTool::TYPE_MORPH:
        {
            pkInfoOut->m_fFrameTransitionOccursInSrc =
                fDesiredTransitionFrame;
            pkInfoOut->m_fTimeToCompleteTransition =
                pkSrcSequence->TimeDivFreq(fDesiredTransitionFrame -
                fBeginFrame) + pkTransition->GetDuration();

            float fDesiredTransitionTime =
                pkSrcSequence->TimeDivFreq(fDesiredTransitionFrame);

            float fDestTransitionStartFrame = 
                pkDesSequence->FindCorrespondingMorphFrame(
                    pkSrcSequence, fDesiredTransitionTime);

            float fPartialDestSeqFrameLength = 
                pkDesSequence->GetEndKeyTime()- fDestTransitionStartFrame;

            float fTranDuration = pkTransition->GetDuration();
            float fTranFrameDuration;

            if (fTranDuration == NiKFMTool::MAX_DURATION)
            {
                // The time of transition duration should never be
                // negative... This code is being pessimistic...
                // we should never hit this block.
                NIASSERT(fTranDuration != NiKFMTool::MAX_DURATION);
                fTranFrameDuration = (pkDesSequence->GetEndKeyTime() -
                    pkDesSequence->GetBeginKeyTime()) - fBeginFrame;
            }
            else
            {
                 fTranFrameDuration = pkDesSequence->TimeMultFreq(
                     fTranDuration);
            }


            if (fTranFrameDuration > fPartialDestSeqFrameLength)
            {
                // We have to loop ... not as easy as a cross fade because
                // we can start anywhere in the destination sequence.
                float fDesFrameLength = pkDesSequence->GetEndKeyTime() -
                    pkDesSequence->GetBeginKeyTime();

                float fRemainingFrameDuration = fTranFrameDuration -
                    fPartialDestSeqFrameLength;

                float fCeiling = 
                    ceilf(fRemainingFrameDuration / fDesFrameLength);

                float fTimeOfDestLoops = (fCeiling * fDesFrameLength);

                float fFinal = (fDesFrameLength - fDestTransitionStartFrame) +
                    fTimeOfDestLoops;
                pkInfoOut->m_fFrameInDestWhenTransitionCompletes =
                    fDesFrameLength - (fFinal - fTranFrameDuration);
            }
            else
            {
                pkInfoOut->m_fFrameInDestWhenTransitionCompletes =
                    fDestTransitionStartFrame + fTranFrameDuration;
            }

            break;
        }
        case NiKFMTool::TYPE_CROSSFADE:
        {
            pkInfoOut->m_fFrameTransitionOccursInSrc =
                fDesiredTransitionFrame;

            float fTranDuration = pkTransition->GetDuration();
            pkInfoOut->m_fTimeToCompleteTransition =
                pkSrcSequence->TimeDivFreq(fDesiredTransitionFrame -
                fBeginFrame) + fTranDuration;

            // Assume that we always start cross fade at start of destination.
            // If a cross fade has a duration that is longer than the
            // destination sequence, then the destination sequence should loop
            // and our time to start in the destination will depend on where
            // the duration ends within the loop...
            float fDesFrameLength = pkDesSequence->GetEndKeyTime() -
                pkDesSequence->GetBeginKeyTime();
            float fTranFrameDuration = pkDesSequence->TimeMultFreq(
                fTranDuration);

            if (fTranFrameDuration > fDesFrameLength)
            {
                if (pkDesSequence->GetCycleType() == NiTimeController::CLAMP)
                {
                    pkInfoOut->m_fFrameInDestWhenTransitionCompletes = 
                        fDesFrameLength;
                }
                else
                {
                    float fCeiling = ceilf(fTranFrameDuration /
                        fDesFrameLength);
                    float fTimeOfDestLoops = (fCeiling * fDesFrameLength);

                    // Note that this is an unbounded frame time
                    pkInfoOut->m_fFrameInDestWhenTransitionCompletes = 
                        fDesFrameLength - (fTimeOfDestLoops -
                        fTranFrameDuration);
                }
            }
            else
            {
                pkInfoOut->m_fFrameInDestWhenTransitionCompletes =
                    fTranFrameDuration;
            }

            break;
        }
        case NiKFMTool::TYPE_CHAIN:
        {
            float fTotalTime = 0.0f;

            // Ensure that Chains exist
            unsigned int uiChainInfoSize = 
                pkTransition->GetChainInfo().GetSize();

            NIASSERT(uiChainInfoSize > 0);

            // Get first chain and its transition information
            NiControllerSequence* pkSrcSeq = pkSrcSequence;
            unsigned int uiSrcID = uiTransSrcID;
            NiKFMTool::Transition::ChainInfo* pkChainInfo;

            // Find completion info of first transition of chain
            float fSrcBeginFrame = pkSrcSeq->GetBeginKeyTime();
            ChainCompletionInfo* pkCCI;
            float fFrameDuration = fDesiredTransitionFrame;

            unsigned int uiIndex;
            for(uiIndex = 0; uiIndex < uiChainInfoSize; uiIndex++)
            {
                pkChainInfo = &pkTransition->GetChainInfo().GetAt(uiIndex);
                unsigned int uiDesID = pkChainInfo->GetSequenceID();
                NiControllerSequence* pkDesSeq = GetSequence(uiDesID);

                pkCCI =
                    FillChainComplete(uiSrcID, uiDesID, fFrameDuration,
                    fSrcBeginFrame, fTotalTime);

                pkInfoOut->m_kChainCompletionInfoSet.Add(pkCCI);

                // now make this chain the source...
                pkSrcSeq = pkDesSeq;
                uiSrcID = uiDesID;

                // Place Duration in unbounded frame time (a bit weird)
                fFrameDuration = pkChainInfo->GetDuration();
                if (fFrameDuration == NiKFMTool::MAX_DURATION)
                {
                    fFrameDuration = (pkSrcSeq->GetEndKeyTime() -
                        pkSrcSeq->GetBeginKeyTime()) - fSrcBeginFrame;
                }
                else
                {
                    // scale duration so that it is in frame time
                        fFrameDuration = pkDesSeq->TimeMultFreq(
                            fFrameDuration);
                }
            }

            pkCCI = FillChainComplete(uiSrcID, uiTransDesID,
                fFrameDuration, fSrcBeginFrame, fTotalTime);
            pkInfoOut->m_kChainCompletionInfoSet.Add(pkCCI);

            pkSrcSeq = GetSequence(uiSrcID);
            fTotalTime += 
                pkDesSequence->GetEndKeyTimeDivFreq() - 
                pkSrcSeq->TimeDivFreq(fSrcBeginFrame);
            pkInfoOut->m_fTimeForChainToComplete = fTotalTime;
            break;
        }
        default:
            NIASSERT(false);
            return false;
            break;
    }

    return true;
}
//---------------------------------------------------------------------------
float NiActorManager::GetNextAnimActivatedTime(SequenceID eSequenceID)
{
    // This function returns the next time that the indicated animation will
    // be fully activated (i.e., not being blended or morphed). If the
    // animation is already active or the next activation time cannot be
    // determined, INVALID_TIME is returned.

    NIASSERT(m_fTime != INVALID_TIME && eSequenceID != INVALID_SEQUENCE_ID);

    // Search for sequence in the extra sequences.
    int iIndex = m_kExtraSequences.Find(eSequenceID);
    if (iIndex != -1)
    {
        NiControllerSequence* pkExtraSeq = GetSequence(eSequenceID);

        // If sequence is inactive, remove it.
        if (pkExtraSeq->GetState() == NiControllerSequence::INACTIVE)
        {
            m_kExtraSequences.RemoveAt(iIndex);
            return INVALID_TIME;
        }

        // If sequence is easing in, return the time when the ease-in will
        // be complete.
        if (pkExtraSeq->GetState() == NiControllerSequence::EASEIN)
        {
            float fEndTime = pkExtraSeq->GetEaseEndTime();
            if (fEndTime > m_fTime)
            {
                return fEndTime;
            }
            else
            {
                return INVALID_TIME;
            }
        }

        // Extra sequence is already activated and will not be activated
        // again in the foreseeable future.
        return INVALID_TIME;
    }
    
    if (eSequenceID == m_eCurID)
    {
        return INVALID_TIME;
    }

    if (eSequenceID == m_eNextID)
    {
        return m_fTransEndTime;
    }

    if (m_eNextID == INVALID_SEQUENCE_ID ||
        !m_kChainIDs.FindPos(eSequenceID))
    {
        return INVALID_TIME;
    }

    float fAnimActivatedTime = m_fTransEndTime;
    NiKFMTool::Transition* pkTransition = m_spKFMTool->GetTransition(m_eCurID,
        m_eNextID);
    NIASSERT(pkTransition && pkTransition->GetType() != NiKFMTool::TYPE_CHAIN);
    float fTransStartFrameTime = m_fTransStartFrameTime;
    if (fTransStartFrameTime == INVALID_TIME)
    {
        fTransStartFrameTime = 0.0f;
    }
    CompletionInfo kInfo;
    NIVERIFY(FindTimeForAnimationToCompleteTransition(
        m_eCurID, m_eNextID, pkTransition, fTransStartFrameTime,
        fTransStartFrameTime, &kInfo));
    
    SequenceID eSrcID = m_eNextID;
    NiControllerSequence* pkSrcSequence = m_pkNextSequence;
    float fStartFrame = kInfo.m_fFrameInDestWhenTransitionCompletes;

    float fTransitionFrame = 0.0f;
    NiTListIterator posID = m_kChainIDs.GetHeadPos();
    NiTListIterator posDuration = m_kChainDurations.GetHeadPos();
    if (!m_kChainDurations.IsEmpty())
    {
        float fDuration = m_kChainDurations.GetHead();
        if (fDuration == NiKFMTool::MAX_DURATION)
        {
            fDuration = (m_pkNextSequence->GetEndKeyTime() -
                m_pkNextSequence->GetBeginKeyTime()) - fStartFrame;
        }
        else
        {
            // scale duration so that it is in frame time
            fDuration = m_pkNextSequence->TimeMultFreq(fDuration);
        }

        fTransitionFrame = fStartFrame + fDuration;

        NIASSERT(posID && posDuration);
        posID = m_kChainIDs.GetNextPos(posID);
        posDuration = m_kChainDurations.GetNextPos(posDuration);
    }

    while (true)
    {
        if (eSequenceID == eSrcID)
        {
            return fAnimActivatedTime;
        }

        if (!posID)
        {
            // This statement will break out of this loop when there are no
            // more intermediate animations.
            return INVALID_TIME;
        }
        NIASSERT(posDuration);

        SequenceID eChainID = m_kChainIDs.GetNext(posID);
        float fDuration = m_kChainDurations.GetNext(posDuration);
        NiControllerSequence* pkChainSeq = GetSequence(eChainID);
        NIASSERT(pkChainSeq);

        NiKFMTool::Transition* pkTransition = m_spKFMTool->GetTransition(
            eSrcID, eChainID);
        NIASSERT(pkTransition &&
            pkTransition->GetType() != NiKFMTool::TYPE_CHAIN);
        NIVERIFY(FindTimeForAnimationToCompleteTransition(eSrcID, eChainID,
            pkTransition, fStartFrame, fTransitionFrame, &kInfo));
        fAnimActivatedTime += kInfo.m_fTimeToCompleteTransition;

        eSrcID = eChainID;
        pkSrcSequence = pkChainSeq;
        fStartFrame = kInfo.m_fFrameInDestWhenTransitionCompletes;

        if (fDuration == NiKFMTool::MAX_DURATION)
        {
            fDuration = (pkChainSeq->GetEndKeyTime() -
                pkChainSeq->GetBeginKeyTime()) - fStartFrame;
        }
        else
        {
            // scale duration so that it is in frame time
            fDuration = pkChainSeq->TimeMultFreq(fDuration);
        }

        fTransitionFrame = fStartFrame + fDuration;
    }

    return INVALID_TIME;
}
//---------------------------------------------------------------------------
float NiActorManager::GetNextAnimDeactivatedTime(SequenceID eSequenceID)
{
    // This function returns the next time that the indicated animation will
    // be fully deactivated (i.e., not being blended or morphed). If the next
    // deactivation time cannot be determined, INVALID_TIME is returned.

    NIASSERT(m_fTime != INVALID_TIME && eSequenceID != INVALID_SEQUENCE_ID);

    // Search for sequence in the extra sequences.
    int iIndex = m_kExtraSequences.Find(eSequenceID);
    if (iIndex != -1)
    {
        NiControllerSequence* pkExtraSeq = GetSequence(eSequenceID);

        // If sequence is inactive, remove it.
        if (pkExtraSeq->GetState() == NiControllerSequence::INACTIVE)
        {
            m_kExtraSequences.RemoveAt(iIndex);
            return INVALID_TIME;
        }

        // If sequence is easing out, return the time when the ease-out will
        // be complete.
        if (pkExtraSeq->GetState() == NiControllerSequence::EASEOUT)
        {
            float fEndTime = pkExtraSeq->GetEaseEndTime();
            if (fEndTime > m_fTime)
            {
                return fEndTime;
            }
            else
            {
                return INVALID_TIME;
            }
        }

        // Extra sequences are deactivated manually, so we cannot predict when
        // that will happen.
        return INVALID_TIME;
    }

    if (eSequenceID == m_eCurID)
    {
        if (m_eNextID == INVALID_SEQUENCE_ID)
        {
            return INVALID_TIME;
        }

        if (m_fTime > m_fTransStartTime)
        {
            return INVALID_TIME;
        }

        return m_fTransStartTime;
    }

    if (m_eNextID == INVALID_SEQUENCE_ID ||
        (eSequenceID == m_eNextID && m_kChainIDs.IsEmpty()) ||
        !m_kChainIDs.FindPos(eSequenceID))
    {
        return INVALID_TIME;
    }

    float fAnimDeactivatedTime = m_fTransStartTime;
    NiKFMTool::Transition* pkTransition = m_spKFMTool->GetTransition(
        m_eCurID,m_eNextID);
    NIASSERT(pkTransition && pkTransition->GetType() != NiKFMTool::TYPE_CHAIN);
    float fTransStartFrameTime = m_fTransStartFrameTime;
    if (fTransStartFrameTime == INVALID_TIME)
    {
        fTransStartFrameTime = 0.0f;
    }

    CompletionInfo kInfo;
    NIVERIFY(FindTimeForAnimationToCompleteTransition(
        m_eCurID, m_eNextID, pkTransition, fTransStartFrameTime,
        fTransStartFrameTime, &kInfo));

    SequenceID eSrcID = m_eNextID;
    NiControllerSequence* pkSrcSequence = m_pkNextSequence;
    float fStartFrame = kInfo.m_fFrameInDestWhenTransitionCompletes;

    float fTransitionFrame = 0.0f;
    float fTransitionDuration = pkTransition->GetDuration();
    NiTListIterator posID = m_kChainIDs.GetHeadPos();
    NiTListIterator posDuration = m_kChainDurations.GetHeadPos();
    if (!m_kChainDurations.IsEmpty())
    {
        float fDuration = m_kChainDurations.GetHead();
        if (fDuration == NiKFMTool::MAX_DURATION)
        {
            fDuration = (m_pkNextSequence->GetEndKeyTime() -
                m_pkNextSequence->GetBeginKeyTime()) - fStartFrame;
        }
        else
        {
            // scale duration so that it is in frame time
            fDuration = m_pkNextSequence->TimeMultFreq(fDuration);
        }

        fTransitionFrame = fStartFrame + fDuration;

        NIASSERT(posID && posDuration);
        posID = m_kChainIDs.GetNextPos(posID);
        posDuration = m_kChainDurations.GetNextPos(posDuration);
    }

    while (true)
    {
        if (!posID)
        {
            // This statement will break out of this loop when there are no
            // more intermediate animations.
            return INVALID_TIME;
        }
        NIASSERT(posDuration);

        SequenceID eChainID = m_kChainIDs.GetNext(posID);
        float fDuration = m_kChainDurations.GetNext(posDuration);
        NiControllerSequence* pkChainSeq = GetSequence(eChainID);
        NIASSERT(pkChainSeq);

        NiKFMTool::Transition* pkTransition = m_spKFMTool->GetTransition(
            eSrcID, eChainID);
        NIASSERT(pkTransition &&
            pkTransition->GetType() != NiKFMTool::TYPE_CHAIN);
        NIVERIFY(FindTimeForAnimationToCompleteTransition(eSrcID, eChainID,
            pkTransition, fStartFrame, fTransitionFrame, &kInfo));
        fAnimDeactivatedTime += (kInfo.m_fFrameTransitionOccursInSrc -
            fStartFrame) + fTransitionDuration;

        if (eSequenceID == eSrcID)
        {
            return fAnimDeactivatedTime;
        }

        eSrcID = eChainID;
        pkSrcSequence = pkChainSeq;
        fStartFrame = kInfo.m_fFrameInDestWhenTransitionCompletes;

        if (fDuration == NiKFMTool::MAX_DURATION)
        {
            fDuration = (pkChainSeq->GetEndKeyTime() -
                pkChainSeq->GetBeginKeyTime()) - fStartFrame;
        }
        else
        {
            // scale duration so that it is in frame time
            fDuration = pkChainSeq->TimeMultFreq(fDuration);
        }

        fTransitionFrame = fStartFrame + fDuration;
        fTransitionDuration = pkTransition->GetDuration();
    }

    return INVALID_TIME;
}
//---------------------------------------------------------------------------
float NiActorManager::GetNextTextKeyEventTime(SequenceID eSequenceID,
    const NiFixedString& kTextKey)
{
    NiTextKeyMatch kMatchObject(kTextKey);
    return GetNextTextKeyEventTime(eSequenceID, &kMatchObject);
}
//---------------------------------------------------------------------------
float NiActorManager::GetNextTextKeyEventTime(SequenceID eSequenceID,
    NiTextKeyMatch* pkMatchObject )
{
    // This function returns the next time that the specified text key in the
    // indicated animation will occur. If the next text key event time cannot
    // be determined or the text key cannot be found, INVALID_TIME is
    // returned.

    NIASSERT(pkMatchObject && m_fTime != INVALID_TIME &&
        eSequenceID != INVALID_SEQUENCE_ID);

    float fTextKeyTime;

    // Search for sequence in the extra sequences.
    int iIndex = m_kExtraSequences.Find(eSequenceID);
    if (iIndex != -1)
    {
        NiControllerSequence* pkExtraSeq = GetSequence(eSequenceID);
        if (pkExtraSeq->GetState() == NiControllerSequence::INACTIVE)
        {
            m_kExtraSequences.RemoveAt(iIndex);
            return INVALID_TIME;
        }

        fTextKeyTime = pkExtraSeq->GetTimeAt(pkMatchObject, m_fTime);
        if (fTextKeyTime == NiControllerSequence::INVALID_TIME ||
            fTextKeyTime < m_fTime ||
            (pkExtraSeq->GetState() == NiControllerSequence::EASEOUT &&
            fTextKeyTime > pkExtraSeq->GetEaseEndTime()))
        {
            return INVALID_TIME;
        }

        return fTextKeyTime;
    }

    if (eSequenceID == m_eCurID)
    {
        fTextKeyTime = m_pkCurSequence->GetTimeAt(pkMatchObject, m_fTime);
        if (fTextKeyTime == NiControllerSequence::INVALID_TIME ||
            fTextKeyTime < m_fTime ||
            (fTextKeyTime == m_fTime &&
                m_pkCurSequence->GetKeyTimeAt(pkMatchObject) ==
                m_pkCurSequence->GetEndKeyTime()))
        {
            return INVALID_TIME;
        }

        if (m_eNextID != INVALID_SEQUENCE_ID &&
            fTextKeyTime > m_fTransStartTime)
        {
            return INVALID_TIME;
        }

        return fTextKeyTime;
    }

    if (m_eNextID == INVALID_SEQUENCE_ID ||
        (eSequenceID != m_eNextID && m_kChainIDs.IsEmpty()) ||
        (eSequenceID != m_eNextID && !m_kChainIDs.FindPos(eSequenceID)))
    {
        return INVALID_TIME;
    }

    fTextKeyTime = m_fTransEndTime;
    NiKFMTool::Transition* pkTransition = m_spKFMTool->GetTransition(
        m_eCurID, m_eNextID);
    NIASSERT(pkTransition && pkTransition->GetType() != NiKFMTool::TYPE_CHAIN);
    float fTransStartFrameTime = m_fTransStartFrameTime;
    if (fTransStartFrameTime == INVALID_TIME)
    {
        fTransStartFrameTime = 0.0f;
    }
    CompletionInfo kInfo;
    NIVERIFY(FindTimeForAnimationToCompleteTransition(
        m_eCurID, m_eNextID, pkTransition, fTransStartFrameTime,
        fTransStartFrameTime, &kInfo));

    SequenceID eSrcID = m_eNextID;
    NiControllerSequence* pkSrcSequence = m_pkNextSequence;
    float fStartFrame = kInfo.m_fFrameInDestWhenTransitionCompletes;

    float fTransitionFrame = 0.0f;
    NiTListIterator posID = m_kChainIDs.GetHeadPos();
    NiTListIterator posDuration = m_kChainDurations.GetHeadPos();
    if (!m_kChainDurations.IsEmpty())
    {
        float fDuration = m_kChainDurations.GetHead();
        if (fDuration == NiKFMTool::MAX_DURATION)
        {
            fDuration = (m_pkNextSequence->GetEndKeyTime() -
                m_pkNextSequence->GetBeginKeyTime()) - fStartFrame;
        }
        else
        {
            // scale duration so that it is in frame time
            fDuration = m_pkNextSequence->TimeMultFreq(fDuration);
        }

        fTransitionFrame = fStartFrame + fDuration;

        NIASSERT(posID && posDuration);
        posID = m_kChainIDs.GetNextPos(posID);
        posDuration = m_kChainDurations.GetNextPos(posDuration);
    }

    while (true)
    {
        if (eSequenceID == eSrcID)
        {
            float fTextKeyFrame = pkSrcSequence->GetKeyTimeAt(pkMatchObject);
            if (fTextKeyFrame == NiControllerSequence::INVALID_TIME ||
                (posID && fTextKeyFrame > fTransitionFrame))
            {
                return INVALID_TIME;
            }

            float fDelayTime = fTextKeyFrame - fStartFrame;
            if (fDelayTime < 0.0f)
            {
                fDelayTime += pkSrcSequence->GetEndKeyTime() -
                    pkSrcSequence->GetBeginKeyTime();
            }
            float fUnscaledTime = fDelayTime / pkSrcSequence->GetFrequency();
            fTextKeyTime += fUnscaledTime;

            return fTextKeyTime;
        }

        if (!posID)
        {
            // This statement will break out of this loop when there are no
            // more intermediate animations.
            return INVALID_TIME;
        }
        NIASSERT(posDuration);

        SequenceID eChainID = m_kChainIDs.GetNext(posID);
        float fDuration = m_kChainDurations.GetNext(posDuration);
        NiControllerSequence* pkChainSeq = GetSequence(eChainID);
        NIASSERT(pkChainSeq);

        NiKFMTool::Transition* pkTransition = m_spKFMTool->GetTransition(
            eSrcID, eChainID);
        NIASSERT(pkTransition &&
            pkTransition->GetType() != NiKFMTool::TYPE_CHAIN);
        NIVERIFY(FindTimeForAnimationToCompleteTransition(eSrcID, eChainID,
            pkTransition, fStartFrame, fTransitionFrame, &kInfo));
        fTextKeyTime += kInfo.m_fTimeToCompleteTransition;

        eSrcID = eChainID;
        pkSrcSequence = pkChainSeq;
        fStartFrame = kInfo.m_fFrameInDestWhenTransitionCompletes;

        if (fDuration == NiKFMTool::MAX_DURATION)
        {
            fDuration = (pkChainSeq->GetEndKeyTime() - 
                pkChainSeq->GetBeginKeyTime()) - fStartFrame;
        }
        else
        {
            // scale duration so that it is in frame time
            fDuration = pkChainSeq->TimeMultFreq(fDuration);
        }

        fTransitionFrame = fStartFrame + fDuration;
    }

    return INVALID_TIME;
}
//---------------------------------------------------------------------------
NiActorManager::ChainCompletionInfo* NiActorManager::FillChainComplete( 
    unsigned int uiSrcID, unsigned int uiDesID,
    float fFrameDuration, float& fSrcBeginFrame, float& fTotalTime)
{
    NiControllerSequence* pkSrcSeq = GetSequence(uiSrcID);
    NiControllerSequence* pkDesSeq = GetSequence(uiDesID);

    // Get the Transition... call recurively
    NiKFMTool::Transition* pkCurTransition = 
        m_spKFMTool->GetTransition(uiSrcID, uiDesID);
    NIASSERT(pkCurTransition);
    
    float fNewTransitionFrame;
    if (fFrameDuration == NiKFMTool::MAX_DURATION)
    {
        fNewTransitionFrame = 
            pkSrcSeq->GetEndKeyTime();

        if (fNewTransitionFrame < fSrcBeginFrame)
            fNewTransitionFrame = fSrcBeginFrame;
    }
    else
    {
        fNewTransitionFrame = 
            fSrcBeginFrame + fFrameDuration;
    }

    CompletionInfo kInfo;
    NIVERIFY(FindTimeForAnimationToCompleteTransition(uiSrcID, uiDesID,
        pkCurTransition, fSrcBeginFrame, fNewTransitionFrame, &kInfo));

    ChainCompletionInfo* pkCCI = NiNew ChainCompletionInfo;
    pkCCI->SetName(pkSrcSeq->GetName());
    pkCCI->m_fSeqStart = fTotalTime;
    pkCCI->m_fInSeqBeginFrame = fSrcBeginFrame;

    fTotalTime += kInfo.m_fTimeToCompleteTransition;

    pkCCI->m_fTransStart = fTotalTime - pkCurTransition
        ->GetDuration();
    pkCCI->m_fTransEnd = fTotalTime;
    pkCCI->m_pkTransition = pkCurTransition;
    pkCCI->m_uiSrcID = uiSrcID;
    pkCCI->m_uiDesID = uiDesID;
    pkCCI->SetNextName(pkDesSeq->GetName());

    fSrcBeginFrame = kInfo.m_fFrameInDestWhenTransitionCompletes;

    return pkCCI;
}
//---------------------------------------------------------------------------
NiKFMTool::Transition::BlendPair* NiActorManager::GetNextBlendPair(
    unsigned int uiTransSrcID, unsigned int uiTransDesID,
    NiKFMTool::Transition* pkTransition, float fFrameTime,
    float& fActualUnboundedFrame)
{
    // This function finds the lowest blend pair after fFrameTime.

    if (pkTransition->GetType() != NiKFMTool::TYPE_BLEND ||
        pkTransition->GetBlendPairs().GetSize() == 0 ||
        !pkTransition->GetBlendPairs().GetAt(0)->GetStartKey().Exists())
    {
        return NULL;
    }

    NiControllerSequence* pkSrcSequence = GetSequence(uiTransSrcID);

    float fBeginFrame = pkSrcSequence->GetBeginKeyTime();
    float fEndFrame = pkSrcSequence->GetEndKeyTime();
    float fSrcFrameLength = fEndFrame - fBeginFrame;

    NIASSERT(fFrameTime >= fBeginFrame);
    
    fActualUnboundedFrame = fFrameTime;
    if (fFrameTime > fEndFrame)
    {
        if (pkSrcSequence->GetCycleType() == NiTimeController::CLAMP)
        {
            return NULL;
        }
        else
        {
            // this can happen, for example with durations of chains...
            float fFloor = floorf(fFrameTime / fSrcFrameLength);
            float fTimeForLooping = (fFloor * fSrcFrameLength);
            fFrameTime = fFrameTime - fTimeForLooping;
        }
    }

    NiKFMTool::Transition::BlendPair* pkSmallestBP = NULL;
    float fSmallestBPFrameTime = NI_INFINITY;

    for (unsigned int ui = 0; ui < pkTransition->GetBlendPairs().GetSize();
        ui++)
    {
        NiKFMTool::Transition::BlendPair* pkBP = pkTransition
            ->GetBlendPairs().GetAt(ui);
        float fBPFrameTime = pkSrcSequence->GetKeyTimeAt(pkBP->GetStartKey())
            - fFrameTime;
        if (fBPFrameTime < 0.0f)
        {
            fBPFrameTime += fSrcFrameLength;
        }

        if (fBPFrameTime < fSmallestBPFrameTime)
        {
            fSmallestBPFrameTime = fBPFrameTime;
            pkSmallestBP = pkBP;
        }
    }
    
    fActualUnboundedFrame += fSmallestBPFrameTime;

    NIASSERT(pkSmallestBP != NULL);
    return pkSmallestBP;
}
//---------------------------------------------------------------------------
void NiActorManager::RaiseAnimActivatedEvents(SequenceID eEventSeqID)
{
    // This function is only called when no animation is active and an
    // an animation is activated. It catches a case that is not handled by
    // the standard event handling code.

    for (unsigned int ui = 0; ui < m_kCallbacks.GetSize(); ui++)
    {
        CallbackData* pkCallbackData = m_kCallbacks.GetAt(ui);
        if (pkCallbackData)
        {
            if (pkCallbackData->m_eEventType == ANIM_ACTIVATED)
            {
                SequenceID eSequenceID = pkCallbackData->m_eSequenceID;
                if (eSequenceID == ANY_SEQUENCE_ID)
                {
                    NiTMapIterator spos = m_kSequenceMap.GetFirstPos();
                    while (spos)
                    {
                        NiControllerSequence* pkSequence;
                        m_kSequenceMap.GetNext(spos, eSequenceID, pkSequence);

                        if (eSequenceID == eEventSeqID)
                        {
                            m_pkCallbackObject->AnimActivated(this,
                                eSequenceID, m_fTime, m_fTime);
                        }
                    }
                }
                else if (eSequenceID == eEventSeqID)
                {
                    m_pkCallbackObject->AnimActivated(this, eSequenceID,
                        m_fTime, m_fTime);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiActorManager::RaiseAnimDeactivatedEvents(SequenceID eEventSeqID)
{
    // This function is only called when the last remaining animation is
    // deactivated on the manager. It catches a case that is not handled by
    // the standard event handling code.

    for (unsigned int ui = 0; ui < m_kCallbacks.GetSize(); ui++)
    {
        CallbackData* pkCallbackData = m_kCallbacks.GetAt(ui);
        if (pkCallbackData)
        {
            if (pkCallbackData->m_eEventType == ANIM_DEACTIVATED)
            {
                SequenceID eSequenceID = pkCallbackData->m_eSequenceID;
                if (eSequenceID == ANY_SEQUENCE_ID)
                {
                    NiTMapIterator spos = m_kSequenceMap.GetFirstPos();
                    while (spos)
                    {
                        NiControllerSequence* pkSequence;
                        m_kSequenceMap.GetNext(spos, eSequenceID, 
                            pkSequence);

                        if (eSequenceID == eEventSeqID)
                        {
                            m_pkCallbackObject->AnimDeactivated(this,
                                eSequenceID, m_fTime, m_fTime);
                        }
                    }
                }
                else if (eSequenceID == eEventSeqID)
                {
                    m_pkCallbackObject->AnimDeactivated(this, eSequenceID,
                        m_fTime, m_fTime);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiActorManager::RemoveInactiveExtraSequences()
{
    unsigned int ui = 0;
    while (ui < m_kExtraSequences.GetSize())
    {
        if (GetSequence(m_kExtraSequences.GetAt(ui))->GetState() ==
            NiControllerSequence::INACTIVE)
        {
            m_kExtraSequences.RemoveAt(ui);
            ui = 0;
        }
        else
        {
            ui++;
        }
    }
}
//---------------------------------------------------------------------------
NiActorManager::CallbackObject::~CallbackObject()
{
    // virtual destructor defined in case derived classes need virtual
    // destruction
}
//---------------------------------------------------------------------------
