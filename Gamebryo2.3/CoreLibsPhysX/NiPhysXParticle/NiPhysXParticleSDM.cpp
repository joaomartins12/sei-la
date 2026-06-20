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
#include "NiPhysXParticlePCH.h"

#include "NiPhysXParticle.h"

NiImplementSDMConstructor(NiPhysXParticle);

#ifdef NIPHYSXPARTICLE_EXPORT
NiImplementDllMain(NiPhysXParticle);
#endif

//---------------------------------------------------------------------------
void NiPhysXParticleSDM::Init()
{
    NiImplementSDMInitCheck();

    NiRegisterStream(NiPhysXParticleSystem);
    NiRegisterStream(NiPhysXPSysData);
    NiRegisterStream(NiPhysXPSysDest);
    NiRegisterStream(NiPhysXPSysProp);
    NiRegisterStream(NiPhysXPSysSrc);
    NiRegisterStream(NiPhysXMeshParticleSystem);
    NiRegisterStream(NiPhysXMeshPSysData);
    NiRegisterStream(NiPhysXMeshPSysProp);
    NiRegisterStream(NiPhysXMeshPSysSrc);

    NiRegisterStream(NiPhysXPSysUpdateModifier);
}
//---------------------------------------------------------------------------
void NiPhysXParticleSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiUnregisterStream(NiPhysXPSysProp);
    NiUnregisterStream(NiPhysXParticleSystem);
    NiUnregisterStream(NiPhysXPSysData);
    NiUnregisterStream(NiPhysXMeshPSysProp);
    NiUnregisterStream(NiPhysXMeshParticleSystem);
    NiUnregisterStream(NiPhysXMeshPSysData);
    NiUnregisterStream(NiPhysXPSysDest);
    NiUnregisterStream(NiPhysXPSysSrc);
    NiUnregisterStream(NiPhysXMeshPSysSrc);

    NiUnregisterStream(NiPhysXPSysUpdateModifier);
}
//---------------------------------------------------------------------------
