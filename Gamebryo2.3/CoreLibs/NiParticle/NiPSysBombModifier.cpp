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

#include "NiPSysBombModifier.h"
#include "NiParticleSystem.h"
#include <NiCloningProcess.h>

NiImplementRTTI(NiPSysBombModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysBombModifier::NiPSysBombModifier(const char* pcName,
    NiAVObject* pkBombObj, NiPoint3 kBombAxis, float fDecay, float fDeltaV,
    DecayType eDecayType, SymmType eSymmType) : NiPSysModifier(pcName,
    ORDER_FORCE), m_pkBombObj(pkBombObj), m_kBombAxis(kBombAxis),
    m_fDecay(fDecay), m_fDeltaV(fDeltaV), m_eDecayType(eDecayType),
    m_eSymmType(eSymmType)
{
}
//---------------------------------------------------------------------------
NiPSysBombModifier::NiPSysBombModifier() : m_pkBombObj(NULL),
    m_kBombAxis(NiPoint3::UNIT_X), m_fDecay(0.0f), m_fDeltaV(0.0f),
    m_eDecayType(NONE), m_eSymmType(SPHERICAL)
{
}
//---------------------------------------------------------------------------
void NiPSysBombModifier::Update(float fTime, NiPSysData* pkData)
{
    NiPoint3 kPosition = NiPoint3::ZERO;
    NiPoint3 kDirection = m_kBombAxis;
    if (pkData->GetNumParticles() > 0 && m_pkBombObj)
    {
        NiTransform kBomb = m_pkBombObj->GetWorldTransform();
        NiTransform kPSys = m_pkTarget->GetWorldTransform();
        NiTransform kInvPSys;
        kPSys.Invert(kInvPSys);
        NiTransform kBombToPSys = kInvPSys * kBomb;

        kPosition = kBombToPSys.m_Translate;
        kDirection = kBombToPSys.m_Rotate * m_kBombAxis;
        kDirection.Unitize();
    }

    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        NiParticleInfo* pkCurrentParticle = &pkData->GetParticleInfo()[us];

        float fStart = pkCurrentParticle->m_fLastUpdate;
        float fEnd = fTime;

        if (fEnd > fStart)
        {
            float fActiveTime = fEnd - fStart;

            NiPoint3 kDir = pkData->GetVertices()[us] - kPosition;
            float fDist = kDir.Length();
            NiPoint3 kForce;            
            if (m_eDecayType == NONE || (fDist <= m_fDecay))
            {
                switch (m_eSymmType)
                {
                    case SPHERICAL:
                        kForce = kDir / fDist;
                        break;
                    case PLANAR:
                        fDist = kDirection * kDir;
                        if (fDist < 0.0f)
                        {
                            fDist = -fDist;
                            kForce = -kDirection;
                        }
                        else
                        {
                            kForce = kDirection;
                        }
                        break;
                    case CYLINDRICAL:
                        // Subtract off the vector component along the
                        // cylinder axis
                        fDist = kDirection * kDir;

                        kForce = kDir - fDist * kDirection;

                        fDist = kForce.Length();
                        if (fDist != 0.0f)
                        {
                            kForce /= fDist;
                        }
                        break;
                    default:
                        NIASSERT(false);
                        break;
                }

                float fDecay = 1.0f;
                if (m_eDecayType == LINEAR)
                {
                    fDecay = (m_fDecay - fDist) / m_fDecay;
                }
                else if (m_eDecayType == EXPONENTIAL)
                {
                    fDecay = NiExp(-fDist / m_fDecay);
                }

                // we modify the velocity by multiplying the following;
                // 1) fDecay - a multiplier between 0 and 1 based off of
                //  how far the particle is from the bomb
                // 2) kForce - a vector representing the direction of force
                //  this is a unit vector.
                // 3) m_fDeltaV - the strength value exported from the modeling
                //  package.  This value is in units of mass / time^2
                // 4) fActiveTime - the change in time from the last update
                pkCurrentParticle->m_kVelocity += 
                    fDecay * m_fDeltaV * fActiveTime * kForce;
            }
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysBombModifier);
//---------------------------------------------------------------------------
void NiPSysBombModifier::CopyMembers(NiPSysBombModifier* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifier::CopyMembers(pkDest, kCloning);

    pkDest->m_kBombAxis = m_kBombAxis;
    pkDest->m_fDecay = m_fDecay;
    pkDest->m_fDeltaV = m_fDeltaV;
    pkDest->m_eDecayType = m_eDecayType;
    pkDest->m_eSymmType = m_eSymmType;
}
//---------------------------------------------------------------------------
void NiPSysBombModifier::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysModifier::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);
    NiPSysBombModifier* pkDest = (NiPSysBombModifier*) pkClone;

    bCloned = kCloning.m_pkCloneMap->GetAt(m_pkBombObj, pkClone);
    if (bCloned)
    {
        pkDest->m_pkBombObj = (NiAVObject*) pkClone;
    }
    else
    {
        pkDest->m_pkBombObj = m_pkBombObj;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysBombModifier);
//---------------------------------------------------------------------------
void NiPSysBombModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_pkBombObj
    m_kBombAxis.LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_fDecay);
    NiStreamLoadBinary(kStream, m_fDeltaV);
    NiStreamLoadEnum(kStream, m_eDecayType);
    NiStreamLoadEnum(kStream, m_eSymmType);
}
//---------------------------------------------------------------------------
void NiPSysBombModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);

    m_pkBombObj = (NiAVObject*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPSysBombModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysBombModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkBombObj);
    m_kBombAxis.SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_fDecay);
    NiStreamSaveBinary(kStream, m_fDeltaV);
    NiStreamSaveEnum(kStream, m_eDecayType);
    NiStreamSaveEnum(kStream, m_eSymmType);
}
//---------------------------------------------------------------------------
bool NiPSysBombModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysBombModifier* pkDest = (NiPSysBombModifier*) pkObject;

    if ((m_pkBombObj && !pkDest->m_pkBombObj) ||
        (!m_pkBombObj && pkDest->m_pkBombObj) ||
        (m_pkBombObj && pkDest->m_pkBombObj &&
            !m_pkBombObj->IsEqual(pkDest->m_pkBombObj)))
    {
        return false;
    }

    if (m_kBombAxis != pkDest->m_kBombAxis ||
        m_fDecay != pkDest->m_fDecay ||
        m_fDeltaV != pkDest->m_fDeltaV ||
        m_eDecayType != pkDest->m_eDecayType ||
        m_eSymmType != pkDest->m_eSymmType)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysBombModifier::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysBombModifier::ms_RTTI.GetName()));

    const char* pcBombObj;
    if (m_pkBombObj)
    {
        pcBombObj = m_pkBombObj->GetName();
    }
    else
    {
        pcBombObj = "None";
    }
    pkStrings->Add(NiGetViewerString("Bomb Object", pcBombObj));
    pkStrings->Add(m_kBombAxis.GetViewerString("Bomb Axis"));
    pkStrings->Add(NiGetViewerString("Decay", m_fDecay));
    pkStrings->Add(NiGetViewerString("DeltaV", m_fDeltaV));
    const char* pcDecayType;
    switch (m_eDecayType)
    {
        case NONE:
            pcDecayType = "NONE";
            break;
        case LINEAR:
            pcDecayType = "LINEAR";
            break;
        case EXPONENTIAL:
            pcDecayType = "EXPONENTIAL";
            break;
        default:
            pcDecayType = "Unknown";
            break;
    }
    pkStrings->Add(NiGetViewerString("Decay Type", pcDecayType));
    const char* pcSymmType;
    switch (m_eSymmType)
    {
        case SPHERICAL:
            pcSymmType = "SPHERICAL";
            break;
        case CYLINDRICAL:
            pcSymmType = "CYLINDRICAL";
            break;
        case PLANAR:
            pcSymmType = "PLANAR";
            break;
        default:
            pcSymmType = "Unknown";
            break;
    }
    pkStrings->Add(NiGetViewerString("Symmetry Type", pcSymmType));
}
//---------------------------------------------------------------------------
