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

#include "NiPSysEmitter.h"
#include "NiParticleSystem.h"

NiImplementRTTI(NiPSysEmitter, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysEmitter::NiPSysEmitter(const char* pcName, unsigned int uiOrder,
    float fSpeed, float fSpeedVar, float fDeclination, float fDeclinationVar,
    float fPlanarAngle, float fPlanarAngleVar, NiColorA kInitialColor,
    float fInitialRadius, float fLifeSpan, float fLifeSpanVar, 
    float fRadiusVar) :
    NiPSysModifier(pcName, uiOrder), m_fSpeed(fSpeed),
    m_fSpeedVar(fSpeedVar), m_fDeclination(fDeclination),
    m_fDeclinationVar(fDeclinationVar), m_fPlanarAngle(fPlanarAngle),
    m_fPlanarAngleVar(fPlanarAngleVar), m_kInitialColor(kInitialColor),
    m_fInitialRadius(fInitialRadius), m_fRadiusVar(fRadiusVar),
    m_fLifeSpan(fLifeSpan), m_fLifeSpanVar(fLifeSpanVar)
{
}
//---------------------------------------------------------------------------
NiPSysEmitter::NiPSysEmitter() : m_fSpeed(0.0f), m_fSpeedVar(0.0f),
    m_fDeclination(0.0f), m_fDeclinationVar(0.0f),
    m_fPlanarAngle(0.0f), m_fPlanarAngleVar(0.0f),
    m_kInitialColor(NiColorA::WHITE), m_fInitialRadius(1.0f), 
    m_fRadiusVar(0.0f), m_fLifeSpan(0.0f), m_fLifeSpanVar(0.0f)
{
}
//---------------------------------------------------------------------------
void NiPSysEmitter::Update(float fTime, NiPSysData* pkData)
{
    // Do nothing.
}
//---------------------------------------------------------------------------
void NiPSysEmitter::EmitParticles(float fTime, unsigned short usNumParticles,
    const float* pfAges)
{
    NiPSysData* pkData = (NiPSysData*) m_pkTarget->GetModelData();

    // Get data arrays.
    NiPoint3* pkVertex = pkData->GetVertices();
    NiColorA* pkColor = pkData->GetColors();
    float* pfRadii = pkData->GetRadii();
    float* pfSize = pkData->GetSizes();


    for (unsigned short us = 0; us < usNumParticles; us++)
    {
        // Check to make sure the particle is not already dead. If so,
        // don't create the particle in the first place.
        float fAge = pfAges[us];
        float fLifeSpan = m_fLifeSpan + m_fLifeSpanVar * (NiUnitRandom() -
            0.5f);
        if (fAge > fLifeSpan)
        {
            continue;
        }

        // Get pointer to new particle data.
        unsigned short usNewIndex = pkData->AddParticle();
        if (usNewIndex == NiPSysData::INVALID_PARTICLE)
        {
            return;
        }

        NiParticleInfo* pkNewParticle = &pkData->GetParticleInfo()
            [usNewIndex];

        // Create new particle from scratch.

        // Calculate new speed
        float fSpeed = m_fSpeed + m_fSpeedVar * (NiUnitRandom() - 0.5f);

        // Calculate new direction
        float fDec = m_fDeclination + 
            m_fDeclinationVar * NiSymmetricRandom();
        float fPlan = m_fPlanarAngle + 
            m_fPlanarAngleVar * NiSymmetricRandom();

        float fSinDec = NiSin(fDec);
        NiPoint3 kDir(fSinDec * NiCos(fPlan), fSinDec * NiSin(fPlan), 
            NiCos(fDec));

        pkNewParticle->m_kVelocity = fSpeed * kDir;

        // Various other initializations
        pkNewParticle->m_fAge = fAge;
        pkNewParticle->m_fLifeSpan = fLifeSpan;
        pkNewParticle->m_usGeneration = 0;

        ComputeInitialPositionAndVelocity(pkVertex[usNewIndex],
            pkNewParticle->m_kVelocity);

        if (pkColor)
        {
            pkColor[usNewIndex] = m_kInitialColor;
        }
        if (pfRadii)
        {
            pfRadii[usNewIndex] = m_fInitialRadius + m_fRadiusVar *
                NiSymmetricRandom();
        }
        if (pfSize)
        {
            pfSize[usNewIndex] = 1.0f;
        }

        pkNewParticle->m_fLastUpdate = fTime - pkNewParticle->m_fAge;

        m_pkTarget->InitializeNewParticle(usNewIndex);
    }

}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiPSysEmitter::CopyMembers(NiPSysEmitter* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifier::CopyMembers(pkDest, kCloning);

    pkDest->m_fSpeed = m_fSpeed;
    pkDest->m_fSpeedVar = m_fSpeedVar;
    pkDest->m_fDeclination = m_fDeclination;
    pkDest->m_fDeclinationVar = m_fDeclinationVar;
    pkDest->m_fPlanarAngle = m_fPlanarAngle;
    pkDest->m_fPlanarAngleVar = m_fPlanarAngleVar;
    pkDest->m_kInitialColor = m_kInitialColor;
    pkDest->m_fInitialRadius = m_fInitialRadius;
    pkDest->m_fLifeSpan = m_fLifeSpan;
    pkDest->m_fLifeSpanVar = m_fLifeSpanVar;
    pkDest->m_fRadiusVar = m_fRadiusVar;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiPSysEmitter::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fSpeed);
    NiStreamLoadBinary(kStream, m_fSpeedVar);
    NiStreamLoadBinary(kStream, m_fDeclination);
    NiStreamLoadBinary(kStream, m_fDeclinationVar);
    NiStreamLoadBinary(kStream, m_fPlanarAngle);
    NiStreamLoadBinary(kStream, m_fPlanarAngleVar);
    m_kInitialColor.LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_fInitialRadius);
    if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 3, 0, 2))
    {
        NiStreamLoadBinary(kStream, m_fRadiusVar);
    }
    NiStreamLoadBinary(kStream, m_fLifeSpan);
    NiStreamLoadBinary(kStream, m_fLifeSpanVar);
}
//---------------------------------------------------------------------------
void NiPSysEmitter::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysEmitter::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysEmitter::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fSpeed);
    NiStreamSaveBinary(kStream, m_fSpeedVar);
    NiStreamSaveBinary(kStream, m_fDeclination);
    NiStreamSaveBinary(kStream, m_fDeclinationVar);
    NiStreamSaveBinary(kStream, m_fPlanarAngle);
    NiStreamSaveBinary(kStream, m_fPlanarAngleVar);
    m_kInitialColor.SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_fInitialRadius);
    NiStreamSaveBinary(kStream, m_fRadiusVar);
    NiStreamSaveBinary(kStream, m_fLifeSpan);
    NiStreamSaveBinary(kStream, m_fLifeSpanVar);
}
//---------------------------------------------------------------------------
bool NiPSysEmitter::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysEmitter* pkDest = (NiPSysEmitter*) pkObject;

    if (pkDest->m_fSpeed != m_fSpeed ||
        pkDest->m_fSpeedVar != m_fSpeedVar ||
        pkDest->m_fDeclination != m_fDeclination ||
        pkDest->m_fDeclinationVar != m_fDeclinationVar ||
        pkDest->m_fPlanarAngle != m_fPlanarAngle ||
        pkDest->m_fPlanarAngleVar != m_fPlanarAngleVar ||
        pkDest->m_kInitialColor != m_kInitialColor ||
        pkDest->m_fInitialRadius != m_fInitialRadius ||
        pkDest->m_fRadiusVar != m_fRadiusVar ||
        pkDest->m_fLifeSpan != m_fLifeSpan ||
        pkDest->m_fLifeSpanVar != m_fLifeSpanVar)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysEmitter::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysEmitter::ms_RTTI
        .GetName()));

    pkStrings->Add(NiGetViewerString("Speed", m_fSpeed));
    pkStrings->Add(NiGetViewerString("Speed Variation", m_fSpeedVar));
    pkStrings->Add(NiGetViewerString("Declination", m_fDeclination));
    pkStrings->Add(NiGetViewerString("Declination Variation",
        m_fDeclinationVar));
    pkStrings->Add(NiGetViewerString("Planar Angle", m_fPlanarAngle));
    pkStrings->Add(NiGetViewerString("Planar Angle Variation",
        m_fPlanarAngleVar));
    pkStrings->Add(m_kInitialColor.GetViewerString("Initial Color"));
    pkStrings->Add(NiGetViewerString("Initial Radius", m_fInitialRadius));
    pkStrings->Add(NiGetViewerString("Radius Variation", m_fRadiusVar));
    pkStrings->Add(NiGetViewerString("Life Span", m_fLifeSpan));
    pkStrings->Add(NiGetViewerString("Life Span Variation", m_fLifeSpanVar));
}
//---------------------------------------------------------------------------
