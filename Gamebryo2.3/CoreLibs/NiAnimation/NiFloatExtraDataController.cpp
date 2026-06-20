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

#include "NiFloatExtraDataController.h"
#include <NiFloatExtraData.h>
#include "NiBlendFloatInterpolator.h"
#include "NiFloatData.h"
#include "NiFloatInterpolator.h"

NiImplementRTTI(NiFloatExtraDataController, NiExtraDataController);

//---------------------------------------------------------------------------
NiFloatExtraDataController::NiFloatExtraDataController(
    const NiFixedString& kFloatExtraDataName) : NiExtraDataController(
    kFloatExtraDataName)
{
}
//---------------------------------------------------------------------------
NiFloatExtraDataController::NiFloatExtraDataController()
{
}
//---------------------------------------------------------------------------
NiInterpolator* NiFloatExtraDataController::CreatePoseInterpolator(
    unsigned short usIndex)
{
    NIASSERT(usIndex == 0);
    return NiNew NiFloatInterpolator(NiSmartPointerCast(NiFloatExtraData,
        m_spExtraData)->GetValue());
}
//---------------------------------------------------------------------------
void NiFloatExtraDataController::SynchronizePoseInterpolator(NiInterpolator* 
    pkInterp, unsigned short usIndex)
{
    NiFloatInterpolator* pkFloatInterp = 
        NiDynamicCast(NiFloatInterpolator, pkInterp);

    NIASSERT(usIndex == 0);
    NIASSERT(pkFloatInterp);
    pkFloatInterp->SetPoseValue(NiSmartPointerCast(NiFloatExtraData,
        m_spExtraData)->GetValue());
}   
//---------------------------------------------------------------------------
NiBlendInterpolator* NiFloatExtraDataController::CreateBlendInterpolator(
    unsigned short usIndex, bool bManagerControlled,
    bool bAccumulateAnimations, float fWeightThreshold,
    unsigned char ucArraySize) const
{
    NIASSERT(usIndex == 0);
    return NiNew NiBlendFloatInterpolator(bManagerControlled, fWeightThreshold,
        ucArraySize);
}
//---------------------------------------------------------------------------
void NiFloatExtraDataController::Update(float fTime)
{
    if (GetManagerControlled())
    {
        m_fScaledTime = INVALID_TIME;
    }
    else if (DontDoUpdate(fTime) &&
        (!m_spInterpolator || !m_spInterpolator->AlwaysUpdate()))
    {
        return;
    }

    if (m_spInterpolator)
    {
        float fValue;
        if (m_spInterpolator->Update(m_fScaledTime, m_pkTarget, fValue))
        {
            if (m_spExtraData)
            {
                NIASSERT(NiIsKindOf(NiFloatExtraData, m_spExtraData));
                NiSmartPointerCast(NiFloatExtraData, m_spExtraData)->SetValue(
                    fValue);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiFloatExtraDataController::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator, unsigned short usIndex) const
{
    NIASSERT(usIndex == 0);
    return pkInterpolator->IsFloatValueSupported();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiFloatExtraDataController);
//---------------------------------------------------------------------------
void NiFloatExtraDataController::CopyMembers(
    NiFloatExtraDataController* pkDest, NiCloningProcess& kCloning)
{
    NiExtraDataController::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiFloatExtraDataController);
//---------------------------------------------------------------------------
void NiFloatExtraDataController::LoadBinary(NiStream& kStream)
{
    NiExtraDataController::LoadBinary(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        kStream.ReadLinkID();   // m_spFloatData
    }
}
//---------------------------------------------------------------------------
void NiFloatExtraDataController::LinkObject(NiStream& kStream)
{
    NiExtraDataController::LinkObject(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        NiFloatData* pkFloatData = (NiFloatData*)
            kStream.GetObjectFromLinkID();
        m_spInterpolator = NiNew NiFloatInterpolator(pkFloatData);
        m_spInterpolator->Collapse();
    }
}
//---------------------------------------------------------------------------
bool NiFloatExtraDataController::RegisterStreamables(NiStream& kStream)
{
    return NiExtraDataController::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiFloatExtraDataController::SaveBinary(NiStream& kStream)
{
    NiExtraDataController::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiFloatExtraDataController::IsEqual(NiObject* pkObject)
{
    return NiExtraDataController::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiFloatExtraDataController::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiExtraDataController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiFloatExtraDataController::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
