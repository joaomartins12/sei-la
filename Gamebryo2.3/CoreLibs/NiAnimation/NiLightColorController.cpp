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

#include <NiBool.h>
#include "NiLightColorController.h"
#include "NiPoint3Interpolator.h"

NiImplementRTTI(NiLightColorController,NiPoint3InterpController);

//---------------------------------------------------------------------------
NiLightColorController::NiLightColorController()
{ 
    m_uFlags = 0;
    SetAmbient(false);
}

//---------------------------------------------------------------------------
NiLightColorController::~NiLightColorController()
{ /* */ }

//---------------------------------------------------------------------------
bool NiLightColorController::InterpTargetIsCorrectType(NiObjectNET* 
    pkTarget) const
{
    return (NiIsKindOf(NiLight, pkTarget));
}
//---------------------------------------------------------------------------
void NiLightColorController::Update(float fTime)
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
        NiPoint3 kValue;
        if (m_spInterpolator->Update(m_fScaledTime, m_pkTarget, kValue))
        {
            NiColor kColor(kValue.x, kValue.y, kValue.z);

            if (kColor.r < 0.0f)
                kColor.r = 0.0f;
            else if (kColor.r > 1.0f)
                kColor.r = 1.0f;

            if (kColor.g < 0.0f)
                kColor.g = 0.0f;
            else if (kColor.g > 1.0f)
                kColor.g = 1.0f;

            if (kColor.b < 0.0f)
                kColor.b = 0.0f;
            else if (kColor.b > 1.0f)
                kColor.b = 1.0f;

            if (m_pkTarget)
            {
                NiLight* pLight = NiDynamicCast(NiLight, m_pkTarget);

                if (GetAmbient())
                {
                    pLight->SetAmbientColor(kColor);
                    pLight->SetDiffuseColor(NiColor::BLACK);
                    pLight->SetSpecularColor(NiColor::BLACK);
                }
                else
                {
                    pLight->SetAmbientColor(NiColor::BLACK);
                    pLight->SetDiffuseColor(kColor);
                    pLight->SetSpecularColor(kColor);
                }
            }
        }
    }
    
}
//---------------------------------------------------------------------------
void NiLightColorController::GetTargetPoint3Value(NiPoint3& kValue)
{
    if (m_pkTarget)
    {
        NiLight* pLight = NiSmartPointerCast(NiLight, m_pkTarget);

        NiColor kColor;
        if (GetAmbient())
        {
            kColor = pLight->GetAmbientColor();
        }
        else
        {
            kColor = pLight->GetDiffuseColor();
        }
        kValue.x = kColor.r;
        kValue.y = kColor.g;
        kValue.z = kColor.b;
    }
}
//---------------------------------------------------------------------------
const char* NiLightColorController::GetCtlrID()
{
    if (GetAmbient())
        return "Ambient";
    else
        return "Diffuse";
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiLightColorController);
//---------------------------------------------------------------------------
void NiLightColorController::CopyMembers(NiLightColorController* pkDest,
    NiCloningProcess& kCloning)
{
    NiPoint3InterpController::CopyMembers(pkDest, kCloning);
    pkDest->m_uFlags = m_uFlags;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiLightColorController);
//---------------------------------------------------------------------------
void NiLightColorController::LoadBinary(NiStream& kStream)
{
    NiPoint3InterpController::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);

    // Pre-Gamebryo 1.2, the streaming data contained pointers to PosData
    // We need to convert this into an interpolator.
    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        kStream.ReadLinkID();   // m_spPosData
    }
   
}
//---------------------------------------------------------------------------
void NiLightColorController::LinkObject(NiStream& kStream)
{
    NiPoint3InterpController::LinkObject(kStream);

    // Pre-Gamebryo 1.2, the streaming data contained pointers to PosData
    // We need to convert this into an interpolator.
    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        // link the shared data
        NiPosDataPtr spPosData = (NiPosData*) kStream.GetObjectFromLinkID();
        m_spInterpolator = NiNew NiPoint3Interpolator(spPosData);
        m_spInterpolator->Collapse();
    }
}
//---------------------------------------------------------------------------
bool NiLightColorController::RegisterStreamables(NiStream& kStream)
{
    if (!NiPoint3InterpController::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiLightColorController::SaveBinary(NiStream& kStream)
{
    NiPoint3InterpController::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
bool NiLightColorController::IsEqual(NiObject* pkObject)
{
    if (!NiPoint3InterpController::IsEqual(pkObject))
        return false;

    NiLightColorController* pkControl = (NiLightColorController*) pkObject;

    if (GetAmbient() != pkControl->GetAmbient())
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiLightColorController::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPoint3InterpController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiLightColorController::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------

