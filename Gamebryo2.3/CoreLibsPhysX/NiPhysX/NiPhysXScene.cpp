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


NiImplementRTTI(NiPhysXScene, NiObjectNET);    
        
//---------------------------------------------------------------------------
NiPhysXScene::NiPhysXScene() : m_pkPhysXScene(0), m_kCompartmentMap(7)
{
    m_pkManager = NiPhysXManager::GetPhysXManager();
    
    m_kSceneXform.MakeIdentity();
    m_fScalePToW = 1.0f;
    
    m_uFlags = 0x0;
    SetUpdateSrc(false);
    SetUpdateDest(false);

    SetDoFixedStep(true);
    m_fTimestep = 1.0f / 60.0f;
    m_uiNumSubSteps = 1;
    m_uiMaxSubSteps = 8;
    
    m_bInSimFetch = false;
    m_fPrevFetchTime = 0.0f;
    m_fNextFetchTime = 0.0f;
    
    SetDebugRender(false, 0);
    m_spDebugRoot = 0;

    m_spSnapshot = 0;
}
//---------------------------------------------------------------------------
NiPhysXScene::~NiPhysXScene()
{
    // Remove any props
    unsigned int uiSize = m_kProps.GetSize();
    for (unsigned int ui = uiSize; ui > 0; ui--)
        RemovePropAt(ui - 1);

    if (m_pkPhysXScene)
        ReleaseScene();

    // The compartment pointers will all be bad now.
    m_kCompartmentMap.RemoveAll();
    
    // Delete all the sources and destinations
    m_kSources.RemoveAll();
    m_kDestinations.RemoveAll();
        
    // Release the smart pointer to the scene root and the debug rendering
    m_spDebugRoot = 0;
    
    // Delete the snapshot
    m_spSnapshot = 0;
}
//---------------------------------------------------------------------------
void NiPhysXScene::SetPhysXScene(NxScene* pkScene)
{
    NIASSERT(!m_pkPhysXScene);

    m_pkPhysXScene = pkScene;
}
//---------------------------------------------------------------------------
void NiPhysXScene::ReleaseScene()
{
    FetchResults(NI_INFINITY, true);
    
    // Catch the case when the SDK has already been shut down.
    if (m_pkManager->m_pkPhysXSDK)
    {
        m_pkManager->WaitSDKLock();

        // Release all content in Props to avoid errors with hanging pointers.
        unsigned int uiNumProps = m_kProps.GetSize();
        for (unsigned int ui = 0; ui < uiNumProps; ui++)
        {
            NiPhysXPropDesc* pkSnapshot = m_kProps.GetAt(ui)->GetSnapshot();
            if (pkSnapshot)
                pkSnapshot->RemoveFromScene(m_pkPhysXScene);
        }
        
        // Release the scene.
        m_pkManager->m_pkPhysXSDK->releaseScene(*m_pkPhysXScene);

        m_pkManager->ReleaseSDKLock();
    }

    m_pkPhysXScene = 0;
    
    m_fPrevFetchTime = 0.0f;
    m_fNextFetchTime = 0.0f;
    
    m_kCompartmentMap.RemoveAll();
}
//---------------------------------------------------------------------------
unsigned int NiPhysXScene::AddProp(NiPhysXProp* pkProp, unsigned int uiState)
{
    // Add to array
    unsigned int uiIndex = m_kProps.AddFirstEmpty(pkProp);
    
    // Add sources and destinations
    unsigned int uiNumDests = pkProp->GetDestinationsCount();
    for (unsigned int ui = 0; ui < uiNumDests; ui++)
    {
        AddDestination(pkProp->GetDestinationAt(ui));
    }
    
    unsigned int uiNumSrcs = pkProp->GetSourcesCount();
    for (unsigned int ui = 0; ui < uiNumSrcs; ui++)
    {
        AddSource(pkProp->GetSourceAt(ui));
    }

    // Create if a scene exists
    if (m_pkPhysXScene)
    {
        NiPhysXPropDesc* pkSnapshot = pkProp->GetSnapshot();
        if (pkSnapshot)
        {
            NxCompartment* pkRBCompartment = 0;
            unsigned int uiID = pkSnapshot->GetRBCompartmentID();
            if (uiID > 0)
            {
                m_kCompartmentMap.GetAt(uiID, pkRBCompartment);
                NIASSERT(pkRBCompartment);
            }
            NxCompartment* pkClothCompartment = 0;
            uiID = pkSnapshot->GetClothCompartmentID();
            if (uiID > 0)
            {
                m_kCompartmentMap.GetAt(uiID, pkClothCompartment);
                NIASSERT(pkClothCompartment);
            }
            NxCompartment* pkFluidCompartment = 0;
            uiID = pkSnapshot->GetFluidCompartmentID();
            if (uiID > 0)
            {
                m_kCompartmentMap.GetAt(uiID, pkFluidCompartment);
                NIASSERT(pkFluidCompartment);
            }
            m_pkManager->WaitSDKLock();
            bool bResult = pkSnapshot->CreateInScene(m_pkPhysXScene,
                pkProp->GetXform(), pkRBCompartment, pkClothCompartment,
                pkFluidCompartment, pkProp->GetKeepMeshes(), uiState);
            m_pkManager->ReleaseSDKLock();
            if (!bResult)
            {
                RemovePropAt(uiIndex);
                return (unsigned int)-1;
            }
        }
    }
    
    pkProp->AttachSceneCallback(this);

    return uiIndex;
}
//---------------------------------------------------------------------------
void NiPhysXScene::RemoveProp(NiPhysXProp* pkTarget)
{
    unsigned int uiSize = m_kProps.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        if (m_kProps.GetAt(ui) == pkTarget)
        {
            RemovePropAt(ui);
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXScene::RemovePropAt(const unsigned int uiIndex)
{
    NiPhysXProp* pkProp = m_kProps.GetAt(uiIndex);
    NIASSERT(pkProp != 0);
    
    // Remove sources and destinations
    unsigned int uiNumDests = pkProp->GetDestinationsCount();
    for (unsigned int ui = 0; ui < uiNumDests; ui++)
    {
        DeleteDestination(pkProp->GetDestinationAt(ui));
    }
    
    unsigned int uiNumSrcs = pkProp->GetSourcesCount();
    for (unsigned int ui = 0; ui < uiNumSrcs; ui++)
    {
        DeleteSource(pkProp->GetSourceAt(ui));
    }
    
    // Remove from scene, if there is one
    if (m_pkPhysXScene)
    {
        NiPhysXPropDesc* pkSnapshot = pkProp->GetSnapshot();
        if (pkSnapshot)
        {
            m_pkManager->WaitSDKLock();
            pkSnapshot->RemoveFromScene(m_pkPhysXScene);
            m_pkManager->ReleaseSDKLock();
        }
    }

    pkProp->DetachSceneCallback(this);

    m_kProps.RemoveAtAndFill(uiIndex);    
}
//---------------------------------------------------------------------------
bool NiPhysXScene::AddCompartment(unsigned int uiID,
    NxCompartmentDesc& pkDescriptor)
{
    NIASSERT(pkDescriptor.isValid());
    NIASSERT(m_pkPhysXScene);
    
    NxCompartment* pkCompartment =
        m_pkPhysXScene->createCompartment(pkDescriptor);
    NIASSERT(pkCompartment);
    
    return AddCompartment(uiID, pkCompartment);
}
//---------------------------------------------------------------------------
bool NiPhysXScene::Simulate(const float fTargetTime, const bool bForceSim)
{
    // Return value is whether or not we are now in a sim/fetch pair
    
    // First test if we should and need the update
    if (!m_pkPhysXScene || (m_fPrevFetchTime >= fTargetTime && !bForceSim))
        return false;
        
    // If we are simulating already, don't go any further
    if (m_bInSimFetch)
        return true;

    // Figure out our target time step. This code takes some convulated
    // steps to be sure that PhysX steps as far as we want and no further.
    // It is necessary because of the way that time is internally measured
    // by PhysX, but not reported back to the application. It will probably
    // have to change if the internal time management done by PhysX is
    // changed.
    float fPhysXStep = 0.0f;
    if (bForceSim && m_fPrevFetchTime > fTargetTime)
    {
        // This will force a zero time step
        m_fNextFetchTime = m_fPrevFetchTime;
    }
    else if (GetBit(FIXED_TIMESTEP_MASK))
    {
        // We aim to keep the time close to an integer multiple of the step.
        // Compute the requested delta, rounding down a little to ensure that
        // we get the right number of steps if it's an exact multiple of
        // the step length. 
        float fDeltaT = fTargetTime - m_fPrevFetchTime - 1.0e-06f;
        
        // Get the number of steps. Use ceiling because we always want to
        // go past the requested time.
        unsigned int uiNumGBSteps = (unsigned int)ceil(fDeltaT / m_fTimestep);
        
        // m_fNextFetchTime might drift away from a multiple of
        // m_fTimestep, but that's OK with us.
        m_fNextFetchTime = m_fPrevFetchTime + uiNumGBSteps * m_fTimestep;
        
        // Set up PhysX. We deliberately ask for a much larger time step
        // so that we force it to take the requested number of iterations.
        // In practice, we are using uiNumNxSteps to control how far
        // PhysX steps.
        unsigned int uiNumNxSteps = uiNumGBSteps * m_uiNumSubSteps;
        if (uiNumNxSteps > m_uiMaxSubSteps)
            uiNumNxSteps = m_uiMaxSubSteps;
        float fStepLength = m_fTimestep / (float)m_uiNumSubSteps;
        m_pkPhysXScene->setTiming(
            fStepLength, uiNumNxSteps, NX_TIMESTEP_FIXED);
        
        // Sure to be bigger than needed
        fPhysXStep = fStepLength * uiNumNxSteps + 1.0f;
    }
    else
    {
        // Variable step length.
        // The first two args to setTiming don't matter.
        m_pkPhysXScene->setTiming(m_fTimestep, 1, NX_TIMESTEP_VARIABLE);
        m_fNextFetchTime = fTargetTime;
        fPhysXStep = m_fNextFetchTime - m_fPrevFetchTime;
    }

    // Lock the SDK. If someone else is working on something preventing
    // simulation from starting, then they must be holding this lock
    m_pkManager->WaitSDKLock();
    
    // Push all the sources to the simulation actors.
    if (GetUpdateSrc())
        UpdateSourcesToActors(m_fPrevFetchTime, m_fNextFetchTime);

    // Do the simulation
    m_pkPhysXScene->simulate(fPhysXStep);
    m_pkPhysXScene->flushStream();
    
    m_bInSimFetch = true;

    m_pkManager->ReleaseSDKLock();
    
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXScene::FetchResults(const float fTargetTime, const bool bBlock,
    NxU32* puiErrorState)
{
    // Return value is whether or not simulation was done.
    
    if (!m_bInSimFetch)
        return true;

    m_pkManager->WaitSDKLock();
    if (bBlock || m_fNextFetchTime <= fTargetTime)
    {
        // We need the results, now!
        m_pkPhysXScene->fetchResults(NX_RIGID_BODY_FINISHED, true,
            puiErrorState);
    }
    else if (!m_pkPhysXScene->fetchResults(NX_RIGID_BODY_FINISHED, false,
        puiErrorState))
    {
        // Results not ready yet
        m_pkManager->ReleaseSDKLock();
        return false;
    }

    m_bInSimFetch = false;

    const NxDebugRenderable* pkRenderState = 0;
    if (GetDebugRender())
        pkRenderState = m_pkPhysXScene->getDebugRenderable();

    // Push all the destinations to the simulation actors.
    if (GetUpdateDest())
        UpdateActorsToDests(false);

    m_pkManager->ReleaseSDKLock();

    // Wait till now because we don't need the SDK lock to do this.
    if (pkRenderState)
        SetDebugGeometry(pkRenderState);
    
    m_fPrevFetchTime = m_fNextFetchTime;

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXScene::UpdateSources(const float fTime, const bool bForce)
{
    if (!GetUpdateSrc())
        return;

    // This needs to extract data from the Gamebryo nodes, BUT NOT push it
    // to PhysX. That gets done immediately before simulation. It's more
    // efficient because we don't lock the SDK and we only update PhysX once
    // for each sim step, even if the application is calling this function
    // at a higher frequency.

    NiTransform kInvRootXform;
    GetSceneXform().Invert(kInvRootXform);
        
    for (unsigned int i = 0; i < m_kSources.GetSize(); i++)
    {
        m_kSources.GetAt(i)->UpdateFromSceneGraph(fTime, kInvRootXform,
            1.0f / m_fScalePToW, bForce);   
    }

    if (bForce)
    {
        m_pkManager->WaitSDKLock();
        UpdateSourcesToActors(fTime, fTime);
        m_pkManager->ReleaseSDKLock();
    }
}
//---------------------------------------------------------------------------
void NiPhysXScene::UpdateSource(NiPhysXSrc* pkSrc, const float fTime,
    const bool bForce)
{
    NIASSERT(pkSrc != 0);

    NiTransform kInvRootXform;
    GetSceneXform().Invert(kInvRootXform);

    pkSrc->UpdateFromSceneGraph(fTime, kInvRootXform, 1.0f / m_fScalePToW,
        bForce);

    if (bForce)
    {
        m_pkManager->WaitSDKLock();
        pkSrc->UpdateToActors(fTime, fTime);
        m_pkManager->ReleaseSDKLock();
    }
}
//---------------------------------------------------------------------------
void NiPhysXScene::UpdateSourcesToActors(const float fTBegin,
    const float fTEnd)
{
    // Push all the source state out to actors in the scene. We
    // assume that we hold the SDK lock at this point.
    for (unsigned int i = 0; i < m_kSources.GetSize(); i++)
        m_kSources.GetAt(i)->UpdateToActors(fTBegin, fTEnd);   
}
//---------------------------------------------------------------------------
void NiPhysXScene::UpdateActorsToDests(const bool bForce)
{
    // Get state from actors. Assume we have the SDK lock to do this.
    for (unsigned int i = 0; i < m_kDestinations.GetSize(); i++)
    {
        m_kDestinations.GetAt(i)->UpdateFromActors(m_fNextFetchTime,
            m_kSceneXform, m_fScalePToW, bForce);
    }
}
//---------------------------------------------------------------------------
void NiPhysXScene::UpdateDestinations(const float fTime, const bool bForce)
{
    if (!GetUpdateDest())
        return;
        
    if (bForce)
    {
        m_pkManager->WaitSDKLock();
        UpdateActorsToDests(bForce);
        m_pkManager->ReleaseSDKLock();
    }

    for (unsigned int i = 0; i < m_kDestinations.GetSize(); i++)
    {
        m_kDestinations.GetAt(i)->
            UpdateSceneGraph(fTime, m_kSceneXform, m_fScalePToW, bForce);
    }  
}    
//---------------------------------------------------------------------------
void NiPhysXScene::UpdateDestination(NiPhysXDest* pkDest, const float fTime,
    const bool bForce)
{
    NIASSERT(pkDest != 0);

    if (bForce)
    {
        m_pkManager->WaitSDKLock();
        pkDest->UpdateFromActors(
            m_fPrevFetchTime, m_kSceneXform, m_fScalePToW, bForce);
        m_pkManager->ReleaseSDKLock();
    }
    
    pkDest->UpdateSceneGraph(fTime, m_kSceneXform, m_fScalePToW, bForce);   
}    
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Debug Rendering
//---------------------------------------------------------------------------
void NiPhysXScene::SetDebugRender(const bool bActive, NiNode* pkDebugParent)
{
    SetBit(bActive, DEBUG_RENDER_MASK);
    
    if (bActive && !m_spDebugRoot)
    {
        NIASSERT(pkDebugParent && "NiPhysXScene::SetDebugRender");
        m_spDebugRoot = NiNew NiNode();
        pkDebugParent->AttachChild(m_spDebugRoot);
    }
    else if (!bActive && m_spDebugRoot)
    {
        if (m_spDebugRoot->GetParent())
            m_spDebugRoot->GetParent()->DetachChild(m_spDebugRoot);
        m_spDebugRoot = 0;
        m_kDebugGeomSizes.RemoveAll();
    }
}
//---------------------------------------------------------------------------
void NiPhysXScene::ClearDebugGeometry()
{
    if (m_spDebugRoot)
    {
        NiAVObjectPtr spTriNode;
        for (unsigned int us = m_spDebugRoot->GetArrayCount(); us > 0; us--)
        {
            spTriNode = m_spDebugRoot->DetachChildAt(us - 1);
            spTriNode = 0;
        }
    }
    m_kDebugGeomSizes.RemoveAll();
}
//---------------------------------------------------------------------------
void NiPhysXScene::SetDebugGeometry(const NxDebugRenderable* pkRenderState)
{
    if (!m_pkPhysXScene)
        return;
        
    NIASSERT(m_spDebugRoot != 0);

    // Set the transform on the debug node to make sure coordinate systems
    // match up.
    NiTransform kParentInverse;
    m_spDebugRoot->GetParent()->GetWorldTransform().Invert(kParentInverse);
    NiTransform kDebugLocal = kParentInverse * GetSceneXform();
    m_spDebugRoot->SetTranslate(kDebugLocal.m_Translate);
    m_spDebugRoot->SetRotate(kDebugLocal.m_Rotate);
    m_spDebugRoot->SetScale(kDebugLocal.m_fScale);

    SetDebugTriangleGeometry(pkRenderState);
    SetDebugLineGeometry(pkRenderState);
    SetDebugPointGeometry(pkRenderState);

    m_spDebugRoot->Update(m_fPrevFetchTime);
}
//---------------------------------------------------------------------------
void NiPhysXScene::SetDebugTriangleGeometry(
    const NxDebugRenderable* pkRenderState)
{
    NxU32 uiNTriangles = pkRenderState->getNbTriangles();
    
    // Figure out how many nodes given Gamebryo's limits (conservative)
    unsigned short usNumNodes = (uiNTriangles >> 13) + 1;
    unsigned short usNumPerNode = uiNTriangles / usNumNodes + 1;
    
    // Keep track of the last node we use for geometry.
    unsigned short usNextNode = 0;

    if (uiNTriangles)
    {
        const NxDebugTriangle* pkNxTriangles = pkRenderState->getTriangles();
        unsigned int uiIndex = 0;

        for (unsigned short us = 0;
            us < usNumNodes && uiIndex < uiNTriangles; us++)
        {
            NiPoint3* pkVerts  = NiNew NiPoint3[usNumPerNode * 3];
            NiPoint3* pkNorms = NiNew NiPoint3[usNumPerNode * 3];
            NiColorA* pkColors = NiNew NiColorA[usNumPerNode * 3];
            unsigned short* pusTriConnect = (unsigned short*)
                NiMalloc(sizeof(unsigned short) * usNumPerNode * 3);
            unsigned int uiNumUsed = 0;
            while (uiNumUsed < usNumPerNode && uiIndex < uiNTriangles)
            {
                unsigned int ui3i = 3 * uiNumUsed;
                NiPhysXTypes::NxVec3ToNiPoint3(
                    pkNxTriangles[uiIndex].p0, pkVerts[ui3i]);
                pkVerts[ui3i] *= m_fScalePToW;
                NiPhysXTypes::NxVec3ToNiPoint3(
                    pkNxTriangles[uiIndex].p1, pkVerts[ui3i+1]);
                pkVerts[ui3i+1] *= m_fScalePToW;
                NiPhysXTypes::NxVec3ToNiPoint3(
                    pkNxTriangles[uiIndex].p2, pkVerts[ui3i+2]);
                pkVerts[ui3i+2] *= m_fScalePToW;
                

                NiPoint3 kE1 = pkVerts[ui3i+1] - pkVerts[ui3i];
                NiPoint3 kE2 = pkVerts[ui3i+2] - pkVerts[ui3i];
                NiPoint3 kNorm = kE1.Cross(kE2);
                kNorm.Unitize();
                
                pkNorms[ui3i] = kNorm;
                pkNorms[ui3i+1] = kNorm;
                pkNorms[ui3i+2] = kNorm;
                
                NiPhysXTypes::NxColorToNiColorA(
                    pkNxTriangles[uiIndex].color, pkColors[ui3i]);
                pkColors[ui3i+1] = pkColors[ui3i];
                pkColors[ui3i+2] = pkColors[ui3i];
                pusTriConnect[ui3i] = ui3i;
                pusTriConnect[ui3i+1] = ui3i+1;
                pusTriConnect[ui3i+2] = ui3i+2;
                
                uiIndex++;
                uiNumUsed++;
            }
            
            NiAVObject* pkNode = 0;
            while (usNextNode < m_spDebugRoot->GetArrayCount())
            {
                pkNode = m_spDebugRoot->GetAt(usNextNode);
                if (pkNode && NiIsKindOf(NiTriShape, pkNode))
                    break;
                
                usNextNode++;
            }

            if (usNextNode == m_spDebugRoot->GetArrayCount())
            {
                NiMaterialProperty* pkMaterial = NiNew NiMaterialProperty();

                NiVertexColorProperty* pkVertColorProp =
                    NiNew NiVertexColorProperty();
                pkVertColorProp->SetSourceMode(
                    NiVertexColorProperty::SOURCE_EMISSIVE);
                pkVertColorProp->SetLightingMode(
                    NiVertexColorProperty::LIGHTING_E);
                
                NiTriShape* pkTriNode = NiNew NiTriShape(uiNumUsed * 3,
                    pkVerts, pkNorms, pkColors, 0, 0,
                    NiGeometryData::NBT_METHOD_NONE,
                    uiNTriangles, pusTriConnect);
                m_spDebugRoot->AttachChild(pkTriNode, true);
                pkTriNode->AttachProperty(pkMaterial);
                pkTriNode->AttachProperty(pkVertColorProp);
                pkTriNode->GetModelData()->SetConsistency(
                    NiGeometryData::VOLATILE);

                // Figure out where we added it
                unsigned int ui = 0;
                while (ui < m_spDebugRoot->GetArrayCount()
                    && m_spDebugRoot->GetAt(ui) != pkTriNode)
                {
                    ui++;
                }
                if (m_kDebugGeomSizes.GetAllocatedSize() <= ui)
                    m_kDebugGeomSizes.SetSize(ui + 1);
                m_kDebugGeomSizes.SetAt(ui, uiNumUsed * 3);

                pkTriNode->UpdateProperties();
            }
            else
            {
                NiTriShape* pkTri = (NiTriShape*)pkNode;
                NiGeometryData* pkGeomData = pkTri->GetModelData();
                pkGeomData->Replace(uiNumUsed * 3, pkVerts, pkNorms,
                    pkColors, 0, 0, NiGeometryData::NBT_METHOD_NONE);
                NiTriShapeData* pkTriData = (NiTriShapeData*)pkGeomData;
                pkTriData->Replace(uiNTriangles, pusTriConnect);
                pkTriData->MarkAsChanged(
                    NiGeometryData::VERTEX_MASK|
                    NiGeometryData::NORMAL_MASK|
                    NiGeometryData::COLOR_MASK|
                    NiTriBasedGeomData::TRIANGLE_INDEX_MASK|
                    NiTriBasedGeomData::TRIANGLE_COUNT_MASK);
                
                if (m_kDebugGeomSizes.GetAt(usNextNode) < uiNumUsed * 3)
                {
                    NiRenderer::GetRenderer()->PurgeGeometryData(pkTriData);
                    m_kDebugGeomSizes.SetAt(usNextNode, uiNumUsed * 3);
                }

                pkTri->UpdateProperties();
            }
                
            usNextNode++;
        }
    }

    // Delete any existing geometry
    NiAVObjectPtr spNode;
    for (unsigned int us = m_spDebugRoot->GetArrayCount();
        us > usNextNode; us--)
    {
        spNode = m_spDebugRoot->GetAt(us - 1);
        
        if (!spNode)
            continue;
            
        if (NiIsKindOf(NiTriShape, spNode))
        {
            spNode = m_spDebugRoot->DetachChildAt(us - 1);
            spNode = 0;
            m_kDebugGeomSizes.SetAt(us - 1, 0);

        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXScene::SetDebugLineGeometry(const NxDebugRenderable* pkRenderState)
{
    NxU32 uiNLines = pkRenderState->getNbLines();

    // Figure out how many nodes given Gamebryo's limits (conservative)
    unsigned short usNumNodes = (uiNLines >> 14) + 1;
    unsigned short usNumPerNode = uiNLines / usNumNodes + 1;
    unsigned short usNextNode = 0;

    if (uiNLines)
    {
        const NxDebugLine* pkNxLines = pkRenderState->getLines();
        unsigned int uiIndex = 0;

        for (unsigned short us = 0;
            us < usNumNodes && uiIndex < uiNLines; us++)
        {
            NiPoint3* pkVerts  = NiNew NiPoint3[usNumPerNode * 2];
            NiColorA* pkColors = NiNew NiColorA[usNumPerNode * 2];
            NiBool* pbLineConnect = (NiBool*)
                NiMalloc(sizeof(NiBool) * usNumPerNode * 2);
            unsigned int uiNumUsed = 0;
            while (uiNumUsed < usNumPerNode && uiIndex < uiNLines)
            {
                unsigned int ui2i = 2 * uiNumUsed;
                NiPhysXTypes::NxVec3ToNiPoint3(pkNxLines[uiIndex].p0,
                    pkVerts[ui2i]);
                pkVerts[ui2i] *= m_fScalePToW;
                NiPhysXTypes::NxVec3ToNiPoint3(pkNxLines[uiIndex].p1,
                    pkVerts[ui2i+1]);
                pkVerts[ui2i+1] *= m_fScalePToW;
                NiPhysXTypes::NxColorToNiColorA(pkNxLines[uiIndex].color,
                    pkColors[ui2i]);
                pkColors[ui2i+1] = pkColors[ui2i];
                pbLineConnect[ui2i] = true;
                pbLineConnect[ui2i+1] = false;
   
                uiIndex++;
                uiNumUsed++;
            }
                        
            NiAVObject* pkNode = 0;
            while (usNextNode < m_spDebugRoot->GetArrayCount())
            {
                pkNode = m_spDebugRoot->GetAt(usNextNode);
                if (pkNode && NiIsKindOf(NiLines, pkNode))
                    break;
                
                usNextNode++;
            }

            if (usNextNode == m_spDebugRoot->GetArrayCount())
            {
                NiMaterialProperty* pkMaterial = NiNew NiMaterialProperty();

                NiVertexColorProperty* pkVertColorProp =
                    NiNew NiVertexColorProperty();
                pkVertColorProp->SetSourceMode(
                    NiVertexColorProperty::SOURCE_EMISSIVE);
                pkVertColorProp->SetLightingMode(
                    NiVertexColorProperty::LIGHTING_E);
                
                NiLines* pkLines = NiNew NiLines(uiNumUsed * 2, pkVerts,
                    pkColors, 0, 0, NiGeometryData::NBT_METHOD_NONE,
                    pbLineConnect);
                m_spDebugRoot->AttachChild(pkLines, true);
                pkLines->AttachProperty(pkMaterial);
                pkLines->AttachProperty(pkVertColorProp);
                pkLines->GetModelData()->
                    SetConsistency(NiGeometryData::VOLATILE);

                // Figure out where we added it
                unsigned int ui = 0;
                while (ui < m_spDebugRoot->GetArrayCount()
                    && m_spDebugRoot->GetAt(ui) != pkLines)
                {
                    ui++;
                }
                if (m_kDebugGeomSizes.GetAllocatedSize() <= ui)
                    m_kDebugGeomSizes.SetSize(ui + 1);
                m_kDebugGeomSizes.SetAt(ui, uiNumUsed * 2);

                pkLines->UpdateProperties();
            }
            else
            {
                NIASSERT(NiIsKindOf(NiLines, pkNode));
                NiLines* pkLines = (NiLines*)pkNode;
                NiGeometryData* pkGeomData = pkLines->GetModelData();
                pkGeomData->Replace(uiNumUsed * 2, pkVerts, 0, pkColors, 0,
                    0, NiGeometryData::NBT_METHOD_NONE);
                NiLinesData* pkLinesData = (NiLinesData*)pkGeomData;
                pkLinesData->Replace(pbLineConnect);
                pkLinesData->MarkAsChanged(
                    NiGeometryData::VERTEX_MASK|
                    NiGeometryData::NORMAL_MASK|
                    NiGeometryData::COLOR_MASK);
                
                if (m_kDebugGeomSizes.GetAt(usNextNode) < uiNumUsed * 2)
                {
                    NiRenderer::GetRenderer()->PurgeGeometryData(pkLinesData);
                    m_kDebugGeomSizes.SetAt(usNextNode, uiNumUsed * 2);
                }

                pkLines->UpdateProperties();
            }
                
            usNextNode++;
        }
    }

    // Delete any existing geometry
    NiAVObjectPtr spNode;
    for (unsigned int us = m_spDebugRoot->GetArrayCount();
        us > usNextNode; us--)
    {
        spNode = m_spDebugRoot->GetAt(us - 1);
        
        if (!spNode)
            continue;
            
        if (NiIsKindOf(NiLines, spNode))
        {
            spNode = m_spDebugRoot->DetachChildAt(us - 1);
            spNode = 0;
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXScene::SetDebugPointGeometry(
    const NxDebugRenderable* pkRenderState)
{   
    NxU32 uiNPoints = pkRenderState->getNbPoints();

    // Figure out how many nodes given Gamebryo's limits (conservative)
    unsigned short usNumNodes = (uiNPoints >> 15) + 1;
    unsigned short usNumPerNode = uiNPoints / usNumNodes + 1;
    unsigned short usNextNode = 0;

    if (uiNPoints)
    {
        const NxDebugPoint* pkNxPoints = pkRenderState->getPoints();
        unsigned int uiIndex = 0;

        for (unsigned short us = 0;
            us < usNumNodes && uiIndex < uiNPoints; us++)
        {
            // Memory is passed into and owned by the GeometryData.
            NiPoint3* pkVerts  = NiNew NiPoint3[usNumPerNode];
            NiColorA* pkColors = NiNew NiColorA[usNumPerNode];
            unsigned int uiNumUsed = 0;
            while (uiNumUsed < usNumPerNode && uiIndex < uiNPoints)
            {
                NiPhysXTypes::NxVec3ToNiPoint3(
                    pkNxPoints[uiIndex].p, pkVerts[uiNumUsed]);
                pkVerts[uiNumUsed] *= m_fScalePToW;
                NiPhysXTypes::NxColorToNiColorA(
                    pkNxPoints[uiIndex].color, pkColors[uiNumUsed]);
                uiNumUsed++;
                uiIndex++;
            }
            
            NiAVObject* pkNode = 0;
            while (usNextNode < m_spDebugRoot->GetArrayCount())
            {
                pkNode = m_spDebugRoot->GetAt(usNextNode);
                if (pkNode && NiIsKindOf(NiParticles, pkNode))
                    break;
                
                usNextNode++;
            }

            if (usNextNode == m_spDebugRoot->GetArrayCount())
            {
                NiMaterialProperty* pkMaterial = NiNew NiMaterialProperty();

                NiVertexColorProperty* pkVertColorProp =
                    NiNew NiVertexColorProperty();
                pkVertColorProp->SetSourceMode(
                    NiVertexColorProperty::SOURCE_EMISSIVE);
                pkVertColorProp->SetLightingMode(
                    NiVertexColorProperty::LIGHTING_E);
                
                NiParticles* pkPoints = NiNew NiParticles(uiNumUsed, pkVerts, 
                    0, pkColors, 0, 0, 0);
                m_spDebugRoot->AttachChild(pkPoints);
                pkPoints->AttachProperty(pkMaterial);
                pkPoints->AttachProperty(pkVertColorProp);
                pkPoints->GetModelData()->SetConsistency(
                    NiGeometryData::VOLATILE);

                // Figure out where we added it
                unsigned int ui = 0;
                while (ui < m_spDebugRoot->GetArrayCount()
                    && m_spDebugRoot->GetAt(ui) != pkPoints)
                {
                    ui++;
                }
                if (m_kDebugGeomSizes.GetAllocatedSize() <= ui)
                    m_kDebugGeomSizes.SetSize(ui + 1);
                m_kDebugGeomSizes.SetAt(ui, uiNumUsed);

                pkPoints->UpdateProperties();
            }
            else
            {
                NIASSERT(NiIsKindOf(NiParticles, pkNode));
                NiParticles* pkPoints = (NiParticles*)pkNode;
                NiGeometryData* pkPointData = pkPoints->GetModelData();
                NIASSERT(pkPointData != 0);
                pkPointData->Replace(uiNumUsed, pkVerts, 0, pkColors, 0, 0,
                    NiGeometryData::NBT_METHOD_NONE); 
                pkPointData->MarkAsChanged(
                    NiGeometryData::VERTEX_MASK|
                    NiGeometryData::COLOR_MASK);

                if (m_kDebugGeomSizes.GetAt(usNextNode) < uiNumUsed)
                {
                    NiRenderer::GetRenderer()->PurgeGeometryData(pkPointData);
                    m_kDebugGeomSizes.SetAt(usNextNode, uiNumUsed);
                }

                pkPoints->UpdateProperties();
            }

            usNextNode++;
       }
    }

    // Delete any existing geometry
    NiAVObjectPtr spNode;
    for (unsigned int us = m_spDebugRoot->GetArrayCount();
        us > usNextNode; us--)
    {
        spNode = m_spDebugRoot->GetAt(us - 1);
        
        if (!spNode)
            continue;
            
        if (NiIsKindOf(NiParticles, spNode))
        {
            spNode = m_spDebugRoot->DetachChildAt(us - 1);
            spNode = 0;
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Snapshot functions
//---------------------------------------------------------------------------
bool NiPhysXScene::CreateSceneFromSnapshot(const unsigned int uiIndex)
{
    NIASSERT(m_spSnapshot != 0);
  
    if (m_pkPhysXScene)
        ReleaseScene();
    
    m_pkManager->WaitSDKLock();
    NxScene* pkScene = m_spSnapshot->CreateScene();
    if (!pkScene)
        return false;
    
    // Create any compartments
    NiTMap<unsigned int, NxCompartmentDesc*>& kCompartmentMap =
        m_spSnapshot->GetCompartmentMap();
    NiTMapIterator kIter = kCompartmentMap.GetFirstPos();
    while (kIter)
    {
        unsigned int uiID;
        NxCompartmentDesc* pkDesc;
        kCompartmentMap.GetNext(kIter, uiID, pkDesc);
        
        NxCompartment* pkCompartment = pkScene->createCompartment(*pkDesc);
        NIASSERT(pkCompartment);
        m_kCompartmentMap.SetAt(uiID, pkCompartment);
    }

    // Create any props.
    unsigned int uiNumProps = m_kProps.GetSize();
    for (unsigned int ui = 0; ui < uiNumProps; ui++)
    {
        NiPhysXProp* pkProp = m_kProps.GetAt(ui);
        NiPhysXPropDesc* pkSnapshot = pkProp->GetSnapshot();
        if (pkSnapshot)
        {
            NxCompartment* pkRBCompartment = 0;
            unsigned int uiID = pkSnapshot->GetRBCompartmentID();
            if (uiID > 0)
            {
                m_kCompartmentMap.GetAt(uiID, pkRBCompartment);
                NIASSERT(pkRBCompartment);
            }
            NxCompartment* pkClothCompartment = 0;
            uiID = pkSnapshot->GetClothCompartmentID();
            if (uiID > 0)
            {
                m_kCompartmentMap.GetAt(uiID, pkClothCompartment);
                NIASSERT(pkClothCompartment);
            }
            NxCompartment* pkFluidCompartment = 0;
            uiID = pkSnapshot->GetFluidCompartmentID();
            if (uiID > 0)
            {
                m_kCompartmentMap.GetAt(uiID, pkFluidCompartment);
                NIASSERT(pkFluidCompartment);
            }
            if (!pkSnapshot->CreateInScene(pkScene, pkProp->GetXform(),
                pkRBCompartment, pkClothCompartment, pkFluidCompartment,
                pkProp->GetKeepMeshes(), uiIndex))
            {
                return false;
            }
        }
    }

    m_pkManager->ReleaseSDKLock();
    
    SetPhysXScene(pkScene);
    
    return true;
}
//---------------------------------------------------------------------------
unsigned int NiPhysXScene::AddSnapshotState()
{
    unsigned int uiStateNum = 0;
    
    m_pkManager->WaitSDKLock();

    // Work through all props, setting snapshot state.
    unsigned int uiNumProps = m_kProps.GetSize();
    for (unsigned int ui = 0; ui < uiNumProps; ui++)
    {
        NiPhysXProp* pkProp = m_kProps.GetAt(ui);
        NiPhysXPropDesc* pkSnapshot = pkProp->GetSnapshot();
        if (pkSnapshot)
        {
            NxMat34 kInvXform;
            pkProp->GetXform().getInverse(kInvXform);
            unsigned int uiThisStateNum;
            uiThisStateNum = pkSnapshot->AddState(m_pkPhysXScene, kInvXform);
            if (ui == 0)
            {
                uiStateNum = uiThisStateNum;
            }
            NIASSERT(uiStateNum == uiThisStateNum);
        }
    }

    m_pkManager->ReleaseSDKLock();
    
    return uiStateNum;
}
//---------------------------------------------------------------------------
void NiPhysXScene::UpdateSnapshotState(const unsigned int uiIndex)
{
    m_pkManager->WaitSDKLock();

    // Work through all props, setting snapshot state.
    unsigned int uiNumProps = m_kProps.GetSize();
    for (unsigned int ui = 0; ui < uiNumProps; ui++)
    {
        NiPhysXProp* pkProp = m_kProps.GetAt(ui);
        NiPhysXPropDesc* pkSnapshot = pkProp->GetSnapshot();
        if (pkSnapshot)
        {
            NxMat34 kInvXform;
            pkProp->GetXform().getInverse(kInvXform);
            pkSnapshot->UpdateState(m_pkPhysXScene, uiIndex, kInvXform);
        }
    }

    m_pkManager->ReleaseSDKLock();
}
//---------------------------------------------------------------------------
void NiPhysXScene::RestoreSnapshotState(const unsigned int uiIndex)
{
    m_pkManager->WaitSDKLock();

    // Work through all props, restoring snapshot state.
    unsigned int uiNumProps = m_kProps.GetSize();
    for (unsigned int ui = 0; ui < uiNumProps; ui++)
    {
        NiPhysXProp* pkProp = m_kProps.GetAt(ui);
        NiPhysXPropDesc* pkSnapshot = pkProp->GetSnapshot();
        if (pkSnapshot)
        {
            pkSnapshot->RestoreState(m_pkPhysXScene, uiIndex,
                pkProp->GetXform(), pkProp->GetKeepMeshes());
        }
    }

    m_pkManager->ReleaseSDKLock();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXScene);
//---------------------------------------------------------------------------
void NiPhysXScene::CopyMembers(NiPhysXScene* pkDest,
    NiCloningProcess& kCloning)
{
    NiObjectNET::CopyMembers(pkDest, kCloning);
    
    pkDest->m_pkManager = m_pkManager;

    pkDest->m_kSceneXform = m_kSceneXform;
    pkDest->m_fScalePToW = m_fScalePToW;
    pkDest->m_pkPhysXScene = 0;

    pkDest->m_kProps.SetSize(m_kProps.GetSize());
    for (unsigned int ui = 0; ui < m_kProps.GetSize(); ui++)
    {
        NiPhysXProp* pkPropClone = (NiPhysXProp*)
            m_kProps.GetAt(ui)->CreateSharedClone(kCloning);
        pkDest->m_kProps.SetAt(ui, pkPropClone);
    }  

    pkDest->m_kSources.SetSize(m_kSources.GetSize());
    for (unsigned int ui = 0; ui < m_kSources.GetSize(); ui++)
    {
        NiPhysXSrc* pkSrcClone = (NiPhysXSrc*)
            m_kSources.GetAt(ui)->CreateSharedClone(kCloning);
        pkDest->m_kSources.SetAt(ui, pkSrcClone);
    }
    pkDest->m_kDestinations.SetSize(m_kDestinations.GetSize());
    for (unsigned int ui = 0; ui < m_kDestinations.GetSize(); ui++)
    {
        NiPhysXDest* pkDestClone = (NiPhysXDest*)
            m_kDestinations.GetAt(ui)->CreateSharedClone(kCloning);
        pkDest->m_kDestinations.SetAt(ui, pkDestClone);
    }

    pkDest->m_fTimestep = m_fTimestep;
    pkDest->m_uiNumSubSteps = m_uiNumSubSteps;
    pkDest->m_uiMaxSubSteps = m_uiMaxSubSteps;

    pkDest->m_fPrevFetchTime = m_fPrevFetchTime;
    pkDest->m_fNextFetchTime = m_fNextFetchTime;

    pkDest->m_bInSimFetch = false;

    pkDest->m_spDebugRoot = 0;
    
    // Snapshots
    if (m_spSnapshot)
    {
        pkDest->m_spSnapshot =
            (NiPhysXSceneDesc*)m_spSnapshot->CreateSharedClone(kCloning);
    }
    else
    {
        pkDest->m_spSnapshot = 0;
    }

    pkDest->m_uFlags = m_uFlags;
}
//---------------------------------------------------------------------------
void NiPhysXScene::ProcessClone(NiCloningProcess& kCloning)
{
    NiObjectNET::ProcessClone(kCloning);

    bool bCloned;
    NiObject* pkCloneObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(this, pkCloneObj);
    if (!bCloned)
        return;

    for (unsigned int ui = 0; ui < m_kProps.GetSize(); ui++)
    {
        bool bJunk;
        NiPhysXProp* pkProp = m_kProps.GetAt(ui);
        if (!kCloning.m_pkProcessMap->GetAt(pkProp, bJunk))
            pkProp->ProcessClone(kCloning);
    }

    for (unsigned int ui = 0; ui < m_kSources.GetSize(); ui++)
    {
        bool bJunk;
        NiPhysXSrc* pkSrc = m_kSources.GetAt(ui);
        if (!kCloning.m_pkProcessMap->GetAt(pkSrc, bJunk))
            pkSrc->ProcessClone(kCloning);
    }
    for (unsigned int ui = 0; ui < m_kDestinations.GetSize(); ui++)
    {
        bool bJunk;
        NiPhysXDest* pkDest = m_kDestinations.GetAt(ui);
        if (!kCloning.m_pkProcessMap->GetAt(pkDest, bJunk))
            pkDest->ProcessClone(kCloning);
    }
    if (m_spSnapshot)
    {
        bool bJunk;
        if (!kCloning.m_pkProcessMap->GetAt(m_spSnapshot, bJunk))
            m_spSnapshot->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXScene::RegisterStreamables(NiStream& kStream)
{
    if (!NiObjectNET::RegisterStreamables(kStream))
        return false;
        
    unsigned int uiSize;

    // Props
    uiSize = m_kProps.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiPhysXPropPtr spProp = m_kProps.GetAt(ui);
        NIASSERT(spProp != 0);
        spProp->RegisterStreamables(kStream);
    }
    
    // Sources and destinations
    uiSize = m_kSources.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiPhysXSrcPtr spSrc = m_kSources.GetAt(ui);
        NIASSERT(spSrc != 0);
        spSrc->RegisterStreamables(kStream);
    }
    uiSize = m_kDestinations.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiPhysXDestPtr spDest = m_kDestinations.GetAt(ui);
        NIASSERT(spDest != 0);
        spDest->RegisterStreamables(kStream);
    }
       
    if (m_spSnapshot)
        m_spSnapshot->RegisterStreamables(kStream);
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXScene::SaveBinary(NiStream& kStream)
{
    NiObjectNET::SaveBinary(kStream);

    m_kSceneXform.SaveBinary(kStream);
        
    NiStreamSaveBinary(kStream, m_fScalePToW);

    unsigned int uiSize;

    // Props
    uiSize = m_kProps.GetSize();
    NiStreamSaveBinary(kStream, uiSize);
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        kStream.SaveLinkID(m_kProps.GetAt(ui));
    }
    
    // Sources and Destinations
    uiSize = m_kSources.GetSize();
    NiStreamSaveBinary(kStream, uiSize);
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        kStream.SaveLinkID(m_kSources.GetAt(ui));
    }
    uiSize = m_kDestinations.GetSize();
    NiStreamSaveBinary(kStream, uiSize);
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        kStream.SaveLinkID(m_kDestinations.GetAt(ui));
    }
    
    NiStreamSaveBinary(kStream, m_fTimestep);
    NiStreamSaveBinary(kStream, m_uiNumSubSteps);
    NiStreamSaveBinary(kStream, m_uiMaxSubSteps);

    kStream.SaveLinkID(m_spSnapshot);
    
    NiStreamSaveBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXScene);
//---------------------------------------------------------------------------
void NiPhysXScene::LoadBinary(NiStream& kStream)
{
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 3, 0, 2))
    {
        LoadOldBinary(kStream);
        return;
    }

    NiObjectNET::LoadBinary(kStream);
    
    m_kSceneXform.LoadBinary(kStream);
    
    NiStreamLoadBinary(kStream, m_fScalePToW);

    unsigned int uiSize;

    // Props
    NiStreamLoadBinary(kStream, uiSize);
    m_kProps.SetSize(uiSize);
    for (unsigned int i = 0; i < uiSize; i++)
    {
        m_kProps.SetAt(i, (NiPhysXProp*)kStream.ResolveLinkID());
    }

    // Sources and Destinations
    NiStreamLoadBinary(kStream, uiSize);
    m_kSources.SetSize(uiSize);
    for (unsigned int i = 0; i < uiSize; i++)
    {
        m_kSources.SetAt(i, (NiPhysXSrc*)kStream.ResolveLinkID());
    }
    NiStreamLoadBinary(kStream, uiSize);
    m_kDestinations.SetSize(uiSize);
    for (unsigned int i = 0; i < uiSize; i++)
    {
        m_kDestinations.SetAt(i, (NiPhysXDest*)kStream.ResolveLinkID());
    }
    
    NiStreamLoadBinary(kStream, m_fTimestep);
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 9))
    {
        NiStreamLoadBinary(kStream, m_uiNumSubSteps);
        NiStreamLoadBinary(kStream, m_uiMaxSubSteps);
    }

    m_spSnapshot = (NiPhysXSceneDesc*)kStream.ResolveLinkID();

    NiStreamLoadBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
void NiPhysXScene::LinkObject(NiStream& kStream)
{
    NiObjectNET::LinkObject(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXScene::LoadOldBinary(NiStream& kStream)
{
    NiObjectNET::LoadBinary(kStream);
    
    m_kSceneXform.LoadBinary(kStream);
    
    NiStreamLoadBinary(kStream, m_fScalePToW);

    unsigned int uiSize;

    // Sources and Destinations
    NiStreamLoadBinary(kStream, uiSize);
    m_kSources.SetSize(uiSize);
    for (unsigned int i = 0; i < uiSize; i++)
    {
        m_kSources.SetAt(i, (NiPhysXSrc*)kStream.ResolveLinkID());
    }
    NiStreamLoadBinary(kStream, uiSize);
    m_kDestinations.SetSize(uiSize);
    for (unsigned int i = 0; i < uiSize; i++)
    {
        m_kDestinations.SetAt(i, (NiPhysXDest*)kStream.ResolveLinkID());
    }
    
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 2, 0, 8))
    {
        NiStreamLoadBinary(kStream, m_fTimestep);
        NiBool bVal;
        NiStreamLoadBinary(kStream, bVal);
        m_bKeepMeshesBackCompatable = bVal ? true : false;
    }

    m_spSnapshot = (NiPhysXSceneDesc*)kStream.ResolveLinkID();
    NIASSERT(m_spSnapshot != 0);

    NiStreamLoadBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
void NiPhysXScene::PostLinkObject(NiStream& kStream)
{
    if (!m_spSnapshot)
        return;

    // Do the work of converting prior versions. All pointers are correctly
    // set at this stage. We have to find content in the snapshot and move it
    // to a new NiPhysXProp object.
    //
    if (m_spSnapshot->GetActorCountBackCompatable())
    {
        NiPhysXProp* pkProp = NiNew NiPhysXProp();
        
        pkProp->SetScaleFactor(GetScaleFactor());
        
        unsigned int uiSize;
        uiSize = m_kSources.GetSize();
        for (unsigned int ui = 0; ui < uiSize; ui++)
        {
            pkProp->AddSource(m_kSources.GetAt(ui));
        }
        uiSize = m_kDestinations.GetSize();
        for (unsigned int ui = 0; ui < uiSize; ui++)
        {
            pkProp->AddDestination(m_kDestinations.GetAt(ui));
        }
        
        pkProp->SetKeepMeshes(m_bKeepMeshesBackCompatable);
        
        NiPhysXPropDesc* pkPropSnapshot = NiNew NiPhysXPropDesc();
        
        uiSize = m_spSnapshot->GetActorCountBackCompatable();
        for (unsigned int ui = 0; ui < uiSize; ui++)
        {
            pkPropSnapshot->AddActor(
                m_spSnapshot->GetActorAtBackCompatable(ui));
        }
        
        uiSize = m_spSnapshot->GetJointCountBackCompatable();
        for (unsigned int ui = 0; ui < uiSize; ui++)
        {
            pkPropSnapshot->AddJoint(
                m_spSnapshot->GetJointAtBackCompatable(ui));
        }
        
        NiTMapIterator kIter = m_spSnapshot->GetFirstMaterialBackCompatable();
        while (kIter)
        {
            NiPhysXMaterialDescPtr spMatDesc;
            NxMaterialIndex usIndex;
            m_spSnapshot->
                GetNextMaterialBackCompatable(kIter, usIndex, spMatDesc);
            NxMaterialDesc kMaterial;
            spMatDesc->ToMaterialDesc(kMaterial);
            pkPropSnapshot->SetMaterialAt(kMaterial, usIndex);
            spMatDesc = 0;
        }
        
        pkPropSnapshot->SetNumStates(
            m_spSnapshot->GetNumStatesBackCompatable());
        
        pkProp->SetSnapshot(pkPropSnapshot);
        
        AddProp(pkProp);
    }
    
    m_spSnapshot->ClearBackCompatable();
}
//---------------------------------------------------------------------------
bool NiPhysXScene::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXScene, pkObject));
    if(!NiObjectNET::IsEqual(pkObject))
        return false;
        
    NiPhysXScene* pkNewObject = (NiPhysXScene*)pkObject;

    if (m_kSceneXform != pkNewObject->m_kSceneXform)
        return false;
        
    if (m_fScalePToW != pkNewObject->m_fScalePToW)
        return false;

    if (m_kProps.GetSize() != pkNewObject->m_kProps.GetSize())
        return false;
    for (unsigned int i = 0; i < m_kProps.GetSize(); i++)
    {
        NiPhysXProp* pkProp = m_kProps.GetAt(i);
        NiPhysXProp* pkNewProp = pkNewObject->m_kProps.GetAt(i);
        if (!pkProp->IsEqual(pkNewProp))
            return false;
    }

    if (m_kSources.GetSize() != pkNewObject->m_kSources.GetSize())
        return false;
    for (unsigned int i = 0; i < m_kSources.GetSize(); i++)
    {
        NiPhysXSrc* pkSrc = m_kSources.GetAt(i);
        NiPhysXSrc* pkNewSrc = pkNewObject->m_kSources.GetAt(i);
        if (!pkSrc->IsEqual(pkNewSrc))
            return false;
    }
    if (m_kDestinations.GetSize() != pkNewObject->m_kDestinations.GetSize())
        return false;
    for (unsigned int i = 0; i < m_kDestinations.GetSize(); i++)
    {
        NiPhysXDest* pkDest = m_kDestinations.GetAt(i);
        NiPhysXDest* pkNewDest = pkNewObject->m_kDestinations.GetAt(i);
        if (!pkDest->IsEqual(pkNewDest))
            return false;
    }

    if (m_fTimestep != pkNewObject->m_fTimestep ||
        m_uiNumSubSteps != pkNewObject->m_uiNumSubSteps ||
        m_uiMaxSubSteps != pkNewObject->m_uiMaxSubSteps)
    {
        return false;
    }
    
    if (m_spSnapshot)
    {
        if (!pkNewObject->m_spSnapshot)
            return false;
        if (!m_spSnapshot->IsEqual(pkNewObject->m_spSnapshot))
            return false;
    }
    else
    {
        if (pkNewObject->m_spSnapshot)
            return false;
    }

    if (m_uFlags != pkNewObject->m_uFlags)
        return false;
        
    return true;
}
//---------------------------------------------------------------------------
