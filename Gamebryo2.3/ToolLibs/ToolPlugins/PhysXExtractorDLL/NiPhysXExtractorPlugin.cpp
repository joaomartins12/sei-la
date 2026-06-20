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

#include "StdAfx.h"
#include "NiPhysXExtractorPlugin.h"
#include "NiPhysXExtractorDialog.h"
#include "NiPhysXExtractorPluginDefines.h"

#include <NxCooking.h>

#include "NiPhysXSharedData.h"


// RTTI implementation macro.
NiImplementRTTI(NiPhysXExtractorPlugin, NiPlugin);

const int NiPhysXExtractorPlugin::NUM_PRIMES = 18;
const unsigned int NiPhysXExtractorPlugin::PRIMES[18] = {
    7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191, 16381, 32749, 65521,
    131071, 262139, 524287, 999983
    };

//---------------------------------------------------------------------------
NiPhysXExtractorPlugin::NiPhysXExtractorPlugin() : NiPlugin(
    "PhysX Extractor Plug-in",          // name
    "1.1",                              // version
    "PhysX content extraction plug-in", // Short description, next is long
    "Extracts the PhysX information in a scene ready for export.")
{
    // If any required internal variables need to be set, set them here
    m_usNextMaterial = 1;
}
//---------------------------------------------------------------------------
NiPhysXExtractorPlugin::~NiPhysXExtractorPlugin()
{
    Reset();
}
//---------------------------------------------------------------------------
NiPluginInfo* NiPhysXExtractorPlugin::GetDefaultPluginInfo()
{
    // Create the default parameters for this plug-in.
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NiPhysXExtractorPlugin");
    pkPluginInfo->SetType("PROCESS");
    
    //
    // Define all the defaults
    //
    pkPluginInfo->SetValue(PHYSX_EXTRACTOR_SCENE_NAME, "PhysX Prop");
    pkPluginInfo->SetFloat(PHYSX_EXTRACTOR_SCALE, 1.0f);

    pkPluginInfo->SetFloat(PHYSX_EXTRACTOR_CONVEX_SKIN, 0.05f);
    pkPluginInfo->SetInt(PHYSX_EXTRACTOR_MATERIAL_BASE, 1);
    pkPluginInfo->SetBool(PHYSX_EXTRACTOR_HARDWARE_COOK, false);
    pkPluginInfo->SetBool(PHYSX_EXTRACTOR_KEEP_ON_LOAD, false);
    pkPluginInfo->SetBool(PHYSX_EXTRACTOR_INFLATE, false);
    pkPluginInfo->SetBool(PHYSX_EXTRACTOR_UNCOMPRESSED, false);

    pkPluginInfo->SetBool(PHYSX_EXTRACTOR_CONVERTPSYS, false);
    pkPluginInfo->SetBool(PHYSX_EXTRACTOR_SCENE_FLUID, false);
    pkPluginInfo->SetBool(PHYSX_EXTRACTOR_PSYS_SAME_SPACE, false);
    pkPluginInfo->SetBool(PHYSX_EXTRACTOR_PSYS_USE_ROT, false);
    
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NiPhysXExtractorPlugin::HasManagementDialog()
{
    // Since we have parameters that can be changed, we provide a management
    // dialog, otherwise we would return false.
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXExtractorPlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
    NiWindowRef hWndParent)
{
    // Handle the user interface for manipulating the execution of this
    // NiPlugin.
    // - If the user clicks 'OK', make the input pkInitialInfo match the new
    //   set of parameters and return true.
    // - If the user clicks 'Cancel', then leave the input pkInitialInfo as
    //   it is and return false.

    NiPhysXExtractorDialog kDialog(m_hModule, hWndParent, pkInitialInfo);

    switch (kDialog.DoModal())
    {
        case IDOK:
            // Copy over the results.
            pkInitialInfo->RemoveAllParameters();
            kDialog.GetResults()->Clone(pkInitialInfo);
            break;
        case IDCANCEL:
            return false;
            break;
        default:
            NIASSERT(false);
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXExtractorPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    // Verify that the NiPluginInfo object can actually be executed by
    // this plug-in. If we were to support previous versions, this is where
    // we would agree to handle the NiPluginInfo. Conversion would come in the 
    // actual Execute call.

    // Other useful values to check would be:
    // - the name of the plug-in,
    // - the application name if your plug-in is application-dependent 
    //   (which you can get from NiFramework)
    // - if any required parameters exist in this NiPluginInfo

    if (pkInfo->GetClassName() == "NiPhysXExtractorPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NiPhysXExtractorPlugin::Execute(
    const NiPluginInfo* pkInfo)
{
    m_pkLogger = 0;
    
    // Reset at the start because we don't know how the last attempt exited.
    Reset();

    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    // Set up for logging errors
    NiExporterOptionsSharedData* pkExporterSharedData = 
        (NiExporterOptionsSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiExporterOptionsSharedData));
    if (pkExporterSharedData)
    {
        if (pkExporterSharedData->GetWriteResultsToLog())
            m_pkLogger = pkExporterSharedData->GetXMLLogger();
    }
    
    // Figure out the target platform
    NiPluginToolkitRenderer kTargetPlatform =
        pkExporterSharedData->GetTargetPlatform();
    
    // Get the scene graph shared data.
    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*)
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));

    if (!pkSGSharedData)
    {
        if (m_pkLogger)
        {
            m_pkLogger->LogElement("PhysXExtractorError",
                "The PhysX Extractor Plug-in could not find the "
                "NiSceneGraphSharedData!");
        }
        else
        {
            NiMessageBox("The PhysX Extractor Plug-in could not find the "
                "NiSceneGraphSharedData!","Scene Graph Shared Data Missing");
        }
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    // Get the PhysX shared data.
    NiPhysXSharedData* pkPhysXSharedData = (NiPhysXSharedData*)
        pkDataList->Get(NiGetSharedDataType(NiPhysXSharedData));

    if (!pkPhysXSharedData)
    {
        pkPhysXSharedData = NiNew NiPhysXSharedData;
        pkDataList->Insert(pkPhysXSharedData);
    }

    // Find the scene root node. We could later replace this with a named
    // node or other method of specifying a PhysX scene root.
    NiNodePtr spSGAll = pkSGSharedData->GetFullSceneGraph();
    NiNode* pkSceneRoot = (NiNode*)spSGAll->GetObjectByName("Scene Root");
    if (!pkSceneRoot)
    {
        pkSceneRoot = (NiNode*)spSGAll->GetObjectByName("SceneNode");
    }
    spSGAll = 0;

    if (!pkSceneRoot)
    {
        if (m_pkLogger)
        {
            m_pkLogger->LogElement("PhysXExtractorError",
                "The PhysX Extractor Plug-in could not find the "
                "scene root node.");
        }
        else
        {
            NiMessageBox("The PhysX Extractor Plug-in could not find the "
                "scene root node.", "Missing scene root");
        }
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }
    
    pkSceneRoot->Update(0.0f);

    // PhysX SDK initialization
    NiPhysXManager* pkManager = NiPhysXManager::GetPhysXManager();
    if (!pkManager->Initialize())
    {
        if (m_pkLogger)
        {
            m_pkLogger->LogElement("PhysXExtractorError",
                "The PhysX Extractor Plug-in failed to initialize the "
                "PhysX SDK.");
        }
        else
        {
            NiMessageBox(
                "The PhysX Extractor Plug-in failed to initialize the "
                "PhysX SDK.", "PhysX SDK failure");
        }
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }
    
    // PhysX scene creation
    NiPhysXProp* pkRBProp = CreateRBProp(pkInfo, pkSceneRoot);  
    if (!pkRBProp)
    {
        if (m_pkLogger)
        {
            m_pkLogger->LogElement("PhysXExtractorError",
                "The PhysX Extractor Plug-in failed to create the "
                "PhysX Rigid Body prop.");
        }
        else
        {
            NiMessageBox(
                "The PhysX Extractor Plug-in failed to create the "
                "PhysX Rigid Body prop.", "PhysX creation failure");
        }
        pkManager->Shutdown();
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }
    
    // Pre-process for various relationships between objects. More efficient
    // to build a map up front than repeatedly search the scene graph for
    // them.
    CreateMaps(pkSceneRoot);
    
    // Prop creation made this snapshot, so we know it exists.
    NiPhysXPropDescPtr spRBPropDesc = pkRBProp->GetSnapshot();
    NIASSERT(spRBPropDesc != 0);

    // Get the skin width
    float fSkinWidth = pkInfo->GetFloat(PHYSX_EXTRACTOR_CONVEX_SKIN);

    // Prepare for mesh cooking
    NxCookingInterface* pkCookingInterface =
        NxGetCookingLib(NX_PHYSICS_SDK_VERSION);
    NIASSERT(pkCookingInterface != 0);

    pkCookingInterface->NxInitCooking(NxGetPhysicsSDKAllocator(),
        pkManager->m_pkPhysXSDK->getFoundationSDK().getErrorStream());

    NxCookingParams kCookingParams;   
    if (kTargetPlatform == RENDER_XENON)
        kCookingParams.targetPlatform = PLATFORM_XENON;
    else if (kTargetPlatform == RENDER_PS3)
        kCookingParams.targetPlatform = PLATFORM_PLAYSTATION3;
    else
        kCookingParams.targetPlatform = PLATFORM_PC; 
    kCookingParams.skinWidth = fSkinWidth;
    pkCookingInterface->NxSetCookingParams(kCookingParams);
    
    // Convert rigid bodies 
    m_usNextMaterial = pkInfo->GetInt(PHYSX_EXTRACTOR_MATERIAL_BASE);
    bool bHardware = pkInfo->GetBool(PHYSX_EXTRACTOR_HARDWARE_COOK);
    bool bInflate = false;
    if (pkInfo->GetParameterIndex(PHYSX_EXTRACTOR_INFLATE)
        != NIPT_INVALID_INDEX)
    {
        bInflate = pkInfo->GetBool(PHYSX_EXTRACTOR_INFLATE);
    }
    bool bUncompressed = false;
    if (pkInfo->GetParameterIndex(PHYSX_EXTRACTOR_UNCOMPRESSED)
        != NIPT_INVALID_INDEX)
    {
        bUncompressed = pkInfo->GetBool(PHYSX_EXTRACTOR_UNCOMPRESSED);
    }
    bool bFluid = pkInfo->GetBool(PHYSX_EXTRACTOR_SCENE_FLUID);    
    
    bool bRigidBodySuccess = ExtractRigidBodiesAndCloth(pkRBProp, spRBPropDesc,
        pkSceneRoot, pkCookingInterface, fSkinWidth, bHardware,
        bInflate, bUncompressed, bFluid);
    if (!bRigidBodySuccess)
    {
        if (m_pkLogger)
        {
            m_pkLogger->LogElement("PhysXExtractorError",
                "The PhysX Extractor Plug-in failed to convert "
                "the rigid bodies.");
        }
        else
        {
            NiMessageBox("The PhysX Extractor Plug-in failed to convert "
                "the rigid bodies.", "PhysX rigid bodies failure");
        }
        pkCookingInterface->NxCloseCooking();
        spRBPropDesc = 0;
        NiDelete pkRBProp;
        pkManager->Shutdown();
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }
    
    if (!PostProcessActors())
    {
        if (m_pkLogger)
        {
            m_pkLogger->LogElement("PhysXExtractorError",
                "The PhysX Extractor Plug-in failed to post process actors.");
        }
        else
        {
            NiMessageBox("The PhysX Extractor Plug-in failed to post process "
                "the actor.", "PhysX failure");
        }
        pkCookingInterface->NxCloseCooking();
        spRBPropDesc = 0;
        NiDelete pkRBProp;
        pkManager->Shutdown();
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }
    
    // Convert joints
    bool bJointSuccess =
        ExtractJoints(spRBPropDesc, 1.0f / pkRBProp->GetScaleFactor());
    if (!bJointSuccess)
    {
        if (m_pkLogger)
        {
            m_pkLogger->LogElement("PhysXExtractorError",
                "The PhysX Extractor Plug-in failed to convert "
                "the joints.");
        }
        else
        {
            NiMessageBox("The PhysX Extractor Plug-in failed to convert "
                "the joints.", "PhysX joints failure");
        }
        pkCookingInterface->NxCloseCooking();
        spRBPropDesc = 0;
        NiDelete pkRBProp;
        pkManager->Shutdown();
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }
    pkPhysXSharedData->AddProp(pkRBProp);

    // Convert particle systems
    NiTPointerMap<NiAVObject*, NiAVObject*> kChangedMap;
    bool bConvertPSysToPSys = pkInfo->GetBool(PHYSX_EXTRACTOR_CONVERTPSYS);
    if (bFluid || bConvertPSysToPSys)
    {
        NiPhysXPSysProp* pkPSysProp = 0;
        NiPhysXMeshPSysProp* pkMeshPSysProp = 0;
        bool bPhysXSpace = pkInfo->GetBool(PHYSX_EXTRACTOR_PSYS_SAME_SPACE);
        bool bPhysXRotates = pkInfo->GetBool(PHYSX_EXTRACTOR_PSYS_USE_ROT);

        bool bPSysSucceeded = FindAndConvertPSys(pkRBProp, pkPSysProp,
            pkMeshPSysProp, pkSceneRoot, bFluid, kChangedMap, bPhysXSpace,
            bPhysXRotates);
        if (!bPSysSucceeded)
        {
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError",
                    "The PhysX Extractor Plug-in failed to convert "
                    "the particle systems.");
            }
            else
            {
                NiMessageBox("The PhysX Extractor Plug-in failed to convert "
                    "the Particle systems.", "PhysX particle systems failure");
            }
            pkCookingInterface->NxCloseCooking();
            NiDelete(pkPSysProp);
            NiDelete(pkMeshPSysProp);
            pkPhysXSharedData->RemoveProp(pkRBProp);
            spRBPropDesc = 0;
            pkManager->Shutdown();
            return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
        }
        
        if (pkPSysProp)
            pkPhysXSharedData->AddProp(pkPSysProp);
        if (pkMeshPSysProp)
            pkPhysXSharedData->AddProp(pkMeshPSysProp);
    }

    EnactNodeSwitch(pkSceneRoot, kChangedMap);
    
    pkCookingInterface->NxCloseCooking();
    spRBPropDesc = 0;
    pkManager->Shutdown();

    // Add the scene to the extra data, if not done already
    NiExtraObjectsSharedData* pkExtraSharedData = (NiExtraObjectsSharedData*)
        pkDataList->Get(NiGetSharedDataType(NiExtraObjectsSharedData));

    if (!pkExtraSharedData)
    {
        pkExtraSharedData = NiNew NiExtraObjectsSharedData;
        pkDataList->Insert(pkExtraSharedData);
    }
    unsigned int uiCount = pkPhysXSharedData->GetPropCount();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        NiPhysXProp* pkProp = pkPhysXSharedData->GetPropAt(ui);
        if (!pkProp)
            continue;
            
        bool bPropExists = false;
        unsigned int uiObjCount = pkExtraSharedData->GetObjectCount();
        for (unsigned int ui = 0; ui < uiObjCount; ui++)
        {
            if (pkExtraSharedData->GetObjectAt(ui) == pkProp)
            {
                bPropExists = true;
                break;
            }
        }
        if (!bPropExists)
        {
            pkExtraSharedData->AddObject(pkProp, pkProp->GetName(), true);
        }
    }

    // Return success.
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
NiPhysXProp* NiPhysXExtractorPlugin::CreateRBProp(const NiPluginInfo* pkInfo,
    NiNode* pkSceneRoot)
{
    NiPhysXProp* pkProp = NiNew NiPhysXProp();
    
    pkProp->SetName((const char*)pkInfo->GetValue(PHYSX_EXTRACTOR_SCENE_NAME));
    pkProp->SetKeepMeshes(
        pkInfo->GetBool(PHYSX_EXTRACTOR_KEEP_ON_LOAD));
    pkProp->SetScaleFactor(1.0f / pkInfo->GetFloat(PHYSX_EXTRACTOR_SCALE));

    NiPhysXPropDesc* pkPropDesc;
    bool bFluid = pkInfo->GetBool(PHYSX_EXTRACTOR_SCENE_FLUID);
    if (bFluid)
    {
        pkPropDesc = NiNew NiPhysXFluidPropDesc();
    }
    else
    {
        pkPropDesc = NiNew NiPhysXPropDesc();
    }
    pkProp->SetSnapshot(pkPropDesc);
    
    // We will be storing one set of state for everything
    pkPropDesc->SetNumStates(1);
    
    return pkProp; 
}
//---------------------------------------------------------------------------
bool NiPhysXExtractorPlugin::ExtractRigidBodiesAndCloth(NiPhysXProp *pkProp,
    NiPhysXPropDesc* pkPropDesc, NiAVObject* pkAVObj,
    NxCookingInterface* pkCookInterface, float fSkinWidth,
    bool bHardware, bool bInflate, bool bUncompressed, bool bFluid)
{
    NiExtraData* pkExtraData = pkAVObj->GetExtraData(ED_ACTOR_TYPE);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiIntegerExtraData, pkExtraData));
        NiIntegerExtraData* pkActorTypeED = (NiIntegerExtraData*)pkExtraData;

        int iPhysXType = pkActorTypeED->GetValue();
        pkAVObj->RemoveExtraData(ED_ACTOR_TYPE);
        if (iPhysXType < PHYSX_ACTOR_NONE)
        {
            bool bSuccess = ExtractRigidBody(pkProp, pkPropDesc, pkAVObj,
                iPhysXType, pkCookInterface, fSkinWidth,
                bHardware, bInflate, bUncompressed, bFluid);
            if (!bSuccess)
                return false;
        }
        else if (iPhysXType == PHYSX_ACTOR_CLOTH)
        {
            bool bSuccess = ExtractCloth(pkProp, pkPropDesc, pkAVObj,
                pkCookInterface, bHardware);
            if (!bSuccess)
                return false;
        }
    }
    
    if (NiIsKindOf(NiNode, pkAVObj))
    {    
        NiNode* pkNode = (NiNode*)pkAVObj;
        
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkObj = pkNode->GetAt(ui);
            if (!pkObj)
                continue;
            bool bSuccess = ExtractRigidBodiesAndCloth(pkProp, pkPropDesc,
                pkObj, pkCookInterface, fSkinWidth, bHardware,
                bInflate, bUncompressed, bFluid);
            if (!bSuccess)
                return false;
        }
    }
 
    return true;    
}
//---------------------------------------------------------------------------
bool NiPhysXExtractorPlugin::ExtractRigidBody(NiPhysXProp *pkProp,
    NiPhysXPropDesc* pkPropDesc, NiAVObject* pkAVObj, int iActorType,
    NxCookingInterface* pkCookInterface, float fSkinWidth,
    bool bHardware, bool bInflate, bool bUncompressed, bool bFluid)
{
    // We know we have physics on this body, and that it is a geometry object
    float fInvScale = 1.0f / pkProp->GetScaleFactor();
    
    // Work through creating an actor
    NiPhysXActorDesc* pkActorDesc = ExtractActor(pkPropDesc, pkAVObj,
        iActorType, pkCookInterface, fInvScale, fSkinWidth,
        bHardware, bInflate, bUncompressed, bFluid);
    NiExtraData* pkExtraData;
    
    if (!pkActorDesc)
        return false;
        
    // At this point we need to check if it's a particle actor.
    // We have everything but it isn't in the scene yet.
    pkExtraData = pkAVObj->GetExtraData(ED_PARTICLE_ACTOR);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiStringExtraData, pkExtraData));
        NiStringExtraData* pkPSysED = (NiStringExtraData*)pkExtraData;
        m_kParticleMap.SetAt(pkPSysED->GetValue(), pkActorDesc);
        pkAVObj->RemoveExtraData(ED_PARTICLE_ACTOR);
        pkAVObj->SetAppCulled(true);

        return true;
    }
    
    // Global pose is node's global pose, scaled
    NxMat34 kPose;
    NiPhysXTypes::NiTransformToNxMat34(pkAVObj->GetWorldRotate(),
        pkAVObj->GetWorldTranslate(), kPose);
    kPose.t *= fInvScale;
    pkActorDesc->SetPose(kPose, 0);
    
    // Set up sources, destinations, update flags, etc.
    if (iActorType == 1)
    {
        // Mark the target as non-optimizable and as always updated
        NiStringExtraData* pkOptKeep =
            (NiStringExtraData*)pkAVObj->GetExtraData("NiOptimizeKeep");
        if (!pkOptKeep)
        {
            pkOptKeep = NiNew NiStringExtraData("NiOptimizeKeep");
            pkAVObj->AddExtraData("NiOptimizeKeep", pkOptKeep);
        }
        pkAVObj->SetSelectiveUpdateTransformsOverride(true);
        
        NiPhysXTransformDest* pkDest =
            NiNew NiPhysXTransformDest(pkAVObj, 0, 0);
        pkProp->AddDestination(pkDest);
        
        pkActorDesc->SetDest(pkDest);
    }
    else if (iActorType == 2)
    {
        NiStringExtraData* pkOptKeep =
            (NiStringExtraData*)pkAVObj->GetExtraData("NiOptimizeKeep");
        if (!pkOptKeep)
        {
            pkOptKeep = NiNew NiStringExtraData("NiOptimizeKeep");
            pkAVObj->AddExtraData("NiOptimizeKeep", pkOptKeep);
        }
        
        NiPhysXDynamicSrc* pkSrc = NiNew NiPhysXDynamicSrc(pkAVObj, 0);
        pkProp->AddSource(pkSrc);
        
        pkActorDesc->SetSource(pkSrc);
    }

    // Add the actor to the Prop Desc
    pkPropDesc->AddActor(pkActorDesc);
    
    // Store it for later lookup
    m_kActorMap.SetAt(pkAVObj, pkActorDesc);
    
    return true;
}
//---------------------------------------------------------------------------
NiPhysXActorDesc* NiPhysXExtractorPlugin::ExtractActor(
    NiPhysXPropDesc* pkPropDesc, NiAVObject* pkAVObj, int iActorType,
    NxCookingInterface* pkCookInterface, float fInvScale,
    float fSkinWidth, bool bHardware, bool bInflate, bool bUncompressed,
    bool bFluid)
{
    // Work through creating an actor
    NiPhysXActorDesc* pkActorDesc = NiNew NiPhysXActorDesc;
    NiExtraData* pkExtraData;

    // The name is taken from the node
    NiFixedString kActorName = pkAVObj->GetName();

    // The body for dynamic or kinematic actors
    NiPhysXBodyDesc* pkBodyDesc = 0;
    float fDensity = 1.0f;
    if (iActorType == PHYSX_ACTOR_DYNAMIC ||
        iActorType == PHYSX_ACTOR_KINEMATIC)
    {
        NxBodyDesc kNxBodyDesc;
        
        // Set the body
        if (iActorType == PHYSX_ACTOR_KINEMATIC)
            kNxBodyDesc.flags |= NX_BF_KINEMATIC;
        
        // Look for mass
        pkExtraData = pkAVObj->GetExtraData(ED_MASS);
        if (pkExtraData)
        {
            NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
            NiFloatExtraData* pkActorMassED = (NiFloatExtraData*)pkExtraData;
            kNxBodyDesc.mass = pkActorMassED->GetValue();
            pkAVObj->RemoveExtraData(ED_MASS);
        }
        // ... and density
        pkExtraData = pkAVObj->GetExtraData(ED_DENSITY);
        if (pkExtraData)
        {
            NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
            fDensity = ((NiFloatExtraData*)pkExtraData)->GetValue() /
                (fInvScale * fInvScale * fInvScale);
            pkAVObj->RemoveExtraData(ED_DENSITY);
        } 

        NxVec3 kLinearVelocity;
        kLinearVelocity.zero();
        pkExtraData = pkAVObj->GetExtraData(ED_LINEAR_VELOCITY);
        if (pkExtraData)
        {
            NIASSERT(NiIsKindOf(NiFloatsExtraData, pkExtraData));
            float* pfVals;
            unsigned int uiCount;
            ((NiFloatsExtraData*)pkExtraData)->GetArray(uiCount, pfVals);
            kLinearVelocity.x = pfVals[0] * fInvScale;
            kLinearVelocity.y = pfVals[1] * fInvScale;
            kLinearVelocity.z = pfVals[2] * fInvScale;
            pkAVObj->RemoveExtraData(ED_LINEAR_VELOCITY);
        } 

        NxVec3 kAngularVelocity;
        kAngularVelocity.zero();
        pkExtraData = pkAVObj->GetExtraData(ED_ANGULAR_VELOCITY);
        if (pkExtraData)
        {
            NIASSERT(NiIsKindOf(NiFloatsExtraData, pkExtraData));
            float* pfVals;
            unsigned int uiCount;
            ((NiFloatsExtraData*)pkExtraData)->GetArray(uiCount, pfVals);
            kAngularVelocity.x = pfVals[0];
            kAngularVelocity.y = pfVals[1];
            kAngularVelocity.z = pfVals[2];
            pkAVObj->RemoveExtraData(ED_ANGULAR_VELOCITY);
        }
        
        pkExtraData = pkAVObj->GetExtraData(ED_DISABLE_GRAVITY);
        if (pkExtraData)
        {
            NIASSERT(NiIsKindOf(NiBooleanExtraData, pkExtraData));
            bool bDisableGravity =
                ((NiBooleanExtraData*)pkExtraData)->GetValue();
            if (bDisableGravity)
                kNxBodyDesc.flags |= NX_BF_DISABLE_GRAVITY;
            pkAVObj->RemoveExtraData(ED_DISABLE_GRAVITY);
        }
        
        pkExtraData = pkAVObj->GetExtraData(ED_LINEAR_DAMPING);
        if (pkExtraData)
        {
            NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
            kNxBodyDesc.linearDamping =
                ((NiFloatExtraData*)pkExtraData)->GetValue();
            pkAVObj->RemoveExtraData(ED_LINEAR_DAMPING);
        } 
         
        pkExtraData = pkAVObj->GetExtraData(ED_ANGULAR_DAMPING);
        if (pkExtraData)
        {
            NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
            kNxBodyDesc.angularDamping =
                ((NiFloatExtraData*)pkExtraData)->GetValue();
            pkAVObj->RemoveExtraData(ED_ANGULAR_DAMPING);
        }
        
        pkExtraData = pkAVObj->GetExtraData(ED_SOLVER_ITERATIONS);
        if (pkExtraData)
        {
            NIASSERT(NiIsKindOf(NiIntegerExtraData, pkExtraData));
            kNxBodyDesc.solverIterationCount =
                ((NiIntegerExtraData*)pkExtraData)->GetValue();
            pkAVObj->RemoveExtraData(ED_SOLVER_ITERATIONS);
        }
        
        pkBodyDesc = NiNew NiPhysXBodyDesc;
        pkBodyDesc->SetFromBodyDesc(kNxBodyDesc, NiPhysXTypes::NXMAT33_ID);
        pkBodyDesc->SetVelocities(kLinearVelocity, kAngularVelocity, 0,
            NiPhysXTypes::NXMAT33_ID);
    }
    
    // ActorGroup information for collision filtering
    pkExtraData = pkAVObj->GetExtraData(ED_ACTOR_GROUP);
    NxActorGroup uiActorGroup = 0;
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiIntegerExtraData, pkExtraData));
        NiIntegerExtraData* pkActorGroupED = (NiIntegerExtraData*)pkExtraData;
        uiActorGroup = pkActorGroupED->GetValue();
        pkAVObj->RemoveExtraData(ED_ACTOR_GROUP);
    }
    
    // Try to find default material properties.
    NxMaterialDesc kMatDesc;
    kMatDesc.restitution = 0.3f;
    kMatDesc.staticFriction = 0.3f;
    kMatDesc.dynamicFriction = 0.3f;
    
    pkExtraData = pkAVObj->GetExtraData(ED_RESTITUTION);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
        kMatDesc.restitution = ((NiFloatExtraData*)pkExtraData)->GetValue();
        pkAVObj->RemoveExtraData(ED_RESTITUTION);
    }
    
    pkExtraData = pkAVObj->GetExtraData(ED_DYNAMIC_FRICTION);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
        kMatDesc.dynamicFriction =
            ((NiFloatExtraData*)pkExtraData)->GetValue();
        pkAVObj->RemoveExtraData(ED_DYNAMIC_FRICTION);
    }
    
    pkExtraData = pkAVObj->GetExtraData(ED_STATIC_FRICTION);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
        kMatDesc.staticFriction =
            ((NiFloatExtraData*)pkExtraData)->GetValue();
        pkAVObj->RemoveExtraData(ED_STATIC_FRICTION);
    }
    
    // Now we need shapes.
    NiTransform kGlobalXform;
    kGlobalXform.m_Rotate = pkAVObj->GetWorldRotate();
    kGlobalXform.m_Translate = pkAVObj->GetWorldTranslate();
    kGlobalXform.m_fScale = pkAVObj->GetWorldScale();
    NiTObjectArray<NiPhysXShapeDescPtr> kShapes;
    if (!ExtractShape(pkPropDesc, pkAVObj, kShapes, false, kGlobalXform,
        kMatDesc, pkCookInterface, fInvScale, fSkinWidth, bHardware,
        bInflate, bUncompressed, bFluid, iActorType == 3))
    {
        NiDelete pkActorDesc;
        NiDelete pkBodyDesc;
        return 0;
    }
    
    pkActorDesc->SetConstants(kActorName, fDensity, 0, uiActorGroup,
        pkBodyDesc, kShapes);
    pkActorDesc->SetPose(NiPhysXTypes::NXMAT34_ID, 0);

    if (!pkActorDesc->IsValid())
    {
        char pcMsg[1024];
        NiSprintf(pcMsg, 1024, "Actor is invalid: %s", pkAVObj->GetName());
        if (m_pkLogger)
        {
            m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
        }
        else
        {
            NiMessageBox(pcMsg, "PhysX Extractor Error");
        }
        NiDelete pkActorDesc;
        return 0;
    }

    // Set up any cloth attached to this actor. We actually attach the cloth
    // to the first shape.
    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_ATTACH_SHAPE);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiIntegersExtraData, pkExtraData));
        unsigned int uiSize;
        int* piArray;
        ((NiIntegersExtraData*)pkExtraData)->GetArray(uiSize, piArray);
        for (unsigned int ui = 0; ui < uiSize; ui++)
        {
            NiPhysXClothDesc::ClothAttachment* pkAttachment = 0;
            if (!m_kAttachmentMap.GetAt(piArray[ui], pkAttachment))
            {
                pkAttachment = NiNew NiPhysXClothDesc::ClothAttachment;
                m_kAttachmentMap.SetAt(piArray[ui], pkAttachment);
            }
            else
            {
                // Attachment has now been fully processed, to remove
                // from map.
                m_kAttachmentMap.SetAt(piArray[ui], 0);
            }
            NIASSERT(kShapes.GetSize() > 0);
            pkAttachment->m_spShape = kShapes.GetAt(0);
        }
        pkAVObj->RemoveExtraData(ED_CLOTH_ATTACH_SHAPE);
    }

    return pkActorDesc;
}
//---------------------------------------------------------------------------
bool NiPhysXExtractorPlugin::ExtractShape(NiPhysXPropDesc* pkPropDesc,
    NiAVObject* pkAVObj, NiTObjectArray<NiPhysXShapeDescPtr>& kShapesArray,
    bool bCompoundPiece, NiTransform& kGlobalXform, NxMaterialDesc &kMatDesc,
    NxCookingInterface* pkCookInterface, float fInvScale, float fSkinWidth,
    bool bHardware, bool bInflate, bool bUncompressed, bool bFluid,
    bool bStatic)
{
    NiPhysXShapeDesc* pkShapeDesc = 0;
    NiExtraData* pkExtraData;

    // First check for proxy
    pkExtraData = pkAVObj->GetExtraData(ED_PROXY_TARGET);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiIntegerExtraData, pkExtraData));
        unsigned int uiID = ((NiIntegerExtraData*)pkExtraData)->GetValue();
        
        NiAVObject* pkProxy;
        bool bFoundProxy = m_kProxyMap.GetAt(uiID, pkProxy);
        if (!bFoundProxy)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024, "Could not find proxy shape for object %s",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            return false;
        }
        
        // Deal with local transform
        NiTransform kProxyXform;
        kProxyXform.m_Rotate = pkProxy->GetWorldRotate();
        kProxyXform.m_Translate = pkProxy->GetWorldTranslate();
        kProxyXform.m_fScale = pkProxy->GetWorldScale();
        
        if (!ExtractShape(pkPropDesc, pkProxy, kShapesArray, false,
            kProxyXform, kMatDesc, pkCookInterface, fInvScale, fSkinWidth,
            bHardware, bInflate, bUncompressed, bFluid, bStatic))
        {
            return false;
        }
        
        NiTransform kDestInv;
        kGlobalXform.Invert(kDestInv);
        NiTransform kLocalXform = kDestInv * kProxyXform;
        
        NxMat34 kNxXform;
        NiPhysXTypes::NiTransformToNxMat34(kLocalXform.m_Rotate,
            kLocalXform.m_Translate, kNxXform);
        kNxXform.t *= fInvScale;
        for (unsigned int ui = 0; ui < kShapesArray.GetSize(); ui++)
        {
            pkShapeDesc = kShapesArray.GetAt(ui);
            if (!pkShapeDesc)
                continue;
                
            // We need to apply it on top of the existing transform
            pkShapeDesc->SetLocalPose(kNxXform * pkShapeDesc->GetLocalPose());
        }

        pkAVObj->RemoveExtraData(ED_PROXY_TARGET);

        return true;
    }

    // A compound shape will only be defined if there is no proxy.
    pkExtraData = pkAVObj->GetExtraData(ED_SHAPE_COMPOUND);
    if (pkExtraData)
    {
        // Get the ID.
        NIASSERT(pkExtraData && NiIsKindOf(NiIntegerExtraData, pkExtraData));
        unsigned int uiID = ((NiIntegerExtraData*)pkExtraData)->GetValue();
        pkAVObj->RemoveExtraData(ED_SHAPE_COMPOUND);
        
        // Get the array of nodes providing shapes.
        NiPhysXExtractorPlugin::NiAVObjectArray* pkPieces;
        m_kShapeGroupMap.GetAt(uiID, pkPieces);
        if (!pkPieces)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024, "No pieces found for compound object %s",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            return false;
        }
        
        // Work through all the pieces
        for (unsigned int ui = 0; ui < pkPieces->GetSize(); ui++)
        {
            NiAVObject* pkPiece = pkPieces->GetAt(ui);
            if (pkPiece)
            {
                ExtractShape(pkPropDesc, pkPiece, kShapesArray, true,
                    kGlobalXform, kMatDesc, pkCookInterface, fInvScale,
                    fSkinWidth, bHardware, bInflate, bUncompressed, bFluid,
                    bStatic);   
            }
        }
        
        return true;
    }
    
    pkShapeDesc = ExtractGeometry(pkAVObj, pkCookInterface, fInvScale,
        bHardware, bInflate, bUncompressed, bFluid, bStatic);
    if (!pkShapeDesc)
        return false;            

    NxSphereShapeDesc kCommonData; // Not going to use the sphere portion.
    kCommonData.name = pkAVObj->GetName();

    kCommonData.localPose = pkShapeDesc->GetLocalPose();

    // ShapeGroup information for collision filtering
    pkExtraData = pkAVObj->GetExtraData(ED_SHAPE_GROUP);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiIntegerExtraData, pkExtraData));
        NiIntegerExtraData* pkShapeGroupED = (NiIntegerExtraData*)pkExtraData;
        kCommonData.group = pkShapeGroupED->GetValue();
        pkAVObj->RemoveExtraData(ED_SHAPE_GROUP);
    }
    
    // Only set density and mass on pieces that are compound, otherwise
    // actor defines take precedence.
    if (bCompoundPiece)
    {
        // Get mass and density
        pkExtraData = pkAVObj->GetExtraData(ED_DENSITY);
        if (pkExtraData)
        {
            NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
            kCommonData.density = 
                ((NiFloatExtraData*)pkExtraData)->GetValue() /
                (fInvScale * fInvScale * fInvScale);
            pkAVObj->RemoveExtraData(ED_DENSITY);
        }    
        pkExtraData = pkAVObj->GetExtraData(ED_MASS);
        if (pkExtraData)
        {
            NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
            kCommonData.mass =
                ((NiFloatExtraData*)pkExtraData)->GetValue();
            pkAVObj->RemoveExtraData(ED_MASS);
        }
        
        // Deal with local transform
        // First to get the shapes to the parent space
        NiTransform kPieceXform;
        kPieceXform.m_Rotate = pkAVObj->GetWorldRotate();
        kPieceXform.m_Translate = pkAVObj->GetWorldTranslate();
        kPieceXform.m_fScale = pkAVObj->GetWorldScale();
        NiTransform kGloabalInv;
        kGlobalXform.Invert(kGloabalInv);
        NiTransform kLocalXform = kGloabalInv * kPieceXform;
        
        // We need to apply it on top of the existing transform
        NxMat34 kNxXform;
        NiPhysXTypes::NiTransformToNxMat34(kLocalXform.m_Rotate,
            kLocalXform.m_Translate, kNxXform);
        kNxXform.t *= fInvScale;
        kCommonData.localPose = kNxXform * kCommonData.localPose;
    }

    pkExtraData = pkAVObj->GetExtraData(ED_RESTITUTION);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
        kMatDesc.restitution = ((NiFloatExtraData*)pkExtraData)->GetValue();
        pkAVObj->RemoveExtraData(ED_RESTITUTION);
    }
    
    pkExtraData = pkAVObj->GetExtraData(ED_DYNAMIC_FRICTION);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
        kMatDesc.dynamicFriction =
            ((NiFloatExtraData*)pkExtraData)->GetValue();
        pkAVObj->RemoveExtraData(ED_DYNAMIC_FRICTION);
    }
    
    pkExtraData = pkAVObj->GetExtraData(ED_STATIC_FRICTION);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
        kMatDesc.staticFriction =
            ((NiFloatExtraData*)pkExtraData)->GetValue();
        pkAVObj->RemoveExtraData(ED_STATIC_FRICTION);
    }
    
    kCommonData.materialIndex =
        pkPropDesc->AddMaterial(kMatDesc, m_usNextMaterial);
    if (kCommonData.materialIndex == m_usNextMaterial)
        m_usNextMaterial++;
    
    // Set the skin width
    kCommonData.skinWidth = fSkinWidth;
    
    pkShapeDesc->FromShapeDesc(kCommonData);
    kShapesArray.AddFirstEmpty(pkShapeDesc);
    
    return true;
}
//---------------------------------------------------------------------------
NiPhysXShapeDesc* NiPhysXExtractorPlugin::ExtractGeometry(NiAVObject* pkAVObj,
    NxCookingInterface* pkCookInterface, float fInvScale, bool bHardware,
    bool bInflate, bool bUncompressed, bool bFluid, bool bStatic)
{
    NiPhysXShapeDesc* pkShapeDesc = 0;
    
    NiExtraData* pkExtraData = 0;
    
    pkExtraData = pkAVObj->GetExtraData(ED_SHAPE_TYPE);
    NIASSERT(pkExtraData && NiIsKindOf(NiIntegerExtraData, pkExtraData));
    int iShapeType = ((NiIntegerExtraData*)pkExtraData)->GetValue();
    pkAVObj->RemoveExtraData(ED_SHAPE_TYPE);

    // The ordering on these conditionals matters
    if ((iShapeType == PHYSX_SHAPE_MESH && !bFluid) || (bFluid && bStatic))
    {
        if (bFluid &&
            iShapeType != PHYSX_SHAPE_MESH &&
            iShapeType != PHYSX_SHAPE_CONVEX)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024, "Static fluid shapes must be mesh-based %s",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            return 0;
        }

        // Figure out which mesh group we are
        pkExtraData = pkAVObj->GetExtraData(ED_SHAPE_MESH_PARENT);
        if (!pkExtraData)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024, "No meshes for shape %s",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            return 0;
        }
        NIASSERT(pkExtraData && NiIsKindOf(NiIntegerExtraData, pkExtraData));
        int iMeshID = ((NiIntegerExtraData*)pkExtraData)->GetValue();
        pkAVObj->RemoveExtraData(ED_SHAPE_MESH_PARENT);

        if (m_kMeshesMap.GetAt(iMeshID, pkShapeDesc))
        {
            // Have it already
            return pkShapeDesc;
        }
        
        // We'll need transform information for the parent
        NiTransform kParentXform;
        kParentXform.m_Rotate = pkAVObj->GetWorldRotate();
        kParentXform.m_Translate = pkAVObj->GetWorldTranslate();
        kParentXform.m_fScale = pkAVObj->GetWorldScale();
        NiTransform kParentInv;
        kParentXform.Invert(kParentInv);
        
        // Iterate over all the meshes in the group
        NiPhysXExtractorPlugin::NiAVObjectArray* pkMeshes;
        m_kMeshGroupMap.GetAt(iMeshID, pkMeshes);
        if (!pkMeshes)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024, "Could not get meshes for shape %s",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            return 0;
        }
        
        // Work through all the pieces, summing up number of vertices and faces
        unsigned short uiMaxNumVerts = 0;
        unsigned short uiMaxNumTris = 0;
        for (unsigned int ui = 0; ui < pkMeshes->GetSize(); ui++)
        {
            NiAVObject* pkMesh = pkMeshes->GetAt(ui);
            if (!pkMesh)
                continue;

            if (!NiIsKindOf(NiGeometry, pkMesh))
            {
                char pcMsg[1024];
                NiSprintf(pcMsg, 1024,
                    "Trying to use non-geometry for a mesh shape: %s",
                    pkMesh->GetName());
                if (m_pkLogger)
                {
                    m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
                }
                else
                {
                    NiMessageBox(pcMsg, "PhysX Extractor Error");
                }
                return 0;
            }
            NiGeometry* pkGeom = (NiGeometry*)pkMesh;
            
            uiMaxNumVerts += pkGeom->GetVertexCount();
            
            NiGeometryData* pkGeomData = pkGeom->GetModelData();
            if (!NiIsKindOf(NiTriBasedGeomData, pkGeomData))
            {
                char pcMsg[1024];
                NiSprintf(pcMsg, 1024,
                    "Trying to use non-trishape geometry for a mesh shape: %s",
                    pkMesh->GetName());
                if (m_pkLogger)
                {
                    m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
                }
                else
                {
                    NiMessageBox(pcMsg, "PhysX Extractor Error");
                }
                return 0;
            }
            NiTriBasedGeomData* pkTriData = (NiTriBasedGeomData*)pkGeomData;

            uiMaxNumTris += pkTriData->GetTriangleCount();
        }

        NxTriangleMeshShapeDesc kTriangleShapeDesc;
        pkShapeDesc = NiNew NiPhysXShapeDesc;
        pkShapeDesc->FromShapeDesc(kTriangleShapeDesc);
      
        // Extract mesh out of Gamebryo. Vertices need to be unique, which
        // makes things more difficult.
        unsigned int uiNumNxVerts = 0;
        NxVec3* pkNxVerts = NiAlloc(NxVec3, uiMaxNumVerts);
        unsigned int uiTriIndex = 0;
        NxU32* pkNxTriData = NiAlloc(NxU32, uiMaxNumTris * 3);

        NiTMap<NxVec3*, unsigned int, NiPhysXNxVec3HashFunctor,
            NiPhysXNxVec3EqualsFunctor> kUniqueVerts(NextPrime(uiMaxNumVerts));
        unsigned int* puiIndexMap = NiAlloc(unsigned int, uiMaxNumVerts);

        for (unsigned int ui = 0; ui < pkMeshes->GetSize(); ui++)
        {
            NiAVObject* pkMesh = pkMeshes->GetAt(ui);
            NiGeometry* pkGeom = (NiGeometry*)pkMesh;
            NiGeometryData* pkGeomData = pkGeom->GetModelData();
            NiTriBasedGeomData* pkTriData = (NiTriBasedGeomData*)pkGeomData;

            // Apply local transformations here, so multiple meshes with
            // different local transforms can be handled.
            // For some reason mesh children aren't always updated
            NiTransform kChildXform;
            kChildXform.m_Rotate = pkMesh->GetWorldRotate();
            kChildXform.m_Translate = pkMesh->GetWorldTranslate();
            kChildXform.m_fScale = pkMesh->GetWorldScale();
            NiTransform kLocalXform = kParentInv * kChildXform;

            NiPoint3* pkVertices = pkGeom->GetVertices();
            float fScale = pkAVObj->GetWorldScale();
            unsigned short usNumVerts = pkGeom->GetVertexCount();
            unsigned short uiBaseNum = uiNumNxVerts;
            for (unsigned short us = 0; us < usNumVerts; us++)
            {
                NxVec3 kLocalPt;
                NiPhysXTypes::NiPoint3ToNxVec3(
                    kLocalXform * pkVertices[us], kLocalPt);
                unsigned int uiIndex = 0;
                bool bExisting = kUniqueVerts.GetAt(&kLocalPt, uiIndex);
                if (bExisting)
                {
                    puiIndexMap[us] = uiIndex;
                }
                else
                {
                    pkNxVerts[uiNumNxVerts] = kLocalPt;
                    puiIndexMap[us] = uiNumNxVerts;
                    kUniqueVerts.SetAt(pkNxVerts + uiNumNxVerts, uiNumNxVerts);
                    uiNumNxVerts++;
                }
            }
            for (unsigned short ui = uiBaseNum; ui < uiNumNxVerts; ui++)
            {
                pkNxVerts[ui] *= fScale * fInvScale;
            }
            
            // Get faces
            unsigned short usNumTris = pkTriData->GetTriangleCount();
            for ( unsigned short us = 0; us < usNumTris; us++)
            {
                unsigned short usV1;
                unsigned short usV2;
                unsigned short usV3;
                pkTriData->GetTriangleIndices(us, usV1, usV2, usV3);
                pkNxTriData[uiTriIndex++] = puiIndexMap[usV1];
                pkNxTriData[uiTriIndex++] = puiIndexMap[usV2];
                pkNxTriData[uiTriIndex++] = puiIndexMap[usV3];
            }
        }
        
        // Set up and cook.
        NxTriangleMeshDesc kTriMeshDesc;
        kTriMeshDesc.numVertices = uiNumNxVerts;
        kTriMeshDesc.numTriangles = uiMaxNumTris;
        kTriMeshDesc.pointStrideBytes = sizeof(NxVec3);
        kTriMeshDesc.triangleStrideBytes = 3 * sizeof(NxU32);
        kTriMeshDesc.points = pkNxVerts;
        kTriMeshDesc.triangles = pkNxTriData;
        if (bHardware)
            kTriMeshDesc.flags = NX_MF_HARDWARE_MESH;
        else
            kTriMeshDesc.flags = 0;

        NiPhysXMemStream* kCookStream = NiNew NiPhysXMemStream();
        bool bCookRes =
            pkCookInterface->NxCookTriangleMesh(kTriMeshDesc, *kCookStream);
        if (!bCookRes)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024, "Could not cook mesh %s",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            NiFree(pkNxVerts);
            NiFree(pkNxTriData);
            NiFree(puiIndexMap);
            NiDelete kCookStream;
            return 0;
        }
        unsigned int uiMeshSize = kCookStream->GetSize();
        unsigned char* pucMeshData = (unsigned char*)kCookStream->GetBuffer();
        StashMeshVersion(pucMeshData);

        NiFree(pkNxVerts);
        NiFree(pkNxTriData);
        NiFree(puiIndexMap);
        NiDelete kCookStream;

        NiPhysXMeshDesc* pkMeshDesc = NiNew NiPhysXMeshDesc;
        pkMeshDesc->SetIsTriMesh();
        pkMeshDesc->SetPlatform(
            pkCookInterface->NxGetCookingParams().targetPlatform);
        pkMeshDesc->SetHardware(bHardware);
        pkMeshDesc->SetName(pkAVObj->GetName());
        pkMeshDesc->SetFlags(0);
        pkMeshDesc->SetData(uiMeshSize, pucMeshData);
        pkShapeDesc->SetMeshDesc(pkMeshDesc);
        
        m_kMeshesMap.SetAt(iMeshID, pkShapeDesc);
    }
    else if (iShapeType == PHYSX_SHAPE_CONVEX)
    {
        // Figure out which mesh group we are
        pkExtraData = pkAVObj->GetExtraData(ED_SHAPE_MESH_PARENT);
        if (!pkExtraData)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024, "No meshes for shape %s",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            return 0;
        }
        NIASSERT(NiIsKindOf(NiIntegerExtraData, pkExtraData));
        int iMeshID = ((NiIntegerExtraData*)pkExtraData)->GetValue();
        pkAVObj->RemoveExtraData(ED_SHAPE_MESH_PARENT);

        if (m_kMeshesMap.GetAt(iMeshID, pkShapeDesc))
        {
            // Have it already
            return pkShapeDesc;
        }
        
        // We'll need transform information for the parent
        NiTransform kParentXform;
        kParentXform.m_Rotate = pkAVObj->GetWorldRotate();
        kParentXform.m_Translate = pkAVObj->GetWorldTranslate();
        kParentXform.m_fScale = pkAVObj->GetWorldScale();
        NiTransform kParentInv;
        kParentXform.Invert(kParentInv);
        
        // Iterate over all the meshes in the group
        NiPhysXExtractorPlugin::NiAVObjectArray* pkMeshes;
        m_kMeshGroupMap.GetAt(iMeshID, pkMeshes);
        if (!pkMeshes)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024, "Could not get meshes for shape %s",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            return 0;
        }
        
        // Work through all the pieces, summing up number of vertices and faces
        unsigned short usMaxNumVerts = 0;
        for (unsigned int ui = 0; ui < pkMeshes->GetSize(); ui++)
        {
            NiAVObject* pkMesh = pkMeshes->GetAt(ui);
            if (!pkMesh)
                continue;
                
            if (!NiIsKindOf(NiGeometry, pkMesh))
            {
                char pcMsg[1024];
                NiSprintf(pcMsg, 1024,
                    "Trying to use non-geometry for a convex shape: %s",
                    pkMesh->GetName());
                if (m_pkLogger)
                {
                    m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
                }
                else
                {
                    NiMessageBox(pcMsg, "PhysX Extractor Error");
                }
                return 0;
            }
            NiGeometry* pkGeom = (NiGeometry*)pkMesh;
            
            usMaxNumVerts += pkGeom->GetVertexCount();
        }

        NxConvexShapeDesc kConvexShapeDesc;
        pkShapeDesc = NiNew NiPhysXShapeDesc;
        pkShapeDesc->FromShapeDesc(kConvexShapeDesc);
      
        // Extract mesh out of Gamebryo.
        NxVec3* pkNxVerts = NiAlloc(NxVec3, usMaxNumVerts);
        unsigned short usNumNxVerts = 0;
        for (unsigned int ui = 0; ui < pkMeshes->GetSize(); ui++)
        {
            NiAVObject* pkMesh = pkMeshes->GetAt(ui);
            NiGeometry* pkGeom = (NiGeometry*)pkMesh;
            NiGeometryData* pkGeomData = pkGeom->GetModelData();

            // Apply local transformations here, so multiple meshes with
            // different local transforms can be handled.
            // For some reason mesh children aren't always updated
            NiTransform kChildXform;
            kChildXform.m_Rotate = pkMesh->GetWorldRotate();
            kChildXform.m_Translate = pkMesh->GetWorldTranslate();
            kChildXform.m_fScale = pkMesh->GetWorldScale();
            NiTransform kLocalXform = kParentInv * kChildXform;

            NiPoint3* pkVertices = pkGeom->GetVertices();
            float fScale = pkAVObj->GetWorldScale();
            unsigned short usNumVerts = pkGeom->GetVertexCount();
            for (unsigned short us = 0; us < usNumVerts; us++)
            {
                NxVec3 kNxVec;
                NiPhysXTypes::NiPoint3ToNxVec3(kLocalXform * pkVertices[us],
                    kNxVec);
                pkNxVerts[usNumNxVerts++] = kNxVec * fScale * fInvScale;
            }
        }

        // Set up the convex mesh descriptor
        NxConvexMeshDesc kConvexDesc;
        kConvexDesc.numVertices = usNumNxVerts;
        kConvexDesc.pointStrideBytes = sizeof(NxVec3);
        kConvexDesc.points = pkNxVerts;
        kConvexDesc.flags = NX_CF_COMPUTE_CONVEX;
        if (bInflate)
            kConvexDesc.flags |= NX_CF_INFLATE_CONVEX;
        if (bUncompressed)
            kConvexDesc.flags |= NX_CF_USE_UNCOMPRESSED_NORMALS;
         
        NiPhysXMemStream* kCookStream = NiNew NiPhysXMemStream();
        bool bCookRes =
            pkCookInterface->NxCookConvexMesh(kConvexDesc, *kCookStream);
        if (!bCookRes)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024, "Could not cook convex mesh %s",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            NiFree(pkNxVerts);
            NiDelete kCookStream;
            return 0;
        }
        unsigned int uiMeshSize = kCookStream->GetSize();
        unsigned char* pucMeshData = (unsigned char*)kCookStream->GetBuffer();
        StashMeshVersion(pucMeshData);
        
        NiFree(pkNxVerts);
        NiDelete kCookStream;

        NiPhysXMeshDesc* pkMeshDesc = NiNew NiPhysXMeshDesc;
        pkMeshDesc->SetIsConvex();
        pkMeshDesc->SetPlatform(
            pkCookInterface->NxGetCookingParams().targetPlatform);
        pkMeshDesc->SetHardware(bHardware);
        pkMeshDesc->SetName(pkAVObj->GetName());
        pkMeshDesc->SetFlags(0);
        pkMeshDesc->SetData(uiMeshSize, pucMeshData);
        pkShapeDesc->SetMeshDesc(pkMeshDesc);
        
        m_kMeshesMap.SetAt(iMeshID, pkShapeDesc);
    }    
    else if (iShapeType == PHYSX_SHAPE_SPHERE)
    {
        // Get the ID for this shape
        pkExtraData = pkAVObj->GetExtraData(ED_SHAPE_BASIC_PARENT);
        if (!pkExtraData)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024, "No geometry for shape %s",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            return 0;
        }
        NIASSERT(NiIsKindOf(NiIntegerExtraData, pkExtraData));
        unsigned int uiShapeID = 
            ((NiIntegerExtraData*)pkExtraData)->GetValue();
        pkAVObj->RemoveExtraData(ED_SHAPE_BASIC_PARENT);
        
        NiAVObject* pkShape;
        bool bFoundShape = m_kBasicShapeMap.GetAt(uiShapeID, pkShape);
        if (!bFoundShape)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024, "Could not find shape for object %s",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            return 0;
        }

        // Deal with local transform
        NiTransform kParentXform;
        kParentXform.m_Rotate = pkAVObj->GetWorldRotate();
        kParentXform.m_Translate = pkAVObj->GetWorldTranslate();
        kParentXform.m_fScale = pkAVObj->GetWorldScale();
        NiTransform kParentInv;
        kParentXform.Invert(kParentInv);
        NiTransform kChildXform;
        kChildXform.m_Rotate = pkShape->GetWorldRotate();
        kChildXform.m_Translate = pkShape->GetWorldTranslate();
        kChildXform.m_fScale = pkShape->GetWorldScale();
        NiTransform kLocalXform = kParentInv * kChildXform;    
        NxMat34 kNxXform;
        NiPhysXTypes::NiTransformToNxMat34(kLocalXform.m_Rotate,
            kLocalXform.m_Translate, kNxXform);
        kNxXform.t *= fInvScale * kLocalXform.m_fScale;

        NxSphereShapeDesc kSphereDesc;
        pkExtraData = pkShape->GetExtraData(ED_SHAPE_RAD);
        NIASSERT(pkExtraData);
        NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
        kSphereDesc.radius = fInvScale * kChildXform.m_fScale *
            ((NiFloatExtraData*)pkExtraData)->GetValue();
        kSphereDesc.localPose = kNxXform;

        pkShapeDesc = NiNew NiPhysXShapeDesc;
        pkShapeDesc->FromSphereDesc(kSphereDesc);

        pkShape->RemoveExtraData(ED_SHAPE_RAD);
    }
    else if (iShapeType == PHYSX_SHAPE_BOX)
    {
        // Get the ID for this shape
        pkExtraData = pkAVObj->GetExtraData(ED_SHAPE_BASIC_PARENT);
        if (!pkExtraData)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024, "No geometry for shape %s",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            return 0;
        }
        NIASSERT(NiIsKindOf(NiIntegerExtraData, pkExtraData));
        unsigned int uiShapeID = 
            ((NiIntegerExtraData*)pkExtraData)->GetValue();
        pkAVObj->RemoveExtraData(ED_SHAPE_BASIC_PARENT);
        
        NiAVObject* pkShape;
        bool bFoundShape = m_kBasicShapeMap.GetAt(uiShapeID, pkShape);
        if (!bFoundShape)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024, "Could not find shape for object %s",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            return 0;
        }

        // Deal with local transform
        NiTransform kParentXform;
        kParentXform.m_Rotate = pkAVObj->GetWorldRotate();
        kParentXform.m_Translate = pkAVObj->GetWorldTranslate();
        kParentXform.m_fScale = pkAVObj->GetWorldScale();
        NiTransform kParentInv;
        kParentXform.Invert(kParentInv);
        NiTransform kChildXform;
        kChildXform.m_Rotate = pkShape->GetWorldRotate();
        kChildXform.m_Translate = pkShape->GetWorldTranslate();
        kChildXform.m_fScale = pkShape->GetWorldScale();
        NiTransform kLocalXform = kParentInv * kChildXform;    
        NxMat34 kNxXform;
        NiPhysXTypes::NiTransformToNxMat34(kLocalXform.m_Rotate,
            kLocalXform.m_Translate, kNxXform);
        kNxXform.t *= fInvScale * kLocalXform.m_fScale;

        NxBoxShapeDesc kBoxDesc;
        pkExtraData = pkShape->GetExtraData(ED_SHAPE_DIM);
        NIASSERT(pkExtraData);
        NIASSERT(NiIsKindOf(NiFloatsExtraData, pkExtraData));
        kBoxDesc.dimensions.x =
            ((NiFloatsExtraData*)pkExtraData)->GetValue(0);
        kBoxDesc.dimensions.y =
            ((NiFloatsExtraData*)pkExtraData)->GetValue(1);
        kBoxDesc.dimensions.z =
            ((NiFloatsExtraData*)pkExtraData)->GetValue(2);

        pkExtraData = pkShape->GetExtraData(ED_SHAPE_OFFSET);
        NIASSERT(pkExtraData && NiIsKindOf(NiBooleanExtraData, pkExtraData));
        bool bOffset = ((NiBooleanExtraData*)pkExtraData)->GetValue();
        
        if (bOffset)
        {
            kBoxDesc.localPose.t.z = kBoxDesc.dimensions.z * 0.5f *
                fInvScale * kChildXform.m_fScale;
        }
        kBoxDesc.dimensions.x = kChildXform.m_fScale *
            NiAbs(kBoxDesc.dimensions.x) * 0.5f * fInvScale;
        kBoxDesc.dimensions.y = kChildXform.m_fScale *
            NiAbs(kBoxDesc.dimensions.y) * 0.5f * fInvScale;
        kBoxDesc.dimensions.z = kChildXform.m_fScale *
            NiAbs(kBoxDesc.dimensions.z) * 0.5f * fInvScale;
        
        kBoxDesc.localPose = kNxXform * kBoxDesc.localPose;

        pkShapeDesc = NiNew NiPhysXShapeDesc;
        pkShapeDesc->FromBoxDesc(kBoxDesc);
        
        pkShape->RemoveExtraData(ED_SHAPE_DIM);
        pkShape->RemoveExtraData(ED_SHAPE_OFFSET);
    }
    else if (iShapeType == PHYSX_SHAPE_CAPSULE)
    {
        // Get the ID for this shape
        pkExtraData = pkAVObj->GetExtraData(ED_SHAPE_BASIC_PARENT);
        if (!pkExtraData)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024, "No geometry for shape %s",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            return 0;
        }
        NIASSERT(NiIsKindOf(NiIntegerExtraData, pkExtraData));
        unsigned int uiShapeID = 
            ((NiIntegerExtraData*)pkExtraData)->GetValue();
        pkAVObj->RemoveExtraData(ED_SHAPE_BASIC_PARENT);
        
        NiAVObject* pkShape;
        bool bFoundShape = m_kBasicShapeMap.GetAt(uiShapeID, pkShape);
        if (!bFoundShape)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024, "Could not find shape for object %s",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            return 0;
        }

        // Deal with local transform
        NiTransform kParentXform;
        kParentXform.m_Rotate = pkAVObj->GetWorldRotate();
        kParentXform.m_Translate = pkAVObj->GetWorldTranslate();
        kParentXform.m_fScale = pkAVObj->GetWorldScale();
        NiTransform kParentInv;
        kParentXform.Invert(kParentInv);
        NiTransform kChildXform;
        kChildXform.m_Rotate = pkShape->GetWorldRotate();
        kChildXform.m_Translate = pkShape->GetWorldTranslate();
        kChildXform.m_fScale = pkShape->GetWorldScale();
        NiTransform kLocalXform = kParentInv * kChildXform;    
        NxMat34 kNxXform;
        NiPhysXTypes::NiTransformToNxMat34(kLocalXform.m_Rotate,
            kLocalXform.m_Translate, kNxXform);
        kNxXform.t *= fInvScale * kLocalXform.m_fScale;

        NxCapsuleShapeDesc kCapsDesc;
        
        pkExtraData = pkShape->GetExtraData(ED_SHAPE_RAD);
        NIASSERT(pkExtraData);
        NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
        kCapsDesc.radius = ((NiFloatExtraData*)pkExtraData)->GetValue() *
            fInvScale * kChildXform.m_fScale;

        pkExtraData = pkShape->GetExtraData(ED_SHAPE_HEIGHT);
        if (pkExtraData)
        {
            NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
            kCapsDesc.height = ((NiFloatExtraData*)pkExtraData)->GetValue()
                * fInvScale * kChildXform.m_fScale;

            kCapsDesc.localPose.M.rotX(NxMath::degToRad(90.0f));
            if (kCapsDesc.height > 0)
            {
                kCapsDesc.localPose.t.z = kCapsDesc.height * 0.5f
                    + kCapsDesc.radius;
            }
            else
            {
                kCapsDesc.localPose.t.z = kCapsDesc.height * 0.5f
                    - kCapsDesc.radius;
            }
            kCapsDesc.height = NiAbs(kCapsDesc.height);
            pkShape->RemoveExtraData(ED_SHAPE_HEIGHT);
        }
        else
        {
            pkExtraData = pkShape->GetExtraData(ED_SHAPE_DIM);
            NIASSERT(NiIsKindOf(NiFloatsExtraData, pkExtraData));
            
            NiPoint3 kP1;
            NiPoint3 kP2;
            kP1.x = ((NiFloatsExtraData*)pkExtraData)->GetValue(0);
            kP1.y = ((NiFloatsExtraData*)pkExtraData)->GetValue(1);
            kP1.z = ((NiFloatsExtraData*)pkExtraData)->GetValue(2);
            kP2.x = ((NiFloatsExtraData*)pkExtraData)->GetValue(3);
            kP2.y = ((NiFloatsExtraData*)pkExtraData)->GetValue(4);
            kP2.z = ((NiFloatsExtraData*)pkExtraData)->GetValue(5);
            pkShape->RemoveExtraData(ED_SHAPE_DIM);
            
            // Midpt first
            NiPoint3 kMidPt = 0.5f * (kP1 + kP2);
            NiPhysXTypes::NiPoint3ToNxVec3(kMidPt, kCapsDesc.localPose.t);
            kCapsDesc.localPose.t *= fInvScale * kChildXform.m_fScale;
            
            // Now set up rotation
            NiPoint3 kAxis = kP1 - kP2;
            kCapsDesc.height =
                kAxis.Unitize() * fInvScale * kChildXform.m_fScale;
            
            NiPoint3 kYAxis = NiPoint3::UNIT_Y;
            NiPoint3 kRotAxis = kAxis.Cross(kYAxis);
            if (kRotAxis.SqrLength() > 1.0e-5f)
            {
                float fSinAngle = kRotAxis.Unitize();
                float fAngle = asin(fSinAngle);
                NiQuaternion kRotQuat(fAngle, kRotAxis);
                NiPhysXTypes::NiQuaternionToNxMat33(kRotQuat,
                    kCapsDesc.localPose.M);
            }
        }
        
        kCapsDesc.localPose = kNxXform * kCapsDesc.localPose;

        pkShapeDesc = NiNew NiPhysXShapeDesc;
        pkShapeDesc->FromCapsuleDesc(kCapsDesc);
        
        pkShape->RemoveExtraData(ED_SHAPE_RAD);
    }
    else
    {
        NiOutputDebugString("Don't know about this shape type\n");
        return 0;
    }

    return pkShapeDesc;
}
//---------------------------------------------------------------------------
bool NiPhysXExtractorPlugin::ExtractCloth(NiPhysXProp* pkProp,
    NiPhysXPropDesc* pkPropDesc, NiAVObject* pkAVObj,
    NxCookingInterface* pkCookInterface, bool bHardware)
{
    // We know we have cloth on this body
    float fInvScale = 1.0f / pkProp->GetScaleFactor();
    
    // Work through creating an actor
    NiPhysXClothDesc* pkClothDesc = 0;
    NiTriShape* pkTriShape = 0;
    bool bSuccess = ExtractClothDesc(pkClothDesc, pkTriShape, pkAVObj,
        pkCookInterface, bHardware, fInvScale);    
    if (!bSuccess)
        return false;
        
    // We have everything but it isn't in the scene yet.
    NxMat34 kPose;
    NiPhysXTypes::NiTransformToNxMat34(
        pkTriShape->GetWorldRotate(), pkTriShape->GetWorldTranslate(), kPose);
    kPose.t *= fInvScale;
    pkClothDesc->SetGlobalPose(kPose);
    
    // Set up sources, destinations, update flags, etc.
    NiStringExtraData* pkOptKeep =
        (NiStringExtraData*)pkTriShape->GetExtraData("NiOptimizeKeep");
    if (!pkOptKeep)
    {
        pkOptKeep = NiNew NiStringExtraData("NiOptimizeKeep");
        pkTriShape->AddExtraData("NiOptimizeKeep", pkOptKeep);
    }
    pkAVObj->SetSelectiveUpdateTransformsOverride(true);

    // Prevent stripification    
    NiStringExtraData* pkNoStripify =
        (NiStringExtraData*)pkTriShape->GetExtraData("NiNoStripify");
    if (!pkNoStripify)
    {
        pkNoStripify = NiNew NiStringExtraData("NiNoStripify");
        pkTriShape->AddExtraData("NiNoStripify", pkNoStripify);
    }

    // Prevent removal of the geometry data after the mesh is loaded
    NIASSERT(pkTriShape->GetModelData());
    pkTriShape->GetModelData()->SetKeepFlags(NiGeometryData::KEEP_ALL);

    // Destination
    NiPhysXClothDest* pkDest = NiNew NiPhysXClothDest(pkTriShape);
    if (pkClothDesc->GetClothFlags() & NX_CLF_TEARABLE)
        pkDest->SetMeshGrowFactor(1.5f);
    else
        pkDest->SetMeshGrowFactor(1.0f);
    NiIntegerExtraData* pkNBTED = (NiIntegerExtraData*)
        pkTriShape->GetExtraData("NBTSet");
    if (pkNBTED)
    {
        pkDest->SetNBTSet(pkNBTED->GetValue());
        pkTriShape->RemoveExtraData("NBTSet");
    }
    pkProp->AddDestination(pkDest);
    
    pkClothDesc->SetDest(pkDest);

    // Add the actor to the Prop Desc
    pkPropDesc->AddCloth(pkClothDesc);

    // Later we need to post-process to set up attachment locations and
    // the set destination parent actors. Can't do it here because actors may
    // not have been processed yet.
    m_kClothMap.SetAt(pkTriShape, pkClothDesc);
    
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXExtractorPlugin::ExtractClothDesc(
    NiPhysXClothDesc*& pkClothDescReturn, NiTriShape*& pkTriShapeReturn,
    NiAVObject* pkAVObj, NxCookingInterface* pkCookInterface,
    bool bHardware, float fInvScale)
{
    pkClothDescReturn = NiNew NiPhysXClothDesc;
    
    NiExtraData* pkExtraData = 0;

    NiFixedString kName = pkAVObj->GetName();
    pkClothDescReturn->SetClothName(kName);

    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_THICKNESS);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
        pkClothDescReturn->SetThickness(
            ((NiFloatExtraData*)pkExtraData)->GetValue());
        pkAVObj->RemoveExtraData(ED_CLOTH_THICKNESS);
    }

    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_DENSITY);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
        pkClothDescReturn->SetDensity(
            ((NiFloatExtraData*)pkExtraData)->GetValue());
        pkAVObj->RemoveExtraData(ED_CLOTH_DENSITY);
    }

    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_BEND_STIFF);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
        pkClothDescReturn->SetBendingStiffness(
            ((NiFloatExtraData*)pkExtraData)->GetValue());
        pkAVObj->RemoveExtraData(ED_CLOTH_BEND_STIFF);
    }
    
    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_STR_STIFF);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
        pkClothDescReturn->SetStretchingStiffness(
            ((NiFloatExtraData*)pkExtraData)->GetValue());
        pkAVObj->RemoveExtraData(ED_CLOTH_STR_STIFF);
    }

    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_DAMPING_COEF);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
        pkClothDescReturn->SetDampingCoefficient(
            ((NiFloatExtraData*)pkExtraData)->GetValue());
        pkAVObj->RemoveExtraData(ED_CLOTH_DAMPING_COEF);
    }    

    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_FRICTION);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
        pkClothDescReturn->SetFriction(
            ((NiFloatExtraData*)pkExtraData)->GetValue());
        pkAVObj->RemoveExtraData(ED_CLOTH_FRICTION);
    }
    
    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_PRESSURE_VAL);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
        pkClothDescReturn->SetPressure(
            ((NiFloatExtraData*)pkExtraData)->GetValue());
        pkAVObj->RemoveExtraData(ED_CLOTH_PRESSURE_VAL);
    }
    
    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_TEAR_FACTOR);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
        pkClothDescReturn->SetTearFactor(
            ((NiFloatExtraData*)pkExtraData)->GetValue());
        pkAVObj->RemoveExtraData(ED_CLOTH_TEAR_FACTOR);
    }

    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_COLLIDE_RESP);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
        pkClothDescReturn->SetCollisionResponseCoefficient(
            ((NiFloatExtraData*)pkExtraData)->GetValue());
        pkAVObj->RemoveExtraData(ED_CLOTH_COLLIDE_RESP);
    }
    
    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_ATTACH_RESP);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
        pkClothDescReturn->SetAttachmentResponseCoefficient(
            ((NiFloatExtraData*)pkExtraData)->GetValue());
        pkAVObj->RemoveExtraData(ED_CLOTH_ATTACH_RESP);
    }
    
    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_ATTACH_TEAR);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
        pkClothDescReturn->SetAttachmentTearFactor(
            ((NiFloatExtraData*)pkExtraData)->GetValue());
        pkAVObj->RemoveExtraData(ED_CLOTH_ATTACH_TEAR);
    }
    
    pkExtraData = pkAVObj->GetExtraData(ED_SOLVER_ITERATIONS);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiIntegerExtraData, pkExtraData));
        pkClothDescReturn->SetSolverIterations(
            ((NiIntegerExtraData*)pkExtraData)->GetValue());
        pkAVObj->RemoveExtraData(ED_SOLVER_ITERATIONS);
    }

    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_EXT_ACCEL);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiFloatsExtraData, pkExtraData));
        NxVec3 kAcceleration;
        kAcceleration.x = ((NiFloatsExtraData*)pkExtraData)->GetValue(0);
        kAcceleration.y = ((NiFloatsExtraData*)pkExtraData)->GetValue(1);
        kAcceleration.z = ((NiFloatsExtraData*)pkExtraData)->GetValue(2);
        pkClothDescReturn->SetExternalAcceleration(kAcceleration);
        pkAVObj->RemoveExtraData(ED_CLOTH_EXT_ACCEL);
    }
        
    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_WAKE_COUNTER);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
        pkClothDescReturn->SetWakeUpCounter(
            ((NiFloatExtraData*)pkExtraData)->GetValue());
        pkAVObj->RemoveExtraData(ED_CLOTH_WAKE_COUNTER);
    }

    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_SLEEP_VEL);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiFloatExtraData, pkExtraData));
        pkClothDescReturn->SetSleepLinearVelocity(
            ((NiFloatExtraData*)pkExtraData)->GetValue());
        pkAVObj->RemoveExtraData(ED_CLOTH_SLEEP_VEL);
    }

    pkExtraData = pkAVObj->GetExtraData(ED_ACTOR_GROUP);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiIntegerExtraData, pkExtraData));
        pkClothDescReturn->SetCollisionGroup(
            ((NiIntegerExtraData*)pkExtraData)->GetValue());
        pkAVObj->RemoveExtraData(ED_ACTOR_GROUP);
    }
    
    NxU32 uiFlags = 0;
    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_PRESSURE);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiBooleanExtraData, pkExtraData));
        if (((NiBooleanExtraData*)pkExtraData)->GetValue())
            uiFlags |= NX_CLF_PRESSURE;
        pkAVObj->RemoveExtraData(ED_CLOTH_PRESSURE);
    }

    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_STATIC);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiBooleanExtraData, pkExtraData));
        if (((NiBooleanExtraData*)pkExtraData)->GetValue())
            uiFlags |= NX_CLF_STATIC;
        pkAVObj->RemoveExtraData(ED_CLOTH_STATIC);
    }

    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_DISABLE_COLL);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiBooleanExtraData, pkExtraData));
        if (((NiBooleanExtraData*)pkExtraData)->GetValue())
            uiFlags |= NX_CLF_DISABLE_COLLISION;
        pkAVObj->RemoveExtraData(ED_CLOTH_DISABLE_COLL);
    }

    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_SELF_COLLIDE);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiBooleanExtraData, pkExtraData));
        if (((NiBooleanExtraData*)pkExtraData)->GetValue())
            uiFlags |= NX_CLF_SELFCOLLISION;
        pkAVObj->RemoveExtraData(ED_CLOTH_SELF_COLLIDE);
    }

    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_GRAVITY);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiBooleanExtraData, pkExtraData));
        if (((NiBooleanExtraData*)pkExtraData)->GetValue())
            uiFlags |= NX_CLF_GRAVITY;
        pkAVObj->RemoveExtraData(ED_CLOTH_GRAVITY);
    }

    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_BENDING);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiBooleanExtraData, pkExtraData));
        if (((NiBooleanExtraData*)pkExtraData)->GetValue())
            uiFlags |= NX_CLF_BENDING;
        pkAVObj->RemoveExtraData(ED_CLOTH_BENDING);
    }

    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_ORTHO_BEND);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiBooleanExtraData, pkExtraData));
        if (((NiBooleanExtraData*)pkExtraData)->GetValue())
            uiFlags |= NX_CLF_BENDING_ORTHO;
        pkAVObj->RemoveExtraData(ED_CLOTH_ORTHO_BEND);
    }

    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_DAMPING);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiBooleanExtraData, pkExtraData));
        if (((NiBooleanExtraData*)pkExtraData)->GetValue())
            uiFlags |= NX_CLF_DAMPING;
        pkAVObj->RemoveExtraData(ED_CLOTH_DAMPING);
    }

    bool bTwoWay = false;
    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_TWO_COLLIDE);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiBooleanExtraData, pkExtraData));
        if (((NiBooleanExtraData*)pkExtraData)->GetValue())
        {
            uiFlags |= NX_CLF_COLLISION_TWOWAY;
            bTwoWay = true;
        }
        pkAVObj->RemoveExtraData(ED_CLOTH_TWO_COLLIDE);
    }

    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_TRI_COLLIDE);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiBooleanExtraData, pkExtraData));
        if (((NiBooleanExtraData*)pkExtraData)->GetValue())
            uiFlags |= NX_CLF_TRIANGLE_COLLISION;
        pkAVObj->RemoveExtraData(ED_CLOTH_TRI_COLLIDE);
    }

    bool bTearable = false;
    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_TEARABLE);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiBooleanExtraData, pkExtraData));
        if (((NiBooleanExtraData*)pkExtraData)->GetValue())
        {
            uiFlags |= NX_CLF_TEARABLE;
            bTearable = true;
        }
        pkAVObj->RemoveExtraData(ED_CLOTH_TEARABLE);
    }

    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_HARDWARE);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiBooleanExtraData, pkExtraData));
        bHardware = ((NiBooleanExtraData*)pkExtraData)->GetValue();
        pkAVObj->RemoveExtraData(ED_CLOTH_HARDWARE);
    }
    if (bHardware)
        uiFlags |= NX_CLF_HARDWARE;
    
    pkClothDescReturn->SetClothFlags(uiFlags);

    pkExtraData = pkAVObj->GetExtraData(ED_CLOTH_ATTACHMENT);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiIntegersExtraData, pkExtraData));
        int* piArray;
        unsigned int uiSize;
        ((NiIntegersExtraData*)pkExtraData)->GetArray(uiSize, piArray);

        NiFloatsExtraData* pkPosnsED = (NiFloatsExtraData*)
            pkAVObj->GetExtraData(ED_CLOTH_ATTACH_POSN);
        float* pfPosnsArray = 0;
        unsigned int uiPosnsSize = 0;
        if (pkPosnsED)
            pkPosnsED->GetArray(uiPosnsSize, pfPosnsArray);

        unsigned int uiPosn = 0;
        unsigned int uiPosnsPosn = 0;
        while (uiPosn < uiSize)
        {
            NiPhysXClothDesc::ClothAttachment* pkAttachment = 0;
            bool bHasShape = (piArray[uiPosn] >= 0);
            if (bHasShape)
            {
                unsigned int uiAttachmentID = (unsigned int)piArray[uiPosn];
                if (!m_kAttachmentMap.GetAt(uiAttachmentID, pkAttachment))
                {
                    pkAttachment = NiNew NiPhysXClothDesc::ClothAttachment;
                    m_kAttachmentMap.SetAt(uiAttachmentID, pkAttachment);
                }
                else
                {
                    // Attachment has now been fully processed, to remove
                    // from map. It is owned by the cloth desc.
                    m_kAttachmentMap.SetAt(uiAttachmentID, 0);
                }
            }
            else
            {
                pkAttachment = NiNew NiPhysXClothDesc::ClothAttachment;
            }
            uiPosn++;
            
            unsigned int uiNumVerts = piArray[uiPosn];
            uiPosn++;

            pkAttachment->m_uiVertexCount = uiNumVerts;
            if (uiNumVerts)
            {
                if (!pkPosnsED)
                {
                    char pcMsg[1024];
                    NiSprintf(pcMsg, 1024, "Cloth attachment positions must "
                        "be given if attachment vertices are given: %s",
                        pkAVObj->GetName());
                    if (m_pkLogger)
                    {
                        m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
                    }
                    else
                    {
                        NiMessageBox(pcMsg, "PhysX Extractor Error");
                    }
                    NiDelete pkClothDescReturn;
                    return false;
                }
            
                pkAttachment->m_puiVertexIDs =
                    NiAlloc(unsigned int, uiNumVerts);
                pkAttachment->m_pkPositions = NiAlloc(NxVec3, uiNumVerts);
                pkAttachment->m_puiFlags = NiAlloc(unsigned int, uiNumVerts);
                
                for (unsigned int ui = 0; ui < uiNumVerts; ui++)
                {
                    pkAttachment->m_puiVertexIDs[ui] = (unsigned int)(~0);
                    pkAttachment->m_pkPositions[ui].x =
                        pfPosnsArray[uiPosnsPosn++] * fInvScale;
                    pkAttachment->m_pkPositions[ui].y =
                        pfPosnsArray[uiPosnsPosn++] * fInvScale;
                    pkAttachment->m_pkPositions[ui].z =
                        pfPosnsArray[uiPosnsPosn++] * fInvScale;

                    pkAttachment->m_puiFlags[ui] = 0;
                    if (bTearable)
                    {
                        pkAttachment->m_puiFlags[ui] |=
                            NX_CLOTH_ATTACHMENT_TEARABLE;
                    }
                    if (bTwoWay)
                    {
                        pkAttachment->m_puiFlags[ui] |=
                            NX_CLOTH_ATTACHMENT_TWOWAY;
                    }
                }
            }
            else
            {
                pkAttachment->m_puiFlags = NiAlloc(unsigned int, 1);
                pkAttachment->m_puiFlags[0] = 0;
                if (bTearable)
                {
                    pkAttachment->m_puiFlags[0] |=
                        NX_CLOTH_ATTACHMENT_TEARABLE;
                }
                if (bTwoWay)
                {
                    pkAttachment->m_puiFlags[0] |=
                        NX_CLOTH_ATTACHMENT_TWOWAY;
                }
            }
            pkClothDescReturn->AddAttachment(pkAttachment);

            // Move past the -1
            uiPosn++;
        }     

        pkAVObj->RemoveExtraData(ED_CLOTH_ATTACHMENT);
        pkAVObj->RemoveExtraData(ED_CLOTH_ATTACH_POSN);
    }

    bool bGotMesh = ExtractClothMesh(pkClothDescReturn, pkTriShapeReturn,
        pkAVObj, pkCookInterface, fInvScale, bHardware);
    if (!bGotMesh)
    {
        NiDelete pkClothDescReturn;
        return false;
    }

    if (!CheckClothAttachments(pkClothDescReturn))
    {
        char pcMsg[1024];
        NiSprintf(pcMsg, 1024, "Cloth attachments failed: %s",
            pkAVObj->GetName());
        if (m_pkLogger)
        {
            m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
        }
        else
        {
            NiMessageBox(pcMsg, "PhysX Extractor Error");
        }
        NiDelete pkClothDescReturn;
        return false;
    }

    if (!pkClothDescReturn->IsValid())
    {
        char pcMsg[1024];
        NiSprintf(pcMsg, 1024, "Cloth is invalid: %s", pkAVObj->GetName());
        if (m_pkLogger)
        {
            m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
        }
        else
        {
            NiMessageBox(pcMsg, "PhysX Extractor Error");
        }
        NiDelete pkClothDescReturn;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXExtractorPlugin::ExtractClothMesh(
    NiPhysXClothDesc* pkClothDesc, NiTriShape*& pkTriShapeReturn,
    NiAVObject* pkAVObj, NxCookingInterface* pkCookInterface,
    float fInvScale, bool bHardware)
{
    NiPhysXMeshDesc* pkMeshDesc = 0;
    
    // Need to find all Gamebryo NiTriShape nodes associated with this node.
    // If there is more than 1 it is an error. If it has been stripified it
    // is an error.
    // Must mark mesh as non-stripify.
    // Must return tri shape node.
    // Must return mesh object after cooking.
    NiExtraData* pkExtraData = 0;

    pkExtraData = pkAVObj->GetExtraData(ED_PROXY_TARGET);
    if (pkExtraData)
    {
        char pcMsg[1024];
        NiSprintf(pcMsg, 1024, "Cloth cannot have a proxy object: %s",
            pkAVObj->GetName());
        if (m_pkLogger)
        {
            m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
        }
        else
        {
            NiMessageBox(pcMsg, "PhysX Extractor Error");
        }
        
        pkAVObj->RemoveExtraData(ED_PROXY_TARGET);
        
        return false;
    }

    pkExtraData = pkAVObj->GetExtraData(ED_SHAPE_COMPOUND);
    if (pkExtraData)
    {
        // Get the ID.
        NIASSERT(pkExtraData && NiIsKindOf(NiIntegerExtraData, pkExtraData));
        unsigned int uiID = ((NiIntegerExtraData*)pkExtraData)->GetValue();
        pkAVObj->RemoveExtraData(ED_SHAPE_COMPOUND);
        
        // Get the array of nodes providing shapes.
        NiPhysXExtractorPlugin::NiAVObjectArray* pkPieces;
        m_kShapeGroupMap.GetAt(uiID, pkPieces);
        if (!pkPieces)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024, "No pieces found for compound object %s",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            return false;
        }
        
        if (pkPieces->GetSize() != 1)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024,
                "Cloth cannot be compound object with multipel pieces: %s",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            return false;
        }

        pkAVObj = pkPieces->GetAt(0);
    }
    
    pkExtraData = pkAVObj->GetExtraData(ED_SHAPE_TYPE);
    NIASSERT(pkExtraData && NiIsKindOf(NiIntegerExtraData, pkExtraData));
    int iShapeType = ((NiIntegerExtraData*)pkExtraData)->GetValue();
    pkAVObj->RemoveExtraData(ED_SHAPE_TYPE);

    NiAVObject* pkMeshAVObj;
    int iMeshID;
    if (iShapeType == PHYSX_SHAPE_MESH || iShapeType == PHYSX_SHAPE_CONVEX)
    {
        // Figure out which mesh group we are
        pkExtraData = pkAVObj->GetExtraData(ED_SHAPE_MESH_PARENT);
        if (!pkExtraData)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024, "No meshes for shape %s",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            return false;
        }
        NIASSERT(pkExtraData && NiIsKindOf(NiIntegerExtraData, pkExtraData));
        iMeshID = ((NiIntegerExtraData*)pkExtraData)->GetValue();
        pkAVObj->RemoveExtraData(ED_SHAPE_MESH_PARENT);

        if (m_kClothMeshesMap.GetAt(iMeshID, pkMeshDesc))
        {
            // Have this mesh already
            pkClothDesc->SetMeshDesc(pkMeshDesc);
            return true;
        }

        NiPhysXShapeDesc* pkShapeDesc;
        if (m_kMeshesMap.GetAt(iMeshID, pkShapeDesc))
        {
            // Have it already
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024,
                "Cannot use instanced geometry for cloth unless\n"
                "all instances are cloth: %s\n",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            return false;
        }
        
        // Find the meshes in the group
        NiPhysXExtractorPlugin::NiAVObjectArray* pkMeshes;
        m_kMeshGroupMap.GetAt(iMeshID, pkMeshes);
        if (!pkMeshes)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024, "Could not get meshes for shape %s",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            return 0;
        }
        if (pkMeshes->GetSize() != 1)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024, "Cloth targets must be a single mesh: %s",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            return 0;
        }

        pkMeshAVObj = pkMeshes->GetAt(0);
    }    
    else if (iShapeType == PHYSX_SHAPE_SPHERE ||
        iShapeType == PHYSX_SHAPE_BOX ||
        iShapeType == PHYSX_SHAPE_CAPSULE)
    {
        // Get the ID for this shape
        pkExtraData = pkAVObj->GetExtraData(ED_SHAPE_BASIC_PARENT);
        if (!pkExtraData)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024, "No geometry for shape %s",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            return false;
        }
        NIASSERT(NiIsKindOf(NiIntegerExtraData, pkExtraData));
        iMeshID = ((NiIntegerExtraData*)pkExtraData)->GetValue();
        pkAVObj->RemoveExtraData(ED_SHAPE_BASIC_PARENT);
        
        if (m_kClothMeshesMap.GetAt(iMeshID, pkMeshDesc))
        {
            // Have this mesh already
            pkClothDesc->SetMeshDesc(pkMeshDesc);
            return true;
        }

        bool bFoundShape = m_kBasicShapeMap.GetAt(iMeshID, pkMeshAVObj);
        if (!bFoundShape)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024, "Could not find shape for object %s",
                pkAVObj->GetName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            return false;
        }

        pkMeshAVObj->RemoveExtraData(ED_SHAPE_RAD);
        pkMeshAVObj->RemoveExtraData(ED_SHAPE_DIM);
        pkMeshAVObj->RemoveExtraData(ED_SHAPE_OFFSET);
        pkMeshAVObj->RemoveExtraData(ED_SHAPE_HEIGHT);
    }
    else
    {
        NiOutputDebugString("Don't know about this shape type\n");
        return false;
    }

    if (!pkMeshAVObj)
        return false;

    // Check we are dealing with a tri shape object
    if (!NiIsExactKindOf(NiTriShape, pkMeshAVObj))
    {
        char pcMsg[1024];
        NiSprintf(pcMsg, 1024,
            "Cloth targets must be NiTriShape objects: %s\n"
            "Do you have stripification turned on? If so, add a\n"
            "\"NiNoStripify\" extra data tag to the cloth\n",
            pkAVObj->GetName());
        if (m_pkLogger)
        {
            m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
        }
        else
        {
            NiMessageBox(pcMsg, "PhysX Extractor Error");
        }
        return 0;
    }
    pkTriShapeReturn = (NiTriShape*)pkMeshAVObj;

    NiTriShapeData* pkTriData =
        (NiTriShapeData*)pkTriShapeReturn->GetModelData();

    // Extract mesh out of Gamebryo. Vertices need to be unique, which
    // makes things more difficult.
    unsigned short usNumVerts = pkTriData->GetVertexCount();
    unsigned short usNumTris = pkTriData->GetTriangleCount();
    NxVec3* pkNxVerts = NiAlloc(NxVec3, usNumVerts);
    NxU16* pkNxTriData = NiAlloc(NxU16, usNumTris * 3);

    NiTMap<NxVec3*, unsigned short, NiPhysXNxVec3HashFunctor,
        NiPhysXNxVec3EqualsFunctor> kVertexMap(NextPrime(usNumVerts));
    unsigned short* pusIndexMap = NiAlloc(unsigned short, usNumVerts);
    
    NiPoint3* pkVertices = pkTriData->GetVertices();
    float fScale = pkTriShapeReturn->GetWorldScale();

    unsigned short usNumNxVerts = 0;
    for (unsigned short us = 0; us < usNumVerts; us++)
    {
        NxVec3 kLocalPt;
        NiPhysXTypes::NiPoint3ToNxVec3(pkVertices[us], kLocalPt);
        unsigned short usIndex = 0;
        bool bExisting = kVertexMap.GetAt(&kLocalPt, usIndex);
        if (bExisting)
        {
            pusIndexMap[us] = usIndex;
        }
        else
        {
            pkNxVerts[usNumNxVerts] = kLocalPt;
            pusIndexMap[us] = usNumNxVerts;
            kVertexMap.SetAt(pkNxVerts + usNumNxVerts, usNumNxVerts);
            usNumNxVerts++;
        }
    }
    for (unsigned short us = 0; us < usNumNxVerts; us++)
    {
        pkNxVerts[us] *= fScale * fInvScale;
    }
    
    // Get faces
    unsigned short usTriIndex = 0;
    for ( unsigned short us = 0; us < usNumTris; us++)
    {
        unsigned short usV1;
        unsigned short usV2;
        unsigned short usV3;
        pkTriData->GetTriangleIndices(us, usV1, usV2, usV3);
        pkNxTriData[usTriIndex++] = pusIndexMap[usV1];
        pkNxTriData[usTriIndex++] = pusIndexMap[usV2];
        pkNxTriData[usTriIndex++] = pusIndexMap[usV3];
    }
    
    // Set up and cook.
    NxClothMeshDesc kClothMeshDesc;
    
    kClothMeshDesc.numVertices = usNumNxVerts;
    kClothMeshDesc.numTriangles = usNumTris;
    kClothMeshDesc.pointStrideBytes = sizeof(NxVec3);
    kClothMeshDesc.triangleStrideBytes = 3 * sizeof(NxU16);
    kClothMeshDesc.points = pkNxVerts;
    kClothMeshDesc.triangles = pkNxTriData;
    
    // Can safely use 16 bit indices because we are using Gamebryo geometry,
    // which has this limitation already.
    kClothMeshDesc.flags = NX_MF_16_BIT_INDICES;
    kClothMeshDesc.target = NX_CLOTH_MESH_UNIVERSAL;
    
    NIASSERT(kClothMeshDesc.isValid());

    NiPhysXMemStream* pkCookStream = NiNew NiPhysXMemStream();
    bool bCookRes =
        pkCookInterface->NxCookClothMesh(kClothMeshDesc, *pkCookStream);
    if (!bCookRes)
    {
        char pcMsg[1024];
        NiSprintf(pcMsg, 1024, "Could not cook cloth mesh: %s",
            pkAVObj->GetName());
        if (m_pkLogger)
        {
            m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
        }
        else
        {
            NiMessageBox(pcMsg, "PhysX Extractor Error");
        }
        NiFree(pkNxVerts);
        NiFree(pkNxTriData);
        NiFree(pusIndexMap);
        NiDelete pkCookStream;
        return 0;
    }
    unsigned int uiMeshSize = pkCookStream->GetSize();
    unsigned char* pucMeshData = (unsigned char*)pkCookStream->GetBuffer();
    StashMeshVersion(pucMeshData);

    // Get the world transform of the tri shape
    NiTransform kNiXform = pkTriShapeReturn->GetWorldTransform();
    NxMat34 kNxXform;
    NiPhysXTypes::NiTransformToNxMat34(kNiXform.m_Rotate,
        kNiXform.m_Translate, kNxXform);
    kNxXform.t *= fInvScale;
    
    for (unsigned short us = 0; us < usNumNxVerts; us++)
    {
        // Now look for the vertex IDs of attached vertices. Put this vertex
        // into world space ...
        NxVec3 kWorldPt = kNxXform * pkNxVerts[us];
        
        ProcessAttachments(pkClothDesc, kWorldPt, us);
    }
    
    pkMeshDesc = NiNew NiPhysXMeshDesc;
    pkMeshDesc->SetIsCloth();
    pkMeshDesc->SetPlatform(
        pkCookInterface->NxGetCookingParams().targetPlatform);
    pkMeshDesc->SetHardware(bHardware);
    pkMeshDesc->SetName(pkAVObj->GetName());
    pkMeshDesc->SetFlags(0);
    pkMeshDesc->SetData(uiMeshSize, pucMeshData);
    pkMeshDesc->SetVertexMap(usNumVerts, pusIndexMap);
    pkClothDesc->SetMeshDesc(pkMeshDesc);
    
    m_kClothMeshesMap.SetAt(iMeshID, pkMeshDesc);
    
    NiFree(pkNxVerts);
    NiFree(pkNxTriData);
    NiDelete pkCookStream;

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXExtractorPlugin::ProcessAttachments(NiPhysXClothDesc* pkClothDesc,
    NxVec3& kWorldPt, unsigned short us)
{
    unsigned int uiNumAttachments = pkClothDesc->GetAttachmentCount();
    for (unsigned int ui = 0; ui < uiNumAttachments; ui++)
    {
        NiPhysXClothDesc::ClothAttachment* pkAttachment =
            pkClothDesc->GetAttachmentAt(ui);
        if (!pkAttachment)
            continue;
        for (unsigned int uj = 0; uj < pkAttachment->m_uiVertexCount; uj++)
        {
            NxVec3 kWorldAttach = pkAttachment->m_pkPositions[uj];
            NxVec3 kDiff = kWorldAttach - kWorldPt;
            if (kDiff.magnitudeSquared() < 0.000001)
                pkAttachment->m_puiVertexIDs[uj] = us;
        }
    }
}
//---------------------------------------------------------------------------
bool NiPhysXExtractorPlugin::CheckClothAttachments(
    NiPhysXClothDesc* pkClothDesc)
{
    unsigned int uiNumAttachments = pkClothDesc->GetAttachmentCount();
    for (unsigned int ui = 0; ui < uiNumAttachments; ui++)
    {
        NiPhysXClothDesc::ClothAttachment* pkAttachment =
            pkClothDesc->GetAttachmentAt(ui);
        if (!pkAttachment)
            continue;
        for (unsigned int uj = 0; uj < pkAttachment->m_uiVertexCount; uj++)
        {
            if (pkAttachment->m_puiVertexIDs[uj] == (unsigned int)(~0))
                return false;
        }
    }
    
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXExtractorPlugin::PostProcessActors()
{
    // Set up actor parents in destinations

    NiTMapIterator kIter = m_kActorMap.GetFirstPos();
    while (kIter)
    {
        NiAVObject* pkAVObj;
        NiPhysXActorDescPtr spActorDesc;
        m_kActorMap.GetNext(kIter, pkAVObj, spActorDesc);

        NiAVObject* pkParentNode = pkAVObj->GetParent();
        if (!pkParentNode)
            continue;
            
        NiPhysXActorDescPtr spParentActorDesc;
        bool bFoundDesc = m_kActorMap.GetAt(pkParentNode, spParentActorDesc);
        if (!bFoundDesc)
            continue;
            
        spActorDesc->SetActorParent(spParentActorDesc);
    }    

    kIter = m_kClothMap.GetFirstPos();
    while (kIter)
    {
        NiAVObject* pkAVObj;
        NiPhysXClothDescPtr spClothDesc;
        m_kClothMap.GetNext(kIter, pkAVObj, spClothDesc);
        
        NiAVObject* pkParentNode = pkAVObj->GetParent();
        if (!pkParentNode)
            continue;
            
        NiPhysXActorDescPtr spActorDesc;
        bool bFoundDesc = m_kActorMap.GetAt(pkParentNode, spActorDesc);
        if (!bFoundDesc)
            continue;
            
        spClothDesc->SetParentActor(spActorDesc);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXExtractorPlugin::ExtractJoints(NiPhysXPropDesc* pkPropDesc,
    float fInvScale)
{
    NiTMapIterator iter = m_kJointsMap.GetFirstPos();
    while (iter)
    {
        unsigned int uiID;
        NiAVObject* pkChildNode;
        unsigned int uiChildIndex;
        NiPhysXActorDescPtr spChildDesc;
        NiAVObject* pkParentNode;
        unsigned int uiParentIndex;
        NiPhysXActorDescPtr spParentDesc;

        JointData* pkJointData;
        m_kJointsMap.GetNext(iter, uiID, pkJointData);
        NIASSERT(pkJointData);
        
        pkChildNode = pkJointData->pkChildNode;
        uiChildIndex = pkJointData->uiChildIndex;
        pkParentNode = pkJointData->pkParentNode;
        uiParentIndex = pkJointData->uiParentIndex;
        
        bool bFoundDesc = m_kActorMap.GetAt(pkChildNode, spChildDesc);
        if (!bFoundDesc)
        {
            return false;
        }
        else if (spChildDesc->GetBodyDesc() == 0)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024,
                "Invalid PhysX joint: Joint child actor %s must be dynamic",
                spChildDesc->GetActorName());
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            spChildDesc = 0;
            spParentDesc = 0;
            return false;
        }

        NiTransform kParentXform;
        kParentXform.MakeIdentity();
        if (pkParentNode)
        {
            bFoundDesc = m_kActorMap.GetAt(pkParentNode, spParentDesc);
            if (!bFoundDesc || spParentDesc->GetBodyDesc() == 0)
            {
                // Parent could be non-phsyical or could be static
                // geometry. In either case, we will attach to the world
                // frame instead.
                kParentXform.m_Rotate = pkParentNode->GetWorldRotate();
                kParentXform.m_Translate = pkParentNode->GetWorldTranslate();
                kParentXform.m_fScale = pkParentNode->GetWorldScale();
                pkParentNode = 0;
                spParentDesc = 0;
            }
        }
        else
        {
            spParentDesc = 0;
        }

        NxD6JointDesc kJointDesc;
        if (pkChildNode->GetExtraData(ED_MAX_JOINT_POSN))
        {
            if (!ExtractMaxJoint(pkPropDesc, fInvScale, pkChildNode,
                uiChildIndex, pkParentNode, uiParentIndex, kJointDesc))
            {
                spChildDesc = 0;
                spParentDesc = 0;
                return false;
            }
        }
        else
        {
            if (!ExtractD6Joint(pkPropDesc, fInvScale, pkChildNode,
                uiChildIndex, kParentXform, kJointDesc))
            {
                spChildDesc = 0;
                spParentDesc = 0;
                return false;
            }
        }

        // Check joint validity
	    if (kJointDesc.flags & NX_D6JOINT_SLERP_DRIVE && (
            kJointDesc.swing1Motion == NX_D6JOINT_MOTION_LOCKED ||
            kJointDesc.swing2Motion == NX_D6JOINT_MOTION_LOCKED ||
            kJointDesc.twistMotion == NX_D6JOINT_MOTION_LOCKED))
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024,
                "%s: Invalid PhysX joint: Slerp drive with angular DOF locked",
                kJointDesc.name ? kJointDesc.name : "<No Name>");
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            spChildDesc = 0;
            spParentDesc = 0;
            return false;
	    }
	    if (kJointDesc.flags & NX_D6JOINT_GEAR_ENABLED &&
	        kJointDesc.twistMotion == NX_D6JOINT_MOTION_LOCKED)
        {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024,
                "%s: Invalid PhysX joint: Gear enabled with twist DOF locked",
                kJointDesc.name ? kJointDesc.name : "<No Name>");
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            spChildDesc = 0;
            spParentDesc = 0;
            return false;
	    }
	    if (spChildDesc == spParentDesc)
	    {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024,
                "%s: Invalid PhysX joint: Actor cannot be jointed to itself",
                kJointDesc.name ? kJointDesc.name : "<No Name>");
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            spChildDesc = 0;
            spParentDesc = 0;
            return false;
	    }
	    if (kJointDesc.maxForce <= 0.0f || kJointDesc.maxTorque <= 0.0f)
	    {
            char pcMsg[1024];
            NiSprintf(pcMsg, 1024,
                "%s: Invalid PhysX joint: Joint force and torque must be +ve",
                kJointDesc.name ? kJointDesc.name : "<No Name>");
            if (m_pkLogger)
            {
                m_pkLogger->LogElement("PhysXExtractorError", pcMsg);
            }
            else
            {
                NiMessageBox(pcMsg, "PhysX Extractor Error");
            }
            spChildDesc = 0;
            spParentDesc = 0;
            return false;
	    }

        NiPhysXD6JointDesc* pkJointDesc = NiNew NiPhysXD6JointDesc();

        pkJointDesc->SetD6JointDesc(kJointDesc);
        pkJointDesc->SetActor(0, spParentDesc);
        pkJointDesc->SetActor(1, spChildDesc);
		
        pkPropDesc->AddJoint(pkJointDesc);

        spChildDesc = 0;
        spParentDesc = 0;
    }

    // Go through again and remove all the extra data
    iter = m_kJointsMap.GetFirstPos();
    while (iter)
    {
        unsigned int uiID;
        NiAVObject* pkChildNode;

        JointData* pkJointData;
        m_kJointsMap.GetNext(iter, uiID, pkJointData);
        NIASSERT(pkJointData);
        
        pkChildNode = pkJointData->pkChildNode;

        if (pkChildNode->GetExtraData(ED_JOINT_POSN1))
            pkChildNode->RemoveExtraData(ED_JOINT_POSN1);
        if (pkChildNode->GetExtraData(ED_JOINT_AXIS1))
            pkChildNode->RemoveExtraData(ED_JOINT_AXIS1);
        if (pkChildNode->GetExtraData(ED_JOINT_NORMAL1))
            pkChildNode->RemoveExtraData(ED_JOINT_NORMAL1);
        if (pkChildNode->GetExtraData(ED_JOINT_POSN2))
            pkChildNode->RemoveExtraData(ED_JOINT_POSN2);
        if (pkChildNode->GetExtraData(ED_JOINT_AXIS2))
            pkChildNode->RemoveExtraData(ED_JOINT_AXIS2);
        if (pkChildNode->GetExtraData(ED_JOINT_NORMAL2))
            pkChildNode->RemoveExtraData(ED_JOINT_NORMAL2);
        if (pkChildNode->GetExtraData(ED_JOINT_LOCKS))
            pkChildNode->RemoveExtraData(ED_JOINT_LOCKS);
        if (pkChildNode->GetExtraData(ED_JOINT_LIMITS))
            pkChildNode->RemoveExtraData(ED_JOINT_LIMITS);
        if (pkChildNode->GetExtraData(ED_JOINT_RESTITUTIONS))
            pkChildNode->RemoveExtraData(ED_JOINT_RESTITUTIONS);
        if (pkChildNode->GetExtraData(ED_JOINT_SPRINGS))
            pkChildNode->RemoveExtraData(ED_JOINT_SPRINGS);
        if (pkChildNode->GetExtraData(ED_JOINT_DAMPINGS))
            pkChildNode->RemoveExtraData(ED_JOINT_DAMPINGS);
        if (pkChildNode->GetExtraData(ED_JOINT_MAX_FORCE))
            pkChildNode->RemoveExtraData(ED_JOINT_MAX_FORCE);
        if (pkChildNode->GetExtraData(ED_JOINT_MAX_TORQUE))
            pkChildNode->RemoveExtraData(ED_JOINT_MAX_TORQUE);
        if (pkChildNode->GetExtraData(ED_JOINT_NAME))
            pkChildNode->RemoveExtraData(ED_JOINT_NAME);
        if (pkChildNode->GetExtraData(ED_JOINT_COLLIDES))
            pkChildNode->RemoveExtraData(ED_JOINT_COLLIDES);
        if (pkChildNode->GetExtraData(ED_JOINT_PROJ_MODE))
            pkChildNode->RemoveExtraData(ED_JOINT_PROJ_MODE);
        if (pkChildNode->GetExtraData(ED_JOINT_PROJ_DIST))
            pkChildNode->RemoveExtraData(ED_JOINT_PROJ_DIST);
        if (pkChildNode->GetExtraData(ED_JOINT_PROJ_ANGLE))
            pkChildNode->RemoveExtraData(ED_JOINT_PROJ_ANGLE);
        if (pkChildNode->GetExtraData(ED_JOINT_GEARING))
            pkChildNode->RemoveExtraData(ED_JOINT_GEARING);
        if (pkChildNode->GetExtraData(ED_JOINT_GEAR_RATIO))
            pkChildNode->RemoveExtraData(ED_JOINT_GEAR_RATIO);
        if (pkChildNode->GetExtraData(ED_MAX_JOINT_POSN))
            pkChildNode->RemoveExtraData(ED_MAX_JOINT_POSN);
        if (pkChildNode->GetExtraData(ED_MAX_JOINT_AXIS))
            pkChildNode->RemoveExtraData(ED_MAX_JOINT_AXIS);
        if (pkChildNode->GetExtraData(ED_MAX_JOINT_NORMAL))
            pkChildNode->RemoveExtraData(ED_MAX_JOINT_NORMAL);
        if (pkChildNode->GetExtraData(ED_MAX_JOINT_LOCKS))
            pkChildNode->RemoveExtraData(ED_MAX_JOINT_LOCKS);
        if (pkChildNode->GetExtraData(ED_MAX_JOINT_LIMITS))
            pkChildNode->RemoveExtraData(ED_MAX_JOINT_LIMITS);
    }
    
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXExtractorPlugin::ExtractMaxJoint(NiPhysXPropDesc* pkPropDesc,
    float fInvScale, NiAVObject* pkChildNode, unsigned int uiChildIndex,
    NiAVObject* pkParentNode, unsigned int uiParentIndex,
    NxD6JointDesc& kJointDesc)
{
    NiExtraData* pkED;
    
    //
    // Coordinate systems. Two styles: Max and Maya.
    //
    NiPoint3 kChildAxis;
    NiPoint3 kChildNormal;
    NiPoint3 kChildAnchor;
    NiPoint3 kParentAxis;
    NiPoint3 kParentNormal;
    NiPoint3 kParentAnchor;

    // Max style
    unsigned int uiJunkNum;
    float* afPosn;
    pkED = pkChildNode->GetExtraData(ED_MAX_JOINT_POSN);
    if (!NiIsKindOf(NiFloatsExtraData, pkED))
        return false;
    ((NiFloatsExtraData*)pkED)->GetArray(uiJunkNum, afPosn);
    NIASSERT(uiJunkNum == 3);

    float* afAxis;
    pkED = pkChildNode->GetExtraData(ED_MAX_JOINT_AXIS);
    if (!NiIsKindOf(NiFloatsExtraData, pkED))
        return false;
    ((NiFloatsExtraData*)pkED)->GetArray(uiJunkNum, afAxis);
    NIASSERT(uiJunkNum == 3);

    float* afNormal;
    pkED = pkChildNode->GetExtraData(ED_MAX_JOINT_NORMAL);
    if (!NiIsKindOf(NiFloatsExtraData, pkED))
        return false;
    ((NiFloatsExtraData*)pkED)->GetArray(uiJunkNum, afNormal);
    NIASSERT(uiJunkNum == 3);

    // We need to convert some coordinate systems
    NiTransform kChildXform;
    kChildXform.m_Rotate = pkChildNode->GetWorldRotate();
    kChildXform.m_Translate = pkChildNode->GetWorldTranslate();
    kChildXform.m_fScale = pkChildNode->GetWorldScale();
    
    kChildAnchor = NiPoint3(afPosn[0], afPosn[1], afPosn[2]);
    kParentAxis = NiPoint3(afAxis[0], afAxis[1], afAxis[2]);
    kParentNormal = NiPoint3(afNormal[0], afNormal[1], afNormal[2]);

    // Catch a bug where Max sets the vector to be 0,0,0.
    if (kParentAxis.SqrLength() < 0.01)
    {
        kParentAxis = kParentNormal.Cross(NiPoint3::UNIT_Y);
        if (kParentAxis.SqrLength() < 0.001f)
        {
            kParentAxis = NiPoint3::UNIT_Z;
        }
    }

    if (pkParentNode)
    {
        // Attached to a parent
        NiTransform kParentXform;
        kParentXform.m_Rotate = pkParentNode->GetWorldRotate();
        kParentXform.m_Translate = pkParentNode->GetWorldTranslate();
        kParentXform.m_fScale = pkParentNode->GetWorldScale();
        
        NiTransform kParentInvXform;
        kParentXform.Invert(kParentInvXform);
        NiTransform kChildToParent = kParentInvXform * kChildXform;
        kParentAnchor = kChildToParent * kChildAnchor;

        NiMatrix3 kChildInvRot = kChildXform.m_Rotate.Inverse();
        kChildAxis =
            kChildInvRot * kParentXform.m_Rotate * kParentAxis;
        kChildNormal =
            kChildInvRot * kParentXform.m_Rotate * kParentNormal;
    }
    else
    {
        // Attached to the world system
        kParentAnchor = kChildXform * kChildAnchor;
        NiMatrix3 kChildInvRot = kChildXform.m_Rotate.Inverse();
        kChildAxis = kChildInvRot * kParentAxis;
        kChildNormal = kChildInvRot * kParentNormal;
    }
    
    NiPhysXTypes::NiPoint3ToNxVec3(kParentAnchor,
        kJointDesc.localAnchor[0]);
    kJointDesc.localAnchor[0] *= fInvScale;
    NiPhysXTypes::NiPoint3ToNxVec3(kParentAxis, kJointDesc.localAxis[0]);
    NiPhysXTypes::NiPoint3ToNxVec3(kParentNormal,
        kJointDesc.localNormal[0]);
    NiPhysXTypes::NiPoint3ToNxVec3(
        kChildAnchor, kJointDesc.localAnchor[1]);
    kJointDesc.localAnchor[1] *= fInvScale;
    NiPhysXTypes::NiPoint3ToNxVec3(kChildAxis, kJointDesc.localAxis[1]);
    NiPhysXTypes::NiPoint3ToNxVec3(
        kChildNormal, kJointDesc.localNormal[1]);

    //
    // Limits
    //
    pkED = pkChildNode->GetExtraData(ED_MAX_JOINT_LOCKS);
    if (!NiIsKindOf(NiIntegersExtraData, pkED))
        return false;
    NiIntegersExtraData* pkIntsED = (NiIntegersExtraData*)pkED;
    unsigned int uiIndex = uiChildIndex * 6;
    kJointDesc.xMotion =
        (NxD6JointMotion)pkIntsED->GetValue(uiIndex++);
    kJointDesc.yMotion =
        (NxD6JointMotion)pkIntsED->GetValue(uiIndex++);
    kJointDesc.zMotion =
        (NxD6JointMotion)pkIntsED->GetValue(uiIndex++);
    kJointDesc.twistMotion =
        (NxD6JointMotion)pkIntsED->GetValue(uiIndex++);
    kJointDesc.swing1Motion =
        (NxD6JointMotion)pkIntsED->GetValue(uiIndex++);
    kJointDesc.swing2Motion =
        (NxD6JointMotion)pkIntsED->GetValue(uiIndex);

    pkED = pkChildNode->GetExtraData(ED_MAX_JOINT_LIMITS);
    if (!NiIsKindOf(NiFloatsExtraData, pkED))
        return false;
    NiFloatsExtraData* pkFloatsED = (NiFloatsExtraData*)pkED;
    uiIndex = uiChildIndex * 5;
    kJointDesc.linearLimit.value =
        pkFloatsED->GetValue(uiIndex++) * fInvScale;
    kJointDesc.twistLimit.low.value = pkFloatsED->GetValue(uiIndex++);
    kJointDesc.twistLimit.high.value = pkFloatsED->GetValue(uiIndex++);
    kJointDesc.swing1Limit.value = pkFloatsED->GetValue(uiIndex++);
    kJointDesc.swing2Limit.value = pkFloatsED->GetValue(uiIndex);
    
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXExtractorPlugin::ExtractD6Joint(NiPhysXPropDesc* pkPropDesc,
    float fInvScale, NiAVObject* pkChildNode, unsigned int uiChildIndex,
    NiTransform& kParentXform, NxD6JointDesc& kJointDesc)
{        
    NiExtraData* pkED;
    
    //
    // Coordinate systems.
    //
    NiPoint3 kChildAxis;
    NiPoint3 kChildNormal;
    NiPoint3 kChildAnchor;
    NiPoint3 kParentAxis;
    NiPoint3 kParentNormal;
    NiPoint3 kParentAnchor;
    NiPoint3 kVec;
    unsigned int uiIndex = uiChildIndex * 3;
    pkED = pkChildNode->GetExtraData(ED_JOINT_POSN1);
    if (!NiIsKindOf(NiFloatsExtraData, pkED))
        return false;
    NiFloatsExtraData* pkFloatsED = (NiFloatsExtraData*)pkED;
    kVec.x = pkFloatsED->GetValue(uiIndex);
    kVec.y = pkFloatsED->GetValue(uiIndex+1);
    kVec.z = pkFloatsED->GetValue(uiIndex+2);
    kParentAnchor = kParentXform * kVec;
    
    pkED = pkChildNode->GetExtraData(ED_JOINT_AXIS1);
    if (!NiIsKindOf(NiFloatsExtraData, pkED))
        return false;
    pkFloatsED = (NiFloatsExtraData*)pkED;
    kVec.x = pkFloatsED->GetValue(uiIndex);
    kVec.y = pkFloatsED->GetValue(uiIndex+1);
    kVec.z = pkFloatsED->GetValue(uiIndex+2);
    kParentAxis = kParentXform.m_Rotate * kVec;
    
    pkED = pkChildNode->GetExtraData(ED_JOINT_NORMAL1);
    if (!NiIsKindOf(NiFloatsExtraData, pkED))
        return false;
    pkFloatsED = (NiFloatsExtraData*)pkED;
    kVec.x = pkFloatsED->GetValue(uiIndex);
    kVec.y = pkFloatsED->GetValue(uiIndex+1);
    kVec.z = pkFloatsED->GetValue(uiIndex+2);
    kParentNormal = kParentXform.m_Rotate * kVec;
    
    pkED = pkChildNode->GetExtraData(ED_JOINT_POSN2);
    if (!NiIsKindOf(NiFloatsExtraData, pkED))
        return false;
    pkFloatsED = (NiFloatsExtraData*)pkED;
    kChildAnchor.x = pkFloatsED->GetValue(uiIndex);
    kChildAnchor.y = pkFloatsED->GetValue(uiIndex+1);
    kChildAnchor.z = pkFloatsED->GetValue(uiIndex+2);
    
    pkED = pkChildNode->GetExtraData(ED_JOINT_AXIS2);
    if (!NiIsKindOf(NiFloatsExtraData, pkED))
        return false;
    pkFloatsED = (NiFloatsExtraData*)pkED;
    kChildAxis.x = pkFloatsED->GetValue(uiIndex);
    kChildAxis.y = pkFloatsED->GetValue(uiIndex+1);
    kChildAxis.z = pkFloatsED->GetValue(uiIndex+2);
    
    pkED = pkChildNode->GetExtraData(ED_JOINT_NORMAL2);
    if (!NiIsKindOf(NiFloatsExtraData, pkED))
        return false;
    pkFloatsED = (NiFloatsExtraData*)pkED;
    kChildNormal.x = pkFloatsED->GetValue(uiIndex);
    kChildNormal.y = pkFloatsED->GetValue(uiIndex+1);
    kChildNormal.z = pkFloatsED->GetValue(uiIndex+2);

    NiPhysXTypes::NiPoint3ToNxVec3(kParentAnchor,
        kJointDesc.localAnchor[0]);
    kJointDesc.localAnchor[0] *= fInvScale;
    NiPhysXTypes::NiPoint3ToNxVec3(kParentAxis, kJointDesc.localAxis[0]);
    NiPhysXTypes::NiPoint3ToNxVec3(kParentNormal,
        kJointDesc.localNormal[0]);
    NiPhysXTypes::NiPoint3ToNxVec3(
        kChildAnchor, kJointDesc.localAnchor[1]);
    kJointDesc.localAnchor[1] *= fInvScale;
    NiPhysXTypes::NiPoint3ToNxVec3(kChildAxis, kJointDesc.localAxis[1]);
    NiPhysXTypes::NiPoint3ToNxVec3(
        kChildNormal, kJointDesc.localNormal[1]);

    //
    // Limits
    //
    pkED = pkChildNode->GetExtraData(ED_JOINT_LOCKS);
    if (!NiIsKindOf(NiIntegersExtraData, pkED))
        return false;
    NiIntegersExtraData* pkIntsED = (NiIntegersExtraData*)pkED;
    uiIndex = uiChildIndex * 6;
    kJointDesc.xMotion =
        (NxD6JointMotion)pkIntsED->GetValue(uiIndex++);
    kJointDesc.yMotion =
        (NxD6JointMotion)pkIntsED->GetValue(uiIndex++);
    kJointDesc.zMotion =
        (NxD6JointMotion)pkIntsED->GetValue(uiIndex++);
    kJointDesc.twistMotion =
        (NxD6JointMotion)pkIntsED->GetValue(uiIndex++);
    kJointDesc.swing1Motion =
        (NxD6JointMotion)pkIntsED->GetValue(uiIndex++);
    kJointDesc.swing2Motion =
        (NxD6JointMotion)pkIntsED->GetValue(uiIndex);

    pkED = pkChildNode->GetExtraData(ED_JOINT_LIMITS);
    if (!NiIsKindOf(NiFloatsExtraData, pkED))
        return false;
    pkFloatsED = (NiFloatsExtraData*)pkED;
    uiIndex = uiChildIndex * 5;
    kJointDesc.linearLimit.value =
        pkFloatsED->GetValue(uiIndex++) * fInvScale;
    kJointDesc.twistLimit.low.value = pkFloatsED->GetValue(uiIndex++);
    kJointDesc.twistLimit.high.value = pkFloatsED->GetValue(uiIndex++);
    kJointDesc.swing1Limit.value = pkFloatsED->GetValue(uiIndex++);
    kJointDesc.swing2Limit.value = pkFloatsED->GetValue(uiIndex);
    
    pkED = pkChildNode->GetExtraData(ED_JOINT_RESTITUTIONS);
    if (pkED)
    {
        if (!NiIsKindOf(NiFloatsExtraData, pkED))
            return false;
        pkFloatsED = (NiFloatsExtraData*)pkED;
        uiIndex = uiChildIndex * 5;
        kJointDesc.linearLimit.restitution =
            pkFloatsED->GetValue(uiIndex++);
        kJointDesc.twistLimit.low.restitution =
            pkFloatsED->GetValue(uiIndex++);
        kJointDesc.twistLimit.high.restitution =
            pkFloatsED->GetValue(uiIndex++);
        kJointDesc.swing1Limit.restitution =
            pkFloatsED->GetValue(uiIndex++);
        kJointDesc.swing2Limit.restitution =
            pkFloatsED->GetValue(uiIndex);
    }
    
    pkED = pkChildNode->GetExtraData(ED_JOINT_SPRINGS);
    if (pkED)
    {
        if (!NiIsKindOf(NiFloatsExtraData, pkED))
           return false;
        pkFloatsED = (NiFloatsExtraData*)pkED;
        uiIndex = uiChildIndex * 5;
        kJointDesc.linearLimit.spring =
            pkFloatsED->GetValue(uiIndex++) / fInvScale;
        kJointDesc.twistLimit.low.spring =
            pkFloatsED->GetValue(uiIndex++);
        kJointDesc.twistLimit.high.spring =
            pkFloatsED->GetValue(uiIndex++);
        kJointDesc.swing1Limit.spring =
            pkFloatsED->GetValue(uiIndex++);
        kJointDesc.swing2Limit.spring =
            pkFloatsED->GetValue(uiIndex);
    }
    
    pkED = pkChildNode->GetExtraData(ED_JOINT_DAMPINGS);
    if (pkED)
    {
        if (!NiIsKindOf(NiFloatsExtraData, pkED))
            return false;
        pkFloatsED = (NiFloatsExtraData*)pkED;
        uiIndex = uiChildIndex * 5;
        kJointDesc.linearLimit.damping =
            pkFloatsED->GetValue(uiIndex++);
        kJointDesc.twistLimit.low.damping =
            pkFloatsED->GetValue(uiIndex++);
        kJointDesc.twistLimit.high.damping =
            pkFloatsED->GetValue(uiIndex++);
        kJointDesc.swing1Limit.damping =
            pkFloatsED->GetValue(uiIndex++);
        kJointDesc.swing2Limit.damping =
            pkFloatsED->GetValue(uiIndex);
    }
    
    //
    // Misc other stuff
    //
    pkED = pkChildNode->GetExtraData(ED_JOINT_MAX_FORCE);
    if (pkED)
    {
        if (!NiIsKindOf(NiFloatsExtraData, pkED))
            return false;
        pkFloatsED = (NiFloatsExtraData*)pkED;
        kJointDesc.maxForce = pkFloatsED->GetValue(uiChildIndex);
    }
    
    pkED = pkChildNode->GetExtraData(ED_JOINT_MAX_TORQUE);
    if (pkED)
    {
        if (!NiIsKindOf(NiFloatsExtraData, pkED))
            return false;
        pkFloatsED = (NiFloatsExtraData*)pkED;
        kJointDesc.maxTorque = pkFloatsED->GetValue(uiChildIndex);
    }

    pkED = pkChildNode->GetExtraData(ED_JOINT_NAME);
    if (pkED)
    {
        if (!NiIsKindOf(NiStringsExtraData, pkED))
            return false;
        NiStringsExtraData* pkStringsED = (NiStringsExtraData*)pkED;
        kJointDesc.name = pkStringsED->GetValue(uiChildIndex);
    }

    pkED = pkChildNode->GetExtraData(ED_JOINT_COLLIDES);
    if (pkED)
    {
        if (!NiIsKindOf(NiIntegersExtraData, pkED))
            return false;
        pkIntsED = (NiIntegersExtraData*)pkED;
        if (pkIntsED->GetValue(uiChildIndex))
            kJointDesc.jointFlags |= NX_JF_COLLISION_ENABLED;
    }

    pkED = pkChildNode->GetExtraData(ED_JOINT_PROJ_MODE);
    if (pkED)
    {
        if (!NiIsKindOf(NiIntegersExtraData, pkED))
            return false;
        pkIntsED = (NiIntegersExtraData*)pkED;
        kJointDesc.projectionMode =
            (NxJointProjectionMode)pkIntsED->GetValue(uiChildIndex);
    }

    pkED = pkChildNode->GetExtraData(ED_JOINT_PROJ_DIST);
    if (pkED)
    {
        if (!NiIsKindOf(NiFloatsExtraData, pkED))
            return false;
        pkFloatsED = (NiFloatsExtraData*)pkED;
        kJointDesc.projectionDistance = pkFloatsED->GetValue(uiChildIndex);
    }

    pkED = pkChildNode->GetExtraData(ED_JOINT_PROJ_ANGLE);
    if (pkED)
    {
        if (!NiIsKindOf(NiFloatsExtraData, pkED))
            return false;
        pkFloatsED = (NiFloatsExtraData*)pkED;
        kJointDesc.projectionAngle = pkFloatsED->GetValue(uiChildIndex);
    }

    pkED = pkChildNode->GetExtraData(ED_JOINT_GEARING);
    if (pkED)
    {
        if (!NiIsKindOf(NiIntegersExtraData, pkED))
            return false;
        pkIntsED = (NiIntegersExtraData*)pkED;
        if (pkIntsED->GetValue(uiChildIndex))
            kJointDesc.flags |= NX_D6JOINT_GEAR_ENABLED;
    }

    pkED = pkChildNode->GetExtraData(ED_JOINT_GEAR_RATIO);
    if (pkED)
    {
        if (!NiIsKindOf(NiFloatsExtraData, pkED))
            return false;
        pkFloatsED = (NiFloatsExtraData*)pkED;
        kJointDesc.gearRatio = pkFloatsED->GetValue(uiChildIndex);
    }

    // Enforce some limits, taken from NxD6JoinDesc::isValid()
	// swing limits are symmetric, thus their range is 0..180 degrees
	if (kJointDesc.swing1Motion == NX_D6JOINT_MOTION_LIMITED)
	{
		if (kJointDesc.swing1Limit.value < 0.0f)
		    kJointDesc.swing1Limit.value = 0.0f;
		if (kJointDesc.swing1Limit.value > NI_PI)
		    kJointDesc.swing1Limit.value = NI_PI - 1.0e-06f;
	}
	if (kJointDesc.swing2Motion == NX_D6JOINT_MOTION_LIMITED)
	{
		if (kJointDesc.swing2Limit.value < 0.0f)
		    kJointDesc.swing2Limit.value = 0.0f;
		if (kJointDesc.swing2Limit.value > NI_PI)
		    kJointDesc.swing2Limit.value = NI_PI - 1.0e-06f;
	}

	// twist limits are asymmetric with -180 <= low < high <= 180 degrees
	if (kJointDesc.twistMotion == NX_D6JOINT_MOTION_LIMITED)
	{
		if (kJointDesc.twistLimit.low.value < -NI_PI)
		    kJointDesc.twistLimit.low.value = -NI_PI + 1.0e-06f;
		if (kJointDesc.twistLimit.high.value > NI_PI)
		    kJointDesc.twistLimit.high.value = NI_PI - 1.0e-06f;
		if (kJointDesc.twistLimit.low.value > kJointDesc.twistLimit.high.value)
		{
		    float fTemp = kJointDesc.twistLimit.low.value;
		    kJointDesc.twistLimit.low.value = kJointDesc.twistLimit.high.value;
		    kJointDesc.twistLimit.high.value = fTemp;
		}
	}

	// in angular limited-free mode, only -90..90 swings are possible
	if (kJointDesc.swing1Motion == NX_D6JOINT_MOTION_LIMITED
	    && kJointDesc.swing2Motion == NX_D6JOINT_MOTION_FREE)
	{
		if (kJointDesc.swing1Limit.value > NI_PI * 0.5f)
		    kJointDesc.swing1Limit.value = NI_PI * 0.499999f;
	}
	if (kJointDesc.swing2Motion == NX_D6JOINT_MOTION_LIMITED
	    && kJointDesc.swing1Motion == NX_D6JOINT_MOTION_FREE) 
	{
		if (kJointDesc.swing2Limit.value > NI_PI * 0.5f)
		    kJointDesc.swing2Limit.value = NI_PI * 0.499999f;
	}

    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXExtractorPlugin::FindAndConvertPSys(NiPhysXProp* pkRBProp,
    NiPhysXPSysProp*& pkPSysProp, NiPhysXMeshPSysProp*& pkMeshProp,
    NiAVObject* pkAVObj, bool bToFluid,
    NiTPointerMap<NiAVObject*, NiAVObject*>& kMap,
    bool bPhysXSpace, bool bPhysXRotates)
{
    if (!NiIsKindOf(NiNode, pkAVObj))
        return true;
        
    NiNode* pkNode = (NiNode*)pkAVObj;
    
    for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkObj = pkNode->GetAt(ui);
        if (!pkObj)
            continue;
        
        if (NiIsKindOf(NiParticleSystem, pkObj))
        {
            NiParticleSystemPtr spNewPSys;
            if (NiIsExactKindOf(NiMeshParticleSystem, pkObj))
            {
                if (bToFluid)
                {
                    spNewPSys = NiNew NiPhysXMeshFluidSystem(
                        (NiMeshParticleSystem*)pkObj, pkRBProp, bPhysXSpace,
                        m_kActorMap);
                }
                else
                {
                    if (!pkMeshProp)
                    {
                        pkMeshProp = NiNew NiPhysXMeshPSysProp();
    
                        NiString kName = pkRBProp->GetName();
                        kName.Concatenate(" Mesh Particles");
                        pkMeshProp->SetName((const char*)kName);
                        pkMeshProp->SetKeepMeshes(pkRBProp->GetKeepMeshes());
                        pkMeshProp->SetScaleFactor(pkRBProp->GetScaleFactor());

                        NiPhysXPropDesc* pkPropDesc = NiNew NiPhysXPropDesc();
                        pkMeshProp->SetSnapshot(pkPropDesc);
                        
                        pkPropDesc->SetNumStates(1);
                    }
                    spNewPSys = ConvertPSysToMeshPSys(pkMeshProp,
                        (NiMeshParticleSystem*)pkObj, bPhysXSpace,
                        bPhysXRotates);
                }
            }
            else
            {
                if (bToFluid)
                {
                    spNewPSys = NiNew NiPhysXFluidSystem(
                        (NiParticleSystem*)pkObj, pkRBProp, bPhysXSpace,
                        m_kActorMap);
                }
                else
                {
                    if (!pkPSysProp)
                    {
                        pkPSysProp = NiNew NiPhysXPSysProp();
    
                        NiString kName = pkRBProp->GetName();
                        kName.Concatenate(" Particles");
                        pkPSysProp->SetName((const char*)kName);
                        pkPSysProp->SetKeepMeshes(pkRBProp->GetKeepMeshes());
                        pkPSysProp->SetScaleFactor(pkRBProp->GetScaleFactor());

                        NiPhysXPropDesc* pkPropDesc = NiNew NiPhysXPropDesc();
                        pkPSysProp->SetSnapshot(pkPropDesc);
                        
                        pkPropDesc->SetNumStates(1);
                    }
                    spNewPSys = ConvertPSysToPSys(
                        pkPSysProp, (NiParticleSystem*)pkObj, bPhysXSpace);
                }
            }
            if (!spNewPSys)
                return false;
            kMap.SetAt(pkObj, spNewPSys);
            pkNode->DetachChildAt(ui);
            pkNode->AttachChild(spNewPSys, true);
        }
        else if (NiIsKindOf(NiNode, pkObj))
        {
            bool bSuccess = FindAndConvertPSys(pkRBProp, pkPSysProp,
                pkMeshProp, pkObj, bToFluid, kMap, bPhysXSpace, bPhysXRotates);
            if (!bSuccess)
                return false;
        }
    }
    
    return true;
}
//---------------------------------------------------------------------------
NiPhysXParticleSystemPtr NiPhysXExtractorPlugin::ConvertPSysToPSys(
    NiPhysXPSysProp *pkProp, NiParticleSystem* pkPSys, bool bPhysXSpace)
{
    NiPhysXParticleSystemPtr spResult =
        NiNew NiPhysXParticleSystem(pkPSys, pkProp, 0, bPhysXSpace);
            
    // Find the actor
    const NiFixedString kName = pkPSys->GetName();
    NiPhysXActorDesc* pkActorDesc;
    bool bFoundActor = m_kParticleMap.GetAt(kName, pkActorDesc);
    if (bFoundActor)
    {
        ((NiPhysXPSysData*)spResult->GetModelData())->SetActorDesc(
            pkActorDesc);
    }
        
    return spResult;
}
//---------------------------------------------------------------------------
NiPhysXMeshParticleSystemPtr NiPhysXExtractorPlugin::ConvertPSysToMeshPSys(
    NiPhysXMeshPSysProp *pkProp, NiMeshParticleSystem* pkPSys,
    bool bPhysXSpace, bool bPhysXRotates)
{
    // Should extract some parameters here, such as pool filling on
    // loading.
    NiPhysXMeshParticleSystemPtr spResult = NiNew NiPhysXMeshParticleSystem(
        pkPSys, pkProp, 0, bPhysXSpace, bPhysXRotates);
    
    NiPhysXMeshPSysData* pkData =
        (NiPhysXMeshPSysData*)spResult->GetModelData();

    // Find actors for particles. They should be in the particle actor map.
    // Find the actor
    const char* pcBaseName = pkPSys->GetName();
    for (unsigned int ui = 0; ui < pkData->GetNumGenerations(); ui++)
    {
        NiPhysXActorDesc* pkActorDesc;
        unsigned int uiStrlen = strlen(pcBaseName) + 10;
        char* pcActorName = NiAlloc(char, uiStrlen);
        NiSprintf(pcActorName, uiStrlen, "%s:%u", pcBaseName, ui);
        NiFixedString kFixedString(pcActorName);
        
        bool bFoundActor = m_kParticleMap.GetAt(kFixedString, pkActorDesc);
        if (bFoundActor)
        {
            pkData->SetActorDescAt(ui, pkActorDesc);
        }
        
        NiFree(pcActorName);
    }

    return spResult;
}
//---------------------------------------------------------------------------
void NiPhysXExtractorPlugin::EnactNodeSwitch(NiAVObject* pkObject,
    NiTPointerMap<NiAVObject*, NiAVObject*>& kMap)
{
    // Finds places that reference into a deleted particle system and
    // switches it to the new one. Should be called after the old particle
    // system has been deleted, so that elements of that system are not
    // switched. It looks for the following:
    // - LookAt targets of NiLookAtInterpolators
    // - objects referenced by particle systems

    // LookAt interpolator target.
    NiTransformController* pkTransformCtlr = NiGetController(
        NiTransformController, pkObject);
    if (pkTransformCtlr)
    {
        NiInterpolator* pkInterp = pkTransformCtlr->GetInterpolator(0);
        if (pkInterp && NiIsKindOf(NiLookAtInterpolator, pkInterp))
        {
            NiLookAtInterpolator* pkLookAtInterp = (NiLookAtInterpolator*)
                pkInterp;
            NiAVObject* pkOld = pkLookAtInterp->GetLookAt();
            if (pkOld)
            {
                NiAVObject* pkNew;
                if (kMap.GetAt(pkOld, pkNew))
                    pkLookAtInterp->SetLookAt(pkNew);
            }
        }
    }
    
    // Objects referenced by particle systems.
    if (NiIsKindOf(NiParticleSystem, pkObject))
    {
        NiParticleSystem* pkPSystem = (NiParticleSystem*) pkObject;
        for (unsigned int ui = 0; ui < pkPSystem->GetModifierCount(); ui++)
        {
            NiPSysModifier* pkModifier = pkPSystem->GetModifierAt(ui);
            if (pkModifier)
            {
                if (NiIsKindOf(NiPSysBombModifier, pkModifier))
                {
                    NiPSysBombModifier* pkMod = 
                        (NiPSysBombModifier*)pkModifier;
                    NiAVObject* pkOld = pkMod->GetBombObj();
                    if (pkOld)
                    {
                        NiAVObject* pkNew;
                        if (kMap.GetAt(pkOld, pkNew))
                            pkMod->SetBombObj(pkNew);
                    }
                }
                else if (NiIsKindOf(NiPSysColliderManager, pkModifier))
                {
                    NiPSysCollider* pkCollider = ((NiPSysColliderManager*)
                        pkModifier)->GetColliders();
                    while (pkCollider)
                    {
                        if (NiIsKindOf(NiPSysPlanarCollider, pkCollider))
                        {
                            NiPSysPlanarCollider* pkPlCol =
                                (NiPSysPlanarCollider*)pkCollider;
                            NiAVObject* pkOld = pkPlCol->GetColliderObj();
                            if (pkOld)
                            {
                                NiAVObject* pkNew;
                                if (kMap.GetAt(pkOld, pkNew))
                                    pkPlCol->SetColliderObj(pkNew);
                            }
                        }
                        else if (NiIsKindOf(NiPSysSphericalCollider,
                            pkCollider))
                        {
                            NiPSysSphericalCollider* pkSpCol =
                                (NiPSysSphericalCollider*)pkCollider;
                            NiAVObject* pkOld = pkSpCol->GetColliderObj();
                            if (pkOld)
                            {
                                NiAVObject* pkNew;
                                if (kMap.GetAt(pkOld, pkNew))
                                    pkSpCol->SetColliderObj(pkNew);
                            }
                        }
                        pkCollider = pkCollider->GetNext();
                    }
                }
                else if (NiIsKindOf(NiPSysGravityModifier, pkModifier))
                {
                    NiPSysGravityModifier* pkMod =
                        (NiPSysGravityModifier*)pkModifier;
                    NiAVObject* pkOld = pkMod->GetGravityObj();
                    if (pkOld)
                    {
                        NiAVObject* pkNew;
                        if (kMap.GetAt(pkOld, pkNew))
                            pkMod->SetGravityObj(pkNew);
                    }
                }
                else if (NiIsKindOf(NiPSysMeshEmitter, pkModifier))
                {
                    NiPSysMeshEmitter* pkMeshEmitter = (NiPSysMeshEmitter*)
                        pkModifier;
                    // Work backwards so that removal works
                    for (unsigned int uj =
                        pkMeshEmitter->GetGeometryEmitterCount();
                        uj > 0; uj--)
                    {
                        NiAVObject* pkOld = pkMeshEmitter
                            ->GetGeometryEmitter(uj - 1);
                        if (pkOld)
                        {
                            NiAVObject* pkNew;
                            if (kMap.GetAt(pkOld, pkNew))
                            {
                                pkMeshEmitter->AddGeometryEmitter(
                                    (NiGeometry*)pkNew);
                                pkMeshEmitter->RemoveGeometryEmitter(uj-1);
                            }
                        }
                    }
                }
                else if (NiIsKindOf(NiPSysVolumeEmitter, pkModifier))
                {
                    NiPSysVolumeEmitter* pkMod =
                        (NiPSysVolumeEmitter*)pkModifier;
                    NiAVObject* pkOld = pkMod->GetEmitterObj();
                    if (pkOld)
                    {
                        NiAVObject* pkNew;
                        if (kMap.GetAt(pkOld, pkNew))
                            pkMod->SetEmitterObj(pkNew);
                    }
                }
            }
        }
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        // Recurse over children.
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
                EnactNodeSwitch(pkChild, kMap);
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXExtractorPlugin::CreateMaps(NiAVObject* pkAVObj)
{
    NiExtraData* pkExtraData;
    
    pkExtraData = pkAVObj->GetExtraData(ED_PROXY_SOURCE);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiIntegerExtraData, pkExtraData));
        unsigned int uiID = ((NiIntegerExtraData*)pkExtraData)->GetValue();
        NIASSERT(uiID > 0);
        m_kProxyMap.SetAt(uiID, pkAVObj);
        
        // Remove it from the scene, effectively
        pkAVObj->SetAppCulled(true);

        pkAVObj->RemoveExtraData(ED_PROXY_SOURCE);
    }

    pkExtraData = pkAVObj->GetExtraData(ED_SHAPE_COMPOUND_ID);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiIntegerExtraData, pkExtraData));
        unsigned int uiID = ((NiIntegerExtraData*)pkExtraData)->GetValue();
        NIASSERT(uiID > 0);
        NiPhysXExtractorPlugin::NiAVObjectArray* pkObjArray;
        bool bArrayExists = m_kShapeGroupMap.GetAt(uiID, pkObjArray);
        if (bArrayExists)
        {
            pkObjArray->Add(pkAVObj);
        }
        else
        {
            pkObjArray = NiNew NiPhysXExtractorPlugin::NiAVObjectArray;
            pkObjArray->Add(pkAVObj);
            m_kShapeGroupMap.SetAt(uiID, pkObjArray);
        }
        pkAVObj->RemoveExtraData(ED_SHAPE_COMPOUND_ID);
    }

    pkExtraData = pkAVObj->GetExtraData(ED_SHAPE_BASIC_CHILD);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiIntegerExtraData, pkExtraData));
        unsigned int uiID = ((NiIntegerExtraData*)pkExtraData)->GetValue();
        NIASSERT(uiID > 0);
        m_kBasicShapeMap.SetAt(uiID, pkAVObj);

        pkAVObj->RemoveExtraData(ED_SHAPE_BASIC_CHILD);
    }

    pkExtraData = pkAVObj->GetExtraData(ED_SHAPE_MESH_CHILD);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiIntegerExtraData, pkExtraData));
        unsigned int uiID = ((NiIntegerExtraData*)pkExtraData)->GetValue();
        NIASSERT(uiID > 0);
        NiPhysXExtractorPlugin::NiAVObjectArray* pkObjArray;
        bool bArrayExists = m_kMeshGroupMap.GetAt(uiID, pkObjArray);
        if (bArrayExists)
        {
            pkObjArray->Add(pkAVObj);
        }
        else
        {
            pkObjArray = NiNew NiPhysXExtractorPlugin::NiAVObjectArray;
            pkObjArray->Add(pkAVObj);
            m_kMeshGroupMap.SetAt(uiID, pkObjArray);
        }
        pkAVObj->RemoveExtraData(ED_SHAPE_MESH_CHILD);
    }
    
    pkExtraData = pkAVObj->GetExtraData(ED_JOINT_CHILD);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiIntegersExtraData, pkExtraData));
        unsigned int uiCount;
        int* aiVals;
        ((NiIntegersExtraData*)pkExtraData)->GetArray(uiCount, aiVals);
        for (unsigned int ui = 0; ui < uiCount; ui++)
        {
            JointData* pkJointData;
            if (m_kJointsMap.GetAt(aiVals[ui], pkJointData))
            {
                pkJointData->pkChildNode = pkAVObj;
                pkJointData->uiChildIndex = ui;
            }
            else
            {
                pkJointData = NiAlloc(JointData, 1);
                pkJointData->pkChildNode = pkAVObj;
                pkJointData->uiChildIndex = ui;
                pkJointData->pkParentNode = 0;
                pkJointData->uiParentIndex = 0;
                m_kJointsMap.SetAt(aiVals[ui], pkJointData);
            }
        }
        pkAVObj->RemoveExtraData(ED_JOINT_CHILD);
    }
    pkExtraData = pkAVObj->GetExtraData(ED_JOINT_PARENT);
    if (pkExtraData)
    {
        NIASSERT(NiIsKindOf(NiIntegersExtraData, pkExtraData));
        unsigned int uiCount;
        int* aiVals;
        ((NiIntegersExtraData*)pkExtraData)->GetArray(uiCount, aiVals);
        for (unsigned int ui = 0; ui < uiCount; ui++)
        {
            JointData* pkJointData;
            if (m_kJointsMap.GetAt(aiVals[ui], pkJointData))
            {
                pkJointData->pkParentNode = pkAVObj;
                pkJointData->uiParentIndex = ui;
            }
            else
            {
                pkJointData = NiAlloc(JointData, 1);
                pkJointData->pkParentNode = pkAVObj;
                pkJointData->uiParentIndex = ui;
                m_kJointsMap.SetAt(aiVals[ui], pkJointData);
            }
        }
        pkAVObj->RemoveExtraData(ED_JOINT_PARENT);
    }
    
    if (NiIsKindOf(NiNode, pkAVObj))
    {
        NiNode* pkNode = (NiNode*)pkAVObj;
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
                CreateMaps(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXExtractorPlugin::Reset()
{
    NiTMapIterator iter = m_kShapeGroupMap.GetFirstPos();
    while (iter)
    {
        unsigned int uiKey;
        NiPhysXExtractorPlugin::NiAVObjectArray* pkVal;
        
        m_kShapeGroupMap.GetNext(iter, uiKey, pkVal);
        NiDelete pkVal;
    }
    
    iter = m_kMeshGroupMap.GetFirstPos();
    while (iter)
    {
        unsigned int uiKey;
        NiPhysXExtractorPlugin::NiAVObjectArray* pkVal;
        
        m_kMeshGroupMap.GetNext(iter, uiKey, pkVal);
        NiDelete pkVal;
    }
    
    iter = m_kAttachmentMap.GetFirstPos();
    while (iter)
    {
        unsigned int uiKey;
        NiPhysXClothDesc::ClothAttachment* pkVal;
        
        m_kAttachmentMap.GetNext(iter, uiKey, pkVal);
        NiDelete pkVal;
    }
    
    iter = m_kJointsMap.GetFirstPos();
    while (iter)
    {
        unsigned int uiKey;
        JointData* pkVal;
        
        m_kJointsMap.GetNext(iter, uiKey, pkVal);
        NiFree(pkVal);
    }

    m_kActorMap.RemoveAll();
    m_kClothMap.RemoveAll();
    m_kProxyMap.RemoveAll();
    m_kShapeGroupMap.RemoveAll();
    m_kBasicShapeMap.RemoveAll();
    m_kMeshGroupMap.RemoveAll();
    m_kParticleMap.RemoveAll();
    m_kMeshesMap.RemoveAll();
    m_kClothMeshesMap.RemoveAll();
    m_kAttachmentMap.RemoveAll();
    m_kJointsMap.RemoveAll();
}
//---------------------------------------------------------------------------
void NiPhysXExtractorPlugin::StashMeshVersion(unsigned char* pucData)
{
    unsigned int uiPhysXVersion = NX_PHYSICS_SDK_VERSION;
    unsigned char ucMismatch = pucData[3] & 1;
    
    char* pcVersionAsChar = (char*)&uiPhysXVersion;
    if (NiSystemDesc::GetSystemDesc().IsLittleEndian() && (ucMismatch == 0))
    {
        // Target endian not same as this platform. Flip
        NiEndian::Swap32(pcVersionAsChar, 1);
    }
    pucData[8] = ((unsigned char*)pcVersionAsChar)[0];
    pucData[9] = ((unsigned char*)pcVersionAsChar)[1];
    pucData[10] = ((unsigned char*)pcVersionAsChar)[2];
    pucData[11] = ((unsigned char*)pcVersionAsChar)[3];
}
//---------------------------------------------------------------------------
unsigned int NiPhysXExtractorPlugin::NextPrime(const unsigned int target)
{
    int i;
    for (i = 0; i < NUM_PRIMES && PRIMES[i] < target; i++);
    return PRIMES[i];
}
//---------------------------------------------------------------------------
