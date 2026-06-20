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

#include "NiControllerExtractor.h"
#include "NiCommand.h"
#include <NiNode.h>
#include <NiStringExtraData.h>
#include <NiTransformInterpolator.h>
#include <NiTransformController.h>
#include <NiLinRotKey.h>
#include <NiLinPosKey.h>
#include <NiLinFloatKey.h>
#include <NiBezPosKey.h>
#include <NiMultiTargetTransformController.h>

static const char* gs_pcTransformControllerType = "NiTransformController;";

//---------------------------------------------------------------------------
NiControllerExtractor::SequenceInfo::SequenceInfo() : m_pcName(NULL),
    m_fStartTime(0.0f), m_fEndTime(0.0f), m_bLoop(false),
    m_bSmoothRotation(false), m_eRotFlags(NO_FLAGS), m_uiTransFlags(NO_FLAGS),
    m_kAccumAxis(NiMatrix3::IDENTITY), m_spTextKeys(NULL),
    m_spCharacterRoot(NULL), m_spLayerRoot(NULL), m_spAccumRoot(NULL)
{
}
//---------------------------------------------------------------------------
NiControllerExtractor::SequenceInfo::SequenceInfo(const char* pcName,
    float fStartTime, float fEndTime, bool bLoop, bool bSmoothRotation,
    AxisFlags eRotFlags, unsigned int uiTransFlags,
    const NiMatrix3& kAccumAxis, NiTextKeyExtraData* pkTextKeys,
    NiAVObject* pkCharacterRoot, NiAVObject* pkLayerRoot,
    NiAVObject* pkAccumRoot) : m_pcName(NULL),
    m_fStartTime(fStartTime), m_fEndTime(fEndTime), m_bLoop(bLoop),
    m_bSmoothRotation(bSmoothRotation), m_eRotFlags(eRotFlags),
    m_uiTransFlags(uiTransFlags), m_kAccumAxis(kAccumAxis),
    m_spTextKeys(pkTextKeys), m_spCharacterRoot(pkCharacterRoot),
    m_spLayerRoot(pkLayerRoot), m_spAccumRoot(pkAccumRoot)
{
    SetName(pcName);
}
//---------------------------------------------------------------------------
NiControllerExtractor::SequenceInfo::~SequenceInfo()
{
    NiFree(m_pcName);
}
//---------------------------------------------------------------------------
void NiControllerExtractor::SequenceInfo::SetName(const char* pcName)
{
    NiFree(m_pcName);
    m_pcName = NULL;

    if (pcName)
    {
        unsigned int uiLen = strlen(pcName) + 1;
        m_pcName = NiAlloc(char, uiLen);
        NiStrcpy(m_pcName, uiLen, pcName);
    }
}
//---------------------------------------------------------------------------
NiAVObject* NiControllerExtractor::SequenceInfo::GetSequenceRoot() const
{
    if (m_spLayerRoot)
    {
        return m_spLayerRoot;
    }
    else
    {
        return m_spCharacterRoot;
    }
}
//---------------------------------------------------------------------------
NiControllerExtractor::NiCCAGlobalInfo::NiCCAGlobalInfo() :
    m_bUseBSplineCompression(true), m_fBSplineCompressionRatio(100.0f),
    m_bUseCompactControlPoints(true)
{
}
//---------------------------------------------------------------------------
NiControllerExtractor::NiCCAInfo::NiCCAInfo() : m_pcSequenceName(NULL),
    m_bUseBSplineCompression(true), m_fBSplineCompressionRatio(100.0f),
    m_bUseCompactControlPoints(true)
{
}
//---------------------------------------------------------------------------
NiControllerExtractor::NiCCAInfo::NiCCAInfo(NiCCAGlobalInfo* pkInfo) :
    m_pcSequenceName(NULL), m_bUseBSplineCompression(true),
    m_fBSplineCompressionRatio(100.0f), m_bUseCompactControlPoints(true)
{
    
    if (pkInfo)
    {
        m_fBSplineCompressionRatio = pkInfo->m_fBSplineCompressionRatio;
        m_bUseBSplineCompression = pkInfo->m_bUseBSplineCompression;
        m_bUseCompactControlPoints = pkInfo->m_bUseCompactControlPoints;
    }
}
//---------------------------------------------------------------------------
NiControllerExtractor::NiCCAInfo::~NiCCAInfo()
{
    SetSequenceName(NULL);
}
//---------------------------------------------------------------------------
void NiControllerExtractor::NiCCAInfo::SetSequenceName(const char* pcName)
{
    NiFree(m_pcSequenceName);
    m_pcSequenceName = NULL;

    if (pcName)
    {
        unsigned int uiLen = strlen(pcName) + 1;
        m_pcSequenceName = NiAlloc(char, uiLen);
        NiStrcpy(m_pcSequenceName, uiLen, pcName);
    }
}
//---------------------------------------------------------------------------
void NiControllerExtractor::ProcessTextKeys(NiAVObject* pkObject,
    InfoArray& kInfoArray, NiCCAGlobalInfoArray& kCCAGlobalInfoArray, 
    NiCCAInfoArray& kCCAInfoArray, NiAVObject* pkLastCharacterRoot,
    NiCCAGlobalInfo* pkLastGlobalInfo)
{
    // Find NiTextKeyExtraData and process it to find out information about
    // separating multiple animation sequences in this scene.

    // Format for start key: "start [-name SequenceName] [-loop] [-smooth]
    //                              [-ar [all|none|x|y|z]]
    //                              [-at [all|none|{xyz}] ..."
    // Format for end key:   "end"

    for (unsigned int ui = 0; ui < pkObject->GetExtraDataSize(); ui++)
    {
        NiExtraData* pkExtra = pkObject->GetExtraDataAt(ui);
        if (NiIsKindOf(NiTextKeyExtraData, pkExtra))
        {
            NiTextKeyExtraDataPtr spTextKeys = (NiTextKeyExtraData*) pkExtra;
            pkObject->RemoveExtraDataAt(ui);

            NiAVObject* pkCharacterRoot = pkObject;
            NiAVObject* pkLayerRoot = NULL;
            SequenceInfo* pkLastSeqInfo = NULL;

            if (pkLastCharacterRoot)
            {
                pkCharacterRoot = pkLastCharacterRoot;
                pkLayerRoot = pkObject;
            }
            else
            {
                pkLastCharacterRoot = pkCharacterRoot;
            }
            NiAVObject* pkAccumRoot = FindAccumRoot(pkCharacterRoot);

            // Find start/end key pairs, store info for each pair as an item
            // in kInfoArray.
            unsigned int uiNumKeys;
            NiTextKey* pkKeys = spTextKeys->GetKeys(uiNumKeys);

            for (unsigned int uj = 0; uj < uiNumKeys; uj++)
            {
                const char* pcKeyText = pkKeys[uj].GetText();
                if (strncmp(pcKeyText, "start", 5) == 0)
                {
                    // Found start key.

                    // Look for corresponding end key.
                    float fEndTime;
                    unsigned int uk;
                    for (uk = uj + 1; uk < uiNumKeys; uk++)
                    {
                        if (strncmp(pkKeys[uk].GetText(), "end", 3) == 0)
                        {
                            // Found end key.
                            fEndTime = pkKeys[uk].GetTime();

                            // Set text on key to be exactly "end" to address
                            // bug with "end " being produced by art tools.
                            pkKeys[uk].SetText("end");

                            break;
                        }
                    }

                    if (uk == uiNumKeys)
                    {
                        // No corresponding end key found. Don't process this
                        // sequence.
                        continue;
                    }

                    NiCommand kCmd(pcKeyText);

                    // Parse name.
                    char acSeqName[80];
                    acSeqName[0] = 0;
                    kCmd.String("name", acSeqName, 80);

                    // No name; give it one (number in order encountered).
                    if (!acSeqName[0])
                    {
                        NiSprintf(acSeqName, 80, "%d", kInfoArray.GetSize());
                    }

                    // Parse transform accumulation options.
                    AxisFlags eRotFlags;
                    unsigned int uiTransFlags;
                    bool bLoop, bSmoothRotation;
                    NiMatrix3 kAccumAxis;
                    if (!ParseAccumulationOptions(kCmd, eRotFlags,
                        uiTransFlags, bLoop, bSmoothRotation, kAccumAxis))
                    {
                        // There was a problem processing the options.
                    }

                    float fStartTime = pkKeys[uj].GetTime();

                    // copy keys in range to sequence's set of keys
                    unsigned int uiSeqKeys = uk - uj + 1;
                    NIASSERT(uiSeqKeys > 0);
                    NiTextKey* pkSeqKeys = NiNew NiTextKey[uiSeqKeys];
                    pkSeqKeys[0].SetTime(0.0f);
                    pkSeqKeys[0].SetText("start");
                    for (unsigned int uiKey = uj + 1; uiKey <= uk; uiKey++)
                    {
                        pkSeqKeys[uiKey - uj].SetTime(pkKeys[uiKey].GetTime()
                            - fStartTime);
                        pkSeqKeys[uiKey - uj].SetText(
                            pkKeys[uiKey].GetText());
                    }
                    NiTextKeyExtraData* pkTextKeys = NiNew NiTextKeyExtraData(
                        pkSeqKeys, uiSeqKeys);

                    // Create sequence info object and add it to info array.
                    pkLastSeqInfo = NiNew SequenceInfo(acSeqName, fStartTime,
                        fEndTime, bLoop, bSmoothRotation, eRotFlags,
                        uiTransFlags, kAccumAxis, pkTextKeys, pkCharacterRoot,
                        pkLayerRoot, pkAccumRoot);
                    kInfoArray.Add(pkLastSeqInfo);
                }

                NiCommand kCmd(pcKeyText);
                
                // Handle global compression options
                if (strstr(pcKeyText, "-Global") != 0)
                {
                    NiCCAGlobalInfoPtr spGlobalInfo = NiNew NiCCAGlobalInfo();
                    spGlobalInfo->m_spCharacterRoot = pkCharacterRoot;
                    char acBool[10];
                    if (kCmd.String("GlobalDontCompress", acBool, 10))
                    {
                        if (NiStricmp(acBool, "true") == 0)
                            spGlobalInfo->m_bUseBSplineCompression = false;
                        else
                            spGlobalInfo->m_bUseBSplineCompression = true;
                    }
                    
                    int iRatio;
                    if (kCmd.Integer("GlobalRatio", iRatio))
                    {
                        spGlobalInfo->m_fBSplineCompressionRatio = 
                            ((float) iRatio) / 100.0f;
                    }

                    if (kCmd.String("GlobalCompressFloats", acBool, 10))
                    {
                        if (NiStricmp(acBool, "true") == 0)
                            spGlobalInfo->m_bUseCompactControlPoints = true;
                        else
                            spGlobalInfo->m_bUseCompactControlPoints = false;
                    }
                    
                    kCCAGlobalInfoArray.Add(spGlobalInfo);
                    pkLastGlobalInfo = spGlobalInfo;
                }

                // Handle sequence-level compression options
                if (strstr(pcKeyText, "-Seq") != 0)
                {
                    NiCCAInfoPtr spInfo = NiNew NiCCAInfo(pkLastGlobalInfo);
                    spInfo->m_spCharacterRoot = pkCharacterRoot;
                    spInfo->m_spObject = NULL;
                    spInfo->SetSequenceName(pkLastSeqInfo->m_pcName);

                    char acBool[10];
                    if (kCmd.String("SeqDontCompress", acBool, 10))
                    {
                        if (NiStricmp(acBool, "true") == 0)
                            spInfo->m_bUseBSplineCompression = false;
                        else
                            spInfo->m_bUseBSplineCompression = true;
                    }
                    
                    int iRatio;
                    if (kCmd.Integer("SeqRatio", iRatio))
                    {
                        spInfo->m_fBSplineCompressionRatio = 
                            ((float) iRatio) / 100.0f;
                    }

                    if (kCmd.String("SeqCompressFloats", acBool, 10))
                    {
                        if (NiStricmp(acBool, "true") == 0)
                            spInfo->m_bUseCompactControlPoints = true;
                        else
                            spInfo->m_bUseCompactControlPoints = false;
                    }
                    
                    kCCAInfoArray.Add(spInfo);
                }

                // Handle object-level compression options
                if (strstr(pcKeyText, "-Obj") != 0)
                {
                    NiCCAInfoPtr spInfo = NiNew NiCCAInfo(pkLastGlobalInfo);
                    spInfo->m_spCharacterRoot = pkCharacterRoot;
                    spInfo->m_spObject = pkObject;
                    spInfo->SetSequenceName(NULL);

                    char acBool[10];
                    if (kCmd.String("ObjDontCompress", acBool, 10))
                    {
                        if (NiStricmp(acBool, "true") == 0)
                            spInfo->m_bUseBSplineCompression = false;
                        else
                            spInfo->m_bUseBSplineCompression = true;
                    }
                    
                    int iRatio;
                    if (kCmd.Integer("ObjRatio", iRatio))
                    {
                        spInfo->m_fBSplineCompressionRatio = 
                            ((float) iRatio) / 100.0f;
                    }

                    if (kCmd.String("ObjCompressFloats", acBool, 10))
                    {
                        if (NiStricmp(acBool, "true") == 0)
                            spInfo->m_bUseCompactControlPoints = true;
                        else
                            spInfo->m_bUseCompactControlPoints = false;
                    }
                    
                    kCCAInfoArray.Add(spInfo);
                }
                
                // Handle object/sequence-level compression options
                for (unsigned int ui = 0; ui < kInfoArray.GetSize(); 
                    ui++)
                {
                    SequenceInfo* pkInfo = kInfoArray.GetAt(ui);
                    if (!pkInfo)
                        continue;

                    char acString[512];
                    NiSprintf(acString, 512, "-%s", pkInfo->m_pcName);
                    if (strstr(pcKeyText, acString) != 0)
                    {
                        NiCCAInfoPtr spInfo = NiNew NiCCAInfo(
                            pkLastGlobalInfo);
                        spInfo->m_spCharacterRoot = pkCharacterRoot;
                        spInfo->m_spObject = pkObject;
                        spInfo->SetSequenceName(pkInfo->m_pcName);

                        char acStringArg[512];
                        NiSprintf(acStringArg, 512, "%s%s", pkInfo->m_pcName, 
                            "DontCompress");
                        char acBool[10];
                        if (kCmd.String(acStringArg, acBool, 10))
                        {
                            if (NiStricmp(acBool, "true") == 0)
                                spInfo->m_bUseBSplineCompression = false;
                            else
                                spInfo->m_bUseBSplineCompression = true;
                        }
                        
                        int iRatio;
                        NiSprintf(acStringArg, 512, "%s%s", pkInfo->m_pcName, 
                            "Ratio");
                        if (kCmd.Integer(acStringArg, iRatio))
                        {
                            spInfo->m_fBSplineCompressionRatio = 
                                ((float) iRatio) / 100.0f;
                        }
                        
                        NiSprintf(acStringArg, 512, "%s%s", pkInfo->m_pcName, 
                            "CompressFloats");

                        if (kCmd.String(acStringArg, acBool, 10))
                        {
                            if (NiStricmp(acBool, "true") == 0)
                                spInfo->m_bUseCompactControlPoints = true;
                            else
                                spInfo->m_bUseCompactControlPoints = false;
                        }
                        
                        kCCAInfoArray.Add(spInfo);
                    }
                }
            }
            
            break;
        }
        
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                ProcessTextKeys(pkChild, kInfoArray, kCCAGlobalInfoArray,
                    kCCAInfoArray, pkLastCharacterRoot, pkLastGlobalInfo);
            }
        }
    }
}
//---------------------------------------------------------------------------
NiControllerSequence* NiControllerExtractor::BuildSequence(
    SequenceInfo& kInfo, bool bRemoveInterpolators, bool bStoreControllers,
    const char* pcExcludedControllerTypes)
{
    EnsureUniqueNames(kInfo.GetSequenceRoot());
    NiInterpController::GuaranteeTimeRange(kInfo.GetSequenceRoot(),
        kInfo.m_fStartTime, kInfo.m_fEndTime, pcExcludedControllerTypes);

    InterpolatorArray kInterpolatorArray(12, 12);
    IDArray kIDArray(12, 12);
    ControllerArray kControllerArray(12, 12);
    float fFrequency, fPhase;
    NiTPointerMap<NiInterpolator*, bool> kProcessedInterpolators;

    ExtractInterpolators(kInfo.GetSequenceRoot(), kInfo, kInterpolatorArray,
        kIDArray, bStoreControllers, kControllerArray, fFrequency, fPhase,
        kProcessedInterpolators, pcExcludedControllerTypes);

    unsigned int uiArraySize = kInterpolatorArray.GetSize();

    NIASSERT(uiArraySize == kInterpolatorArray.GetEffectiveSize() &&
        uiArraySize == kIDArray.GetSize() &&
        uiArraySize == kIDArray.GetEffectiveSize());
    NIASSERT(!bStoreControllers ||
        (kControllerArray.GetSize() == uiArraySize &&
            kControllerArray.GetEffectiveSize() == uiArraySize));

    if (uiArraySize == 0)
    {
        return NULL;
    }

    NiControllerSequence* pkSequence = NiNew NiControllerSequence(
        kInfo.m_pcName, uiArraySize, 1);
    pkSequence->SetBeginKeyTime(0.0f);
    pkSequence->SetEndKeyTime(kInfo.m_fEndTime - kInfo.m_fStartTime);
    if (kInfo.m_bLoop)
    {
        pkSequence->SetCycleType(NiTimeController::LOOP);
    }
    else
    {
        pkSequence->SetCycleType(NiTimeController::CLAMP);
    }
    pkSequence->SetFrequency(fFrequency);
    unsigned int ui;
    for (ui = 0; ui < uiArraySize; ui++)
    {
        NiInterpolator* pkInterpolator = kInterpolatorArray.GetAt(ui);
        NiControllerSequence::IDTag* pkIDTag = kIDArray.GetAt(ui);
        NIASSERT(pkInterpolator && pkIDTag);
        unsigned int uiNewIndex = pkSequence->AddInterpolator(pkInterpolator,
            *pkIDTag);
        NiDelete pkIDTag;

        pkInterpolator->Collapse();

        if (bStoreControllers)
        {
            NiInterpController* pkInterpCtlr = kControllerArray.GetAt(ui);
            NIASSERT(pkInterpCtlr);
            pkSequence->SetControllerAt(pkInterpCtlr, uiNewIndex);
        }
    }
    pkSequence->SetTextKeys(kInfo.m_spTextKeys);

    if (!kInfo.m_spAccumRoot)
    {
        kInfo.m_spAccumRoot = kInfo.m_spCharacterRoot;
    }
    RearrangeTransforms(kInfo.m_spAccumRoot, pkSequence, kInfo,
        bStoreControllers, pcExcludedControllerTypes);
    pkSequence->SetAccumRootName(kInfo.m_spAccumRoot->GetName());

    if (bRemoveInterpolators)
    {
        RemoveInterpolators(kInfo.GetSequenceRoot(),
            pcExcludedControllerTypes);
        if (!pcExcludedControllerTypes || !strstr(pcExcludedControllerTypes,
            gs_pcTransformControllerType))
        {
            ConsolidateTransformControllers(kInfo.GetSequenceRoot(),
                kInfo.m_spAccumRoot);
        }
    }

    return pkSequence;
}
//---------------------------------------------------------------------------
void NiControllerExtractor::BuildSequences(const InfoArray& kInfoArray,
    SequenceArray& kSequenceArray, bool bRemoveInterpolators,
    bool bStoreControllers, const char* pcExcludedControllerTypes)
{
    kSequenceArray.RemoveAll();
    kSequenceArray.SetSize(kInfoArray.GetEffectiveSize());

    unsigned int ui;
    for (ui = 0; ui < kInfoArray.GetSize(); ui++)
    {
        SequenceInfo* pkInfo = kInfoArray.GetAt(ui);
        if (pkInfo)
        {
            NiControllerSequence* pkSequence = BuildSequence(*pkInfo, false,
                bStoreControllers, pcExcludedControllerTypes);
            if (pkSequence)
            {
                kSequenceArray.SetAt(ui, pkSequence);
            }
        }
    }

    if (bRemoveInterpolators)
    {
        bool bConsolidated = false;
        for (ui = 0; ui < kInfoArray.GetSize(); ui++)
        {
            SequenceInfo* pkInfo = kInfoArray.GetAt(ui);
            if (pkInfo)
            {
                RemoveInterpolators(pkInfo->GetSequenceRoot(),
                    pcExcludedControllerTypes);
                if (!bConsolidated && (!pcExcludedControllerTypes || !strstr(
                    pcExcludedControllerTypes, gs_pcTransformControllerType)))
                {
                    ConsolidateTransformControllers(pkInfo->GetSequenceRoot(),
                        pkInfo->m_spAccumRoot);
                    bConsolidated = true;
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiControllerExtractor::RemoveInterpolators(NiAVObject* pkObject,
    const char* pcExcludedControllerTypes)
{
    char acTemp[NI_MAX_PATH];

    // Remove all interpolators from this object.
    NiTimeController* pkCtlr = pkObject->GetControllers();
    while (pkCtlr)
    {
        NiInterpController* pkInterpCtlr = NiDynamicCast(NiInterpController,
            pkCtlr);
        if (pkInterpCtlr)
        {
            NiSprintf(acTemp, NI_MAX_PATH, "%s;",
                pkInterpCtlr->GetRTTI()->GetName());
            if (!pcExcludedControllerTypes ||
                !strstr(pcExcludedControllerTypes, acTemp))
            {
                pkInterpCtlr->SetManagerControlled(true);
                for (unsigned short us = 0;
                    us < pkInterpCtlr->GetInterpolatorCount(); us++)
                {
                    pkInterpCtlr->SetInterpolator(NULL, us);
                }
            }
        }
        pkCtlr = pkCtlr->GetNext();
    }

    // Iterate through all the properties on this object.
    NiTListIterator kPos = pkObject->GetPropertyList().GetHeadPos();
    while (kPos)
    {
        NiProperty* pkProp = pkObject->GetPropertyList().GetNext(kPos);
        if (pkProp)
        {
            // Remove all interpolators from this property.
            pkCtlr = pkProp->GetControllers();
            while (pkCtlr)
            {
                NiInterpController* pkInterpCtlr = NiDynamicCast(
                    NiInterpController, pkCtlr);
                if (pkInterpCtlr)
                {
                    NiSprintf(acTemp, NI_MAX_PATH, "%s;",
                        pkInterpCtlr->GetRTTI()->GetName());
                    if (!pcExcludedControllerTypes ||
                        !strstr(pcExcludedControllerTypes, acTemp))
                    {
                        pkInterpCtlr->SetManagerControlled(true);
                        for (unsigned short us = 0;
                            us < pkInterpCtlr->GetInterpolatorCount(); us++)
                        {
                            pkInterpCtlr->SetInterpolator(NULL, us);
                        }
                    }
                }
                pkCtlr = pkCtlr->GetNext();
            }
        }
    }

    // Recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                RemoveInterpolators(pkChild, pcExcludedControllerTypes);
            }
        }
    }
}
//---------------------------------------------------------------------------
NiAVObject* NiControllerExtractor::FindAccumRoot(NiAVObject* pkObject)
{
    unsigned int ui;
    for (ui = 0; ui < pkObject->GetExtraDataSize(); ui++)
    {
        NiStringExtraData* pkStringExtra = NiDynamicCast(NiStringExtraData,
            pkObject->GetExtraDataAt(ui));
        if (pkStringExtra)
        {
            const char* pcString = pkStringExtra->GetValue();
            if (pcString && strstr(pcString, "KFAccumRoot"))
            {
                if (strcmp(pcString, "KFAccumRoot") == 0 ||
                    strcmp(pcString, "KFAccumRoot =") == 0)
                {
                    if (!pkObject->RemoveExtraDataAt(ui))
                    {
                        NiOutputDebugString("Error: Unsuccessful attempt to "
                            "remove extra data.\n");
                    }
                }

                return pkObject;
            }
        }
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                NiAVObject* pkAccumRoot = FindAccumRoot(pkChild);
                if (pkAccumRoot)
                {
                    return pkAccumRoot;
                }
            }
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
bool NiControllerExtractor::ParseAccumulationOptions(NiCommand& kCmd,
    AxisFlags& eRotFlags, unsigned int& uiTransFlags, bool& bLoop,
    bool& bSmoothRotation, NiMatrix3& kAccumAxis)
{
    kCmd.Boolean("smooth", bSmoothRotation);
    kCmd.Boolean("loop", bLoop);

    eRotFlags = NO_FLAGS;
    uiTransFlags = NO_FLAGS;
    char acAccumRot[16], acAccumTrans[16], acAccumAxis[16];
    acAccumAxis[0] = acAccumRot[0] = acAccumTrans[0] = '\0';

    // Rotations
    kCmd.String ("ar", acAccumRot, 16);
    if (strncmp(acAccumRot, "all", 3) == 0)
    {
        eRotFlags = ALL_FLAGS;
    }
    else if (strncmp(acAccumRot, "none", 4) == 0)
    {
        eRotFlags = NO_FLAGS;
    }
    else if (strncmp(acAccumRot, "x", 1) == 0)
    {
        eRotFlags = X_FLAG;
    }
    else if (strncmp(acAccumRot, "y", 1) == 0)
    {
        eRotFlags = Y_FLAG;
    }
    else if (strncmp(acAccumRot, "z", 1) == 0)
    {
        eRotFlags = Z_FLAG;
    }
    else if (acAccumRot[0] != '\0')
    {
        NiOutputDebugString("-ar flag requires \"all\", \"none\", \"x\", "
            "\"y\", or \"z\"\n");
        return false;
    }

    // Translations
    kCmd.String ("at", acAccumTrans, 16);
    if (strncmp(acAccumTrans, "all", 3) == 0)
    {
        uiTransFlags = ALL_FLAGS;
    }
    else if (strncmp(acAccumTrans, "none", 4) == 0)
    {
        uiTransFlags = NO_FLAGS;
    }
    else if (acAccumTrans[0] != '\0')
    {
        uiTransFlags = NO_FLAGS;
        if (strchr(acAccumTrans, 'x'))
        {
            uiTransFlags |= X_FLAG;
        }
        if (strchr(acAccumTrans, 'y'))
        {
            uiTransFlags |= Y_FLAG;
        }
        if (strchr(acAccumTrans, 'z'))
        {
            uiTransFlags |= Z_FLAG;
        }

        if (uiTransFlags == NO_FLAGS)
        {
            NiOutputDebugString("-at flag requires \"all\", \"none\", or a "
                "combination of \"x\",\"y\", and/or \"z\"\n");
            return false;
        }
    }

    // Axis around which to accumulate rotations.
    // Front faces x    -> MakeIdentity
    //             negx -> MakeZRotation(NI_PI)
    //             y    -> MakeZRotation(NI_HALF_PI)
    //             negy -> MakeZRotation(-NI_HALF_PI)
    // Character Studio defaults to -y, but identity is the default here
    // so that customers do not have to convert their old datasets.
    kAccumAxis.MakeIdentity();
    if (eRotFlags == ALL_FLAGS ||
        eRotFlags == Z_FLAG)
    {
        kCmd.String("front", acAccumAxis, 16);
        if (strncmp(acAccumAxis, "x", 1) == 0)
        {
            kAccumAxis.MakeIdentity();
        }
        else if (strncmp(acAccumAxis, "y", 1) == 0)
        {
            kAccumAxis.MakeZRotation(NI_HALF_PI);
        }
        else if (strncmp(acAccumAxis, "negx", 4) == 0)
        {
            kAccumAxis.MakeZRotation(NI_PI);
        }
        else if (strncmp(acAccumAxis, "negy", 4) == 0)
        {
            kAccumAxis.MakeZRotation(-NI_HALF_PI);
        }
        else if (acAccumAxis[0] != '\0')
        {
            NiOutputDebugString("-front flag requires \"x\", \"y\", "
                "\"negx\", or \"negy\".\n");
            return false;
        }
    }
    
    return true;
}
//---------------------------------------------------------------------------
void NiControllerExtractor::EnsureUniqueNames(NiAVObject* pkObject)
{
    NameArray kNameArray;
    unsigned int uiIndex = 0;
    EnsureUniqueNameRecursive(pkObject, kNameArray, uiIndex);
}
//---------------------------------------------------------------------------
void NiControllerExtractor::EnsureUniqueNameRecursive(NiAVObject* pkObject,
    NameArray& kNameArray, unsigned int& uiIndex)
{
    // Check the name on this object.
    const char* pcOrigName = pkObject->GetName();
    if (!pcOrigName)
    {
        char acNewName[256];
        NiSprintf(acNewName, 256, "object%u", uiIndex++);
        pkObject->SetName(acNewName);
        kNameArray.Add(acNewName);
    }
    else
    {
        bool bFoundDuplicate = false;
        for (unsigned int ui = 0; ui < kNameArray.GetSize(); ui++)
        {
            const char* pcStoredName = kNameArray.GetAt(ui);
            if (pcStoredName && strcmp(pcStoredName, pcOrigName) == 0)
            {
                char acNewName[256];
                NiSprintf(acNewName, 256, "%s@#%u", pcOrigName, uiIndex++);
                pkObject->SetName(acNewName);
                kNameArray.Add(acNewName);
                bFoundDuplicate = true;
                break;
            }
        }
        if (!bFoundDuplicate)
        {
            kNameArray.Add((char*) pcOrigName);
        }
    }

    // Recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                EnsureUniqueNameRecursive(pkChild, kNameArray, uiIndex);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiControllerExtractor::ExtractInterpolators(NiAVObject* pkObject,
    const SequenceInfo& kInfo, InterpolatorArray& kInterpolatorArray,
    IDArray& kIDArray, bool bStoreControllers,
    ControllerArray& kControllerArray, float& fFrequency, float& fPhase,
    NiTPointerMap<NiInterpolator*, bool>& kProcessedInterpolators,
    const char* pcExcludedControllerTypes)
{
    char acTemp[NI_MAX_PATH];

    const char* pcObjectName = pkObject->GetName();

    // Extract time controllers on this object.
    NiTimeController* pkCtlr = pkObject->GetControllers();
    while (pkCtlr)
    {
        NiInterpController* pkInterpCtlr = NiDynamicCast(
            NiInterpController, pkCtlr);
        if (pkInterpCtlr)
        {
            NiSprintf(acTemp, NI_MAX_PATH, "%s;",
                pkInterpCtlr->GetRTTI()->GetName());
            if (!pcExcludedControllerTypes ||
                !strstr(pcExcludedControllerTypes, acTemp))
            {
                for (unsigned short us = 0;
                    us < pkInterpCtlr->GetInterpolatorCount(); us++)
                {
                    NiInterpolator* pkInterpolator =
                        pkInterpCtlr->GetInterpolator(us);
                    if (pkInterpolator)
                    {
                        bool bTemp;
                        if (kProcessedInterpolators.GetAt(pkInterpolator,
                            bTemp))
                        {
                            continue;
                        }
                        kProcessedInterpolators.SetAt(pkInterpolator, true);

                        NiInterpolator* pkSeqInterp =
                            pkInterpolator->GetSequenceInterpolator(
                            kInfo.m_fStartTime, kInfo.m_fEndTime);
                        if (pkSeqInterp)
                        {
                            kInterpolatorArray.Add(pkSeqInterp);
                            kIDArray.Add(NiNew NiControllerSequence::IDTag(
                                pcObjectName, NULL,
                                pkInterpCtlr->GetRTTI()->GetName(),
                                pkInterpCtlr->GetCtlrID(),
                                pkInterpCtlr->GetInterpolatorID(us)));
                            if (bStoreControllers)
                            {
                                NiTimeControllerPtr spNext = pkInterpCtlr
                                    ->GetNext();
                                pkInterpCtlr->SetNext(NULL);
                                NiInterpController* pkSeqCtlr =
                                    (NiInterpController*)
                                    pkInterpCtlr->Clone();
                                pkInterpCtlr->SetNext(spNext);
                                for (unsigned short usTemp = 0;
                                    usTemp < pkSeqCtlr->GetInterpolatorCount();
                                    usTemp++)
                                {
                                    pkSeqCtlr->SetInterpolator(NULL, usTemp);
                                }
                                kControllerArray.Add(pkSeqCtlr);
                                pkSeqCtlr->SetTarget(NULL);
                            }
                            fFrequency = pkInterpCtlr->GetFrequency();
                            fPhase = pkInterpCtlr->GetPhase();
                        }
                    }
                }
            }
        }
        pkCtlr = pkCtlr->GetNext();
    }

    // Iterate through the properties on this object.
    NiTListIterator kPos = pkObject->GetPropertyList().GetHeadPos();
    while (kPos)
    {
        NiProperty* pkProp = pkObject->GetPropertyList().GetNext(kPos);
        if (pkProp)
        {
            // Extract time controllers on this property.
            pkCtlr = pkProp->GetControllers();
            while (pkCtlr)
            {
                NiInterpController* pkInterpCtlr = NiDynamicCast(
                    NiInterpController, pkCtlr);
                if (pkInterpCtlr)
                {
                    NiSprintf(acTemp, NI_MAX_PATH, "%s;",
                        pkInterpCtlr->GetRTTI()->GetName());
                    if (!pcExcludedControllerTypes ||
                        !strstr(pcExcludedControllerTypes, acTemp))
                    {
                        for (unsigned short us = 0;
                            us < pkInterpCtlr->GetInterpolatorCount(); us++)
                        {
                            NiInterpolator* pkInterpolator =
                                pkInterpCtlr->GetInterpolator(us);
                            if (pkInterpolator)
                            {
                                bool bTemp;
                                if (kProcessedInterpolators.GetAt(
                                    pkInterpolator, bTemp))
                                {
                                    continue;
                                }
                                kProcessedInterpolators.SetAt(pkInterpolator,
                                    true);

                                NiInterpolator* pkSeqInterp =
                                    pkInterpolator->GetSequenceInterpolator(
                                    kInfo.m_fStartTime, kInfo.m_fEndTime);
                                if (pkSeqInterp)
                                {
                                    kInterpolatorArray.Add(pkSeqInterp);
                                    kIDArray.Add(NiNew
                                        NiControllerSequence::IDTag(
                                        pcObjectName,
                                        pkProp->GetRTTI()->GetName(),
                                        pkInterpCtlr->GetRTTI()->GetName(),
                                        pkInterpCtlr->GetCtlrID(),
                                        pkInterpCtlr->GetInterpolatorID(us)));
                                    if (bStoreControllers)
                                    {
                                        NiTimeControllerPtr spNext =
                                            pkInterpCtlr->GetNext();
                                        pkInterpCtlr->SetNext(NULL);
                                        NiInterpController* pkSeqCtlr =
                                            (NiInterpController*)
                                            pkInterpCtlr->Clone();
                                        pkInterpCtlr->SetNext(spNext);
                                        for (unsigned short usTemp = 0;
                                            usTemp <
                                            pkSeqCtlr->GetInterpolatorCount();
                                            usTemp++)
                                        {
                                            pkSeqCtlr->SetInterpolator(NULL,
                                                usTemp);
                                        }
                                        pkSeqCtlr->SetInterpolator(pkSeqInterp,
                                            us);
                                        kControllerArray.Add(pkSeqCtlr);
                                        pkSeqCtlr->SetTarget(NULL);
                                    }
                                    fFrequency = pkInterpCtlr->GetFrequency();
                                    fPhase = pkInterpCtlr->GetPhase();
                                }
                            }
                        }
                    }
                }
                pkCtlr = pkCtlr->GetNext();
            }
        }
    }

    // Recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                ExtractInterpolators(pkChild, kInfo, kInterpolatorArray,
                    kIDArray, bStoreControllers, kControllerArray, fFrequency,
                    fPhase, kProcessedInterpolators,
                    pcExcludedControllerTypes);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiControllerExtractor::RearrangeTransforms(NiAVObject* pkAccumRoot,
    NiControllerSequence* pkSequence, const SequenceInfo& kInfo,
    bool bStoreControllers, const char* pcExcludedControllerTypes)
{
    // If this is a layer sequence, exit immediately.
    if (kInfo.m_spLayerRoot)
    {
        return;
    }

    // Get the base (Accum) node.
    NiNode* pkBaseNode = NiDynamicCast(NiNode, pkAccumRoot);
    if (!pkBaseNode)
    {
        // Transformation accumulation should only be happening to NiNode
        // objects.
        NiOutputDebugString("NiControllerExtractor: Trying to rearrange "
            "transforms on a non-NiNode. Transformations will not be "
            "rearranged.\n");
        return;
    }

    // Get/create the child (NonAccum) node.
    NiNode* pkChildNode = GetChildNode(pkBaseNode);

    // If NiTransformControllers are not being extracted, return now to avoid
    // overwriting their interpolators.
    if (pcExcludedControllerTypes && strstr(pcExcludedControllerTypes,
        gs_pcTransformControllerType))
    {
        // If we're not extracting tansforms, it's equivalent to
        // everything staying on the accumulation node
        pkSequence->SetAccumTranslation(true, true, true);
        pkSequence->SetAccumRotation(true, true, true);
        pkSequence->SetAccumFrontFacing(0, false);
        return;
    }

    // Get the base interpolator.
    NiTransformInterpolator* pkBaseInterp = GetBaseInterpolator(pkSequence,
        pkBaseNode, kInfo.m_bLoop, bStoreControllers);

    // Set all the accumulation flags
    pkSequence->SetAccumTranslation(
        kInfo.m_uiTransFlags & X_FLAG,
        kInfo.m_uiTransFlags & Y_FLAG,
        kInfo.m_uiTransFlags & Z_FLAG);
    pkSequence->SetAccumRotation(
        kInfo.m_eRotFlags & X_FLAG,
        kInfo.m_eRotFlags & Y_FLAG,
        kInfo.m_eRotFlags & Z_FLAG);
    if (kInfo.m_kAccumAxis.GetEntry(0, 0) > 0.9f &&
        kInfo.m_kAccumAxis.GetEntry(0, 0) < 1.1f)
    {
        pkSequence->SetAccumFrontFacing(0, false);
    }
    else if (kInfo.m_kAccumAxis.GetEntry(0, 0) > -1.1f &&
        kInfo.m_kAccumAxis.GetEntry(0, 0) < -0.9f)
    {
        pkSequence->SetAccumFrontFacing(0, true);
    }
    else if (kInfo.m_kAccumAxis.GetEntry(0, 1) > 0.9f &&
        kInfo.m_kAccumAxis.GetEntry(0, 1) < 1.1f)
    {
        pkSequence->SetAccumFrontFacing(1, false);
    }
    else
    {
        pkSequence->SetAccumFrontFacing(1, true);
    }

    // Check whether transforms need to be rearranged.
    bool bRearrangeTransforms = true;
    if (kInfo.m_eRotFlags == ALL_FLAGS &&
        kInfo.m_uiTransFlags == ALL_FLAGS &&
        kInfo.m_kAccumAxis == NiMatrix3::IDENTITY)
    {
        bRearrangeTransforms = false;
    }

    // Create keyframe controller for the non-accum node.
    NiTransformInterpolator* pkChildInterp = GetChildInterpolator(
        pkBaseInterp, pkSequence, pkChildNode, bRearrangeTransforms,
        bStoreControllers);

    // If transforms should not be rearranged, return.
    if (!bRearrangeTransforms)
    {
        return;
    }

    // Compute accum axis quaternions.
    NiMatrix3 kAccumAxisInv = kInfo.m_kAccumAxis.Inverse();
    NiQuaternion kQuatAccumAxis, kQuatAccumAxisInv;
    kQuatAccumAxis.FromRotation(kInfo.m_kAccumAxis);
    kQuatAccumAxisInv.FromRotation(kAccumAxisInv);

    // Handle rotation accumulation.
    if (kInfo.m_bSmoothRotation)
    {
        // Perform smooth rotation accumulation.
        unsigned int uiBaseNumRotKeys;
        NiRotKey::KeyType eBaseRotType;
        unsigned char ucBaseRotSize;
        NiRotKey* pkBaseRotKeys = pkBaseInterp->GetRotData(
            uiBaseNumRotKeys, eBaseRotType, ucBaseRotSize);

        // Smooth rotation is not supported for Euler rotation keys.
        if (eBaseRotType != NiRotKey::EULERKEY)
        {
            NiRotKey* pkLoRotKey = pkBaseRotKeys->GetKeyAt(0, ucBaseRotSize);
            NiRotKey* pkHiRotKey = pkBaseRotKeys->GetKeyAt(
                uiBaseNumRotKeys - 1, ucBaseRotSize);

            // Break down rotations into Euler angles, checking against
            // eRotFlags.
            float fLoXRot, fLoYRot, fLoZRot;
            float fHiXRot, fHiYRot, fHiZRot;
            NiMatrix3 kRotation;
            NiQuaternion kLoRotation, kHiRotation;
            switch (kInfo.m_eRotFlags)
            {
            case X_FLAG:
                pkLoRotKey->GetQuaternion().ToRotation(kRotation);
                kRotation.ToEulerAnglesXYZ(fLoXRot, fLoYRot, fLoZRot);
                kRotation.FromEulerAnglesXYZ(fLoXRot, 0.0f, 0.0f);
                kLoRotation.FromRotation(kRotation);
                pkHiRotKey->GetQuaternion().ToRotation(kRotation);
                kRotation.ToEulerAnglesXYZ(fHiXRot, fHiYRot, fHiZRot);
                kRotation.FromEulerAnglesXYZ(fHiXRot, 0.0f, 0.0f);
                kHiRotation.FromRotation(kRotation);
                break;
            case Y_FLAG:
                pkLoRotKey->GetQuaternion().ToRotation(kRotation);
                kRotation.ToEulerAnglesYXZ(fLoYRot, fLoXRot, fLoZRot);
                kRotation.FromEulerAnglesYXZ(fLoYRot, 0.0f, 0.0f);
                kLoRotation.FromRotation(kRotation);
                pkHiRotKey->GetQuaternion().ToRotation(kRotation);
                kRotation.ToEulerAnglesYXZ(fHiYRot, fHiXRot, fHiZRot);
                kRotation.FromEulerAnglesYXZ(fHiYRot, 0.0f, 0.0f);
                kHiRotation.FromRotation(kRotation);
                break;
            case Z_FLAG:
                pkLoRotKey->GetQuaternion().ToRotation(kRotation);
                kRotation.ToEulerAnglesZXY(fLoZRot, fLoXRot, fLoYRot);
                kRotation.FromEulerAnglesZXY(fLoZRot, 0.0f, 0.0f);
                kLoRotation.FromRotation(kRotation);
                pkHiRotKey->GetQuaternion().ToRotation(kRotation);
                kRotation.ToEulerAnglesZXY(fHiZRot, fHiXRot, fHiYRot);
                kRotation.FromEulerAnglesZXY(fHiZRot, 0.0f, 0.0f);
                kHiRotation.FromRotation(kRotation);
                break;
            case NO_FLAGS:
                kLoRotation = NiQuaternion::IDENTITY;
                kHiRotation = NiQuaternion::IDENTITY;
                break;
            default:    // ALL_FLAGS
                kLoRotation = pkLoRotKey->GetQuaternion();
                kHiRotation = pkHiRotKey->GetQuaternion();
                break;
            }

            // Create new rot keys at high and low time values.
            float fLoTime = pkLoRotKey->GetTime();
            float fHiTime = pkHiRotKey->GetTime();
            NiRotKey::ArrayFunction pfnArrayFunc = NiRotKey::GetArrayFunction(
                eBaseRotType);
            NIASSERT(pfnArrayFunc);
            NiRotKey* pkNewRotKeys = (NiRotKey*) pfnArrayFunc(2);
            pkNewRotKeys->GetKeyAt(0, ucBaseRotSize)->SetTime(fLoTime);
            pkNewRotKeys->GetKeyAt(0, ucBaseRotSize)->SetQuaternion(kLoRotation
                * kQuatAccumAxis);
            pkNewRotKeys->GetKeyAt(1, ucBaseRotSize)->SetTime(fHiTime);
            pkNewRotKeys->GetKeyAt(1, ucBaseRotSize)->SetQuaternion(kHiRotation
                * kQuatAccumAxis);

            // Apply any rotation not included in the base rot keys to the
            // child rot keys.
            unsigned int uiChildNumRotKeys;
            NiRotKey::KeyType eChildRotType;
            unsigned char ucChildRotSize;
            NiRotKey* pkChildRotKeys = pkChildInterp->GetRotData(
                uiChildNumRotKeys, eChildRotType, ucChildRotSize);
            NIASSERT(uiChildNumRotKeys == uiBaseNumRotKeys);
            unsigned int ui, uiLastRotIdx = 0;
            for (ui = 0; ui < uiBaseNumRotKeys; ui++)
            {
                NiRotKey* pkBaseRotKey = pkBaseRotKeys->GetKeyAt(ui, 
                    ucBaseRotSize);
                float fTime = pkBaseRotKey->GetTime();

                NiQuaternion kBaseQuat = pkBaseRotKey->GetQuaternion();
                NiMatrix3 kBaseRot;
                kBaseQuat.ToRotation(kBaseRot);

                NiMatrix3 kAccumRot, kNonAccumRot;
                FactorRot(kBaseRot, kInfo.m_eRotFlags, kAccumRot,
                    kNonAccumRot);
                NiQuaternion kNonAccumQuat;
                kNonAccumQuat.FromRotation(kNonAccumRot);

                NiRotKey* pkChildRotKey = pkChildRotKeys->GetKeyAt(ui, 
                    ucChildRotSize);
                NIASSERT(pkChildRotKey->GetTime() == fTime);
                pkChildRotKey->SetQuaternion(kQuatAccumAxisInv *
                    kNonAccumQuat * pkChildRotKey->GetQuaternion());
            }

            // Apply any rotation not included in the base rot key to the
            // child pos keys.
            unsigned int uiChildNumPosKeys;
            NiPosKey::KeyType eChildPosType;
            unsigned char ucChildPosSize;
            NiPosKey* pkChildPosKeys = pkChildInterp->GetPosData(
                uiChildNumPosKeys, eChildPosType, ucChildPosSize);
            uiLastRotIdx = 0;
            for (ui = 0; ui < uiChildNumPosKeys; ui++)
            {
                NiPosKey* pkChildPosKey = pkChildPosKeys->GetKeyAt(ui,
                    ucChildPosSize);
                float fTime = pkChildPosKey->GetTime();

                NiQuaternion kBaseQuat = NiRotKey::GenInterp(fTime,
                    pkBaseRotKeys, eBaseRotType, uiBaseNumRotKeys,
                    uiLastRotIdx, ucBaseRotSize);
                NiMatrix3 kBaseRot;
                kBaseQuat.ToRotation(kBaseRot);

                NiMatrix3 kAccumRot, kNonAccumRot;
                FactorRot(kBaseRot, kInfo.m_eRotFlags, kAccumRot,
                    kNonAccumRot);

                pkChildPosKey->SetPos(kAccumAxisInv * kNonAccumRot *
                    pkChildPosKey->GetPos());

                // Rotate NiBezPosKey tangents.
                if (eChildPosType == NiPosKey::BEZKEY)
                {
                    NiBezPosKey* pkChildBezPosKey = (NiBezPosKey*)
                        pkChildPosKey;
                    pkChildBezPosKey->SetInTan(kAccumAxisInv * kNonAccumRot *
                        pkChildBezPosKey->GetInTan());
                    pkChildBezPosKey->SetOutTan(kAccumAxisInv * kNonAccumRot *
                        pkChildBezPosKey->GetOutTan());
                }
            }

            // Replace base rotation keys with linear rot keys.
            pkBaseInterp->ReplaceRotData(pkNewRotKeys, 2,
                NiAnimationKey::LINKEY);
        }
    }
    else if (kInfo.m_eRotFlags != ALL_FLAGS ||
        kInfo.m_kAccumAxis != NiMatrix3::IDENTITY)
    {
        // Perform standard rotation accumulation.
        unsigned int uiBaseNumRotKeys;
        NiRotKey::KeyType eBaseRotType;
        unsigned char ucBaseRotSize;
        NiRotKey* pkBaseRotKeys = pkBaseInterp->GetRotData(uiBaseNumRotKeys,
            eBaseRotType, ucBaseRotSize);

        if (eBaseRotType == NiRotKey::EULERKEY)
        {
            // Only ALL_FLAGS and NO_FLAGS are supported for NiEulerRotKey
            // accumulation.
            if (kInfo.m_eRotFlags == NO_FLAGS)
            {
                // Copy the entire rot key array from the base controller to
                // the child controller.
                pkChildInterp->ReplaceRotData(pkBaseRotKeys, uiBaseNumRotKeys,
                    eBaseRotType);
                NiTransformData* pkBaseKFData = pkBaseInterp
                    ->GetTransformData();
                NIASSERT(pkBaseKFData);
                pkBaseKFData->SetRotAnim(NULL, 0, NiRotKey::NOINTERP);

                // Replace the base rot keys with a single identity
                // NiLinRotKey.
                NiLinRotKey* pkLinRotKeys = NiNew NiLinRotKey[1];
                pkLinRotKeys[0].SetTime(0.0f);
                pkLinRotKeys[0].SetQuaternion(NiQuaternion::IDENTITY);
                pkBaseInterp->ReplaceRotData(pkLinRotKeys, 1,
                    NiRotKey::LINKEY);
            }
        }
        else
        {
            // Apply non-accumulating rotations to child rot keys.
            unsigned int uiChildNumRotKeys;
            NiRotKey::KeyType eChildRotType;
            unsigned char ucChildRotSize;
            NiRotKey* pkChildRotKeys = pkChildInterp->GetRotData(
                uiChildNumRotKeys, eChildRotType, ucChildRotSize);
            NIASSERT(uiChildNumRotKeys == uiBaseNumRotKeys);
            unsigned int ui, uiLastRotIdx = 0;
            for (ui = 0; ui < uiBaseNumRotKeys; ui++)
            {
                NiRotKey* pkBaseRotKey = pkBaseRotKeys->GetKeyAt(ui, 
                    ucBaseRotSize);
                float fTime = pkBaseRotKey->GetTime();

                NiQuaternion kBaseQuat = pkBaseRotKey->GetQuaternion();
                NiMatrix3 kBaseRot;
                kBaseQuat.ToRotation(kBaseRot);

                NiMatrix3 kAccumRot, kNonAccumRot;
                FactorRot(kBaseRot, kInfo.m_eRotFlags, kAccumRot,
                    kNonAccumRot);
                NiQuaternion kNonAccumQuat;
                kNonAccumQuat.FromRotation(kNonAccumRot);

                NiRotKey* pkChildRotKey = pkChildRotKeys->GetKeyAt(ui, 
                    ucChildRotSize);
                NIASSERT(pkChildRotKey->GetTime() == fTime);
                pkChildRotKey->SetQuaternion(kQuatAccumAxisInv *
                    kNonAccumQuat * pkChildRotKey->GetQuaternion());
            }

            // Apply non-accumulating rotations to child pos keys.
            unsigned int uiChildNumPosKeys;
            NiPosKey::KeyType eChildPosType;
            unsigned char ucChildPosSize;
            NiPosKey* pkChildPosKeys = pkChildInterp->GetPosData(
                uiChildNumPosKeys, eChildPosType, ucChildPosSize);
            uiLastRotIdx = 0;
            for (ui = 0; ui < uiChildNumPosKeys; ui++)
            {
                NiPosKey* pkChildPosKey = pkChildPosKeys->GetKeyAt(ui, 
                    ucChildPosSize);
                float fTime = pkChildPosKey->GetTime();

                NiQuaternion kBaseQuat = NiRotKey::GenInterp(fTime,
                    pkBaseRotKeys, eBaseRotType, uiBaseNumRotKeys,
                    uiLastRotIdx, ucBaseRotSize);
                NiMatrix3 kBaseRot;
                kBaseQuat.ToRotation(kBaseRot);
            
                NiMatrix3 kAccumRot, kNonAccumRot;
                FactorRot(kBaseRot, kInfo.m_eRotFlags, kAccumRot,
                    kNonAccumRot);

                pkChildPosKey->SetPos(kAccumAxisInv * kNonAccumRot *
                    pkChildPosKey->GetPos());

                // Rotate NiBezPosKey tangents.
                if (eChildPosType == NiPosKey::BEZKEY)
                {
                    NiBezPosKey* pkChildBezPosKey = (NiBezPosKey*)
                        pkChildPosKey;
                    pkChildBezPosKey->SetInTan(kAccumAxisInv * kNonAccumRot *
                        pkChildBezPosKey->GetInTan());
                    pkChildBezPosKey->SetOutTan(kAccumAxisInv * kNonAccumRot *
                        pkChildBezPosKey->GetOutTan());
                }
            }

            // Remove non-accumulating rotations from base rot keys.
            for (ui = 0; ui < uiBaseNumRotKeys; ui++)
            {
                NiRotKey* pkBaseRotKey = pkBaseRotKeys->GetKeyAt(ui, 
                    ucBaseRotSize);

                NiQuaternion kBaseQuat = pkBaseRotKey->GetQuaternion();
                NiMatrix3 kBaseRot;
                kBaseQuat.ToRotation(kBaseRot);
            
                NiMatrix3 kAccumRot, kNonAccumRot;
                FactorRot(kBaseRot, kInfo.m_eRotFlags, kAccumRot,
                    kNonAccumRot);
                kBaseQuat.FromRotation(kAccumRot * kInfo.m_kAccumAxis);
                pkBaseRotKey->SetQuaternion(kBaseQuat);
            }
        }
    }

    // Fill derived values again, since we changed the base rot keys.
    FillDerivedValues(pkBaseInterp, pkBaseNode);

    // Handle translation accumulation.
    if (kInfo.m_uiTransFlags != ALL_FLAGS)
    {
        unsigned int uiBaseNumPosKeys;
        NiPosKey::KeyType eBasePosType;
        unsigned char ucBasePosSize;
        NiPosKey* pkBasePosKeys = pkBaseInterp->GetPosData(uiBaseNumPosKeys,
            eBasePosType, ucBasePosSize);

        unsigned int uiBaseNumRotKeys;
        NiRotKey::KeyType eBaseRotType;
        unsigned char ucBaseRotSize;
        NiRotKey* pkBaseRotKeys = pkBaseInterp->GetRotData(uiBaseNumRotKeys,
            eBaseRotType, ucBaseRotSize);
        NIASSERT(uiBaseNumRotKeys > 0);

        unsigned int uiBaseNumScaleKeys;
        NiFloatKey::KeyType eBaseScaleType;
        unsigned char ucBaseScaleSize;
        NiFloatKey* pkBaseScaleKeys = pkBaseInterp->GetScaleData(
            uiBaseNumScaleKeys, eBaseScaleType, ucBaseScaleSize);
        NIASSERT(uiBaseNumScaleKeys > 0);

        // Apply non-accumulating translations to child pos keys.
        unsigned int uiChildNumPosKeys;
        NiPosKey::KeyType eChildPosType;
        unsigned char ucChildPosSize;
        NiPosKey* pkChildPosKeys = pkChildInterp->GetPosData(uiChildNumPosKeys,
            eChildPosType, ucChildPosSize);
        NIASSERT(uiChildNumPosKeys == uiBaseNumPosKeys);
        NIASSERT(eChildPosType == eBasePosType);
        float fPrevTime = -NI_INFINITY;
        unsigned int ui, uiLastRotIdx = 0, uiLastPosIdx = 0,
            uiLastScaleIdx = 0;
        for (ui = 0; ui < uiBaseNumPosKeys; ui++)
        {
            NiPosKey* pkBasePosKey = pkBasePosKeys->GetKeyAt(
                ui, ucBasePosSize);
            float fTime = pkBasePosKey->GetTime();

            NiQuaternion kBaseQuat = NiRotKey::GenInterp(fTime,
                pkBaseRotKeys, eBaseRotType, uiBaseNumRotKeys,
                uiLastRotIdx, ucBaseRotSize);
            NiMatrix3 kBaseRot;
            kBaseQuat.ToRotation(kBaseRot);

            float fBaseScale = NiFloatKey::GenInterp(fTime,
                pkBaseScaleKeys, eBaseScaleType, uiBaseNumScaleKeys,
                uiLastScaleIdx, ucBaseScaleSize);

            NiMatrix3 kBaseScaleRotInv = kBaseRot.Transpose() * (1.0f /
                fBaseScale);

            NiPoint3 kBaseTrans = pkBasePosKey->GetPos();

            NiPoint3 kAccumTrans, kNonAccumTrans;
            FactorTrans(kBaseTrans, kInfo.m_uiTransFlags, kAccumTrans,
                kNonAccumTrans);

            NiPosKey* pkChildPosKey = pkChildPosKeys->GetKeyAt(ui, 
                ucChildPosSize);
            NIASSERT(pkChildPosKey->GetTime() == fTime);
            pkChildPosKey->SetPos(pkChildPosKey->GetPos() +
                kBaseScaleRotInv * kNonAccumTrans);

            // Handle NiBezPosKey tangents.
            if (eBasePosType == NiPosKey::BEZKEY)
            {
                NiBezPosKey* pkBaseBezPosKey = (NiBezPosKey*) pkBasePosKey;
                NiBezPosKey* pkChildBezPosKey = (NiBezPosKey*) pkChildPosKey;

                if (fPrevTime == -NI_INFINITY)
                {
                    pkChildBezPosKey->SetInTan(NiPoint3::ZERO);
                }
                else
                {
                    float fTimeDiff = fTime - fPrevTime;

                    NiPoint3 kAccumInTan, kNonAccumInTan;
                    FactorTrans(pkBaseBezPosKey->GetInTan(),
                        kInfo.m_uiTransFlags, kAccumInTan, kNonAccumInTan);

                    NiPoint3 kNewInTan = pkChildBezPosKey->GetInTan() +
                        kBaseScaleRotInv * kNonAccumInTan;
                    kNewInTan.Unitize();
                    pkChildBezPosKey->SetInTan(kNewInTan * fTimeDiff);
                }

                if (ui == uiBaseNumPosKeys - 1)
                {
                    pkChildBezPosKey->SetOutTan(NiPoint3::ZERO);
                }
                else
                {
                    float fTimeDiff = pkBasePosKeys->GetKeyAt(ui + 1, 
                        ucBasePosSize)->GetTime() - fTime;

                    NiPoint3 kAccumOutTan, kNonAccumOutTan;
                    FactorTrans(pkBaseBezPosKey->GetOutTan(),
                        kInfo.m_uiTransFlags, kAccumOutTan, kNonAccumOutTan);

                    NiPoint3 kNewOutTan = pkChildBezPosKey->GetOutTan() +
                        kBaseScaleRotInv * kNonAccumOutTan;
                    kNewOutTan.Unitize();
                    pkChildBezPosKey->SetOutTan(kNewOutTan * fTimeDiff);
                }
            }

            fPrevTime = fTime;
        }

        // Remove non-accumulating translation from base pos keys.
        fPrevTime = -NI_INFINITY;
        for (ui = 0; ui < uiBaseNumPosKeys; ui++)
        {
            NiPosKey* pkBasePosKey = pkBasePosKeys->GetKeyAt(ui, 
                ucBasePosSize);
            float fTime = pkBasePosKey->GetTime();
            NiPoint3 kBaseTrans = pkBasePosKey->GetPos();

            NiPoint3 kAccumTrans, kNonAccumTrans;
            FactorTrans(kBaseTrans, kInfo.m_uiTransFlags, kAccumTrans,
                kNonAccumTrans);

            pkBasePosKey->SetPos(kAccumTrans);

            // Handle NiBezPosKey tangents.
            if (eBasePosType == NiPosKey::BEZKEY)
            {
                NiBezPosKey* pkBaseBezPosKey = (NiBezPosKey*) pkBasePosKey;

                if (fPrevTime == -NI_INFINITY)
                {
                    pkBaseBezPosKey->SetInTan(NiPoint3::ZERO);
                }
                else
                {
                    float fTimeDiff = fTime - fPrevTime;

                    NiPoint3 kAccumInTan, kNonAccumInTan;
                    FactorTrans(pkBaseBezPosKey->GetInTan(),
                        kInfo.m_uiTransFlags, kAccumInTan, kNonAccumInTan);

                    kAccumInTan.Unitize();
                    pkBaseBezPosKey->SetInTan(kAccumInTan * fTimeDiff);
                }

                if (ui == uiBaseNumPosKeys - 1)
                {
                    pkBaseBezPosKey->SetOutTan(NiPoint3::ZERO);
                }
                else
                {
                    float fTimeDiff = pkBasePosKeys->GetKeyAt(ui + 1, 
                        ucBasePosSize)->GetTime() - fTime;

                    NiPoint3 kAccumOutTan, kNonAccumOutTan;
                    FactorTrans(pkBaseBezPosKey->GetOutTan(),
                        kInfo.m_uiTransFlags, kAccumOutTan, kNonAccumOutTan);

                    kAccumOutTan.Unitize();
                    pkBaseBezPosKey->SetOutTan(kAccumOutTan * fTimeDiff);
                }
            }

            fPrevTime = fTime;
        }
    }

    FillDerivedValues(pkBaseInterp, pkBaseNode);
    FillDerivedValues(pkChildInterp, pkChildNode);
    
    pkBaseInterp->Collapse();
    pkChildInterp->Collapse();
}
//---------------------------------------------------------------------------
NiNode* NiControllerExtractor::GetChildNode(NiNode* pkBaseNode)
{
    NIASSERT(pkBaseNode);

    // Get the name of the base node.
    const char* pcBaseNodeName = pkBaseNode->GetName();
    NIASSERT(pcBaseNodeName);

    // Check whether or not the child node already exists.
    NiNode* pkChildNode = NULL;
    if (pkBaseNode->GetChildCount() == 1)
    {
        for (unsigned int ui = 0; ui < pkBaseNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkBaseNode->GetAt(ui);
            if (pkChild && NiIsKindOf(NiNode, pkChild))
            {
                const char* pcChildName = pkChild->GetName();
                if (strstr(pcChildName, " NonAccum"))
                {
                    pkChildNode = (NiNode*) pkChild;
                    break;
                }
            }
        }
    }

    // If the child node does not exist, create it.
    if (!pkChildNode)
    {
        // Create the child node.
        char acChildNodeName[NI_MAX_PATH];
        NiSprintf(acChildNodeName, NI_MAX_PATH, "%s NonAccum", pcBaseNodeName);
        pkChildNode = NiNew NiNode;
        pkChildNode->SetName(acChildNodeName);
        pkChildNode->SetSelectiveUpdateTransformsOverride(
            pkBaseNode->GetSelectiveUpdateTransformsOverride());
        pkChildNode->SetSelectiveUpdate(pkBaseNode->GetSelectiveUpdate());
        pkChildNode->SetSelectiveUpdateTransforms(pkBaseNode
            ->GetSelectiveUpdateTransforms());
        pkChildNode->SetSelectiveUpdatePropertyControllers(pkBaseNode
            ->GetSelectiveUpdatePropertyControllers());
        pkChildNode->SetSelectiveUpdateRigid(pkBaseNode
            ->GetSelectiveUpdateRigid());

        // Propagate children from the base node to the child node, and
        // attach the child node as a child of the base node.
        for (unsigned ui = 0; ui < pkBaseNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkBaseNode->GetAt(ui);
            if (pkChild)
            {
                pkChildNode->AttachChild(pkChild);
            }
        }
        pkBaseNode->AttachChild(pkChildNode);
    }

    return pkChildNode;
}
//---------------------------------------------------------------------------
NiTransformInterpolator* NiControllerExtractor::GetBaseInterpolator(
    NiControllerSequence* pkSequence, NiNode* pkBaseNode, bool bLoop,
    bool bStoreControllers)
{
    NiTransformInterpolator* pkBaseInterp = NULL;

    // Search for base transform interpolator.
    const char* pcBaseNodeName = pkBaseNode->GetName();
    for (unsigned int ui = 0; ui < pkSequence->GetArraySize(); ui++)
    {
        char* pcObjectName = NULL;
        pkSequence->GetObjectNameAt(ui, pcObjectName);
        NiInterpolator* pkInterp = pkSequence->GetInterpolatorAt(ui);
        if (pcObjectName && strcmp(pcObjectName, pcBaseNodeName) == 0 &&
            pkInterp && NiIsKindOf(NiTransformInterpolator, pkInterp))
        {
            pkBaseInterp = (NiTransformInterpolator*) pkInterp;
            NiFree(pcObjectName);
            break;
        }
        NiFree(pcObjectName);
    }

    // If the base interpolator does not exist, create it.
    if (!pkBaseInterp)
    {
        NiTransformController* pkBaseCtlr = NiGetController(
            NiTransformController, pkBaseNode);
        if (!pkBaseCtlr)
        {
            pkBaseCtlr = NiNew NiTransformController;
            pkBaseCtlr->SetFrequency(pkSequence->GetFrequency());
            pkBaseCtlr->SetCycleType(pkSequence->GetCycleType());
            pkBaseCtlr->SetTarget(pkBaseNode);
        }

        NiInterpController* pkSeqCtlr = NULL;
        if (bStoreControllers)
        {
            NiTimeControllerPtr spNext = pkBaseCtlr->GetNext();
            pkBaseCtlr->SetNext(NULL);
            pkSeqCtlr = (NiInterpController*) pkBaseCtlr->Clone();
            pkBaseCtlr->SetNext(spNext);
            for (unsigned short us = 0;
                us < pkSeqCtlr->GetInterpolatorCount(); us++)
            {
                pkSeqCtlr->SetInterpolator(NULL, us);
            }
        }

        pkBaseInterp = NiNew NiTransformInterpolator;
        unsigned int uiNewIndex = pkSequence->AddInterpolator(pkBaseInterp,
            pcBaseNodeName, NULL, pkBaseCtlr->GetRTTI()->GetName(),
            pkBaseCtlr->GetCtlrID(), pkBaseCtlr->GetInterpolatorID());

        if (bStoreControllers)
        {
            pkSeqCtlr->SetInterpolator(pkBaseInterp);
            pkSequence->SetControllerAt(pkSeqCtlr, uiNewIndex);
            pkSeqCtlr->SetTarget(NULL);
        }
    }

    FillDerivedValues(pkBaseInterp, pkBaseNode);

    return pkBaseInterp;
}
//---------------------------------------------------------------------------
NiTransformInterpolator* NiControllerExtractor::GetChildInterpolator(
    NiTransformInterpolator* pkBaseInterp, NiControllerSequence* pkSequence,
    NiNode* pkChildNode, bool bCreateKeys, bool bStoreControllers)
{
    // Get the child node name.
    const char* pcChildNodeName = pkChildNode->GetName();
    NIASSERT(pcChildNodeName);

    // Get or create the child transform controller.
    NiTransformController* pkChildCtlr = NiGetController(
        NiTransformController, pkChildNode);
    if (!pkChildCtlr)
    {
        pkChildCtlr = NiNew NiTransformController;
        pkChildCtlr->SetFrequency(pkSequence->GetFrequency());
        pkChildCtlr->SetCycleType(pkSequence->GetCycleType());
        pkChildCtlr->SetTarget(pkChildNode);
    }
    
    NiInterpController* pkSeqCtlr = NULL;
    if (bStoreControllers)
    {
        NiTimeControllerPtr spNext = pkChildCtlr->GetNext();
        pkChildCtlr->SetNext(NULL);
        pkSeqCtlr = (NiInterpController*) pkChildCtlr->Clone();
        pkChildCtlr->SetNext(spNext);
        for (unsigned short us = 0; us < pkSeqCtlr->GetInterpolatorCount();
            us++)
        {
            pkSeqCtlr->SetInterpolator(NULL, us);
        }
    }

    // Create the child interpolator.
    NiTransformInterpolator* pkChildInterp = NiNew NiTransformInterpolator;
    unsigned int uiNewIndex = pkSequence->AddInterpolator(pkChildInterp,
        pcChildNodeName, NULL, pkChildCtlr->GetRTTI()->GetName(),
        pkChildCtlr->GetCtlrID(), pkChildCtlr->GetInterpolatorID());

    if (bStoreControllers)
    {
        pkSeqCtlr->SetInterpolator(pkChildInterp);
        pkSequence->SetControllerAt(pkSeqCtlr, uiNewIndex);
        pkSeqCtlr->SetTarget(NULL);
    }

    // If requested, create keys in the child KF controller for every key in
    // the base KF controller.
    if (bCreateKeys)
    {
        // Create rot keys.
        unsigned int uiNumRotKeys;
        NiRotKey::KeyType eRotType;
        unsigned char ucRotSize;
        NiRotKey* pkRotKeys = pkBaseInterp->GetRotData(uiNumRotKeys,
            eRotType, ucRotSize);
        if (eRotType != NiRotKey::NOINTERP && eRotType != NiRotKey::EULERKEY)
        {
            NiRotKey::InsertFunction pfnRotInsertFunc = NiRotKey::
                GetInsertFunction(eRotType);
            NIASSERT(pfnRotInsertFunc);
            unsigned int uiNumNewRotKeys = 0;
            NiRotKey* pkNewRotKeys = NULL;
            NiRotKey::SetDefault(NiQuaternion::IDENTITY);
            for (unsigned int ui = 0; ui < uiNumRotKeys; ui++)
            {
                pfnRotInsertFunc(pkRotKeys->GetKeyAt(ui,ucRotSize)->GetTime(),
                    (NiAnimationKey*&) pkNewRotKeys, uiNumNewRotKeys);
            }
            pkChildInterp->ReplaceRotData(pkNewRotKeys, uiNumNewRotKeys,
                eRotType);
        }

        // Create pos keys.
        unsigned int uiNumPosKeys;
        NiPosKey::KeyType ePosType;
        unsigned char ucPosSize;
        NiPosKey* pkPosKeys = pkBaseInterp->GetPosData(uiNumPosKeys,
            ePosType, ucPosSize);
        if (ePosType != NiPosKey::NOINTERP)
        {
            NiPosKey::InsertFunction pfnPosInsertFunc = NiPosKey::
                GetInsertFunction(ePosType);
            NIASSERT(pfnPosInsertFunc);
            unsigned int uiNumNewPosKeys = 0;
            NiPosKey* pkNewPosKeys = NULL;
            NiPosKey::SetDefault(NiPoint3::ZERO);
            for (unsigned int ui = 0; ui < uiNumPosKeys; ui++)
            {
                pfnPosInsertFunc(pkPosKeys->GetKeyAt(ui,ucPosSize)->GetTime(),
                    (NiAnimationKey*&) pkNewPosKeys, uiNumNewPosKeys);
            }
            pkChildInterp->ReplacePosData(pkNewPosKeys, uiNumNewPosKeys,
                ePosType);
        }

        // Create scale keys.
        unsigned int uiNumScaleKeys;
        NiFloatKey::KeyType eScaleType;
        unsigned char ucScaleSize;
        NiFloatKey* pkScaleKeys = pkBaseInterp->GetScaleData(uiNumScaleKeys,
            eScaleType, ucScaleSize);
        if (eScaleType != NiFloatKey::NOINTERP)
        {
            NiFloatKey::InsertFunction pfnScaleInsertFunc = NiFloatKey::
                GetInsertFunction(eScaleType);
            NIASSERT(pfnScaleInsertFunc);
            unsigned int uiNumNewScaleKeys = 0;
            NiFloatKey* pkNewScaleKeys = NULL;
            NiFloatKey::SetDefault(1.0f);
            for (unsigned int ui = 0; ui < uiNumScaleKeys; ui++)
            {
                pfnScaleInsertFunc(pkScaleKeys->GetKeyAt(ui,
                    ucScaleSize)->GetTime(),(NiAnimationKey*&) pkNewScaleKeys,
                    uiNumNewScaleKeys);
            }
            pkChildInterp->ReplaceScaleData(pkNewScaleKeys, uiNumNewScaleKeys,
                eScaleType);
        }
    }

    FillDerivedValues(pkChildInterp, pkChildNode);

    return pkChildInterp;
}
//---------------------------------------------------------------------------
void NiControllerExtractor::FillDerivedValues(
    NiTransformInterpolator* pkInterp, NiAVObject* pkObject)
{
    // Get pose value, if any.
    NiQuatTransform kPoseValue;
    pkInterp->Update(0.0f, NULL, kPoseValue);

    // Fill derived values for rot keys.
    unsigned int uiNumRotKeys = 0;
    NiRotKey::KeyType eRotType;
    unsigned char ucRotSize;
    NiRotKey* pkRotKeys = pkInterp->GetRotData(uiNumRotKeys, eRotType, 
        ucRotSize);
    if (uiNumRotKeys == 0)
    {
        // If no rot keys exist, create one.
        uiNumRotKeys = 1;
        eRotType = NiRotKey::LINKEY;
        ucRotSize = sizeof(NiLinRotKey);
        NiQuaternion kQuat;
        if (kPoseValue.IsRotateValid())
        {
            kQuat = kPoseValue.GetRotate();
        }
        else
        {
            pkObject->GetRotate(kQuat);
        }
        pkRotKeys = NiNew NiLinRotKey[uiNumRotKeys];
        pkRotKeys->GetKeyAt(0, ucRotSize)->SetTime(0.0f);
        pkRotKeys->GetKeyAt(0, ucRotSize)->SetQuaternion(kQuat);
        pkInterp->ReplaceRotData(pkRotKeys, uiNumRotKeys, eRotType);
    }
    NIASSERT(eRotType != NiRotKey::NOINTERP);
    NiRotKey::FillDerivedValsFunction pfnRotFillDerivedVals =
        NiRotKey::GetFillDerivedFunction(eRotType);
    NIASSERT(pfnRotFillDerivedVals);
    pfnRotFillDerivedVals(pkRotKeys, uiNumRotKeys, ucRotSize);

    // Fill derived values for pos keys.
    unsigned int uiNumPosKeys = 0;
    NiPosKey::KeyType ePosType;
    unsigned char ucPosSize;
    NiPosKey* pkPosKeys = pkInterp->GetPosData(uiNumPosKeys, ePosType,
        ucPosSize);
    if (uiNumPosKeys == 0)
    {
        // If no pos keys exist, create one.
        uiNumPosKeys = 1;
        ePosType = NiPosKey::LINKEY;
        ucPosSize = sizeof(NiLinPosKey);
        NiPoint3 kTrans;
        if (kPoseValue.IsTranslateValid())
        {
            kTrans = kPoseValue.GetTranslate();
        }
        else
        {
            kTrans = pkObject->GetTranslate();
        }
        pkPosKeys = NiNew NiLinPosKey[uiNumPosKeys];
        pkPosKeys->GetKeyAt(0, ucPosSize)->SetTime(0.0f);
        pkPosKeys->GetKeyAt(0, ucPosSize)->SetPos(kTrans);
        pkInterp->ReplacePosData(pkPosKeys, uiNumPosKeys, ePosType);
    }
    NIASSERT(ePosType != NiPosKey::NOINTERP);
    NiPosKey::FillDerivedValsFunction pfnPosFillDerivedVals =
        NiPosKey::GetFillDerivedFunction(ePosType);
    NIASSERT(pfnPosFillDerivedVals);
    pfnPosFillDerivedVals(pkPosKeys, uiNumPosKeys, ucPosSize);

    // Fill derived values for scale keys.
    unsigned int uiNumScaleKeys = 0;
    NiFloatKey::KeyType eScaleType;
    unsigned char ucScaleSize;
    NiFloatKey* pkScaleKeys = pkInterp->GetScaleData(uiNumScaleKeys,
        eScaleType, ucScaleSize);
    if (uiNumScaleKeys == 0)
    {
        // If no scale keys exist, create one.
        uiNumScaleKeys = 1;
        eScaleType = NiFloatKey::LINKEY;
        float fScale;
        if (kPoseValue.IsScaleValid())
        {
            fScale = kPoseValue.GetScale();
        }
        else
        {
            fScale = pkObject->GetScale();
        }
        ucScaleSize = sizeof(NiLinFloatKey);
        pkScaleKeys = NiNew NiLinFloatKey[uiNumScaleKeys];
        pkScaleKeys->GetKeyAt(0, ucScaleSize)->SetTime(0.0f);
        pkScaleKeys->GetKeyAt(0, ucScaleSize)->SetValue(fScale);
        pkInterp->ReplaceScaleData(pkScaleKeys, uiNumScaleKeys,
            eScaleType);
    }
    NIASSERT(eScaleType != NiFloatKey::NOINTERP);
    NiFloatKey::FillDerivedValsFunction pfnScaleFillDerivedVals =
        NiFloatKey::GetFillDerivedFunction(eScaleType);
    NIASSERT(pfnScaleFillDerivedVals);
    pfnScaleFillDerivedVals(pkScaleKeys, uiNumScaleKeys, ucScaleSize);
}
//---------------------------------------------------------------------------
void NiControllerExtractor::FactorRot(const NiMatrix3& kFullRot,
    AxisFlags eRotFlags, NiMatrix3& kAccumRot, NiMatrix3& kNonAccumRot)
{
    float fXAngle, fYAngle, fZAngle;
    switch (eRotFlags)
    {
    case X_FLAG:
        kFullRot.ToEulerAnglesXYZ(fXAngle, fYAngle, fZAngle);
        kAccumRot.FromEulerAnglesXYZ(fXAngle, 0.0f, 0.0f);
        kNonAccumRot.FromEulerAnglesXYZ(0.0f, fYAngle, fZAngle);
        break;
    case Y_FLAG:
        kFullRot.ToEulerAnglesYXZ(fYAngle, fXAngle, fZAngle);
        kAccumRot.FromEulerAnglesYXZ(fYAngle, 0.0f, 0.0f);
        kNonAccumRot.FromEulerAnglesYXZ(0.0f, fXAngle, fZAngle);
        break;
    case Z_FLAG:
        kFullRot.ToEulerAnglesZXY(fZAngle, fXAngle, fYAngle);
        kAccumRot.FromEulerAnglesZXY(fZAngle, 0.0f, 0.0f);
        kNonAccumRot.FromEulerAnglesZXY(0.0f, fXAngle, fYAngle);
        break;
    case NO_FLAGS:
        kAccumRot = NiMatrix3::IDENTITY;
        kNonAccumRot = kFullRot;
        break;
    default:
        kAccumRot = kFullRot;
        kNonAccumRot = NiMatrix3::IDENTITY;
        break;
    }
}
//---------------------------------------------------------------------------
void NiControllerExtractor::FactorTrans(const NiPoint3& kFullTrans,
    unsigned int uiTransFlags, NiPoint3& kAccumTrans,
    NiPoint3& kNonAccumTrans)
{
    kAccumTrans = NiPoint3::ZERO;
    kNonAccumTrans = kFullTrans;
    if (uiTransFlags & X_FLAG)
    {
        kAccumTrans.x = kFullTrans.x;
        kNonAccumTrans.x = 0.0f;
    }
    if (uiTransFlags & Y_FLAG)
    {
        kAccumTrans.y = kFullTrans.y;
        kNonAccumTrans.y = 0.0f;
    }
    if (uiTransFlags & Z_FLAG)
    {
        kAccumTrans.z = kFullTrans.z;
        kNonAccumTrans.z = 0.0f;
    }
}
//---------------------------------------------------------------------------
void NiControllerExtractor::ConsolidateTransformControllers(
    NiAVObject* pkSequenceRoot, const NiAVObject* pkAccumRoot)
{
    // This function replaces all NiTransformControllers in the scene graph
    // with an NiMultiTargetTransformController.

    unsigned short usCtlrCount = FindNumTransformControllers(
        pkSequenceRoot, pkAccumRoot);

    if (usCtlrCount == 0)
    {
        return;
    }

    NiMultiTargetTransformController* pkCtlr = NiGetController(
        NiMultiTargetTransformController, pkSequenceRoot);
    if (!pkCtlr)
    {
        pkCtlr = NiNew NiMultiTargetTransformController(usCtlrCount);
        pkCtlr->SetTarget(pkSequenceRoot);
    }
    else
    {
        NIASSERT(pkCtlr->GetArrayCount() >= usCtlrCount);
    }
    pkCtlr->SetManagerControlled(true);

    ReplaceTransformControllers(pkSequenceRoot, pkAccumRoot, pkCtlr);
}
//---------------------------------------------------------------------------
unsigned short NiControllerExtractor::FindNumTransformControllers(
    NiAVObject* pkObject, const NiAVObject* pkAccumRoot)
{
    unsigned short usCtlrCount = 0;

    NiTransformController* pkCtlr = NiGetController(NiTransformController,
        pkObject);
    if (pkCtlr != NULL && pkObject != pkAccumRoot)
    {
        usCtlrCount++;
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                usCtlrCount += FindNumTransformControllers(pkChild,
                    pkAccumRoot);
            }
        }
    }

    return usCtlrCount;
}
//---------------------------------------------------------------------------
void NiControllerExtractor::ReplaceTransformControllers(
    NiAVObject* pkObject, const NiAVObject* pkAccumRoot,
    NiMultiTargetTransformController* pkMultiCtlr)
{
    NiTransformController* pkCtlr = NiGetController(NiTransformController,
        pkObject);
    if (pkCtlr != NULL && pkObject != pkAccumRoot)
    {
        unsigned short usIndex = pkMultiCtlr->AddInterpolatorTarget(pkObject);
        NIASSERT(usIndex != NiMultiTargetTransformController::INVALID_INDEX);
        pkObject->RemoveController(pkCtlr);
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                ReplaceTransformControllers(pkChild, pkAccumRoot,
                    pkMultiCtlr);
            }
        }
    }
}
//---------------------------------------------------------------------------
