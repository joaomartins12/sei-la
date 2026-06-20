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

#include "NiPSysPlanarCollider.h"
#include "NiPSysColliderManager.h"
#include "NiParticleSystem.h"
#include <NiAVObject.h>
#include <NiCloningProcess.h>

NiImplementRTTI(NiPSysPlanarCollider, NiPSysCollider);

static const float gs_fEpsilon = 0.001f;

//---------------------------------------------------------------------------
NiPSysPlanarCollider::NiPSysPlanarCollider(float fBounce,
    bool bSpawnOnCollide, bool bDieOnCollide,
    NiPSysSpawnModifier* pkSpawnModifier, NiAVObject* pkColliderObj,
    float fWidth, float fHeight, NiPoint3 kXAxis, NiPoint3 kYAxis) :
    NiPSysCollider(fBounce, bSpawnOnCollide, bDieOnCollide, pkSpawnModifier),
    m_pkColliderObj(pkColliderObj), m_fWidth(fWidth), m_fHeight(fHeight),
    m_kXAxis(kXAxis), m_kYAxis(kYAxis), m_fHalfSqScaledWidth(0.5f),
    m_fHalfSqScaledHeight(0.5f), m_kPlane(NiPlane(NiPoint3::ZERO, 0.0f)),
    m_kPosition(NiPoint3::ZERO), m_kRotation(NiMatrix3::IDENTITY)
{
    m_kLastColliderTransform.MakeIdentity();
    m_kLastPSysTransform.MakeIdentity();
}
//---------------------------------------------------------------------------
NiPSysCollider* NiPSysPlanarCollider::Resolve(float fInitialTime,
    float& fCollisionTime, NiPSysData* pkData, unsigned short usCurParticle)
{
    bool bCollision = false;


    NiPoint3* pkParticlePos = &pkData->GetVertices()[usCurParticle];
    NiParticleInfo* pkCurParticle = &pkData->GetParticleInfo()
        [usCurParticle];

    float fDistToPlane = m_kPlane.Distance(*pkParticlePos);
    if (fDistToPlane < gs_fEpsilon && -fDistToPlane < gs_fEpsilon)
    {
        // Make sure collision point is inside rectangle
        NiPoint3 kDisplacementFromCenter = *pkParticlePos - m_kPosition;
        NiPoint3 kRotatedXAxis = m_kRotation * m_kXAxis;
        NiPoint3 kRotatedYAxis = m_kRotation * m_kYAxis;
        NiPoint3 kXComponent = (kRotatedXAxis * kDisplacementFromCenter) *
            kRotatedXAxis;
        NiPoint3 kYComponent = (kRotatedYAxis * kDisplacementFromCenter) *
            kRotatedYAxis;

        if (kXComponent.SqrLength() < m_fHalfSqScaledWidth &&
            kYComponent.SqrLength() < m_fHalfSqScaledHeight)
        {
            // Initially colliding with plane
            m_fCollisionTime = fCollisionTime = fInitialTime;
            m_kCollisionPoint = *pkParticlePos;

            // No other collisions can occur before this one
            return this;
        }
    }
    else
    {
        float fDeltaTime = fCollisionTime - fInitialTime;
        NiPoint3 kTravelDisplacement = 
            fDeltaTime * pkCurParticle->m_kVelocity;
        float fTravelDistAlongNormal = m_kPlane.GetNormal() * 
            kTravelDisplacement;
        float fSum = fDistToPlane + fTravelDistAlongNormal;
        if (NiAbs(fSum) < gs_fEpsilon)
        {
            fSum = 0.0f;
        }
        if (fDistToPlane * fSum < 0.0f)
        {
            // Collision with plane - test rectangle
            float fTimeToPlane = -fDistToPlane * fDeltaTime / 
                fTravelDistAlongNormal;

            NIASSERT(fTimeToPlane > 0.0f && fTimeToPlane < fDeltaTime);

            NiPoint3 kCollisionPoint = *pkParticlePos + 
                fTimeToPlane * pkCurParticle->m_kVelocity;

            // Make sure collision point is on same side of plane.  Since we
            // are on the backside of the plane due to floating point 
            // precision issues, we scale by 2 ^ -21.  This gives us
            // significant precision since IEEE 754 has 23 bits of mantissa
            // but also gives a small amount of error tolerance for
            // non-standard implementations.
            static const float sfCollisionEpsilon = NiPow(2.0f, -21.0f);
            if (m_kPlane.Distance(kCollisionPoint) * fDistToPlane < 0.0f)
            {
                float fMaxValue = NiAbs(kCollisionPoint.x);
                if (NiAbs(kCollisionPoint.y) > fMaxValue)
                {
                    fMaxValue = NiAbs(kCollisionPoint.y);
                }
                if (NiAbs(kCollisionPoint.z) > fMaxValue)
                {
                    fMaxValue = NiAbs(kCollisionPoint.z);
                }

                if (fDistToPlane < 0.0f)
                {
                    kCollisionPoint -= fMaxValue * sfCollisionEpsilon *
                        m_kPlane.GetNormal();
                }
                else
                {
                    kCollisionPoint += fMaxValue * sfCollisionEpsilon *
                        m_kPlane.GetNormal();
                }
            }

            // Make sure collision point is inside rectangle
            NiPoint3 kDisplacementFromCenter = kCollisionPoint - m_kPosition;
            NiPoint3 kRotatedXAxis = m_kRotation * m_kXAxis;
            NiPoint3 kRotatedYAxis = m_kRotation * m_kYAxis;
            NiPoint3 kXComponent = (kRotatedXAxis * kDisplacementFromCenter) *
                kRotatedXAxis;
            NiPoint3 kYComponent = (kRotatedYAxis * kDisplacementFromCenter) *
                kRotatedYAxis;
            if (kXComponent.SqrLength() < m_fHalfSqScaledWidth &&
                kYComponent.SqrLength() < m_fHalfSqScaledHeight)
            {
                bCollision = true;

                m_fCollisionTime = fCollisionTime = fInitialTime +
                    fTimeToPlane;
                m_kCollisionPoint = kCollisionPoint;
            }
        }
    }

    NiPSysCollider* pkResult = NiPSysCollider::Resolve(fInitialTime,
        fCollisionTime, pkData, usCurParticle);
    if (pkResult)
    {
        // Other collision happened first.
        return pkResult;
    }

    return (bCollision ? this : NULL);
}
//---------------------------------------------------------------------------
void NiPSysPlanarCollider::Update(float fTime, NiPSysData* pkData,
    unsigned short usCurParticle)
{
    NiParticleInfo* pkCurParticle = &pkData->GetParticleInfo()[usCurParticle];

    // Update velocity.
    float fDot = pkCurParticle->m_kVelocity * m_kPlane.GetNormal();
    NiPoint3 kOffset = fDot * m_kPlane.GetNormal();

    // If initially colliding, check velocity in direction of normal.
    // If it's too low, eliminate the velocity in that direction.
    if (m_fCollisionTime == fTime && fDot < gs_fEpsilon)
    {
        pkCurParticle->m_kVelocity -= kOffset;
    }
    else
    {
        pkCurParticle->m_kVelocity -= kOffset + kOffset;
        pkCurParticle->m_kVelocity *= m_fBounce;
    }

    NiPSysCollider::Update(fTime, pkData, usCurParticle);
}
//---------------------------------------------------------------------------
void NiPSysPlanarCollider::SetResolveConstants()
{
    // Only compute m_kPlane if it has never been computed or the collider
    // object's world transform has changed.
    if (m_kPlane == NiPlane(NiPoint3::ZERO, 0.0f) || (m_pkColliderObj &&
        (m_kLastColliderTransform != m_pkColliderObj->GetWorldTransform() ||
        m_kLastPSysTransform != m_pkManager->GetSystemPointer()
        ->GetWorldTransform())))
    {
        m_kPosition = NiPoint3::ZERO;
        m_kRotation = NiMatrix3::IDENTITY;
        NiPoint3 kNormal = m_kXAxis.Cross(m_kYAxis);
        kNormal.Unitize();
        float fScaledWidth = m_fWidth;
        float fScaledHeight = m_fHeight;

        if (m_pkColliderObj)
        {
            NiTransform kCollider = m_pkColliderObj->GetWorldTransform();
            NiTransform kPSys = m_pkManager->GetSystemPointer()
                ->GetWorldTransform();
            NiTransform kInvPSys;
            kPSys.Invert(kInvPSys);
            NiTransform kColliderToPSys = kInvPSys * kCollider;

            m_kPosition = kColliderToPSys.m_Translate;
            m_kRotation = kColliderToPSys.m_Rotate;
            kNormal = m_kRotation * kNormal;
            kNormal.Unitize();
            fScaledWidth *= kColliderToPSys.m_fScale;
            fScaledHeight *= kColliderToPSys.m_fScale;

            m_kLastColliderTransform = kCollider;
            m_kLastPSysTransform = kPSys;
        }

        // Find the half width and height because we will be useing distance
        // from the plane center not the total width.
        fScaledWidth *= 0.5f;
        fScaledHeight *= 0.5f;


        m_fHalfSqScaledWidth = fScaledWidth * fScaledWidth;
        m_fHalfSqScaledHeight = fScaledHeight * fScaledHeight;
        m_kPlane = NiPlane(kNormal, m_kPosition);
    }

    // Set the Resolve constants on the next collider
    if (m_spNext)
        m_spNext->SetResolveConstants();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysPlanarCollider);
//---------------------------------------------------------------------------
void NiPSysPlanarCollider::CopyMembers(NiPSysPlanarCollider* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysCollider::CopyMembers(pkDest, kCloning);

    pkDest->m_fWidth = m_fWidth;
    pkDest->m_fHeight = m_fHeight;
    pkDest->m_kXAxis = m_kXAxis;
    pkDest->m_kYAxis = m_kYAxis;
}
//---------------------------------------------------------------------------
void NiPSysPlanarCollider::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysCollider::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);
    NiPSysPlanarCollider* pkDest = (NiPSysPlanarCollider*) pkClone;

    if (m_pkColliderObj)
    {
        bCloned = kCloning.m_pkCloneMap->GetAt(m_pkColliderObj, pkClone);
        if (bCloned)
        {
            pkDest->m_pkColliderObj = (NiAVObject*) pkClone;
        }
        else
        {
            pkDest->m_pkColliderObj = m_pkColliderObj;
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysPlanarCollider);
//---------------------------------------------------------------------------
void NiPSysPlanarCollider::LoadBinary(NiStream& kStream)
{
    NiPSysCollider::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_pkColliderObj
    NiStreamLoadBinary(kStream, m_fWidth);
    NiStreamLoadBinary(kStream, m_fHeight);
    m_kXAxis.LoadBinary(kStream);
    m_kYAxis.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysPlanarCollider::LinkObject(NiStream& kStream)
{
    NiPSysCollider::LinkObject(kStream);

    m_pkColliderObj = (NiAVObject*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPSysPlanarCollider::RegisterStreamables(NiStream& kStream)
{
    return NiPSysCollider::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysPlanarCollider::SaveBinary(NiStream& kStream)
{
    NiPSysCollider::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkColliderObj);
    NiStreamSaveBinary(kStream, m_fWidth);
    NiStreamSaveBinary(kStream, m_fHeight);
    m_kXAxis.SaveBinary(kStream);
    m_kYAxis.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysPlanarCollider::IsEqual(NiObject* pkObject)
{
    if (!NiPSysCollider::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysPlanarCollider* pkDest = (NiPSysPlanarCollider*) pkObject;

    if ((m_pkColliderObj && !pkDest->m_pkColliderObj) ||
        (!m_pkColliderObj && pkDest->m_pkColliderObj) ||
        (m_pkColliderObj && pkDest->m_pkColliderObj &&
            !m_pkColliderObj->IsEqual(pkDest->m_pkColliderObj)))
    {
        return false;
    }

    if (m_fWidth != pkDest->m_fWidth ||
        m_fHeight != pkDest->m_fHeight ||
        m_kXAxis != pkDest->m_kXAxis ||
        m_kYAxis != pkDest->m_kYAxis)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysPlanarCollider::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysCollider::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysPlanarCollider::ms_RTTI.GetName()));

    const char* pcColliderObj;
    if (m_pkColliderObj)
    {
        pcColliderObj = m_pkColliderObj->GetName();
    }
    else
    {
        pcColliderObj = "None";
    }
    pkStrings->Add(NiGetViewerString("Collider Object", pcColliderObj));
    pkStrings->Add(NiGetViewerString("Width", m_fWidth));
    pkStrings->Add(NiGetViewerString("Height", m_fHeight));
    pkStrings->Add(m_kXAxis.GetViewerString("X-Axis"));
    pkStrings->Add(m_kYAxis.GetViewerString("Y-Axis"));
}
//---------------------------------------------------------------------------
