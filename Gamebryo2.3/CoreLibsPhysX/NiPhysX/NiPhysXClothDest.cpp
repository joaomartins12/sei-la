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

NiImplementRTTI(NiPhysXClothDest, NiPhysXDest);

//---------------------------------------------------------------------------
NiPhysXClothDest::NiPhysXClothDest(NiTriShape* pkTargetNode) : NiPhysXDest(),
    m_kSplitParentMap(17)
{
    m_spTarget = pkTargetNode;
    NIASSERT(m_spTarget);
    NiTriShapeData* pkTriData = (NiTriShapeData*)m_spTarget->GetModelData();
    m_usOriginalVertexCount = pkTriData->GetVertexCount();

    m_pkCloth = 0;
    m_pusVertexMap = 0;
    m_pkParentActor = 0;
    m_kWorldXform.MakeIdentity();
    m_fMeshGrowFactor = 1.5;
    m_bDoSleepXforms = false;
    m_bSleeping = false;
    m_usNBTSet = 0;

    m_kClothData.verticesPosBegin = 0;
    m_kClothData.numVerticesPtr = 0;
    m_kClothData.indicesBegin = 0;
    m_kClothData.numIndicesPtr = 0;
    m_kClothData.parentIndicesBegin = 0;
    m_kClothData.numParentIndicesPtr = 0;
    m_kClothData.verticesNormalBegin = 0;
}
//---------------------------------------------------------------------------
NiPhysXClothDest::NiPhysXClothDest() : m_kSplitParentMap(17)
{
    m_spTarget = 0;
    m_pkCloth = 0;
    m_pusVertexMap = 0;
    m_pkParentActor = 0;
    m_kWorldXform.MakeIdentity();
    m_fMeshGrowFactor = 1.5;
    m_bDoSleepXforms = true;
    m_bSleeping = false;

    m_kClothData.verticesPosBegin = 0;
    m_kClothData.numVerticesPtr = 0;
    m_kClothData.indicesBegin = 0;
    m_kClothData.numIndicesPtr = 0;
    m_kClothData.parentIndicesBegin = 0;
    m_kClothData.numParentIndicesPtr = 0;
    m_kClothData.verticesNormalBegin = 0;
}
//---------------------------------------------------------------------------
NiPhysXClothDest::~NiPhysXClothDest()
{
    m_spTarget = 0;
    
    DeallocateClothData();
}
//---------------------------------------------------------------------------
void NiPhysXClothDest::SetCloth(NxCloth* pkCloth,
    const unsigned short* pusVertexMap)
{
    RestoreTriangleIndices();

    DeallocateClothData();

    m_pkCloth = pkCloth;
    
    if (!m_pkCloth)
        return;

    AllocateClothData(pusVertexMap);
    
    m_pkCloth->setMeshData(m_kClothData);
    
    m_bSleeping = false;
}
//---------------------------------------------------------------------------
void NiPhysXClothDest::UpdateFromActors(const float fT,
    const NiTransform& kRootTransform, const float fScalePToW,
    const bool bForce)
{
    if (!m_pkCloth)
    {
        return;
    }
    
    if (!m_bActive && !bForce)
        return;

    if (m_pkCloth->isSleeping())
    {
        if (m_bSleeping)
        {
            // Code path if the cloth is asleep and we have updated already
            if (m_bDoSleepXforms)
            {
                NiAVObject* pkParent = m_spTarget->GetParent();
                if (pkParent)
                {
                    // Figure out where we want the node to be in the world.
                    NiTransform kTarget = kRootTransform * m_kWorldXform;
                    
                    // Get the parent's inverse
                    NiTransform kParentInv;
                    pkParent->GetWorldTransform().Invert(kParentInv);
                    
                    // Get the offset transform to make the new pose match
                    // the old one
                    NiTransform kOffset = kTarget * kParentInv;
                        
                    m_spTarget->SetRotate(kOffset.m_Rotate);
                    m_spTarget->SetTranslate(kOffset.m_Translate);
                    m_spTarget->SetScale(kOffset.m_fScale);
                }
                else
                {
                    // We need the global transform such that the node
                    // ends up at m_kWorldXform in PhysX space
                    NiTransform kOffset = kRootTransform * m_kWorldXform;
                    m_spTarget->SetRotate(kOffset.m_Rotate);
                    m_spTarget->SetTranslate(kOffset.m_Translate);
                    m_spTarget->SetScale(kOffset.m_fScale);
                }
            }
            return;
        }
        else
        {
            // First sleeping frame
            if (m_bDoSleepXforms)
            {
                // While the cloth is sleeping, is it presumably static in
                // the PhysX coordinate system. Hence, we need to keep the
                // target Gamebryo node in a fixed location in the PhysX
                // coordinate system. So record that location.
                NiTransform kInvRootTransform;
                kRootTransform.Invert(kInvRootTransform);
                const NiTransform kSrcXform = m_spTarget->GetWorldTransform();
                m_kWorldXform = kInvRootTransform * kSrcXform;
            }

            m_bSleeping = true;
        }
    }
    else
    {
        m_bSleeping = false;
    }

    // 1. Compute the transformation from PhysX global coordinates,
    //    which is the reference system for the NxMeshData object, to
    //    Gamebryo scene graph local coordinates. If the actor parent is
    //    present, we obtain its PhysX pose and use the parent transform
    //    to compute the relative pose between PhysX global coordinates
    //    and each target mesh. If the actor parent is not set, we use the
    //    target’s world transformation in combination with the PhysX root
    //    transform passed as an argument to UpdateFromActors. This code is
    //    very similar to the existing rigid-body code.
    NiTransform kXform;
    if (m_pkParentActor)
    {
        // X(Geom<-Cloth) = X(Geom<-Cloth)
        // = X(Geom<-Parent) X(PhysXParent<-Cloth)
        // = X(Parent<-Geom)^-1 X(PhysXScene<-Parent)^-1
        
        // Get the parent's transform, inverted
        NxMat34 kPose = m_pkParentActor->getGlobalPose();
        NiTransform kParent;
        NiPhysXTypes::NxMat34ToNiTransform(kPose, kParent.m_Rotate, 
            kParent.m_Translate);
        kParent.m_fScale = 1.0f / fScalePToW;

        // Get the target's local transform
        NiTransform kTargetLocal;
        kTargetLocal.m_Rotate = m_spTarget->GetRotate();
        kTargetLocal.m_Translate = m_spTarget->GetTranslate();
        kTargetLocal.m_fScale = m_spTarget->GetScale();

        // We have all we need
        (kParent * kTargetLocal).Invert(kXform);
    }
    else
    {
        // X(Geom<-PhysX) = X(Geom<-World) X(World<-PhysX)
        //                = X(World<-Geom)^-1 X(World<-PhysX)
        
        // Get the target's world transform: X(Geom<-World)
        NiTransform kTargetWorld;
        kTargetWorld.m_Rotate = m_spTarget->GetWorldRotate();
        kTargetWorld.m_Translate = m_spTarget->GetWorldTranslate();
        kTargetWorld.m_fScale = m_spTarget->GetWorldScale();
        NiTransform kTargetInv;
        kTargetWorld.Invert(kTargetInv);
        
        // Get a transform to take PhysX to world
        NiTransform kPhysXWorld = kRootTransform;
        kPhysXWorld.m_fScale *= fScalePToW;

        kXform = kTargetInv * kPhysXWorld;
    }

    unsigned short usChangedFlags = 0;
    NiTriShapeDynamicData* pkGeomData =
        (NiTriShapeDynamicData*)m_spTarget->GetModelData();

    // 2. Process any tears. This means updating the vertex counts, copying
    //    texture coordinates, colors and updating triangle index information.
    //    We make a major assumption here, enforced by the code that
    //    reallocates the Gamebryo mesh data to match the PhysX data:
    //    - There is a 1-1 correspondence between triangles, including
    //      vertex order.
    unsigned int uiNumNxVerts = m_kClothData.numVerticesPtr[0];
    if (uiNumNxVerts > m_uiPreviousVertexCount)
    {
        if (m_uiPreviousVertexCount == pkGeomData->GetActiveVertexCount())
        {
            // 1-1 mapping between GB vertices and Px vertices
            ProcessBasicTearing(pkGeomData, usChangedFlags);
        }
        else
        {
            // Many-1 mapping between GB vertices and Px vertices
            ProcessGeneralTearing(pkGeomData, usChangedFlags);
        }
    }
    else if (uiNumNxVerts < m_uiPreviousVertexCount)
    {
        // Typically, this means we have not taken a simulation step
        // to get any mesh data.
        return;
    }

    // 3. Transform all of the vertices from the NxMeshData into the
    //    NiGeometryData.
    NiPoint3* pkGeomVerts = pkGeomData->GetVertices();
    NxVec3* pkNxVerts = (NxVec3*)m_kClothData.verticesPosBegin;
    unsigned short usNumActive = pkGeomData->GetActiveVertexCount();
    for (unsigned short us = 0; us < usNumActive; us++)
    {
        NiPoint3 kV;
        NiPhysXTypes::NxVec3ToNiPoint3(pkNxVerts[m_pusVertexMap[us]], kV);
        pkGeomVerts[us] =
            (kXform.m_Rotate * (kXform.m_fScale * kV)) + kXform.m_Translate;
    }
    usChangedFlags |= NiGeometryData::VERTEX_MASK;
    
    // 4. If necessary, compute normals and NBT frames for the vertices.
    if (pkGeomData->GetNormals())
    {
        if (pkGeomData->GetNormalBinormalTangentMethod() ==
            NiGeometryData::NBT_METHOD_NONE)
        {            
            RecomputeNormals(pkGeomData);
        }
        else
        {
            RecomputeNBTs(pkGeomData);
        }
        usChangedFlags |= NiGeometryData::NORMAL_MASK;
    }

    // 5. Update the bound and mark the NiGeometryData as changed.
    pkGeomData->GetBound().ComputeFromData(usNumActive, pkGeomVerts);
    pkGeomData->MarkAsChanged(usChangedFlags);

    m_uiPreviousVertexCount = uiNumNxVerts;
}
//---------------------------------------------------------------------------
void NiPhysXClothDest::UpdateSceneGraph(const float fT,
    const NiTransform& kRootTransform, const float fScalePToW,
    const bool bForce)
{
    // Deliberately empty
}
//---------------------------------------------------------------------------
void NiPhysXClothDest::ProcessBasicTearing(NiTriShapeDynamicData* pkGeomData,
    unsigned short& usChangedFlags)
{
    unsigned short usNumNiVerts = pkGeomData->GetVertexCount();
    unsigned int uiNumNxVerts = m_kClothData.numVerticesPtr[0];
    unsigned short* pusParentIndex =
        (unsigned short*)m_kClothData.parentIndicesBegin;

    // Copy colors for new vertices
    NiColorA* pkColors = pkGeomData->GetColors();
    if (pkColors)
    {
        for (unsigned int ui = m_uiPreviousVertexCount; ui < uiNumNxVerts;
            ui++)
        {
            pkColors[ui] = pkColors[pusParentIndex[ui]];
        }
        usChangedFlags |= NiGeometryData::COLOR_MASK;
    }
    
    // Copy textures for new vertices
    NiPoint2* pkTextures = pkGeomData->GetTextures();
    if (pkTextures)
    {
        unsigned short usNumSets = pkGeomData->GetTextureSets();
        for (unsigned short us = 0; us < usNumSets; us++)
        {
            unsigned short usOffset = usNumNiVerts * us;
            for (unsigned int ui = m_uiPreviousVertexCount;
                ui < uiNumNxVerts; ui++)
            {
                pkTextures[ui + usOffset] =
                    pkTextures[pusParentIndex[ui] + usOffset];
            }
        }
        usChangedFlags |= NiGeometryData::TEXTURE_MASK;
    }
    
    // Add to end of vertex map
    for (unsigned int ui = m_uiPreviousVertexCount; ui < uiNumNxVerts; ui++)
    {
        m_pusVertexMap[ui] = ui;
    }
    
    // Update triangles
    unsigned short usNumIndices = pkGeomData->GetTriangleCount() * 3;
    unsigned short* pusNxIndices = (unsigned short*)m_kClothData.indicesBegin;
    unsigned short* pusNiIndices = pkGeomData->GetTriList();
    NIASSERT(usNumIndices == m_kClothData.numIndicesPtr[0]);
    for (unsigned short us = 0; us < usNumIndices; us++)
    {
        if (pusNxIndices[us] >= m_uiPreviousVertexCount)
        {
            m_kSplitParentMap.SetAt(pusNxIndices[us], pusNiIndices[us]);
            pusNiIndices[us] = pusNxIndices[us];
        }
    }
    
    usChangedFlags |= NiTriShapeData::TRIANGLE_INDEX_MASK;

    pkGeomData->SetActiveVertexCount(uiNumNxVerts);
}
//---------------------------------------------------------------------------
void NiPhysXClothDest::ProcessGeneralTearing(NiTriShapeDynamicData* pkGeomData,
    unsigned short& usChangedFlags)
{
    // In this instance, many Gamebryo vertices may map to the same PhysX
    // vertex, and many trianges may share the same Gamebryo vertex. The
    // most obvious case is a cube with multiple copies of each corner vertex
    // so that different normals can be specified. Other cases include
    // texture seams.
    
    // As a result, the processing is very different from the simple case.
    // For every reference to a newly created PhysX vertex, we need to decide
    // what to do with the corresponding Gamebryo vertex. Here, we simply
    // duplicate the old vertex data and create a new Gamebryo vertex to
    // match the new PhysX vertex. This may create extra vertices - the
    // required vertex may already exist in Gamebryo. However, we have no
    // easy way to know this, short of identifying all the vertices that
    // may need duplication, then making a second pass to see if they actually
    // need it. Instead we trade memory for speed.

    unsigned short usNumNiVerts = pkGeomData->GetActiveVertexCount();
    unsigned int uiNumNxVerts = m_kClothData.numVerticesPtr[0];

    // Iterate over all triangles, building up information about how torn
    // vertices are used.
    unsigned short usNumIndices = pkGeomData->GetTriangleCount() * 3;
    unsigned short* pusNxIndices = (unsigned short*)m_kClothData.indicesBegin;
    unsigned short* pusNiIndices = pkGeomData->GetTriList();
    NIASSERT(usNumIndices == m_kClothData.numIndicesPtr[0]);
    
    // Bad to use maps at runtime, but there is no better solution.
    // This code should be rarely executed (how often does topology really
    // change, and how many vertices are really affected?)

    // Map of vertices that have been replicated. The key is a combination
    // of the previous Gamebryo index for the vertex and the new PhysX
    // index. The value is the new Gamebryo index for the replicated
    // vertex. This handles a single PhysX old vertex being split into
    // more than two new vertices.
    NiTMap<unsigned int, unsigned int> kReplicated(7);
    for (unsigned short us = 0; us < usNumIndices; us++)
    {
        unsigned int uiNxIndex = pusNxIndices[us];
        if (uiNxIndex >= m_uiPreviousVertexCount)
        {
            // Have a vertex that refers to a new vertex. Get the
            // Gamebryo vertex index.
            unsigned int uiGBIndex = pusNiIndices[us];
            
            // Check if this vertex is already replicated
            unsigned int uiMapKey = (uiNxIndex << 16) | uiGBIndex;
            unsigned int uiNewGBIndex = 0;
            if (kReplicated.GetAt(uiMapKey, uiNewGBIndex))
            {
                // We have already duplicated this Gamebryo vertex for
                // the corresponding PhysX vertex. So go ahead and simply
                // change the vertex ID in the array and we are done.
                NIASSERT(uiNxIndex == m_pusVertexMap[uiNewGBIndex]);
                pusNiIndices[us] = uiNewGBIndex;
                continue; // Breaks us out to the next triangle index
            }

            // Here if the vertex has not already been replicated.
            // At this point we must replicate. Sometimes we will replicate
            // when we don't have to (Gamebryo already has the vertex split
            // or some other reason, such as a texture seam) but we need
            // reliable parent indices from PhysX to do it, but the parent
            // is always the very original vertex, not the one from which
            // a new vertex was split.

            // If you hit this assert, there isn't enough space
            // for the replicated vertex, so you need to allocate
            // a bigger growth factor.
            NIASSERT(usNumNiVerts < pkGeomData->GetVertexCount() - 1);
            uiNewGBIndex = usNumNiVerts++;
            
            // Copy all data over and set maps and indexes
            CopyVertexData(uiGBIndex, uiNewGBIndex, pkGeomData,
                usChangedFlags);
            m_pusVertexMap[uiNewGBIndex] = uiNxIndex;
            pusNiIndices[us] = uiNewGBIndex;
            kReplicated.SetAt(uiMapKey, uiNewGBIndex);
            m_kSplitParentMap.SetAt(uiNewGBIndex, uiGBIndex);
        }
    }

    usChangedFlags |= NiTriShapeData::TRIANGLE_INDEX_MASK;

    pkGeomData->SetActiveVertexCount(usNumNiVerts);
}
//---------------------------------------------------------------------------
void NiPhysXClothDest::CopyVertexData(unsigned int uiSrc,
    unsigned int uiDest, NiTriShapeDynamicData* pkGeomData,
    unsigned short& usChangedFlags)
{
    // Copy colors for new vertices
    NiColorA* pkColors = pkGeomData->GetColors();
    if (pkColors)
    {
        pkColors[uiDest] = pkColors[uiSrc];
        usChangedFlags |= NiGeometryData::COLOR_MASK;
    }
    
    // Copy textures for new vertices
    NiPoint2* pkTextures = pkGeomData->GetTextures();
    unsigned short usNumNiVerts = pkGeomData->GetVertexCount();
    if (pkTextures)
    {
        unsigned short usNumSets = pkGeomData->GetTextureSets();
        for (unsigned short us = 0; us < usNumSets; us++)
        {
            unsigned short usOffset = usNumNiVerts * us;
            pkTextures[uiDest + usOffset] = pkTextures[uiSrc + usOffset];
        }
        usChangedFlags |= NiGeometryData::TEXTURE_MASK;
    }
}
//---------------------------------------------------------------------------
void NiPhysXClothDest::RecomputeNormals(NiTriShapeDynamicData* pkGeomData)
{
    unsigned int uiNumActive = pkGeomData->GetActiveVertexCount();
    unsigned int uiNumTriangles = pkGeomData->GetTriangleCount();

    NiPoint3* pkVertices = pkGeomData->GetVertices();
    NiPoint3* pkNormals = pkGeomData->GetNormals();
    unsigned short* pusIndices = pkGeomData->GetTriList();

    // Clear out normals
    memset(pkNormals, 0, sizeof(NiPoint3) * uiNumActive);
       
    // This code has been refactored somewhat from the art tool version to
    // reduce redundant computation, adat copying and function calls.
    for (unsigned int uiCalc = 0; uiCalc < uiNumTriangles; uiCalc++)
    {
        unsigned int uiIndex = uiCalc * 3;

        // First compute normal, doing edge vectors in the process
        NiPoint3 kV0 = pkVertices[pusIndices[uiIndex]];
        NiPoint3 kV1 = pkVertices[pusIndices[uiIndex + 1]];
        NiPoint3 kV2 = pkVertices[pusIndices[uiIndex + 2]];

        // The edge vectors in cartesian coordinates
        NiPoint3 kEC0 = kV1 - kV0;
        NiPoint3 kEC1 = kV2 - kV0;
        
        // Take the cross product to get the face normal
        NiPoint3 kNorm = kEC0.Cross(kEC1);
        if (kNorm.Unitize() > 0.0f)
        {
            // Push the face normal to the vertices
            pkNormals[pusIndices[uiIndex]] += kNorm;
            pkNormals[pusIndices[uiIndex + 1]] += kNorm;
            pkNormals[pusIndices[uiIndex + 2]] += kNorm;
        }
    }

    NiPoint3::UnitizeVectors(pkNormals, uiNumActive, sizeof(NiPoint3));
}
//---------------------------------------------------------------------------
void NiPhysXClothDest::RecomputeNBTs(NiTriShapeDynamicData* pkGeomData)
{
    // This code is cderived from that found in the art plug-ins. Changes
    // to one or the other should be mirrored in all locations.
    unsigned int uiNumVertices = pkGeomData->GetVertexCount();
    unsigned int uiNumActive = pkGeomData->GetActiveVertexCount();
    unsigned int uiNumTriangles = pkGeomData->GetTriangleCount();

    NiPoint3* pkVertices = pkGeomData->GetVertices();
    NiPoint3* pkNormals = pkGeomData->GetNormals();

    NIASSERT(m_usNBTSet < pkGeomData->GetTextureSets());
    NiPoint2* pkTextures = pkGeomData->GetTextureSet(m_usNBTSet);
    NIASSERT(pkTextures);

    unsigned short* pusIndices = pkGeomData->GetTriList();

    // Clear out NBT info
    memset(pkNormals, 0, sizeof(NiPoint3) * 3 * uiNumVertices);

    // This code has been refactored somewhat from the art tool version to
    // reduce redundant computation, adat copying and function calls.
    for (unsigned int uiCalc = 0; uiCalc < uiNumTriangles; uiCalc++)
    {
        unsigned int uiIndex = uiCalc * 3;
        unsigned short usCurrentTriangle = uiIndex;

        // First compute normal, doing edge vectors in the process
        NiPoint3 kV0 = pkVertices[pusIndices[uiIndex]];
        NiPoint3 kV1 = pkVertices[pusIndices[uiIndex + 1]];
        NiPoint3 kV2 = pkVertices[pusIndices[uiIndex + 2]];

        // The edge vectors in cartesian coordinates
        NiPoint3 kEC0 = kV1 - kV0;
        NiPoint3 kEC1 = kV2 - kV0;
        
        // Take the cross product to get the face normal
        NiPoint3 kNorm = kEC0.Cross(kEC1);
        if (kNorm.Unitize() > 0.0f)
        {
            // Push the face normal to the vertices
            pkNormals[pusIndices[uiIndex]] += kNorm;
            pkNormals[pusIndices[uiIndex + 1]] += kNorm;
            pkNormals[pusIndices[uiIndex + 2]] += kNorm;
        }

        // Do the first NBT for the first set of textures
        NiPoint2 kT0 = pkTextures[pusIndices[uiIndex]];
        NiPoint2 kT1 = pkTextures[pusIndices[uiIndex + 1]];
        NiPoint2 kT2 = pkTextures[pusIndices[uiIndex + 2]];  

        unsigned int uiTriIndex = pusIndices[usCurrentTriangle];
        NiPoint2 kET0 = kT1 - kT0;
        NiPoint2 kET1 = kT2 - kT0;
        FindNBT(pkNormals, uiTriIndex, uiNumVertices, kEC0, kEC1, kET0, kET1);
        
        uiTriIndex = pusIndices[usCurrentTriangle + 1];
        kEC0 = kV0 - kV1;
        kEC1 = kV2 - kV1;
        kET0 = kT0 - kT1;
        kET1 = kT2 - kT1;
        FindNBT(pkNormals, uiTriIndex, uiNumVertices, kEC0, kEC1, kET0, kET1);

        uiTriIndex = pusIndices[usCurrentTriangle + 2];
        kEC0 = kV1 - kV2;
        kEC1 = kV0 - kV2;
        kET0 = kT1 - kT2;
        kET1 = kT0 - kT2;
        FindNBT(pkNormals, uiTriIndex, uiNumVertices, kEC0, kEC1, kET0, kET1);
    }

    NiPoint3::UnitizeVectors(pkNormals, uiNumActive, sizeof(NiPoint3));
    NiPoint3::UnitizeVectors(pkNormals + uiNumVertices, uiNumActive,
        sizeof(NiPoint3));
    NiPoint3::UnitizeVectors(pkNormals + 2 * uiNumVertices, uiNumActive,
        sizeof(NiPoint3));
}
//----------------------------------------------------------------------------
void NiPhysXClothDest::FindNBT(NiPoint3* pkNBTs, unsigned int uiNBTIndex, 
    unsigned int uiNumVertices, NiPoint3& kEC0, NiPoint3& kEC1, 
    NiPoint2& kET0, NiPoint2& kET1)
{
    NiPoint3 kBinormal, kTangent;

    // If this value is 0, then we can't calculate blending values.
    // We'll use the other edge.  If that has a zero value, then the
    // triangle is parametrically degenerate.
    if (kET0.x == 0)
    {
        NiPoint3 kSwapPt3 = kEC1;
        kEC1 = kEC0;
        kEC0 = kSwapPt3;

        NiPoint2 kSwapPt2 = kET1;
        kET1 = kET0;
        kET0 = kSwapPt2;
    }

    if (kET0.x == 0)
    {
        return;
    }

    // The weighting factors for the equation:
    // Binormal = a * kEC0 + b * kEC1.  b = kET0.x/m.  
    // Thus calculate fMTerm and check for a 0 value.
    float fBTerm, fATerm, fMTerm; 

    fMTerm = ((kET1.x * kET0.y) - (kET0.x * kET1.y));
    if (fMTerm == 0)
    {
        //pkNBTs[uiNBTIndex] += kNorm0;
        return;
    }

    // At this point, it's safe to calculate the full values for a, b, and T
    fBTerm = kET0.y / fMTerm;
    fATerm = (1 - (fBTerm * kET1.x))/(kET0.x);

    kTangent = (fATerm * kEC0) + (fBTerm * kEC1);
    kTangent.Unitize();

    // We have to repeat with the binormal vector now which will have
    // different blending values.
    fMTerm = ((kET0.x * kET1.y) - (kET1.x * kET0.y));
    if (fMTerm == 0)
        return;

    fBTerm = kET0.x / fMTerm;
    fATerm = (-1.0f * (fBTerm * kET1.x))/(kET0.x);

    kBinormal = (fATerm * kEC0) + (fBTerm * kEC1);
    kBinormal.Unitize();

    pkNBTs[uiNumVertices + uiNBTIndex] += kBinormal;
    pkNBTs[(2 * uiNumVertices) + uiNBTIndex] += kTangent;

    return;
}
//----------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions done rarely, at setup or destruction.
//---------------------------------------------------------------------------
void NiPhysXClothDest::RestoreTriangleIndices()
{
    if (!m_spTarget)
        return;
        
    NiTriShapeDataPtr spTriData = (NiTriShapeData*)m_spTarget->GetModelData();
    NIASSERT(spTriData != 0);
    
    unsigned short usNumIndices = spTriData->GetTriangleCount() * 3;
    unsigned short* pusIndices = spTriData->GetTriList();
    if (!pusIndices)
    {
        // possible if this mesh had its indices removed after loading.
        NiOutputDebugString("Missing cloth geometry. Did you set pre-cache "
            "options on all cloth geometry to volatile?\n");
        return;
    }
    for (unsigned short us = 0; us < usNumIndices; us++)
    {
        unsigned short usChild = pusIndices[us];
        unsigned short usParent;
        while (m_kSplitParentMap.GetAt(usChild, usParent))
        {
            usChild = usParent;
        }
        pusIndices[us] = usChild;
    }
}
//---------------------------------------------------------------------------
void NiPhysXClothDest::AllocateClothData(const unsigned short* pusVertexMap)
{
    DeallocateClothData();

    // There is the size of the PhysX mesh, without redundant vertices, and
    // the size of the Gamebryo mesh, with redudant vertices (possibly).
    
    // Deal with the PhysX side of things
    NxClothMesh* pkNxMesh = m_pkCloth->getClothMesh();
    NxClothMeshDesc kNxMeshDesc;
    pkNxMesh->saveToDesc(kNxMeshDesc);
    unsigned short usNumNxVerts = (unsigned short)
        (kNxMeshDesc.numVertices * m_fMeshGrowFactor);
    unsigned short usNumNxTris = kNxMeshDesc.numTriangles;
    
    m_kClothData.maxVertices = usNumNxVerts;

    m_kClothData.verticesPosByteStride = sizeof(NxVec3);
    m_kClothData.verticesPosBegin = NiAlloc(NxVec3, usNumNxVerts);
    m_kClothData.numVerticesPtr = NiAlloc(NxU32, 1);
    m_kClothData.numVerticesPtr[0] = 0;

    m_kClothData.verticesNormalByteStride = 0;
    m_kClothData.verticesNormalBegin = 0;
 
    m_kClothData.maxIndices = usNumNxTris * 3;
    m_kClothData.indicesByteStride = sizeof(NxU16);
    m_kClothData.indicesBegin = NiAlloc(NxU16, m_kClothData.maxIndices);
    m_kClothData.numIndicesPtr = NiAlloc(NxU32, 1);
    m_kClothData.numIndicesPtr[0] = 0;

    m_kClothData.maxParentIndices = usNumNxVerts;
    m_kClothData.parentIndicesByteStride = sizeof(NxU16);
    m_kClothData.parentIndicesBegin = NiAlloc(NxU16, usNumNxVerts);
    m_kClothData.numParentIndicesPtr = NiAlloc(NxU32, 1);
 
    m_kClothData.flags = NX_MDF_16_BIT_INDICES;
    
    m_uiPreviousVertexCount = kNxMeshDesc.numVertices;
    
    //
    // Deal with the Gamebryo side
    //
    NiTriShapeDataPtr spTriData = (NiTriShapeData*)m_spTarget->GetModelData();
    unsigned short usNumNiVerts = (unsigned short)
        (m_usOriginalVertexCount * m_fMeshGrowFactor);
        
    // Set up the vertex map
    NIASSERT(pusVertexMap);
    m_pusVertexMap = NiAlloc(unsigned short, usNumNiVerts);
    for (unsigned int ui = 0; ui < m_usOriginalVertexCount; ui++)
    {
        m_pusVertexMap[ui] = pusVertexMap[ui];
    }

    // Replace the geometry data with dynamic geometry data sized to handle
    // growth in geometry due to tearing. Copy all per-vertex data over
    // directly, but set it up so that the Gamebryo triangle data matches
    // the NxClothMesh triangle data to make it faster to process tearing.
    NiPoint3* pkVertices = NiNew NiPoint3[usNumNiVerts];
    NiMemcpy(pkVertices, m_usOriginalVertexCount * sizeof(NiPoint3),
        spTriData->GetVertices(), m_usOriginalVertexCount * sizeof(NiPoint3));

    NiPoint3* pkNormals = 0;
    if (spTriData->GetNormals())
    {
        if (spTriData->GetNormalBinormalTangentMethod() ==
            NiGeometryData::NBT_METHOD_NONE)
        {
            pkNormals = NiNew NiPoint3[usNumNiVerts];
            NiMemcpy(pkNormals,
                m_usOriginalVertexCount * sizeof(NiPoint3),
                spTriData->GetNormals(),
                m_usOriginalVertexCount * sizeof(NiPoint3));
        }
        else
        {
            NIASSERT(spTriData->GetNormalBinormalTangentMethod() ==
                NiGeometryData::NBT_METHOD_NDL
                && "Must use NDL-style NBTs with cloth geometry\n");
            pkNormals = NiNew NiPoint3[usNumNiVerts * 3];
            NiMemcpy(pkNormals,
                m_usOriginalVertexCount * sizeof(NiPoint3),
                spTriData->GetNormals(),
                m_usOriginalVertexCount * sizeof(NiPoint3));
            NiMemcpy(pkNormals + usNumNiVerts,
                m_usOriginalVertexCount * sizeof(NiPoint3),
                spTriData->GetNormals() + usNumNiVerts,
                m_usOriginalVertexCount * sizeof(NiPoint3));
            NiMemcpy(pkNormals + 2 * usNumNiVerts,
                m_usOriginalVertexCount * sizeof(NiPoint3),
                spTriData->GetNormals() + 2 * usNumNiVerts,
                m_usOriginalVertexCount * sizeof(NiPoint3));
        }
    }

    NiColorA* pkColors = 0;
    if (spTriData->GetColors())
    {
        pkColors = NiNew NiColorA[usNumNiVerts];
        NiMemcpy(pkColors, m_usOriginalVertexCount * sizeof(NiColorA),
            spTriData->GetColors(),
            m_usOriginalVertexCount * sizeof(NiColorA));
    }
    
    NiPoint2* pkTextures = 0;
    unsigned short usNumSets = spTriData->GetTextureSets();
    if (spTriData->GetTextures())
    {
        pkTextures = NiNew NiPoint2[usNumNiVerts * usNumSets];
        for (unsigned short us = 0; us < usNumSets; us++)
        {
            NiMemcpy(pkTextures + us * usNumNiVerts,
                m_usOriginalVertexCount * sizeof(NiPoint2),
                spTriData->GetTextures() + us * usNumNiVerts,
                m_usOriginalVertexCount * sizeof(NiPoint2));
        }
    }

    // Copy triangle data.
    unsigned short usNumNiTris = spTriData->GetTriangleCount();
    NIASSERT(usNumNiTris == kNxMeshDesc.numTriangles);
    unsigned short* pusNewTris = NiAlloc(unsigned short, usNumNiTris * 3);
    NiMemcpy(pusNewTris, usNumNiTris * 3 * sizeof(unsigned short),
            spTriData->GetTriList(),
            usNumNiTris * 3 * sizeof(unsigned short));

    // Allocate the new data
    NiTriShapeDynamicData* pkNewData = NiNew NiTriShapeDynamicData(
        usNumNiVerts, pkVertices, pkNormals, pkColors, pkTextures, usNumSets,
        spTriData->GetNormalBinormalTangentMethod(),
        usNumNiTris, pusNewTris, m_usOriginalVertexCount, usNumNiTris);

    pkNewData->SetConsistency(NiGeometryData::VOLATILE);

    m_spTarget->SetModelData(pkNewData);
    
    m_kSplitParentMap.RemoveAll();
    
    spTriData = 0; // Will delete unused geometry data
}
//---------------------------------------------------------------------------
void NiPhysXClothDest::DeallocateClothData()
{
    NiFree(m_kClothData.verticesPosBegin);
    NiFree(m_kClothData.numVerticesPtr);
    NiFree(m_kClothData.indicesBegin);
    NiFree(m_kClothData.numIndicesPtr);
    NiFree(m_kClothData.parentIndicesBegin);
    NiFree(m_kClothData.numParentIndicesPtr);
    NiFree(m_pusVertexMap);
    
    m_kClothData.verticesPosBegin = 0;
    m_kClothData.numVerticesPtr = 0;
    m_kClothData.indicesBegin = 0;
    m_kClothData.numIndicesPtr = 0;
    m_kClothData.parentIndicesBegin = 0;
    m_kClothData.numParentIndicesPtr = 0;
    m_kClothData.verticesNormalBegin = 0;
    m_pusVertexMap = 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXClothDest);
//---------------------------------------------------------------------------
void NiPhysXClothDest::CopyMembers(NiPhysXClothDest* pkDest,
    NiCloningProcess& kCloning)
{
    NiPhysXDest::CopyMembers(pkDest, kCloning);
    
    // Target must have been cloned first, or this fails. Standard usage for
    // PhysX cloning is to clone the scene graph first, then then physics
    // with the same cloning process.
    NiObject* pkClonedTargetObj = 0;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(m_spTarget, pkClonedTargetObj));
    pkDest->m_spTarget = (NiTriShape*)pkClonedTargetObj;

    pkDest->m_pkCloth = 0;

    pkDest->m_pusVertexMap = 0;

    pkDest->m_pkParentActor = 0;

    pkDest->m_fMeshGrowFactor = m_fMeshGrowFactor;
    pkDest->m_bDoSleepXforms = m_bDoSleepXforms;
    pkDest->m_usOriginalVertexCount = m_usOriginalVertexCount;
    pkDest->m_usNBTSet = m_usNBTSet;

    pkDest->m_kWorldXform.MakeIdentity();
    pkDest->m_bSleeping = false;
    pkDest->m_uiPreviousVertexCount = 0;
}
//---------------------------------------------------------------------------
void NiPhysXClothDest::ProcessClone(NiCloningProcess& kCloning)
{
    NiObject::ProcessClone(kCloning);
}
//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXClothDest::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXDest::RegisterStreamables(kStream))
        return false;

    if (m_spTarget)
        m_spTarget->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXClothDest::SaveBinary(NiStream& kStream)
{
    NiPhysXDest::SaveBinary(kStream);
    
    kStream.SaveLinkID(m_spTarget);

    NiStreamSaveBinary(kStream, m_fMeshGrowFactor);

    NiBool bBool(m_bDoSleepXforms);
    NiStreamSaveBinary(kStream, bBool);
    
    NiStreamSaveBinary(kStream, m_usNBTSet);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXClothDest);
//---------------------------------------------------------------------------
void NiPhysXClothDest::LoadBinary(NiStream& kStream)
{
    NiPhysXDest::LoadBinary(kStream);

    m_spTarget = (NiTriShape*)kStream.ResolveLinkID();
    
    NiStreamLoadBinary(kStream, m_fMeshGrowFactor);
    
    NiBool bBool;
    NiStreamLoadBinary(kStream, bBool);
    m_bDoSleepXforms = bBool ? true : false;
    
    NiStreamLoadBinary(kStream, m_usNBTSet);
}
//---------------------------------------------------------------------------
void NiPhysXClothDest::LinkObject(NiStream& kStream)
{
    NiPhysXDest::LinkObject(kStream);

    NIASSERT(m_spTarget);
    NiTriShapeData* pkTriData = (NiTriShapeData*)m_spTarget->GetModelData();
    m_usOriginalVertexCount = pkTriData->GetVertexCount();
}
//---------------------------------------------------------------------------
bool NiPhysXClothDest::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXClothDest, pkObject));
    if(!NiPhysXDest::IsEqual(pkObject))
        return false;
        
    NiPhysXClothDest* pkThatObject = (NiPhysXClothDest*)pkObject;
    
    if (m_spTarget)
    {
        if (pkThatObject->m_spTarget)
        {
            if (!m_spTarget->IsEqual(pkThatObject->m_spTarget))
                return false;
        }
        else
        {
            return false;
        }
    }
    else if (pkThatObject->m_spTarget)
    {
        return false;
    }
    
    if (m_fMeshGrowFactor != pkThatObject->m_fMeshGrowFactor ||
        m_bDoSleepXforms != pkThatObject->m_bDoSleepXforms ||
        m_usNBTSet != pkThatObject->m_usNBTSet)
        return false;

    return true;
}
//---------------------------------------------------------------------------
