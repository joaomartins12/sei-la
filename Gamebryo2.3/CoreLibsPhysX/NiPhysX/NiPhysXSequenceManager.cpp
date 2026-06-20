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

#include "NiPhysXPCH.h"

#include "NiPhysXSequenceManager.h"
#include "NiPhysXAccumRigidBodyDest.h"
#include "NiPhysXAccumInterpolatorDest.h"
#include "NiPhysXScene.h"

NiImplementRTTI(NiPhysXSequenceManager, NiObject);

//---------------------------------------------------------------------------
NiPhysXSequenceManager::NiPhysXSequenceManager()
{
    m_spProp = 0;
}
//---------------------------------------------------------------------------
NiPhysXSequenceManager::~NiPhysXSequenceManager()
{
    CleanMaps();
    m_spProp = 0;
}
//---------------------------------------------------------------------------
bool NiPhysXSequenceManager::SetManager(NiControllerManager* pkCM,
    NiPhysXProp* pkProp)
{
    // Check for existing controller
    if (m_spProp)
        return false;
    
    m_spProp = pkProp;
    
    // Find all the objects controlled by this prop.
    FindTransformDests(pkProp);

    // Iterate over the sequences in the controller.
    unsigned int uiSeqCount = pkCM->GetSequenceCount();
    for (unsigned int ui = 0; ui < uiSeqCount; ui++)
    {
        NiControllerSequence* pkSeq = pkCM->GetSequenceAt(ui);
        if (!pkSeq)
            continue;
        
        if (!AddSequence(pkSeq))
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXSequenceManager::FindTransformDests(NiPhysXProp* pkProp)
{
    // Iterate over all the destinations in the PhysX prop, looking for
    // interpolators that are driving transforms.
    unsigned int uiNumDests = pkProp->GetDestinationsCount();
    for (unsigned int ui = 0; ui < uiNumDests; ui++)
    {
        NiPhysXDest* pkDest = pkProp->GetDestinationAt(ui);
            
        // Get the target object
        NiAVObject* pkTarget = 0;
        NiPhysXTransformDest* pkTransformDest = 0;
        if (NiIsKindOf(NiPhysXTransformDest, pkDest))
        {
            pkTransformDest = (NiPhysXTransformDest*)pkDest;
            pkTarget = pkTransformDest->GetTarget();
        }
        else
        {
            continue;
        }
        NIASSERT(pkTarget && pkTransformDest);
        
        // Stick it in the map
        SequenceActorData* pkData = NiNew SequenceActorData;
        pkData->m_spDestination = pkTransformDest;
        m_kOriginalDestMap.SetAt(pkTarget, pkData);
    }
}
//---------------------------------------------------------------------------
bool NiPhysXSequenceManager::AddSequence(NiControllerSequence* pkSeq)
{
    // Iterate through all the interpolators in the sequence
    // and try to find matching physics controlled object
    unsigned int uiNumInterps = pkSeq->GetArraySize();
    for (unsigned int ui = 0; ui < uiNumInterps; ui++)
    {
        NiInterpolator* pkInterp = pkSeq->GetInterpolatorAt(ui);
        
        // We're only interested in transform controllers.
        if (!NiIsKindOf(NiTransformInterpolator, pkInterp) &&
            !NiIsKindOf(NiBSplineTransformInterpolator, pkInterp))
            continue;
            
        // Get the controller. It might be a NiMultiObjectController.
        NiInterpController* pkCtrl = pkSeq->GetControllerAt(ui);
                    
        // Get the target
        NiAVObject* pkTarget;
        if (NiIsKindOf(NiMultiTargetTransformController, pkCtrl))
        {
            NiMultiTargetTransformController* pkMOCtrl =
                (NiMultiTargetTransformController*)pkCtrl;
            
            // Get the blend interpolator for this interpolator
            NiBlendInterpolator* pkBlendInterp =
                pkSeq->GetBlendInterpolatorAt(ui);
            NIASSERT(pkBlendInterp != 0);
            
            // Look for this interpolator
            unsigned short usCount = pkMOCtrl->GetInterpolatorCount();
            unsigned short usIndex = 0;
            while (usIndex < usCount &&
                pkBlendInterp != pkMOCtrl->GetInterpolator(usIndex))
            {
                usIndex++;
            }
            NIASSERT(usIndex < usCount);
                
            // Map it to the controlled object
            pkTarget = pkMOCtrl->GetInterpolatorTarget(usIndex);
        }
        else
        {
            NiObjectNET* pkObj = pkCtrl->GetTarget();
            pkTarget = NiDynamicCast(NiAVObject, pkObj);
        }

        // Look for physics control
        SequenceActorData* pkActorData = 0;
        if (m_kOriginalDestMap.GetAt(pkTarget, pkActorData) &&
            !pkActorData->m_spSource)
        {
            // If the target is the accum root and has a non-accum child,
            // then we need to attach the source to the non-accum child.
            if (pkTarget == pkSeq->GetAccumRoot())
            {
                // Find the NonAccum node            
                NiNode* pkTargetNode = NiDynamicCast(NiNode, pkTarget);
                NIASSERT(pkTargetNode->GetChildCount() == 1);
                unsigned int uiIndex = 0;
                NiAVObject* pkChild = 0;
                do
                {
                    pkChild = pkTargetNode->GetAt(uiIndex++);
                }
                while (!pkChild);
                const char* pcName = pkChild->GetName();
                if (strstr(pcName, "NonAccum"))
                {
                    pkTarget = pkChild;
                }
            }

            // No source, make one
            pkActorData->m_spSource = NiNew NiPhysXDynamicSrc(pkTarget,
                pkActorData->m_spDestination->GetActor());
            m_spProp->AddSource(pkActorData->m_spSource);
            pkActorData->m_spSource->SetActive(true);
            pkActorData->m_spSource->SetInterpolate(true);
            
            // Also set the actor to be kinematic, for now, and
            // put the actor in the map of physics active counts
            NxActor* pkActor = pkActorData->m_spDestination->GetActor();
            NIASSERT(pkActor);
            pkActor->raiseBodyFlag(NX_BF_KINEMATIC);
            m_kActorActiveCount.SetAt(pkActor, 0);
        
            // Deactivate the transform destination and remove it from the
            // prop (and scene)
            m_spProp->DeleteDestination(pkActorData->m_spDestination);
            pkActorData->m_spDestination->SetActive(false);
        }
    }
    
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXSequenceManager::RemoveSequence(NiControllerSequence* pkSeq)
{
    pkSeq->RemoveActivationCallbackObject(this);

    SequenceData* pkSeqData;
    if (m_kSequences.GetAt(pkSeq, pkSeqData))
    {
        SetActivity(pkSeqData, false);
        unsigned int uiCount = pkSeqData->m_kActorData.GetSize();
        for (unsigned int ui = 0; ui < uiCount; ui++)
        {
            SequenceActorData* pkActorData = pkSeqData->m_kActorData.GetAt(ui);
            m_spProp->DeleteDestination(pkActorData->m_spDestination);
        }

        m_kSequences.RemoveAt(pkSeq);
        NiDelete pkSeqData;
    }
    
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXSequenceManager::MakeSequencePhysical(NiControllerSequence* pkSeq)
{
    // Look for the accumulation root and the non-accum node
    NiAVObject* pkAccumRoot = pkSeq->GetAccumRoot();
    NiAVObject* pkNonAccumNode = 0;
    if (pkAccumRoot)
    {
        // Find the NonAccum node            
        NiNode* pkTargetNode = NiDynamicCast(NiNode, pkAccumRoot);
        NIASSERT(pkTargetNode->GetChildCount() == 1);
        unsigned int uiIndex = 0;
        NiAVObject* pkChild = 0;
        do
        {
            pkChild = pkTargetNode->GetAt(uiIndex++);
        }
        while (!pkChild);
        const char* pcName = pkChild->GetName();
        if (strstr(pcName, "NonAccum"))
        {
            pkNonAccumNode = pkChild;
        }
        else
        {
            pkAccumRoot = 0;
        }
    }
    
    // We will need to find both interpolators for accum and non-accum
    NiInterpolator* pkAccumInterp = 0;
    NiInterpolator* pkNonAccumInterp = 0;
    NiBlendInterpolator* pkAccumBlendInterp = 0;

    // Create the data for this sequence
    SequenceData* pkSequenceData = NiNew SequenceData;

    // Iterate through all the interpolators in the sequence
    // and try to find matching physics controlled object
    unsigned int uiNumInterps = pkSeq->GetArraySize();
    for (unsigned int ui = 0; ui < uiNumInterps; ui++)
    {
        NiInterpolator* pkInterp = pkSeq->GetInterpolatorAt(ui);
        
        // We're only interested in transform controllers.
        if (!NiIsKindOf(NiTransformInterpolator, pkInterp) &&
            !NiIsKindOf(NiBSplineTransformInterpolator, pkInterp))
            continue;
            
        // Get the controller. It might be a NiMultiObjectController.
        NiInterpController* pkCtrl = pkSeq->GetControllerAt(ui);
        NiBlendInterpolator* pkBlendInterp = pkSeq->GetBlendInterpolatorAt(ui);
                    
        // Get the target
        NiAVObject* pkTarget;
        if (NiIsKindOf(NiMultiTargetTransformController, pkCtrl))
        {
            NiMultiTargetTransformController* pkMOCtrl =
                (NiMultiTargetTransformController*)pkCtrl;

            NIASSERT(pkBlendInterp != 0);
            
            // Look for this interpolator
            unsigned short usCount = pkMOCtrl->GetInterpolatorCount();
            unsigned short usIndex = 0;
            while (usIndex < usCount &&
                pkBlendInterp != pkMOCtrl->GetInterpolator(usIndex))
            {
                usIndex++;
            }
            NIASSERT(usIndex < usCount);
                
            // Map it to the controlled object
            pkTarget = pkMOCtrl->GetInterpolatorTarget(usIndex);
        }
        else
        {
            NiObjectNET* pkObj = pkCtrl->GetTarget();
            pkTarget = NiDynamicCast(NiAVObject, pkObj);
        }
        
        // Do not process accmulation related nodes until the end
        if (pkTarget == pkAccumRoot)
        {
            pkAccumInterp = pkInterp;
            pkAccumBlendInterp = pkBlendInterp;
            continue;
        }
        if (pkTarget == pkNonAccumNode)
        {
            pkNonAccumInterp = pkInterp;
            continue;
        }
        
        // Look for physics control
        SequenceActorData *pkOriginalActorData = 0;
        if (m_kOriginalDestMap.GetAt(pkTarget, pkOriginalActorData))
        {
            NiPhysXTransformDest* pkOriginalDest = NiDynamicCast(
                NiPhysXTransformDest, pkOriginalActorData->m_spDestination);

            // Create a new interpolator destination using actor
            // information from the old destination.
            NiPhysXRigidBodyDestPtr spNewDest = NiNew NiPhysXInterpolatorDest(
                pkInterp, pkTarget->GetParent(), pkOriginalDest->GetActor(),
                pkOriginalDest->GetActorParent());

            // Disable the old destination and add the new one. They are
            // inactive right now.
            spNewDest->SetActive(false);
            m_spProp->AddDestination(spNewDest);
            
            // Add this source to the set for this sequence.
            SequenceActorData* pkNewActorData = NiNew SequenceActorData;
            pkNewActorData->m_spSource = pkOriginalActorData->m_spSource;
            pkNewActorData->m_spDestination = spNewDest;
            pkSequenceData->m_kActorData.Add(pkNewActorData);
        }
    }
    
    // Deal with accumulation nodes
    if (pkAccumInterp && pkNonAccumInterp)
    {
        // Look for physics control
        SequenceActorData *pkOriginalActorData = 0;
        if (m_kOriginalDestMap.GetAt(pkAccumRoot, pkOriginalActorData))
        {
            NiPhysXTransformDest* pkOriginalDest = NiDynamicCast(
                NiPhysXTransformDest, pkOriginalActorData->m_spDestination);

            // What we do now depends on whether we have an accum interpolator
            // on the accum root
            NiPhysXRigidBodyDest* pkNewDest = 0;
            if (NiIsKindOf(NiBlendAccumTransformInterpolator,
                pkAccumBlendInterp))
            {
                NiPhysXAccumRigidBodyDest* pkNewAccumDest =
                    NiNew NiPhysXAccumInterpolatorDest(pkAccumInterp,
                    pkNonAccumInterp,
                    (NiBlendAccumTransformInterpolator*)pkAccumBlendInterp,
                    pkAccumRoot->GetParent(),
                    pkOriginalDest->GetActor(),
                    pkOriginalDest->GetActorParent());
                    
                // Set all the accum flags
                bool bX, bY, bZ;
                pkSeq->GetAccumTranslation(bX, bY, bZ);
                pkNewAccumDest->SetAccumTranslation(bX, bY, bZ);
                pkSeq->GetAccumRotation(bX, bY, bZ);
                pkNewAccumDest->SetAccumRotation(bX, bY, bZ);
                unsigned char ucAxis;
                bool bNeg;
                pkSeq->GetAccumFrontFacing(ucAxis, bNeg);
                pkNewAccumDest->SetAccumFrontFacing(ucAxis, bNeg);
                
                pkNewDest = pkNewAccumDest;
            }
            else
            {
                NIASSERT(NiIsKindOf(NiNode, pkAccumRoot));
                pkNewDest = NiNew NiPhysXInterpolatorDest(pkNonAccumInterp,
                    (NiNode*)pkAccumRoot, pkOriginalDest->GetActor(),
                    pkOriginalDest->GetActorParent());
            }        

            // Disable the old destination and add the new one. They are
            // inactive right now.
            pkNewDest->SetActive(false);
            pkNewDest->SetInterpolate(true);
            m_spProp->AddDestination(pkNewDest);

            SequenceActorData* pkNewActorData = NiNew SequenceActorData;
            pkNewActorData->m_spSource = pkOriginalActorData->m_spSource;
            pkNewActorData->m_spDestination = pkNewDest;
            pkSequenceData->m_kActorData.Add(pkNewActorData);
        }
    }
    else if (pkAccumInterp || pkNonAccumInterp)
    {
        NIASSERT(false &&
            "NiPhysXSequenceManager::MakeSequencePhysical\n"
            "Found only one of NonAccum interp and Accum interp\n");
    }

    // Add to physics sequences
    pkSequenceData->m_bIsActive = false;

    // Handle the activity status
    NiControllerSequence::AnimState eState = pkSeq->GetState();
    if (eState == NiControllerSequence::TRANSDEST ||
        eState == NiControllerSequence::ANIMATING)
    {
        SetActivity(pkSequenceData, true);
    }

    pkSeq->AddActivationCallbackObject(this);
    
    m_kSequences.SetAt(pkSeq, pkSequenceData);

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXSequenceManager::ActivationChanged(NiControllerSequence* pkSeq,
    NiControllerSequence::AnimState eState)
{
    // Find the sequence
    SequenceData* pkSeqData;
    NIVERIFY(m_kSequences.GetAt(pkSeq, pkSeqData));

    if (eState == NiControllerSequence::INACTIVE ||
        eState == NiControllerSequence::TRANSSOURCE)
    {
        // Inactive physics sequence. If we're the source of a transition
        // we are inactive because transitions out of physics are
        // intended to return to animation and may be long. We do not want
        // the physics to diverge from the animation while the transition
        // is going on. Also, blend transitions by definition should blend
        // from a fixed frame.
        SetActivity(pkSeqData, false);
    }
    else if (eState == NiControllerSequence::TRANSDEST ||
        eState == NiControllerSequence::ANIMATING)
    {
        // If we are the destination of a transition then we always
        // play physics.
        SetActivity(pkSeqData, true);
    }
}
//---------------------------------------------------------------------------
bool NiPhysXSequenceManager::SetActivity(SequenceData* pkSeqData, bool bActive)
{
    if (pkSeqData->m_bIsActive == bActive)
        return false;
    
    // Get some scene data to enable actor updating.
    float fT = -1.0f;
    NiTransform kRootTransform;
    float fScalePToW;
    if (bActive && m_spProp)
    {
        NiPhysXScene* pkScene = m_spProp->GetScene();
        if (pkScene)
        {
            fT = pkScene->GetPrevFetchTime();
            kRootTransform = pkScene->GetSceneXform();
            fScalePToW = pkScene->GetScaleFactor();
        }
    }

    unsigned int uiCount = pkSeqData->m_kActorData.GetSize();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        SequenceActorData* pkActorData = pkSeqData->m_kActorData.GetAt(ui);
        pkActorData->m_spDestination->SetActive(bActive);
        if (bActive && fT >= 0.0f)
        {
            // We need to update actors here to flush new state from the
            // kinematically controlled actors into the rigid body
            // destinations. 
            pkActorData->m_spDestination->UpdateFromActors(
                fT, kRootTransform, fScalePToW, true);
        }
        
        NxActor* pkActor = pkActorData->m_spDestination->GetActor();
        NIASSERT(pkActor);
        unsigned short usCount = 0;
        NIVERIFY(m_kActorActiveCount.GetAt(pkActor, usCount));
        if (bActive)
        {
            if (usCount == 0)
            {
                pkActor->wakeUp();
                pkActor->clearBodyFlag(NX_BF_KINEMATIC);
                pkActorData->m_spSource->SetActive(false);
            }
            usCount++;
        }
        else
        {
            NIASSERT(usCount - 1 < usCount); // unsigned count > 0
            usCount--;
            if (usCount == 0)
            {
                pkActor->raiseBodyFlag(NX_BF_KINEMATIC);
                pkActorData->m_spSource->SetActive(true);
            }
        }
        m_kActorActiveCount.SetAt(pkActor, usCount);
    }
    pkSeqData->m_bIsActive = bActive;

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXSequenceManager::CleanMaps()
{
    NiTMapIterator kIter = m_kSequences.GetFirstPos();
    while (kIter)
    {
        NiControllerSequence* pkSeq;
        SequenceData* pkData;
        m_kSequences.GetNext(kIter, pkSeq, pkData);
        if (pkSeq)
        {
            pkSeq->RemoveActivationCallbackObject(this);
        }
        if (pkData)
        {
            NiDelete pkData;
        }
    }
    m_kSequences.RemoveAll();

    kIter = m_kOriginalDestMap.GetFirstPos();
    while (kIter)
    {
        NiAVObject* pkObj;
        SequenceActorData* pkData;
        m_kOriginalDestMap.GetNext(kIter, pkObj, pkData);
        if (pkData)
        {
            NiDelete pkData;
        }
    }
    m_kOriginalDestMap.RemoveAll();

    m_kActorActiveCount.RemoveAll();
}
//---------------------------------------------------------------------------
NiPhysXSequenceManager::SequenceData::SequenceData()
{
    m_bIsActive = false;
}
//---------------------------------------------------------------------------
NiPhysXSequenceManager::SequenceData::~SequenceData()
{
    unsigned int uiCount = m_kActorData.GetSize();
    for (unsigned int ui = 0; ui < uiCount; ui++)
        NiDelete m_kActorData.GetAt(ui);
    m_kActorData.RemoveAll();
}
//---------------------------------------------------------------------------
NiPhysXSequenceManager::SequenceActorData::SequenceActorData()
{
    m_spDestination = 0;
    m_spSource = 0;
}
//---------------------------------------------------------------------------
NiPhysXSequenceManager::SequenceActorData::~SequenceActorData()
{
    m_spDestination = 0;
    m_spSource = 0;
}
//---------------------------------------------------------------------------

