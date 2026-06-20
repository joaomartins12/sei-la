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

#include "NiPSysRotationModifier.h"
#include "NiParticleSystem.h"

NiImplementRTTI(NiPSysRotationModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysRotationModifier::NiPSysRotationModifier(const char* pcName,
    float fInitialRotSpeed, float fInitialRotSpeedVar,
    bool bRandomRotSpeedSign, float fInitialRotAngle,
    float fInitialRotAngleVar, bool bRandomInitialAxis, NiPoint3 kInitialAxis)
    : NiPSysModifier(pcName, ORDER_GENERAL),
    m_fInitialRotSpeed(fInitialRotSpeed),
    m_fInitialRotSpeedVar(fInitialRotSpeedVar),
    m_fInitialRotAngle(fInitialRotAngle),
    m_fInitialRotAngleVar(fInitialRotAngleVar),
    m_kInitialAxis(kInitialAxis), m_bRandomInitialAxis(bRandomInitialAxis),
    m_bRandomRotSpeedSign(bRandomRotSpeedSign)
{
}
//---------------------------------------------------------------------------
NiPSysRotationModifier::NiPSysRotationModifier() :
    m_fInitialRotSpeed(0.0f), m_fInitialRotSpeedVar(0.0f),
    m_fInitialRotAngle(0.0f), m_fInitialRotAngleVar(0.0f), 
    m_kInitialAxis(NiPoint3::UNIT_X), m_bRandomInitialAxis(true),
    m_bRandomRotSpeedSign(false)
{
}
//---------------------------------------------------------------------------
void NiPSysRotationModifier::Update(float fTime, NiPSysData* pkData)
{
    float* pfRotationAngles = pkData->GetRotationAngles();
    if (pfRotationAngles)
    {
        const float* pfRotationSpeeds = pkData->GetRotationSpeeds();
        NIASSERT(pfRotationSpeeds);

        for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
        {
            NiParticleInfo* pkCurrentParticle = &pkData->GetParticleInfo()[us];

            float& fRotationAngle = pfRotationAngles[us];

            float fDeltaTime = fTime - pkCurrentParticle->m_fLastUpdate;
            fRotationAngle += fDeltaTime * pfRotationSpeeds[us];

            // Make sure we don't accidentally get in long loop
            static const float fTenPi = 10.0f * NI_PI;
            if (fRotationAngle > fTenPi)
            {
                fRotationAngle = 0.0f;
            }
            else
            {
                while (fRotationAngle > NI_TWO_PI)
                {
                    fRotationAngle -= NI_TWO_PI;
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPSysRotationModifier::Initialize(NiPSysData* pkData,
    unsigned short usNewParticle)
{
    if (pkData->GetRotationAxes())
    {
        NiPoint3& kRotationAxis = pkData->GetRotationAxes()[usNewParticle];
        
        if (m_bRandomInitialAxis)
        {
            // This is the most random axis - is it most efficient?
            float fPhi = NiUnitRandom() * NI_PI;
            float fZ = NiCos(fPhi);
            float fHypot = NiSqrt(1.0f - fZ * fZ);
            float fTheta = NiUnitRandom() * NI_TWO_PI;
            kRotationAxis.x = fHypot * NiCos(fTheta);
            kRotationAxis.y = fHypot * NiSin(fTheta);
            kRotationAxis.z = fZ;
        }
        else
        {
            kRotationAxis = m_kInitialAxis;
        }
    }

    if (pkData->GetRotationAngles())
    {
        pkData->GetRotationAngles()[usNewParticle] = m_fInitialRotAngle +
            m_fInitialRotAngleVar * NiSymmetricRandom();
        
        float fInitialRotSpeed = m_fInitialRotSpeed + m_fInitialRotSpeedVar *
            NiSymmetricRandom();
        if (m_bRandomRotSpeedSign)
        {
            fInitialRotSpeed = (NiUnitRandom() > 0.5f) ? fInitialRotSpeed :
                -fInitialRotSpeed;
        }
        NIASSERT(pkData->GetRotationSpeeds());
        pkData->GetRotationSpeeds()[usNewParticle] = fInitialRotSpeed;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysRotationModifier);
//---------------------------------------------------------------------------
void NiPSysRotationModifier::CopyMembers(NiPSysRotationModifier* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifier::CopyMembers(pkDest, kCloning);

    pkDest->m_fInitialRotSpeed = m_fInitialRotSpeed;
    pkDest->m_fInitialRotSpeedVar = m_fInitialRotSpeedVar;
    pkDest->m_bRandomRotSpeedSign = m_bRandomRotSpeedSign;
    pkDest->m_fInitialRotAngle = m_fInitialRotAngle;
    pkDest->m_fInitialRotAngleVar = m_fInitialRotAngleVar;
    pkDest->m_bRandomInitialAxis = m_bRandomInitialAxis;
    pkDest->m_kInitialAxis = m_kInitialAxis;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysRotationModifier);
//---------------------------------------------------------------------------
void NiPSysRotationModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fInitialRotSpeed);

    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 0, 0, 2))
    {
        NiStreamLoadBinary(kStream, m_fInitialRotSpeedVar);
        NiStreamLoadBinary(kStream, m_fInitialRotAngle);
        NiStreamLoadBinary(kStream, m_fInitialRotAngleVar);

        NiBool bRandomRotSpeedSign;
        NiStreamLoadBinary(kStream, bRandomRotSpeedSign);
        m_bRandomRotSpeedSign = (bRandomRotSpeedSign != 0);
    }

    NiBool bRandomInitialAxis;
    NiStreamLoadBinary(kStream, bRandomInitialAxis);
    m_bRandomInitialAxis = (bRandomInitialAxis != 0);
    m_kInitialAxis.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysRotationModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
void NiPSysRotationModifier::PostLinkObject(NiStream& kStream)
{
    NiPSysModifier::PostLinkObject(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 0, 0, 2))
    {
        // Set the rotation speed for all active particles.
        float* pfRotationSpeeds = ((NiPSysData*)
            m_pkTarget->GetModelData())->GetRotationSpeeds();
        if (pfRotationSpeeds)
        {
            for (unsigned short us = 0; us < m_pkTarget->GetNumParticles();
                us++)
            {
                pfRotationSpeeds[us] = m_fInitialRotSpeed;
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiPSysRotationModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysRotationModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fInitialRotSpeed);
    NiStreamSaveBinary(kStream, m_fInitialRotSpeedVar);
    NiStreamSaveBinary(kStream, m_fInitialRotAngle);
    NiStreamSaveBinary(kStream, m_fInitialRotAngleVar);
    NiStreamSaveBinary(kStream, NiBool(m_bRandomRotSpeedSign));
    NiStreamSaveBinary(kStream, NiBool(m_bRandomInitialAxis));
    m_kInitialAxis.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysRotationModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysRotationModifier* pkDest = (NiPSysRotationModifier*) pkObject;

    if (pkDest->m_fInitialRotSpeed != m_fInitialRotSpeed ||
        pkDest->m_fInitialRotSpeedVar != m_fInitialRotSpeedVar ||
        pkDest->m_bRandomRotSpeedSign != m_bRandomRotSpeedSign ||
        pkDest->m_fInitialRotAngle != m_fInitialRotAngle ||
        pkDest->m_fInitialRotAngleVar != m_fInitialRotAngleVar ||
        pkDest->m_bRandomInitialAxis != m_bRandomInitialAxis ||
        pkDest->m_kInitialAxis != m_kInitialAxis)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysRotationModifier::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysRotationModifier::ms_RTTI
        .GetName()));

    pkStrings->Add(NiGetViewerString("Initial Rotation Speed",
        m_fInitialRotSpeed));
    pkStrings->Add(NiGetViewerString("Initial Rotation Speed Variation",
        m_fInitialRotSpeedVar));
    pkStrings->Add(NiGetViewerString("Random Rot Speed Sign",
        m_bRandomRotSpeedSign));
    pkStrings->Add(NiGetViewerString("Initial Rotation Angle",
        m_fInitialRotAngle));
    pkStrings->Add(NiGetViewerString("Initial Rotation Angle Variation",
        m_fInitialRotAngleVar));
    pkStrings->Add(NiGetViewerString("Random Initial Axis",
        m_bRandomInitialAxis));
    pkStrings->Add(m_kInitialAxis.GetViewerString("Initial Axis"));
}
//---------------------------------------------------------------------------
