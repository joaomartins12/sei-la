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

#include "NiFloatController.h"
#include "NiFloatData.h"
#include <NiObjectNET.h>

NiImplementRTTI(NiFloatController,NiTimeController);

//---------------------------------------------------------------------------
NiFloatController::~NiFloatController ()
{
    m_spFloatData = 0;
}
//---------------------------------------------------------------------------
NiFloatKey* NiFloatController::GetAnim(unsigned int& uiNumKeys,
    NiFloatKey::KeyType& eType, unsigned char& ucSize) const
{
    if ( m_spFloatData )
        return m_spFloatData->GetAnim(uiNumKeys,eType,ucSize);

    uiNumKeys = 0;
    eType = NiAnimationKey::NOINTERP;
    ucSize = 0;
    return 0;
}
//---------------------------------------------------------------------------
void NiFloatController::ReplaceAnim(unsigned int uiNumKeys,
    NiFloatKey* pkKeys, NiFloatKey::KeyType eType)
{
    NIASSERT( uiNumKeys > 0 );

    if ( !m_spFloatData )
        m_spFloatData = NiNew NiFloatData;
    m_spFloatData->ReplaceAnim(pkKeys,uiNumKeys,eType);

    unsigned char ucSize = NiFloatKey::GetKeySize(
        eType);

    m_uiLastIdx = 0;
    m_fLoKeyTime = pkKeys->GetKeyAt(0, ucSize)->GetTime();
    m_fHiKeyTime = pkKeys->GetKeyAt(uiNumKeys - 1, ucSize)->GetTime();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiFloatController::CopyMembers(NiFloatController* pDest,
    NiCloningProcess& kCloning)
{
    NiTimeController::CopyMembers(pDest, kCloning);

    if (m_spFloatData)
    {
        // NOTE.  By calling CreateClone() rather than passing m_spModelData
        // directly, this allows any change in what "clone" means to be
        // encapsulated by the CreateClone() call and does not require
        // changing the code here.
        pDest->SetFloatData((NiFloatData*) m_spFloatData->CreateClone(
            kCloning));
    }
}
//---------------------------------------------------------------------------
void NiFloatController::ProcessClone(NiCloningProcess& kCloning)
{
    NiTimeController::ProcessClone(kCloning);

    if (m_spFloatData)
    {
        m_spFloatData->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiFloatController::LoadBinary(NiStream& stream)
{
    NiTimeController::LoadBinary(stream);

    stream.ReadLinkID();   // m_spFloatData
}
//---------------------------------------------------------------------------
void NiFloatController::LinkObject(NiStream& stream)
{
    NiTimeController::LinkObject(stream);

    m_spFloatData = (NiFloatData*) stream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiFloatController::RegisterStreamables(NiStream& stream)
{
    if ( !NiTimeController::RegisterStreamables(stream) )
        return false;

    m_spFloatData->RegisterStreamables(stream);
    return true;
}
//---------------------------------------------------------------------------
void NiFloatController::SaveBinary(NiStream& stream)
{
    NiTimeController::SaveBinary(stream);

    stream.SaveLinkID(m_spFloatData);
}
//---------------------------------------------------------------------------
bool NiFloatController::IsEqual(NiObject* pObject)
{
    if ( !NiTimeController::IsEqual(pObject) )
        return false;

    NiFloatController* pControl = (NiFloatController*) pObject;

    if ( !m_spFloatData->IsEqual(pControl->m_spFloatData) )
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiFloatController::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiTimeController::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiFloatController::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
