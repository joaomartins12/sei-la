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

#include <NiOptimize.h>
#include <NiTriBasedGeom.h>

typedef NiTPrimitiveSet<NiTriBasedGeom*> NiTriBasedGeomSet;

class Interaction : public NiMemObject
{
public:
    const NiAVObject* m_pkBone;
    float m_fWeight;
};

class SkinVert : public NiMemObject
{
public:
    NiPoint3 m_kPos;
    NiTObjectSet<Interaction> m_kBones;
    bool m_bInitialized;

    SkinVert();
};

//---------------------------------------------------------------------------
SkinVert::SkinVert()
{
    m_bInitialized = false;
}

//---------------------------------------------------------------------------
static int FloatCompare(const float* pf0, const float* pf1,
    unsigned int uiNum)
{
    for (unsigned int i = 0; i < uiNum; i++)
    {
        if (pf0[i] < pf1[i])
            return -1;
        if (pf0[i] > pf1[i])
            return 1;
    }

    return 0;
}

//---------------------------------------------------------------------------
static int ComparePoint3(const void* pv0, const void* pv1)
{
    const NiPoint3* pkV0 = (const NiPoint3*) pv0;
    const NiPoint3* pkV1 = (const NiPoint3*) pv1;

    return FloatCompare(&pkV0->x, &pkV1->x, 3);
}

//---------------------------------------------------------------------------
static int CompareObjectTransform(const NiAVObject* pk0,
    const NiAVObject* pk1)
{
    const NiTransform& kW0 = pk0->GetWorldTransform();
    const NiTransform& kW1 = pk1->GetWorldTransform();

    int iCmp;

    for (unsigned int r = 0; r < 3; r++)
    {
        NiPoint3 kCol0, kCol1;

        kW0.m_Rotate.GetCol(r, kCol0);
        kW1.m_Rotate.GetCol(r, kCol1);

        iCmp = FloatCompare(&kCol0.x, &kCol1.x, 3);
        if (iCmp != 0)
            return iCmp;
    }

    iCmp = FloatCompare(&kW0.m_Translate.x, &kW1.m_Translate.x, 3);
    if (iCmp != 0)
        return iCmp;

    return FloatCompare(&kW0.m_fScale, &kW1.m_fScale, 1);
}

//---------------------------------------------------------------------------
static int CompareObjectTransformV(const void* pv0, const void* pv1)
{
    // Like the above function, but takes void arguments so it can be used
    // by qsort.

    const NiAVObject** ppk0 = (const NiAVObject**) pv0;
    const NiAVObject** ppk1 = (const NiAVObject**) pv1;
    return CompareObjectTransform(*ppk0, *ppk1);
}

//---------------------------------------------------------------------------
static int CompareSkinVert(const void* pvKey, const void* pvDatum)
{
    const NiPoint3* pkKey = (const NiPoint3*) pvKey;
    const SkinVert* pkDatum = (const SkinVert*) pvDatum;

    return FloatCompare(&pkKey->x, &pkDatum->m_kPos.x, 3);
}

//---------------------------------------------------------------------------
static void Weld(NiTriBasedGeom** ppkGeom, unsigned int uiObjects)
{
    // Count verts.
    unsigned int c, uiVertices = 0;

    for (c = 0; c < uiObjects; c++)
    {
        NiTriBasedGeom* pkGeom = ppkGeom[c];

        NiSkinInstance* pkSkinInstance = pkGeom->GetSkinInstance();
        NIASSERT(pkSkinInstance != NULL);

        NiSkinPartition* pkSkinPartition = pkSkinInstance->GetSkinPartition();
        NIASSERT(pkSkinPartition != NULL);

        uiVertices += pkGeom->GetVertexCount();
    }

    // Allocate array.
    NiPoint3* pkVerts = NiNew NiPoint3[uiVertices];
    NiPoint3* p = pkVerts;

    // Gather verts.
    for (c = 0; c < uiObjects; c++)
    {
        NiTriBasedGeom* pkGeom = ppkGeom[c];
        const NiPoint3* pkChildVerts = pkGeom->GetVertices();
        unsigned int uiChildVerts = pkGeom->GetVertexCount();
        for (unsigned int v = 0; v < uiChildVerts; v++)
        {
            p[v] = pkChildVerts[v];
        }

        p += uiChildVerts;
    }

    // Sort verts.
    qsort(pkVerts, uiVertices, sizeof(pkVerts[0]), ComparePoint3);

    // Count unique verts.
    unsigned int uiUniqueVerts = 1;
    unsigned int v;
    for (v = 1; v < uiVertices; v++)
    {
        if (FloatCompare(&pkVerts[v - 1].x, &pkVerts[v].x, 3) != 0)
        {
            uiUniqueVerts++;
        }
    }

    // Allocate array.
    SkinVert* pkSkinVerts = NiNew SkinVert[uiUniqueVerts];

    // Gather unique verts.
    pkSkinVerts[0].m_kPos = pkVerts[0];
    uiUniqueVerts = 1;
    for (v = 1; v < uiVertices; v++)
    {
        if (FloatCompare(&pkVerts[v - 1].x, &pkVerts[v].x, 3) != 0)
        {
            pkSkinVerts[uiUniqueVerts++].m_kPos = pkVerts[v];
        }
    }

    NiDelete[] pkVerts;

    // For each object a vert appears in, eliminate any bone's influence if
    // that bone does not influence every instance of the vertex.

    for (c = 0; c < uiObjects; c++)
    {
        NiTriBasedGeom* pkGeom = ppkGeom[c];
        const NiPoint3* pkChildVerts = pkGeom->GetVertices();

        NiSkinInstance* pkSkinInstance = pkGeom->GetSkinInstance();
        NiSkinPartition* pkSkinPartition = pkSkinInstance->GetSkinPartition();
        NiSkinPartition::Partition* pkPartition =
            pkSkinPartition->GetPartitions();
        unsigned int uiPartitions = pkSkinPartition->GetPartitionCount();
        NiAVObject*const* ppkBones = pkSkinInstance->GetBones();

        for (unsigned int p = 0; p < uiPartitions; p++)
        {
            const unsigned short* pusBones = pkPartition->m_pusBones;
            const float* pfWeights = pkPartition->m_pfWeights;
            const unsigned char* pucBonePalette = 
                pkPartition->m_pucBonePalette;

            for (v = 0; v < pkPartition->m_usVertices; v++)
            {
                unsigned int uiVert = pkPartition->m_pusVertexMap[v];
                SkinVert* pkSearchVert = (SkinVert*) bsearch(
                    &pkChildVerts[uiVert], pkSkinVerts, uiUniqueVerts,
                    sizeof(pkSkinVerts[0]), CompareSkinVert);

                NIASSERT(pkSearchVert != NULL);

                if (!pkSearchVert->m_bInitialized)
                {
                    unsigned int b;
                    for (b = 0; b < pkPartition->m_usBonesPerVertex; b++)
                    {
                        Interaction kInteraction;

                        if (pfWeights[b] > 0.0f)
                        {
                            kInteraction.m_fWeight = pfWeights[b];
                            unsigned int uiBone = pusBones[
                                (pucBonePalette == NULL)
                                ? b : pucBonePalette[b]];
                            kInteraction.m_pkBone = ppkBones[uiBone];
                            pkSearchVert->m_kBones.Add(kInteraction);
                        }
                    }

                    pkSearchVert->m_bInitialized = true;
                }
                else
                {
                    // Intersect bone sets
                    unsigned int bv;

                    for (bv = 0; bv < pkSearchVert->m_kBones.GetSize(); )
                    {
                        const NiAVObject* pkBone =
                            pkSearchVert->m_kBones.GetAt(bv).m_pkBone;

                        unsigned int b;
                        for (b = 0; b < pkPartition->m_usBonesPerVertex; b++)
                        {
                            if (pfWeights[b] > 0.0f)
                            {
                                unsigned int uiBone = pusBones[
                                    (pucBonePalette == NULL)
                                    ? b : pucBonePalette[b]];
                                if (ppkBones[uiBone] == pkBone)
                                    break;
                            }
                        }

                        if (b == pkPartition->m_usBonesPerVertex)
                        {
                            // Bone not found.
                            // Remove it from pkSearchVert->m_kBones.
                            pkSearchVert->m_kBones.RemoveAt(bv);
                        }
                        else
                        {
                            bv++;
                        }
                    }
                }

                pfWeights += pkPartition->m_usBonesPerVertex;
                if (pucBonePalette != NULL)
                {
                    pucBonePalette += pkPartition->m_usBonesPerVertex;
                }
            }

            pkPartition++;
        }
    }

    // Unitize weights.
    for (v = 0; v < uiUniqueVerts; v++)
    {
        float fWeightSum = 0.0f;
        unsigned int b;

        for (b = 0; b < pkSkinVerts[v].m_kBones.GetSize(); b++)
        {
            fWeightSum += pkSkinVerts[v].m_kBones.GetAt(b).m_fWeight;
        }

        for (b = 0; b < pkSkinVerts[v].m_kBones.GetSize(); b++)
        {
            pkSkinVerts[v].m_kBones.GetAt(b).m_fWeight /= fWeightSum;
        }
    }

    // Assign NiNew weights.
    for (c = 0; c < uiObjects; c++)
    {
        NiTriBasedGeom* pkGeom = ppkGeom[c];
        const NiPoint3* pkChildVerts = pkGeom->GetVertices();
        NiSkinInstance* pkSkinInstance = pkGeom->GetSkinInstance();
        NiSkinPartition* pkSkinPartition = pkSkinInstance->GetSkinPartition();
        NiSkinPartition::Partition* pkPartition =
            pkSkinPartition->GetPartitions();
        unsigned int uiPartitions = pkSkinPartition->GetPartitionCount();
        NiAVObject*const* ppkBones = pkSkinInstance->GetBones();

        for (unsigned int p = 0; p < uiPartitions; p++)
        {
            const unsigned short* pusBones = pkPartition->m_pusBones;
            float* pfWeights = pkPartition->m_pfWeights;
            const unsigned char* pucBonePalette = 
                pkPartition->m_pucBonePalette;

            for (v = 0; v < pkPartition->m_usVertices; v++)
            {
                unsigned int uiVert = pkPartition->m_pusVertexMap[v];
                SkinVert* pkSearchVert = (SkinVert*) bsearch(
                    &pkChildVerts[uiVert], pkSkinVerts, uiUniqueVerts,
                    sizeof(pkSkinVerts[0]), CompareSkinVert);

                NIASSERT(pkSearchVert != NULL);
                NiTObjectSet<Interaction>& kBones = 
                    pkSearchVert->m_kBones;

                if (kBones.GetSize() > 0)
                {
                    unsigned int b;

                    for (b = 0; b < pkPartition->m_usBonesPerVertex; b++)
                    {
                        unsigned int uiBone = pusBones[(pucBonePalette == NULL)
                            ? b : pucBonePalette[b]];
                        const NiAVObject* pkBone = ppkBones[uiBone];

                        if (pfWeights[b] > 0.0f)
                        {
                            unsigned int bb;
                            pfWeights[b] = 0.0f;

                            for (bb = 0; bb < kBones.GetSize(); bb++)
                            {
                                if (pkBone == kBones.GetAt(bb).m_pkBone)
                                {
                                    pfWeights[b] = kBones.GetAt(bb).m_fWeight;
                                    break;
                                }
                            }
                        }
                    }
                }

                pfWeights += pkPartition->m_usBonesPerVertex;
                if (pucBonePalette != NULL)
                {
                    pucBonePalette += pkPartition->m_usBonesPerVertex;
                }
            }

            pkPartition++;
        }
    }

    NiDelete[] pkSkinVerts;
}

//---------------------------------------------------------------------------
static void GatherSkins(NiAVObject* pkObject,
    NiTriBasedGeomSet& kSkins)
{
    // Add all geometry objects that have skin partitions to kSkins.

    if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*) pkObject;

        for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);
            if (pkChild)
                GatherSkins(pkChild, kSkins);
        }
    }
    else if (NiIsKindOf(NiTriBasedGeom, pkObject))
    {
        NiTriBasedGeom* pkGeom = (NiTriBasedGeom*) pkObject;
        NiSkinInstance* pkSkinInstance = pkGeom->GetSkinInstance();
        if (pkSkinInstance != NULL)
        {
            NiSkinPartition* pkSkinPartition = pkSkinInstance->
                GetSkinPartition();
            if (pkSkinPartition != NULL)
            {
                kSkins.Add(pkGeom);
            }
        }
    }
}

//---------------------------------------------------------------------------
void NiOptimize::WeldSkin(NiAVObject* pkScene)
{        
    // Only objects with identical world transforms are welded. Call Update
    // to make sure world transforms have been initialized.
    pkScene->Update(0.0f);

    NiTriBasedGeomSet kSkins;
    
    GatherSkins(pkScene, kSkins);
    
    // Sort objects based on world transform and pass groups of objects
    // with the same world transform to Weld.

    unsigned int uiSkins = kSkins.GetSize();
    NiTriBasedGeom** ppkBase = kSkins.GetBase();
    qsort(ppkBase, uiSkins, sizeof(NiTriBasedGeom*), CompareObjectTransformV);
    
    unsigned int uiProcessed, uiAligned;
    
    for (uiProcessed = 0; uiProcessed < uiSkins; uiProcessed += uiAligned)
    {
        const NiAVObject* pkRef = kSkins.GetAt(uiProcessed);
        
        unsigned int j = uiProcessed + 1;
        for (; j < uiSkins && 
            CompareObjectTransform(pkRef, kSkins.GetAt(j)) == 0;
            j++)
        {
        }
        
        uiAligned = j - uiProcessed;
        
        if (uiAligned > 1)
        {
            Weld(&ppkBase[uiProcessed], uiAligned);
        }
    }
}
