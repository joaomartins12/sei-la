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

#include "NiPoint3InterpController.h"
#include "NiBlendPoint3Interpolator.h"
#include "NiPoint3Interpolator.h"
#include <NiPoint3.h>

NiImplementRTTI(NiPoint3InterpController, NiSingleInterpController);

//---------------------------------------------------------------------------
NiPoint3InterpController::NiPoint3InterpController() 
{
}
//---------------------------------------------------------------------------
NiInterpolator* NiPoint3InterpController::CreatePoseInterpolator(
    unsigned short usIndex)
{
    NIASSERT(usIndex == 0);
    NiPoint3 kValue;
    GetTargetPoint3Value(kValue);
    return NiNew NiPoint3Interpolator(kValue);
}
//---------------------------------------------------------------------------
void NiPoint3InterpController::SynchronizePoseInterpolator(NiInterpolator* 
    pkInterp, unsigned short usIndex)
{
    NiPoint3Interpolator* pkPoint3Interp = 
        NiDynamicCast(NiPoint3Interpolator, pkInterp);

    NIASSERT(usIndex == 0);
    NIASSERT(pkPoint3Interp);
    NiPoint3 kValue;
    GetTargetPoint3Value(kValue);
    pkPoint3Interp->SetPoseValue(kValue);
}   
//---------------------------------------------------------------------------
NiBlendInterpolator* NiPoint3InterpController::CreateBlendInterpolator(
    unsigned short usIndex, bool bManagerControlled,
    bool bAccumulateAnimations, float fWeightThreshold,
    unsigned char ucArraySize) const
{
    NIASSERT(usIndex == 0);
    return NiNew NiBlendPoint3Interpolator(bManagerControlled,
        fWeightThreshold, ucArraySize);
}
//---------------------------------------------------------------------------
bool NiPoint3InterpController::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator, unsigned short usIndex) const
{
    NIASSERT(usIndex == 0);
    return pkInterpolator->IsPoint3ValueSupported();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiPoint3InterpController::CopyMembers(NiPoint3InterpController* pkDest,
    NiCloningProcess& kCloning)
{
    NiSingleInterpController::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiPoint3InterpController::LoadBinary(NiStream& kStream)
{
    NiSingleInterpController::LoadBinary(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        return;
    }
}
//---------------------------------------------------------------------------
void NiPoint3InterpController::LinkObject(NiStream& kStream)
{
    NiSingleInterpController::LinkObject(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        return;
    }
}
//---------------------------------------------------------------------------
bool NiPoint3InterpController::RegisterStreamables(NiStream& kStream)
{
    return NiSingleInterpController::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPoint3InterpController::SaveBinary(NiStream& kStream)
{
    NiSingleInterpController::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPoint3InterpController::IsEqual(NiObject* pkObject)
{
    return NiSingleInterpController::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPoint3InterpController::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiSingleInterpController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPoint3InterpController::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
