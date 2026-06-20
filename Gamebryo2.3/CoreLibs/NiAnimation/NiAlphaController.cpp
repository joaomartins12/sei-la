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

#include "NiAlphaController.h"
#include <NiMaterialProperty.h>
#include "NiFloatInterpolator.h"
#include "NiFloatData.h"

NiImplementRTTI(NiAlphaController, NiFloatInterpController);

//---------------------------------------------------------------------------
NiAlphaController::NiAlphaController()
{
}
//---------------------------------------------------------------------------
bool NiAlphaController::InterpTargetIsCorrectType(NiObjectNET* pkTarget) const
{
    return (NiIsKindOf(NiMaterialProperty, pkTarget));
}
//---------------------------------------------------------------------------
void NiAlphaController::GetTargetFloatValue(float& fValue)
{
    fValue = ((NiMaterialProperty*) m_pkTarget)->GetAlpha();
}
//---------------------------------------------------------------------------
void NiAlphaController::Update(float fTime)
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
            ((NiMaterialProperty*)m_pkTarget)->SetAlpha(fValue);
    }
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiAlphaController);
//---------------------------------------------------------------------------
void NiAlphaController::CopyMembers(NiAlphaController* pkDest,
    NiCloningProcess& kCloning)
{
    NiFloatInterpController::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiAlphaController);

//---------------------------------------------------------------------------
void NiAlphaController::LoadBinary(NiStream& kStream)
{
    NiFloatInterpController::LoadBinary(kStream);
    
    // This code will replace the streaming code for NiFloatController.
    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        kStream.ReadLinkID();   // m_spFloatData
    }
}
//---------------------------------------------------------------------------
void NiAlphaController::LinkObject(NiStream& kStream)
{
    NiFloatInterpController::LinkObject(kStream);
    
    // This code will replace the streaming code for NiFloatController.
    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        NiFloatData* pkFloatData = (NiFloatData*) 
            kStream.GetObjectFromLinkID(); 

        m_spInterpolator = NiNew NiFloatInterpolator(
            pkFloatData);
        m_spInterpolator->Collapse();
    }
}
//---------------------------------------------------------------------------
bool NiAlphaController::RegisterStreamables(NiStream& kStream)
{
    return NiFloatInterpController::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiAlphaController::SaveBinary(NiStream& kStream)
{
    NiFloatInterpController::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiAlphaController::IsEqual(NiObject* pkObject)
{
    return NiFloatInterpController::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiAlphaController::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiFloatInterpController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiAlphaController::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------

