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


#include "NiCollisionPCH.h" // Precompiled header.

#include "NiBoundingVolume.h"
#include "NiCollisionData.h"
#include "NiCollisionUtils.h"
#include <NiNode.h>
#include <NiTriBasedGeom.h>
#include "NiTriIntersect.h"

namespace NiCollisionUtils
{
//---------------------------------------------------------------------------
// Bounding volume intersection routines.
//---------------------------------------------------------------------------
bool BoundingVolumeTestIntersect(float fTime, NiAVObject* pkObj1, 
    NiAVObject* pkObj2)
{
    // NOTE:  User responsible for UpdateWorldData on all ABVs prior to call,
    // most likely by calling NiCollisionGroup::UpdateWorldABVs().
    
    NiCollisionData* pkData1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkData2 = NiGetCollisionData(pkObj2);

    if (!pkData1 || !pkData2)
        return false;

    if (pkData1->GetModelSpaceABV())
    {
        if (pkData2->GetModelSpaceABV())
        {
            // Both objects are using alternate bounding volumes.
            return NiBoundingVolume::TestIntersect(fTime, 
                *(pkData1->GetWorldSpaceABV()), pkData1->GetWorldVelocity(), 
                *(pkData2->GetWorldSpaceABV()), pkData2->GetWorldVelocity());
        }
        else
        {
            // Only object 1 is using an alternate bounding volume.
            return NiBoundingVolume::TestIntersect(fTime, 
                *(pkData1->GetWorldSpaceABV()), pkData1->GetWorldVelocity(), 
                *NiBoundingVolume::ConvertToSphereBV_Fast(
                pkObj2->GetWorldBound()), pkData2->GetWorldVelocity());
        }
    }
    else
    {
        if (pkData2->GetModelSpaceABV())
        {
            // Only object 2 is using an alternate bounding volume.
            return NiBoundingVolume::TestIntersect(fTime, 
                *NiBoundingVolume::ConvertToSphereBV_Fast(
                pkObj1->GetWorldBound()), pkData1->GetWorldVelocity(), 
                *(pkData2->GetWorldSpaceABV()), pkData2->GetWorldVelocity());
        }
        else
        {
            // Neither object is using an alternate bounding volume, so rely
            // on NiBound::TestIntersect.
            return NiBound::TestIntersect(fTime, 
                pkObj1->GetWorldBound(), pkData1->GetWorldVelocity(), 
                pkObj2->GetWorldBound(), pkData2->GetWorldVelocity());
        }
    }
}
//---------------------------------------------------------------------------
bool BoundingVolumeFindIntersect(float fTime, NiAVObject* pkObj1,
    NiAVObject* pkObj2, float& fIntrTime, NiPoint3& kIntrPoint, 
    bool bCalcNormals, NiPoint3& kNormal0, NiPoint3& kNormal1)
{
    // NOTE:  User responsible for UpdateWorldData on all ABVs prior to call,
    // most likely by calling NiCollisionGroup::UpdateWorldABVs().
    
    NiCollisionData* pkData1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkData2 = NiGetCollisionData(pkObj2);

    if (!pkData1 || !pkData2)
        return false;

    if (pkData1->GetModelSpaceABV())
    {
        if (pkData2->GetModelSpaceABV())
        {
            // Both objects are using alternate bounding volumes.
            return NiBoundingVolume::FindIntersect(fTime, 
                *(pkData1->GetWorldSpaceABV()), pkData1->GetWorldVelocity(),
                *(pkData2->GetWorldSpaceABV()), pkData2->GetWorldVelocity(),
                fIntrTime, kIntrPoint, bCalcNormals, kNormal0, kNormal1);
        }
        else
        {
            // Only object 1 is using an alternate bounding volume.
            return NiBoundingVolume::FindIntersect(fTime, 
                *(pkData1->GetWorldSpaceABV()), pkData1->GetWorldVelocity(),
                *NiBoundingVolume::ConvertToSphereBV_Fast(
                pkObj2->GetWorldBound()), pkData2->GetWorldVelocity(),
                fIntrTime, kIntrPoint, bCalcNormals, kNormal0, kNormal1);
        }
    }
    else
    {
        if (pkData2->GetModelSpaceABV())
        {
            // Only object 2 is using an alternate bounding volume.
            return NiBoundingVolume::FindIntersect(fTime, 
                *NiBoundingVolume::ConvertToSphereBV_Fast(
                pkObj1->GetWorldBound()), pkData1->GetWorldVelocity(), 
                *(pkData2->GetWorldSpaceABV()), pkData2->GetWorldVelocity(),
                fIntrTime, kIntrPoint, bCalcNormals, kNormal0, kNormal1);
        }
        else
        {
            // Neither object is using an alternate bounding volume, so rely
            // on NiBound::FindIntersect.
            return NiBound::FindIntersect(fTime, 
                pkObj1->GetWorldBound(), pkData1->GetWorldVelocity(), 
                pkObj2->GetWorldBound(), pkData2->GetWorldVelocity(), 
                fIntrTime, kIntrPoint, bCalcNormals, kNormal0, kNormal1);
        }
    }
}
//---------------------------------------------------------------------------
// Triangle-[triangle | bv] intersection routines.
//---------------------------------------------------------------------------
int TriTriTestIntersect(float fTime, NiAVObject* pkGeom1, 
    NiAVObject* pkGeom2, bool& bCollision)
{
    NiCollisionData* pkData1 = NiGetCollisionData(pkGeom1);
    NiCollisionData* pkData2 = NiGetCollisionData(pkGeom2);

    NIASSERT(pkData1 && pkData2);

    if (!pkData1 || !pkData2)
        return NiCollisionGroup::CONTINUE_COLLISIONS;

    unsigned short usTrianglesI = pkData1->GetTriangleCount();
    unsigned short usTrianglesJ = pkData2->GetTriangleCount();
    
    for (unsigned short i = 0; i < usTrianglesI; i++)
    {
        for (unsigned short j = 0; j < usTrianglesJ; j++)
        {
            NiPoint3* apkP[3];
            if (!pkData1->GetWorldTriangle(i, apkP[0], apkP[1], apkP[2]))
                continue;

            NiPoint3* apkQ[3];
            if (!pkData2->GetWorldTriangle(j, apkQ[0], apkQ[1], apkQ[2]))
                continue;

            NiTriIntersect kTI(apkP, pkData1->GetWorldVelocity(), apkQ, 
                pkData2->GetWorldVelocity());

            if (kTI.Test(fTime))
            {
                bCollision = true;
                return NiCollisionGroup::CONTINUE_COLLISIONS;
            }
        }
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int TriTriFindIntersect(float fTime, NiAVObject* pkGeom1, NiAVObject* pkGeom2,
    bool bCalcNormals, NiCollisionGroup::Intersect& kIntr, bool& bCollision)
{
    NiCollisionData* pkData1 = NiGetCollisionData(pkGeom1);
    NiCollisionData* pkData2 = NiGetCollisionData(pkGeom2);

    NIASSERT(pkData1 && pkData2);

    if (!pkData1 || !pkData2)
        return NiCollisionGroup::CONTINUE_COLLISIONS;

    unsigned short usTrianglesI = pkData1->GetTriangleCount();
    unsigned short usTrianglesJ = pkData2->GetTriangleCount();
 
    for (unsigned short i = 0; i < usTrianglesI; i++)
    {     
        for (unsigned short j = 0; j < usTrianglesJ; j++)
        {
            NiPoint3* apkP[3];
            if (!pkData1->GetWorldTriangle(i, apkP[0], apkP[1], apkP[2]))
                continue;
            
            NiPoint3* apkQ[3];
            if (!pkData2->GetWorldTriangle(j, apkQ[0], apkQ[1], apkQ[2]))
                continue;
            
            NiTriIntersect kTI(apkP, pkData1->GetWorldVelocity(), apkQ, 
                pkData2->GetWorldVelocity());

            if (kTI.Find(fTime, kIntr.fTime, kIntr.kPoint, bCalcNormals, 
                &kIntr.kNormal0, &kIntr.kNormal1))
            {
                kIntr.appkTri1 = (const NiPoint3**)&apkP[0];
                kIntr.appkTri2 = (const NiPoint3**)&apkQ[0];

                bCollision = true;

                int iRet = pkData1->FindCollisionProcessing(kIntr,
                    pkData1->GetPropagationMode(),
                    pkData2->GetPropagationMode());

                if (iRet == NiCollisionGroup::TERMINATE_COLLISIONS)
                    return NiCollisionGroup::TERMINATE_COLLISIONS;

                if (iRet == NiCollisionGroup::BREAKOUT_COLLISIONS)
                    return NiCollisionGroup::CONTINUE_COLLISIONS;
            }
        }
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
bool TriToBndVolTestIntersect(float fTime, NiAVObject* pkTBG,
    NiAVObject* pkOther, bool& bCollision)
{
    NiCollisionData* pkData1 = NiGetCollisionData(pkTBG);
    NiCollisionData* pkData2 = NiGetCollisionData(pkOther);

    NIASSERT(pkData1 && pkData2);

    NiCollisionData* pkTriData;
    NiCollisionData* pkABVData;

    NiTransform kTrans;

    NiCollisionData::CollisionMode eMode1 = pkData1->GetCollisionMode();

    if (eMode1 == NiCollisionData::USE_TRI ||
        eMode1 == NiCollisionData::USE_OBB)
    {
        pkTriData = pkData1;
        pkABVData = pkData2;
        kTrans = pkOther->GetWorldTransform();
    }
    else    // USE_ABV or USE_NIBOUND
    {
        pkTriData = pkData2;
        pkABVData = pkData1;
        kTrans = pkTBG->GetWorldTransform();
    }
  
    if (!pkABVData->GetModelSpaceABV())
        return NiCollisionGroup::CONTINUE_COLLISIONS;

    if (pkTriData->GetWorldVertices() == NULL)
    {
        pkTriData->CreateWorldVertices();
    }
    pkTriData->UpdateWorldVertices();

    // 'Other' is using an alternate bounding volume.
    NiBoundingVolume* pkB2 = pkABVData->GetWorldSpaceABV();
    NIASSERT(pkB2);
    pkB2->UpdateWorldData(*pkABVData->GetModelSpaceABV(), kTrans);
   
    unsigned short usTriangles = pkTriData->GetTriangleCount();

    for (unsigned short i = 0; i < usTriangles; i++)
    {
        NiPoint3* apkVerts[3];
        if (!pkTriData->GetWorldTriangle(i,
            apkVerts[0], apkVerts[1], apkVerts[2]))
        {
            continue;
        }
       
        if (NiBoundingVolume::TestTriIntersect(fTime, *pkB2, 
            pkABVData->GetWorldVelocity(), 
            *apkVerts[0], *apkVerts[1], *apkVerts[2], 
            pkTriData->GetWorldVelocity()))
        {
            bCollision = true;

            int iRet = pkTriData->TestCollisionProcessing(
                pkTriData->GetPropagationMode(),
                pkABVData->GetPropagationMode());

            if (iRet == NiCollisionGroup::TERMINATE_COLLISIONS)
                return NiCollisionGroup::TERMINATE_COLLISIONS;
        }
    }
    
    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int TriToBndVolFindIntersect(float fTime, NiAVObject* pkTBG,
    NiAVObject* pkOther, bool bCalcNormals,
    NiCollisionGroup::Intersect& kIntr, bool &bCollision)
{
    NiCollisionData* pkData1 = NiGetCollisionData(pkTBG);
    NiCollisionData* pkData2 = NiGetCollisionData(pkOther);

    NIASSERT(pkData1 && pkData2);

    NiCollisionData* pkTriData;
    NiCollisionData* pkABVData;

    NiTransform kTrans;

    NiCollisionData::CollisionMode eMode1 = pkData1->GetCollisionMode();

    if (eMode1 == NiCollisionData::USE_TRI ||
        eMode1 == NiCollisionData::USE_OBB)
    {
        pkTriData = pkData1;
        pkABVData = pkData2;
        kTrans = pkOther->GetWorldTransform();
    }
    else    // USE_ABV or USE_NIBOUND
    {
        pkTriData = pkData2;
        pkABVData = pkData1;
        kTrans = pkTBG->GetWorldTransform();
    }
  
    if (!pkABVData->GetModelSpaceABV())
        return NiCollisionGroup::CONTINUE_COLLISIONS;

    if (pkTriData->GetWorldVertices() == NULL)
    {
        pkTriData->CreateWorldVertices();
    }
    pkTriData->UpdateWorldVertices();

    // 'Other' is using an alternate bounding volume.
    NiBoundingVolume* pkB2 = pkABVData->GetWorldSpaceABV();
    NIASSERT(pkB2);
    pkB2->UpdateWorldData(*pkABVData->GetModelSpaceABV(), kTrans);

    unsigned short usTriangles = pkTriData->GetTriangleCount();

    for (unsigned short i = 0; i < usTriangles; i++)
    {
        NiPoint3* apkVerts[3];
        if (!pkTriData->GetWorldTriangle(i, 
            apkVerts[0], apkVerts[1], apkVerts[2]))
        {
            continue;
        }

        if (NiBoundingVolume::FindTriIntersect(fTime, *pkB2, 
             pkABVData->GetWorldVelocity(), 
             *apkVerts[0], *apkVerts[1], *apkVerts[2], 
             pkTriData->GetWorldVelocity(), kIntr.fTime, kIntr.kPoint, 
             bCalcNormals, kIntr.kNormal0, kIntr.kNormal1))
        {
            if (pkTriData == pkData1)
                kIntr.appkTri1 = (const NiPoint3**)&apkVerts[0];
            else
                kIntr.appkTri2 = (const NiPoint3**)&apkVerts[0];

            bCollision = true;

            int iRet = pkTriData->FindCollisionProcessing(kIntr,
                pkTriData->GetPropagationMode(),
                pkABVData->GetPropagationMode());

            if (iRet == NiCollisionGroup::TERMINATE_COLLISIONS)
                return NiCollisionGroup::TERMINATE_COLLISIONS;

            if (iRet == NiCollisionGroup::BREAKOUT_COLLISIONS)
                return NiCollisionGroup::CONTINUE_COLLISIONS;
        }
    }
    return NiCollisionGroup::CONTINUE_COLLISIONS;
}

//---------------------------------------------------------------------------
// picking intersection routines
//---------------------------------------------------------------------------
bool FindBoundIntersect(NiAVObject* pkObj, const NiPoint3& kOrigin, 
    const NiPoint3& kDir)
{
    // Determine if ray intersects bounding sphere of object. If sphere is
    // (X-C)*(X-C) = R^2 and ray is X = t*D+L for t >= 0, then intersection
    // is obtained by plugging X into sphere equation to get quadratic
    // (D*D)t^2 + 2*(D*(L-C))t + (L-C)*(L-C)-R^2 = 0.  Define a = D*D, 
    // b = D*(L-C), and c = (L-C)*(L-C)-R^2.  Intersection occurs whenever
    // b*b >= a*c (quadratic has at least one real root).  This is clearly
    // true if c <= 0, so that test is made for a quick out.  If c > 0 and
    // b >= 0, then the sphere center is "behind" the ray (so to speak) and
    // the sphere cannot intersect the ray. Otherwise, b*b >= a*c is checked.

    NiBound kWorld = pkObj->GetWorldBound();
    NiPoint3 kDiff = kOrigin - kWorld.GetCenter();
    float fRadius = kWorld.GetRadius();
    float c = kDiff * kDiff - fRadius * fRadius;
    if (c <= 0.0f)
        return true;

    float b = kDir * kDiff;
    if (b >= 0.0f)
        return false;

    float a = kDir * kDir;
    return (b * b >= a * c);
}
//---------------------------------------------------------------------------

} // namespace
