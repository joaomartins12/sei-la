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

#include "NiLookAtController.h"

#include <NiCloningProcess.h>
#include <NiNode.h>

NiImplementRTTI(NiLookAtController,NiTimeController);

//---------------------------------------------------------------------------
void NiLookAtController::Update(float fTime)
{
    if (!m_pkLookAt || !GetActive())
        return;

    // m_pkLookAt is assumed to have been updated before the LookAtController
    // is evaluated. If this assumption is violated then the LookAtController
    // will appear to be a frame behind in the animation.
    NiPoint3 kTargetWorldPos = m_pkLookAt->GetWorldTranslate();

    // pkTarget has not yet had its world data updated. We compute what the
    // world translation will be in here so that we can set kUp the rotation
    // correctly.
    NiMatrix3 kParentWorldRot;
    NiPoint3 kMyWorldPos;
    NiAVObject* pkTarget = (NiAVObject*) m_pkTarget;
    NiNode* pkParent = pkTarget->GetParent();
    if (pkParent)
    {
        kParentWorldRot = pkParent->GetWorldRotate();
        kMyWorldPos = pkParent->GetWorldTranslate() +
            pkParent->GetWorldScale() * (kParentWorldRot *
            pkTarget->GetTranslate());
    }
    else
    {
        kParentWorldRot = NiMatrix3::IDENTITY;
        kMyWorldPos = pkTarget->GetTranslate();
    }
    
    NiMatrix3 kRot;
    NiPoint3 kAt = kTargetWorldPos - kMyWorldPos;
    if (kAt.SqrLength() < 0.001f)
    {
        // We have zero vector so don't update the transform
        kRot.MakeIdentity();
    }
    else    
    {
        NiPoint3 kUp, kRight;
        float fDot;

        kAt.Unitize();

        if (kAt.z < 1.001f && kAt.z > 0.999f)
        {
            // kUp & kAt are too close - use the Y axis as an alternate kUp
            kUp = NiPoint3::UNIT_Y;
            fDot = kAt.y;
        }
        else
        {
            kUp = NiPoint3::UNIT_Z;
            fDot = kAt.z;
        }


        // subtract off the component of 'kUp' in the 'kAt' direction
        kUp -= fDot*kAt;
        kUp.Unitize();

        if (!GetFlip())
            kAt = -kAt;

        kRight = kUp.Cross(kAt);

        switch (GetAxis())
        {
        case X:
            kRot.SetCol(0, kAt);
            kRot.SetCol(1, kUp);
            kRot.SetCol(2, -kRight);
            break;
        case Y:
            kRot.SetCol(0, kRight);
            kRot.SetCol(1, kAt);
            kRot.SetCol(2, -kUp);
            break;
        case Z:
            kRot.SetCol(0, kRight);
            kRot.SetCol(1, kUp);
            kRot.SetCol(2, kAt);
            break;
        }

        // kAt this point "kRot" is the desired world rotation. The next
        // line converts it into the model space rotation necessary to
        // result in the desired world space rotation.
        kRot = kParentWorldRot.TransposeTimes(kRot);
    }

    pkTarget->SetRotate(kRot);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiLookAtController);
//---------------------------------------------------------------------------
void NiLookAtController::CopyMembers(NiLookAtController* pkDest,
    NiCloningProcess& kCloning)
{
    NiTimeController::CopyMembers(pkDest, kCloning);

    pkDest->m_uFlags = m_uFlags;
    pkDest->SetFlip(GetFlip());
    pkDest->SetAxis(GetAxis());
    pkDest->SetLookAt(GetLookAt());
}
//---------------------------------------------------------------------------
void NiLookAtController::ProcessClone(
    NiCloningProcess& kCloning)
{
    NiTimeController::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);

    if (m_pkLookAt)
    {
        NiLookAtController* pkLookAt = (NiLookAtController*) pkClone;
        NiObject* pkClonedLookAt;
        bCloned = kCloning.m_pkCloneMap->GetAt(m_pkLookAt, pkClonedLookAt);

        if (!bCloned)
        {
            pkClonedLookAt = m_pkLookAt;
        }

        pkLookAt->m_pkLookAt = (NiAVObject*) pkClonedLookAt;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiLookAtController);
//---------------------------------------------------------------------------
void NiLookAtController::LoadBinary(NiStream& kStream)
{
    NiTimeController::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);

    kStream.ReadLinkID();   // m_pkLookAt
}
//---------------------------------------------------------------------------
void NiLookAtController::LinkObject(NiStream& kStream)
{
    NiTimeController::LinkObject(kStream);

    m_pkLookAt = (NiAVObject*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiLookAtController::RegisterStreamables(NiStream& kStream)
{
    if (!NiTimeController::RegisterStreamables(kStream))
        return false;

    if (m_pkLookAt != NULL)
        m_pkLookAt->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiLookAtController::SaveBinary(NiStream& kStream)
{
    NiTimeController::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uFlags);
    kStream.SaveLinkID(m_pkLookAt);
}
//---------------------------------------------------------------------------
bool NiLookAtController::IsEqual(NiObject* pkObject)
{
    if (!NiTimeController::IsEqual(pkObject))
        return false;

    NiLookAtController* pkControl = (NiLookAtController*) pkObject;

    if ((m_pkLookAt && !pkControl->m_pkLookAt) ||
         (!m_pkLookAt && pkControl->m_pkLookAt))
    {
        return false;
    }

    if (m_pkLookAt)
    {
        if (!m_pkLookAt->IsEqual(pkControl->m_pkLookAt))
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiLookAtController::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiTimeController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiLookAtController::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
