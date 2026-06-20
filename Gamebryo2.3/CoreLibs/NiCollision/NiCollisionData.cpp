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


#include "NiCollisionPCH.h" // Precompiled header

#include <NiTriShape.h>
#include <NiTriStrips.h>
#include "NiBoundingVolume.h"
#include "NiCollisionData.h"
#include "NiOBBRoot.h"

NiImplementRTTI(NiCollisionData, NiCollisionObject);

// Disable world velocity computing by default
bool NiCollisionData::ms_bComputeWorldVelocity = false;

//---------------------------------------------------------------------------
NiCollisionData::NiCollisionData(NiAVObject* pkSceneObject) :
    NiCollisionObject(pkSceneObject)
{
    m_pkWorldVertex = 0;
    m_pkWorldNormal = 0;
    m_bWorldVerticesNeedUpdate = false;
    m_bWorldNormalsNeedUpdate = false;

    m_kLocalVelocity = NiPoint3::ZERO;
    m_kWorldVelocity = NiPoint3::ZERO;

    m_ePropagationMode = PROPAGATE_ALWAYS;
    m_eCollisionMode = NOTEST;

    m_pkModelABV = 0;
    m_pkWorldABV = 0;

    m_pkOBBRoot = 0;

    m_usNumTriangles = 0;
    
    m_pfnCollideCallback = 0;
    m_pvCollideCallbackData = 0;
    m_bAuxCallbacks = false;
}
//---------------------------------------------------------------------------
// This constructor is protected.
NiCollisionData::NiCollisionData() : NiCollisionObject(0)
{
    m_pkWorldVertex = 0;
    m_pkWorldNormal = 0;
    m_bWorldVerticesNeedUpdate = false;
    m_bWorldNormalsNeedUpdate = false;

    m_kLocalVelocity = NiPoint3::ZERO;
    m_kWorldVelocity = NiPoint3::ZERO;

    m_ePropagationMode = PROPAGATE_ALWAYS;
    m_eCollisionMode = NOTEST;

    m_pkModelABV = 0;
    m_pkWorldABV = 0;

    m_pkOBBRoot = 0;

    m_usNumTriangles = 0;
    
    m_pfnCollideCallback = 0;
    m_pvCollideCallbackData = 0;
    m_bAuxCallbacks = false;
}
//---------------------------------------------------------------------------
NiCollisionData::~NiCollisionData()
{    
    NiDelete[] m_pkWorldVertex;
    NiDelete[] m_pkWorldNormal;
    NiDelete m_pkModelABV;
    NiDelete m_pkWorldABV;
    DestroyOBB();
}
//---------------------------------------------------------------------------
void NiCollisionData::SetSceneGraphObject(NiAVObject* pkSceneObject)
{
    NiCollisionObject::SetSceneGraphObject(pkSceneObject);
    SetPrecacheAttributes(m_pkSceneObject, m_eCollisionMode);
}

//---------------------------------------------------------------------------
NiNode* NiCollisionData::FindAncestorWithCollisionData(NiAVObject* pkObj)
{
    if (pkObj == NULL)
        return NULL; 

    NiNode* pkParent = pkObj->GetParent();

    if (pkParent == NULL)
        return NULL;

    NiCollisionData* pkData = NiGetCollisionData(pkParent);

    if (pkData)
        return pkParent;

    return FindAncestorWithCollisionData(pkParent);
}
//---------------------------------------------------------------------------
void NiCollisionData::UpdateWorldData()
{   
    if (m_pkModelABV)
    {
        m_pkWorldABV->UpdateWorldData(*m_pkModelABV, 
            m_pkSceneObject->GetWorldTransform());
    }

    if (ms_bComputeWorldVelocity)
    {
        NiAVObject* pkParent = m_pkSceneObject->GetParent();

        if (pkParent)
        {
            NiNode* pkAncestorWithCD
                = FindAncestorWithCollisionData(m_pkSceneObject);

            NiCollisionData* pkParentData = NULL;

            if (pkAncestorWithCD)
                pkParentData = NiGetCollisionData(pkAncestorWithCD);
          
            if (pkParentData)
            {
                m_kWorldVelocity = pkParent->GetWorldRotate() 
                    * m_kLocalVelocity + pkParentData->GetWorldVelocity();
            }
            else
            {
                m_kWorldVelocity = pkParent->GetWorldRotate() 
                    * m_kLocalVelocity;
            }
        }
        else
        {
            m_kWorldVelocity = m_kLocalVelocity;
        }
    }

    MarkVerticesAsChanged();
    MarkNormalsAsChanged();
}
//---------------------------------------------------------------------------
void NiCollisionData::RecreateWorldData()
{
}
//---------------------------------------------------------------------------
void NiCollisionData::CreateOBB(int iBinSize)
{
    if (NiIsKindOf(NiTriShape, m_pkSceneObject))
    {
        if (!m_pkOBBRoot)
        {
            if (m_pkWorldVertex == NULL)
            {
                CreateWorldVertices();
                UpdateWorldVertices();
            }

            NiTriShape* pkShape = (NiTriShape*)m_pkSceneObject;

            m_pkOBBRoot = NiNew NiOBBRoot(pkShape->GetTriangleCount(),
                pkShape->GetTriList(), pkShape->GetVertices(), 
                m_pkWorldVertex, iBinSize);
            NIASSERT(m_pkOBBRoot);
        }
    }
    else if (NiIsKindOf(NiTriStrips, m_pkSceneObject))
    {
        if (!m_pkOBBRoot)
        {
            if (m_pkWorldVertex == NULL)
            {
                CreateWorldVertices();
                UpdateWorldVertices();
            }

            NiTriStrips* pkStrips = (NiTriStrips*)m_pkSceneObject;

            // NiOBBRoot constructor requires a TriShape style connectivity 
            // list, so create a temporary one.

            unsigned int i, j;
            unsigned int uiUsedTriangles = 0;
            unsigned int uiTriangles = pkStrips->GetTriangleCount();

            unsigned short* pTriList = NiAlloc(unsigned short,
                3 * uiTriangles);
            NIASSERT(pTriList != NULL);

            for (i = j = 0; i < uiTriangles; i++)
            {
                pkStrips->GetTriangleIndices(i, 
                    pTriList[j], pTriList[j + 1], pTriList[j + 2]);

                if (pTriList[j] == pTriList[j + 1] ||
                    pTriList[j] == pTriList[j + 2] ||
                    pTriList[j + 1] == pTriList[j + 2])
                    continue;

                j += 3;
                uiUsedTriangles++;
            }

            m_pkOBBRoot = NiNew NiOBBRoot(uiUsedTriangles, pTriList, 
                pkStrips->GetVertices(), m_pkWorldVertex, iBinSize);
            NIASSERT(m_pkOBBRoot);

            NiFree(pTriList);
        }
    }
}
//---------------------------------------------------------------------------
void NiCollisionData::DestroyOBB()
{
    NiDelete m_pkOBBRoot;
    m_pkOBBRoot = 0;
}
//---------------------------------------------------------------------------
bool NiCollisionData::TestOBBCollisions(float fDeltaTime,
    NiCollisionData* pkTestData, NiAVObject* pThisObj, NiAVObject* pOtherObj,
    int iMaxDepth, int iTestMaxDepth)
{
    m_pkOBBRoot->SetDeltaTime(fDeltaTime);
    m_pkOBBRoot->SetVelocity(m_kWorldVelocity);
    m_pkOBBRoot->Transform(m_pkSceneObject->GetWorldTransform());

    pkTestData->m_pkOBBRoot->SetDeltaTime(fDeltaTime);
    pkTestData->m_pkOBBRoot->SetVelocity(pkTestData->m_kWorldVelocity);
    pkTestData->m_pkOBBRoot->Transform(pOtherObj->GetWorldTransform());

    return m_pkOBBRoot->TestCollision(pkTestData->m_pkOBBRoot, pThisObj, 
        pOtherObj, iMaxDepth, iTestMaxDepth);
}
//---------------------------------------------------------------------------
int NiCollisionData::FindOBBCollisions(float fDeltaTime,
    NiCollisionData* pkTestData, NiAVObject* pThisRoot,
    NiAVObject* pOtherRoot, NiAVObject* pThisObj, NiAVObject* pOtherObj,
    int iMaxDepth, int iTestMaxDepth, bool& bCollision)
{
    m_pkOBBRoot->SetDeltaTime(fDeltaTime);
    m_pkOBBRoot->SetVelocity(m_kWorldVelocity);
    m_pkOBBRoot->Transform(m_pkSceneObject->GetWorldTransform());

    pkTestData->m_pkOBBRoot->SetDeltaTime(fDeltaTime);
    pkTestData->m_pkOBBRoot->SetVelocity(pkTestData->m_kWorldVelocity);
    pkTestData->m_pkOBBRoot->Transform(pOtherObj->GetWorldTransform());

    return m_pkOBBRoot->FindCollisions(pkTestData->m_pkOBBRoot, pThisRoot,
        pOtherRoot, pThisObj, pOtherObj, iMaxDepth, iTestMaxDepth,
        bCollision);
}
//---------------------------------------------------------------------------
void NiCollisionData::SetModelSpaceABV(NiBoundingVolume* pkBV)
{
    // Remove any existing model and world ABVs
    NiDelete m_pkModelABV;
    NiDelete m_pkWorldABV;

    // NiCollisionData takes ownership of the input bounding volume
    m_pkModelABV = pkBV;

    if (m_pkModelABV)  
    {
        // Create world bounding volume 
        m_pkWorldABV = m_pkModelABV->Create();

        // m_pkSceneObject may not exist while cloning, so check for it.
        if (m_pkSceneObject)
        {
            // Make it current, since collidees might not get updated again.
            m_pkWorldABV->UpdateWorldData(*m_pkModelABV, 
                m_pkSceneObject->GetWorldTransform());
        }
    }
    else
    {
        m_pkWorldABV = NULL;
    }
}
//----------------------------------------------------------------------------
// This "internal" member function exists primarily to simplify maintenance of
// propagation flags, in that it mirrors the functionality of the
// FindCollisionProcessing() member function which follows it.
//
// Here's the behavior for the propagation flags:
//
//  -  PROPAGATE_NEVER represets the case that a "real" collision is found.
//     No more collision detection work needs to be done for TestCollisions,
//     since it would not change the result, so we want to
//     TERMINATE_COLLISIONS to save unnecessary work for TestCollision.
//     (In the case of FindCollisions, the callback can choose whether to
//     CONTINUE_COLLISIONS (likely) or TERMINATE_COLLISIONS.)
//
//  -  PROPAGATE_ALWAYS represents another case that a "real" collision is
//     found.  Typically, this flag is used on the limbs of a character, where
//     we are actually interested in the collision result at a node as well as
//     wanting to propagate further down the hierarchy.  Thus, we want no
//     culling to be done in this case.  Still, no more work needs to be done
//     for TestCollisions, since one collision is enough to answer whether a
//     collision is found.  So, we TERMINATE_COLLISIONS.
//     (In the case of FindCollisions, the callback can choose whether to
//     CONTINUE_COLLISIONS (likely) or TERMINATE_COLLISIONS.)
//
//  -  PROPAGATE_ON_SUCCESS functions as a collision culling mechanism:  When
//     a collision is found, by default, this propagation flag tells the
//     collision system to "pass on" responsibility for testing collisions
//     further down the hierarchy.  In this case, we must CONTINUE_COLLISIONS
//     until a "real", non-culling collision is found.
//
//  -  (PROPAGATE_ON_FAILURE is typically not used.)
//
// (For auxiliary callbacks (a diagnostic tool), PROPAGATE_ON_SUCCESS is
// treated as a "real collision", so we TERMINATE_COLLISIONS in that case.)

int NiCollisionData::TestCollisionProcessing(PropagationMode ePropagationMode1,
    PropagationMode ePropagationMode2)
{
    bool bExecCallback = true;

    if (m_bAuxCallbacks == false)
    {
        if ((ePropagationMode1 == PROPAGATE_ON_SUCCESS ||
             ePropagationMode1 == PROPAGATE_ON_FAILURE) ||
            (ePropagationMode2 == PROPAGATE_ON_SUCCESS ||
             ePropagationMode2 == PROPAGATE_ON_FAILURE))
             bExecCallback = false;
    }

    if (bExecCallback)
    {
        return NiCollisionGroup::TERMINATE_COLLISIONS;
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//----------------------------------------------------------------------------
// This "internal" member function manages propagation flags and conditionally
// calls a FindCollision callback, to handle actual collision processing.
//
// Here's the behavior for the propagation flags:
//
//  -  PROPAGATE_NEVER represets the case that a "real" collision is found.
//     In the case of FindCollisions, the callback can choose whether or not
//     to TERMINATE_COLLISIONS.  However, the user will usually want to
//     CONTINUE_COLLISIONS, in order to return a detected collision if one
//     exists.
//
//  -  PROPAGATE_ALWAYS represents another case that a "real" collision is
//     found.  Typically, this flag is used on the limbs of a character, where
//     we are actually interested in the collision result at a node as well as
//     wanting to propagate further down the hierarchy.  Thus, we want no
//     culling to be done in this case.
//     In the case of FindCollisions, the callback can choose whether or not
//     to TERMINATE_COLLISIONS.  However, the user will usually want to
//     CONTINUE_COLLISIONS, in order to return a detected collision if one
//     exists.
//
//  -  PROPAGATE_ON_SUCCESS functions as a collision culling mechanism:  When
//     a collision is found, by default, this propagation flag tells the
//     collision system to "pass on" responsibility for testing collisions
//     further down the hierarchy.  In this case, we must CONTINUE_COLLISIONS
//     until a "real", non-culling collision is found, and the FindCollision
//     callback is not called.
//
//  -  (PROPAGATE_ON_FAILURE is typically not used.)
//
// (For auxiliary callbacks (a diagnostic tool), PROPAGATE_ON_SUCCESS is
// treated as a "real collision".)

int NiCollisionData::FindCollisionProcessing(
    NiCollisionGroup::Intersect& kIntr, PropagationMode ePropagationMode1,
    PropagationMode ePropagationMode2)
{
    int iRet0 = NiCollisionGroup::CONTINUE_COLLISIONS;
    int iRet1 = NiCollisionGroup::CONTINUE_COLLISIONS;

    bool bExecCallback = true;

    if (m_bAuxCallbacks == false)
    {
        if ((ePropagationMode1 == PROPAGATE_ON_SUCCESS ||
             ePropagationMode1 == PROPAGATE_ON_FAILURE) ||
            (ePropagationMode2 == PROPAGATE_ON_SUCCESS ||
             ePropagationMode2 == PROPAGATE_ON_FAILURE))
             bExecCallback = false;
    }

    if (bExecCallback && kIntr.pkObj0)
    {
        NiCollisionData* pkData = NiGetCollisionData(kIntr.pkObj0);

        if (pkData)
        {
            NiCollisionGroup::Callback pfnCB0 = pkData->GetCollideCallback();

            if (pfnCB0)
                iRet0 = pfnCB0(kIntr);
            else
            {
                // Fall back to root's callback.
                if (kIntr.pkRoot0)
                {
                    NiCollisionData* pkData = NiGetCollisionData(
                        kIntr.pkRoot0);

                    if (pkData)
                    {
                        NiCollisionGroup::Callback pfnCB0 =
                            pkData->GetCollideCallback();

                        if (pfnCB0)
                            iRet0 = pfnCB0(kIntr);
                    }
                }
            }
        }
    }
           
    // Swap so that 0 object is always the calling callback.
    NiCollisionGroup::Intersect kIntrTmp = kIntr;
    kIntr.appkTri1 = kIntrTmp.appkTri2;
    kIntr.appkTri2 = kIntrTmp.appkTri1;
    kIntr.kNormal0 = kIntrTmp.kNormal1;
    kIntr.kNormal1 = kIntrTmp.kNormal0;
    kIntr.pkObj0 = kIntrTmp.pkObj1;
    kIntr.pkObj1 = kIntrTmp.pkObj0;
    kIntr.pkRoot0 = kIntrTmp.pkRoot1;
    kIntr.pkRoot1 = kIntrTmp.pkRoot0;

    if (bExecCallback && kIntr.pkObj0)
    {
        NiCollisionData* pkData = NiGetCollisionData(kIntr.pkObj0);

        if (pkData)
        {
            // Swap so that 0 object is the calling callback.
            NiCollisionGroup::Callback pfnCb1 = pkData->GetCollideCallback();

            if (pfnCb1)
            {
                iRet1 = pfnCb1(kIntr);
            }
            else
            {
                // Fall back to root's call back
                if (kIntr.pkRoot0)
                {
                    NiCollisionData* pkData
                        = NiGetCollisionData(kIntr.pkRoot0);

                    if (pkData)
                    {
                        NiCollisionGroup::Callback pfnCB0 
                            = pkData->GetCollideCallback();

                        if (pfnCB0)
                            iRet1 = pfnCB0(kIntr);
                    }
                }
            }
        }
    }
    // Return to original values.
    kIntr.appkTri1 = kIntrTmp.appkTri1;
    kIntr.appkTri2 = kIntrTmp.appkTri2;
    kIntr.kNormal0 = kIntrTmp.kNormal0;
    kIntr.kNormal1 = kIntrTmp.kNormal1;
    kIntr.pkObj0 = kIntrTmp.pkObj0;
    kIntr.pkObj1 = kIntrTmp.pkObj1;
    kIntr.pkRoot0 = kIntrTmp.pkRoot0;
    kIntr.pkRoot1 = kIntrTmp.pkRoot1;

    if (iRet0 == NiCollisionGroup::CONTINUE_COLLISIONS && 
        iRet1 == NiCollisionGroup::CONTINUE_COLLISIONS)
    {
        return NiCollisionGroup::CONTINUE_COLLISIONS;
    }
    else if (iRet0 == NiCollisionGroup::TERMINATE_COLLISIONS ||
        iRet1 == NiCollisionGroup::TERMINATE_COLLISIONS)
    {
        return NiCollisionGroup::TERMINATE_COLLISIONS;
    }
    else
    {
        return NiCollisionGroup::BREAKOUT_COLLISIONS;
    }
}
//---------------------------------------------------------------------------
void NiCollisionData::CreateWorldVertices()
{
    NiTriBasedGeom* pkGeom = NiDynamicCast(NiTriBasedGeom, m_pkSceneObject);
    if (!pkGeom)
        return;

    if (!m_pkWorldVertex && pkGeom->GetVertexCount())
    {
        m_usNumVertices = pkGeom->GetVertexCount();
        m_pkWorldVertex = NiNew NiPoint3[m_usNumVertices];
        m_bWorldVerticesNeedUpdate = true;
        m_usNumTriangles = pkGeom->GetTriangleCount();
    }
}
//---------------------------------------------------------------------------
void NiCollisionData::UpdateWorldVertices()
{
    NiGeometry* pkGeom = NiDynamicCast(NiGeometry, m_pkSceneObject);
    if (!pkGeom)
        return;

    if (m_pkWorldVertex && m_bWorldVerticesNeedUpdate)
    {
        NiProcessorSpecificCode::TransformPoints(
            pkGeom->GetActiveVertexCount(),
            (const float*)pkGeom->GetVertices(),
            (float*)m_pkWorldVertex, &(pkGeom->GetWorldTransform()));

        m_bWorldVerticesNeedUpdate = false;
    }
}
//---------------------------------------------------------------------------
void NiCollisionData::DestroyWorldVertices()
{
    NiDelete[] m_pkWorldVertex;
    m_pkWorldVertex = 0;
    m_bWorldVerticesNeedUpdate = false;
}
//---------------------------------------------------------------------------
void NiCollisionData::CreateWorldNormals()
{
    NiGeometry* pkGeom = NiDynamicCast(NiGeometry, m_pkSceneObject);
    if (!pkGeom)
        return;

    if (!m_pkWorldNormal && pkGeom->GetVertexCount() && pkGeom->GetNormals())
    {
        m_pkWorldNormal = NiNew NiPoint3[pkGeom->GetVertexCount()];
        m_bWorldNormalsNeedUpdate = true;
    }
}
//---------------------------------------------------------------------------
void NiCollisionData::UpdateWorldNormals()
{
    NiGeometry* pkGeom = NiDynamicCast(NiGeometry, m_pkSceneObject);
    if (!pkGeom)
        return;

    if (m_pkWorldNormal && m_bWorldNormalsNeedUpdate)
    {
        NiProcessorSpecificCode::TransformVectors(
            pkGeom->GetActiveVertexCount(),
            (const float*)pkGeom->GetNormals(), (float*)m_pkWorldNormal,
            (NiMatrix3*)&(pkGeom->GetWorldRotate()));

        m_bWorldNormalsNeedUpdate = false;
    }
}
//---------------------------------------------------------------------------
void NiCollisionData::DestroyWorldNormals()
{
    NiDelete[] m_pkWorldNormal;
    m_pkWorldNormal = 0;
    m_bWorldNormalsNeedUpdate = false;
}
//---------------------------------------------------------------------------
void NiCollisionData::Initialize(void *pkData)
{
    // Initialize in context of NiCollisionData is used only to convert
    // older Nif files.

    // Data is interpreted as (this, propagatemode, bool, kstream).
    size_t* puiArray = (size_t*)pkData;
    SetSceneGraphObject((NiAVObject*)puiArray[0]);

    // The m_ePropagateMode member no longer exist as it's functionality has
    // been split into 2 separate members:
    //   PropagationMode m_ePropagationMode;
    //   CollisionMode m_eCollisionMode;
    // for clarity and robustness. 
    // Hence, we store the value of the obsolete m_ePropagateMode member into
    // the new m_ePropagationMode member.  However, this value will need to be
    // "converted" to make since with the new system.  This step is done in
    // the convert function.
    SetPropagationMode((PropagationMode)puiArray[1]); 

    unsigned int uiCreateABVFromStream = puiArray[2];

    if (uiCreateABVFromStream)
    {
        NiBoundingVolume* pkBV 
            = NiBoundingVolume::CreateFromStream(*(NiStream*)puiArray[3]);

        SetModelSpaceABV(pkBV);
    }
}
//---------------------------------------------------------------------------
bool NiCollisionData::GetWorldTriangle( unsigned short usTriangle, 
    NiPoint3*& pkP0, NiPoint3*& pkP1, NiPoint3*& pkP2)
{
    NiTriBasedGeom* pkGeom = (NiTriBasedGeom*)m_pkSceneObject;
    NIASSERT(NiIsKindOf(NiTriBasedGeom, pkGeom));

    NIASSERT(usTriangle < pkGeom->GetTriangleCount());
    
    if (m_pkWorldVertex)
    {
        unsigned short i0, i1, i2;
        
        pkGeom->GetTriangleIndices(usTriangle, i0, i1, i2);

        if (i0 != i1 && i0 != i2 && i1 != i2)
        {
            pkP0 = &m_pkWorldVertex[i0];
            pkP1 = &m_pkWorldVertex[i1];
            pkP2 = &m_pkWorldVertex[i2];
            return true;
        }
    }

    pkP0 = 0;
    pkP1 = 0;
    pkP2 = 0;

    return false;
}
//---------------------------------------------------------------------------
void NiCollisionData::RemapOldCollisionMode()
{
    unsigned int uiOldMode = m_eCollisionMode;
    switch(uiOldMode)
    {
    case 0:
        m_eCollisionMode = USE_OBB;
        break;
    case 1:
        m_eCollisionMode = USE_TRI;
        break;
    case 2:
        m_eCollisionMode = USE_ABV;
        break;
    case 3:
        m_eCollisionMode = NOTEST;
        break;
    case 4:
        m_eCollisionMode = USE_NIBOUND;
        break;
    default:
        // We should never hit this assert since it means we've streamed in
        // an old collision mode that didn't exist.
        NIASSERT(false);
        m_eCollisionMode = NOTEST;
        break;
    };
}
//---------------------------------------------------------------------------
void NiCollisionData::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiCollisionObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiCollisionData::ms_RTTI.GetName()));

    AddViewerStrings(pkStrings);
}
//---------------------------------------------------------------------------
void NiCollisionData::AddViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiCollisionObject::AddViewerStrings(pkStrings);
    
    pkStrings->Add(NiGetViewerString(NiCollisionData::ms_RTTI.GetName()));
    
    // Propagate Mode
    pkStrings->Add(GetViewerString("m_ePropagationMode",
        GetPropagationMode()));
    pkStrings->Add(GetViewerString("m_eCollisionMode", GetCollisionMode()));

    if (m_pkModelABV)
        m_pkModelABV->AddViewerStrings("m_pkModelABV->Type", pkStrings);

    // Local and world velocity
    pkStrings->Add(m_kLocalVelocity.GetViewerString("m_kLocalVelocity"));
    pkStrings->Add(m_kWorldVelocity.GetViewerString("m_kWorldVelocity"));
}
//---------------------------------------------------------------------------
char* NiCollisionData::GetViewerString(const char* pcPrefix,
    CollisionMode eMode)
{
    unsigned int uiLen = strlen(pcPrefix) + 17;
    char* pString = NiAlloc(char, uiLen);

    switch (eMode)
    {
    case USE_OBB:
        NiSprintf(pString, uiLen, "%s = USE_OBB", pcPrefix);
        break;
    case USE_TRI:
        NiSprintf(pString, uiLen, "%s = USE_TRI", pcPrefix);
        break;
    case USE_ABV:
        NiSprintf(pString, uiLen, "%s = USE_ABV", pcPrefix);
        break;
    case NOTEST:
        NiSprintf(pString, uiLen, "%s = NOTEST", pcPrefix);
        break;
    case USE_NIBOUND:
        NiSprintf(pString, uiLen, "%s = USE_NIBOUND", pcPrefix);
        break;
    default:
        NiSprintf(pString, uiLen, "%s = UNKNOWN!!!", pcPrefix);
        break;
    }

    return pString;
}
//---------------------------------------------------------------------------
char* NiCollisionData::GetViewerString(const char* pcPrefix,
    PropagationMode eMode)
{
    unsigned int uiLen = strlen(pcPrefix) + 32;
    char* pString = NiAlloc(char, uiLen);

    switch (eMode)
    {
    case PROPAGATE_ON_SUCCESS:
        NiSprintf(pString, uiLen, "%s = PROPAGATE_ON_SUCCESS", pcPrefix);
        break;
    case PROPAGATE_ON_FAILURE:
        NiSprintf(pString, uiLen, "%s = PROPAGATE_ON_FAILURE", pcPrefix);
        break;
    case PROPAGATE_ALWAYS:
        NiSprintf(pString, uiLen, "%s = PROPAGATE_ALWAYS", pcPrefix);
        break;
    case PROPAGATE_NEVER:
        NiSprintf(pString, uiLen, "%s = PROPAGATE_NEVER", pcPrefix);
        break;
    default:
        NiSprintf(pString, uiLen, "%s = UNKNOWN!!!", pcPrefix);
        break;
    }

    return pString;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiCollisionData);
//---------------------------------------------------------------------------
void NiCollisionData::CopyMembers(NiCollisionData* pkDest,
    NiCloningProcess& kCloning)
{
    NiCollisionObject::CopyMembers(pkDest, kCloning);

    // m_pkScreneObject must be filled in by caller.

    pkDest->m_ePropagationMode = m_ePropagationMode;
    pkDest->m_eCollisionMode = m_eCollisionMode;

    if (m_pkModelABV)
        pkDest->SetModelSpaceABV(m_pkModelABV->Clone());

    if (m_pkOBBRoot)
    {
        if (pkDest->m_pkWorldVertex == NULL)
        {
            pkDest->CreateWorldVertices();
            pkDest->UpdateWorldVertices();
        }

        pkDest->CreateOBB();
    }

    // Adding to the NiCollisiongGroup, setting up the callbacks, data, 
    // etc, need to be done by the application, since cloning doesn't handle
    // it.
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiCollisionData);
//---------------------------------------------------------------------------
void NiCollisionData::LoadBinary(NiStream& kStream)
{
    NiCollisionObject::LoadBinary(kStream);

    NiStreamLoadEnum(kStream, m_ePropagationMode);
    NiStreamLoadEnum(kStream, m_eCollisionMode);
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 3, 0, 5))
        RemapOldCollisionMode();

    NiBool bABV;
    NiStreamLoadBinary(kStream,bABV);

    if (bABV)
    {
        NIASSERT(m_pkModelABV == NULL);
        NIASSERT(m_pkWorldABV == NULL);

        m_pkModelABV = NiBoundingVolume::CreateFromStream(kStream);

        // World ABV data can not be updated until streaming is done.
        m_pkWorldABV = m_pkModelABV->Create();
    }
}
//---------------------------------------------------------------------------
void NiCollisionData::LinkObject(NiStream& kStream)
{
    NiCollisionObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiCollisionData::RegisterStreamables(NiStream& kStream)
{
    if (!NiCollisionObject::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiCollisionData::SaveBinary(NiStream& kStream)
{
    NiCollisionObject::SaveBinary(kStream);

    NiStreamSaveEnum(kStream, m_ePropagationMode);
    NiStreamSaveEnum(kStream, m_eCollisionMode);

    if (m_pkModelABV)
    {
        NiStreamSaveBinary(kStream, true);
        m_pkModelABV->SaveBinary(kStream);
    }
    else
    {
        NiStreamSaveBinary(kStream, false);
    }
}
//---------------------------------------------------------------------------
bool NiCollisionData::IsEqual(NiObject* pkObject)
{
    if (!NiCollisionObject::IsEqual(pkObject))
         return false;

    if (NiIsKindOf(NiCollisionData,pkObject) == false)
        return false;

    NiCollisionData* pkCD = (NiCollisionData*)pkObject;

    if (pkCD->GetCollisionMode() != GetCollisionMode())
        return false;

    if (pkCD->GetPropagationMode() != GetPropagationMode())
        return false;

    if (pkCD->GetModelSpaceABV() && GetModelSpaceABV())
    {
        // Both have same type of model ABVs.
        if (pkCD->GetModelSpaceABV()->Type() != GetModelSpaceABV()->Type())
            return false;
    }
    else
    {
        if (pkCD->GetModelSpaceABV() != GetModelSpaceABV())
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiCollisionData::SetPrecacheAttributes(NiAVObject* pkObj, 
    CollisionMode eMode)
{

    if (NiIsKindOf(NiGeometry, pkObj))
    {
        NiGeometryData* pkGeomData = ((NiGeometry*) pkObj)->GetModelData();
        if (pkGeomData)
        {
            if (eMode == USE_OBB || eMode == USE_TRI)
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
}
//---------------------------------------------------------------------------

