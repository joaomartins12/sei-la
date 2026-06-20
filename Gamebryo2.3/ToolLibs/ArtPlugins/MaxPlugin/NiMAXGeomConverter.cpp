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

#include "MaxImmerse.h"
#include "NiMaxGeomConverter.h"
#include "NiMAXHierConverter.h"
#include "NiUtils.h"
#include "NiMAXCustAttribConverter.h"

extern const char* gpcMultiMtlName;

NiTPointerList<NiSkinInstance *> NiMAXGeomConverter::m_skinList2;

#if CURRENT_VERSION < 3
static bool MoveSkins(NiAVObject* pkObject);
#endif

//---------------------------------------------------------------------------
void NiMAXGeomConverter::Init()
{
    NIASSERT(m_skinList2.IsEmpty());
}
//---------------------------------------------------------------------------
void RemoveDeadSkins(NiAVObject* pkObj)
{
    if (NiIsKindOf(NiNode, pkObj))
    {
        NiNode* pkNode = (NiNode*) pkObj;
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            RemoveDeadSkins(pkNode->GetAt(ui));
        }
    }

    if (NiIsKindOf(NiGeometry, pkObj))
    {
        NiGeometry* pkGeom = (NiGeometry*) pkObj;
        NiSkinInstance* pkSkin = pkGeom->GetSkinInstance();
        if (pkSkin && pkSkin->GetSkinData() == NULL)
        {
            pkGeom->SetSkinInstance(NULL);
        }
    }
}
//---------------------------------------------------------------------------
int NiMAXGeomConverter::Finish(NiNode* pRoot, TimeValue animStart)
{
    // resolve bone pointers in any skin instances 
    if (m_skinList2.IsEmpty())
        return(W3D_STAT_OK);

    NiTListIterator pSkinList = m_skinList2.GetHeadPos();
    bool bDeadSkins = false;
    while (pSkinList)
    {
        NiSkinInstance* pSkin = m_skinList2.GetNext(pSkinList); 

        NIASSERT(pSkin);
        if (!ResolveBones(pSkin))
        {
            bDeadSkins = true;
            NILOGWARNING("Skin uses bones that weren't exported. Skinning"
                " will be disabled!\n");
        }
    }

    if (bDeadSkins)
    {
        RemoveDeadSkins(pRoot);
    }

#if CURRENT_VERSION < 3
    MoveSkins(pRoot);
#endif
    
    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
#if CURRENT_VERSION < 3
bool MoveSkins(NiAVObject* pkObject)
{
    if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*) pkObject;
        unsigned int uiChildren = pkNode->GetArrayCount();

        // By the time this loop exits, the number of children that pkNode 
        // has could change (because a skinned object might be moved), but 
        // if that happens, we only want to loop over the children that
        // already exist before we start the loop.

        for (unsigned int i = 0; i < uiChildren; i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);
            if (pkChild != NULL)
            {
                if (!MoveSkins(pkChild))
                    break;
            }
        }
    }
    else if (NiIsKindOf(NiGeometry, pkObject))
    {
        NiGeometry* pkGeometry = (NiGeometry*) pkObject;
        // If the object is skinned, make it a sibling of its root bone.
        const NiSkinInstance* pkSkinInstance = pkGeometry->GetSkinInstance();
        if (pkSkinInstance != NULL)
        {
            NiNode* pkRootParent = (NiNode*) pkSkinInstance->GetRootParent();
            NIASSERT(NiIsKindOf(NiNode, pkRootParent));

            // If multiple skinned TriShapes were created because of multiple
            // materials in MAX, we can move all the skinned objects at 
            // once by moving their parent node.

            // if the skin is a child of an __NDL_MultiMtl_Node then 
            // set spRelocatedObject to the __NDL_MultiMtl_Node.  Otherwise
            // set it to the NiTriShape.

            NiAVObjectPtr spRelocatedObject;
            if (strcmp(gpcMultiMtlName, 
                pkGeometry->GetParent()->GetName()) == 0)
            {
                NiNode* pkNode = pkGeometry->GetParent();

                // If these assertions fail, insert assignments to make them 
                // true.
                NIASSERT(pkNode->GetRotate() == NiMatrix3::IDENTITY);
                NIASSERT(pkNode->GetTranslate() == NiPoint3::ZERO);
                NIASSERT(pkNode->GetScale() == 1.0f);

                unsigned int i, uiChildren = pkNode->GetArrayCount();

                for (i = 0; i < uiChildren; i++)
                {
                    NiAVObject* pkChild = pkNode->GetAt(i);
                    NIASSERT(pkChild != NULL);
                    pkChild->SetRotate(NiMatrix3::IDENTITY);
                    pkChild->SetTranslate(NiPoint3::ZERO);
                    pkChild->SetScale(1.0f);
                }
                
                spRelocatedObject = pkNode;
            }
            else
            {
                spRelocatedObject = pkGeometry;
                pkGeometry->SetRotate(NiMatrix3::IDENTITY);
                pkGeometry->SetTranslate(NiPoint3::ZERO);
                pkGeometry->SetScale(1.0f);
            }

            // check if spRelocatedObject's parent has any NiDynamicEffects
            // on it
            NiNode* pkROParent = spRelocatedObject->GetParent();
            const NiDynamicEffectList* pkList = &pkROParent->GetEffectList();
            if (!pkList->IsEmpty())
            {
                // insert a new node to put the effects on
                NiNodePtr spNode = NiNew NiNode;
                spNode->AttachChild(spRelocatedObject);

                NiTListIterator kIter = pkList->GetHeadPos();
                while (kIter)
                {
                    NiDynamicEffect* pkEffect = pkList->GetNext(kIter);
                    pkEffect->AttachAffectedNode(spNode);
                }
                spRelocatedObject = spNode;
            }

            spRelocatedObject->SetName(pkROParent->GetName());
            pkRootParent->AttachChild(spRelocatedObject, false);

            // delete spRelocatedObject's old parent.
            pkROParent->GetParent()->DetachChild(pkROParent);

            return false;
        }
    }

    return true;
}
#endif
//---------------------------------------------------------------------------
void NiMAXGeomConverter::Shutdown()
{
    m_skinList2.RemoveAll();
}

//---------------------------------------------------------------------------
Modifier* NiMAXGeomConverter::FindModifier(Object* pObj, Class_ID kModID)
{
    if (!pObj)
        return(NULL);

    IDerivedObject *pDerived = NULL;
    if (pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID)
    {
        pDerived = (IDerivedObject *) pObj;

        for (int i = 0; i < pDerived->NumModifiers(); i++)
        {
            Modifier* pMod = pDerived->GetModifier(i);

            Class_ID cID = pMod->ClassID();
            if (cID == kModID)
               return(pMod);
        }
    }

    if (pDerived)
        return FindModifier(pDerived->GetObjRef(), kModID);
    else
        return(NULL);
}
//---------------------------------------------------------------------------
void NiMAXGeomConverter::ConvertSkin(
    ISkin* pkSkinMod, 
    NiOptVertex* pkVertices, unsigned int uiNumVerts,
    unsigned short* pusRemapping,
    unsigned int& uiNumBones, INode**& ppkBones, INode*& pkRootParent, 
    NiTransform*& pkInitXforms, NiTransform& kRootParentToSkin,
    INode* pkMaxNode, bool bRigidVertices)
{
    CHECK_MEMORY();

    // Get skin initialization matrix
    Matrix3 kMaxInitTM;
    pkSkinMod->GetSkinInitTM(pkMaxNode, kMaxInitTM, true);
    bool bInitTMOkay;        
    NiTransform kNiInitTM;
    bInitTMOkay = MaxToNI(kMaxInitTM, kNiInitTM);
    NIASSERT(bInitTMOkay);
    // invert it
    kNiInitTM.Invert(kRootParentToSkin);
      
    // allocate ni bones and init transforms -- one extra for skin as bone
    uiNumBones = pkSkinMod->GetNumBones();
    if (!uiNumBones)
        return;
    
    ++uiNumBones;

    ppkBones = NiAlloc(INode*, uiNumBones);
    pkInitXforms = NiNew NiTransform[uiNumBones];

    ISkinContextData* pkSkinData = pkSkinMod->GetContextInterface(pkMaxNode);

    unsigned int uiHighestMAXVertex = 0;
    for (unsigned int i = 0; i < uiNumVerts; i++)
    {
        if (pusRemapping[i] > uiHighestMAXVertex)
            uiHighestMAXVertex = pusRemapping[i];
    }

    if (uiHighestMAXVertex >= pkSkinData->GetNumPoints())
    {
        NILOGWARNING("Object has a skin modifier that references fewer"
            " vertices than the object's geometry contains.\n"
            " Make sure skin modifier is applied to a mesh rather than"
            " a surface.\n");
        return;
    }

    // fill in bones and init matrices
    unsigned int uiBone = 0;
    for (; uiBone < uiNumBones-1; uiBone++)
    {
        ppkBones[uiBone] = pkSkinMod->GetBone(uiBone);
        
        pkSkinMod->GetBoneInitTM(ppkBones[uiBone], kMaxInitTM);
        bInitTMOkay = MaxToNI(kMaxInitTM, pkInitXforms[uiBone]);
        NIASSERT(bInitTMOkay);
    
        // invert initial pose matrices, and factor out skin's init tm.
        pkInitXforms[uiBone].Invert(pkInitXforms[uiBone]);
        pkInitXforms[uiBone] = pkInitXforms[uiBone] * kNiInitTM;
    }

    // skin as bone
    ppkBones[uiBone] = pkMaxNode;
    pkInitXforms[uiBone].MakeIdentity();

    // fill in bone assignments and weights per vertex
    for (unsigned int i = 0; i < uiNumVerts; i++)
    {
        // convert the NI index into the appropriate MAX index
        // We will use this index to get the bone information
        // from the pkSkinData.
        unsigned int uiMAXVertex = pusRemapping[i];

        unsigned int uiNumVertBones = 
            pkSkinData->GetNumAssignedBones(uiMAXVertex) + 1;

        pkVertices[i].m_uiNumBones = uiNumVertBones;
        pkVertices[i].m_pfWeight = NiAlloc(float, uiNumVertBones);
        pkVertices[i].m_puiBoneIndex = NiAlloc(unsigned int, uiNumVertBones);

        float fTotalWeight = 0.0f;

        bool bWarnedAlready = false;
        for (uiBone = 0; uiBone < uiNumVertBones-1; uiBone++)
        {
            // weight
            pkVertices[i].m_pfWeight[uiBone] = 
                pkSkinData->GetBoneWeight(uiMAXVertex, uiBone);

            // Check for -1.#IND, which can occur when Max attempts to
            // normalize weights and all the weights are 0.0f. 
            // Condition in the if statement must be set up this way - 
            // don't change it without testing this case.
            if (!(pkVertices[i].m_pfWeight[uiBone] >= -FLT_MAX &&
                pkVertices[i].m_pfWeight[uiBone] <= FLT_MAX))
            {
                if (!bWarnedAlready)
                {
                    char acstring[1024];
                    NiSprintf(acstring, 1024, "Vertex #%d in object \"%s\" "
                        "has unassigned bone weights. Please make sure that "
                        "the weights are properly set for this vertex.",
                        i, (const char*)NiMAXHierConverter::
                        GetCurrentExportObjectName());
                    NILOGWARNING(acstring);
                    bWarnedAlready = true;
                }
                pkVertices[i].m_pfWeight[uiBone] = 0.0f;
            }

            fTotalWeight += pkVertices[i].m_pfWeight[uiBone];
        
            // assignment 
            pkVertices[i].m_puiBoneIndex[uiBone] = 
                pkSkinData->GetAssignedBone(uiMAXVertex, uiBone);
        }

        // skin as bone

        // weight
        if (fTotalWeight >= 1.0f)
            pkVertices[i].m_pfWeight[uiBone] = 0.0f;
        else
            pkVertices[i].m_pfWeight[uiBone] = 1.0f - fTotalWeight;
            
        // assignment 
        pkVertices[i].m_puiBoneIndex[uiBone] = uiNumBones - 1;

        
        if (bRigidVertices)
        {
            float fBestWeight = -1.0f;
            unsigned int uiBestBone = (unsigned int) -1;

            for (unsigned int uiBone = 0; uiBone < uiNumVertBones; uiBone++)
            {
                // weight
                if (pkVertices[i].m_pfWeight[uiBone] > fBestWeight)
                {
                    fBestWeight = pkVertices[i].m_pfWeight[uiBone];
                    uiBestBone = pkVertices[i].m_puiBoneIndex[uiBone];
                }
            }

            if (uiBestBone != (unsigned int) -1)
            {
                pkVertices[i].m_uiNumBones = 1;
                pkVertices[i].m_pfWeight[0] = 1.0f;
                pkVertices[i].m_puiBoneIndex[0] = uiBestBone;
            }
            else
            {
                pkVertices[i].m_uiNumBones = 1;
                pkVertices[i].m_pfWeight[0] = 0.0f;
                pkVertices[i].m_puiBoneIndex[0] = 0;
            }
        }
    }

    // find root bone
    INode* pkParent = ppkBones[0];
    while (pkParent != NULL)
    {
        pkRootParent = pkParent;
        pkParent = pkParent->GetParentNode();
    }
    CHECK_MEMORY();

}
//---------------------------------------------------------------------------



#ifndef NO_CSTUDIO
NiMAXGeomConverter::NiBoneList::NiBoneList(unsigned int uiTableSize)
{
    m_uiTableSize = uiTableSize;

    m_ppBones = NiAlloc(NtBoneEle*, uiTableSize);
    NIASSERT(m_ppBones);
    memset(m_ppBones, 0, uiTableSize * sizeof(NtBoneEle *));

    m_uiNumBones = 0;
}

//---------------------------------------------------------------------------
NiMAXGeomConverter::NiBoneList::~NiBoneList()
{
    for (unsigned int i = 0; i < m_uiTableSize; i++)
    {
        NtBoneEle* pNext;
        for (NtBoneEle* pCur = m_ppBones[i]; pCur; pCur = pNext)
        {
            pNext = pCur->pNext;
            NiDelete pCur;
        }
    }

    NiFree(m_ppBones);
    m_ppBones = NULL;
    m_uiNumBones = 0;
}

//---------------------------------------------------------------------------
unsigned int NiMAXGeomConverter::NiBoneList::AddBone(INode* pBone, 
    NtPointTracker *pTracker)
{
    if (pBone == NULL)
        return 0;

    unsigned int uiHashVal = ((((int ) pBone) >> 2) ^ 0x19fa83) % 
        m_uiTableSize;
    
    NtBoneEle* pCur;
    NtBoneEle* pPrev;

    for (pPrev = NULL, pCur = m_ppBones[uiHashVal]; 
         pCur; 
         pPrev = pCur, pCur = pCur->pNext)
    {
        if (pCur->pNode == pBone)
            return(uiHashVal);
        
        // The following will keep list in order of increasing
        // INode* addresses.  pNew will be inserted before pCur.
        // The search/insert time benefits to this optimization are
        // probably negligable.
        if (pCur->pNode > pBone)
            break;
    }
    
    NtBoneEle* pNew = NiNew NtBoneEle;
    NIASSERT(pNew);
    
    pNew->pNode = pBone;
    pNew->uiID = m_uiNumBones;

    Matrix3 initTM;

    int iStatus = pTracker->pPExport->GetInitNodeTM(pBone, initTM);

    if (iStatus != MATRIX_RETURNED || 
        !MaxToNI(initTM, pNew->initXform))
    {
        // Bone init matrices are bogus, set flag to disable skin.
        pTracker->bInvalid = true;
    }

    if (pPrev == NULL)
    {
        // insert at beginning
        pNew->pNext = m_ppBones[uiHashVal];
        m_ppBones[uiHashVal] = pNew;
    }
    else
    {
        // insert in middle or at end
        pNew->pNext = pCur;
        pPrev->pNext = pNew;
    }
    
    m_uiNumBones++;
    return(uiHashVal);
}
//---------------------------------------------------------------------------
void NiMAXGeomConverter::TrackInit(NtPointTracker* pTracker, 
    unsigned int uiNumVerts)
{
    if (!pTracker)  
        return;

    pTracker->ppBoneData = NiAlloc(NtBoneData*, uiNumVerts);
    NIASSERT(pTracker->ppBoneData);
    
    memset(pTracker->ppBoneData, 0, uiNumVerts * sizeof(NtBoneData *));
    pTracker->uiNumVerts = uiNumVerts;

    pTracker->pBoneList = NiNew NiBoneList(113);
    NIASSERT(pTracker->pBoneList);
}

//---------------------------------------------------------------------------
// Extract the skin & bone information for vertex "iIndex"
void NiMAXGeomConverter::TrackVert(NtPointTracker* pTracker, int iIndex)
{
    
    if (!pTracker)  
        return;

    IPhyVertexExport* pVertex = (IPhyVertexExport *) 
        pTracker->pMCExport->GetVertexInterface(iIndex);

    if (pVertex)
    {
        NtBoneData* pBone;

        int iType = pVertex->GetVertexType();

        if (iType == RIGID_BLENDED_TYPE)
        {
            IPhyBlendedRigidVertex *pBlended;
            int i;

            // multiple bones for this vertex
            pBlended = (IPhyBlendedRigidVertex *) pVertex;

            for (i = 0; i < pBlended->GetNumberNodes(); i++)
            {
                pBone = NiNew NtBoneData;

                // add this bone to the start of this vertex' list
                pBone->pNext = pTracker->ppBoneData[iIndex];
                pTracker->ppBoneData[iIndex] = pBone;

                pBone->pBone = pBlended->GetNode(i);
                pBone->uiBoneIndex = 
                        pTracker->pBoneList->AddBone(pBone->pBone,
                        pTracker);

                pBone->fWeight = pBlended->GetWeight(i);
            }
        }
        else
        {
            NIASSERT( iType == RIGID_TYPE );

            IPhyRigidVertex *pRigid;

            // only one bone for this vertex
            pRigid = (IPhyRigidVertex *) pVertex;

            pBone = NiNew NtBoneData;

            // add this bone to the start of this vertex' list
            pBone->pNext = NULL;
            pTracker->ppBoneData[iIndex] = pBone;

            pBone->pBone = pRigid->GetNode();
            pBone->uiBoneIndex = pTracker->pBoneList->AddBone(pBone->pBone,
                pTracker);
            pBone->fWeight = 1.0f;
        }

        pTracker->pMCExport->ReleaseVertexInterface(pVertex);
    }
    else
        pTracker->bInvalid = true;


    // CS3 floating bones support
#if CURRENT_VERSION > 3

    IPhyFloatingVertex* pkFVertex = (IPhyFloatingVertex*) 
        pTracker->pMCExport->GetFloatingVertexInterface(iIndex);
    if (pkFVertex)
    {
        for (int i = 0; i < pkFVertex->GetNumberNodes(); i++)
        {
            NtBoneData* pBone = NiNew NtBoneData;

            // add this bone to the start of this vertex' list
            pBone->pNext = pTracker->ppBoneData[iIndex];
            pTracker->ppBoneData[iIndex] = pBone;

            pBone->pBone = pkFVertex->GetNode(i);
            pBone->uiBoneIndex = 
                    pTracker->pBoneList->AddBone(pBone->pBone,
                    pTracker);

            float fTotalWeight;
            pBone->fWeight = pkFVertex->GetWeight(i, fTotalWeight);
        }

        pTracker->pMCExport->ReleaseVertexInterface(pkFVertex);
    }

#endif
}

//---------------------------------------------------------------------------
void NiMAXGeomConverter::TrackDelete(NtPointTracker* pTracker)
{

    for (unsigned int i = 0; i < pTracker->uiNumVerts; i++)
    {
        NtBoneData *pCur, *pNext;

        for (pCur = pTracker->ppBoneData[i]; pCur; pCur = pNext)
        {
            pNext = pCur->pNext;
            NiDelete pCur;
        }
    }

    NiFree(pTracker->ppBoneData);
    pTracker->ppBoneData = NULL;
    pTracker->uiNumVerts = 0;

    NiDelete pTracker->pBoneList;
    pTracker->pBoneList = NULL;
}

//---------------------------------------------------------------------------
// This routine moves the skin and bone information from the NtPointTracker 
// into the NiOptVertex array.
void NiMAXGeomConverter::TrackExtractInfo(NtPointTracker* pTracker, 
    NiOptVertex* pVertices, unsigned int uiNumVerts,
    unsigned short* pRemapping, unsigned int* pNumBones, INode*** pppBones,
    INode **ppRootParent, NiTransform** ppInitXforms,
    NiTransform* pkRootParentToSkin)
{
    *pNumBones = 0;
    *pppBones = NULL;
    *ppRootParent = NULL;
    *ppInitXforms = 0;

    if (!pTracker)
        return;

    if (pTracker->bInvalid)
    {
        NILOGWARNING("Object has a Physique modifier that has\n"
                " invalid initial bone transformation matrices.\n"
                " Delete the Physique modifier and create a new one\n"
                " in order to export skinning data correctly.\n");
        return;
    }

    unsigned int uiNumBones = *pNumBones = 
        pTracker->pBoneList->GetNumBones();

    INode** ppBones = NiAlloc(INode *, uiNumBones);
    NIASSERT(ppBones);
    NiTransform* pInitXforms = NiNew NiTransform[uiNumBones];

    for (unsigned int i = 0; i < uiNumVerts; i++)
    {
        // convert the NI index into the appropriate MAX index
        // We will use this index to get the information from the ppBoneData
        // array
        unsigned int uiVertex = pRemapping[i];

        NIASSERT(uiVertex < pTracker->uiNumVerts);
        NIASSERT(pTracker->ppBoneData[uiVertex]);

        // Count the number of bones influencing this vertex. We could
        // count this up as the bones are accumulated.
        unsigned int uiCurNumBones = 0;
        NtBoneData* pCur;
        for (pCur = pTracker->ppBoneData[uiVertex]; pCur; pCur = pCur->pNext)
            uiCurNumBones++;

        pVertices[i].m_uiNumBones = uiCurNumBones;

        pVertices[i].m_puiBoneIndex = NiAlloc(unsigned int, uiCurNumBones);
        NIASSERT(pVertices[i].m_puiBoneIndex);

        pVertices[i].m_pfWeight = NiAlloc(float, uiCurNumBones);
        NIASSERT(pVertices[i].m_pfWeight);
        
        unsigned int j;
        for (pCur = pTracker->ppBoneData[uiVertex], j = 0; 
             pCur; 
             pCur = pCur->pNext, j++)
        {
             NiBoneList::NtBoneEle* pBoneEle =
                pTracker->pBoneList->GetBoneEle(pCur);

             unsigned int uiBoneID = pBoneEle->uiID;

             NIASSERT(uiBoneID < uiNumBones);

             pVertices[i].m_puiBoneIndex[j] = uiBoneID;
             pVertices[i].m_pfWeight[j] = pCur->fWeight;

             ppBones[uiBoneID] = pCur->pBone;
             pInitXforms[uiBoneID] = pBoneEle->initXform;
        }
    }

#ifdef _DEBUG
    for (unsigned int uiB = 0; uiB < uiNumBones; uiB++)
    {
        NIASSERT(ppBones[uiB]);
    }
#endif

    // find root bone

#if CURRENT_VERSION >= 3
    INode* pParent = ppBones[0];

    while (pParent != NULL)
    {
        *ppRootParent = pParent;
        pParent = pParent->GetParentNode();
    }
    pTracker->kSkinInitTM.Invert(*pkRootParentToSkin);
#else
    *ppRootParent = pTracker->pkSkin->GetParentNode()->GetParentNode();
    pkRootParentToSkin->MakeIdentity();
#endif

    // invert initial pose matrices, and factor out skin's init tm.
    
    for (unsigned int uiBone = 0; uiBone < uiNumBones; uiBone++)
    {
        pInitXforms[uiBone].Invert(pInitXforms[uiBone]);
        pInitXforms[uiBone] = pInitXforms[uiBone] * pTracker->kSkinInitTM;
    }

    *pppBones = ppBones;
    *ppInitXforms = pInitXforms;

    // We can delete all the information since it is now stored in the
    // NiOptVertices
    TrackDelete(pTracker);
}
//---------------------------------------------------------------------------
#endif

bool NiMAXGeomConverter::ResolveBones(NiSkinInstance* pSkin)
{
    NiAVObject* pRootParent =
        NiMAXHierConverter::Lookup((INode*) pSkin->GetRootParent(), NULL);
    if (!pRootParent)
    {
        NiSkinData* pkSkinData = pSkin->GetSkinData();
        pSkin->SetSkinData(NULL);
        pSkin->SetRootParent(NULL);
        return false;
    }
    pSkin->SetRootParent(pRootParent);
    
    unsigned int uiBones = pSkin->GetSkinData()->GetBoneCount();
    NiAVObject*const* ppkBones = pSkin->GetBones();
    for (unsigned int i = 0; i < uiBones; i++)
    {
        void* pBone = ppkBones[i];
        NiAVObject* pkBone = NiMAXHierConverter::Lookup((INode*) pBone, NULL);
        if (!pkBone)
        {
            NiSkinData* pkSkinData = pSkin->GetSkinData();
            pSkin->SetSkinData(NULL);
            pSkin->SetRootParent(NULL);
            return false;
        }
        pSkin->SetBone(i, pkBone);
    }

    return true;
}
//---------------------------------------------------------------------------

