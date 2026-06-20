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

#include "NiBoolInterpController.h"
#include "NiBlendBoolInterpolator.h"
#include "NiBoolInterpolator.h"

NiImplementRTTI(NiBoolInterpController, NiSingleInterpController);

//---------------------------------------------------------------------------
NiBoolInterpController::NiBoolInterpController()
{
}
//---------------------------------------------------------------------------
NiInterpolator* NiBoolInterpController::CreatePoseInterpolator(
    unsigned short usIndex)
{
    NIASSERT(usIndex == 0);
    bool bValue;
    GetTargetBoolValue(bValue);
    return NiNew NiBoolInterpolator(bValue);
}
//---------------------------------------------------------------------------
void NiBoolInterpController::SynchronizePoseInterpolator(NiInterpolator* 
    pkInterp, unsigned short usIndex)
{
    NiBoolInterpolator* pkBoolInterp = 
        NiDynamicCast(NiBoolInterpolator, pkInterp);

    NIASSERT(usIndex == 0);
    NIASSERT(pkBoolInterp);
    bool bValue;
    GetTargetBoolValue(bValue);
    pkBoolInterp->SetPoseValue(bValue);
}   
//---------------------------------------------------------------------------
NiBlendInterpolator* NiBoolInterpController::CreateBlendInterpolator(
    unsigned short usIndex, bool bManagerControlled,
    bool bAccumulateAnimations, float fWeightThreshold,
    unsigned char ucArraySize) const
{
    NIASSERT(usIndex == 0);
    return NiNew NiBlendBoolInterpolator(bManagerControlled, fWeightThreshold,
        ucArraySize);
}
//---------------------------------------------------------------------------
bool NiBoolInterpController::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator, unsigned short usIndex) const
{
    NIASSERT(usIndex == 0);
    return pkInterpolator->IsBoolValueSupported();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiBoolInterpController::CopyMembers(NiBoolInterpController* pkDest,
    NiCloningProcess& kCloning)
{
    NiSingleInterpController::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiBoolInterpController::LoadBinary(NiStream& kStream)
{
    NiSingleInterpController::LoadBinary(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        return;
    }
}
//---------------------------------------------------------------------------
void NiBoolInterpController::LinkObject(NiStream& kStream)
{
    NiSingleInterpController::LinkObject(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        return;
    }
}
//---------------------------------------------------------------------------
bool NiBoolInterpController::RegisterStreamables(NiStream& kStream)
{
    return NiSingleInterpController::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiBoolInterpController::SaveBinary(NiStream& kStream)
{
    NiSingleInterpController::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiBoolInterpController::IsEqual(NiObject* pkObject)
{
    return NiSingleInterpController::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBoolInterpController::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiSingleInterpController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiBoolInterpController::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
