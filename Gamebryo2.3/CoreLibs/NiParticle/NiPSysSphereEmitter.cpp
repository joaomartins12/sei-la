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

#include "NiPSysSphereEmitter.h"
#include "NiParticleSystem.h"
#include <NiCloningProcess.h>

NiImplementRTTI(NiPSysSphereEmitter, NiPSysVolumeEmitter);

//---------------------------------------------------------------------------
NiPSysSphereEmitter::NiPSysSphereEmitter(const char* pcName,
    NiAVObject* pkEmitterObj, float fEmitterRadius, float fSpeed, 
    float fSpeedVar, float fDeclination, float fDeclinationVar, 
    float fPlanarAngle, float fPlanarAngleVar, NiColorA kInitialColor, 
    float fInitialRadius, float fLifeSpan, float fLifeSpanVar, 
    float fRadiusVar) : NiPSysVolumeEmitter(pcName, pkEmitterObj, fSpeed, 
    fSpeedVar, fDeclination, fDeclinationVar, fPlanarAngle, fPlanarAngleVar, 
    kInitialColor, fInitialRadius, fLifeSpan, fLifeSpanVar, fRadiusVar),
    m_fEmitterRadius(fEmitterRadius)
{
}
//---------------------------------------------------------------------------
NiPSysSphereEmitter::NiPSysSphereEmitter() : 
    m_fEmitterRadius(0.0f)
{
}
//---------------------------------------------------------------------------
void NiPSysSphereEmitter::ComputeVolumeInitialPositionAndVelocity(
    NiTransform& kEmitterToPSys, NiPoint3& kPosition, NiPoint3& kVelocity)
{
    // Compute random initial position in Sphere.
    // Compute random initial position in Cylinder.   
    float fRadius = m_fEmitterRadius * NiUnitRandom();
    float fPhi = NiUnitRandom() * NI_TWO_PI;
    float fTheta = NiUnitRandom() * NI_TWO_PI;

    float fSinTheta, fCosTheta, fSinPhi, fCosPhi;
    NiSinCos(fPhi, fSinPhi, fCosPhi);
    NiSinCos(fTheta, fSinTheta, fCosTheta);

    kPosition.x = fRadius * fSinTheta * fCosPhi;
    kPosition.y = fRadius * fSinTheta * fSinPhi;
    kPosition.z = fRadius * fCosTheta;
  
    // Update position.
    kPosition = kEmitterToPSys * kPosition;

    // Update velocity.
    kVelocity = kEmitterToPSys.m_Rotate * kVelocity;

}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysSphereEmitter);
//---------------------------------------------------------------------------
void NiPSysSphereEmitter::CopyMembers(NiPSysSphereEmitter* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysVolumeEmitter::CopyMembers(pkDest, kCloning);

    pkDest->m_fEmitterRadius = m_fEmitterRadius;
}
//---------------------------------------------------------------------------
void NiPSysSphereEmitter::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysVolumeEmitter::ProcessClone(kCloning);

}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysSphereEmitter);
//---------------------------------------------------------------------------
void NiPSysSphereEmitter::LoadBinary(NiStream& kStream)
{
    NiPSysVolumeEmitter::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fEmitterRadius);
}
//---------------------------------------------------------------------------
void NiPSysSphereEmitter::LinkObject(NiStream& kStream)
{
    NiPSysVolumeEmitter::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysSphereEmitter::RegisterStreamables(NiStream& kStream)
{
    return NiPSysVolumeEmitter::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysSphereEmitter::SaveBinary(NiStream& kStream)
{
    NiPSysVolumeEmitter::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fEmitterRadius);
}
//---------------------------------------------------------------------------
bool NiPSysSphereEmitter::IsEqual(NiObject* pkObject)
{
    if (!NiPSysVolumeEmitter::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysSphereEmitter* pkDest = (NiPSysSphereEmitter*) pkObject;

    if (m_fEmitterRadius != pkDest->m_fEmitterRadius)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysSphereEmitter::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysVolumeEmitter::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysSphereEmitter::ms_RTTI.GetName()));
    pkStrings->Add(NiGetViewerString("Radius", m_fEmitterRadius));
}
//---------------------------------------------------------------------------
