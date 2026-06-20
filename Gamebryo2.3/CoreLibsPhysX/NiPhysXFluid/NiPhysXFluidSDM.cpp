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
#include "NiPhysXFluidPCH.h"

#include "NiPhysXFluid.h"

NiImplementSDMConstructor(NiPhysXFluid);

#ifdef NIPHYSXFLUID_EXPORT
NiImplementDllMain(NiPhysXFluid);
#endif

//---------------------------------------------------------------------------
void NiPhysXFluidSDM::Init()
{
    NiImplementSDMInitCheck();

    NiRegisterStream(NiPhysXFluidSystem);
    NiRegisterStream(NiPhysXFluidData);
    NiRegisterStream(NiPhysXMeshFluidSystem);
    NiRegisterStream(NiPhysXMeshFluidData);
    NiRegisterStream(NiPhysXFluidDest);
    NiRegisterStream(NiPhysXFluidEmitter);
    NiRegisterStream(NiPhysXFluidAgeDeathModifier);
    NiRegisterStream(NiPhysXFluidUpdateModifier);
    NiRegisterStream(NiPhysXFluidPropDesc);
    NiRegisterStream(NiPhysXFluidSceneDesc);
    NiRegisterStream(NiPhysXFluidDesc);
    NiRegisterStream(NiPhysXFluidEmitterDesc);
}
//---------------------------------------------------------------------------
void NiPhysXFluidSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiUnregisterStream(NiPhysXFluidSystem);
    NiUnregisterStream(NiPhysXFluidData);
    NiUnregisterStream(NiPhysXMeshFluidSystem);
    NiUnregisterStream(NiPhysXMeshFluidData);
    NiUnregisterStream(NiPhysXFluidDest);
    NiUnregisterStream(NiPhysXFluidEmitter);
    NiUnregisterStream(NiPhysXFluidAgeDeathModifier);
    NiUnregisterStream(NiPhysXFluidUpdateModifier);
    NiUnregisterStream(NiPhysXFluidPropDesc);
    NiUnregisterStream(NiPhysXFluidSceneDesc);
    NiUnregisterStream(NiPhysXFluidDesc);
    NiUnregisterStream(NiPhysXFluidEmitterDesc);
}
//---------------------------------------------------------------------------
