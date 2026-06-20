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
#include "NiMainPCH.h"

#include "NiSkinInstance.h"
#include "NiRenderer.h"

//---------------------------------------------------------------------------
NiSkinInstance::~NiSkinInstance()
{
    NiFree(m_ppkBones);

    NiRenderer::RemoveSkinInstance(this);
}
//---------------------------------------------------------------------------
void NiSkinInstance::Deform(const NiPoint3* pkSrcVertex,
    const NiPoint3* pkSrcNormal, unsigned int uiVertices,
    NiPoint3* pkDstVertex, NiPoint3* pkDstNormal, NiPoint3* pkDstBinormal, 
    NiPoint3* pkDstTangent, unsigned int uiStride) const
{
    NIASSERT(uiStride % sizeof(float) == 0);

    unsigned int i, j;
    NiTransform kWorldToSkin, kWorldToRootParent;
    m_pkRootParent->GetWorldTransform().Invert(kWorldToRootParent);
    kWorldToSkin = m_spSkinData->GetRootParentToSkin() * kWorldToRootParent;
    const NiSkinData::BoneData* pkBoneData = m_spSkinData->GetBoneData();
    unsigned int uiBones = m_spSkinData->GetBoneCount();

    if (pkSrcNormal == NULL || pkDstNormal == NULL)
    {
        // Positions only
        NiPoint3* pkV;

        if (uiStride == sizeof(pkDstVertex[0]))
        {
            memset(pkDstVertex, 0, sizeof(pkDstVertex[0]) * uiVertices);
        }
        else
        {
            pkV = pkDstVertex;

            for (i = 0; i < uiVertices; i++)
            {
                *pkV = NiPoint3::ZERO;
                pkV = (NiPoint3*) ((unsigned int) pkV + uiStride);
            }
        }
        
        for (i = 0; i < uiBones; i++)
        {
            unsigned int uiBoneVerts = pkBoneData[i].m_usVerts;
            NiTransform kXform;
            
            kXform = kWorldToSkin * m_ppkBones[i]->GetWorldTransform() *
                pkBoneData[i].m_kSkinToBone;
            
            NiMatrix3 kScaledRotate = kXform.m_Rotate * kXform.m_fScale;

            const NiSkinData::BoneVertData* pkBoneVertData =
                pkBoneData[i].m_pkBoneVertData;
            
            for (j = 0; j < uiBoneVerts; j++)
            {
                unsigned short usVert = pkBoneVertData[j].m_usVert;
                NIASSERT(usVert < uiVertices);
                const float fWeight = (pkBoneVertData[j].m_fWeight);
                pkV = (NiPoint3*) ((unsigned int) pkDstVertex + usVert *
                    uiStride);

                NiPoint3 kVertex;
                NiMatrixTimesPointPlusPoint(kVertex, kScaledRotate,
                    (pkSrcVertex[usVert]), kXform.m_Translate);

                // The following assembly is to perform:
                // *pkV += fWeight * kVertex;
                NiPoint3* pkVertex = &kVertex;

__asm
{
    mov eax, pkV;
    mov ecx, pkVertex;

    fld DWORD PTR [ecx];        v.x
    fmul fWeight;               w*v.x

    fld DWORD PTR [ecx+4];      v.y w*v.x
    fmul fWeight;               w*v.y w*v.x
    fxch;                       w*v.x w*v.y
    fadd DWORD PTR [eax];       x+w*v.x w*v.y

    fld DWORD PTR [ecx+8];      v.z x+w*v.x w*v.y
    fmul fWeight;               w*v.z x+w*v.x w*v.y
    fxch ST(2);                 w*v.y x+w*v.x w*v.z
    fadd DWORD PTR [eax+4];     y+w*v.y x+w*v.x w*v.z
    fxch;                       x+w*v.x y+w*v.y w*v.z
    fstp DWORD PTR [eax];       y+w*v.y w*v.z
    fxch;                       w*v.z y+w*v.y
    fadd DWORD PTR [eax+8];     z+w*v.z y+w*v.y
    fxch;                       y+w*v.y z+w*v.z
    fstp DWORD PTR [eax+4];     z+w*v.z
    fstp DWORD PTR [eax+8];     
}

            }
        }
    }
    else if (pkDstBinormal == NULL || pkDstTangent == NULL)
    {
        // Positions and normals only

        NiPoint3* pkV;
        NiPoint3* pkN;

        if (uiStride == sizeof(pkDstVertex[0]))
        {
            memset(pkDstVertex, 0, sizeof(pkDstVertex[0]) * uiVertices);
            memset(pkDstNormal, 0, sizeof(pkDstNormal[0]) * uiVertices);
        }
        else
        {
            pkV = pkDstVertex;
            pkN = pkDstNormal;

            for (i = 0; i < uiVertices; i++)
            {
                *pkV = NiPoint3::ZERO;
                pkV = (NiPoint3*) ((unsigned int) pkV + uiStride);
                *pkN = NiPoint3::ZERO;
                pkN = (NiPoint3*) ((unsigned int) pkN + uiStride);
            }
        }
        
        for (i = 0; i < uiBones; i++)
        {
            unsigned int uiBoneVerts = pkBoneData[i].m_usVerts;
            NiTransform kXform;
            
            kXform = kWorldToSkin * m_ppkBones[i]->GetWorldTransform() *
                pkBoneData[i].m_kSkinToBone;
            
            const NiSkinData::BoneVertData* pkBoneVertData =
                pkBoneData[i].m_pkBoneVertData;
            
            NiMatrix3 kScaledRotate = kXform.m_Rotate * kXform.m_fScale;

            for (j = 0; j < uiBoneVerts; j++)
            {
                unsigned short usVert = pkBoneVertData[j].m_usVert;
                NIASSERT(usVert < uiVertices);
                float fWeight = pkBoneVertData[j].m_fWeight;
             
                unsigned int uiOffset = usVert * uiStride;
                pkV = (NiPoint3*) ((unsigned int) pkDstVertex + uiOffset);
                NiPoint3 kVertex;
                NiMatrixTimesPointPlusPoint(kVertex, kScaledRotate,
                    (pkSrcVertex[usVert]), kXform.m_Translate);
                pkN = (NiPoint3*) ((unsigned int) pkDstNormal + uiOffset);
                NiPoint3 kNormal;
                NiMatrixTimesPoint(kNormal, kXform.m_Rotate, 
                    (pkSrcNormal[usVert]));
                
                *pkV += fWeight * kVertex;
                *pkN += fWeight * kNormal;
            }
        }
        
        NiPoint3::UnitizeVectors(pkDstNormal, uiVertices, uiStride);
    }
    else
    {
        // Positions, normals, binormals, and tangents

        const NiPoint3* pkSrcBinormal = pkSrcNormal + uiVertices;
        const NiPoint3* pkSrcTangent = pkSrcBinormal + uiVertices;

        NiPoint3* pkV;
        NiPoint3* pkN;
        NiPoint3* pkB;
        NiPoint3* pkT;

        if (uiStride == sizeof(pkDstVertex[0]))
        {
            memset(pkDstVertex, 0, sizeof(pkDstVertex[0]) * uiVertices);
            memset(pkDstNormal, 0, sizeof(pkDstNormal[0]) * uiVertices);
            memset(pkDstBinormal, 0, sizeof(pkDstBinormal[0]) * uiVertices);
            memset(pkDstTangent, 0, sizeof(pkDstTangent[0]) * uiVertices);
        }
        else
        {
            pkV = pkDstVertex;
            pkN = pkDstNormal;
            pkB = pkDstBinormal;
            pkT = pkDstTangent;

            for (i = 0; i < uiVertices; i++)
            {
                *pkV = NiPoint3::ZERO;
                pkV = (NiPoint3*) ((unsigned int) pkV + uiStride);
                *pkN = NiPoint3::ZERO;
                pkN = (NiPoint3*) ((unsigned int) pkN + uiStride);
                *pkB = NiPoint3::ZERO;
                pkB = (NiPoint3*) ((unsigned int) pkB + uiStride);
                *pkT = NiPoint3::ZERO;
                pkT = (NiPoint3*) ((unsigned int) pkT + uiStride);
            }
        }
        
        for (i = 0; i < uiBones; i++)
        {
            unsigned int uiBoneVerts = pkBoneData[i].m_usVerts;
            NiTransform kXform;
            
            kXform = kWorldToSkin * m_ppkBones[i]->GetWorldTransform() *
                pkBoneData[i].m_kSkinToBone;
            
            const NiSkinData::BoneVertData* pkBoneVertData =
                pkBoneData[i].m_pkBoneVertData;
            
            NiMatrix3 kScaledRotate = kXform.m_Rotate * kXform.m_fScale;

            for (j = 0; j < uiBoneVerts; j++)
            {
                unsigned short usVert = pkBoneVertData[j].m_usVert;
                NIASSERT(usVert < uiVertices);
                float fWeight = pkBoneVertData[j].m_fWeight;
             
                unsigned int uiOffset = usVert * uiStride;
                pkV = (NiPoint3*) ((unsigned int) pkDstVertex + uiOffset);
                NiPoint3 kVertex;
                NiMatrixTimesPointPlusPoint(kVertex, kScaledRotate,
                    (pkSrcVertex[usVert]), kXform.m_Translate);
                pkN = (NiPoint3*) ((unsigned int) pkDstNormal + uiOffset);
                NiPoint3 kNormal;
                NiMatrixTimesPoint(kNormal, kXform.m_Rotate, 
                    (pkSrcNormal[usVert]));
                pkB = (NiPoint3*) ((unsigned int) pkDstBinormal + uiOffset);
                NiPoint3 kBinormal;
                NiMatrixTimesPoint(kBinormal, kXform.m_Rotate, 
                    (pkSrcBinormal[usVert]));
                pkT = (NiPoint3*) ((unsigned int) pkDstTangent + uiOffset);
                NiPoint3 kTangent;
                NiMatrixTimesPoint(kTangent, kXform.m_Rotate, 
                    (pkSrcTangent[usVert]));
                
                *pkV += fWeight * kVertex;
                *pkN += fWeight * kNormal;
                *pkB += fWeight * kBinormal;
                *pkT += fWeight * kTangent;
            }
        }
        
        NiPoint3::UnitizeVectors(pkDstNormal, uiVertices, uiStride);
        NiPoint3::UnitizeVectors(pkDstBinormal, uiVertices, uiStride);
        NiPoint3::UnitizeVectors(pkDstTangent, uiVertices, uiStride);
    }
}
//---------------------------------------------------------------------------
