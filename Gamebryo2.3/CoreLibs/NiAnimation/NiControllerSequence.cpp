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

#include "NiControllerSequence.h"
#include <NiCloningProcess.h>
#include "NiControllerManager.h"
#include <NiAVObjectPalette.h>
#include "NiMultiTargetTransformController.h"
#include "NiTransformController.h"
#include "NiTextKeyMatch.h"

NiImplementRTTI(NiControllerSequence, NiObject);

const float NiControllerSequence::INVALID_TIME = -FLT_MAX;
const unsigned int NiControllerSequence::INVALID_INDEX = (unsigned int) -1;

//---------------------------------------------------------------------------
NiControllerSequence::NiControllerSequence(const NiFixedString& kName,
    unsigned int uiArraySize, unsigned int uiArrayGrowBy) : 
    m_uiArraySize(uiArraySize), m_uiArrayGrowBy(uiArrayGrowBy),
    m_pkInterpArray(NULL), m_pkIDTagArray(NULL), m_fSeqWeight(1.0f),
    m_eCycleType(NiTimeController::LOOP), m_fFrequency(1.0f),
    m_fBeginKeyTime(NI_INFINITY), m_fEndKeyTime(-NI_INFINITY),
    m_fLastTime(-NI_INFINITY), m_fWeightedLastTime(-NI_INFINITY),
    m_fLastScaledTime(-NI_INFINITY), m_pkOwner(NULL), m_eState(INACTIVE),
    m_fOffset(-NI_INFINITY), m_fStartTime(-NI_INFINITY),
    m_fEndTime(-NI_INFINITY), m_fDestFrame(-NI_INFINITY),
    m_pkPartnerSequence(NULL), m_spDeprecatedStringPalette(NULL),
    m_pkAccumRoot(NULL), m_uFlags(0)
{
    SetName(kName);

    if(m_uiArraySize > 0)
    {
        // Allocate memory for arrays.
        m_pkInterpArray = NiNew InterpArrayItem[m_uiArraySize];
        m_pkIDTagArray = NiNew IDTag[m_uiArraySize];
    }
}
//---------------------------------------------------------------------------
NiControllerSequence::NiControllerSequence() : 
    m_uiArraySize(0), m_uiArrayGrowBy(0), m_pkInterpArray(NULL),
    m_pkIDTagArray(NULL), m_fSeqWeight(1.0f),
    m_eCycleType(NiTimeController::LOOP), m_fFrequency(1.0f), 
    m_fBeginKeyTime(NI_INFINITY), m_fEndKeyTime(-NI_INFINITY),
    m_fLastTime(-NI_INFINITY), m_fWeightedLastTime(-NI_INFINITY),
    m_fLastScaledTime(-NI_INFINITY), m_pkOwner(NULL), m_eState(INACTIVE),
    m_fOffset(-NI_INFINITY), m_fStartTime(-NI_INFINITY),
    m_fEndTime(-NI_INFINITY), m_fDestFrame(-NI_INFINITY),
    m_pkPartnerSequence(NULL), m_pkAccumRoot(NULL), m_uFlags(0)
{
}
//---------------------------------------------------------------------------
NiControllerSequence::~NiControllerSequence()
{
    NiDelete[] m_pkInterpArray;
    NiDelete[] m_pkIDTagArray;
}
//---------------------------------------------------------------------------
NiControllerSequencePtr NiControllerSequence::CreateSequenceFromFile(
    const char* pcFilename, unsigned int uiIndex)
{
    NiStream kStream;
    if (!kStream.Load(pcFilename))
    {
        return NULL;
    }

    return CreateSequenceFromFile(kStream, uiIndex);
}
//---------------------------------------------------------------------------
NiControllerSequencePtr NiControllerSequence::CreateSequenceFromFile(
    const NiStream& kStream, unsigned int uiIndex)
{
    if (uiIndex >= kStream.GetObjectCount())
    {
        return NULL;
    }

    return NiDynamicCast(NiControllerSequence, kStream.GetObjectAt(uiIndex));
}
//---------------------------------------------------------------------------
bool NiControllerSequence::CreateAllSequencesFromFile(const char* pcFilename,
    NiControllerSequencePointerArray& kSequences)
{
    NiStream kStream;
    if (!kStream.Load(pcFilename))
    {
        return false;
    }

    return CreateAllSequencesFromFile(kStream, kSequences);
}
//---------------------------------------------------------------------------
bool NiControllerSequence::CreateAllSequencesFromFile(const NiStream& kStream,
    NiControllerSequencePointerArray& kSequences)
{
    kSequences.RemoveAll();

    for (unsigned int ui = 0; ui < kStream.GetObjectCount(); ui++)
    {
        NiControllerSequencePtr spSequence = CreateSequenceFromFile(kStream,
            ui);
        if (spSequence)
        {
            kSequences.Add(spSequence);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiControllerSequence::AddInterpolator(
    NiInterpolator* pkInterpolator, const NiFixedString& kAVObjectName,
    const NiFixedString& kPropertyType, const NiFixedString& kCtlrType,
    const NiFixedString& kCtlrID, const NiFixedString& kInterpolatorID)
{
    IDTag kIDTag(kAVObjectName, kPropertyType,
        kCtlrType, kCtlrID, kInterpolatorID);
    return AddInterpolator(pkInterpolator, kIDTag);
}
//---------------------------------------------------------------------------
NiInterpolatorPtr NiControllerSequence::RemoveInterpolator(
    unsigned int uiIndex)
{
    NIASSERT(uiIndex < m_uiArraySize);

    NiInterpolatorPtr spInterpolator = m_pkInterpArray[uiIndex]
        .m_spInterpolator;
    m_pkInterpArray[uiIndex].ClearValues();
    m_pkIDTagArray[uiIndex].ClearValues();

    return spInterpolator;
}
//---------------------------------------------------------------------------
void NiControllerSequence::ReplaceInterpolatorAt(NiInterpolator* pkInterp, 
    unsigned int uiIndex)
{
    NIASSERT(uiIndex < m_uiArraySize);
    m_pkInterpArray[uiIndex].m_spInterpolator = pkInterp;
}
//---------------------------------------------------------------------------
bool NiControllerSequence::AddNameControllerPair(
    const char* pcObjectName, NiTimeController* pkController)
{
    // This function is deprecated. The new AddInterpolator function should
    // be used instead. This function will still work, although it will
    // convert old-style name/controller pairs into interpolators and ID tags.

    // NOTE: This function will change the cycle type, frequency, and
    // play backwards properties of the controller passed in to match the
    // sequence.

    if (!pcObjectName || !pkController ||
        !NiIsKindOf(NiInterpController, pkController))
    {
        return false;
    }

    // Ensure that controller properties match sequence properties.
    pkController->SetCycleType(m_eCycleType);
    pkController->SetFrequency(m_fFrequency);
    pkController->SetPlayBackwards(false);

    NiInterpController* pkInterpCtlr = (NiInterpController*) pkController;

    bool bSuccess = ConvertOldNameControllerPair(pcObjectName, pkInterpCtlr);

    return bSuccess;
}
//---------------------------------------------------------------------------
void NiControllerSequence::RemoveNameControllerPair(unsigned int uiIndex)
{
    // This function is deprecated. The new RemoveInterpolator function
    // should be used instead.

    RemoveInterpolator(uiIndex);
}
//---------------------------------------------------------------------------
void NiControllerSequence::GetObjectNameAt(unsigned int uiIndex,
    char*& pcObjectName) const
{
    NIASSERT(uiIndex < m_uiArraySize);

    // This function has been changed to return a newly-allocated character
    // array. The calling object is responsible for deleting this string.

    if (m_pkInterpArray[uiIndex].m_spInterpolator == NULL)
    {
        pcObjectName = NULL;
        return;
    }

    IDTag& kIDTag = m_pkIDTagArray[uiIndex];
    unsigned int uiStrLen = strlen(kIDTag.GetAVObjectName()) + 1;
    if (kIDTag.GetPropertyType())
    {
        const char* pcPropString = "PROP\n";
        uiStrLen += strlen(pcPropString) + strlen(kIDTag.GetPropertyType()) +
            1;
        pcObjectName = NiAlloc(char, uiStrLen);
        NiSprintf(pcObjectName, uiStrLen, "%s\n%s%s", 
            (const char*) (kIDTag.GetAVObjectName()), 
            pcPropString,
            (const char*) (kIDTag.GetPropertyType()));
    }
    else
    {
        pcObjectName = NiAlloc(char, uiStrLen);
        NiStrcpy(pcObjectName, uiStrLen, kIDTag.GetAVObjectName());
    }
}
//---------------------------------------------------------------------------
float NiControllerSequence::GetTimeAt(const NiFixedString& kTextKey,
    float fCurrentTime)
{
    NiTextKeyMatch kMatchObject( kTextKey );
    return GetTimeAt( &kMatchObject, fCurrentTime );
}
//---------------------------------------------------------------------------
float NiControllerSequence::GetTimeAt( NiTextKeyMatch * pkMatchObject, 
    float fCurrentTime )
{
    // Need to find first occurence of pcTextKey _after_ fCurrentTime.

    float fMatchFrame = m_fBeginKeyTime;
    float fCurrentFrame = ComputeScaledTime(fCurrentTime + m_fOffset, false);

    // Only LOOP and CLAMP cycle types are supported by sequences.
    NIASSERT(m_eCycleType == NiTimeController::CLAMP ||
        m_eCycleType == NiTimeController::LOOP);

    bool bFirstMatch = (m_eCycleType == NiTimeController::LOOP);
    bool bExactMatch = false;
    bool bFoundMatch = false;

    if (m_spTextKeys)
    {
        unsigned int uiNumKeys;
        NiTextKey* pkKeys = m_spTextKeys->GetKeys(uiNumKeys);
        for (unsigned int i = 0; i < uiNumKeys; i++)
        {
            if ( pkMatchObject->IsKeyMatch( pkKeys[i].GetText() ))
            {
                bFoundMatch = true;

                float fTime = pkKeys[i].GetTime();
                if (fTime >= fCurrentFrame)
                {
                    fMatchFrame = fTime;
                    bExactMatch = true;
                    break;
                }
                if (bFirstMatch)
                {
                    fMatchFrame = fTime;
                    bFirstMatch = false;
                }
            }
        }
    }

    if (!bFoundMatch)
    {
        return INVALID_TIME;
    }

    if (!bExactMatch && m_eCycleType == NiTimeController::LOOP)
    {
        fMatchFrame += m_fEndKeyTime - m_fBeginKeyTime;
    }

    float fUnscaledTime = (fMatchFrame - fCurrentFrame) /
        m_fFrequency + fCurrentTime;

    return fUnscaledTime;
}
//---------------------------------------------------------------------------
float NiControllerSequence::GetKeyTimeAt(const NiFixedString& kTextKey) const
{
    if ( !kTextKey.Exists() )
    {
        return INVALID_TIME;
    }
    else
    {
        NiTextKeyMatch kMatchObject(kTextKey);
        return GetKeyTimeAt(&kMatchObject);
    }
}
//---------------------------------------------------------------------------
float NiControllerSequence::GetKeyTimeAt(NiTextKeyMatch* pkMatchObject) const
{
    if (!pkMatchObject || !m_spTextKeys)
    {
        return INVALID_TIME;
    }

    unsigned int uiNumKeys;
    NiTextKey* pkKeys = m_spTextKeys->GetKeys(uiNumKeys);
    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        if ( pkMatchObject->IsKeyMatch(pkKeys[ui].GetText()))
        {
            return pkKeys[ui].GetTime();
        }
    }

    return INVALID_TIME;
}
//---------------------------------------------------------------------------
float NiControllerSequence::GetKeyTimeAtDivFreq(
    const NiFixedString& kTextKey) const
{
    float fTime = GetKeyTimeAt(kTextKey);

    if (fTime == INVALID_TIME)
        return INVALID_TIME;

    return TimeDivFreq(fTime);
}
//---------------------------------------------------------------------------
unsigned int NiControllerSequence::AddInterpolator(
    NiInterpolator* pkInterpolator, const IDTag& kIDTag)
{
    NIASSERT(pkInterpolator);

    unsigned int uiIndex;
    for (uiIndex = 0; uiIndex < m_uiArraySize; uiIndex++)
    {
        if (m_pkInterpArray[uiIndex].m_spInterpolator == NULL)
        {
            break;
        }
    }
    if (uiIndex == m_uiArraySize && !ReallocateArrays())
    {
        return INVALID_INDEX;
    }

    m_pkInterpArray[uiIndex].m_spInterpolator = pkInterpolator;
    m_pkIDTagArray[uiIndex] = kIDTag;
    
    return uiIndex;
}
//---------------------------------------------------------------------------
void NiControllerSequence::SortByBlendInterpolator()
{
    for (unsigned int uiIndex = 0; uiIndex < m_uiArraySize; uiIndex++)
    {
        NiBlendInterpolator* pkCurInterp = m_pkInterpArray[uiIndex]
            .m_pkBlendInterp;
        InterpArrayItem kTempItem = m_pkInterpArray[uiIndex];
        IDTag kTempIDTag = m_pkIDTagArray[uiIndex];

        for (unsigned int uiSort = 0; uiSort < uiIndex; uiSort++)
        {
            if (m_pkInterpArray[uiSort].m_pkBlendInterp > pkCurInterp)
            {
                m_pkInterpArray[uiIndex] = m_pkInterpArray[uiSort];
                m_pkIDTagArray[uiIndex] = m_pkIDTagArray[uiSort];
                m_pkInterpArray[uiSort] = kTempItem;
                m_pkIDTagArray[uiSort] = kTempIDTag;
                pkCurInterp = m_pkInterpArray[uiIndex].m_pkBlendInterp;
                kTempItem = m_pkInterpArray[uiIndex];
                kTempIDTag = m_pkIDTagArray[uiIndex];
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiControllerSequence::Update(float fTime, bool bUpdateInterpolators)
{
    if (m_eState == INACTIVE)
    {
        return;
    }

    if (m_fOffset == -NI_INFINITY)
    {
        m_fOffset = -fTime;
    }

    if (m_fStartTime == -NI_INFINITY)
    {
        m_fStartTime = fTime;
        m_fEndTime = fTime + m_fEndTime;
    }

    float fEaseSpinner = 1.0f;
    float fTransSpinner = 1.0f;
    switch (m_eState)
    {
        case EASEIN:
            if (fTime < m_fEndTime)
            {
                NIASSERT(fTime >= m_fStartTime && m_fEndTime != m_fStartTime);
                fEaseSpinner = (fTime - m_fStartTime) / (m_fEndTime -
                    m_fStartTime);
            }
            else
            {
                m_eState = ANIMATING;
            }
            break;
        case TRANSDEST:
            if (fTime < m_fEndTime)
            {
                NIASSERT(fTime >= m_fStartTime && m_fEndTime != m_fStartTime);
                fTransSpinner = (fTime - m_fStartTime) / (m_fEndTime -
                    m_fStartTime);
            }
            else
            {
                if (m_fDestFrame != -NI_INFINITY)
                {
                    // This case is hit when we were blending in this
                    // sequence. In this case, we need to reset the sequence
                    // offset and clear the destination frame.
                    m_fOffset = -fTime + m_fDestFrame;
                    m_fDestFrame = -NI_INFINITY;
                }
                m_eState = ANIMATING;
            }
            break;
        case EASEOUT:
            if (fTime < m_fEndTime)
            {
                NIASSERT(fTime >= m_fStartTime && m_fEndTime != m_fStartTime);
                fEaseSpinner = (m_fEndTime - fTime) / (m_fEndTime -
                    m_fStartTime);
            }
            else
            {
                Deactivate(0.0f, false);
                return;
            }
            break;
        case MORPHSOURCE:
        {
            NIASSERT(m_pkPartnerSequence);

            // Compute initial offset for partner sequence, undoing phase
            // and frequency adjustments. This assumes the phase and
            // frequency will not change between now and the end time of
            // the morph.
            float fStartFrame = m_pkPartnerSequence
                ->FindCorrespondingMorphFrame(this, m_fOffset + fTime);
            fStartFrame /= m_pkPartnerSequence->m_fFrequency;
            m_pkPartnerSequence->m_fOffset = fStartFrame - fTime;

            // Change sequence state appropriately.
            m_eState = TRANSSOURCE;

            // This case statement intentionally does not break. The code
            // for the TRANSSOURCE case should be subsequently run.
        }
        case TRANSSOURCE:
            if (fTime < m_fEndTime)
            {
                NIASSERT(fTime >= m_fStartTime && m_fEndTime != m_fStartTime);
                fTransSpinner = (m_fEndTime - fTime) / (m_fEndTime -
                    m_fStartTime);
            }
            else
            {
                Deactivate(0.0f, true);
                return;
            }
            break;
        default:
            // Is there something better we can do here?
            // possible cases are INACTIVE and ANIMATING
            break;
    }

    if (bUpdateInterpolators)
    {
        float fUpdateTime;
        if (m_fDestFrame != -NI_INFINITY)
        {
            fUpdateTime = m_fDestFrame;
        }
        else if (m_pkPartnerSequence)
        {
            if (m_pkPartnerSequence->GetLastTime() !=
                m_pkPartnerSequence->m_fOffset + fTime)
            {
                m_pkPartnerSequence->Update(fTime, false);
            }

            fUpdateTime = FindCorrespondingMorphFrame(m_pkPartnerSequence,
                m_pkPartnerSequence->m_fOffset + fTime);
            fUpdateTime /= m_fFrequency;
        }
        else
        {
            fUpdateTime = m_fOffset + fTime;
        }

        SetInterpsWeightAndTime(m_fSeqWeight * fTransSpinner, fEaseSpinner,
            ComputeScaledTime(fUpdateTime));
    }
}
//---------------------------------------------------------------------------
bool NiControllerSequence::StoreTargets(NiAVObject* pkRoot)
{
    // This function resolves and stores the blend interpolator pointer using
    // the IDTag for each interpolator.

    NIASSERT(pkRoot);

    NiAVObjectPalette* pkObjectPalette = m_pkOwner->GetObjectPalette();
    NIASSERT(pkObjectPalette);

    if (m_kAccumRootName.Exists())
    {
        m_pkAccumRoot = pkObjectPalette->GetAVObject(m_kAccumRootName);
    }

    bool bAddedControllers = ResolveTransformInterpolators(pkRoot,
        pkObjectPalette);

    for (unsigned int ui = 0; ui < m_uiArraySize; ui++)
    {
        NiInterpolator* pkInterpolator = m_pkInterpArray[ui].m_spInterpolator;
        if (!pkInterpolator)
        {
            continue;
        }
        
        const IDTag& kIDTag = m_pkIDTagArray[ui];

        // Resolve dependencies in the interpolator.
        if (!pkInterpolator->ResolveDependencies(pkObjectPalette))
        {
            NILOG("NiControllerSequence::StoreTargets "
                "failed to resolve dependencies for the interpolator with the"
                "following identifiers:\n\tm_pcAVObjectName\t%s\n\t"
                "m_pcPropertyType\t%s\nm_pcCtlrType\t\t%s\nm_pcCtlrID\t\t\t%s"
                "\nm_pcInterpolatorID\t%s\n", 
                (const char*) (kIDTag.GetAVObjectName()),
                (const char*) (kIDTag.GetPropertyType()), 
                (const char*) (kIDTag.GetCtlrType()),
                (const char*) (kIDTag.GetCtlrID()), 
                (const char*) (kIDTag.GetInterpolatorID()));
            continue;
        }

         // Continue if we've already resolved this blend interpolator.
        if (m_pkInterpArray[ui].m_pkBlendInterp != NULL)
        {
            continue;
        }

        // Retrieve controller target.
        NiAVObject* pkAVObj = pkObjectPalette->GetAVObject(
            kIDTag.GetAVObjectName());
        NiObjectNET* pkTarget = pkAVObj;
        if (pkTarget && kIDTag.GetPropertyType())
        {
            NiTListIterator kIter = pkAVObj->GetPropertyList().GetHeadPos();
            while (kIter)
            {
                NiProperty* pkProp = pkAVObj->GetPropertyList().GetNext(
                    kIter);
                if (strcmp(pkProp->GetRTTI()->GetName(),
                    kIDTag.GetPropertyType()) == 0)
                {
                    pkTarget = pkProp;
                    break;
                }
            }
        }
        if (!pkTarget)
        {
            NILOG("NiControllerSequence::StoreTargets "
                "failed to find target with the following identifiers:\n\t"
                "m_pcAVObjectName\t%s\n\tm_pcPropertyType\t%s\n",
                (const char*) (kIDTag.GetAVObjectName()),
                (const char*) (kIDTag.GetPropertyType()));
            continue;
        }

        // Retrieve controller.
        NiInterpController* pkInterpCtlr = NULL;
        NiTimeController* pkCtlr = pkTarget->GetControllers();
        while (pkCtlr)
        {
            if (strcmp(pkCtlr->GetRTTI()->GetName(), kIDTag.GetCtlrType())
                == 0)
            {
                NIASSERT(NiIsKindOf(NiInterpController, pkCtlr));
                const char* pcCtlrID = ((NiInterpController*) pkCtlr)
                    ->GetCtlrID();
                if ((!pcCtlrID && !kIDTag.GetCtlrID()) ||
                    (pcCtlrID && kIDTag.GetCtlrID() &&
                        strcmp(pcCtlrID, kIDTag.GetCtlrID()) == 0))
                {
                    pkInterpCtlr = (NiInterpController*) pkCtlr;
                    break;
                }
            }
            pkCtlr = pkCtlr->GetNext();
        }
        if (!pkInterpCtlr)
        {
            if (m_pkInterpArray[ui].m_spInterpCtlr)
            {
                pkInterpCtlr = m_pkInterpArray[ui].m_spInterpCtlr;
                pkInterpCtlr->SetTarget(pkTarget);
                bAddedControllers = true;

                if (pkTarget == m_pkOwner->GetTarget())
                {
                    // Controller needs to be _after_ current
                    // NiControllerManager so that the manager will be
                    // deleted and update properly.
                    pkTarget->RemoveController(pkInterpCtlr);
                    pkInterpCtlr->SetNext(m_pkOwner->GetNext());
                    m_pkOwner->SetNext(pkInterpCtlr);
                }
            }
            else
            {
                NILOG("NiControllerSequence::"
                    "StoreTargets failed to find target with the following "
                    "identifiers:\n\tm_pcAVObjectName\t%s\n\tm_pcPropertyType"
                    "\t%s\n\tm_pcCtlrType\t\t%s\n\tm_pcCtlrID\t\t\t%s\n",
                    (const char*) (kIDTag.GetAVObjectName()),
                    (const char*) (kIDTag.GetPropertyType()),
                    (const char*) (kIDTag.GetCtlrType()), 
                    (const char*) (kIDTag.GetCtlrID()));
                continue;
            }
        }

        // Retrieve blend interpolator.
        NiBlendInterpolator* pkBlendInterp = NULL;
        unsigned short usInterpIdx = 
            pkInterpCtlr->GetInterpolatorIndex(kIDTag.GetInterpolatorID());

        if (usInterpIdx != NiInterpController::INVALID_INDEX)
        {
            NiBlendInterpolator* pkTempBlendInterp = NiDynamicCast(
                NiBlendInterpolator, pkInterpCtlr->GetInterpolator(
                usInterpIdx));

            bool bAccumulateAnimations = false;

            if (m_pkAccumRoot == pkTarget && 
                m_pkOwner->GetCumulativeAnimations())
            {
                bAccumulateAnimations = true;
            }

            if (!pkTempBlendInterp)
            {
                pkTempBlendInterp = pkInterpCtlr->CreateBlendInterpolator(
                    usInterpIdx, true, bAccumulateAnimations);
                pkInterpCtlr->SetInterpolator(pkTempBlendInterp,
                    usInterpIdx);
            }
            pkBlendInterp = pkTempBlendInterp;
        }

        if (!pkBlendInterp)
        {
            NILOG("NiControllerSequence::StoreTargets "
                "failed to find target with the following identifiers:\n\t"
                "m_pcAVObjectName\t%s\n\tm_pcPropertyType\t%s\n\tm_pcCtlrType"
                "\t\t%s\n\tm_pcCtlrID\t\t\t%s\n\tm_pcInterpolatorID\t%s\n",
                (const char*) (kIDTag.GetAVObjectName()),
                (const char*) (kIDTag.GetPropertyType()),
                (const char*) (kIDTag.GetCtlrType()),
                (const char*) (kIDTag.GetCtlrID()),
                (const char*) (kIDTag.GetInterpolatorID()));
            continue;
        }

        pkInterpCtlr->SetManagerControlled(true);
        m_pkInterpArray[ui].m_spInterpCtlr = pkInterpCtlr;
        m_pkInterpArray[ui].m_pkBlendInterp = pkBlendInterp;
    }

    if (bAddedControllers)
    {
        NIASSERT(NiIsKindOf(NiAVObject, m_pkOwner->GetTarget()));
        bool bSelectiveUpdate, bRigid;
        ((NiAVObject*) m_pkOwner->GetTarget())->SetSelectiveUpdateFlags(
            bSelectiveUpdate, true, bRigid);
    }

    SortByBlendInterpolator();
    return true;
}
//---------------------------------------------------------------------------
bool NiControllerSequence::ResolveTransformInterpolators(NiAVObject* pkRoot,
    NiAVObjectPalette* pkObjectPalette)
{
    bool bAddedController = false;

    NiMultiTargetTransformControllerPtr spCtlr = 
        NiGetController(NiMultiTargetTransformController, pkRoot);
    if (spCtlr == NULL)
    {
        unsigned short usTransformInterpCount = 0;

        for (unsigned int ui = 0; ui < m_uiArraySize; ui++)
        {
            IDTag* pkID = &m_pkIDTagArray[ui];   

            if (pkID->GetCtlrType().Exists() && 
                NiStricmp(pkID->GetCtlrType(), "NiTransformController") == 0)
            {
                usTransformInterpCount++;
            }
        }

        if (usTransformInterpCount == 0)
        {
            return bAddedController;
        }

        spCtlr = NiNew NiMultiTargetTransformController(
            usTransformInterpCount);
        spCtlr->SetTarget(pkRoot);
        spCtlr->SetManagerControlled(true);
        bAddedController = true;

        if (pkRoot == m_pkOwner->GetTarget())
        {
            // Controller needs to be _after_ current
            // NiControllerManager so that the manager will be
            // deleted and update properly.
            pkRoot->RemoveController(spCtlr);
            spCtlr->SetNext(m_pkOwner->GetNext());
            m_pkOwner->SetNext(spCtlr);
        }
    }

    for (unsigned int ui = 0; ui < m_uiArraySize; ui++)
    {
        InterpArrayItem* pkItem = &m_pkInterpArray[ui];
        IDTag* pkID = &m_pkIDTagArray[ui];   

        if (pkID->GetCtlrType().Exists() && 
            strcmp(pkID->GetCtlrType(), "NiTransformController") == 0)
        {
            NiAVObject* pkAVObj = pkObjectPalette->GetAVObject(
                pkID->GetAVObjectName());

            if (pkAVObj == NULL || (m_pkAccumRoot == pkAVObj && 
                m_pkOwner->GetCumulativeAnimations()))
            {
                continue;
            }

            NiTransformController* pkTransCtlr = NiGetController(
                NiTransformController, pkAVObj);
            
            unsigned short usIndex = spCtlr->GetInterpolatorTargetIndex(
                pkAVObj);

            if (usIndex == NiMultiTargetTransformController::INVALID_INDEX)
            {
                usIndex = spCtlr->AddInterpolatorTarget(pkAVObj);

                // There may be no more room for another interpolator,
                // in that case, we should just resolve the normal
                // NiTransformController instead of trying to
                // resize the NiMultiTargetTransformController
                if(usIndex == NiMultiTargetTransformController::
                    INVALID_INDEX)
                {
                    continue;
                }
            }
            else
            {
                spCtlr->SetInterpolatorTarget(usIndex, pkAVObj);
            }

            pkAVObj->RemoveController(pkTransCtlr);

            pkItem->m_spInterpCtlr = spCtlr;
            pkItem->m_pkBlendInterp = spCtlr->GetBlendInterpolator(
                usIndex);
            pkItem->m_pkBlendInterp->SetManagerControlled(true);
        }
    }

    return bAddedController;
}
//---------------------------------------------------------------------------
void NiControllerSequence::ClearTargets()
{
    for (unsigned int ui = 0; ui < m_uiArraySize; ui++)
    {
        m_pkInterpArray[ui].m_spInterpCtlr = NULL;
        m_pkInterpArray[ui].m_pkBlendInterp = NULL;
    }
}
//---------------------------------------------------------------------------
bool NiControllerSequence::Activate(char cPriority, bool bStartOver,
    float fWeight, float fEaseInTime, NiControllerSequence* pkTimeSyncSeq,
    bool bTransition)
{
    NIASSERT(m_pkOwner);

    if (m_eState != INACTIVE)
    {
        NiOutputDebugString("Attempting to activate a sequence that is "
            "already animating!\n");
        return false;
    }

    m_pkPartnerSequence = NULL;
    if (pkTimeSyncSeq)
    {
        if (!VerifyDependencies(pkTimeSyncSeq) ||
            !VerifyMatchingMorphKeys(pkTimeSyncSeq))
        {
            return false;
        }
        m_pkPartnerSequence = pkTimeSyncSeq;
    }

    // Attach the interpolators to their blend interpolators.
    AttachInterpolators(cPriority);

    // Set parameters.
    m_fSeqWeight = fWeight;

    if (fEaseInTime > 0.0f)
    {
        if (bTransition)
        {
            m_eState = TRANSDEST;
        }
        else
        {
            m_eState = EASEIN;
        }
        m_fStartTime = -NI_INFINITY;
        m_fEndTime = fEaseInTime;
    }
    else
    {
        m_eState = ANIMATING;
    }

    if (bStartOver)
    {
        ResetSequence();
    }

    m_fLastTime = -NI_INFINITY;

    unsigned int uiCount = m_kActivationCallbacks.GetSize();
    for (unsigned int ui = 0; ui < uiCount; ui++)
        m_kActivationCallbacks.GetAt(ui)->ActivationChanged(this, m_eState);

    return true;
}
//---------------------------------------------------------------------------
bool NiControllerSequence::Deactivate(float fEaseOutTime, bool bTransition)
{
    if (m_eState == INACTIVE)
    {
        return false;
    }

    if (fEaseOutTime > 0.0f)
    {
        if (bTransition)
        {
            m_eState = TRANSSOURCE;
        }
        else
        {
            m_eState = EASEOUT;
        }
        m_fStartTime = -NI_INFINITY;
        m_fEndTime = fEaseOutTime;
    }
    else
    {
        // Store the new offset.
        if (m_fLastTime != -NI_INFINITY)
        {
            m_fOffset += (m_fWeightedLastTime / m_fFrequency) - m_fLastTime;
        }

        m_eState = INACTIVE;

        m_pkPartnerSequence = NULL;
        m_fDestFrame = -NI_INFINITY;

        DetachInterpolators();
    }

    unsigned int uiCount = m_kActivationCallbacks.GetSize();
    for (unsigned int ui = 0; ui < uiCount; ui++)
        m_kActivationCallbacks.GetAt(ui)->ActivationChanged(this, m_eState);

    return true;
}
//---------------------------------------------------------------------------
bool NiControllerSequence::StartBlend(NiControllerSequence* pkDestSequence,
    float fDuration, const char* pcDestTextKey, int iPriority,
    float fSourceWeight, float fDestWeight,
    NiControllerSequence* pkTimeSyncSeq)
{
    // Determine the destination frame.
    float fDestFrame = 0.0f;
    if (pcDestTextKey)
    {
        // The blend is to some point in the animation - not necessarily 
        // the start. We must first find where in the animation we will
        // be starting.
        float fKeyTime = pkDestSequence->GetKeyTimeAt(pcDestTextKey);
        if (fKeyTime != INVALID_TIME)
        {
            fDestFrame = fKeyTime;
        }
    }

    return StartBlend(pkDestSequence, fDuration, fDestFrame, iPriority,
        fSourceWeight, fDestWeight, pkTimeSyncSeq);
}
//---------------------------------------------------------------------------
bool NiControllerSequence::StartBlend(NiControllerSequence* pkDestSequence,
    float fDuration, float fDestFrame, int iPriority, float fSourceWeight,
    float fDestWeight, NiControllerSequence* pkTimeSyncSeq)
{
    // Deactivate source sequence first.
    Deactivate(0.0f, true);

    if (fDuration <= 0.0f)
    {
        fDuration = 0.0001f;
    }

    // The following "Frame" variables must be divided by frequency
    // because they are eventually passed into ComputeScaledTime.
    // Must set them here because they are the only way to figure out if
    // we are in a blend transition, and the activation callbacks might
    // need that info.
    m_fDestFrame = m_fLastScaledTime;
    m_fDestFrame /= GetFrequency();

    fDestFrame /= pkDestSequence->GetFrequency();
    pkDestSequence->m_fDestFrame = fDestFrame;

    // Activate source and destination sequences.
    if (!Activate(iPriority, false, fSourceWeight, 0.0f, NULL, true) ||
        !pkDestSequence->Activate(iPriority, false, fDestWeight, fDuration,
        pkTimeSyncSeq, true))
    {
        return false;
    }

    // Ease out source sequence.
    Deactivate(fDuration, true);

    return true;
}
//---------------------------------------------------------------------------
bool NiControllerSequence::StartMorph(NiControllerSequence* pkDestSequence,
    float fDuration, int iPriority, float fSourceWeight, float fDestWeight)
{
    // Deactivate source sequence first.
    Deactivate(0.0f, true);

    // Activate source and destination sequences.
    if (!Activate(iPriority, false, fSourceWeight, fDuration, pkDestSequence,
        true) || !pkDestSequence->Activate(iPriority, true, fDestWeight,
        fDuration, NULL, true))
    {
        return false;
    }

    // Set sequence state for source sequence. The source sequence state is
    // set to MORPHSOURCE so that special processing can be done on the first
    // update. After this processing is complete, the state will be changed to
    // TRANSSOURCE.
    m_eState = MORPHSOURCE;

    return true;
}
//---------------------------------------------------------------------------
void NiControllerSequence::PopulatePoseSequence(
    NiControllerSequence* pkPoseSequence, unsigned int uiArraySize,
    NiControllerSequence* pkSequenceToSynchronize)
{
    // Clear pose sequence arrays.
    unsigned int ui;
    for (ui = 0; ui < pkPoseSequence->GetArraySize(); ui++)
    {
        pkPoseSequence->RemoveInterpolator(ui);
    }

    // Copy timing parameters.
    pkPoseSequence->SetCycleType(m_eCycleType);
    pkPoseSequence->SetFrequency(m_fFrequency);
    pkPoseSequence->SetBeginKeyTime(m_fBeginKeyTime);
    pkPoseSequence->SetEndKeyTime(m_fEndKeyTime);

    // Create pose interpolators for temp sequence.
    for (ui = 0; ui < m_uiArraySize; ui++)
    {
        InterpArrayItem& kItem = m_pkInterpArray[ui];
        NiInterpController* pkCtlr = kItem.m_spInterpCtlr;

        if (!kItem.m_spInterpolator || !pkCtlr)
        {
            continue;
        }

        bool bAddPoseInterpolator = false;
        if (pkSequenceToSynchronize)
        {
            for (unsigned int uj = 0;
                uj < pkSequenceToSynchronize->GetArraySize(); uj++)
            {
                if (pkCtlr == pkSequenceToSynchronize->GetControllerAt(uj))
                {
                    bAddPoseInterpolator = true;
                    break;
                }
            }
        }
        else
        {
            bAddPoseInterpolator = true;
        }

        if (bAddPoseInterpolator)
        {
            // Add pose interpolator.
            for (unsigned int uj = 0; uj < pkCtlr->GetInterpolatorCount();
                uj++)
            {
                if (pkCtlr->GetInterpolator(uj) == kItem.m_pkBlendInterp)
                {
                    NiInterpolator* pkInterpolator =
                        pkCtlr->CreatePoseInterpolator(uj);
                    unsigned int uiNewIdx = pkPoseSequence->AddInterpolator(
                        pkInterpolator, IDTag());
                    InterpArrayItem& kPoseItem =
                        pkPoseSequence->m_pkInterpArray[uiNewIdx];
                    kPoseItem.m_pkBlendInterp = kItem.m_pkBlendInterp;
                    kPoseItem.m_spInterpCtlr = kItem.m_spInterpCtlr;
                    break;
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiControllerSequence::AttachInterpolators(char cPriority)
{
    for (unsigned int ui = 0; ui < m_uiArraySize; ui++)
    {
        InterpArrayItem& kItem = m_pkInterpArray[ui];
        if (kItem.m_spInterpolator != NULL)
        {
            // kItem.m_pkBlendInterp may be NULL if the interpolator was
            // not successfully attached.
            if (kItem.m_pkBlendInterp != NULL)
            {
                kItem.m_ucBlendIdx = kItem.m_pkBlendInterp->AddInterpInfo(
                    kItem.m_spInterpolator, 0.0f, cPriority);
                NIASSERT(kItem.m_ucBlendIdx !=
                    NiBlendInterpolator::INVALID_INDEX);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiControllerSequence::DetachInterpolators()
{
    for (unsigned int ui = 0; ui < m_uiArraySize; ui++)
    {
        InterpArrayItem& kItem = m_pkInterpArray[ui];
        if (kItem.m_pkBlendInterp)
        {
            kItem.m_pkBlendInterp->RemoveInterpInfo(kItem.m_ucBlendIdx);
        }
    }
}
//---------------------------------------------------------------------------
float NiControllerSequence::FindCorrespondingMorphFrame(
    NiControllerSequence* pkSourceSequence, float fSourceSequenceTime)
{
    // This function uses all existing "morph:" key pairs between this
    // sequence and pkSourceSequence to map the time in pkSourceSequence,
    // fSourceSequenceTime, to a corresponding time in this sequence.
    // fSourceSequenceTime need not be within the range of pkSourceSequence,
    // since ComputeScaledTime will be called on it. The time returned is
    // within the range of this sequence.

    // Compute the scaled source sequence time.
    float fScaledSourceTime = pkSourceSequence->ComputeScaledTime(
        fSourceSequenceTime, false);

    // Find the "morph:" keys in the (first set of text keys of the) source 
    // sequence surrounding fSourceSequenceTime.
    // Assumptions: 
    // - keys are in chronological order
    // - at least one "morph:" key exists
    // - in any "combined sequence" (>1 set of _text keys_), the _first_ set
    //   determines the morph values
    // - MORPHING SEQUENCES ARE PURELY LOOPED (not reversed or clamped)
    //   If they are not looped, the morph will only work if, during
    //   the transition, neither sequence hits its endpoint
    unsigned int uiNumKeys;
    NiTextKey* pkKeys = pkSourceSequence->GetTextKeys()->GetKeys(uiNumKeys);
    NiTextKey* pkFirstKey = NULL;
    NiTextKey* pkPrevKey = NULL;
    NiTextKey* pkNextKey = NULL;
    NiTextKey* pkLastKey = NULL;
    unsigned int ui;
    for (ui = 0; ui < uiNumKeys; ui++)
    {
        const char* pcKeyText = pkKeys[ui].GetText();
        if (pcKeyText && strncmp(pcKeyText, "morph:", 6) == 0)
        {
            pkLastKey = &(pkKeys[ui]);
            if (!pkFirstKey)
                pkFirstKey = pkLastKey;

            if (pkKeys[ui].GetTime() < fScaledSourceTime)
                pkPrevKey = pkLastKey;
            else if (!pkNextKey)
                pkNextKey = pkLastKey;
        }
    }

    // If this assertion is hit, the base sequence does not have
    // any "morph:" text keys.
    NIASSERT(pkNextKey || pkPrevKey);

    bool bCreateNext = false;

    // Find the frame location relative to the base morph frames
    float fSourceRatio;
    if (!pkNextKey || !pkPrevKey)
    {
        // If either prev or next key is not found, supply one based on 
        //   the CycleType attribute of the sequence
        if (!pkNextKey)
        {
            // Assuming LOOP
            float fStartTime = pkPrevKey->GetTime();
            float fEndTime = pkSourceSequence->m_fEndKeyTime - 
                pkSourceSequence->m_fBeginKeyTime + pkFirstKey->GetTime();
            fSourceRatio = (fScaledSourceTime - fStartTime) / 
                (fEndTime - fStartTime);
            pkNextKey = pkFirstKey;
            bCreateNext = true;
        }
        else // (!pkPrevKey)
        {
            // Assuming LOOP
            float fStartTime = pkSourceSequence->m_fBeginKeyTime - 
                pkSourceSequence->m_fEndKeyTime + pkLastKey->GetTime();
            float fEndTime = pkNextKey->GetTime();
            fSourceRatio = (fScaledSourceTime - fStartTime) / 
                (fEndTime - fStartTime);
            pkPrevKey = pkLastKey;
        }
    }
    else
    {
        float fStartTime = pkPrevKey->GetTime();
        float fEndTime = pkNextKey->GetTime();
        fSourceRatio = (fScaledSourceTime - fStartTime) / 
            (fEndTime - fStartTime);
    }

    // Find the corresponding keys in the Target sequence
    // Assumptions: 
    // - there is exactly one "morph:" key in the Target sequence with the
    //   same name as each key in the base sequence
    // - the corresponding keys are in the same order in both sequences
    // - if the morph extends beyond the length of a sequence (ie loops or 
    //   reverses), both sequences have the same CycleType value
    pkKeys = m_spTextKeys->GetKeys(uiNumKeys);

    bool bPrevFound = (pkPrevKey == NULL); 
    bool bNextFound = (pkNextKey == NULL); 
    NiTextKey* pkTargetPrevKey = NULL;
    NiTextKey* pkTargetNextKey = NULL;
    for (ui = 0; ui < uiNumKeys && !(bPrevFound && bNextFound); ui++)
    {
        if (!bPrevFound && 
            strcmp(pkKeys[ui].GetText(), pkPrevKey->GetText()) == 0)
        {
            pkTargetPrevKey = &(pkKeys[ui]);
            bPrevFound = true;
        }
        if (!bNextFound && 
            strcmp(pkKeys[ui].GetText(), pkNextKey->GetText()) == 0)
        {
            pkTargetNextKey = &(pkKeys[ui]);
            bNextFound = true;
        }
    }

    // If this assertion is hit, the target sequence does not contain a
    // corresponding "morph:" text key.
    NIASSERT(bPrevFound && bNextFound);

    // Find the corresponding frame based on the target morph frames
    float fTargetHi = 0.0f, fTargetLo = 0.0f;

    if (pkTargetPrevKey == pkTargetNextKey)
    {
        fTargetLo = pkTargetPrevKey->GetTime();
        fTargetHi = pkTargetNextKey->GetTime();

        // We are in the situation where only 1 morph tag was found
        if (bCreateNext)
            fTargetHi += m_fEndKeyTime - m_fBeginKeyTime;
        else // bCreatePrev implied
            fTargetLo -= (m_fEndKeyTime - m_fBeginKeyTime);
    }
    else
    {
        if (pkTargetNextKey)
            fTargetHi = pkTargetNextKey->GetTime();
        if (pkTargetPrevKey)
            fTargetLo = pkTargetPrevKey->GetTime();

        // Handle case where tags "cross"
        if (fTargetLo > fTargetHi)
            fTargetHi += m_fEndKeyTime - m_fBeginKeyTime;
    }

    float fTargetFrame = fTargetLo + fSourceRatio * (fTargetHi - fTargetLo);

    // Ensure the target frame is within the range of this sequence.
    if (fTargetFrame < m_fBeginKeyTime)
        fTargetFrame += m_fEndKeyTime - m_fBeginKeyTime;
    else if (fTargetFrame > m_fEndKeyTime)
        fTargetFrame -= m_fEndKeyTime - m_fBeginKeyTime;

    return fTargetFrame;
}
//---------------------------------------------------------------------------
bool NiControllerSequence::ConvertOldNameControllerPair(
    const char* pcObjectName, NiInterpController* pkController)
{
    NIASSERT(pcObjectName && pkController);

    char acObjectName[NI_MAX_PATH], acTargetType[NI_MAX_PATH],
        acTargetName[NI_MAX_PATH];
    NiStrcpy(acObjectName, NI_MAX_PATH, pcObjectName);
    acTargetType[0] = '\0';
    acTargetName[0] = '\0';

    bool bSubTarget = false;
    char* pcPtr = strchr(acObjectName, '\n');
    if (pcPtr)
    {
        NiStrcpy(acTargetType, NI_MAX_PATH, pcPtr + 1);
        *pcPtr = '\0';
        pcPtr = strchr(acTargetType, '\n');
        if (!pcPtr)
        {
            NIASSERT(!"Error: A target type was specified without a target!");
        }
        NiStrcpy(acTargetName, NI_MAX_PATH, pcPtr + 1);
        *pcPtr = '\0';

        bSubTarget = true;
    }

    for (unsigned short us = 0; us < pkController->GetInterpolatorCount();
        us++)
    {
        IDTag kIDTag;
        kIDTag.SetAVObjectName(acObjectName);
        if (strcmp(acTargetType, "PROP") == 0)
        {
            kIDTag.SetPropertyType(acTargetName);
        }
        kIDTag.SetCtlrType(pkController->GetRTTI()->GetName());
        kIDTag.SetCtlrID(pkController->GetCtlrID());
        kIDTag.SetInterpolatorID(pkController->GetInterpolatorID(us));

        NiInterpolator* pkInterpolator = pkController->GetInterpolator(us);
        NIASSERT(pkInterpolator);

        unsigned int uiIndex = AddInterpolator(pkInterpolator, kIDTag);
        if (uiIndex == INVALID_INDEX)
        {
            return false;
        }

        m_pkInterpArray[uiIndex].m_spInterpCtlr = pkController;
    }

    return true;
}
//---------------------------------------------------------------------------
float NiControllerSequence::ComputeScaledTime(float fTime,
    bool bStoreLastTime)
{
    // ComputeScaledTime converts the incoming time value to scaled time
    // using the object's frequency and phase values in combination with the
    // cycle type.  The returned scaled time will most often represent the
    // time units found in the animation keys.

    float fScaledTime = 0.0f;

    float fWeightedLastTime = m_fWeightedLastTime;
    float fDeltaTime;
    if (m_fLastTime == -NI_INFINITY)
    {
        fWeightedLastTime = 0.0f;
        fDeltaTime = fTime;
    }
    else
    {
        // Calculate time since last update
        fDeltaTime = fTime - m_fLastTime;
    }


    // Scale time linearly by frequency value 
    // specified by the application.
    //
    // time' = freq * time
    //
    fWeightedLastTime += fDeltaTime * m_fFrequency;
    fScaledTime = fWeightedLastTime;

    // Switch on m_CycleType
    switch (m_eCycleType)
    {
        case NiTimeController::LOOP:
        {
            // Loop -- only need fractional time
            float fSpan = m_fEndKeyTime - m_fBeginKeyTime;
            if (fSpan != 0.0f)
            {
                if (fScaledTime != m_fEndKeyTime)
                {
                    fScaledTime = 
                        NiFmod(fScaledTime - m_fBeginKeyTime, fSpan)
                        + m_fBeginKeyTime;
                }

                if (fScaledTime < m_fBeginKeyTime)
                {
                    fScaledTime += fSpan;
                }
            }
            else
            {
                fScaledTime = m_fBeginKeyTime;
            }
            break;
        }
        case NiTimeController::CLAMP:
            // Clamp - do nothing, conditions below will handle clamping.
            break;
        default:
            // Only LOOP and CLAMP cycle types are supported by sequences.
            NIASSERT(false);
            break;
    }

    if (fScaledTime > m_fEndKeyTime)
    {
        fScaledTime = m_fEndKeyTime;
    }
    else if (fScaledTime < m_fBeginKeyTime)
    {
        fScaledTime = m_fBeginKeyTime;
    }

    if (bStoreLastTime)
    {
        m_fWeightedLastTime = fWeightedLastTime;
        m_fLastTime = fTime;
        m_fLastScaledTime = fScaledTime;
    }

    return fScaledTime;
}
//---------------------------------------------------------------------------
void NiControllerSequence::SetInterpsWeightAndTime(float fWeight,
    float fEaseSpinner, float fTime)
{
    NIASSERT(m_eState != INACTIVE);

    for (unsigned int ui = 0; ui < m_uiArraySize; ui++)
    {
        InterpArrayItem& kItem = m_pkInterpArray[ui];
        if (kItem.m_pkBlendInterp)
        {
            kItem.m_pkBlendInterp->SetWeight(fWeight, kItem.m_ucBlendIdx);
            kItem.m_pkBlendInterp->SetEaseSpinner(fEaseSpinner,
                kItem.m_ucBlendIdx);
            kItem.m_pkBlendInterp->SetTime(fTime, kItem.m_ucBlendIdx);
        }
    }
}
//---------------------------------------------------------------------------
bool NiControllerSequence::VerifyDependencies(
    NiControllerSequence* pkSequence)
{
    NiControllerSequence* pkPartnerSequence = pkSequence->m_pkPartnerSequence;
    if (pkPartnerSequence)
    {
        if (pkPartnerSequence == this)
        {
            return false;
        }
        else
        {
            return VerifyDependencies(pkPartnerSequence);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiControllerSequence::VerifyMatchingMorphKeys(
    NiControllerSequence* pkTimeSyncSeq)
{
    if (!m_spTextKeys || !pkTimeSyncSeq->m_spTextKeys)
    {
        return false;
    }

    unsigned int uiNumKeys1, uiNumKeys2;
    NiTextKey* pkKeys1 = pkTimeSyncSeq->m_spTextKeys->GetKeys(uiNumKeys1);
    NiTextKey* pkKeys2 = m_spTextKeys->GetKeys(uiNumKeys2);

    // Check that every "morph:" text key in pkTimeSyncSeq has an exact
    // match in this sequence.
    bool bMorphKeysExist1 = false;
    unsigned int ui;
    for (ui = 0; ui < uiNumKeys1; ui++)
    {
        const char* pcText1 = pkKeys1[ui].GetText();
        NIASSERT(pcText1);
        if (strncmp(pcText1, "morph:", 6) == 0)
        {
            bMorphKeysExist1 = true;
            bool bFound = false;
            for (unsigned int uj = 0; uj < uiNumKeys2; uj++)
            {
                const char* pcText2 = pkKeys2[uj].GetText();
                NIASSERT(pcText2);
                if (strcmp(pcText1, pcText2) == 0)
                {
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                return false;
            }
        }
    }
    if (!bMorphKeysExist1)
    {
        return false;
    }

    // Check that every "morph:" text key in this sequence has an exact match
    // in pkTimeSyncSequence.
    bool bMorphKeysExist2 = false;
    for (ui = 0; ui < uiNumKeys2; ui++)
    {
        const char* pcText2 = pkKeys2[ui].GetText();
        NIASSERT(pcText2);
        if (strncmp(pcText2, "morph:", 6) == 0)
        {
            bMorphKeysExist2 = true;
            bool bFound = false;
            for (unsigned int uj = 0; uj < uiNumKeys1; uj++)
            {
                const char* pcText1 = pkKeys1[uj].GetText();
                NIASSERT(pcText1);
                if (strcmp(pcText2, pcText1) == 0)
                {
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                return false;
            }
        }
    }
    if (!bMorphKeysExist2)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiControllerSequence::SetArraySize(unsigned int uiNewArraySize)
{
    // Allocate new arrays.
    InterpArrayItem* pkNewInterpArray = NiNew InterpArrayItem[uiNewArraySize];
    IDTag* pkNewIDTagArray = NiNew IDTag[uiNewArraySize];

    // Copy existing data.
    for (unsigned int ui = 0; ui < uiNewArraySize; ui++)
    {
        if (ui < m_uiArraySize)
        {
            pkNewInterpArray[ui] = m_pkInterpArray[ui];
            pkNewIDTagArray[ui] = m_pkIDTagArray[ui];
        }
    }

    // Delete existing data.
    NiDelete[] m_pkInterpArray;
    NiDelete[] m_pkIDTagArray;

    // Update variables.
    m_uiArraySize = uiNewArraySize;
    m_pkInterpArray = pkNewInterpArray;
    m_pkIDTagArray = pkNewIDTagArray;
}
//---------------------------------------------------------------------------
bool NiControllerSequence::ReallocateArrays()
{
    if (m_uiArrayGrowBy == 0)
    {
        return false;
    }

    // Allocate new arrays.
    unsigned int uiNewArraySize = m_uiArraySize + m_uiArrayGrowBy;
    InterpArrayItem* pkNewInterpArray = NiNew InterpArrayItem[uiNewArraySize];
    IDTag* pkNewIDTagArray = NiNew IDTag[uiNewArraySize];

    // Copy existing data.
    for (unsigned int ui = 0; ui < uiNewArraySize; ui++)
    {
        if (ui < m_uiArraySize)
        {
            pkNewInterpArray[ui] = m_pkInterpArray[ui];
            pkNewIDTagArray[ui] = m_pkIDTagArray[ui];
        }
    }

    // Delete existing data.
    NiDelete[] m_pkInterpArray;
    NiDelete[] m_pkIDTagArray;

    // Update variables.
    m_uiArraySize = uiNewArraySize;
    m_pkInterpArray = pkNewInterpArray;
    m_pkIDTagArray = pkNewIDTagArray;

    return true;
}
//---------------------------------------------------------------------------
bool NiControllerSequence::CanSyncTo(NiControllerSequence* pkTargetSequence)
{
    if (!pkTargetSequence)
        return false;

    if (!VerifyDependencies(pkTargetSequence) ||
        !VerifyMatchingMorphKeys(pkTargetSequence))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiControllerSequence);
//---------------------------------------------------------------------------
void NiControllerSequence::CopyMembers(NiControllerSequence* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    pkDest->SetName(m_kName);

    pkDest->m_uiArraySize = m_uiArraySize;
    pkDest->m_uiArrayGrowBy = m_uiArrayGrowBy;
    pkDest->m_pkInterpArray = NiNew InterpArrayItem[m_uiArraySize];
    pkDest->m_pkIDTagArray = NiNew IDTag[m_uiArraySize];
    for (unsigned int ui = 0; ui < m_uiArraySize; ui++)
    {
        if (m_pkInterpArray[ui].m_spInterpolator != NULL)
        {
            if (!m_pkOwner)
            {
                pkDest->m_pkInterpArray[ui].m_spInterpolator =
                    (NiInterpolator*) m_pkInterpArray[ui].m_spInterpolator
                    ->CreateSharedClone(kCloning);

                if (m_pkInterpArray[ui].m_spInterpCtlr != NULL)
                {
                    // This case should only happen if this sequence has
                    // been converted from an old sequence and has not been
                    // yet added to a manager. In this case, the interp
                    // controller must have CreateClone called on it, since
                    // it does not yet exist in the scene graph.
                    pkDest->m_pkInterpArray[ui].m_spInterpCtlr =
                        (NiInterpController*) m_pkInterpArray[ui]
                        .m_spInterpCtlr->CreateClone(kCloning);
                }
            }
            else
            {
                pkDest->m_pkInterpArray[ui].m_spInterpolator =
                    (NiInterpolator*) m_pkInterpArray[ui].m_spInterpolator
                    ->CreateSharedClone(kCloning);

                pkDest->m_pkInterpArray[ui].m_ucBlendIdx =
                    m_pkInterpArray[ui].m_ucBlendIdx;
            }
        }

        pkDest->m_pkIDTagArray[ui] = m_pkIDTagArray[ui];
    }

    pkDest->m_fSeqWeight = m_fSeqWeight;

    pkDest->m_spTextKeys = m_spTextKeys;

    pkDest->m_eCycleType = m_eCycleType;
    pkDest->m_fFrequency = m_fFrequency;
    pkDest->m_fBeginKeyTime = m_fBeginKeyTime;
    pkDest->m_fEndKeyTime = m_fEndKeyTime;

    pkDest->SetAccumRootName(m_kAccumRootName);
    pkDest->m_uFlags = m_uFlags; // For accum flags
}
//---------------------------------------------------------------------------
void NiControllerSequence::ProcessClone(NiCloningProcess& kCloning)
{
    NiObject::ProcessClone(kCloning);

    NiObject* pkClone = 0;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);
    NiControllerSequence* pkDest = (NiControllerSequence*) pkClone;

    for (unsigned int ui = 0; ui < m_uiArraySize; ui++)
    {
        if (m_pkInterpArray[ui].m_spInterpolator != NULL)
        {
            if (!m_pkOwner)
            {
                m_pkInterpArray[ui].m_spInterpolator->ProcessClone(kCloning);

                if (m_pkInterpArray[ui].m_spInterpCtlr)
                {
                    m_pkInterpArray[ui].m_spInterpCtlr->ProcessClone(
                        kCloning);
                }
            }
            else
            {
                if (m_eState == INACTIVE)
                {
                    m_pkInterpArray[ui].m_spInterpolator->ProcessClone(
                        kCloning);
                }
                else
                {
                    bCloned = kCloning.m_pkCloneMap->GetAt(
                        m_pkInterpArray[ui].m_spInterpolator, pkClone);
                    if (bCloned)
                    {
                        pkDest->m_pkInterpArray[ui].m_spInterpolator =
                            (NiInterpolator*) pkClone;
                    }
                    else
                    {
                        pkDest->m_pkInterpArray[ui].m_spInterpolator =
                            m_pkInterpArray[ui].m_spInterpolator;
                    }
                }

                bCloned = kCloning.m_pkCloneMap->GetAt(
                    m_pkInterpArray[ui].m_spInterpCtlr, pkClone);
                if (bCloned)
                {
                    pkDest->m_pkInterpArray[ui].m_spInterpCtlr =
                        (NiInterpController*) pkClone;
                }
                else
                {
                    pkDest->m_pkInterpArray[ui].m_spInterpCtlr =
                        m_pkInterpArray[ui].m_spInterpCtlr;
                }

                bCloned = kCloning.m_pkCloneMap->GetAt(
                    m_pkInterpArray[ui].m_pkBlendInterp, pkClone);
                if (bCloned)
                {
                    pkDest->m_pkInterpArray[ui].m_pkBlendInterp =
                        (NiBlendInterpolator*) pkClone;
                }
                else
                {
                    pkDest->m_pkInterpArray[ui].m_pkBlendInterp =
                        m_pkInterpArray[ui].m_pkBlendInterp;
                }
            }
        }
    }

    bCloned = kCloning.m_pkCloneMap->GetAt(m_pkOwner, pkClone);
    if (bCloned)
    {
        pkDest->m_pkOwner = (NiControllerManager*) pkClone;
    }
    else
    {
        pkDest->m_pkOwner = m_pkOwner;
    }

    bCloned = kCloning.m_pkCloneMap->GetAt(m_pkPartnerSequence, pkClone);
    if (bCloned)
    {
        pkDest->m_pkPartnerSequence = (NiControllerSequence*) pkClone;
    }
    else
    {
        pkDest->m_pkPartnerSequence = m_pkPartnerSequence;
    }

    bCloned = kCloning.m_pkCloneMap->GetAt(m_pkAccumRoot, pkClone);
    if (bCloned)
    {
        pkDest->m_pkAccumRoot = (NiAVObject*) pkClone;
    }
    else
    {
        pkDest->m_pkAccumRoot = m_pkAccumRoot;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiControllerSequence::InterpArrayItem::RegisterStreamables(
    NiStream& kStream)
{
    if (m_spInterpolator)
    {
        m_spInterpolator->RegisterStreamables(kStream);
    }
    if (m_spInterpCtlr)
    {
        m_spInterpCtlr->RegisterStreamables(kStream);
    }
}
//---------------------------------------------------------------------------
void NiControllerSequence::InterpArrayItem::SaveBinary(NiStream& kStream)
{
    kStream.SaveLinkID(m_spInterpolator);
    kStream.SaveLinkID(m_spInterpCtlr);

    // The blend interpolator pointer is not saved out because it will be
    // resolved again when the sequence is added to the manager upon load.
    // Saving this link ID will result in an assertion failure if
    // m_spInterpCtlr does not stream the blend interpolator. Because the
    // blend interpolator is not saved, neither is the blend index.
}
//---------------------------------------------------------------------------
void NiControllerSequence::InterpArrayItem::LoadBinary(NiStream& kStream)
{
    m_spInterpolator = (NiInterpolator*) kStream.ResolveLinkID();
    m_spInterpCtlr = (NiInterpController*) kStream.ResolveLinkID();

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 111))
    {
        // A link ID for the blend interpolator used to be saved out but is
        // no longer. This pointer will be resolved when the sequence is
        // added to an NiControllerManager.
        kStream.ResolveLinkID();

        // The blend index also used to be streamed but no longer needs to be.
        unsigned short usBlendIdx;
        NiStreamLoadBinary(kStream, usBlendIdx);
    }
}
//---------------------------------------------------------------------------
bool NiControllerSequence::InterpArrayItem::IsEqual(
    const InterpArrayItem& kDest)
{
    if ((m_spInterpolator && !kDest.m_spInterpolator) ||
        (!m_spInterpolator && kDest.m_spInterpolator) ||
        (m_spInterpolator && kDest.m_spInterpolator && 
        !m_spInterpolator->IsEqual(kDest.m_spInterpolator)))
    {
        return false;
    }

    if ((m_spInterpCtlr && !kDest.m_spInterpCtlr) ||
        (!m_spInterpCtlr && kDest.m_spInterpCtlr) ||
        (m_spInterpCtlr && kDest.m_spInterpCtlr &&
        !m_spInterpCtlr->IsEqual(kDest.m_spInterpCtlr)))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiControllerSequence::IDTag::RegisterStreamables(NiStream& kStream)
{
    kStream.RegisterFixedString(m_kAVObjectName);
    kStream.RegisterFixedString(m_kPropertyType);
    kStream.RegisterFixedString(m_kCtlrType);
    kStream.RegisterFixedString(m_kCtlrID);
    kStream.RegisterFixedString(m_kInterpolatorID);

}
//---------------------------------------------------------------------------
void NiControllerSequence::IDTag::SaveBinary(NiStream& kStream)
{
    kStream.SaveFixedString(m_kAVObjectName);
    kStream.SaveFixedString(m_kPropertyType);
    kStream.SaveFixedString(m_kCtlrType);
    kStream.SaveFixedString(m_kCtlrID);
    kStream.SaveFixedString(m_kInterpolatorID);
}
//---------------------------------------------------------------------------
void NiControllerSequence::IDTag::LoadBinary(NiStream& kStream,
    void*& pvDeprecatedExtraData)
{
    // There have been 3 representations of ID Tag's.
    // Each older version must be converted to use NiFixedStrings
    // The "pvDeprecatedExtraData" argument is used to contain the
    // NiStringPalette version. See it's comments below for more details.

    // All strings were stored as unique character arrays.
    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 113))
    {
        kStream.LoadCStringAsFixedString(m_kAVObjectName);
        kStream.LoadCStringAsFixedString(m_kPropertyType);
        kStream.LoadCStringAsFixedString(m_kCtlrType);
        kStream.LoadCStringAsFixedString(m_kCtlrID);
        kStream.LoadCStringAsFixedString(m_kInterpolatorID);
    }
    // All strings were stored in an NiStringPalette with handles 
    // that indicated the offset into the master character buffer
    // for each string. These strings were not ref-counted.
    else if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 1))
    {
        // All NiStringPalette's for a given NiControllerSequence should
        // point to the same value. If not, the conversion code will fail.
#ifdef _DEBUG
        void* pvPreviousExtraData = pvDeprecatedExtraData;
#endif

        // Each IDTag::LoadBinary checks agains the results of the previous
        // IDTag's NiStringPalette. Note that IDTags used to own a smart
        // pointer to the NiStringPalette. To save memory, this smart pointer
        // has not been preserved. It is assumed that the stream holds a 
        // pointer and the NiControllerSequence holds a pointer.
        pvDeprecatedExtraData = (NiStringPalette*) kStream.ResolveLinkID();

        NIASSERT((pvPreviousExtraData == NULL) ||
            (pvPreviousExtraData == pvDeprecatedExtraData));

        // We need to load in the handles for the NiStringPalette.
        unsigned int uiNameValue = 0;
        unsigned int uiPropertyType = 0;
        unsigned int uiCtlrType = 0;
        unsigned int uiCtlrID = 0;
        unsigned int uiInterpolatorID = 0;

        NiStreamLoadBinary(kStream, uiNameValue);
        NiStreamLoadBinary(kStream, uiPropertyType);
        NiStreamLoadBinary(kStream, uiCtlrType);
        NiStreamLoadBinary(kStream, uiCtlrID);
        NiStreamLoadBinary(kStream, uiInterpolatorID);

        // Because we have no way of knowing if NiStringPalette has been loaded
        // we must save the handles for use later. The NiFixedStrings that will
        // eventually contain the real values are our best candidate. However,
        // they only store character arrays. This leads to two implementation
        // options, convert NiFixedStrings to unsigned int storage or vice 
        // versa. For memory usage, we have chosen to implement the former.
        // Touching the NiFixedString values after this code may result in
        // a segfault.
        // IDTag::HandleDeprecatedExtraData will be called by the sequence
        // once all objects can be resolved.
#ifdef _DEBUG
        NIASSERT(sizeof(NiFixedString) >= sizeof(unsigned int));
        NiFixedString kDefault;
        NIASSERT(m_kAVObjectName == kDefault);
        NIASSERT(m_kPropertyType == kDefault);
        NIASSERT(m_kCtlrType == kDefault);
        NIASSERT(m_kCtlrID == kDefault);
        NIASSERT(m_kInterpolatorID == kDefault);
#endif

        *((unsigned int*)&m_kAVObjectName) = uiNameValue;
        *((unsigned int*)&m_kPropertyType) = uiPropertyType;
        *((unsigned int*)&m_kCtlrType) = uiCtlrType;
        *((unsigned int*)&m_kCtlrID) = uiCtlrID;
        *((unsigned int*)&m_kInterpolatorID) = uiInterpolatorID;
    }
    // This is the current approach of storing the id information in
    // NiFixedStrings
    else
    {
        kStream.LoadFixedString(m_kAVObjectName);
        kStream.LoadFixedString(m_kPropertyType);
        kStream.LoadFixedString(m_kCtlrType);
        kStream.LoadFixedString(m_kCtlrID);
        kStream.LoadFixedString(m_kInterpolatorID);
    }
}
//---------------------------------------------------------------------------
void NiControllerSequence::IDTag::HandleDeprecatedExtraData(
    NiStringPalette* pkPalette, NiStream& kStream)
{
    // This method is the companion to the conversion code in 
    // IDTag::LoadBinary. Please see the comments for the code above 
    // for implemetation details.

    // Note that this method is only valid when used for data with
    // NiStringPalette's and handles. Calling it on any other occasion
    // will result in bogus information.
    NIASSERT(kStream.GetFileVersion() >= NiStream::GetVersion(10, 1, 0, 113));
    NIASSERT(kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 1));
    NIASSERT(pkPalette);

    // Previously, we stored the handles in the NiFixedString's memory
    // space, making the NiFixedStrings dangerous to use.
    unsigned int uiAVObjectName = *((unsigned int*)&m_kAVObjectName);
    unsigned int uiPropertyType = *((unsigned int*)&m_kPropertyType);
    unsigned int uiCtlrType = *((unsigned int*)&m_kCtlrType);
    unsigned int uiCtlrID = *((unsigned int*)&m_kCtlrID);
    unsigned int uiInterpolatorID = 
        *((unsigned int*)&m_kInterpolatorID);

    // It is assumed that the NiFixedStrings were in their default
    // constructed forms prior to being clobbered over with the 
    // palette handles. We now return them to that previous state.
    NiFixedString kString;

    NiMemcpy(&m_kAVObjectName, sizeof(NiFixedString),
        &kString, sizeof(NiFixedString));
    NiMemcpy(&m_kPropertyType, sizeof(NiFixedString),
            &kString, sizeof(NiFixedString));
    NiMemcpy(&m_kCtlrType, sizeof(NiFixedString),
            &kString, sizeof(NiFixedString));
    NiMemcpy(&m_kCtlrID, sizeof(NiFixedString),
            &kString, sizeof(NiFixedString));
    NiMemcpy(&m_kInterpolatorID, sizeof(NiFixedString),
            &kString, sizeof(NiFixedString));

    // The handles may be in one of two states: invalid or valid.
    // Invalid handles are 0xffffffff. Since the previous block of
    // code has set the NiFixedStrings to their default, empty state,
    // we only need to set them if the handles are valid.

    if (uiAVObjectName != NiControllerSequence::INVALID_INDEX)
    {
        m_kAVObjectName = pkPalette->GetBasePointer() + 
            uiAVObjectName;
    }

    if (uiPropertyType != NiControllerSequence::INVALID_INDEX)
    {
        m_kPropertyType = pkPalette->GetBasePointer() + 
            uiPropertyType;
    }

    if (uiCtlrType != NiControllerSequence::INVALID_INDEX)
    {
        m_kCtlrType = pkPalette->GetBasePointer() + 
            uiCtlrType;
    }

    if (uiCtlrID != NiControllerSequence::INVALID_INDEX)
    {
        m_kCtlrID = pkPalette->GetBasePointer() + 
            uiCtlrID;
    }

    if (uiInterpolatorID != NiControllerSequence::INVALID_INDEX)
    {
        m_kInterpolatorID = pkPalette->GetBasePointer() + 
            uiInterpolatorID;
    }

}
//---------------------------------------------------------------------------
bool NiControllerSequence::IDTag::IsEqual(const IDTag& kDest)
{
    if (m_kAVObjectName != kDest.m_kAVObjectName ||
        m_kPropertyType != kDest.m_kPropertyType ||
        m_kCtlrType != kDest.m_kCtlrType ||
        m_kCtlrID != kDest.m_kCtlrID ||
        m_kInterpolatorID != kDest.m_kInterpolatorID)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiControllerSequence::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
    {
        return false;
    }

    for (unsigned int ui = 0; ui < m_uiArraySize; ui++)
    {
        m_pkInterpArray[ui].RegisterStreamables(kStream);
        m_pkIDTagArray[ui].RegisterStreamables(kStream);
    }

    if (m_spTextKeys)
    {
        m_spTextKeys->RegisterStreamables(kStream);
    }

    kStream.RegisterFixedString(m_kName);
    kStream.RegisterFixedString(m_kAccumRootName);
    return true;
}
//---------------------------------------------------------------------------
void NiControllerSequence::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    kStream.SaveFixedString(m_kName);

    NiStreamSaveBinary(kStream, m_uiArraySize);
    NiStreamSaveBinary(kStream, m_uiArrayGrowBy);
    for (unsigned int ui = 0; ui < m_uiArraySize; ui++)
    {
        m_pkInterpArray[ui].SaveBinary(kStream);
        m_pkIDTagArray[ui].SaveBinary(kStream);
    }

    NiStreamSaveBinary(kStream, m_fSeqWeight);

    kStream.SaveLinkID(m_spTextKeys);

    NiStreamSaveEnum(kStream, m_eCycleType);
    NiStreamSaveBinary(kStream, m_fFrequency);
    NiStreamSaveBinary(kStream, m_fBeginKeyTime);
    NiStreamSaveBinary(kStream, m_fEndKeyTime);

    kStream.SaveLinkID(m_pkOwner);

    kStream.SaveFixedString(m_kAccumRootName);
    NiStreamSaveBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiControllerSequence);

//---------------------------------------------------------------------------
void NiControllerSequence::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    // Conversion code.
    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        kStream.LoadCStringAsFixedString(m_kName);
        kStream.LoadCStringAsFixedString(m_kAccumRootName);
        kStream.ReadLinkID();   // m_spTextKeys

        NiStreamLoadBinary(kStream, m_uiArraySize);
        m_uiArrayGrowBy = 12;
        m_pkInterpArray = NiNew InterpArrayItem[m_uiArraySize];
        m_pkIDTagArray = NiNew IDTag[m_uiArraySize];

        for (unsigned int ui = 0; ui < m_uiArraySize; ui++)
        {
            NiFixedString kStr;
            kStream.LoadCStringAsFixedString(kStr);
            m_pkIDTagArray[ui].SetAVObjectName(kStr);

            kStream.ReadLinkID();   // m_kControllers[ui]
        }

        return;
    }

    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 1))
    {
        kStream.LoadCStringAsFixedString(m_kName);
    }
    else
    {
        kStream.LoadFixedString(m_kName);
    }

    NiStreamLoadBinary(kStream, m_uiArraySize);
    NiStreamLoadBinary(kStream, m_uiArrayGrowBy);
    m_pkInterpArray = NiNew InterpArrayItem[m_uiArraySize];
    m_pkIDTagArray = NiNew IDTag[m_uiArraySize];

    // The old string palette conversion code exists so 
    // that the older NiStringPalette/handle data can be
    // converted to NiFixedString's for the IDTag's.
    // Please see IDTag::LoadBinary for more information
    void* pvOldStringPaletteAddress = NULL;

    for (unsigned int ui = 0; ui < m_uiArraySize; ui++)
    {
        m_pkInterpArray[ui].LoadBinary(kStream);
        m_pkIDTagArray[ui].LoadBinary(kStream,
            pvOldStringPaletteAddress);
    }

    NiStreamLoadBinary(kStream, m_fSeqWeight);

    kStream.ReadLinkID();   // m_spTextKeys

    NiStreamLoadEnum(kStream, m_eCycleType);
    NiStreamLoadBinary(kStream, m_fFrequency);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 3, 0, 1))
    {
        // Phase is no longer supported and is thrown out.
        float fPhase;
        NiStreamLoadBinary(kStream, fPhase);
    }

    NiStreamLoadBinary(kStream, m_fBeginKeyTime);
    NiStreamLoadBinary(kStream, m_fEndKeyTime);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 107))
    {
        NiBool bPlayBackwards;
        NiStreamLoadBinary(kStream, bPlayBackwards);
    }

    kStream.ReadLinkID();   // m_pkOwner

    
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 1))
    {
        kStream.LoadCStringAsFixedString(m_kAccumRootName);      
    }
    else
    {
        kStream.LoadFixedString(m_kAccumRootName);
    }
    
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 8))
    {
        NiStreamLoadBinary(kStream, m_uFlags);
    }

    if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 1, 0, 113) &&
        kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 1))
    {
        // The old string palette conversion code exists so 
        // that the older NiStringPalette/handle data can be
        // converted to NiFixedString's for the IDTag's.
        // Please see IDTag::LoadBinary for more information
        
        NiStringPalette* pkPalette = 
            (NiStringPalette*) kStream.ResolveLinkID();
        NIASSERT(pvOldStringPaletteAddress == pkPalette);
        m_spDeprecatedStringPalette = pkPalette;
    }
    else
    {
        m_spDeprecatedStringPalette = NULL;
    }
}
//---------------------------------------------------------------------------
void NiControllerSequence::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);

    // Conversion code.
    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        m_spTextKeys = (NiTextKeyExtraData*) kStream.GetObjectFromLinkID();

        for (unsigned int ui = 0; ui < m_uiArraySize; ui++)
        {
            NiTimeController* pkCtlr = (NiTimeController*)
                kStream.GetObjectFromLinkID();
            NIASSERT(NiIsKindOf(NiInterpController, pkCtlr));
            m_pkInterpArray[ui].m_spInterpCtlr = (NiInterpController*) pkCtlr;
        }

        return;
    }

    m_spTextKeys = (NiTextKeyExtraData*) kStream.GetObjectFromLinkID();
    m_pkOwner = (NiControllerManager*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
void NiControllerSequence::PostLinkObject(NiStream& kStream)
{
    NiObject::PostLinkObject(kStream);

    if (m_spDeprecatedStringPalette)
    {
        // The old string palette conversion code exists so 
        // that the older NiStringPalette/handle data can be
        // converted to NiFixedString's for the IDTag's.
        // Please see IDTag::HandleDeprecatedExtraData for more information
        
        for (unsigned int ui = 0; ui < m_uiArraySize; ui++)
        {
           m_pkIDTagArray[ui].HandleDeprecatedExtraData( 
               m_spDeprecatedStringPalette, kStream);
        }

        // The NiStringPalette is no longer needed.
        m_spDeprecatedStringPalette = NULL;
    }
    
    // Conversion code.
    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        // Find maximum time range for sequence.
        m_fBeginKeyTime = NI_INFINITY;
        m_fEndKeyTime = -NI_INFINITY;
        unsigned int ui;
        for (ui = 0; ui < m_uiArraySize; ui++)
        {
            NiTimeController* pkCtlr = m_pkInterpArray[ui].m_spInterpCtlr;
            if (pkCtlr->GetBeginKeyTime() < m_fBeginKeyTime)
            {
                m_fBeginKeyTime = pkCtlr->GetBeginKeyTime();
            }
            if (pkCtlr->GetEndKeyTime() > m_fEndKeyTime)
            {
                m_fEndKeyTime = pkCtlr->GetEndKeyTime();
            }
        }
        if (m_fBeginKeyTime == NI_INFINITY && m_fEndKeyTime == -NI_INFINITY)
        {
            m_fBeginKeyTime = m_fEndKeyTime = 0.0f;
        }

        // Retreive names and controllers stored earlier.
        NiTPrimitiveArray<char*> kNames(m_uiArraySize);
        NiTObjectArray<NiInterpControllerPtr> kCtlrs(m_uiArraySize);
        for (ui = 0; ui < m_uiArraySize; ui++)
        {
            const char* pcStoredName = m_pkIDTagArray[ui].GetAVObjectName();
            NIASSERT(pcStoredName);
            unsigned int uiLen = strlen(pcStoredName) + 1;
            char* pcName = NiAlloc(char, uiLen);
            NiStrcpy(pcName, uiLen, pcStoredName);
            kNames.SetAt(ui, pcName);
            m_pkIDTagArray[ui].ClearValues();

            NiInterpController* pkCtlr = m_pkInterpArray[ui].m_spInterpCtlr;
            NIASSERT(pkCtlr);
            kCtlrs.SetAt(ui, pkCtlr);
            m_pkInterpArray[ui].ClearValues();
        }

        // Add name/controller pairs.
        NIASSERT(kNames.GetSize() == kCtlrs.GetSize() &&
            kNames.GetEffectiveSize() == kCtlrs.GetEffectiveSize());
        for (ui = 0; ui < kCtlrs.GetSize(); ui++)
        {
            NiInterpController* pkCtlr = kCtlrs.GetAt(ui);
            NIASSERT(pkCtlr);
            if (ui == 0)
            {
                m_eCycleType = pkCtlr->GetCycleType();
                if (m_eCycleType != NiTimeController::CLAMP &&
                    m_eCycleType != NiTimeController::LOOP)
                {
                    // Only LOOP and CLAMP are supported by sequences. Set
                    // this sequence to LOOP.
                    m_eCycleType = NiTimeController::LOOP;
                }
                m_fFrequency = pkCtlr->GetFrequency();
            }

            const char* pcName = kNames.GetAt(ui);
            NIASSERT(pcName);

            NIVERIFY(AddNameControllerPair(pcName, pkCtlr));
        }

        // Delete allocated names.
        for (ui = 0; ui < kNames.GetSize(); ui++)
        {
            NiFree(kNames.GetAt(ui));
        }

        return;
    }
}
//---------------------------------------------------------------------------
bool NiControllerSequence::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
    {
        return false;
    }

    NiControllerSequence* pkDest = (NiControllerSequence*) pkObject;

    if (m_kName != pkDest->m_kName)
    {
        return false;
    }

    if (m_uiArraySize != pkDest->m_uiArraySize ||
        m_uiArrayGrowBy != pkDest->m_uiArrayGrowBy)
    {
        return false;
    }

    for (unsigned int ui = 0; ui < m_uiArraySize; ui++)
    {
        if (!m_pkInterpArray[ui].IsEqual(pkDest->m_pkInterpArray[ui]) ||
            !m_pkIDTagArray[ui].IsEqual(pkDest->m_pkIDTagArray[ui]))
        {
            return false;
        }
    }

    if (m_fSeqWeight != pkDest->m_fSeqWeight)
    {
        return false;
    }

    if ((m_spTextKeys && !pkDest->m_spTextKeys) ||
        (!m_spTextKeys && pkDest->m_spTextKeys) ||
        (m_spTextKeys && pkDest->m_spTextKeys &&
            !m_spTextKeys->IsEqual(pkDest->m_spTextKeys)))
    {
        return false;
    }

    if (m_eCycleType != pkDest->m_eCycleType ||
        m_fFrequency != pkDest->m_fFrequency ||
        m_fBeginKeyTime != pkDest->m_fBeginKeyTime ||
        m_fEndKeyTime != pkDest->m_fEndKeyTime)
    {
        return false;
    }

    if ((m_pkOwner && !pkDest->m_pkOwner) ||
        (!m_pkOwner && pkDest->m_pkOwner) ||
        (m_pkOwner && pkDest->m_pkOwner &&
            !m_pkOwner->IsEqual(pkDest->m_pkOwner)))
    {
        return false;
    }

    if (m_kAccumRootName != pkDest->m_kAccumRootName)
    {
        return false;
    }
    
    if (m_uFlags != pkDest->m_uFlags)
    {
        return false;
    }
    
    return true;
}
//---------------------------------------------------------------------------
NiControllerSequence::CallbackObject::~CallbackObject()
{
}
//---------------------------------------------------------------------------
