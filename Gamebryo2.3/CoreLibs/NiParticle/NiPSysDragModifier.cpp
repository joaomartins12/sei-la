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
#include "NiParticlePCH.h"

#include "NiPSysDragModifier.h"
#include "NiParticleSystem.h"
#include <NiCloningProcess.h>

NiImplementRTTI(NiPSysDragModifier, NiPSysModifier);
#define DRAG_MIN_SAMPLE_TIME 0.0333333f
//---------------------------------------------------------------------------
NiPSysDragModifier::NiPSysDragModifier(const char* pcName,
    NiAVObject* pkDragObj, NiPoint3 kDragAxis, float fPercentage,
    float fRange, float fFalloff) : 
    NiPSysModifier(pcName, ORDER_FORCE),
    m_pkDragObj(pkDragObj), m_kDragAxis(kDragAxis),
    m_fPercentage(fPercentage), m_fRange(fRange), m_fFalloff(fFalloff)
{
}
//---------------------------------------------------------------------------
NiPSysDragModifier::NiPSysDragModifier() : m_pkDragObj(NULL),
    m_kDragAxis(NiPoint3::UNIT_X), m_fPercentage(0.05f),
    m_fRange(NI_INFINITY), m_fFalloff(NI_INFINITY)
{
}
//---------------------------------------------------------------------------
void NiPSysDragModifier::Update(float fTime, NiPSysData* pkData)
{
    if (m_fPercentage > 0.0f)
    {
        NiPoint3 kPosition = NiPoint3::ZERO;
        NiPoint3 kDragAxis = m_kDragAxis;
        if (pkData->GetNumParticles() > 0 && m_pkDragObj)
        {
            NiTransform kDrag = m_pkDragObj->GetWorldTransform();
            NiTransform kPSys = m_pkTarget->GetWorldTransform();
            NiTransform kInvPSys;
            kPSys.Invert(kInvPSys);
            NiTransform kDragToPSys = kInvPSys * kDrag;

            kPosition = kDragToPSys.m_Translate;
            kDragAxis = kDragToPSys.m_Rotate * m_kDragAxis;
            kDragAxis.Unitize();
        }
        else // No particles to deal with or no drag object, do nothing
        {
            NIASSERT(m_pkDragObj);
            return;
        }

        float fLenSquared = kDragAxis.SqrLength();
        float fRangeDifference = m_fFalloff - m_fRange;
                
        for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
        {
            NiPoint3 kParticlePosition = pkData->GetVertices()[us];
            NiParticleInfo* pkCurrentParticle = &pkData
                ->GetParticleInfo()[us];

            float fDistance = (kParticlePosition - kPosition).Length();
            float fPercentage = m_fPercentage;
            float fDeltaT = (fTime - pkCurrentParticle->m_fLastUpdate) / 
                (DRAG_MIN_SAMPLE_TIME);

            if (fDistance > m_fRange)
            {
                if (fDistance >= m_fFalloff)
                    continue;

                float fDifference = fDistance - m_fRange;
                fPercentage = (1.0f - fDifference / fRangeDifference) * 
                    fPercentage;
            }
        
        
            // Project the drag axis onto the velocity vector
            float fProj = pkCurrentParticle->m_kVelocity.Dot(kDragAxis);

            // Ensure you never decelerate so much we go backwards
            if ((fPercentage * fDeltaT) > 1.0f)
            {
                pkCurrentParticle->m_kVelocity += (-fProj / fLenSquared) * 
                    kDragAxis;
            }
            else
            {
                pkCurrentParticle->m_kVelocity += -fPercentage * fDeltaT * 
                    (fProj / fLenSquared) * kDragAxis;
            }
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysDragModifier);
//---------------------------------------------------------------------------
void NiPSysDragModifier::CopyMembers(NiPSysDragModifier* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifier::CopyMembers(pkDest, kCloning);

    pkDest->m_kDragAxis = m_kDragAxis;
    pkDest->m_fPercentage = m_fPercentage;
    pkDest->m_fRange = m_fRange;
    pkDest->m_fFalloff = m_fFalloff;
}
//---------------------------------------------------------------------------
void NiPSysDragModifier::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysModifier::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);
    NiPSysDragModifier* pkDest = (NiPSysDragModifier*) pkClone;

    if (m_pkDragObj)
    {
        bCloned = kCloning.m_pkCloneMap->GetAt(m_pkDragObj, pkClone);
        if (bCloned)
        {
            NIASSERT(m_pkDragObj->GetRTTI() == pkClone->GetRTTI());
            pkDest->m_pkDragObj = (NiAVObject*) pkClone;
        }
        else
        {
            pkDest->m_pkDragObj = m_pkDragObj;
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysDragModifier);
//---------------------------------------------------------------------------
void NiPSysDragModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_pkDragObj
    m_kDragAxis.LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_fPercentage);
    NiStreamLoadBinary(kStream, m_fRange);
    NiStreamLoadBinary(kStream, m_fFalloff);

}
//---------------------------------------------------------------------------
void NiPSysDragModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);

    m_pkDragObj = (NiAVObject*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPSysDragModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysDragModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkDragObj);
    m_kDragAxis.SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_fPercentage);
    NiStreamSaveBinary(kStream, m_fRange);
    NiStreamSaveBinary(kStream, m_fFalloff);
}
//---------------------------------------------------------------------------
bool NiPSysDragModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysDragModifier* pkDest = (NiPSysDragModifier*) pkObject;

    if ((m_pkDragObj && !pkDest->m_pkDragObj) ||
        (!m_pkDragObj && pkDest->m_pkDragObj) ||
        (m_pkDragObj && pkDest->m_pkDragObj &&
            !m_pkDragObj->IsEqual(pkDest->m_pkDragObj)))
    {
        return false;
    }

    if (pkDest->m_kDragAxis != m_kDragAxis ||
        pkDest->m_fPercentage != m_fPercentage ||
        pkDest->m_fRange != m_fRange ||
        pkDest->m_fFalloff != m_fFalloff)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysDragModifier::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysDragModifier::ms_RTTI.GetName()));

    const char* pcDragObj;
    if (m_pkDragObj)
    {
        pcDragObj = m_pkDragObj->GetName();
    }
    else
    {
        pcDragObj = "None";
    }
    pkStrings->Add(NiGetViewerString("Drag Object", pcDragObj));
    pkStrings->Add(m_kDragAxis.GetViewerString("Drag Axis"));
    pkStrings->Add(NiGetViewerString("Percentage", m_fPercentage));
    pkStrings->Add(NiGetViewerString("Range", m_fRange));
    pkStrings->Add(NiGetViewerString("RangeFalloff", m_fFalloff));
}
//---------------------------------------------------------------------------
