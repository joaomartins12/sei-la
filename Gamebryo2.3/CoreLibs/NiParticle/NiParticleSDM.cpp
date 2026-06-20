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

#include "NiParticleSDM.h"
#include "NiParticle.h"

NiImplementSDMConstructor(NiParticle);

#ifdef NIPARTICLE_EXPORT
NiImplementDllMain(NiParticle);
#endif

//---------------------------------------------------------------------------
void NiParticleSDM::Init()
{
    NiImplementSDMInitCheck();

    NiRegisterStream(NiMeshParticleSystem);
    NiRegisterStream(NiMeshPSysData);
    NiRegisterStream(NiParticleSystem);
    NiRegisterStream(NiPSysAirFieldAirFrictionCtlr);
    NiRegisterStream(NiPSysAirFieldInheritVelocityCtlr);
    NiRegisterStream(NiPSysAirFieldModifier);
    NiRegisterStream(NiPSysAirFieldSpreadCtlr);
    NiRegisterStream(NiPSysAgeDeathModifier);
    NiRegisterStream(NiPSysBombModifier);
    NiRegisterStream(NiPSysBoundUpdateModifier);
    NiRegisterStream(NiPSysBoxEmitter);
    NiRegisterStream(NiPSysColliderManager);
    NiRegisterStream(NiPSysColorModifier);
    NiRegisterStream(NiPSysCylinderEmitter);
    NiRegisterStream(NiPSysData);
    NiRegisterStream(NiPSysDragFieldModifier);
    NiRegisterStream(NiPSysDragModifier);
    NiRegisterStream(NiPSysEmitterCtlr);
    NiRegisterStream(NiPSysEmitterCtlrData);
    NiRegisterStream(NiPSysEmitterDeclinationCtlr);
    NiRegisterStream(NiPSysEmitterDeclinationVarCtlr);
    NiRegisterStream(NiPSysEmitterInitialRadiusCtlr);
    NiRegisterStream(NiPSysEmitterLifeSpanCtlr);
    NiRegisterStream(NiPSysEmitterPlanarAngleCtlr);
    NiRegisterStream(NiPSysEmitterPlanarAngleVarCtlr);
    NiRegisterStream(NiPSysEmitterSpeedCtlr);
    NiRegisterStream(NiPSysFieldAttenuationCtlr);
    NiRegisterStream(NiPSysFieldMagnitudeCtlr);
    NiRegisterStream(NiPSysFieldMaxDistanceCtlr);
    NiRegisterStream(NiPSysGravityModifier);
    NiRegisterStream(NiPSysGravityFieldModifier);
    NiRegisterStream(NiPSysGravityStrengthCtlr);
    NiRegisterStream(NiPSysGrowFadeModifier);
    NiRegisterStream(NiPSysInitialRotAngleCtlr);
    NiRegisterStream(NiPSysInitialRotAngleVarCtlr);
    NiRegisterStream(NiPSysInitialRotSpeedCtlr);
    NiRegisterStream(NiPSysInitialRotSpeedVarCtlr);
    NiRegisterStream(NiPSysMeshEmitter);
    NiRegisterStream(NiPSysMeshUpdateModifier);
    NiRegisterStream(NiPSysModifierActiveCtlr);
    NiRegisterStream(NiPSysPlanarCollider);
    NiRegisterStream(NiPSysPositionModifier);
    NiRegisterStream(NiPSysRadialFieldModifier);
    NiRegisterStream(NiPSysResetOnLoopCtlr);
    NiRegisterStream(NiPSysRotationModifier);
    NiRegisterStream(NiPSysSpawnModifier);
    NiRegisterStream(NiPSysSphereEmitter);
    NiRegisterStream(NiPSysSphericalCollider);
    NiRegisterStream(NiPSysTurbulenceFieldModifier);
    NiRegisterStream(NiPSysUpdateCtlr);
    NiRegisterStream(NiPSysVortexFieldModifier);

    NiPSysUpdateTask::_SDMInit();
}
//---------------------------------------------------------------------------
void NiParticleSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiUnregisterStream(NiMeshParticleSystem);
    NiUnregisterStream(NiMeshPSysData);
    NiUnregisterStream(NiParticleSystem);
    NiUnregisterStream(NiPSysAirFieldAirFrictionCtlr);
    NiUnregisterStream(NiPSysAirFieldInheritVelocityCtlr);
    NiUnregisterStream(NiPSysAirFieldModifier);
    NiUnregisterStream(NiPSysAirFieldSpreadCtlr);
    NiUnregisterStream(NiPSysAgeDeathModifier);
    NiUnregisterStream(NiPSysBombModifier);
    NiUnregisterStream(NiPSysBoundUpdateModifier);
    NiUnregisterStream(NiPSysBoxEmitter);
    NiUnregisterStream(NiPSysColliderManager);
    NiUnregisterStream(NiPSysColorModifier);
    NiUnregisterStream(NiPSysCylinderEmitter);
    NiUnregisterStream(NiPSysData);
    NiUnregisterStream(NiPSysDragFieldModifier);
    NiUnregisterStream(NiPSysDragModifier);
    NiUnregisterStream(NiPSysEmitterCtlr);
    NiUnregisterStream(NiPSysEmitterCtlrData);
    NiUnregisterStream(NiPSysEmitterDeclinationCtlr);
    NiUnregisterStream(NiPSysEmitterDeclinationVarCtlr);
    NiUnregisterStream(NiPSysEmitterInitialRadiusCtlr);
    NiUnregisterStream(NiPSysEmitterLifeSpanCtlr);
    NiUnregisterStream(NiPSysEmitterPlanarAngleCtlr);
    NiUnregisterStream(NiPSysEmitterPlanarAngleVarCtlr);
    NiUnregisterStream(NiPSysEmitterSpeedCtlr);
    NiUnregisterStream(NiPSysFieldAttenuationCtlr);
    NiUnregisterStream(NiPSysFieldMagnitudeCtlr);
    NiUnregisterStream(NiPSysFieldMaxDistanceCtlr);
    NiUnregisterStream(NiPSysGravityModifier);
    NiUnregisterStream(NiPSysGravityFieldModifier);
    NiUnregisterStream(NiPSysGravityStrengthCtlr);
    NiUnregisterStream(NiPSysGrowFadeModifier);
    NiUnregisterStream(NiPSysInitialRotAngleCtlr);
    NiUnregisterStream(NiPSysInitialRotAngleVarCtlr);
    NiUnregisterStream(NiPSysInitialRotSpeedCtlr);
    NiUnregisterStream(NiPSysInitialRotSpeedVarCtlr);
    NiUnregisterStream(NiPSysMeshEmitter);
    NiUnregisterStream(NiPSysMeshUpdateModifier);
    NiUnregisterStream(NiPSysModifierActiveCtlr);
    NiUnregisterStream(NiPSysPlanarCollider);
    NiUnregisterStream(NiPSysPositionModifier);
    NiUnregisterStream(NiPSysRadialFieldModifier);
    NiUnregisterStream(NiPSysResetOnLoopCtlr);
    NiUnregisterStream(NiPSysRotationModifier);
    NiUnregisterStream(NiPSysSpawnModifier);
    NiUnregisterStream(NiPSysSphereEmitter);
    NiUnregisterStream(NiPSysSphericalCollider);
    NiUnregisterStream(NiPSysTurbulenceFieldModifier);
    NiUnregisterStream(NiPSysUpdateCtlr);
    NiUnregisterStream(NiPSysVortexFieldModifier);

    NiPSysUpdateTask::_SDMShutdown();
}
//---------------------------------------------------------------------------
