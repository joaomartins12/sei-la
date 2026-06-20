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

#include <NiAVObject.h>
#include "NiFloatData.h"
#include "NiRollController.h"

NiImplementRTTI(NiRollController,NiFloatController);

//---------------------------------------------------------------------------
void NiRollController::Update(float fTime)
{
    if ( !m_pkTarget || DontDoUpdate(fTime) )
        return;

    unsigned int uiNumKeys;
    unsigned char ucSize;
    NiFloatKey::KeyType eType;
    NiFloatKey* pkKeys = GetAnim(uiNumKeys,eType,ucSize);

    float fRoll = NiFloatKey::GenInterp(m_fScaledTime,pkKeys,eType,uiNumKeys,
        m_uiLastIdx, ucSize);

    NiMatrix3 rot = ((NiAVObject*)m_pkTarget)->GetRotate();
    NiMatrix3 roll;
    roll.MakeZRotation(-fRoll);
    rot = rot*roll;
    ((NiAVObject*)m_pkTarget)->SetRotate(rot);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiRollController);
//---------------------------------------------------------------------------
void NiRollController::CopyMembers(NiRollController* pDest,
    NiCloningProcess& kCloning)
{
    NiFloatController::CopyMembers(pDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiRollController);
//---------------------------------------------------------------------------
void NiRollController::LoadBinary(NiStream& stream)
{
    NiFloatController::LoadBinary(stream);
}
//---------------------------------------------------------------------------
void NiRollController::LinkObject(NiStream& stream)
{
    NiFloatController::LinkObject(stream);
}
//---------------------------------------------------------------------------
bool NiRollController::RegisterStreamables(NiStream& stream)
{
    return NiFloatController::RegisterStreamables(stream);
}
//---------------------------------------------------------------------------
void NiRollController::SaveBinary(NiStream& stream)
{
    NiFloatController::SaveBinary(stream);
}
//---------------------------------------------------------------------------
bool NiRollController::IsEqual(NiObject* pObject)
{
    return NiFloatController::IsEqual(pObject);
}
//---------------------------------------------------------------------------
void NiRollController::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiTimeController::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiRollController::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
