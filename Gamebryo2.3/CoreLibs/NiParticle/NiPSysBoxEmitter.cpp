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

#include "NiPSysBoxEmitter.h"
#include "NiParticleSystem.h"
#include <NiCloningProcess.h>

NiImplementRTTI(NiPSysBoxEmitter, NiPSysVolumeEmitter);

//---------------------------------------------------------------------------
NiPSysBoxEmitter::NiPSysBoxEmitter(const char* pcName,
    NiAVObject* pkEmitterObj, float fEmitterWidth, float fEmitterHeight,
    float fEmitterDepth, float fSpeed, float fSpeedVar, float fDeclination,
    float fDeclinationVar, float fPlanarAngle, float fPlanarAngleVar,
    NiColorA kInitialColor, float fInitialRadius, float fLifeSpan, 
    float fLifeSpanVar, float fRadiusVar) : NiPSysVolumeEmitter(pcName, 
    pkEmitterObj, fSpeed, fSpeedVar, fDeclination, fDeclinationVar, 
    fPlanarAngle, fPlanarAngleVar, kInitialColor, fInitialRadius, 
    fLifeSpan, fLifeSpanVar, fRadiusVar), m_fEmitterWidth(fEmitterWidth), 
    m_fEmitterHeight(fEmitterHeight), m_fEmitterDepth(fEmitterDepth)
{
}
//---------------------------------------------------------------------------
NiPSysBoxEmitter::NiPSysBoxEmitter() : 
    m_fEmitterWidth(0.0f), m_fEmitterHeight(0.0f), m_fEmitterDepth(0.0f)
{
}
//---------------------------------------------------------------------------
void NiPSysBoxEmitter::ComputeVolumeInitialPositionAndVelocity(
    NiTransform& kEmitterToPSys, NiPoint3& kPosition, NiPoint3& kVelocity)
{
    // Compute random initial position in Box.
    kPosition = NiPoint3(
        m_fEmitterWidth * (NiUnitRandom() - 0.5f),
        m_fEmitterHeight * (NiUnitRandom() - 0.5f),
        m_fEmitterDepth * (NiUnitRandom() - 0.5f));
   
    // Update position.
    kPosition = kEmitterToPSys * kPosition;

    // Update velocity.
    kVelocity = kEmitterToPSys.m_Rotate * kVelocity;

}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysBoxEmitter);
//---------------------------------------------------------------------------
void NiPSysBoxEmitter::CopyMembers(NiPSysBoxEmitter* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysVolumeEmitter::CopyMembers(pkDest, kCloning);

    pkDest->m_fEmitterWidth = m_fEmitterWidth;
    pkDest->m_fEmitterHeight = m_fEmitterHeight;
    pkDest->m_fEmitterDepth = m_fEmitterDepth;
}
//---------------------------------------------------------------------------
void NiPSysBoxEmitter::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysVolumeEmitter::ProcessClone(kCloning);

}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysBoxEmitter);
//---------------------------------------------------------------------------
void NiPSysBoxEmitter::LoadBinary(NiStream& kStream)
{
    NiPSysVolumeEmitter::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fEmitterWidth);
    NiStreamLoadBinary(kStream, m_fEmitterHeight);
    NiStreamLoadBinary(kStream, m_fEmitterDepth);
}
//---------------------------------------------------------------------------
void NiPSysBoxEmitter::LinkObject(NiStream& kStream)
{
    NiPSysVolumeEmitter::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysBoxEmitter::RegisterStreamables(NiStream& kStream)
{
    return NiPSysVolumeEmitter::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysBoxEmitter::SaveBinary(NiStream& kStream)
{
    NiPSysVolumeEmitter::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fEmitterWidth);
    NiStreamSaveBinary(kStream, m_fEmitterHeight);
    NiStreamSaveBinary(kStream, m_fEmitterDepth);
}
//---------------------------------------------------------------------------
bool NiPSysBoxEmitter::IsEqual(NiObject* pkObject)
{
    if (!NiPSysVolumeEmitter::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysBoxEmitter* pkDest = (NiPSysBoxEmitter*) pkObject;

    if (m_fEmitterWidth != pkDest->m_fEmitterWidth ||
        m_fEmitterHeight != pkDest->m_fEmitterHeight ||
        m_fEmitterDepth != pkDest->m_fEmitterDepth)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysBoxEmitter::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysVolumeEmitter::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysBoxEmitter::ms_RTTI.GetName()));
    pkStrings->Add(NiGetViewerString("Width", m_fEmitterWidth));
    pkStrings->Add(NiGetViewerString("Height", m_fEmitterHeight));
    pkStrings->Add(NiGetViewerString("Depth", m_fEmitterDepth));
}
//---------------------------------------------------------------------------
