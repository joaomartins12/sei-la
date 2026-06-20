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

#include "NiPSysGravityModifier.h"
#include "NiParticleSystem.h"
#include <NiCloningProcess.h>

NiImplementRTTI(NiPSysGravityModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysGravityModifier::NiPSysGravityModifier(const char* pcName,
    NiAVObject* pkGravityObj, NiPoint3 kGravityAxis, float fDecay,
    float fStrength, ForceType eType, float fTurbulence, float fScale) : 
    NiPSysModifier(pcName, ORDER_FORCE),
    m_pkGravityObj(pkGravityObj), m_kGravityAxis(kGravityAxis),
    m_fDecay(fDecay), m_fStrength(fStrength), m_eType(eType),
    m_fTurbulence(fTurbulence), m_fScale(fScale)
{
}
//---------------------------------------------------------------------------
NiPSysGravityModifier::NiPSysGravityModifier() :
    m_pkGravityObj(NULL), m_kGravityAxis(NiPoint3::UNIT_X), m_fDecay(0.0f),
    m_fStrength(1.0f), m_eType(FORCE_PLANAR),m_fTurbulence(0.0f),
    m_fScale(1.0f)
{
}
//---------------------------------------------------------------------------
void NiPSysGravityModifier::Update(float fTime, NiPSysData* pkData)
{

    if ((pkData->GetNumParticles() == 0) || (!m_pkGravityObj))
        return;


    NiPoint3 kDirection = m_kGravityAxis;
    NiTransform kGravity = m_pkGravityObj->GetWorldTransform();
    NiTransform kPSys = m_pkTarget->GetWorldTransform();
    NiTransform kInvPSys;
    kPSys.Invert(kInvPSys);

    NiTransform kGravityToPSys = kInvPSys * kGravity;
    
    NiPoint3 kPosition = kGravityToPSys.m_Translate;
    kDirection = kGravityToPSys.m_Rotate * m_kGravityAxis;
    kDirection.Unitize();


    float fStrength = m_fStrength * 1.6f;
    const unsigned short usNumParticles = pkData->GetNumParticles();
    unsigned short us;

    switch (m_eType)
    {
    case FORCE_PLANAR:
        {

            // No Decay No Turbulence
            if ((m_fDecay == 0.0f) && (m_fTurbulence == 0.0f))
            {
                NiParticleInfo* pkCurrentParticle = pkData->GetParticleInfo();
                
                for (us = 0; us < usNumParticles; us++, pkCurrentParticle++)
                {
                    pkCurrentParticle->m_kVelocity += kDirection *
                        (fStrength * (fTime -
                        pkCurrentParticle->m_fLastUpdate));
                }
            }

            // No Decay Turbulence
            else if ((m_fDecay == 0.0f) && (m_fTurbulence != 0.0f))
            {
                NiParticleInfo* pkCurrentParticle = pkData->GetParticleInfo();
                NiPoint3 kTurbulence;
                
                float fTurbulenceScale = m_fTurbulence * m_fScale * 500.0f;

                for (us = 0; us < usNumParticles; us++, pkCurrentParticle++)
                {
            
                    kTurbulence.x = NiSymmetricRandom() * fTurbulenceScale;
                    kTurbulence.y = NiSymmetricRandom() * fTurbulenceScale;
                    kTurbulence.z = NiSymmetricRandom() * fTurbulenceScale;


                    pkCurrentParticle->m_kVelocity += (kTurbulence +
                        (kDirection * fStrength)) * (fTime -
                        pkCurrentParticle->m_fLastUpdate);
                }
            }

            // Decay No Turbulence
            else if ((m_fDecay != 0.0f) && (m_fTurbulence == 0.0f))
            {

                NiParticleInfo* pkCurrentParticle = pkData->GetParticleInfo();
                NiPoint3* pkParticlePosition = pkData->GetVertices();
                float fDecay;
                
                for (us = 0; us < usNumParticles; us++, pkCurrentParticle++,
                    pkParticlePosition++)
                {
                    NiPoint3 kDir = kPosition - *pkParticlePosition;

                    float fDist = kDirection * kDir;
                    if (fDist < 0.0f)
                        fDecay = NiExp(m_fDecay * fDist);
                    else
                        fDecay = NiExp(-m_fDecay * fDist);
                    
                    pkCurrentParticle->m_kVelocity += kDirection * 
                        (fStrength * fDecay * (fTime - 
                        pkCurrentParticle->m_fLastUpdate));
                }

            }
            // Decay And Turbulence
            else if ((m_fDecay != 0.0f) && (m_fTurbulence != 0.0f))
            {

                NiParticleInfo* pkCurrentParticle = pkData->GetParticleInfo();
                NiPoint3* pkParticlePosition = pkData->GetVertices();
                
                NiPoint3 kTurbulence;
                
                float fTurbulenceScale = m_fTurbulence * m_fScale * 500.0f;
                float fDecay;

                for (us = 0; us < usNumParticles; us++, pkCurrentParticle++,
                    pkParticlePosition++)
                {
            
                    kTurbulence.x = NiSymmetricRandom() * fTurbulenceScale;
                    kTurbulence.y = NiSymmetricRandom() * fTurbulenceScale;
                    kTurbulence.z = NiSymmetricRandom() * fTurbulenceScale;

                    NiPoint3 kDir = kPosition - *pkParticlePosition;

                    float fDist = kDirection * kDir;
                    if (fDist < 0.0f)
                        fDecay = NiExp(m_fDecay * fDist);
                    else
                        fDecay = NiExp(-m_fDecay * fDist);
                    
                    pkCurrentParticle->m_kVelocity += (kTurbulence +
                        (kDirection * fStrength * fDecay)) * (fTime -
                        pkCurrentParticle->m_fLastUpdate);
                }

            }

            break;
        }
    case FORCE_SPHERICAL:
        {

            // No Decay No Turbulence
            if ((m_fDecay == 0.0f) && (m_fTurbulence == 0.0f))
            {
                NiParticleInfo* pkCurrentParticle = pkData->GetParticleInfo();
                NiPoint3* pkParticlePosition = pkData->GetVertices();
                
                for (us = 0; us < usNumParticles; us++, pkCurrentParticle++,
                    pkParticlePosition++)
                {
                    NiPoint3 kDir = kPosition - *pkParticlePosition;
                    kDir.Unitize();

                    pkCurrentParticle->m_kVelocity += kDir * (fStrength *
                        (fTime - pkCurrentParticle->m_fLastUpdate));
                }
            }

            // No Decay Turbulence
            else if ((m_fDecay == 0.0f) && (m_fTurbulence != 0.0f))
            {
                NiParticleInfo* pkCurrentParticle = pkData->GetParticleInfo();
                NiPoint3* pkParticlePosition = pkData->GetVertices();
                NiPoint3 kTurbulence;
                
                float fTurbulenceScale = m_fTurbulence * m_fScale * 500.0f;

                for (us = 0; us < usNumParticles; us++, pkCurrentParticle++,
                    pkParticlePosition++)
                {
                    NiPoint3 kDir = kPosition - *pkParticlePosition;
                    kDirection.Unitize();
            
                    kTurbulence.x = NiSymmetricRandom() * fTurbulenceScale;
                    kTurbulence.y = NiSymmetricRandom() * fTurbulenceScale;
                    kTurbulence.z = NiSymmetricRandom() * fTurbulenceScale;


                    pkCurrentParticle->m_kVelocity += (kTurbulence + (kDir *
                        fStrength)) * (fTime -
                        pkCurrentParticle->m_fLastUpdate);
                }
            }

            // Decay No Turbulence
            else if ((m_fDecay != 0.0f) && (m_fTurbulence == 0.0f))
            {

                NiParticleInfo* pkCurrentParticle = pkData->GetParticleInfo();
                NiPoint3* pkParticlePosition = pkData->GetVertices();
                float fDecay;
                
                for (us = 0; us < usNumParticles; us++, pkCurrentParticle++,
                    pkParticlePosition++)
                {
                    NiPoint3 kDir = kPosition - *pkParticlePosition;
                    float fDist = kDir.Unitize();

                    fDecay = NiExp(-m_fDecay * fDist);
                    
                    pkCurrentParticle->m_kVelocity += kDir * (fStrength *
                        fDecay * (fTime - pkCurrentParticle->m_fLastUpdate));
                }

            }
            // Decay And Turbulence
            else if ((m_fDecay != 0.0f) && (m_fTurbulence != 0.0f))
            {

                NiParticleInfo* pkCurrentParticle = pkData->GetParticleInfo();
                NiPoint3* pkParticlePosition = pkData->GetVertices();
                
                NiPoint3 kTurbulence;
                
                float fTurbulenceScale = m_fTurbulence * m_fScale * 500.0f;
                float fDecay;

                for (us = 0; us < usNumParticles; us++, pkCurrentParticle++,
                    pkParticlePosition++)
                {
            
                    kTurbulence.x = NiSymmetricRandom() * fTurbulenceScale;
                    kTurbulence.y = NiSymmetricRandom() * fTurbulenceScale;
                    kTurbulence.z = NiSymmetricRandom() * fTurbulenceScale;

                    NiPoint3 kDir = kPosition - *pkParticlePosition;
                    float fDist = kDir.Unitize();

                    fDecay = NiExp(-m_fDecay * fDist);
                    
                    pkCurrentParticle->m_kVelocity += (kTurbulence + 
                        (kDirection * fStrength * fDecay)) * (fTime -
                        pkCurrentParticle->m_fLastUpdate);
                }

            }
            
            break;
        }
    default:
        {
            // Unknown force type.
            NIASSERT(false);
            break;
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysGravityModifier);
//---------------------------------------------------------------------------
void NiPSysGravityModifier::CopyMembers(NiPSysGravityModifier* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifier::CopyMembers(pkDest, kCloning);

    pkDest->m_kGravityAxis = m_kGravityAxis;
    pkDest->m_fDecay = m_fDecay;
    pkDest->m_fStrength = m_fStrength;
    pkDest->m_eType = m_eType;
    pkDest->m_fTurbulence = m_fTurbulence;
    pkDest->m_fScale = m_fScale;
}
//---------------------------------------------------------------------------
void NiPSysGravityModifier::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysModifier::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);
    NiPSysGravityModifier* pkDest = (NiPSysGravityModifier*) pkClone;

    if (m_pkGravityObj)
    {
        bCloned = kCloning.m_pkCloneMap->GetAt(m_pkGravityObj, pkClone);
        if (bCloned)
        {
            NIASSERT(m_pkGravityObj->GetRTTI() == pkClone->GetRTTI());
            pkDest->m_pkGravityObj = (NiAVObject*) pkClone;
        }
        else
        {
            pkDest->m_pkGravityObj = m_pkGravityObj;
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysGravityModifier);
//---------------------------------------------------------------------------
void NiPSysGravityModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_pkGravityObj
    m_kGravityAxis.LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_fDecay);
    NiStreamLoadBinary(kStream, m_fStrength);
    NiStreamLoadEnum(kStream, m_eType);

    NiStreamLoadBinary(kStream, m_fTurbulence);
    NiStreamLoadBinary(kStream, m_fScale);
}
//---------------------------------------------------------------------------
void NiPSysGravityModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);

    m_pkGravityObj = (NiAVObject*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPSysGravityModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysGravityModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkGravityObj);
    m_kGravityAxis.SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_fDecay);
    NiStreamSaveBinary(kStream, m_fStrength);
    NiStreamSaveEnum(kStream, m_eType);
    NiStreamSaveBinary(kStream, m_fTurbulence);
    NiStreamSaveBinary(kStream, m_fScale);
}
//---------------------------------------------------------------------------
bool NiPSysGravityModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysGravityModifier* pkDest = (NiPSysGravityModifier*) pkObject;

    if ((m_pkGravityObj && !pkDest->m_pkGravityObj) ||
        (!m_pkGravityObj && pkDest->m_pkGravityObj) ||
        (m_pkGravityObj && pkDest->m_pkGravityObj &&
            !m_pkGravityObj->IsEqual(pkDest->m_pkGravityObj)))
    {
        return false;
    }

    if (pkDest->m_kGravityAxis != m_kGravityAxis ||
        pkDest->m_fDecay != m_fDecay ||
        pkDest->m_fStrength != m_fStrength ||
        pkDest->m_eType != m_eType ||
        pkDest->m_fTurbulence != m_fTurbulence ||
        pkDest->m_fScale != m_fScale)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysGravityModifier::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiPSysGravityModifier::ms_RTTI.GetName()));

    const char* pcGravityObj;
    if (m_pkGravityObj)
    {
        pcGravityObj = m_pkGravityObj->GetName();
    }
    else
    {
        pcGravityObj = "None";
    }
    pkStrings->Add(NiGetViewerString("Gravity Object", pcGravityObj));
    pkStrings->Add(m_kGravityAxis.GetViewerString("Gravity Axis"));
    pkStrings->Add(NiGetViewerString("Decay", m_fDecay));
    pkStrings->Add(NiGetViewerString("Strength", m_fStrength));
    pkStrings->Add(NiGetViewerString("Turbulence", m_fTurbulence));
    pkStrings->Add(NiGetViewerString("TurbulenceScale", m_fScale));
    const char* pcForceType = NULL;
    if (m_eType == FORCE_PLANAR)
    {
        pcForceType = "FORCE_PLANAR";
    }
    else if (m_eType == FORCE_SPHERICAL)
    {
        pcForceType = "FORCE_SPHERICAL";
    }
    else
    {
        pcForceType = "Unknown";
    }
    pkStrings->Add(NiGetViewerString("Force Type", pcForceType));
}
//---------------------------------------------------------------------------
