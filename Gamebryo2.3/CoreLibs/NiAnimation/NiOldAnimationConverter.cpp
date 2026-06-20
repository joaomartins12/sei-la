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

#include "NiOldAnimationConverter.h"

//---------------------------------------------------------------------------
void NiOldAnimationConverter::Convert(NiStream& kStream, NiObject* pkObject)
{
    // If this file is greater than 10.1.0.104, then it is already correct
    if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 1, 0, 104))
    {
        return;
    }

    if (NiIsKindOf(NiObjectNET, pkObject))
    {
        RecurseScene(kStream, (NiObjectNET*) pkObject);
    }
}
//---------------------------------------------------------------------------
void NiOldAnimationConverter::RecurseScene(NiStream& kStream, 
    NiObjectNET* pkObject)
{
    if (pkObject == NULL)
        return;

    if (NiIsKindOf (NiAVObject, pkObject))
    {
        // Iterate over all time controllers, converting as necessary
        NiTimeController* pkController = pkObject->GetControllers();
        NiTimeController* pkNextController = NULL;
        while (pkController != NULL)
        {
            // Hold on to the next pointer, as it might change in the
            // conversion process
            pkNextController = pkController->GetNext();
            if (NiIsKindOf(NiRollController, pkController))
                ConvertRollController((NiRollController*) pkController);
            else if (NiIsKindOf(NiLookAtController, pkController))
                ConvertLookAtController((NiLookAtController*) pkController);
            else if (NiIsKindOf(NiPathController, pkController))
                ConvertPathController((NiPathController*) pkController);
            
            pkController = pkNextController;
        }

        // If we ever need to convert property time controllers, this code 
        // should be uncommented
//        NiPropertyList& kPropList = pkAVObject->GetPropertyList();
//        NiTListIterator kNext = kPropList.GetHead();
//        while (kNext != NULL)
//        {
//            NiProperty* pkProperty = kPropList.GetNext(kNext);
//            RecurseScene(kStream, kProperty);
//        }

        // If this is an NiNode, we need to convert its children
        if (NiIsKindOf(NiNode, pkObject))
        {
            NiNode* pkNode = (NiNode*) pkObject;
            for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
                RecurseScene(kStream, pkNode->GetAt(ui));
        }
    }
}
//---------------------------------------------------------------------------
void NiOldAnimationConverter::ConvertLookAtController(NiLookAtController* 
    pkController)
{
    NiObjectNET* pkTarget = pkController->GetTarget();

    bool bSetOnTarget = false;
    NiTransformController* pkTransformController = NiGetController(
        NiTransformController, pkTarget);

    if (pkTransformController == NULL ||
        !NiIsKindOf(NiTransformInterpolator,
            pkTransformController->GetInterpolator()))
    {
        bSetOnTarget = true;
        pkTransformController = NiNew NiTransformController();
        SynchronizeTimeControllers(pkController, pkTransformController);
    }

    NiLookAtInterpolator* pkLookAtInterp = NiNew NiLookAtInterpolator();
    pkLookAtInterp->SetLookAt(pkController->GetLookAt());
    pkLookAtInterp->SetFlip(pkController->GetFlip());
    pkLookAtInterp->SetAxis((NiLookAtInterpolator::Axis) 
        pkController->GetAxis());


    NiInterpolatorPtr spInterp = pkTransformController->GetInterpolator();

    if (spInterp && NiIsKindOf(NiTransformInterpolator, spInterp))
    {
        NiTransformInterpolator* pkOldInterp = NiSmartPointerCast(
            NiTransformInterpolator, spInterp);

        if (pkOldInterp->GetNumPosKeys())
        {
            unsigned int uiNumKeys;
            NiAnimationKey::KeyType eType;
            unsigned char ucSize;

            NiPoint3Interpolator* pkTranslateInterp =
                NiNew NiPoint3Interpolator;
            NiPosKey* pkKeys = (NiPosKey*)pkOldInterp->GetPosData(
                uiNumKeys, eType, ucSize);

            pkTranslateInterp->SetKeys(pkKeys, uiNumKeys, eType);
            // Transfer ownership of the keys
            pkOldInterp->GetTransformData()->SetPosAnim(NULL, 0, 
                NiAnimationKey::NOINTERP);
            pkLookAtInterp->SetTranslateInterpolator(pkTranslateInterp);
        }

        if (pkOldInterp->GetNumScaleKeys())
        {
            unsigned int uiNumKeys;
            NiAnimationKey::KeyType eType;
            unsigned char ucSize;

            NiFloatInterpolator* pkScaleInterp = NiNew NiFloatInterpolator;
            NiFloatKey* pkKeys = (NiFloatKey*)pkOldInterp->GetScaleData(
                uiNumKeys, eType, ucSize);

            pkScaleInterp->SetKeys(pkKeys, uiNumKeys, eType);
            
            // Transfer ownership of the keys
            pkOldInterp->GetTransformData()->SetScaleAnim(NULL, 0, 
                NiAnimationKey::NOINTERP);
            pkLookAtInterp->SetScaleInterpolator(pkScaleInterp);
        }
    }

    pkTransformController->SetInterpolator(pkLookAtInterp);
    pkLookAtInterp->Collapse();
    
    if (bSetOnTarget)
    {
        pkTransformController->SetTarget(pkTarget);
    }

    if (pkTarget)
    {
        pkTarget->RemoveController(pkController);
    }
}
//---------------------------------------------------------------------------
void NiOldAnimationConverter::ConvertRollController(NiRollController* 
    pkController)
{
    NiObjectNET* pkTarget = pkController->GetTarget();

    NiTransformController* pkTransformController = NULL;
    NiLookAtInterpolator* pkLookAtInterpolator = NULL;
    NiTimeController* pkCtlr = pkTarget->GetControllers();
    while (pkCtlr)
    {
        if (NiIsKindOf(NiTransformController, pkCtlr))
        {
            NiInterpolator* pkInterp = ((NiTransformController*) pkCtlr)
                ->GetInterpolator();
            if (NiIsKindOf(NiLookAtInterpolator, pkInterp))
            {
                pkTransformController = (NiTransformController*) pkCtlr;
                pkLookAtInterpolator = (NiLookAtInterpolator*) pkInterp;
                break;
            }
        }
        pkCtlr = pkCtlr->GetNext();
    }

    if (pkTransformController != NULL && pkLookAtInterpolator != NULL)
    {
        NiFloatData* pkData = pkController->GetFloatData();
        NiFloatInterpolator* pkRollInterp = NiNew NiFloatInterpolator(pkData);

        pkLookAtInterpolator->SetRollInterpolator(pkRollInterp);
        SynchronizeTimeControllers(pkController, pkTransformController);
            
        float fStart;
        float fEnd;

        pkLookAtInterpolator->GetActiveTimeRange(fStart, fEnd);
        pkLookAtInterpolator->GuaranteeTimeRange(fStart, fEnd);
        pkTransformController->ResetTimeExtrema();
        pkLookAtInterpolator->Collapse();
    }

    if (pkTarget)
    {
        pkTarget->RemoveController(pkController);
    }
}
//---------------------------------------------------------------------------
void NiOldAnimationConverter::ConvertPathController(NiPathController* 
    pkController)
{
    NiObjectNET* pkTarget = pkController->GetTarget();

    NiTransformControllerPtr spTransformController = NiGetController(
        NiTransformController, pkTarget);

    NiPathInterpolator* pkInterp = NiNew NiPathInterpolator(
        pkController->GetPathData(), pkController->GetPctData());
    
    pkInterp->SetAllowFlip(pkController->GetAllowFlip());
    pkInterp->SetBank(pkController->GetBank());
    pkInterp->SetBankDir((NiPathInterpolator::BankDir) 
        pkController->GetBankDir());
    pkInterp->SetConstVelocity(pkController->GetConstVelocity());
    pkInterp->SetFollow(pkController->GetFollow());
    pkInterp->SetMaxBankAngle(pkController->GetMaxBankAngle());
    pkInterp->SetSmoothing(pkController->GetSmoothing());
    pkInterp->SetFollowAxis(pkController->GetFollowAxis());
    pkInterp->SetFlip(pkController->GetFlip());
    pkInterp->SetCurveTypeOpen(pkController->GetCurveTypeOpen());

    // If a transform controller does not exist or the one that was found
    // already has an interpolator, create and attach a new transform
    // controller.
    if (spTransformController == NULL ||
        spTransformController->GetInterpolator() != NULL)
    {
        spTransformController = NiNew NiTransformController();
        spTransformController->SetInterpolator(pkInterp);
        spTransformController->SetTarget(pkTarget);
        SynchronizeTimeControllers(pkController, spTransformController);

        // Ensure that the new NiTransformController is immediately after
        // the NiPathController in pkTarget's controller list. Since
        // SetTarget prepends controllers to the target, we know that
        // spTransformController is the first controller in pkTarget's
        // controller list. spTransformController is a smart pointer to ensure
        // that it is not deleted during this list re-order.
        pkTarget->SetControllers(spTransformController->GetNext());
        spTransformController->SetNext(pkController->GetNext());
        pkController->SetNext(spTransformController);
    }
    else
    {
        spTransformController->SetInterpolator(pkInterp);
        SynchronizeTimeControllers(pkController, spTransformController);
    }

    pkInterp->Collapse();

    if (pkTarget)
    {
        pkTarget->RemoveController(pkController);
    }
}
//---------------------------------------------------------------------------
void NiOldAnimationConverter::SynchronizeTimeControllers(
    const NiTimeController* pkSrc, NiTimeController* pkDest)
{
    pkDest->SetAnimType(pkSrc->GetAnimType());
    pkDest->SetCycleType(pkSrc->GetCycleType());
    pkDest->SetPlayBackwards(pkSrc->GetPlayBackwards());
    pkDest->SetFrequency(pkSrc->GetFrequency());
    pkDest->SetBeginKeyTime(pkSrc->GetBeginKeyTime());
    pkDest->SetEndKeyTime(pkSrc->GetEndKeyTime());
    pkDest->SetActive(pkSrc->GetActive());
}
//---------------------------------------------------------------------------
