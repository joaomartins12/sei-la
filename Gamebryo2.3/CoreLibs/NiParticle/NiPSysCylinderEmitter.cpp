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

#include "NiPSysCylinderEmitter.h"
#include "NiParticleSystem.h"
#include <NiCloningProcess.h>

NiImplementRTTI(NiPSysCylinderEmitter, NiPSysVolumeEmitter);

//---------------------------------------------------------------------------
NiPSysCylinderEmitter::NiPSysCylinderEmitter(const char* pcName,
    NiAVObject* pkEmitterObj, float fEmitterRadius, float fEmitterHeight,
    float fSpeed, float fSpeedVar, float fDeclination,
    float fDeclinationVar, float fPlanarAngle, float fPlanarAngleVar,
    NiColorA kInitialColor, float fInitialRadius, float fLifeSpan, 
    float fLifeSpanVar, float fRadiusVar) : NiPSysVolumeEmitter(pcName, 
    pkEmitterObj, fSpeed, fSpeedVar, fDeclination, fDeclinationVar, 
    fPlanarAngle, fPlanarAngleVar, kInitialColor, fInitialRadius,
    fLifeSpan, fLifeSpanVar, fRadiusVar), m_fEmitterRadius(fEmitterRadius), 
    m_fEmitterHeight(fEmitterHeight)
{
}
//---------------------------------------------------------------------------
NiPSysCylinderEmitter::NiPSysCylinderEmitter() : 
    m_fEmitterRadius(0.0f), m_fEmitterHeight(0.0f)
{
}
//---------------------------------------------------------------------------
void NiPSysCylinderEmitter::ComputeVolumeInitialPositionAndVelocity(
    NiTransform& kEmitterToPSys, NiPoint3& kPosition, NiPoint3& kVelocity)
{
    // Compute random initial position in Cylinder.   
    float fRadius = m_fEmitterRadius * NiUnitRandom();
    float fPhi = NiUnitRandom() * NI_TWO_PI;

    float fSinPhi, fCosPhi;
    NiSinCos(fPhi, fSinPhi, fCosPhi);
    
    kPosition.x = fRadius * fCosPhi;
    kPosition.y = fRadius * fSinPhi;
    kPosition.z = (NiUnitRandom()- 0.5f) * m_fEmitterHeight ;

    // Update position.
    kPosition = kEmitterToPSys * kPosition;

    // Update velocity.
    kVelocity = kEmitterToPSys.m_Rotate * kVelocity;

}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysCylinderEmitter);
//---------------------------------------------------------------------------
void NiPSysCylinderEmitter::CopyMembers(NiPSysCylinderEmitter* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysVolumeEmitter::CopyMembers(pkDest, kCloning);

    pkDest->m_fEmitterRadius = m_fEmitterRadius;
    pkDest->m_fEmitterHeight = m_fEmitterHeight;
}
//---------------------------------------------------------------------------
void NiPSysCylinderEmitter::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysVolumeEmitter::ProcessClone(kCloning);

}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysCylinderEmitter);
//---------------------------------------------------------------------------
void NiPSysCylinderEmitter::LoadBinary(NiStream& kStream)
{
    NiPSysVolumeEmitter::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fEmitterRadius);
    NiStreamLoadBinary(kStream, m_fEmitterHeight);
}
//---------------------------------------------------------------------------
void NiPSysCylinderEmitter::LinkObject(NiStream& kStream)
{
    NiPSysVolumeEmitter::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysCylinderEmitter::RegisterStreamables(NiStream& kStream)
{
    return NiPSysVolumeEmitter::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysCylinderEmitter::SaveBinary(NiStream& kStream)
{
    NiPSysVolumeEmitter::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fEmitterRadius);
    NiStreamSaveBinary(kStream, m_fEmitterHeight);
}
//---------------------------------------------------------------------------
bool NiPSysCylinderEmitter::IsEqual(NiObject* pkObject)
{
    if (!NiPSysVolumeEmitter::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysCylinderEmitter* pkDest = (NiPSysCylinderEmitter*) pkObject;

    if (m_fEmitterRadius != pkDest->m_fEmitterRadius ||
        m_fEmitterHeight != pkDest->m_fEmitterHeight)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysCylinderEmitter::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysVolumeEmitter::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiPSysCylinderEmitter::ms_RTTI.GetName()));
    pkStrings->Add(NiGetViewerString("Radius", m_fEmitterRadius));
    pkStrings->Add(NiGetViewerString("Height", m_fEmitterHeight));
}
//---------------------------------------------------------------------------
