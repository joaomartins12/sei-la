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
//---------------------------------------------------------------------------
#include "NiNBTCalculation.h"

#include <NiSystem.h>

#include <NiGeometry.h>
#include <NiTriShape.h>
#include <NiTriShapeData.h>
#include <NiTriStrips.h>
#include <NiTriStripsData.h>

// ATi method
#include "ATi/NmFileIO.h"

// MAX method
#pragma conform(forScope, push, f, off)
#include "Max.h"
#pragma conform(forScope, pop, f)

//---------------------------------------------------------------------------
NiNBTCalculation::NiNBTCalculation()
{
}
//---------------------------------------------------------------------------
NiNBTCalculation::~NiNBTCalculation()
{
}
//---------------------------------------------------------------------------
bool NiNBTCalculation::SetupBinormalTangentData(NiGeometry* pkGeometry,
        unsigned short usUVSet)
{
    NiGeometryData::DataFlags eNBTMethod = 
        pkGeometry->GetNormalBinormalTangentMethod();

    // Don't need to calculate if no method specified
    if (eNBTMethod == NiGeometryData::NBT_METHOD_NONE)
        return true;

    // Skinned objects should not require special treatment, and we should
    // only have NiTriShapes at this point
    if (NiIsExactKindOf(NiTriShape, pkGeometry))
    {
        CreateTangentSpace_TriShape((NiTriShape*)pkGeometry, eNBTMethod, 
            usUVSet);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiNBTCalculation::CreateTangentSpace_TriShape(NiTriShape* pkTriShape,
    NiGeometryData::DataFlags eNBTMethod, unsigned short usUVSet)
{
    unsigned int uiNumVertices = pkTriShape->GetVertexCount();
    unsigned int uiNumIndices = pkTriShape->GetTriListLength();

    NiPoint3* pkVertices = pkTriShape->GetVertices();
    NiPoint3* pkNormals = pkTriShape->GetNormals();

    if (usUVSet >= pkTriShape->GetTextureSets())
        return false;

    NiPoint2* pkTextures = pkTriShape->GetTextureSet(usUVSet);
    
    if (!pkTextures)
        return false;

    unsigned short* pusIndices = pkTriShape->GetTriList();

    bool bReturn = false;
    switch (eNBTMethod)
    {
    case NiGeometryData::NBT_METHOD_NDL:
        bReturn = CreateBasisMatrices_NDL(uiNumVertices, pkVertices,
            pkNormals, pkTextures, uiNumIndices, pusIndices);
        break;
    case NiGeometryData::NBT_METHOD_ATI:
        bReturn = CreateBasisMatrices_ATI(uiNumVertices, pkVertices,
            pkNormals, pkTextures, uiNumIndices, pusIndices);
        break;
    case NiGeometryData::NBT_METHOD_MAX:
        bReturn = CreateBasisMatrices_MAX(uiNumVertices, pkVertices,
            pkNormals, pkTextures, uiNumIndices, pusIndices);
        break;
    }

    return bReturn;
}
//---------------------------------------------------------------------------
bool NiNBTCalculation::CreateBasisMatrices_NDL(unsigned int uiNumVertices, 
    NiPoint3* pkVertices, NiPoint3* pkNormals, NiPoint2* pkTextures, 
    unsigned int uiNumIndices, unsigned short* pusIndices)
{
    NIASSERT(pkVertices);
    NIASSERT(pkNormals);
    NIASSERT(pkTextures);
    NIASSERT(pusIndices);

    int iNumNBTVectors = 3 * uiNumVertices;
    NiPoint3* pkNBTs = pkNormals;

    // Clear out binormals and tangents.
    memset(pkNBTs + uiNumVertices, 0, sizeof(*pkNormals) * 2 * uiNumVertices);
       
    // The vertices, normals and UVs of a given triangle.  
    // For determining NBT for kV0.
    NiPoint3 kV0, kV1, kV2;  
    NiPoint3 kN0, kN1, kN2;  
    NiPoint2 kT0, kT1, kT2;  

    // The number of calculated NBTs.  Dependent upon the number of 
    // triangles and the type of pkData
    unsigned int uiNumberOfCalcs;  
    unsigned short usCurrentTriangle = 0;

    uiNumberOfCalcs = uiNumIndices / 3;

    NIASSERT(uiNumberOfCalcs < 65536);

    for (unsigned int uiCalc = 0; uiCalc < uiNumberOfCalcs; uiCalc++)
    {
        unsigned int uiIndex = uiCalc * 3;
        usCurrentTriangle = uiIndex;
        kV0 = pkVertices[pusIndices[uiIndex]];
        kV1 = pkVertices[pusIndices[uiIndex + 1]];
        kV2 = pkVertices[pusIndices[uiIndex + 2]];
        kN0 = pkNormals[pusIndices[uiIndex]];
        kN1 = pkNormals[pusIndices[uiIndex + 1]];
        kN2 = pkNormals[pusIndices[uiIndex + 2]];
        kT0 = pkTextures[pusIndices[uiIndex]];
        kT1 = pkTextures[pusIndices[uiIndex + 1]];
        kT2 = pkTextures[pusIndices[uiIndex + 2]];
        
        unsigned int uiTriIndex = pusIndices[usCurrentTriangle];
        FindNBT(pkNBTs, uiTriIndex, uiNumVertices, kV0, kV1, kV2, kN0, kN1, 
            kN2, kT0, kT1, kT2);
        uiTriIndex = pusIndices[usCurrentTriangle + 1];
        FindNBT(pkNBTs, uiTriIndex, uiNumVertices, kV1, kV0, kV2, kN1, kN0,
            kN2, kT1, kT0, kT2);
        uiTriIndex = pusIndices[usCurrentTriangle + 2];
        FindNBT(pkNBTs, uiTriIndex, uiNumVertices, kV2, kV1, kV0, kN2, kN1, 
            kN0, kT2, kT1, kT0);
    }

    NiPoint3::UnitizeVectors(pkNBTs, uiNumVertices * 3, sizeof(NiPoint3));

    return true;
}
//----------------------------------------------------------------------------
void NiNBTCalculation::FindNBT(NiPoint3* pkNBTs, unsigned int uiNBTIndex, 
    unsigned int uiNumVertices, NiPoint3 kVert0, NiPoint3 kVert1, 
    NiPoint3 kVert2, NiPoint3 kNorm0, NiPoint3 kNorm1, NiPoint3 kNorm2, 
    NiPoint2 kTex0, NiPoint2 kTex1, NiPoint2 kTex2)
{
    // The edge vectors in cartesian coordinates
    NiPoint3 kEC0, kEC1; 
    // The edge vectors in texture coordinates
    NiPoint2 kET0, kET1; 

    kEC0 = kVert1 - kVert0;
    kEC1 = kVert2 - kVert0;

    kET0 = kTex1 - kTex0;
    kET1 = kTex2 - kTex0;

    NiPoint3 kBinormal, kTangent;

    // If this value is 0, then we can't calculate blending values.
    // We'll use the other edge.  If that has a zero value, then the
    // triangle is parametrically degenerate.
    if (kET0.x == 0)
    {
        kEC1 = kVert1 - kVert0;
        kEC0 = kVert2 - kVert0;

        kET1 = kTex1 - kTex0;
        kET0 = kTex2 - kTex0;
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
bool NiNBTCalculation::CreateBasisMatrices_ATI(unsigned int uiNumVertices, 
    NiPoint3* pkVertices, NiPoint3* pkNormals, NiPoint2* pkTextures, 
    unsigned int uiNumIndices, unsigned short* pusIndices)
{
    NIASSERT(pkVertices);
    NIASSERT(pkNormals);
    NIASSERT(pkTextures);
    NIASSERT(pusIndices);

    if (!pkTextures || !pkVertices || !pkNormals ||!pusIndices)
        return false;

    NiPoint3* pkBinormals = &pkNormals[uiNumVertices];
    NiPoint3* pkTangents = &pkBinormals[uiNumVertices];

    // Clear them out
    memset((void*)pkBinormals, 0, sizeof(NiPoint3) * uiNumVertices);
    memset((void*)pkTangents, 0, sizeof(NiPoint3) * uiNumVertices);

    // Put data into ATI's data structures
    NIASSERT((uiNumIndices % 3) == 0);
    unsigned int uiNumTris = uiNumIndices / 3;
    NmRawTriangle* pkTris = NiExternalNew NmRawTriangle[uiNumTris];
    unsigned int uiIndex = 0;
    unsigned int i = 0;
    for (; i < uiNumTris; i++)
    {
        NmRawTriangle* pkTri = &pkTris[i];

        for (unsigned int j = 0; j < 3; j++)
        {
            unsigned int uiGeomIndex = pusIndices[uiIndex++];

            pkTri->vert[j].x = pkVertices[uiGeomIndex].x;
            pkTri->vert[j].y = pkVertices[uiGeomIndex].y;
            pkTri->vert[j].z = pkVertices[uiGeomIndex].z;

            pkTri->norm[j].x = pkNormals[uiGeomIndex].x;
            pkTri->norm[j].y = pkNormals[uiGeomIndex].y;
            pkTri->norm[j].z = pkNormals[uiGeomIndex].z;

            pkTri->texCoord[j].u = pkTextures[uiGeomIndex].x;
            pkTri->texCoord[j].v = pkTextures[uiGeomIndex].y;
        }
    }
    NIASSERT(uiIndex == uiNumIndices);

    NmRawTangentSpaceD* pkTSData = NULL;

    bool bSuccess = NmComputeTangentsD(uiNumTris, pkTris, &pkTSData);

    uiIndex = 0;
    for (i = 0; i < uiNumTris; i++)
    {
        NmRawTangentSpaceD* pkTri = &pkTSData[i];
        for (unsigned int j = 0; j < 3; j++)
        {
            unsigned int uiGeomIndex = pusIndices[uiIndex++];

            pkTangents[uiGeomIndex].x = (float)pkTri->tangent[j].x;
            pkTangents[uiGeomIndex].y = (float)pkTri->tangent[j].y;
            pkTangents[uiGeomIndex].z = (float)pkTri->tangent[j].z;

            pkBinormals[uiGeomIndex].x = (float)pkTri->binormal[j].x;
            pkBinormals[uiGeomIndex].y = (float)pkTri->binormal[j].y;
            pkBinormals[uiGeomIndex].z = (float)pkTri->binormal[j].z;
        }
    }

    NiExternalDelete [] pkTris;
    NiExternalDelete [] pkTSData;
    
    return true;
}
//---------------------------------------------------------------------------
bool NiNBTCalculation::CreateBasisMatrices_MAX(unsigned int uiNumVertices, 
    NiPoint3* pkVertices, NiPoint3* pkNormals, NiPoint2* pkTextures, 
    unsigned int uiNumIndices, unsigned short* pusIndices)
{
    // The Max API for computing Binormals and tangents is only in Max 7+
#if MAX_RELEASE >= 7000
    NIASSERT(pkVertices);
    NIASSERT(pkNormals);
    NIASSERT(pkTextures);
    NIASSERT(pusIndices);

    int iNumNBTVectors = 3 * uiNumVertices;
    NiPoint3* pkNBTs = pkNormals;

    // Clear out binormals and tangents.
    memset(pkNBTs + uiNumVertices, 0, sizeof(*pkNormals) * 2 * uiNumVertices);

    // The vertices and UVs of a given triangle in Max's data format
    Point3 kTriUVs[3];
    Point3 kTriXYZs[3];
    // the return value for Max's Binorm Tangent calculation
    Point3 kTangBinorm[2];

    // The number of calculated NBTs
    unsigned int uiNumberOfCalcs;  
    uiNumberOfCalcs = uiNumIndices / 3;

    NIASSERT(uiNumberOfCalcs < USHRT_MAX);

    for (unsigned int uiCalc = 0; uiCalc < uiNumberOfCalcs; uiCalc++)
    {
        unsigned int uiIndex = uiCalc * 3;
        unsigned short usIndex1 = pusIndices[uiIndex];
        unsigned short usIndex2 = pusIndices[uiIndex + 1];
        unsigned short usIndex3 = pusIndices[uiIndex + 2];

        // store Gamebryo vertex data in Max data structure
        kTriXYZs[0].x = pkVertices[usIndex1].x;
        kTriXYZs[0].y = pkVertices[usIndex1].y;
        kTriXYZs[0].z = pkVertices[usIndex1].z;
        kTriXYZs[1].x = pkVertices[usIndex2].x;
        kTriXYZs[1].y = pkVertices[usIndex2].y;
        kTriXYZs[1].z = pkVertices[usIndex2].z;
        kTriXYZs[2].x = pkVertices[usIndex3].x;
        kTriXYZs[2].y = pkVertices[usIndex3].y;
        kTriXYZs[2].z = pkVertices[usIndex3].z;

        // store Gamebryo texture coordinate data in max data structure
        kTriUVs[0].x = pkTextures[usIndex1].x;
        kTriUVs[0].y = pkTextures[usIndex1].y;
        kTriUVs[1].x = pkTextures[usIndex2].x;
        kTriUVs[1].y = pkTextures[usIndex2].y;
        kTriUVs[2].x = pkTextures[usIndex3].x;
        kTriUVs[2].y = pkTextures[usIndex3].y;

        // compute NBT frame
        ComputeTangentAndBinormal(kTriUVs, kTriXYZs, kTangBinorm);

        // if a triangle's UVs are CCW, we must flip the Binormals
        // first get the triangle's UV edge vectors
        NiPoint3 kP1P2Vector;
        NiPoint3 kP1P3Vector;
        kP1P2Vector.x = pkTextures[usIndex2].x - pkTextures[usIndex1].x;
        kP1P2Vector.y = pkTextures[usIndex2].y - pkTextures[usIndex1].y;
        kP1P2Vector.z = 0.0f;
        kP1P3Vector.x = pkTextures[usIndex3].x - pkTextures[usIndex1].x;
        kP1P3Vector.y = pkTextures[usIndex3].y - pkTextures[usIndex1].y;
        kP1P3Vector.z = 0.0f;
        // take the cross product of the triangle edges
        NiPoint3 kCrossResult = kP1P2Vector.Cross(kP1P3Vector);
        // a positive z component indicates Counter-Clockwise
        // Counter-clockwise triangles need to have their Binormal flipped
        float fCCW = (kCrossResult.z > 0.0f) ? 1.0f : -1.0f;

        // vert 0 of triangle
        pkNBTs[uiNumVertices + usIndex1].x += fCCW * kTangBinorm[1].x;
        pkNBTs[uiNumVertices + usIndex1].y += fCCW * kTangBinorm[1].y;
        pkNBTs[uiNumVertices + usIndex1].z += fCCW * kTangBinorm[1].z;
        pkNBTs[(2 * uiNumVertices) + usIndex1].x += kTangBinorm[0].x;
        pkNBTs[(2 * uiNumVertices) + usIndex1].y += kTangBinorm[0].y;
        pkNBTs[(2 * uiNumVertices) + usIndex1].z += kTangBinorm[0].z;

        // vert 1 of triangle
        pkNBTs[uiNumVertices + usIndex2].x += fCCW * kTangBinorm[1].x;
        pkNBTs[uiNumVertices + usIndex2].y += fCCW * kTangBinorm[1].y;
        pkNBTs[uiNumVertices + usIndex2].z += fCCW * kTangBinorm[1].z;
        pkNBTs[(2 * uiNumVertices) + usIndex2].x += kTangBinorm[0].x;
        pkNBTs[(2 * uiNumVertices) + usIndex2].y += kTangBinorm[0].y;
        pkNBTs[(2 * uiNumVertices) + usIndex2].z += kTangBinorm[0].z;

        // vert 2 of triangle
        pkNBTs[uiNumVertices + usIndex3].x += fCCW * kTangBinorm[1].x;
        pkNBTs[uiNumVertices + usIndex3].y += fCCW * kTangBinorm[1].y;
        pkNBTs[uiNumVertices + usIndex3].z += fCCW * kTangBinorm[1].z;
        pkNBTs[(2 * uiNumVertices) + usIndex3].x += kTangBinorm[0].x;
        pkNBTs[(2 * uiNumVertices) + usIndex3].y += kTangBinorm[0].y;
        pkNBTs[(2 * uiNumVertices) + usIndex3].z += kTangBinorm[0].z;
    }

    NiPoint3::UnitizeVectors(pkNBTs, uiNumVertices * 3, sizeof(NiPoint3));

    return true;
#else
    // if we are using a version of Max older than 7, just call the ATI method
    return CreateBasisMatrices_ATI(uiNumVertices, pkVertices, pkNormals, 
        pkTextures, uiNumIndices, pusIndices);
#endif
}
//---------------------------------------------------------------------------
