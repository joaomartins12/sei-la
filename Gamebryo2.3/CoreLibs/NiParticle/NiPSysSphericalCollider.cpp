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

#include "NiPSysSphericalCollider.h"
#include <NiAVObject.h>
#include <NiPSysColliderManager.h>
#include <NiParticleSystem.h>
#include <NiCloningProcess.h>

NiImplementRTTI(NiPSysSphericalCollider, NiPSysCollider);

static const float gs_fEpsilon = 0.001f;

//---------------------------------------------------------------------------
NiPSysSphericalCollider::NiPSysSphericalCollider(float fBounce,
    bool bSpawnOnCollide, bool bDieOnCollide,
    NiPSysSpawnModifier* pkSpawnModifier, NiAVObject* pkColliderObj,
    float fRadius) : NiPSysCollider(fBounce, bSpawnOnCollide, bDieOnCollide,
    pkSpawnModifier), m_pkColliderObj(pkColliderObj), m_fRadius(1.0f),
    m_fRadiusSq(1.0f), m_kPosition(NiPoint3::ZERO)
{
    SetRadius(fRadius);

    // Initialize last transforms to invalid values.
    m_kLastColliderTransform.m_Translate = m_kLastPSysTransform.m_Translate =
        NiPoint3(-NI_INFINITY, -NI_INFINITY, -NI_INFINITY);
    m_kLastColliderTransform.m_Rotate = m_kLastPSysTransform.m_Rotate =
        NiMatrix3::ZERO;
    m_kLastColliderTransform.m_fScale = m_kLastPSysTransform.m_fScale =
        -NI_INFINITY;
}
//---------------------------------------------------------------------------
NiPSysCollider* NiPSysSphericalCollider::Resolve(float fInitialTime,
    float& fCollisionTime, NiPSysData* pkData, unsigned short usCurParticle)
{
    bool bCollision = false;

 
    NiPoint3* pkParticlePos = &pkData->GetVertices()[usCurParticle];
    NiParticleInfo* pkCurParticle = &pkData->GetParticleInfo()[usCurParticle];

    // Check for initial position
    NiPoint3 kCurrentDisplacement = *pkParticlePos - m_kPosition;
    float fCurrentDistSq = kCurrentDisplacement.SqrLength();
    float fDiff = fCurrentDistSq - m_fRadiusSq;
    bool bInsideSphere = -fDiff > gs_fEpsilon;
    if (bInsideSphere || fDiff > gs_fEpsilon)
    {
        // Initially inside or outside sphere
        float fTime = -(pkCurParticle->m_kVelocity * kCurrentDisplacement);

        if (bInsideSphere || fTime > 0)
        {
            float fSpeedSq = pkCurParticle->m_kVelocity.SqrLength();
            fTime /= fSpeedSq;

            NiPoint3 kClosestDisplacement = *pkParticlePos + 
                fTime * pkCurParticle->m_kVelocity - m_kPosition;

            float fClosestDistSq = kClosestDisplacement.SqrLength();
            if (bInsideSphere || fClosestDistSq < m_fRadiusSq)
            {
                float fHalfChordDistSq = m_fRadiusSq - fClosestDistSq;

                if (bInsideSphere)
                    fTime += NiSqrt(fHalfChordDistSq / fSpeedSq);
                else
                    fTime -= NiSqrt(fHalfChordDistSq / fSpeedSq);

                if (fTime < fCollisionTime - fInitialTime)
                {
                    bCollision = true;
                    m_fCollisionTime = fCollisionTime = fInitialTime + fTime;

                    // Calculate collision Point
                    m_kCollisionPoint = *pkParticlePos +
                        fTime * pkCurParticle->m_kVelocity;

                }
            }
        }
    }
    else
    {
        // Do nothing - particle was likely spawned here
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
void NiPSysSphericalCollider::Update(float fTime, NiPSysData* pkData,
    unsigned short usCurParticle)
{
    NiParticleInfo* pkCurParticle = &pkData->GetParticleInfo()[usCurParticle];

    // Update velocity.
    NiPoint3 kNormal = (m_kCollisionPoint - m_kPosition) / m_fScaledRadius;

    float fDot = pkCurParticle->m_kVelocity * kNormal;
    NiPoint3 kOffset = fDot * kNormal;
    pkCurParticle->m_kVelocity -= kOffset + kOffset;
    pkCurParticle->m_kVelocity *= m_fBounce;

    NiPSysCollider::Update(fTime, pkData, usCurParticle);
}
//---------------------------------------------------------------------------
void NiPSysSphericalCollider::SetResolveConstants()
{
    // Only compute variables if the collider object's world transform has
    // changed.
    if (m_kLastColliderTransform != m_pkColliderObj->GetWorldTransform() ||
        m_kLastPSysTransform != m_pkManager->GetSystemPointer()
        ->GetWorldTransform())
    {
        NiTransform kCollider;
        if (m_pkColliderObj)
        {
            kCollider = m_pkColliderObj->GetWorldTransform();
        }
        else
        {
            kCollider.MakeIdentity();
        }

        NiTransform kPSys = m_pkManager->GetSystemPointer()
            ->GetWorldTransform();
        NiTransform kInvPSys;
        kPSys.Invert(kInvPSys);
        NiTransform kColliderToPSys = kInvPSys * kCollider;

        m_kPosition = kColliderToPSys.m_Translate;
        m_fScaledRadius = kColliderToPSys.m_fScale * m_fRadius;
        m_fRadiusSq = m_fScaledRadius * m_fScaledRadius;

        m_kLastPSysTransform = kPSys;
        m_kLastColliderTransform = kCollider;
    }

    // Set the Resolve constants on the next collider
    if (m_spNext)
        m_spNext->SetResolveConstants();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysSphericalCollider);
//---------------------------------------------------------------------------
void NiPSysSphericalCollider::CopyMembers(NiPSysSphericalCollider* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysCollider::CopyMembers(pkDest, kCloning);

    pkDest->m_fRadius = m_fRadius;
}
//---------------------------------------------------------------------------
void NiPSysSphericalCollider::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysCollider::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);
    NiPSysSphericalCollider* pkDest = (NiPSysSphericalCollider*) pkClone;

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
NiImplementCreateObject(NiPSysSphericalCollider);
//---------------------------------------------------------------------------
void NiPSysSphericalCollider::LoadBinary(NiStream& kStream)
{
    NiPSysCollider::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_pkColliderObj
    NiStreamLoadBinary(kStream, m_fRadius);
}
//---------------------------------------------------------------------------
void NiPSysSphericalCollider::LinkObject(NiStream& kStream)
{
    NiPSysCollider::LinkObject(kStream);

    m_pkColliderObj = (NiAVObject*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPSysSphericalCollider::RegisterStreamables(NiStream& kStream)
{
    return NiPSysCollider::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysSphericalCollider::SaveBinary(NiStream& kStream)
{
    NiPSysCollider::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkColliderObj);
    NiStreamSaveBinary(kStream, m_fRadius);
}
//---------------------------------------------------------------------------
bool NiPSysSphericalCollider::IsEqual(NiObject* pkObject)
{
    if (!NiPSysCollider::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysSphericalCollider* pkDest = (NiPSysSphericalCollider*) pkObject;

    if ((m_pkColliderObj && !pkDest->m_pkColliderObj) ||
        (!m_pkColliderObj && pkDest->m_pkColliderObj) ||
        (m_pkColliderObj && pkDest->m_pkColliderObj &&
            !m_pkColliderObj->IsEqual(pkDest->m_pkColliderObj)))
    {
        return false;
    }

    if (m_fRadius != pkDest->m_fRadius)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysSphericalCollider::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysCollider::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysSphericalCollider::ms_RTTI
        .GetName()));

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
    pkStrings->Add(NiGetViewerString("Radius", m_fRadius));
}
//---------------------------------------------------------------------------
