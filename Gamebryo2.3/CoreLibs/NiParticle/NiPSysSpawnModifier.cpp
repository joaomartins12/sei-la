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

#include "NiPSysSpawnModifier.h"
#include "NiParticleSystem.h"

NiImplementRTTI(NiPSysSpawnModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysSpawnModifier::NiPSysSpawnModifier(const char* pcName,
    unsigned short usNumSpawnGenerations, float fPercentageSpawned,
    unsigned short usMinNumToSpawn, unsigned short usMaxNumToSpawn,
    float fSpawnSpeedChaos, float fSpawnDirChaos, float fLifeSpan,
    float fLifeSpanVar) : NiPSysModifier(pcName, ORDER_EMITTER),
    m_usNumSpawnGenerations(usNumSpawnGenerations),
    m_fPercentageSpawned(fPercentageSpawned),
    m_usMinNumToSpawn(usMinNumToSpawn), m_usMaxNumToSpawn(usMaxNumToSpawn),
    m_fSpawnSpeedChaos(fSpawnSpeedChaos), m_fSpawnDirChaos(fSpawnDirChaos),
    m_fLifeSpan(fLifeSpan), m_fLifeSpanVar(fLifeSpanVar)
{
}
//---------------------------------------------------------------------------
NiPSysSpawnModifier::NiPSysSpawnModifier() : m_usNumSpawnGenerations(1),
    m_fPercentageSpawned(1.0f), m_usMinNumToSpawn(1), m_usMaxNumToSpawn(1),
    m_fSpawnSpeedChaos(0.0f), m_fSpawnDirChaos(0.0f), m_fLifeSpan(0.0f),
    m_fLifeSpanVar(0.0f)
{
}
//---------------------------------------------------------------------------
void NiPSysSpawnModifier::Update(float fTime, NiPSysData* pkData)
{
    // Do nothing.
}
//---------------------------------------------------------------------------
void NiPSysSpawnModifier::SpawnParticles(float fCurrentTime, float fSpawnTime,
    unsigned short usOldIndex, NiParticleSystem* pkOldPSystem)
{
    NiPSysData* pkOldData = (NiPSysData*) pkOldPSystem->GetModelData();
    NiParticleInfo* pkOldParticle = &pkOldData->GetParticleInfo()[usOldIndex];

    // Do not spawn if exceeded number of spawn generations or percentage
    // spawned.
    if (pkOldParticle->m_usGeneration >= m_usNumSpawnGenerations ||
        NiUnitRandom() > m_fPercentageSpawned)
    {
        return;
    }

    // Determine number to spawn.
    NIASSERT(m_usMaxNumToSpawn >= m_usMinNumToSpawn);
    float fVariation = NiUnitRandom() * (m_usMaxNumToSpawn -
        m_usMinNumToSpawn);
    unsigned short usVariation = (unsigned short) fVariation;
    if (NiFmod(fVariation, 1.0f) > 0.5f)
    {
        usVariation++;
    }
    unsigned short usNumToSpawn = m_usMinNumToSpawn + usVariation;
    if (usNumToSpawn == 0)
    {
        usNumToSpawn = 1;
    }

    // Spawn particles.
    for (unsigned short us = 0; us < usNumToSpawn; us++)
    {
        SpawnParticle(fCurrentTime, fSpawnTime, usOldIndex, pkOldPSystem);
    }
}
//---------------------------------------------------------------------------
void NiPSysSpawnModifier::SpawnParticle(float fCurrentTime, float fSpawnTime,
    unsigned short usOldIndex, NiParticleSystem* pkOldPSystem)
{
    // Get old particle information.
    NiPSysData* pkOldData = (NiPSysData*) pkOldPSystem->GetModelData();
    NiParticleInfo* pkOldParticle = &pkOldData->GetParticleInfo()
        [usOldIndex];
    NiPoint3* pkOldVertices = pkOldData->GetVertices();
    NiColorA* pkOldColors = pkOldData->GetColors();
    float* pfOldRadii = pkOldData->GetRadii();
    float* pfOldSizes = pkOldData->GetSizes();
    float* pfOldRotationAngles = pkOldData->GetRotationAngles();
    NiPoint3* pkOldRotationAxes = pkOldData->GetRotationAxes();
    float* pfOldRotationSpeeds = pkOldData->GetRotationSpeeds();
    NiPoint3 kOldPosition = pkOldVertices[usOldIndex];
    NiPoint3 kOldVelocity = pkOldParticle->m_kVelocity;

    // Transform old position and velocity.
    if (pkOldPSystem != m_pkTarget)
    {
        NiTransform kOldXForm = pkOldPSystem->GetWorldTransform();
        kOldPosition = kOldXForm * kOldPosition;
        kOldVelocity = kOldXForm.m_Rotate * kOldVelocity;

        NiTransform kNewXForm = m_pkTarget->GetWorldTransform();
        NiTransform kInvNewXForm;
        kNewXForm.Invert(kInvNewXForm);
        kOldPosition = kInvNewXForm * kOldPosition;
        kOldVelocity = kInvNewXForm.m_Rotate * kOldVelocity;
    }

    // Get pointer to new particle data.
    NiPSysData* pkNewData = (NiPSysData*) m_pkTarget->GetModelData();
    unsigned short usNewIndex = pkNewData->AddParticle();
    if (usNewIndex == NiPSysData::INVALID_PARTICLE)
    {
        return;
    }

    // Get new particle information.
    NiParticleInfo* pkNewParticle = &pkNewData->GetParticleInfo()[usNewIndex];
    NiPoint3* pkNewVertices = pkNewData->GetVertices();
    NiColorA* pkNewColors = pkNewData->GetColors();
    float* pfNewRadii = pkNewData->GetRadii();
    float* pfNewSizes = pkNewData->GetSizes();
    float* pfNewRotationAngles = pkNewData->GetRotationAngles();
    NiPoint3* pkNewRotationAxes = pkNewData->GetRotationAxes();
    float* pfNewRotationSpeeds = pkNewData->GetRotationSpeeds();

    // Calculate age based on spawn time.
    float fAge = (fCurrentTime - fSpawnTime);

    // Calculate new velocity based on original and speed, direction chaos.
    float fOrigSpeed = kOldVelocity.Length();

    // Add or subtract based on speed up/slow down factor.
    float fDeltaSpeedFactor = m_fSpawnSpeedChaos * NiUnitRandom();
    float fNewSpeed = (1.0f + fDeltaSpeedFactor) * fOrigSpeed;

    // Calculate directional chaos.
    float fDecChaos = NiUnitRandom() * m_fSpawnDirChaos * NI_PI;
    float fPlanChaos = NiUnitRandom() * NI_TWO_PI;

    float fSinDecChaos = NiSin(fDecChaos);
    NiPoint3 kDirChaos(fSinDecChaos * NiCos(fPlanChaos),
        fSinDecChaos * NiSin(fPlanChaos), NiCos(fDecChaos));

    // Rotate directional chaos to line up with original direction.
    // Original direction = (x,y,z), length = l
    // Rotation: angle = acos(z/l), axis = (y,-x,0)
    float fX = kOldVelocity.x;
    float fY = kOldVelocity.y;
    NiMatrix3 kS(NiPoint3(0.0f,0.0f,fX),NiPoint3(0.0f,0.0f,fY),
        NiPoint3(-fX,-fY,0.0f));

    float fTestVal = (kDirChaos.Cross(kOldVelocity))
        * NiPoint3(fY, -fX, 0.0f);
    NiMatrix3 kRot = NiMatrix3::IDENTITY;
    if (fTestVal > ms_fEpsilon)
    {
        // fX or fY != 0
        kRot = kRot + kS * (kRot + kS *
            ((fOrigSpeed - kOldVelocity.z) / 
            (fX * fX + fY * fY))) * (1.0f / fOrigSpeed);
    }
    else if (-fTestVal > ms_fEpsilon)
    {
        // fX or fY != 0
        kRot = kRot - kS * (kRot - kS *
            ((fOrigSpeed - kOldVelocity.z) / 
            (fX * fX + fY * fY))) * (1.0f / fOrigSpeed);
    }
    else // fX and fY == 0.0f
    {
        if (kOldVelocity.z < 0.0f)
        {
            kRot = NiMatrix3(NiPoint3(-1.0f, 0.0f, 0.0f),
                NiPoint3(0.0f, -1.0f, 0.0f), NiPoint3(0.0f, 0.0f, -1.0f));
        }
    }

    NiMatrix3 kTempMat = kRot * fNewSpeed;
    pkNewParticle->m_kVelocity = kTempMat * kDirChaos;

    // Various other initializations.
    pkNewParticle->m_fAge = fAge;
    pkNewParticle->m_fLifeSpan = m_fLifeSpan + m_fLifeSpanVar * 
        (NiUnitRandom() - 0.5f);
    pkNewParticle->m_usGeneration = pkOldParticle->m_usGeneration + 1;

    // pVertex must exist.
    pkNewVertices[usNewIndex] = kOldPosition;

    if (pkOldColors && pkNewColors)
    {
        pkNewColors[usNewIndex] = pkOldColors[usOldIndex];
    }
    if (pfOldRadii && pfNewRadii)
    {
        pfNewRadii[usNewIndex] = pfOldRadii[usOldIndex];
    }
    if (pfOldSizes && pfNewSizes)
    {
        pfNewSizes[usNewIndex] = pfOldSizes[usOldIndex];
    }
    if (pfOldRotationAngles && pfNewRotationAngles)
    {
        pfNewRotationAngles[usNewIndex] = pfOldRotationAngles[usOldIndex];
        NIASSERT(pfOldRotationSpeeds && pfNewRotationSpeeds);
        pfNewRotationSpeeds[usNewIndex] = pfOldRotationSpeeds[usOldIndex];
    }
    if (pkOldRotationAxes && pkNewRotationAxes)
    {
        pkNewRotationAxes[usNewIndex] = pkOldRotationAxes[usOldIndex];
    }

    pkNewParticle->m_fLastUpdate = fCurrentTime - pkNewParticle->m_fAge;

    m_pkTarget->InitializeNewParticle(usNewIndex);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysSpawnModifier);
//---------------------------------------------------------------------------
void NiPSysSpawnModifier::CopyMembers(NiPSysSpawnModifier* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifier::CopyMembers(pkDest, kCloning);

    pkDest->m_usNumSpawnGenerations = m_usNumSpawnGenerations;
    pkDest->m_fPercentageSpawned = m_fPercentageSpawned;
    pkDest->m_usMinNumToSpawn = m_usMinNumToSpawn;
    pkDest->m_usMaxNumToSpawn = m_usMaxNumToSpawn;
    pkDest->m_fSpawnSpeedChaos = m_fSpawnSpeedChaos;
    pkDest->m_fSpawnDirChaos = m_fSpawnDirChaos;
    pkDest->m_fLifeSpan = m_fLifeSpan;
    pkDest->m_fLifeSpanVar = m_fLifeSpanVar;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysSpawnModifier);
//---------------------------------------------------------------------------
void NiPSysSpawnModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_usNumSpawnGenerations);
    NiStreamLoadBinary(kStream, m_fPercentageSpawned);
    NiStreamLoadBinary(kStream, m_usMinNumToSpawn);
    NiStreamLoadBinary(kStream, m_usMaxNumToSpawn);
    NiStreamLoadBinary(kStream, m_fSpawnSpeedChaos);
    NiStreamLoadBinary(kStream, m_fSpawnDirChaos);
    NiStreamLoadBinary(kStream, m_fLifeSpan);
    NiStreamLoadBinary(kStream, m_fLifeSpanVar);
}
//---------------------------------------------------------------------------
void NiPSysSpawnModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysSpawnModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysSpawnModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_usNumSpawnGenerations);
    NiStreamSaveBinary(kStream, m_fPercentageSpawned);
    NiStreamSaveBinary(kStream, m_usMinNumToSpawn);
    NiStreamSaveBinary(kStream, m_usMaxNumToSpawn);
    NiStreamSaveBinary(kStream, m_fSpawnSpeedChaos);
    NiStreamSaveBinary(kStream, m_fSpawnDirChaos);
    NiStreamSaveBinary(kStream, m_fLifeSpan);
    NiStreamSaveBinary(kStream, m_fLifeSpanVar);
}
//---------------------------------------------------------------------------
bool NiPSysSpawnModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysSpawnModifier* pkDest = (NiPSysSpawnModifier*) pkObject;

    if (pkDest->m_usNumSpawnGenerations != m_usNumSpawnGenerations ||
        pkDest->m_fPercentageSpawned != m_fPercentageSpawned ||
        pkDest->m_usMinNumToSpawn != m_usMinNumToSpawn ||
        pkDest->m_usMaxNumToSpawn != m_usMaxNumToSpawn ||
        pkDest->m_fSpawnSpeedChaos != m_fSpawnSpeedChaos ||
        pkDest->m_fSpawnDirChaos != m_fSpawnDirChaos ||
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
void NiPSysSpawnModifier::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysSpawnModifier::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("Num Spawn Generations",
        m_usNumSpawnGenerations));
    pkStrings->Add(NiGetViewerString("Percentage Spawned",
        m_fPercentageSpawned));
    pkStrings->Add(NiGetViewerString("Min Num to Spawn", m_usMinNumToSpawn));
    pkStrings->Add(NiGetViewerString("Max Num to Spawn", m_usMaxNumToSpawn));
    pkStrings->Add(NiGetViewerString("Spawn Speed Chaos",
        m_fSpawnSpeedChaos));
    pkStrings->Add(NiGetViewerString("Spawn Dir Chaos", m_fSpawnDirChaos));
    pkStrings->Add(NiGetViewerString("Life Span", m_fLifeSpan));
    pkStrings->Add(NiGetViewerString("Life Span Variation", m_fLifeSpanVar));
}
//---------------------------------------------------------------------------
