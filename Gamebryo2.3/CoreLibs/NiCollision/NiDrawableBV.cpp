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
#include "NiCollisionPCH.h"

#include <NiBoundingVolume.h>
#include <NiColor.h>
#include <NiMaterialProperty.h>
#include <NiMath.h>
#include <NiNode.h>
#include <NiTriShape.h>
#include <NiVertexColorProperty.h>
#include <NiWireframeProperty.h>
#include "NiBoxBV.h"
#include "NiCapsuleBV.h"
#include "NiCollisionData.h"
#include "NiSphereBV.h"
#include "NiUnionBV.h"
#include "NiDrawableBV.h"
#include "NiHalfSpaceBV.h"

//---------------------------------------------------------------------------
NiNode* NiDrawableBV::CreateWireframeBV(const NiBoundingVolume* pkBound, 
    const NiColor& kColor, const float fMult, NiAVObject* pkObject)
{
    // bounding volume must exist
    if (!pkBound)
        return 0;

    NiNode* pkRoot = NiNew NiNode;
    NIASSERT(pkRoot);
    NiCollisionData* pkCollisionData = NiNew NiCollisionData(pkRoot);
    NIASSERT(pkCollisionData);
    pkCollisionData->SetCollisionMode(NiCollisionData::NOTEST);
    pkCollisionData->SetPropagationMode(NiCollisionData::PROPAGATE_NEVER);

    NiTriShape* pkShape;
    switch (pkBound->Type())
    {
        case NiBoundingVolume::BOX_BV:
            pkShape = CreateFromBox(((NiBoxBV*)pkBound)->GetBox(), fMult);
            pkRoot->AttachChild(pkShape);
            break;
        case NiBoundingVolume::CAPSULE_BV:
            pkShape = CreateFromCapsule(
                ((NiCapsuleBV*)pkBound)->GetCapsule(), fMult);
            pkRoot->AttachChild(pkShape);
            break;
        case NiBoundingVolume::SPHERE_BV:
            pkShape = CreateFromSphere(
                ((NiSphereBV*)pkBound)->GetSphere(), fMult);
            pkRoot->AttachChild(pkShape);
            break;
       case NiBoundingVolume::UNION_BV:
           CreateFromUnion((NiUnionBV*)pkBound, pkRoot, fMult);
           break;
        case NiBoundingVolume::HALFSPACE_BV:
            pkShape = CreateFromHalfSpaceBV((NiHalfSpaceBV*)pkBound, fMult,
                pkObject);
            pkRoot->AttachChild(pkShape);
            break;
        default:  // New BV type?
            NiDelete pkRoot;
            return 0;
            break;
    }

    NiWireframeProperty* pkWire = NiNew NiWireframeProperty;
    pkWire->SetWireframe(true);
    pkRoot->AttachProperty(pkWire);

    NiVertexColorProperty* pkVC = NiNew NiVertexColorProperty;
    pkVC->SetSourceMode(NiVertexColorProperty::SOURCE_IGNORE);
    pkVC->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    pkRoot->AttachProperty(pkVC);

    NiMaterialProperty* pkMat = NiNew NiMaterialProperty;
    pkMat->SetEmittance(kColor);
    pkRoot->AttachProperty(pkMat);

    return pkRoot;
}
//---------------------------------------------------------------------------
NiTriShape* NiDrawableBV::CreateFromHalfSpaceBV(
    const NiHalfSpaceBV* pkHalfSpaceBV, const float fMult, 
    const NiAVObject* pkObject)
{
    NiPoint3 kAxis = pkHalfSpaceBV->GetPlane().GetNormal();
    NiPoint3 kCenter = pkHalfSpaceBV->GetCenter();
    float fExtent = pkObject->GetWorldBound().GetRadius();

    // create two basis vectors that are perpendicular to the normal
    NiPoint3 kBasis1;
    if (NiAbs(kAxis.x) >= NiAbs(kAxis.y) && NiAbs(kAxis.x) >= NiAbs(kAxis.z))
    {
        kBasis1.x = -kAxis.y;
        kBasis1.y = kAxis.x;
        kBasis1.z = kAxis.z;
    }
    else
    {
        kBasis1.x = kAxis.x;
        kBasis1.y = kAxis.z;
        kBasis1.z = -kAxis.y;
    }

    kBasis1.Unitize();
    NiPoint3 kBasis0 = kBasis1.Cross(kAxis);

    // use fMult to determine the size of the plane representation
    kBasis0 *= fExtent;
    kBasis1 *= fExtent;
    NiPoint3 kNorm0 = kBasis0 * 0.05f;
    NiPoint3 kNorm1 = kBasis1 * 0.05f;
    NiPoint3 kNorm2 = kAxis * fExtent * 0.5f;
    NiPoint3 kOffset = kNorm2 * (1.0f - fMult);

    const unsigned int uiNumVerts = 12;
    const unsigned int uiNumTris = 6;

    NiPoint3* pkPoints = NiNew NiPoint3[uiNumVerts];

    // plane
    pkPoints[0] = kCenter + kBasis0 + kBasis1 + kOffset;
    pkPoints[1] = kCenter + kBasis0 - kBasis1 + kOffset;
    pkPoints[2] = kCenter - kBasis0 - kBasis1 + kOffset;
    pkPoints[3] = kCenter - kBasis0 + kBasis1 + kOffset;
    // normal base
    pkPoints[4] = kCenter + kNorm0 + kNorm1 + kOffset;
    pkPoints[5] = kCenter + kNorm0 - kNorm1 + kOffset; 
    pkPoints[6] = kCenter - kNorm0 - kNorm1 + kOffset; 
    pkPoints[7] = kCenter - kNorm0 + kNorm1 + kOffset;
    // normal tip
    pkPoints[8] = kCenter + kNorm0 + kNorm1 + kNorm2;
    pkPoints[9] = kCenter + kNorm0 - kNorm1 + kNorm2; 
    pkPoints[10] = kCenter - kNorm0 - kNorm1 + kNorm2; 
    pkPoints[11] = kCenter - kNorm0 + kNorm1 + kNorm2; 

    unsigned short* pusConn = NiAlloc(unsigned short, 3 * uiNumTris);
    pusConn[0]  = 0;  pusConn[1]  = 3;  pusConn[2]  = 1;
    pusConn[3]  = 3;  pusConn[4]  = 2;  pusConn[5]  = 1;
    pusConn[6]  = 8;  pusConn[7]  = 10; pusConn[8]  = 4;
    pusConn[9]  = 10; pusConn[10] = 6;  pusConn[11] = 4;
    pusConn[12] = 9;  pusConn[13] = 11; pusConn[14] = 5;
    pusConn[15] = 11; pusConn[16] = 7;  pusConn[17] = 5;

    return NiNew NiTriShape(uiNumVerts, pkPoints, 0, 0, 0, 0, 
        NiGeometryData::NBT_METHOD_NONE, uiNumTris, pusConn);
}
//---------------------------------------------------------------------------
NiTriShape* NiDrawableBV::CreateFromBox(const NiBox& kBox, const float fMult)
{
    const float afExtent[3] = {
        kBox.m_afExtent[0] * fMult, 
        kBox.m_afExtent[1] * fMult, 
        kBox.m_afExtent[2] * fMult
    };

    const unsigned int uiNumVerts = 8;
    const unsigned int uiNumTris = 12;

    NiPoint3* pkPoints = NiNew NiPoint3[uiNumVerts];

    pkPoints[0] = kBox.m_kCenter
        + kBox.m_akAxis[0] * afExtent[0]
        - kBox.m_akAxis[1] * afExtent[1]
        - kBox.m_akAxis[2] * afExtent[2];
    pkPoints[1] = kBox.m_kCenter 
        + kBox.m_akAxis[0] * afExtent[0]
        + kBox.m_akAxis[1] * afExtent[1]
        - kBox.m_akAxis[2] * afExtent[2];
    pkPoints[2] = kBox.m_kCenter 
        - kBox.m_akAxis[0] * afExtent[0]
        + kBox.m_akAxis[1] * afExtent[1]
        - kBox.m_akAxis[2] * afExtent[2];
    pkPoints[3] = kBox.m_kCenter 
        - kBox.m_akAxis[0] * afExtent[0]
        - kBox.m_akAxis[1] * afExtent[1]
        - kBox.m_akAxis[2] * afExtent[2];
    pkPoints[4] = kBox.m_kCenter 
        + kBox.m_akAxis[0] * afExtent[0]
        - kBox.m_akAxis[1] * afExtent[1]
        + kBox.m_akAxis[2] * afExtent[2];
    pkPoints[5] = kBox.m_kCenter 
        + kBox.m_akAxis[0] * afExtent[0]
        + kBox.m_akAxis[1] * afExtent[1]
        + kBox.m_akAxis[2] * afExtent[2];
    pkPoints[6] = kBox.m_kCenter 
        - kBox.m_akAxis[0] * afExtent[0]
        + kBox.m_akAxis[1] * afExtent[1]
        + kBox.m_akAxis[2] * afExtent[2];
    pkPoints[7] = kBox.m_kCenter 
        - kBox.m_akAxis[0] * afExtent[0]
        - kBox.m_akAxis[1] * afExtent[1]
        + kBox.m_akAxis[2] * afExtent[2];

    unsigned short* pusConn = NiAlloc(unsigned short, 3 * uiNumTris);
    pusConn[0]  = 3;  pusConn[1]  = 2;  pusConn[2]  = 1;
    pusConn[3]  = 3;  pusConn[4]  = 1;  pusConn[5]  = 0;
    pusConn[6]  = 0;  pusConn[7]  = 1;  pusConn[8]  = 5;
    pusConn[9]  = 0;  pusConn[10] = 5;  pusConn[11] = 4;
    pusConn[12] = 4;  pusConn[13] = 5;  pusConn[14] = 6;
    pusConn[15] = 4;  pusConn[16] = 6;  pusConn[17] = 7;
    pusConn[18] = 7;  pusConn[19] = 6;  pusConn[20] = 2;
    pusConn[21] = 7;  pusConn[22] = 2;  pusConn[23] = 3;
    pusConn[24] = 1;  pusConn[25] = 2;  pusConn[26] = 6;
    pusConn[27] = 1;  pusConn[28] = 6;  pusConn[29] = 5;
    pusConn[30] = 0;  pusConn[31] = 4;  pusConn[32] = 7;
    pusConn[33] = 0;  pusConn[34] = 7;  pusConn[35] = 3;

    return NiNew NiTriShape(uiNumVerts, pkPoints, 0, 0, 0, 0, 
        NiGeometryData::NBT_METHOD_NONE, uiNumTris, pusConn);
}
//---------------------------------------------------------------------------
NiTriShape* NiDrawableBV::CreateFromCapsule(const NiCapsule& kCapsule, 
    const float fMult)
{
    // 1.05f makes wireframe visible over actual BV if fMult = 1.0
    float fMultExt = fMult * 1.05f;
    const float fRadius = kCapsule.m_fRadius * fMultExt;
    NiPoint3 kDirection = kCapsule.m_kSegment.m_kDirection;

    if (kDirection == NiPoint3::ZERO)
    {
        NiSphere kSphere;
        kSphere.m_kCenter = kCapsule.m_kSegment.m_kOrigin;
        kSphere.m_fRadius = fRadius;
        return CreateFromSphere(kSphere, fMult);
    }

    // compute unit-length kCapsule kAxis
    NiPoint3 kAxis = kDirection;
    kAxis.Unitize();

    // compute end points of kCapsule kAxis
    NiPoint3 kEndPt0 = kCapsule.m_kSegment.m_kOrigin
        - (kDirection + fRadius * kAxis) * (fMultExt - 1.0f);
    NiPoint3 kEndPt1 = kCapsule.m_kSegment.m_kOrigin 
        + kCapsule.m_kSegment.m_kDirection * fMultExt;

    // create two basis vectors that are perpendicular to the kCapsule kAxis
    NiPoint3 kBasis1;
    if (NiAbs(kAxis.x) >= NiAbs(kAxis.y) && NiAbs(kAxis.x) >= NiAbs(kAxis.z))
    {
        kBasis1.x = -kAxis.y;
        kBasis1.y = kAxis.x;
        kBasis1.z = kAxis.z;
    }
    else
    {
        kBasis1.x = kAxis.x;
        kBasis1.y = kAxis.z;
        kBasis1.z = -kAxis.y;
    }

    kBasis1.Unitize();
    NiPoint3 kBasis0 = kBasis1.Cross(kAxis);
    kBasis0 *= fRadius;
    kBasis1 *= fRadius;

    // cross section of kCapsule is 16-sided regular polygon
    const unsigned int uiNumSides = 16;
    unsigned int uiNumVerts = 4 * uiNumSides + 2;
    unsigned int uiNumTris = 8 * uiNumSides;
    NiPoint3* pkPoints = NiNew NiPoint3[uiNumVerts];
    unsigned short* pusConn = NiAlloc(unsigned short, uiNumTris * 3);

    pkPoints[uiNumVerts-2] = kEndPt0 - kAxis * fRadius;
    pkPoints[uiNumVerts-1] = kEndPt1 + kAxis * fRadius;

    float fCapScale = NiSqrt(0.5f);
    NiPoint3 kCap0 = kEndPt0 - kAxis * (fRadius * fCapScale);
    NiPoint3 kCap1 = kEndPt1 + kAxis * (fRadius * fCapScale);

    float fIncr = 2.0f * NI_PI / (float)uiNumSides;
    float fTheta = 0.0f;
    unsigned int i;
    for (i = 0; i < uiNumSides; i++)
    {
        NiPoint3 kOffset = kBasis0 * NiCos(fTheta) + kBasis1 * NiSin(fTheta);

        pkPoints[i] = kCap0 + kOffset * fCapScale;
        pkPoints[i + uiNumSides] = kEndPt0 + kOffset;
        pkPoints[i + uiNumSides * 2] = kEndPt1 + kOffset;
        pkPoints[i + uiNumSides * 3] = kCap1 + kOffset * fCapScale;

        fTheta += fIncr;
    }

    unsigned short* pusConnTmp = pusConn;
    unsigned int j;
    for (i = 0; i < uiNumSides-1; i++)
    {
        // side
        for (j = 0; j < 3; j++)
        {
            *pusConnTmp++ = i + uiNumSides * j;
            *pusConnTmp++ = i + 1 + uiNumSides * j;
            *pusConnTmp++ = i + 1 + uiNumSides * (j + 1);

            *pusConnTmp++ = i + uiNumSides * j;
            *pusConnTmp++ = i + 1 + uiNumSides * (j + 1);
            *pusConnTmp++ = i + uiNumSides * (j + 1);
        }

        // end caps
        *pusConnTmp++ = i + 1;
        *pusConnTmp++ = i;
        *pusConnTmp++ = uiNumVerts - 2;

        *pusConnTmp++ = i + uiNumSides * 3;
        *pusConnTmp++ = i + 1 + uiNumSides * 3;
        *pusConnTmp++ = uiNumVerts - 1;
    }

    for (j = 0; j < 3; j++)
    {
        *pusConnTmp++ = uiNumSides * (j + 1) - 1;
        *pusConnTmp++ = uiNumSides * j;
        *pusConnTmp++ = uiNumSides * (j + 1);

        *pusConnTmp++ = uiNumSides * (j + 1) - 1;
        *pusConnTmp++ = uiNumSides * (j + 1);
        *pusConnTmp++ = uiNumSides * (j + 2) - 1;
    }

    *pusConnTmp++ = 0;
    *pusConnTmp++ = uiNumSides - 1;
    *pusConnTmp++ = uiNumVerts - 2;

    *pusConnTmp++ = uiNumSides * 4 - 1;
    *pusConnTmp++ = uiNumSides * 3;
    *pusConnTmp++ = uiNumVerts - 1;

    return NiNew NiTriShape(uiNumVerts, pkPoints, 0, 0, 0, 0, 
        NiGeometryData::NBT_METHOD_NONE, uiNumTris, pusConn);
}
//---------------------------------------------------------------------------
NiTriShape* NiDrawableBV::CreateFromSphere(const NiSphere& kSphere, 
    const float fMult)
{
    // 1.05f makes wireframe visible over actual BV if fMult = 1.0
    float fMultExt = fMult * 1.05f;

    // create a kSphere by one subdivision of an icosahedron
    const float fGold = 0.5f * (NiSqrt(5.0f) + 1.0f);
    
    NiPoint3 kCenter = kSphere.m_kCenter;

    NiPoint3 akIcoVerts[12] =
    {
        NiPoint3(fGold,  1.0f,  0.0f), 
        NiPoint3(-fGold,  1.0f,  0.0f), 
        NiPoint3(fGold, -1.0f,  0.0f), 
        NiPoint3(-fGold, -1.0f,  0.0f), 
        NiPoint3(1.0f,  0.0f,  fGold), 
        NiPoint3(1.0f,  0.0f, -fGold), 
        NiPoint3(-1.0f,  0.0f,  fGold), 
        NiPoint3(-1.0f,  0.0f, -fGold), 
        NiPoint3(0.0f,  fGold,  1.0f), 
        NiPoint3(0.0f, -fGold,  1.0f), 
        NiPoint3(0.0f,  fGold, -1.0f), 
        NiPoint3(0.0f, -fGold, -1.0f)
    };

    const unsigned short ausIcoPolys[] =
    {
        0, 8, 4, 
        0, 5, 10, 
        2, 4, 9, 
        2, 11, 5, 
        1, 6, 8, 
        1, 10, 7, 
        3, 9, 6, 
        3, 7, 11, 
        0, 10, 8, 
        1, 8, 10, 
        2, 9, 11, 
        3, 11, 9, 
        4, 2, 0, 
        5, 0, 2, 
        6, 1, 3, 
        7, 3, 1, 
        8, 6, 4, 
        9, 4, 6, 
        10, 5, 7, 
        11, 7, 5
    };

    unsigned int uiNumVerts = 12;
    unsigned int uiNumTris = 20;
    unsigned int uiTotalVerts = 72;
    unsigned int uiTotalTris = 80;

    // Create an array to store all of the points and two arrays (one for
    // scratch space) to store the triangle connectivity.
    NiPoint3* pkPoints = NiNew NiPoint3[uiTotalVerts];
    unsigned int uiTotalTris_X_3 = uiTotalTris * 3;
    unsigned short* pusConn = NiAlloc(unsigned short, uiTotalTris_X_3);
    unsigned short* pusConnTmp = NiAlloc(unsigned short, uiTotalTris_X_3);
    unsigned int uiDestSize = uiTotalTris_X_3 * sizeof(*pusConn);

    // starting with an icosahedron, normalize the vertices
    unsigned int i;
    for(i = 0; i < uiNumVerts; i++)
    {
        akIcoVerts[i].Unitize();
        pkPoints[i] = akIcoVerts[i];
    }

    // start with the icosahedron connectivity, subdivide once
    NiMemcpy(pusConn, uiDestSize, ausIcoPolys, uiDestSize);

    // store traversal pointers for the connectivity arrays
    unsigned short* pusSrc = pusConn;
    unsigned short* pusDest = pusConnTmp;

    for (unsigned int t=0; t < uiNumTris; t++)
    {
        // get the indices to the triangle's vertices
        unsigned short v1 = *pusSrc++, v2 = *pusSrc++, v3 = *pusSrc++;

        // compute the three new verts as triangle edge midpoints and
        // normalize the points back onto the kSphere
        pkPoints[uiNumVerts] = pkPoints[v1] + pkPoints[v2];
        pkPoints[uiNumVerts].Unitize();
        pkPoints[uiNumVerts + 1] = pkPoints[v2] + pkPoints[v3];
        pkPoints[uiNumVerts + 1].Unitize();
        pkPoints[uiNumVerts + 2] = pkPoints[v3] + pkPoints[v1];
        pkPoints[uiNumVerts + 2].Unitize();

        // create 4 new triangles to retessellate the old triangle
        *pusDest++ = v1;
        *pusDest++ = uiNumVerts;
        *pusDest++ = uiNumVerts + 2;

        *pusDest++ = v2;
        *pusDest++ = uiNumVerts + 1;
        *pusDest++ = uiNumVerts;

        *pusDest++ = v3;
        *pusDest++ = uiNumVerts + 2;
        *pusDest++ = uiNumVerts + 1;

        *pusDest++ = uiNumVerts;
        *pusDest++ = uiNumVerts + 1;
        *pusDest++ = uiNumVerts + 2;

        // update the number of vertices
        uiNumVerts += 3;
    }

    // swap the two temporary connectivity arrays
    unsigned short* pusTmp = pusConn;
    pusConn = pusConnTmp;
    pusConnTmp = pusTmp;

    // update the number of tris
    uiNumTris *= 4;

    // scale the vertices and translate
    NiPoint3* pkTmpPoints = pkPoints;
    for (i = 0; i < uiNumVerts; i++)
    {
        *pkTmpPoints = (*pkTmpPoints) * kSphere.m_fRadius * fMultExt + 
            kCenter;
        pkTmpPoints++;
    }

    // free the setup arrays
    NiFree(pusConnTmp);

    return NiNew NiTriShape(uiNumVerts, pkPoints, 0, 0, 0, 0, 
        NiGeometryData::NBT_METHOD_NONE, uiNumTris, pusConn);
}
//---------------------------------------------------------------------------
void NiDrawableBV::CreateFromUnion(const NiUnionBV* pkUnion, 
    NiNode* pkParent, const float fMult)
{
    NIASSERT(pkParent);

    NiTriShape* pkShape;

    for (unsigned int i = 0; i < pkUnion->GetSize(); i++)
    {
        const NiBoundingVolume* pkBound = pkUnion->GetBoundingVolume(i);

        switch (pkBound->Type())
        {
        case NiBoundingVolume::BOX_BV:
            pkShape = CreateFromBox(((NiBoxBV*)pkBound)->GetBox(), fMult);
            pkParent->AttachChild(pkShape);
            break;
        case NiBoundingVolume::CAPSULE_BV:
            pkShape = CreateFromCapsule(
                ((NiCapsuleBV*)pkBound)->GetCapsule(), fMult);
            pkParent->AttachChild(pkShape);
            break;
        case NiBoundingVolume::SPHERE_BV:
            pkShape = CreateFromSphere(((NiSphereBV*)pkBound)->GetSphere(),
                fMult);
            pkParent->AttachChild(pkShape);
            break;
        case NiBoundingVolume::UNION_BV:
            {
                NiNode* pkNode = NiNew NiNode;
                CreateFromUnion((NiUnionBV*)pkBound, pkNode, fMult);
                pkParent->AttachChild(pkNode);
                break;
            }
        default:  // HALFSPACE_BV
            break;
        }
   }
}
//---------------------------------------------------------------------------
NiTriShape* NiDrawableBV::CreateTubeFromCapsule(const NiCapsule& kCapsule,
    const float fMult, const unsigned int uiNumSides)
{
    const float fRadius = kCapsule.m_fRadius * fMult;
    NiPoint3 kDirection = kCapsule.m_kSegment.m_kDirection;
    NiPoint3 kEndPt0 = kCapsule.m_kSegment.m_kOrigin;
    NiPoint3 kEndPt1 = kEndPt0 + kDirection;

    // compute unit-length kCapsule kAxis
    NiPoint3 kAxis = kDirection;
    kAxis.Unitize();

    // create two basis vectors that are perpendicular to the kCapsule kAxis
    NiPoint3 kBasis1;
    if (NiAbs(kAxis.x) >= NiAbs(kAxis.y) && NiAbs(kAxis.x) >= NiAbs(kAxis.z))
    {
        kBasis1.x = -kAxis.y;
        kBasis1.y = kAxis.x;
        kBasis1.z = kAxis.z;
    }
    else
    {
        kBasis1.x = kAxis.x;
        kBasis1.y = kAxis.z;
        kBasis1.z = -kAxis.y;
    }

    kBasis1.Unitize();
    NiPoint3 kBasis0 = kBasis1.Cross(kAxis);
    kBasis0 *= fRadius;
    kBasis1 *= fRadius;

    // cross section of kCylinder is uiNumSides-sided regular polygon
    unsigned int uiNumVerts = uiNumSides + uiNumSides;
    unsigned int uiNumTris = uiNumVerts;
    NiPoint3* pkPoints = NiNew NiPoint3[uiNumVerts];
    unsigned short* pusConn = NiAlloc(unsigned short, uiNumTris * 3);

    float fIncr = 2.0f * NI_PI / (float)uiNumSides;
    float fTheta = 0.0f;
    unsigned int i;
    for (i = 0; i < uiNumSides; i++)
    {
        NiPoint3 kOffset = kBasis0 * NiCos(fTheta) + kBasis1 * NiSin(fTheta);

        pkPoints[i] = kEndPt0 + kOffset;
        pkPoints[i + uiNumSides] = kEndPt1 + kOffset;

        fTheta += fIncr;
    }

    unsigned short* pusConnTmp = pusConn;
    for (i = 0; i < uiNumSides - 1; i++)
    {
        *pusConnTmp++ = i;
        *pusConnTmp++ = i + uiNumSides;
        *pusConnTmp++ = i + 1 + uiNumSides;

        *pusConnTmp++ = i;
        *pusConnTmp++ = i + 1 + uiNumSides;
        *pusConnTmp++ = i + 1;
    }

    *pusConnTmp++ = 0;
    *pusConnTmp++ = uiNumSides + uiNumSides - 1;
    *pusConnTmp++ = uiNumSides;

    *pusConnTmp++ = uiNumSides - 1;
    *pusConnTmp++ = 0;
    *pusConnTmp++ = uiNumSides + uiNumSides - 1;

    return NiNew NiTriShape(uiNumVerts, pkPoints, 0, 0, 0, 0, 
        NiGeometryData::NBT_METHOD_NONE, uiNumTris, pusConn);
}
//---------------------------------------------------------------------------
