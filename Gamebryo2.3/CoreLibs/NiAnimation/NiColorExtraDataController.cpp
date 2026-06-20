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

#include "NiColorExtraDataController.h"
#include <NiColorExtraData.h>
#include "NiBlendColorInterpolator.h"
#include "NiColorData.h"
#include "NiColorInterpolator.h"

NiImplementRTTI(NiColorExtraDataController, NiExtraDataController);

//---------------------------------------------------------------------------
NiColorExtraDataController::NiColorExtraDataController(
    const NiFixedString& kColorExtraDataName) : NiExtraDataController(
    kColorExtraDataName)
{
}
//---------------------------------------------------------------------------
NiColorExtraDataController::NiColorExtraDataController()
{
}
//---------------------------------------------------------------------------
NiInterpolator* NiColorExtraDataController::CreatePoseInterpolator(
    unsigned short usIndex)
{
    NIASSERT(usIndex == 0);
    return NiNew NiColorInterpolator(NiSmartPointerCast(NiColorExtraData,
        m_spExtraData)->GetValue());
}
//---------------------------------------------------------------------------
void NiColorExtraDataController::SynchronizePoseInterpolator(NiInterpolator* 
    pkInterp, unsigned short usIndex)
{
    NiColorInterpolator* pkColorInterp = 
        NiDynamicCast(NiColorInterpolator, pkInterp);

    NIASSERT(usIndex == 0);
    NIASSERT(pkColorInterp);
    pkColorInterp->SetPoseValue(NiSmartPointerCast(NiColorExtraData,
        m_spExtraData)->GetValue());
}   
//---------------------------------------------------------------------------
NiBlendInterpolator* NiColorExtraDataController::CreateBlendInterpolator(
    unsigned short usIndex, bool bManagerControlled,
    bool bAccumulateAnimations, float fWeightThreshold,
    unsigned char ucArraySize) const
{
    NIASSERT(usIndex == 0);
    return NiNew NiBlendColorInterpolator(bManagerControlled, fWeightThreshold,
        ucArraySize);
}
//---------------------------------------------------------------------------
void NiColorExtraDataController::Update(float fTime)
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
        NiColorA kValue;
        if (m_spInterpolator->Update(m_fScaledTime, m_pkTarget, kValue))
        {
            if (m_spExtraData)
            {
                NIASSERT(NiIsKindOf(NiColorExtraData, m_spExtraData));
                NiSmartPointerCast(NiColorExtraData, m_spExtraData)->SetValue(
                    kValue);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiColorExtraDataController::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator, unsigned short usIndex) const
{
    NIASSERT(usIndex == 0);
    return pkInterpolator->IsColorAValueSupported();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiColorExtraDataController);
//---------------------------------------------------------------------------
void NiColorExtraDataController::CopyMembers(
    NiColorExtraDataController* pkDest, NiCloningProcess& kCloning)
{
    NiExtraDataController::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiColorExtraDataController);

//---------------------------------------------------------------------------
void NiColorExtraDataController::LoadBinary(NiStream& kStream)
{
    NiExtraDataController::LoadBinary(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        kStream.ReadLinkID();   // m_spColorData
    }
}
//---------------------------------------------------------------------------
void NiColorExtraDataController::LinkObject(NiStream& kStream)
{
    NiExtraDataController::LinkObject(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        NiColorData* pkColorData = (NiColorData*)
            kStream.GetObjectFromLinkID();
        m_spInterpolator = NiNew NiColorInterpolator(pkColorData);
        m_spInterpolator->Collapse();
    }
}
//---------------------------------------------------------------------------
bool NiColorExtraDataController::RegisterStreamables(NiStream& kStream)
{
    return NiExtraDataController::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiColorExtraDataController::SaveBinary(NiStream& kStream)
{
    NiExtraDataController::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiColorExtraDataController::IsEqual(NiObject* pkObject)
{
    return NiExtraDataController::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiColorExtraDataController::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiExtraDataController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiColorExtraDataController::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
