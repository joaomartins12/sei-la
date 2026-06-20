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
#include "NiEntityPCH.h"

#include "NiExternalAssetKFHandler.h"
#include "NiParamsKF.h"
#include "NiEntityErrorInterface.h"

#include <NiCloningProcess.h>

NiFixedString NiExternalAssetKFHandler::T_ASSETTYPE;
NiFixedString NiExternalAssetKFHandler::ERR_FILE_LOAD_FAILED;

//---------------------------------------------------------------------------
NiExternalAssetKFHandler::NiExternalAssetKFHandler(bool bCloneFromPristine) :
    m_bCloneFromPristine(bCloneFromPristine) 
{
}
//---------------------------------------------------------------------------
NiExternalAssetKFHandler::~NiExternalAssetKFHandler()
{
    UnloadAll();
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFHandler::GetCloneFromPristine(
    NiExternalAssetParams* pkParams)
{
    return m_bCloneFromPristine;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFHandler::SetCloneFromPristine(bool bCloneFromPristine,
    NiExternalAssetParams* pkParams)
{
    // This function is only valid when no assets have already been loaded.
    if (m_kCtrlSeqArrayMap.GetCount() == 0)
    {
        m_bCloneFromPristine = bCloneFromPristine;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFHandler::CanHandleParams(
    NiExternalAssetParams* pkParams)
{
    if (!pkParams)
        return false;

    if (NiIsKindOf(NiParamsKF, pkParams))
        return true;
    else 
        return false;
}
//---------------------------------------------------------------------------
NiFixedString NiExternalAssetKFHandler::GetAssetType()
{
    return T_ASSETTYPE;
}
//---------------------------------------------------------------------------
unsigned int NiExternalAssetKFHandler::GetNumAssetsRegistered(
    NiExternalAssetParams* pkParams)
{
    return m_kCtrlSeqArrayMap.GetCount();
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFHandler::Register(NiExternalAssetParams* pkParams)
{
    if (!CanHandleParams(pkParams))
        return false;

    const char* pcAssetName = pkParams->GetAssetPath();

    NiTObjectArray<NiControllerSequencePtr>* pkCtrlSeqArray = NULL;
    if (m_kCtrlSeqArrayMap.GetAt(pcAssetName, pkCtrlSeqArray))
    {
        // Already registered
        return true;
    }

    m_kCtrlSeqArrayMap.SetAt(pcAssetName, NULL);
    return true;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFHandler::LoadAll(
    NiExternalAssetParams* pkParams, NiEntityErrorInterface* pkErrorHandler, 
    NiExternalAssetManager* pkAssetManager)
{
    if (pkParams)
    {
        // This function doesn't make use of pkParams, except for early
        // exiting if a param type was passed in that it can't handle.
        if (!CanHandleParams(pkParams))
            return false;
    }

    bool bSuccess = true; 

    NiTMapIterator kIter = m_kCtrlSeqArrayMap.GetFirstPos();
    while(kIter)
    {
        const char* pcAssetName;
        NiTObjectArray<NiControllerSequencePtr>* pkCtrlSeqPtrArray;
        m_kCtrlSeqArrayMap.GetNext(kIter, pcAssetName, pkCtrlSeqPtrArray);

        if (pkCtrlSeqPtrArray)
        {
            // Already loaded
            continue;
        }

        if (!LoadKFFile(pcAssetName, pkErrorHandler, pkAssetManager))
            bSuccess = false;
    }

    return bSuccess;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFHandler::Load(NiExternalAssetParams* pkParams,
    NiEntityErrorInterface* pkErrorHandler,
    NiExternalAssetManager* pkAssetManager)
{
    if (!CanHandleParams(pkParams))
        return false;

    const char* pcAssetName = pkParams->GetAssetPath();

    NiTObjectArray<NiControllerSequencePtr>* pkCtrlSeqArray = NULL;
    m_kCtrlSeqArrayMap.GetAt(pcAssetName, pkCtrlSeqArray);
    if (pkCtrlSeqArray)
    {
        return true; // Already loaded
    }

    return LoadKFFile(pcAssetName, pkErrorHandler, pkAssetManager);
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFHandler::LoadKFFile(const char* pcAssetName,
    NiEntityErrorInterface* pkErrorHandler,
    NiExternalAssetManager* pkAssetManager)
{
    NiTObjectArray<NiControllerSequencePtr> kSequences;
    NiBool bSuccess = NiControllerSequence::CreateAllSequencesFromFile(
        pcAssetName, kSequences);

    if (!bSuccess)
    {
        if (pkErrorHandler)
        {
            pkErrorHandler->ReportError(ERR_FILE_LOAD_FAILED, pcAssetName,
                "","");
        }
        return false;
    }

    // Store kSequences...
    NiTObjectArray<NiControllerSequencePtr>* pkCtrlSeqArray = 
        NiNew NiTObjectArray<NiControllerSequencePtr>;
    pkCtrlSeqArray->SetSize(kSequences.GetSize());
    for(unsigned int ui=0; ui<kSequences.GetSize(); ui++)
    {
        pkCtrlSeqArray->SetAt(ui, kSequences.GetAt(ui));
    }

    m_kCtrlSeqArrayMap.SetAt(pcAssetName, pkCtrlSeqArray);

    // Create clonearray for this asset
    NiTObjectSet<NiControllerSequencePtr>* pkSet = NULL;
    NIASSERT(m_kMapToCloneSet.GetAt(pcAssetName, pkSet) == false);
    pkSet = NiNew NiTObjectSet<NiControllerSequencePtr>;
    m_kMapToCloneSet.SetAt(pcAssetName, pkSet);

    return true;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFHandler::Retrieve(NiExternalAssetParams* pkParams) 
{
    if (!CanHandleParams(pkParams))
        return false;

    NiParamsKF* pkKFParams = (NiParamsKF*)pkParams;

    const char* pcAssetName = pkParams->GetAssetPath();

    // Retrieve the controller array
    NiTObjectArray<NiControllerSequencePtr>* pkCtrlSeqArray = NULL;
    if (m_kCtrlSeqArrayMap.GetAt(pcAssetName, pkCtrlSeqArray))
    {
        if (!pkCtrlSeqArray)
        {
            return false;
        }
    }

    unsigned int uiAnimIndex = pkKFParams->GetAnimIndex();
    if (uiAnimIndex >= pkCtrlSeqArray->GetSize())
    {
        NIASSERT(!"AnimIndex is out of range!");
        return false;
    }

    bool bNeedToClone = false;
    if (GetCloneFromPristine())
    {
        // Always clone if pristine is set to true
        bNeedToClone = true;
    }
    else 
    {
        NiControllerSequencePtr spCtrlPtr = (NiControllerSequence*)
            pkCtrlSeqArray->GetAt(uiAnimIndex);

        if (spCtrlPtr->GetRefCount() > 2)
        {
            // If the controller sequence has an external reference (ie, if
            // the ref count is greater than 2: one for our local ref and
            // one for our stored ref), then cloning is needed. This is the
            // case where CloneFromPristine is false and a clone exists from
            // the original.
            bNeedToClone = true;
        }
    }

    if (bNeedToClone)
    {
        NiCloningProcess kCloning;
        kCloning.m_eCopyType = NiObjectNET::COPY_EXACT;
        NiControllerSequencePtr spCtrlPtr = (NiControllerSequence*)
            pkCtrlSeqArray->GetAt(uiAnimIndex)->Clone(kCloning);
        pkKFParams->SetControllerSequence(spCtrlPtr);

        // The clone is recorded to m_kMapToCloneSet
        NiTObjectSet<NiControllerSequencePtr>* pkSet = NULL;
        NIVERIFY(m_kMapToCloneSet.GetAt(pcAssetName, pkSet));
        NIASSERT(pkSet);
        pkSet->AddUnique(spCtrlPtr);
    }
    else
    {
        pkKFParams->SetControllerSequence(pkCtrlSeqArray->GetAt(uiAnimIndex));
    }

    return true;
}
//---------------------------------------------------------------------------
void NiExternalAssetKFHandler::Unload(NiExternalAssetParams* pkParams,
    NiExternalAssetManager* pkAssetManager)
{
    if (!CanHandleParams(pkParams))
        return;

    const char* pcAssetName = pkParams->GetAssetPath();

    Unload(pcAssetName);
}
//---------------------------------------------------------------------------
void NiExternalAssetKFHandler::UnloadAll(NiExternalAssetParams* pkParams,
    NiExternalAssetManager* pkAssetManager)
{
    if (pkParams)
    {
        // This function doesn't make use of pkParams, except for early
        // exiting if a param type was passed in that it can't handle.
        if (!CanHandleParams(pkParams))
            return;
    }

    // Remove all the controller sequences from each array before 
    // removing all the arrays.
    NiTMapIterator kIter = m_kCtrlSeqArrayMap.GetFirstPos();
    while(kIter)
    {
        const char* pcAsset;
        NiTObjectArray<NiControllerSequencePtr>* pkCtrlSeqArray = NULL;
        m_kCtrlSeqArrayMap.GetNext(kIter, pcAsset, pkCtrlSeqArray);

        if (pkCtrlSeqArray)
            pkCtrlSeqArray->RemoveAll();

        NiDelete(pkCtrlSeqArray);
    }
    m_kCtrlSeqArrayMap.RemoveAll();

    kIter = m_kMapToCloneSet.GetFirstPos();
    while(kIter)
    {
        NiTObjectSet<NiControllerSequencePtr>* pkSet = NULL;
        const char* pcAssetName;

        m_kMapToCloneSet.GetNext(kIter, pcAssetName, pkSet);

        pkSet->RemoveAll();
        NiDelete pkSet;
    }
    m_kMapToCloneSet.RemoveAll();
}
//---------------------------------------------------------------------------
void NiExternalAssetKFHandler::Unload(const char* pcAssetName)
{
    if (pcAssetName)
    {
        NiTObjectArray<NiControllerSequencePtr>* pkCtrlSeqArray = NULL;
        bool bSuccess = m_kCtrlSeqArrayMap.GetAt(pcAssetName, pkCtrlSeqArray);

        // Remove all the controller sequences before removing the array
        if (bSuccess && pkCtrlSeqArray)
            pkCtrlSeqArray->RemoveAll();

        m_kCtrlSeqArrayMap.RemoveAt(pcAssetName);
    }

    NiTObjectSet<NiControllerSequencePtr>* pkSet = NULL;
    bool bSuccess = m_kMapToCloneSet.GetAt(pcAssetName, pkSet);
    if (bSuccess && pkSet)
    {
        NIVERIFY(m_kMapToCloneSet.RemoveAt(pcAssetName));

        pkSet->RemoveAll();
        NiDelete pkSet;
    }
}
//---------------------------------------------------------------------------
void NiExternalAssetKFHandler::UnloadAllUnusedAssets(
    NiExternalAssetParams* pkParams, NiExternalAssetManager* pkAssetManager)
{
    if (pkParams)
    {
        // This function doesn't make use of pkParams, except for early
        // exiting if a param type was passed in that it can't handle.
        if (!CanHandleParams(pkParams))
            return;
    }

    // Iterate through the clone map
    // For all clones that have only 1 ref count, unload this asset and
    // release all smartpointer references.

    NiTMapIterator kIter = m_kMapToCloneSet.GetFirstPos();

    while(kIter)
    {
        NiTObjectSet<NiControllerSequencePtr>* pkSet = NULL;

        const char* pcAssetName;
        m_kMapToCloneSet.GetNext(kIter, pcAssetName, pkSet);

        unsigned int uiSetSize = pkSet->GetSize();
        if (uiSetSize > 0)
        {
            for(int i=uiSetSize-1; i>=0; i--)
            {
                NiControllerSequence* pkSeq = pkSet->GetAt(i);

                if (pkSeq->GetRefCount() > 1)
                    continue;

                // Remove the clone
                pkSet->RemoveAt(i);
            }
            uiSetSize = pkSet->GetSize();
        }

        if (uiSetSize == 0)
        {
            // If the size is 0 and clone from pristine is true, then the
            // asset is not being used. However, if clone from pristine is
            // false, then it is possible that there are no clones, yet the
            // original asset is being externally used. This is checked for
            // by analyzing the ref count.

            if (GetCloneFromPristine())
            {
                Unload(pcAssetName);
            }
            else
            {
                // May have a clone from the non-pristine source.
                NiTObjectArray<NiControllerSequencePtr>* spDirtySeqArray =
                    NULL;

                bool bSuccess = m_kCtrlSeqArrayMap.GetAt(pcAssetName,
                    spDirtySeqArray);

                if (bSuccess && spDirtySeqArray)
                {
                    // Check if the only references that are stored are the
                    // ones this class is storing. If there are any external
                    // references, we mark the asset as "dirty" and keep 
                    // the entire array. Otherwise, we unload it.
                    // *Every* element must have a ref count of one in order
                    // to unload it.
                    bool bDirty = false;
                    unsigned int uiArraySize = spDirtySeqArray->GetSize();
                    for(unsigned int ui=0; ui<uiArraySize; ui++)
                    {
                        NiControllerSequence* pkSeq = 
                            spDirtySeqArray->GetAt(ui);

                        if (pkSeq->GetRefCount() != 1)
                        {
                            bDirty = true;
                            break;
                        }
                    }

                    if (!bDirty)
                    {
                        Unload(pcAssetName);                           
                    }
                }
            }
        }
    }

    return;
}
//---------------------------------------------------------------------------
void NiExternalAssetKFHandler::_SDMInit()
{
    T_ASSETTYPE = "KF";
    ERR_FILE_LOAD_FAILED = "KF file load failed.";
}
//---------------------------------------------------------------------------
void NiExternalAssetKFHandler::_SDMShutdown()
{
    T_ASSETTYPE = NULL;
    ERR_FILE_LOAD_FAILED = NULL;
}
//---------------------------------------------------------------------------
