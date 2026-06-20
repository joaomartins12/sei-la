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
#include "NiPhysXPCH.h"

#include "NiPhysX.h"

NiImplementSDMConstructor(NiPhysX);

#ifdef NIPHYSX_EXPORT
NiImplementDllMain(NiPhysX);
#endif

//---------------------------------------------------------------------------
void NiPhysXSDM::Init()
{
    NiImplementSDMInitCheck();
    
    NiRegisterStream(NiPhysXAccumInterpolatorDest);
    NiRegisterStream(NiPhysXAccumRigidBodyDest);
    NiRegisterStream(NiPhysXAccumTransformDest);
    NiRegisterStream(NiPhysXActorDesc);
    NiRegisterStream(NiPhysXBodyDesc);
    NiRegisterStream(NiPhysXClothDesc);
    NiRegisterStream(NiPhysXClothDest);
    NiRegisterStream(NiPhysXDest);
    NiRegisterStream(NiPhysXDynamicSrc);
    NiRegisterStream(NiPhysXInterpolatorDest);
    NiRegisterStream(NiPhysXD6JointDesc);
    NiRegisterStream(NiPhysXJointDesc);
    NiRegisterStream(NiPhysXKinematicSrc);
    NiRegisterStream(NiPhysXMaterialDesc);
    NiRegisterStream(NiPhysXMeshDesc);
    NiRegisterStream(NiPhysXProp);
    NiRegisterStream(NiPhysXPropDesc);
    NiRegisterStream(NiPhysXRigidBodyDest);
    NiRegisterStream(NiPhysXRigidBodySrc);
    NiRegisterStream(NiPhysXScene);
    NiRegisterStream(NiPhysXSceneDesc);
    NiRegisterStream(NiPhysXSDKDesc);
    NiRegisterStream(NiPhysXShapeDesc);
    NiRegisterStream(NiPhysXSrc);
    NiRegisterStream(NiPhysXTransformDest);

    NiPhysXManager::ms_pPhysXManager = NiNew NiPhysXManager();
    NIASSERT(NiPhysXManager::ms_pPhysXManager);

    // NiPhysXTypes had it's own SDM functions
    // NiPhysXTypes::_SDMInit();
}
//---------------------------------------------------------------------------
void NiPhysXSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();
 
    NiUnregisterStream(NiPhysXAccumInterpolatorDest);
    NiUnregisterStream(NiPhysXAccumRigidBodyDest);
    NiUnregisterStream(NiPhysXAccumTransformDest);
    NiUnregisterStream(NiPhysXActorDesc);
    NiUnregisterStream(NiPhysXBodyDesc);
    NiUnregisterStream(NiPhysXClothDesc);
    NiUnregisterStream(NiPhysXClothDest);
    NiUnregisterStream(NiPhysXDest);
    NiUnregisterStream(NiPhysXDynamicSrc);
    NiUnregisterStream(NiPhysXInterpolatorDest);
    NiUnregisterStream(NiPhysXD6JointDesc);
    NiUnregisterStream(NiPhysXJointDesc);
    NiUnregisterStream(NiPhysXKinematicSrc);
    NiUnregisterStream(NiPhysXMaterialDesc);
    NiUnregisterStream(NiPhysXMeshDesc);
    NiUnregisterStream(NiPhysXProp);
    NiUnregisterStream(NiPhysXPropDesc);
    NiUnregisterStream(NiPhysXRigidBodyDest);
    NiUnregisterStream(NiPhysXRigidBodySrc);
    NiUnregisterStream(NiPhysXScene);
    NiUnregisterStream(NiPhysXSceneDesc);
    NiUnregisterStream(NiPhysXSDKDesc);
    NiUnregisterStream(NiPhysXShapeDesc);
    NiUnregisterStream(NiPhysXSrc);
    NiUnregisterStream(NiPhysXTransformDest);

    // NiPhysXTypes had it's own SDM functions
    // NiPhysXTypes::_SDMShutdown();
    
    NiDelete NiPhysXManager::ms_pPhysXManager;
}
//---------------------------------------------------------------------------
