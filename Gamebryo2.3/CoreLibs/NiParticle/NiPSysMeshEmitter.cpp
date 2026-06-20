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

#include "NiPSysMeshEmitter.h"
#include "NiParticleSystem.h"
#include <NiCloningProcess.h>
#include <NiLines.h>
#include <NiTriBasedGeom.h>

NiImplementRTTI(NiPSysMeshEmitter, NiPSysEmitter);

#define MAX_DEGENERATE_TRIANGLES 6

//---------------------------------------------------------------------------
NiPSysMeshEmitter::NiPSysMeshEmitter(const char* pcName, 
    NiGeometry* pkGeomEmitter, MeshEmissionType eMeshEmissionType, 
    InitialVelocityType eInitialVelocityType, NiPoint3 kEmitAxis,
    float fSpeed, float fSpeedVar, float fDeclination, float fDeclinationVar,
    float fPlanarAngle, float fPlanarAngleVar, NiColorA kInitialColor, 
    float fInitialRadius, float fLifeSpan, float fLifeSpanVar, 
    float fRadiusVar) : 
    NiPSysEmitter(pcName, ORDER_EMITTER, fSpeed, fSpeedVar, fDeclination,
    fDeclinationVar, fPlanarAngle, fPlanarAngleVar, kInitialColor,
    fInitialRadius, fLifeSpan, fLifeSpanVar, fRadiusVar), 
    m_kGeomEmitterArray(1, 2), m_eInitVelocityType(eInitialVelocityType),
    m_eEmissionType(eMeshEmissionType), m_kEmitAxis(kEmitAxis)
{
    if (pkGeomEmitter)
        m_kGeomEmitterArray.Add(pkGeomEmitter);
}
//---------------------------------------------------------------------------
NiPSysMeshEmitter::NiPSysMeshEmitter() : m_kGeomEmitterArray(1, 2), 
    m_eInitVelocityType(NI_VELOCITY_USE_NORMALS), 
    m_eEmissionType(NI_EMIT_FROM_VERTICES), m_kEmitAxis(NiPoint3::UNIT_X)
{
}
//---------------------------------------------------------------------------
void NiPSysMeshEmitter::ComputeInitialPositionAndVelocity(
    NiPoint3& kPosition, NiPoint3& kVelocity)
{
    unsigned int uiGeometryEmitterCount = GetGeometryEmitterCount();
    if (uiGeometryEmitterCount == 0)
        return;

    // Randomly select a mesh from the geometry emitter array
    unsigned int uiWhichEmitter = (int) (NiUnitRandom() * (float) 
        (uiGeometryEmitterCount));
    if (uiWhichEmitter == uiGeometryEmitterCount)
        uiWhichEmitter = uiGeometryEmitterCount - 1;

    NiGeometry* pkGeomEmitter = GetGeometryEmitter(uiWhichEmitter);

    if (!pkGeomEmitter)
    {
        return;
    }

    // We have separate paths to take if we are skinned, so check to see 
    // if we have an NiSkinInstance. We absolutely must have a partitioned
    // mesh. If necessary, we will partition the mesh for you.
    NiSkinInstance* pkSkin = pkGeomEmitter->GetSkinInstance();
    NiSkinnedEmitterData* pkEmitData = GetSkinnedEmitterData(uiWhichEmitter);
    if (pkSkin && pkEmitData && pkEmitData->m_spSkinPartition == NULL)
        pkEmitData->CreatePartition(pkGeomEmitter);

    switch (m_eEmissionType)
    {
        default:
        // Emit from the vertex positions themselves
        case NI_EMIT_FROM_VERTICES:
            {
                bool bEmitted;
                if (pkSkin)
                {
                    bEmitted = EmitFromSkinnedVertex(pkEmitData,
                        pkGeomEmitter, kPosition, kVelocity);
                }
                else
                {
                    bEmitted = EmitFromVertex(pkGeomEmitter, kPosition, 
                        kVelocity);
                }
                NIASSERT(bEmitted);
                break;
            }
        // Emit from the faces of triangles
        case NI_EMIT_FROM_FACE_CENTER:
        case NI_EMIT_FROM_FACE_SURFACE:
            {
                bool bEmitted;
                if (pkSkin)
                {
                    bEmitted = EmitFromSkinnedFace(pkEmitData, pkGeomEmitter,
                        kPosition, kVelocity);
                }
                else
                {
                    bEmitted = EmitFromFace(pkGeomEmitter, kPosition, 
                        kVelocity);
                }
                NIASSERT(bEmitted);
                break;
            }
        // Emit from the edges of the triangles
        case NI_EMIT_FROM_EDGE_CENTER:
        case NI_EMIT_FROM_EDGE_SURFACE:
            {
                bool bEmitted;
                if (pkSkin)
                {
                    bEmitted = EmitFromSkinnedEdge(pkEmitData, pkGeomEmitter,
                        kPosition, kVelocity);
                }
                else
                {
                    bEmitted = EmitFromEdge(pkGeomEmitter, kPosition, 
                        kVelocity);
                }
                NIASSERT(bEmitted);
                break;
            }
    };

}
//---------------------------------------------------------------------------
bool NiPSysMeshEmitter::EmitFromVertex(NiGeometry* pkGeomEmitter, 
    NiPoint3& kPosition, NiPoint3& kVelocity)
{
    unsigned int uiWhichVertex = (int) (NiUnitRandom() * (float) 
        (pkGeomEmitter->GetVertexCount()-1));
    const NiPoint3* pkVertices =  pkGeomEmitter->GetVertices();
    const NiPoint3* pkNormals = pkGeomEmitter->GetNormals();

    if (pkVertices == NULL)
    { 
        NIASSERT(!"There are no vertices to emit from! Did you precache "
            "the geom?");
        return false;
    }

    // Compute random initial position in Mesh.
    kPosition = pkVertices[uiWhichVertex];
    NiPoint3 kNormal = pkNormals[uiWhichVertex];

    if (m_eInitVelocityType == NI_VELOCITY_USE_NORMALS && 
        pkGeomEmitter->GetNormals())
    {
        float fSpeed = kVelocity.Length();
        kVelocity = fSpeed * kNormal;
    }

    TransformIntoCoordinateSpace(pkGeomEmitter, kPosition, kVelocity);
    return true;
}
//---------------------------------------------------------------------------
bool NiPSysMeshEmitter::EmitFromFace(NiGeometry* pkGeomEmitter, 
    NiPoint3& kPosition, NiPoint3& kVelocity)
{
    unsigned short usIndex0;
    unsigned short usIndex1;
    unsigned short usIndex2;

    if (NiIsKindOf(NiTriBasedGeom, pkGeomEmitter))
    {
        // Select a random triangle from the mesh
        NiTriBasedGeom* pkTBGeom = (NiTriBasedGeom*) pkGeomEmitter;
        unsigned int uiTriCount = pkTBGeom->GetActiveTriangleCount();
        NIASSERT(uiTriCount > 0);
        unsigned int uiWhichTri = (int) (NiUnitRandom() * (float) 
           (uiTriCount));
        if (uiWhichTri == uiTriCount)
        {
            uiWhichTri = uiTriCount - 1;
        }

        // Scan for degenerate triangles
        int iDegenerateScan;
        for (iDegenerateScan = 0; iDegenerateScan < MAX_DEGENERATE_TRIANGLES; 
            iDegenerateScan++)
        {
            pkTBGeom->GetTriangleIndices(uiWhichTri, usIndex0,
                usIndex1, usIndex2);

            // Check for Degenerate
            if ((usIndex0 == usIndex1) ||
                (usIndex0 == usIndex2) ||
                (usIndex1 == usIndex2))
            {
                // Move to the next triangle and look for wraping
                if (++uiWhichTri >= uiTriCount)
                    uiWhichTri = 0;
            }
            else
            {
                // Break out of the loop
                break;
            }
        }

        // If this assertion is found we will be emitting from a degenerate
        // triangle.
        NIASSERT(iDegenerateScan < MAX_DEGENERATE_TRIANGLES);
    }
    else if (NiIsKindOf(NiLines, pkGeomEmitter))
    {
        // It makes no sense to emit from a face here, just emit
        // from an edge
        return EmitFromEdge(pkGeomEmitter, kPosition, kVelocity);        
    }
    else
    {
        // Nothing we can do here!
        NIASSERT(!"Unknown NiGeometry derived class!");
        return false;
    }

    const NiPoint3* pkVertices =  pkGeomEmitter->GetVertices();
    const NiPoint3* pkNormals = pkGeomEmitter->GetNormals();

    
    if (pkVertices == NULL)
    { 
        NIASSERT(!"There are no vertices to emit from! Did you precache the "
            "geom?");
        return false;
    }

    // Ascertain the center of the triangle by averaging all the vertices
    NiPoint3 kVertex0 = pkVertices[usIndex0];
    NiPoint3 kVertex1 = pkVertices[usIndex1];
    NiPoint3 kVertex2 = pkVertices[usIndex2];

    NiPoint3 kCenterpoint = (kVertex0 + kVertex1 + kVertex2)/3.0f;
    kPosition = kCenterpoint;


    // We can emit along the vertex normals
    if (m_eInitVelocityType == NI_VELOCITY_USE_NORMALS && pkNormals)
    {
        // Compute the average normal
        NiPoint3 kNormal0 = pkNormals[usIndex0];
        NiPoint3 kNormal1 = pkNormals[usIndex1];
        NiPoint3 kNormal2 = pkNormals[usIndex2];
        NiPoint3 kAvgNormal = (kNormal0 + kNormal1 + kNormal2)/3.0f;
        NiPoint3::UnitizeVector(kAvgNormal);
        float fSpeed = kVelocity.Length();
        kVelocity = fSpeed * kAvgNormal;
    }

    // We can emit from a random position on the surface of the
    // triangle
    if (m_eEmissionType == NI_EMIT_FROM_FACE_SURFACE)
    {
        NiPoint3 kDir1 = kVertex1 - kVertex0;
        NiPoint3 kDir2 = kVertex2 - kVertex0;

        float fSqrt = NiFastSqrt(NiUnitRandom());

        kPosition = kVertex0 + fSqrt * (NiUnitRandom() * kDir2 - kDir1) + 
            kDir1;
    }

    TransformIntoCoordinateSpace(pkGeomEmitter, kPosition, kVelocity);
    return true;
}
//---------------------------------------------------------------------------
bool NiPSysMeshEmitter::EmitFromEdge(NiGeometry* pkGeomEmitter, 
    NiPoint3& kPosition, NiPoint3& kVelocity)
{
    unsigned short usIndex0;
    unsigned short usIndex1;

    if (NiIsKindOf(NiTriBasedGeom, pkGeomEmitter))
    {
        // Since we don't keep track of edges, we're going to randomly
        // select a triangle and then randomly select an edge of that
        // triangle
        unsigned short usIndex2;
        NiTriBasedGeom* pkTBGeom = (NiTriBasedGeom*) pkGeomEmitter;
        unsigned int uiTriCount = pkTBGeom->GetActiveTriangleCount();
        NIASSERT(uiTriCount != 0);
        unsigned int uiWhichTri = (int) (NiUnitRandom() * (float)uiTriCount);
        if (uiWhichTri == uiTriCount)
        {
            uiWhichTri = uiTriCount - 1;
        }

        // Scan for degenerate triangles
        int iDegenerateScan;
        for (iDegenerateScan = 0; iDegenerateScan < MAX_DEGENERATE_TRIANGLES; 
            iDegenerateScan++)
        {
            pkTBGeom->GetTriangleIndices(uiWhichTri, usIndex0,
                usIndex1, usIndex2);

            // Check for Degenerate
            if ((usIndex0 == usIndex1) ||
                (usIndex0 == usIndex2) ||
                (usIndex1 == usIndex2))
            {
                // Move to the next triangle and look for wraping
                if (++uiWhichTri >= uiTriCount)
                    uiWhichTri = 0;
            }
            else
            {
                // Break out of the loop
                break;
            }
        }

        // If this assertion is found we will be emitting from a degenerate
        // triangle.
        NIASSERT(iDegenerateScan < MAX_DEGENERATE_TRIANGLES);

        switch(NiRand()%3)
        {
            default:
            case 0:
                break;
            case 1:
                usIndex0 = usIndex1;
                usIndex1 = usIndex2;
                break;
            case 2:
                usIndex1 = usIndex2;
                break;
        }

    }
    else if (NiIsKindOf(NiLines, pkGeomEmitter))
    {
        unsigned int uiWhichVertex = (int) (NiUnitRandom() * (float) 
            (pkGeomEmitter->GetVertexCount()-1));
        usIndex0 = uiWhichVertex;
        usIndex1 = uiWhichVertex + 1;
    }
    else
    {
        NIASSERT(!"Unknown NiGeometry derived class!");
        return false;
    }

    const NiPoint3* pkVertices =  pkGeomEmitter->GetVertices();
    const NiPoint3* pkNormals = pkGeomEmitter->GetNormals();

    
    if (pkVertices == NULL)
    { 
        NIASSERT(!"There are no vertices to emit from! Did you precache the "
            "geom?");
        return false;
    }

    // Compute the centerpoint of the edge
    NiPoint3 kVertex0 = pkVertices[usIndex0];
    NiPoint3 kVertex1 = pkVertices[usIndex1];

    NiPoint3 kCenterpoint = (kVertex0 + kVertex1)/2.0f;
    kPosition = kCenterpoint;


    if (m_eInitVelocityType == NI_VELOCITY_USE_NORMALS  && pkNormals)
    {
        // Compute the average normal
        NiPoint3 kNormal0 = pkNormals[usIndex0];
        NiPoint3 kNormal1 = pkNormals[usIndex1];
        NiPoint3 kAvgNormal = (kNormal0 + kNormal1)/2.0f;
        NiPoint3::UnitizeVector(kAvgNormal);
        float fSpeed = kVelocity.Length();
        kVelocity = fSpeed * kAvgNormal;
    }

    if (m_eEmissionType == NI_EMIT_FROM_EDGE_SURFACE)
    {
        // Emit from a random position on the line segment
        kPosition = kVertex0 + (kVertex1 - kVertex0) * NiUnitRandom();
    }

    TransformIntoCoordinateSpace(pkGeomEmitter, kPosition, kVelocity);
    return true;
}
//---------------------------------------------------------------------------
bool NiPSysMeshEmitter::EmitFromSkinnedVertex(
    NiSkinnedEmitterData* pkSkinEmitterData, NiGeometry* pkGeomEmitter,
    NiPoint3& kPosition, NiPoint3& kVelocity)
{
    // I'm not going to bother supporting skinned lines.
    if (!NiIsKindOf( NiTriBasedGeom, pkGeomEmitter))
        return false;

    // We absolutely have to have skin partitions 
    NIASSERT(pkSkinEmitterData->m_spSkinPartition);
    if (!pkSkinEmitterData->m_spSkinPartition)
        return false;

    unsigned short usPartCount = 
        pkSkinEmitterData->m_spSkinPartition->GetPartitionCount();

    // Select a random skin partition
    unsigned int uiWhichPartition = (int) (NiUnitRandom() * (float)
        (usPartCount));
    uiWhichPartition = NiMin((int) uiWhichPartition, usPartCount - 1);


    NiSkinPartition::Partition& kSubPartition = pkSkinEmitterData
        ->m_spSkinPartition->GetPartitions()[uiWhichPartition];
    
    NIASSERT(kSubPartition.m_usVertices > 0);
    unsigned int uiWhichVertex = (int) (NiUnitRandom() * (float) 
        (kSubPartition.m_usVertices-1));

    NiSkinInstance* pkSkin = pkGeomEmitter->GetSkinInstance();
    if (!pkSkin)
        return false;

    NiPoint3 kVertex;
    NiPoint3 kNormal;

    // We need to get the skinned mesh position, so we deform the vertex in
    // software
    DeformBySkin(kVertex, kNormal, (NiTriBasedGeom*) pkGeomEmitter, pkSkin,
        &kSubPartition, uiWhichVertex);

    kPosition = kVertex;
    if (m_eInitVelocityType == NI_VELOCITY_USE_NORMALS && 
        pkGeomEmitter->GetNormals())
    {
        float fSpeed = kVelocity.Length();
        kVelocity = fSpeed * kNormal;
    }
    
//    TransformIntoSkinnedCoordinateSpace(pkSkin, 
//        kPosition, kVelocity);

    TransformIntoCoordinateSpace(pkGeomEmitter, 
        kPosition, kVelocity);
   return true;
}
//---------------------------------------------------------------------------
bool NiPSysMeshEmitter::EmitFromSkinnedFace(
    NiSkinnedEmitterData* pkSkinEmitterData, NiGeometry* pkGeomEmitter,
    NiPoint3& kPosition, NiPoint3& kVelocity)
{
    // I'm not going to bother supporting skinned lines
    if (!NiIsKindOf( NiTriBasedGeom, pkGeomEmitter))
        return false;

    // We require skin partition
    NIASSERT(pkSkinEmitterData->m_spSkinPartition);
    if (!pkSkinEmitterData->m_spSkinPartition)
        return false;

    unsigned short usPartCount = 
        pkSkinEmitterData->m_spSkinPartition->GetPartitionCount();

    // Select a random skin partition
    unsigned int uiWhichPartition = (int) (NiUnitRandom() * (float)
        (usPartCount));
    uiWhichPartition = NiMin((int) uiWhichPartition, usPartCount - 1);

    NiSkinPartition::Partition& kSubPartition = pkSkinEmitterData
        ->m_spSkinPartition->GetPartitions()[uiWhichPartition];

    NIASSERT(kSubPartition.m_usVertices > 0);

    // We may not have a triangle in this partition, in this case, emit from
    // a vertex
    if (kSubPartition.m_usTriangles == 0)
    {
        return EmitFromSkinnedVertex(pkSkinEmitterData, pkGeomEmitter,
            kPosition, kVelocity);
    }

    // Randomly select a triangle
    unsigned int uiWhichTriangle = (int) (NiUnitRandom() * ((float) 
        kSubPartition.m_usTriangles));
    uiWhichTriangle = NiMin((int) uiWhichTriangle,
        kSubPartition.m_usTriangles - 1);

    NIASSERT(uiWhichTriangle < kSubPartition.m_usTriangles);

    NiSkinInstance* pkSkin = pkGeomEmitter->GetSkinInstance();
    if (!pkSkin)
        return false;

    unsigned int uiWhichVertex0;
    unsigned int uiWhichVertex1;
    unsigned int uiWhichVertex2;

    if (kSubPartition.m_usStrips != 0)
    {
       uiWhichVertex0 = kSubPartition.m_pusTriList[uiWhichTriangle];
       uiWhichVertex1 = kSubPartition.m_pusTriList[uiWhichTriangle + 1];
       uiWhichVertex2 = kSubPartition.m_pusTriList[uiWhichTriangle + 2];
    }
    else
    {
       uiWhichVertex0 = kSubPartition.m_pusTriList[uiWhichTriangle*3];
       uiWhichVertex1 = kSubPartition.m_pusTriList[uiWhichTriangle*3 + 1];
       uiWhichVertex2 = kSubPartition.m_pusTriList[uiWhichTriangle*3 + 2];
    }

    // We must now deform the vertices of the triangle
    NiPoint3 kVertex0;
    NiPoint3 kNormal0;
    NiPoint3 kVertex1;
    NiPoint3 kNormal1;
    NiPoint3 kVertex2;
    NiPoint3 kNormal2;

    NIASSERT(uiWhichVertex0 < kSubPartition.m_usVertices);
    NIASSERT(uiWhichVertex1 < kSubPartition.m_usVertices);
    NIASSERT(uiWhichVertex2 < kSubPartition.m_usVertices);

    DeformBySkin(kVertex0, kNormal0, (NiTriBasedGeom*) pkGeomEmitter, pkSkin,
        &kSubPartition, uiWhichVertex0);
    DeformBySkin(kVertex1, kNormal1, (NiTriBasedGeom*) pkGeomEmitter, pkSkin,
        &kSubPartition, uiWhichVertex1);
    DeformBySkin(kVertex2, kNormal2, (NiTriBasedGeom*) pkGeomEmitter, pkSkin,
        &kSubPartition, uiWhichVertex2);


    // Determine the average location of the vertices
    NiPoint3 kCenterpoint = (kVertex0 + kVertex1 + kVertex2)/3.0f;
    kPosition = kCenterpoint;


    if (m_eInitVelocityType == NI_VELOCITY_USE_NORMALS )
    {
        // Compute the average normal
        NiPoint3 kAvgNormal = (kNormal0 + kNormal1 + kNormal2)/3.0f;
        NiPoint3::UnitizeVector(kAvgNormal);
        float fSpeed = kVelocity.Length();
        kVelocity = fSpeed * kAvgNormal;
    }

    if (m_eEmissionType == NI_EMIT_FROM_FACE_SURFACE)
    {
        // Emit from a random position on the face of the skinned
        // triangle
        NiPoint3 kDir1 = kVertex1 - kVertex0;
        NiPoint3 kDir2 = kVertex2 - kVertex0;

        float fSqrt = NiFastSqrt(NiUnitRandom());

        kPosition = kVertex0 + fSqrt * (NiUnitRandom() * kDir2 - kDir1) + 
            kDir1;
    }

    TransformIntoCoordinateSpace(pkGeomEmitter, 
        kPosition, kVelocity);
    return true;
}
//---------------------------------------------------------------------------
bool NiPSysMeshEmitter::EmitFromSkinnedEdge(
    NiSkinnedEmitterData* pkSkinEmitterData, NiGeometry* pkGeomEmitter,
    NiPoint3& kPosition, NiPoint3& kVelocity)
{
    // I'm not going to bother supporting skinned lines
    if (!NiIsKindOf( NiTriBasedGeom, pkGeomEmitter))
        return false;

    // We require skin partitions
    NIASSERT(pkSkinEmitterData->m_spSkinPartition);
    if (!pkSkinEmitterData->m_spSkinPartition)
        return false;

    unsigned short usPartCount = 
        pkSkinEmitterData->m_spSkinPartition->GetPartitionCount();

    // Select a random skin partition
    unsigned int uiWhichPartition = (int) (NiUnitRandom() * (float)
        (usPartCount));
    uiWhichPartition = NiMin((int) uiWhichPartition, usPartCount - 1);

    NiSkinPartition::Partition& kSubPartition = pkSkinEmitterData
        ->m_spSkinPartition->GetPartitions()[uiWhichPartition];

    // Emit from a skinned vertex if there are no triangles in this partition
    if (kSubPartition.m_usTriangles == 0)
    {
        return EmitFromSkinnedVertex(pkSkinEmitterData, pkGeomEmitter,
            kPosition, kVelocity);
    }
  
    // Randomly select a triangle
    unsigned int uiWhichTriangle = (int) (NiUnitRandom() * ((float) 
        kSubPartition.m_usTriangles));
    uiWhichTriangle = NiMin((int) uiWhichTriangle,
        kSubPartition.m_usTriangles - 1);

    NiSkinInstance* pkSkin = pkGeomEmitter->GetSkinInstance();
    if (!pkSkin)
        return false;

    unsigned int uiWhichVertex0;
    unsigned int uiWhichVertex1;
    unsigned int uiWhichVertex2;

    if (kSubPartition.m_usStrips != 0)
    {
       uiWhichVertex0 = kSubPartition.m_pusTriList[uiWhichTriangle];
       uiWhichVertex1 = kSubPartition.m_pusTriList[uiWhichTriangle + 1];
       uiWhichVertex2 = kSubPartition.m_pusTriList[uiWhichTriangle + 2];
    }
    else
    {
       uiWhichVertex0 = kSubPartition.m_pusTriList[uiWhichTriangle*3];
       uiWhichVertex1 = kSubPartition.m_pusTriList[uiWhichTriangle*3 + 1];
       uiWhichVertex2 = kSubPartition.m_pusTriList[uiWhichTriangle*3 + 2];
    }

    // Randomly select an edge of the triangle
    switch(NiRand()%3)
    {
        default:
        case 0:
            break;
        case 1:
            uiWhichVertex0 = uiWhichVertex1;
            uiWhichVertex1 = uiWhichVertex2;
            break;
        case 2:
            uiWhichVertex1 = uiWhichVertex2;
            break;
    }

    // Deform the vertices of the edge
    NiPoint3 kVertex0;
    NiPoint3 kNormal0;
    NiPoint3 kVertex1;
    NiPoint3 kNormal1;

    DeformBySkin(kVertex0, kNormal0, (NiTriBasedGeom*) pkGeomEmitter, pkSkin,
        &kSubPartition, uiWhichVertex0);
    DeformBySkin(kVertex1, kNormal1, (NiTriBasedGeom*) pkGeomEmitter, pkSkin,
        &kSubPartition, uiWhichVertex1);


    // Compute the centerpoint
    NiPoint3 kCenterpoint = (kVertex0 + kVertex1)/2.0f;
    kPosition = kCenterpoint;

    if (m_eInitVelocityType == NI_VELOCITY_USE_NORMALS)
    {
        // Compute the average normal
        NiPoint3 kAvgNormal = (kNormal0 + kNormal1)/2.0f;
        NiPoint3::UnitizeVector(kAvgNormal);
        float fSpeed = kVelocity.Length();
        kVelocity = fSpeed * kAvgNormal;
    }

    if (m_eEmissionType == NI_EMIT_FROM_EDGE_SURFACE)
    {
        kPosition = kVertex0 + (kVertex1 - kVertex0) * NiUnitRandom();
    }

    TransformIntoCoordinateSpace(pkGeomEmitter, 
        kPosition, kVelocity);

    return true;
}
//---------------------------------------------------------------------------
void NiPSysMeshEmitter::TransformIntoSkinnedCoordinateSpace(
    NiSkinInstance* pkSkin, NiPoint3& kPosition, NiPoint3& kVelocity)
{
    NiAVObject* pkRoot = pkSkin->GetRootParent();
    if (pkRoot)
    {
        NiTransform kEmitter = pkRoot->GetWorldTransform();
        NiTransform kPSys = m_pkTarget->GetWorldTransform();
        NiTransform kInvPSys;
        kPSys.Invert(kInvPSys);
        NiTransform kEmitterToPSys = kInvPSys * kEmitter;

        // Update position.
        kPosition = kEmitterToPSys * kPosition;
        
        // Update velocity
        if (m_eInitVelocityType == NI_VELOCITY_USE_DIRECTION)
        {
            // Emit along a specific axis relative to the emitter
            NiPoint3 kDirection = kEmitterToPSys.m_Rotate * m_kEmitAxis;
            float fSpeed = m_fSpeed + m_fSpeedVar * (NiUnitRandom() - 0.5f);
            kDirection.Unitize();
            kVelocity = kDirection * fSpeed;
        }

    }

}
//---------------------------------------------------------------------------
void NiPSysMeshEmitter::TransformIntoCoordinateSpace(NiAVObject* pkObj, 
    NiPoint3& kPosition, NiPoint3& kVelocity)
{
    // Modify velocity and position based on emitter object transforms.
    if (pkObj)
    {
        NiTransform kEmitter = pkObj->GetWorldTransform();
        NiTransform kPSys = m_pkTarget->GetWorldTransform();
        NiTransform kInvPSys;
        kPSys.Invert(kInvPSys);
        NiTransform kEmitterToPSys = kInvPSys * kEmitter;

        // Update position.
        kPosition = kEmitterToPSys * kPosition;

        // Update velocity
        switch (m_eInitVelocityType)
        {
        case NI_VELOCITY_USE_NORMALS:
            // Should have be precomputed by the Emit methods
            kVelocity = kEmitterToPSys.m_Rotate * kVelocity;
            break;
        case NI_VELOCITY_USE_RANDOM:
            kVelocity.x = NiSymmetricRandom();
            kVelocity.y = NiSymmetricRandom();
            kVelocity.z = NiSymmetricRandom();
            kVelocity.Unitize();
            kVelocity *= m_fSpeed + m_fSpeedVar * (NiUnitRandom() - 0.5f);
            break;
        case NI_VELOCITY_USE_DIRECTION:
            {
                // Emit along a specific axis relative to the emitter
                NiPoint3 kDirection = kEmitterToPSys.m_Rotate * m_kEmitAxis;
                float fSpeed = m_fSpeed + m_fSpeedVar *
                    (NiUnitRandom() - 0.5f);
                kDirection.Unitize();
                kVelocity = kDirection * fSpeed;
            }
            break;

        default:
            // Perhaps do something smarter here.  Is this an error we
            // should assert?
            break;
        }

    }

}
//---------------------------------------------------------------------------
bool NiPSysMeshEmitter::NiSkinnedEmitterData::CreatePartition(
    NiGeometry* pkGeom)
{
    if (!NiIsKindOf(NiTriBasedGeom, pkGeom))
        return false;

    NiTriBasedGeom* pkTriGeom = (NiTriBasedGeom*) pkGeom;
    if (pkTriGeom && pkTriGeom->GetSkinInstance())
    {
        NiSkinInstance* pkSkin = pkTriGeom->GetSkinInstance();
        NiSkinPartition* pkPartition = pkSkin->GetSkinPartition();

        // If we don't have a partition, make one
        if (pkPartition == NULL)
        {
            NiSkinData* pkData = pkSkin->GetSkinData();
            if (!pkData)
                return false;

            // Keep it simple, stupid.. use the maximum number of bones in the
            // mesh for the maximum number of bones in the partition. Limit
            // the mesh to 4 bones influencing a vertex.
            unsigned int uiMaxNumMatrices = pkData->GetBoneCount();
            if (uiMaxNumMatrices < 4)
                uiMaxNumMatrices = 4;
            pkPartition = NiNew NiSkinPartition;
            bool bMade = pkPartition->MakePartitions((NiTriBasedGeomData*) 
                pkTriGeom->GetModelData(), pkSkin->GetSkinData(), 
                uiMaxNumMatrices, 4, false);
            if (!bMade)
                return false;
        }
        // Otherwise, just use the partition on the object

        m_spSkinPartition = pkPartition;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiPSysMeshEmitter::DeformBySkin(NiPoint3& kVert, NiPoint3& kNormal, 
    NiTriBasedGeom* pkGeom, NiSkinInstance* pkSkin,  
    NiSkinPartition::Partition* pkPart, unsigned int uiWhichIndex)
{
    NIASSERT(pkGeom);
    NIASSERT(pkPart);
    NIASSERT(pkSkin);

    NiSkinData* pkSkinData = pkSkin->GetSkinData();
    unsigned int i = 0;
    NiTransform kWorldToSkin, kWorldToRootParent;
    pkSkin->GetRootParent()->GetWorldTransform().Invert(kWorldToRootParent);
    kWorldToSkin = pkSkinData->GetRootParentToSkin() * kWorldToRootParent;
    const NiSkinData::BoneData* pkBoneData = pkSkinData->GetBoneData();

    NiPoint3 kDstVert = NiPoint3::ZERO;
    NiPoint3 kDstNormal = NiPoint3::ZERO;

    // Positions and normals only
    NiPoint3* pVertices = pkGeom->GetVertices();
    NiPoint3* pNormals = pkGeom->GetNormals();
    NIASSERT(pVertices);
    
    NiPoint3 kSrcVert = pVertices[pkPart->m_pusVertexMap[uiWhichIndex]];
    NiPoint3 kSrcNorm;
    if (pNormals)
        kSrcNorm = pNormals[pkPart->m_pusVertexMap[uiWhichIndex]];
    else
        kSrcNorm = NiPoint3(1.0f, 0.0f, 0.0f);

    for (i = 0; i < pkPart->m_usBonesPerVertex; i++)
    {
        unsigned short usBoneIndex = 0;
        
        if (pkPart->m_pucBonePalette)
        {
            unsigned short usBonesLookup = pkPart->m_pucBonePalette[
                uiWhichIndex*pkPart->m_usBonesPerVertex + i];
            usBoneIndex = pkPart->m_pusBones[usBonesLookup];
        }
        else
        {
            usBoneIndex = pkPart->m_pusBones[i];
        }

        NiTransform kXform;
        
        NIASSERT(usBoneIndex < pkSkinData->GetBoneCount());
        NiAVObject* pkBone = pkSkin->GetBones()[usBoneIndex];
        kXform = kWorldToSkin * pkBone->GetWorldTransform() *
            pkBoneData[usBoneIndex].m_kSkinToBone;
               
        NiMatrix3 kScaledRotate = kXform.m_Rotate * kXform.m_fScale;
        
        float fWeight = pkPart->m_pfWeights[4*uiWhichIndex + i];
     
        NiPoint3 kVertex;
        NiMatrixTimesPointPlusPoint(kVertex, kScaledRotate, 
            kSrcVert, kXform.m_Translate);
        NiPoint3 kNormal;
        NiMatrixTimesPoint(kNormal, kXform.m_Rotate, kSrcNorm);
        
        kDstVert += fWeight * kVertex;
        kDstNormal += fWeight * kNormal;
        
    }
    
    kDstNormal.Unitize();
    kVert = kDstVert;
    kNormal = kDstNormal;
}
//---------------------------------------------------------------------------
void NiPSysMeshEmitter::SetPrecacheAttributes(NiGeometry * pkObj)
{
    if (pkObj)
    {
        NiGeometryData* pkGeomData = pkObj->GetModelData();
        if (pkGeomData)
        {
            unsigned int uiKeepFlags = pkGeomData->GetKeepFlags();
            uiKeepFlags |= NiGeometryData::KEEP_XYZ;
            uiKeepFlags |= NiGeometryData::KEEP_NORM;
            uiKeepFlags |= NiGeometryData::KEEP_INDICES;
            uiKeepFlags |= NiGeometryData::KEEP_BONEDATA;
            pkGeomData->SetKeepFlags(uiKeepFlags);
        }
    }
}
//---------------------------------------------------------------------------
void NiPSysMeshEmitter::AddGeometryEmitter(NiGeometry* pkGeomEmitter)
{
    if (pkGeomEmitter)
    {
#ifdef DEBUG
        unsigned int uiIndex = 
#endif
            m_kGeomEmitterArray.AddFirstEmpty(pkGeomEmitter);

        NiPSysMeshEmitter::NiSkinnedEmitterData* spEmitterData = NULL;
        if (pkGeomEmitter->GetSkinInstance())
        {
            NiSkinInstance* pkSkinInstance = pkGeomEmitter->GetSkinInstance();
            NiSkinPartition* pkPartition = pkSkinInstance->GetSkinPartition();
            
            if (pkPartition)
            {
                spEmitterData = NiNew NiSkinnedEmitterData;
                spEmitterData->m_spSkinPartition = pkPartition;
            }
            else
            {
                spEmitterData = NiNew NiSkinnedEmitterData;
            }
        }
        else
        {
            spEmitterData = NiNew NiSkinnedEmitterData;
        }

#ifdef DEBUG
        unsigned int uiIndex2 = 
#endif
            m_kSkinnedEmitterData.AddFirstEmpty(spEmitterData);
#ifdef DEBUG
        NIASSERT(uiIndex == uiIndex2);
        NIASSERT(pkGeomEmitter->GetModelData() != NULL);
#endif

        SetPrecacheAttributes(pkGeomEmitter);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysMeshEmitter);
//---------------------------------------------------------------------------
void NiPSysMeshEmitter::CopyMembers(NiPSysMeshEmitter* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysEmitter::CopyMembers(pkDest, kCloning);

    pkDest->m_kGeomEmitterArray.SetSize(m_kGeomEmitterArray.GetSize());
    pkDest->m_eInitVelocityType = m_eInitVelocityType;
    pkDest->m_eEmissionType = m_eEmissionType;
    pkDest->m_kEmitAxis = m_kEmitAxis;
}
//---------------------------------------------------------------------------
void NiPSysMeshEmitter::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysEmitter::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);
    NiPSysMeshEmitter* pkDest = (NiPSysMeshEmitter*) pkClone;

    for (unsigned int ui = 0; ui < GetGeometryEmitterCount(); ui++)
    {
        NiGeometry* pkEmitter = GetGeometryEmitter(ui);
        if (pkEmitter)
        {
            bCloned = kCloning.m_pkCloneMap->GetAt(pkEmitter, pkClone);
            if (bCloned)
            {
                NIASSERT(pkEmitter->GetRTTI() == pkClone->GetRTTI());
                pkDest->AddGeometryEmitter((NiGeometry*) pkClone);
            }
            else
            {
                pkDest->AddGeometryEmitter((NiGeometry*) pkEmitter);
            }
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysMeshEmitter);
//---------------------------------------------------------------------------
void NiPSysMeshEmitter::LoadBinary(NiStream& kStream)
{
    NiPSysEmitter::LoadBinary(kStream);

    kStream.ReadMultipleLinkIDs();   // m_kGeomEmitterArray

    NiStreamLoadEnum(kStream, m_eInitVelocityType);
    NiStreamLoadEnum(kStream, m_eEmissionType);
    m_kEmitAxis.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysMeshEmitter::LinkObject(NiStream& kStream)
{
    NiPSysEmitter::LinkObject(kStream);

     // link children
    unsigned int uiSize = kStream.GetNumberOfLinkIDs();
    if (uiSize)
    {
        m_kGeomEmitterArray.SetSize(uiSize);
        for (unsigned int i = 0; i < uiSize; i++)
        {
            NiGeometry* pkChild = 
                (NiGeometry*) kStream.GetObjectFromLinkID();

            AddGeometryEmitter((NiGeometry*) pkChild);
        }
    }
}
//---------------------------------------------------------------------------
bool NiPSysMeshEmitter::RegisterStreamables(NiStream& kStream)
{
    return NiPSysEmitter::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysMeshEmitter::SaveBinary(NiStream& kStream)
{
    NiPSysEmitter::SaveBinary(kStream);

    // save GeomEmitters
    unsigned int uiGeomEmittersSize = m_kGeomEmitterArray.GetSize();
    NiStreamSaveBinary(kStream, uiGeomEmittersSize);
    for (unsigned int i = 0; i < uiGeomEmittersSize; i++)
    {
        NiGeometry* pkGeom = m_kGeomEmitterArray.GetAt(i);
        kStream.SaveLinkID(pkGeom);
    }

    NiStreamSaveEnum(kStream, m_eInitVelocityType);
    NiStreamSaveEnum(kStream, m_eEmissionType);
    m_kEmitAxis.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysMeshEmitter::IsEqual(NiObject* pkObject)
{
    if (!NiPSysEmitter::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysMeshEmitter* pkDest = (NiPSysMeshEmitter*) pkObject;

    // children
    unsigned int uiCount0 = m_kGeomEmitterArray.GetSize();
    unsigned int uiCount1 = pkDest->m_kGeomEmitterArray.GetSize();
    if (uiCount0 != uiCount1)
        return false;

    for (unsigned int i = 0; i < uiCount0; i++)
    {
        NiAVObject* pkGeomEmitter0 = m_kGeomEmitterArray.GetAt(i);
        NiAVObject* pkGeomEmitter1 = pkDest->m_kGeomEmitterArray.GetAt(i);
        if ((pkGeomEmitter0 && !pkGeomEmitter1) || 
            (!pkGeomEmitter0 && pkGeomEmitter1))
        {
            return false;
        }

        if (pkGeomEmitter0 && !pkGeomEmitter0->IsEqual(pkGeomEmitter1))
            return false;
    }

    if (m_eInitVelocityType != pkDest->m_eInitVelocityType)
        return false;

    if (m_eEmissionType != pkDest->m_eEmissionType)
        return false;

    if (m_kEmitAxis != pkDest->m_kEmitAxis)
        return false;

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysMeshEmitter::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysEmitter::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysMeshEmitter::ms_RTTI.GetName()));
    for (unsigned int ui = 0; ui < GetGeometryEmitterCount(); ui++)
    {
        NiGeometry* pkGeom = GetGeometryEmitter(ui);
        if (pkGeom)
        {
            pkStrings->Add(NiGetViewerString("Emitter Object", 
                pkGeom->GetName()));
        }
    }

    switch (m_eEmissionType)
    {
        case NI_EMIT_FROM_VERTICES:
            pkStrings->Add(NiGetViewerString("m_eEmissionType",
                "NI_EMIT_FROM_VERTICES"));
            break;
        case NI_EMIT_FROM_FACE_CENTER:
            pkStrings->Add(NiGetViewerString("m_eEmissionType",
                "NI_EMIT_FROM_FACE_CENTER"));
            break;
        case NI_EMIT_FROM_EDGE_CENTER:
            pkStrings->Add(NiGetViewerString("m_eEmissionType",
                "NI_EMIT_FROM_EDGE_CENTER"));
            break;
        case NI_EMIT_FROM_FACE_SURFACE:
            pkStrings->Add(NiGetViewerString("m_eEmissionType",
                "NI_EMIT_FROM_FACE_SURFACE"));
            break;
        case NI_EMIT_FROM_EDGE_SURFACE:
            pkStrings->Add(NiGetViewerString("m_eEmissionType",
                "NI_EMIT_FROM_EDGE_SURFACE"));
            break;
        case NI_EMIT_MAX:
        default:
            pkStrings->Add(NiGetViewerString("m_eEmissionType",
                "UNKNOWN!!!"));
            break;
    }

    switch (m_eInitVelocityType)
    {
        case NI_VELOCITY_USE_NORMALS:
            pkStrings->Add(NiGetViewerString("m_eInitVelocityType",
                "NI_VELOCITY_USE_NORMALS"));
            break;
        case NI_VELOCITY_USE_RANDOM:
            pkStrings->Add(NiGetViewerString("m_eInitVelocityType",
                "NI_VELOCITY_USE_RANDOM"));
            break;
        case NI_VELOCITY_USE_DIRECTION:
            pkStrings->Add(NiGetViewerString("m_eInitVelocityType",
                "NI_VELOCITY_USE_DIRECTION"));
            break;
        case NI_VELOCITY_MAX:
        default:
            pkStrings->Add(NiGetViewerString("m_eInitVelocityType",
                "UNKNOWN!!!"));
            break;
    }

    pkStrings->Add(NiGetViewerString("m_kEmissionAxis.x", 
        m_kEmitAxis.x));
    pkStrings->Add(NiGetViewerString("m_kEmissionAxis.y", 
        m_kEmitAxis.y));
    pkStrings->Add(NiGetViewerString("m_kEmissionAxis.z", 
        m_kEmitAxis.z));
    
}
//---------------------------------------------------------------------------
