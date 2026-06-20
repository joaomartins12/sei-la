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

#include "NiStripify.h"
#include "NiStripMaker.h"
#include "NvTriStrip.h"
#include "xbstrip.h"
#include <NiObjectNET.h>
#include <NiTriShape.h>
#include <NiTriStrips.h>
#include <NiNode.h>
#include <NiSystem.h>
#include <NiGeomMorpherController.h>
#include <NiBoneLODController.h>
#include <NiStringExtraData.h>
#include <NiParticleSystem.h>
#include <NiPSysMeshEmitter.h>
#include <NiRoom.h>

// static class data
NiStripify::OkayCallback NiStripify::ms_pfnOkayCallback = NULL;
NiStripify::FinishedCallback NiStripify::ms_pfnFinishedCallback = NULL;
NiStripify::ErrorCallback NiStripify::ms_pfnErrorCallback = NULL;

void* NiStripify::ms_pvCallbackData;
NiStripify::Stats NiStripify::ms_kStats;
NiStripify::Stats NiStripify::ms_knVStats;

// bone LOD controller static variables
NiTPrimitiveSet<NiBoneLODController*>* NiStripify::ms_pkBoneLODSet = NULL;

// Mesh Emitter static variable
NiTPrimitiveSet<NiPSysMeshEmitter*>* NiStripify::ms_pkMeshEmitterSet = NULL;

// Portal Fixture static variable
NiTPrimitiveSet<NiRoom*>* NiStripify::ms_pkPortalRoomSet = NULL;

// static functions
static void RetargetControllers(NiObjectNET* pkDst, 
    NiTimeController* pkController);
static void CopyMembers(NiTriStrips* pkTriStrips, NiTriShape* pkTriShape);

unsigned int NiStripify::ms_uiIndexSum = 0;
bool NiStripify::ms_bUse_NextGeneration = false;

//---------------------------------------------------------------------------
void NiStripify::Init()
{
    NIASSERT(!ms_pkBoneLODSet);
    ms_pkBoneLODSet = NiNew NiTPrimitiveSet<NiBoneLODController*>;
    NIASSERT(!ms_pkMeshEmitterSet);
    ms_pkMeshEmitterSet = NiNew NiTPrimitiveSet<NiPSysMeshEmitter*>;
    NIASSERT(!ms_pkPortalRoomSet);
    ms_pkPortalRoomSet = NiNew NiTPrimitiveSet<NiRoom*>;
}
//---------------------------------------------------------------------------
void NiStripify::Shutdown()
{
    NiDelete ms_pkBoneLODSet;
    ms_pkBoneLODSet = NULL;
    NiDelete ms_pkMeshEmitterSet;
    ms_pkMeshEmitterSet = NULL;
    NiDelete ms_pkPortalRoomSet;
    ms_pkPortalRoomSet = NULL;
}
//---------------------------------------------------------------------------
NiTriStrips* NiStripify::Stripify(NiTriShape* pkTriShape, ShapeMap& map)
{
    NiTriStrips* pkStrips;
    NiTriStripsData* pkStripsData;
    NiTriShapeData* pkShapeData = (NiTriShapeData*)
        pkTriShape->GetModelData();
    NIASSERT(NiIsKindOf(NiTriShapeData, pkShapeData));

    if (map.GetAt(pkShapeData, pkStripsData))
    {
        pkStrips = NiNew NiTriStrips(pkStripsData);
        NIASSERT(pkStrips != NULL);
        CopyMembers(pkStrips, pkTriShape);
    }
    else
    {
        pkStrips = Stripify(pkTriShape);
        if (pkStrips != NULL)
        {
            pkStripsData = (NiTriStripsData*) pkStrips->GetModelData();
            map.SetAt(pkShapeData, pkStripsData);
        }
    }

    return pkStrips;
}

//---------------------------------------------------------------------------
void NiStripify::Stripify(NiNode* pkNode, ShapeMap& map)
{
    unsigned int i;

    for (i = 0; i < pkNode->GetArrayCount(); i++)
    {
        NiAVObject* pkChild = pkNode->GetAt(i);

        if (pkChild != NULL)
        {
            if (NiIsKindOf(NiNode, pkChild))
            {
                if (ms_pfnOkayCallback == NULL ||
                    ms_pfnOkayCallback(pkChild, ms_pvCallbackData))
                {
                    if (NoStripifyExtraDataIsNotPresent(pkChild))
                        Stripify((NiNode*) pkChild, map);
                }
            }
            else if (NiIsExactKindOf(NiTriShape, pkChild) && 
                NoStripifyExtraDataIsNotPresent(pkChild))
            {
                // Don't need to call ms_pfnOkayCallback here. It
                // is called by NiStripify::Stripify(NiTriShape*).
                NiTriStrips* pkStrips = Stripify((NiTriShape*) pkChild,
                    map);
                if (pkStrips != NULL)
                {
                    UpdateBoneLODSet((NiTriShape*) pkChild, pkStrips);

                    // Update any mesh emitters that use this Shape
                    UpdateMeshEmitterSet((NiTriShape*) pkChild, pkStrips);

                    // Update any portal objects that refer to this shape
                    // Updating fixtures adds the new object as a child
                    if (!UpdateFixtureSet((NiTriShape*) pkChild, pkStrips))
                    {
                        pkNode->SetAt(i, pkStrips);
                    }

                    if (ms_pfnFinishedCallback != NULL)
                    {
                        ms_pfnFinishedCallback(pkStrips);
                    }
                }
            }
        }
    }
}

//---------------------------------------------------------------------------
template<class T> void NiTCopy(T* pDst, const T* pSrc, unsigned int uiEls)
{
    unsigned int uiSize = sizeof(T) * uiEls;
    NiMemcpy(pDst, pSrc, uiSize);
}

//---------------------------------------------------------------------------
void RetargetControllers(NiObjectNET* pkDst, NiTimeController* pkController)
{
    // recurse before retargetting controller so that timecontroller list
    // does not get reversed.

    if (pkController != NULL)
    {
        RetargetControllers(pkDst, pkController->GetNext());
        pkController->SetTarget(pkDst);
    }
}


//---------------------------------------------------------------------------
// This CopyMembers() cannibalizes the trishape as it copies its data to the
//    tristrips object.  It should not be confused with cloning member
//    functions of the same name in other classes.
void CopyMembers(NiTriStrips* pkTriStrips, NiTriShape* pkTriShape)
{
    pkTriStrips->SetName(pkTriShape->GetName());
    pkTriStrips->SetAppCulled(pkTriShape->GetAppCulled());
    pkTriStrips->SetTranslate(pkTriShape->GetTranslate());
    pkTriStrips->SetRotate(pkTriShape->GetRotate());
    pkTriStrips->SetScale(pkTriShape->GetScale());
    pkTriStrips->SetSkinInstance(pkTriShape->GetSkinInstance());
    pkTriStrips->SetCollisionObject(pkTriShape->GetCollisionObject());

    for (unsigned int ui = 0; ui < pkTriShape->GetMaterialCount(); ui++)
    {
        const NiMaterialInstance* pkInstance = 
            pkTriShape->GetMaterialInstance(ui);
        pkTriStrips->ApplyMaterial(pkInstance->GetMaterial(), 
            pkInstance->GetMaterialExtraData());
    }
    pkTriStrips->SetActiveMaterial(pkTriShape->GetActiveMaterial());

    // properties
    NiTListIterator kPos = pkTriShape->GetPropertyList().GetHeadPos();
    while (kPos)
    {
        NiProperty* pkProp = pkTriShape->GetPropertyList().GetNext(kPos);
        pkTriStrips->AttachProperty(pkProp);
    }

    for (unsigned int i=0; i < pkTriShape->GetExtraDataSize(); i++)
    {
        NiExtraData* pkExtra = pkTriShape->GetExtraDataAt(i);
        pkTriStrips->AddExtraData(pkExtra);
    }

    // time controllers
    RetargetControllers(pkTriStrips, pkTriShape->GetControllers());
}

//---------------------------------------------------------------------------
bool  NiStripify::NoStripifyExtraDataIsNotPresent(NiObjectNET* pkObject)
{
    if (pkObject == NULL)
        return false;

    for (unsigned int ui = 0; ui < pkObject->GetExtraDataSize(); ui++)
    {
        NiExtraData* pkED = pkObject->GetExtraDataAt(ui);
        if (NiIsKindOf(NiStringExtraData, pkED))
        {
            NiStringExtraData* pkStringED = (NiStringExtraData*) pkED;
            const char* pcValue = pkStringED->GetValue();
            const char* pcFoundLocation = strstr(pcValue, "NiNoStripify");
            if (pcFoundLocation != NULL)
                return false;
        }
    }
    return true;
}

//---------------------------------------------------------------------------
void NiStripify::BuildBoneLODSet(NiAVObject* pkObject)
{
    NiTimeController* pkCtlr = pkObject->GetControllers();
    while (pkCtlr)
    {
        if (NiIsKindOf(NiBoneLODController, pkCtlr))
        {
            ms_pkBoneLODSet->Add((NiBoneLODController*) pkCtlr);
            break;
        }

        pkCtlr = pkCtlr->GetNext();
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                BuildBoneLODSet(pkChild);
            }
        }
    }
}

//---------------------------------------------------------------------------
void NiStripify::UpdateBoneLODSet(NiTriShape* pkShape, NiTriStrips* pkStrips)
{
    for (unsigned int ui = 0; ui < ms_pkBoneLODSet->GetSize(); ui++)
    {
        NiBoneLODController* pkBoneLOD = ms_pkBoneLODSet->GetAt(ui);
        pkBoneLOD->ReplaceSkin(pkShape, pkStrips);
    }
}

//---------------------------------------------------------------------------
bool NiStripify::CheckForBoneLOD(NiTriBasedGeom* pkGeom)
{
    for (unsigned int ui = 0; ui < ms_pkBoneLODSet->GetSize(); ui++)
    {
        NiBoneLODController* pkBoneLOD = ms_pkBoneLODSet->GetAt(ui);
        if (pkBoneLOD->FindGeom(pkGeom))
        {
            return true;
        }
    }

    return false;
}

//---------------------------------------------------------------------------
void NiStripify::StripifyBoneLODPartitions()
{
    for (unsigned int ui = 0; ui < ms_pkBoneLODSet->GetSize(); ui++)
    {
        NiBoneLODController* pkBoneLOD = ms_pkBoneLODSet->GetAt(ui);

        NiTPrimitiveSet<NiTriBasedGeom*> kGeoms;
        NiTPrimitiveSet<NiSkinInstance*> kInstances;
        pkBoneLOD->GetSkinData(kGeoms, kInstances);

        for (unsigned int uj = 0; uj < kInstances.GetSize(); uj++)
        {
            NiSkinInstance* pkSkinInstance = kInstances.GetAt(uj);
            NiSkinPartition* pkSkinPartition = pkSkinInstance
                ->GetSkinPartition();
            if (pkSkinPartition)
            {
                if (ms_bUse_NextGeneration)
                {
#if defined(_STRIPIFY_USE_NVIDIA_)
                    StripifyPartitions_nVidia(pkSkinPartition);
#else   //#if defined(_STRIPIFY_USE_NVIDIA_)
                    StripifyPartitions_Xbox(pkSkinPartition);
#endif  //#if defined(_STRIPIFY_USE_NVIDIA_)
                }
                else
                {
                    Stripify(pkSkinPartition);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiStripify::BuildMeshEmitterSet(NiAVObject* pkObject)
{
    if (NiIsKindOf(NiParticleSystem, pkObject))
    {
        NiParticleSystem* pkPSystem = (NiParticleSystem*)pkObject;

        for(unsigned int uiLoop = 0; uiLoop < pkPSystem->GetModifierCount();
            uiLoop++)
        {
            if (NiIsKindOf(NiPSysMeshEmitter, 
                pkPSystem->GetModifierAt(uiLoop)))
            {
                ms_pkMeshEmitterSet->Add(
                    (NiPSysMeshEmitter*)pkPSystem->GetModifierAt(uiLoop));
            }
        }
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                BuildMeshEmitterSet(pkChild);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiStripify::UpdateMeshEmitterSet(NiTriShape* pkShape, 
    NiTriStrips* pkStrip)
{
    // Look through all mesh emitters
    for (unsigned int ui = 0; ui < ms_pkMeshEmitterSet->GetSize(); ui++)
    {
        NiPSysMeshEmitter* pkEmitter = ms_pkMeshEmitterSet->GetAt(ui);

        // Look at all of the Geometry they hold
        for (unsigned int uiGeom = 0; 
            uiGeom < pkEmitter->GetGeometryEmitterCount(); uiGeom++)
        {
            // If the Shape is found remove it and add the strip
            if (pkEmitter->GetGeometryEmitter(uiGeom) == pkShape)
            {
                pkEmitter->RemoveGeometryEmitter(uiGeom);
                pkEmitter->AddGeometryEmitter(pkStrip);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiStripify::BuildFixtureSet(NiAVObject* pkObject)
{
    if (NiIsKindOf(NiRoom, pkObject))
    {
        ms_pkPortalRoomSet->Add((NiRoom*)pkObject);
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                BuildFixtureSet(pkChild);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiStripify::UpdateFixtureSet(NiTriShape* pkShape, NiTriStrips* pkStrips)
{
    NiTriShapePtr spShape = pkShape;
    for (unsigned int ui = 0; ui < ms_pkPortalRoomSet->GetSize(); ui++)
    {
        NiRoom* pkRoom = ms_pkPortalRoomSet->GetAt(ui);
        NiAVObjectPtr spFixture = pkRoom->DetachFixture(spShape);
        if (spFixture != NULL)
        {
            pkRoom->AttachFixture(pkStrips);
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
void NiStripify::Stripify(NiNode* pkNode)
{
    // Build the bone LOD set
    ms_pkBoneLODSet->RemoveAll();
    BuildBoneLODSet(pkNode);

    // Build the Mesh Emitter set
    ms_pkMeshEmitterSet->RemoveAll();
    BuildMeshEmitterSet(pkNode);

    // Build the Portal Fixture set
    ms_pkPortalRoomSet->RemoveAll();
    BuildFixtureSet(pkNode);

    ShapeMap map;
    if (NoStripifyExtraDataIsNotPresent(pkNode))
        Stripify(pkNode, map);

    StripifyBoneLODPartitions();

    // Empty the Mesh Emitter Set
    ms_pkMeshEmitterSet->RemoveAll();
}

//---------------------------------------------------------------------------
NiTriStrips* NiStripify::Stripify(NiTriShape* pkTriShape)
{
    bool bBoneLODControlled = CheckForBoneLOD(pkTriShape);

    //  Check for a skin instance with already converted partitions.
    NiSkinInstance* pkSkinInstance = pkTriShape->GetSkinInstance();
    if (pkSkinInstance && pkSkinInstance->GetSkinPartition() &&
        pkSkinInstance->GetSkinPartition()->GetPartitions() &&
        (pkSkinInstance->GetSkinPartition()->GetPartitions()->m_usStrips > 0))
    {
        return NULL;
    }

    if (ms_bUse_NextGeneration && !bBoneLODControlled)
    {
#if defined(_STRIPIFY_USE_NVIDIA_)
        return Stripify_nVidia(pkTriShape);
#else   //#if defined(_STRIPIFY_USE_NVIDIA_)
        return Stripify_Xbox(pkTriShape);
#endif  //#if defined(_STRIPIFY_USE_NVIDIA_)
    }

    //  We need to update the indices in a skinned object also
    if (pkSkinInstance && pkSkinInstance->GetSkinPartition() &&
        bBoneLODControlled)
    {
        return NULL;
    }

    unsigned short usVerts, usShapeTris, usStripsTris, usStrips;
    unsigned short* pusStripLengths;
    unsigned short* pusStripLists;
    NiTriStripsData* pkStripsData;
    NiTriShapeData* pkShapeData;

    if (ms_pfnOkayCallback != NULL &&
        !ms_pfnOkayCallback(pkTriShape, ms_pvCallbackData))
    {
        return NULL;
    }

    usVerts = pkTriShape->GetVertexCount();
    usShapeTris = pkTriShape->GetTriangleCount();

    NiStripMaker strips(pkTriShape->GetTriList(), usShapeTris);
    strips.GetStripData(usStripsTris, usStrips, pusStripLengths, 
        pusStripLists);

    NiGeometryData::DataFlags eNBTMethod = 
        pkTriShape->GetNormalBinormalTangentMethod();

    // usStripsTris does not necessarily equal usShapeTris, since the
    // stripping code will eliminate degenerate triangles, i.e. triangles 
    // that do not have 3 unique vertices.
    pkStripsData = NiNew NiTriStripsData(usVerts, NULL, NULL, NULL, NULL, 0, 
        eNBTMethod, usStripsTris, usStrips, pusStripLengths, pusStripLists);
    NIASSERT(pkStripsData != NULL);

    // update stats
    ms_kStats.m_uiShapeTriangles += usShapeTris;
    ms_kStats.m_uiStripsTriangles += usStripsTris;
    ms_kStats.m_uiStrips += usStrips;
    ms_kStats.m_uiConvertedShapes++;
    ms_kStats.m_uiStripLengthSum += pkStripsData->GetStripLengthSum();

    pkShapeData = (NiTriShapeData *) pkTriShape->GetModelData();
    NIASSERT(NiIsKindOf(NiTriShapeData, pkShapeData));

    pkStripsData->SetConsistency(pkShapeData->GetConsistency());
    pkStripsData->SetCompressFlags(pkShapeData->GetCompressFlags());
    pkStripsData->SetKeepFlags(pkShapeData->GetKeepFlags());

    pkStripsData->CreateVertices(false);
    NiTCopy(pkStripsData->GetVertices(), pkShapeData->GetVertices(), 
        usVerts);
    pkStripsData->SetBound(pkShapeData->GetBound());

    if (pkShapeData->GetNormals() != NULL)
    {
        pkStripsData->CreateNormals(false);
        unsigned int uiNormalFactor = 
            (eNBTMethod == NiGeometryData::NBT_METHOD_NONE ? 1 : 3);
        NiTCopy(pkStripsData->GetNormals(), pkShapeData->GetNormals(), 
            usVerts * uiNormalFactor);
    }

    if (pkShapeData->GetColors() != NULL)
    {
        pkStripsData->CreateColors(false);
        NiTCopy(pkStripsData->GetColors(), pkShapeData->GetColors(), 
            usVerts);
    }

    if (pkShapeData->GetTextures() != NULL)
    {
        pkStripsData->CreateTextures(false, 
            pkShapeData->GetTextureSets());
        NiTCopy(pkStripsData->GetTextures(), pkShapeData->GetTextures(), 
            usVerts * pkShapeData->GetTextureSets());
    }

    if (pkSkinInstance && pkSkinInstance->GetSkinPartition())
    {
        Stripify(pkSkinInstance->GetSkinPartition());
    }

    NiTriStrips* pkStrips = NiNew NiTriStrips(pkStripsData);
    NIASSERT(pkStrips != NULL);

    CopyMembers(pkStrips, pkTriShape);

    return pkStrips;
}
//---------------------------------------------------------------------------
#if defined(_STRIPIFY_USE_NVIDIA_)
NiTriStrips* NiStripify::Stripify_nVidia(NiTriShape* pkTriShape)
{
    unsigned short usVerts, usShapeTris, usStripsTris, usStrips;
    unsigned short* pusStripLengths;
    unsigned short* pusStripLists;
    NiTriStripsData* pkStripsData;
    NiTriShapeData* pkShapeData;

    unsigned short usNumSections;
    PrimitiveGroup* pOldPG = 0;
    PrimitiveGroup* pNewPG = 0;

    //  We need to update the indices in a skinned object also
    NiSkinInstance* pkSkinInstance = pkTriShape->GetSkinInstance();

    if (ms_pfnOkayCallback != NULL &&
        !ms_pfnOkayCallback(pkTriShape, ms_pvCallbackData))
    {
        return NULL;
    }

    usVerts = pkTriShape->GetVertexCount();
    usShapeTris = pkTriShape->GetTriangleCount();

    //  We have to make sure it does not have a morpher controller 
    //  attached, since the nVidia routines alter the index order!
    NiTimeController* pkCtrl;
    for (pkCtrl = pkTriShape->GetControllers(); pkCtrl != NULL;
            pkCtrl = pkCtrl->GetNext())
    {
        if (NiIsKindOf(NiGeomMorpherController, pkCtrl))
        {
            //  Skip this one!
            return 0;
        }
    }

    // Special case for stripifying NiSkinPartitions with the 
    // nVidia algorithm 
    if (pkSkinInstance && pkSkinInstance->GetSkinPartition())
    {
        return StripifyPartitions_nVidia(pkSkinInstance->GetSkinPartition());
    }

    Run_nVidia_Stripper(pkTriShape->GetTriList(), usShapeTris, usVerts,
        &pOldPG, &pNewPG, usNumSections);

    if (!Get_nVidia_Stripper_Results(pOldPG, pNewPG, usNumSections, 
        usStripsTris, usStrips, pusStripLengths, pusStripLists))
    {
        //  Skip this one!
        return 0;
    }

    NiGeometryData::DataFlags eNBTMethod = 
        pkTriShape->GetNormalBinormalTangentMethod();

    // usStripsTris does not necessarily equal usShapeTris, since the
    // stripping code will eliminate degenerate triangles, i.e. triangles 
    // that do not have 3 unique vertices.
    pkStripsData = NiNew NiTriStripsData(usVerts, NULL, NULL, NULL, NULL,
        0, eNBTMethod, usStripsTris, usStrips, pusStripLengths,
        pusStripLists);
    NIASSERT(pkStripsData != NULL);

    // update stats
    ms_knVStats.m_uiShapeTriangles += usShapeTris;
    ms_kStats.m_uiShapeTriangles += usShapeTris;
    ms_kStats.m_uiStripsTriangles += usStripsTris;
    ms_kStats.m_uiStrips += usStrips;
    ms_kStats.m_uiConvertedShapes++;
    ms_kStats.m_uiStripLengthSum += pkStripsData->GetStripLengthSum();

    pkShapeData = (NiTriShapeData *) pkTriShape->GetModelData();
    NIASSERT(NiIsKindOf(NiTriShapeData, pkShapeData));

    pkStripsData->SetConsistency(pkShapeData->GetConsistency());
    pkStripsData->SetCompressFlags(pkShapeData->GetCompressFlags());
    pkStripsData->SetKeepFlags(pkShapeData->GetKeepFlags());

    pkStripsData->CreateVertices(false);

    //  Need to remap the vertices!
    unsigned short usSection;
    unsigned int uiIndex, uiOldIndex, uiNewIndex;

    NiPoint3* pkShapeVerts = pkShapeData->GetVertices();
    NiPoint3* pkStripVerts = pkStripsData->GetVertices();

    for (usSection = 0; usSection < usNumSections; usSection++)
    {
        for (uiIndex = 0; uiIndex < pNewPG[usSection].numIndices; 
            uiIndex++)
        {
            //  Old index
            uiOldIndex = pOldPG[usSection].indices[uiIndex];
            //  New index
            uiNewIndex = pNewPG[usSection].indices[uiIndex];

            unsigned int uiSize = sizeof(NiPoint3);
            NiMemcpy(&pkStripVerts[uiNewIndex], &pkShapeVerts[uiOldIndex],
                uiSize);
        }
    }
    pkStripsData->SetBound(pkShapeData->GetBound());

    NiPoint3* pkShapeNorms = 0;
    NiPoint3* pkStripNorms = 0;
    NiColorA* pkShapeColors = 0;
    NiColorA* pkStripColors = 0;
    NiPoint2* pkShapeUVs = 0;
    NiPoint2* pkStripUVs = 0;

    if (pkShapeData->GetNormals() != NULL)
    {
        pkStripsData->CreateNormals(false);
        pkShapeNorms = pkShapeData->GetNormals();
        pkStripNorms = pkStripsData->GetNormals();
    }

    if (pkShapeData->GetColors() != NULL)
    {
        pkStripsData->CreateColors(false);
        pkShapeColors = pkShapeData->GetColors();
        pkStripColors = pkStripsData->GetColors();
    }

    if (pkShapeData->GetTextures() != NULL)
    {
        pkStripsData->CreateTextures(false, 
            pkShapeData->GetTextureSets());

        pkShapeUVs = pkShapeData->GetTextures();
        pkStripUVs = pkStripsData->GetTextures();
    }

    unsigned int uiNormalFactor = 
        (eNBTMethod == NiGeometryData::NBT_METHOD_NONE ? 1 : 3);

    for (usSection = 0; usSection < usNumSections; usSection++)
    {
        for (uiIndex = 0; uiIndex < pNewPG[usSection].numIndices; 
            uiIndex++)
        {
            //  Old index
            uiOldIndex = pOldPG[usSection].indices[uiIndex];
            //  New index
            uiNewIndex = pNewPG[usSection].indices[uiIndex];

            if (pkShapeNorms)
            {
                unsigned short usNorm;
                for (usNorm = 0; usNorm < uiNormalFactor; usNorm++)
                {
                    unsigned int uiNNewIndex = uiNewIndex +
                        usNorm * usVerts;
                    unsigned int uiNOldIndex = uiOldIndex +
                        usNorm * usVerts;
                    pkStripNorms[uiNNewIndex] = pkShapeNorms[uiNOldIndex];
                }
            }
            if (pkShapeColors)
                pkStripColors[uiNewIndex] = pkShapeColors[uiOldIndex];
            if (pkShapeUVs)
            {
                unsigned short usUVSet;
                for (usUVSet = 0; 
                    usUVSet < pkShapeData->GetTextureSets(); usUVSet++)
                {
                    unsigned int uiTNewIndex = uiNewIndex + 
                        usUVSet * usVerts;
                    unsigned int uiTOldIndex = uiOldIndex + 
                        usUVSet * usVerts;
                    pkStripUVs[uiTNewIndex] = pkShapeUVs[uiTOldIndex];
                }
            }
        }
    }

    //  Need to adjust the indices in the skinned object!
    for (usSection = 0; usSection < usNumSections; usSection++)
    {
        if (pkSkinInstance)
        {
            NiSkinData* pkSkinData = pkSkinInstance->GetSkinData();
            if (pkSkinData && 
                pkSkinData->GetBoneData()[0].m_pkBoneVertData != NULL)
            {
                unsigned int uiBone;
                unsigned int uiBoneCount = pkSkinData->GetBoneCount();
                NiSkinData::BoneData* pkBoneData = 
                    pkSkinData->GetBoneData();

                for (uiBone = 0; uiBone < uiBoneCount; uiBone++)
                {
                    NiSkinData::BoneVertData* pkBoneVertData = 
                        pkBoneData[uiBone].m_pkBoneVertData;

                    unsigned short usVert;
                    for (usVert = 0; 
                        usVert < pkBoneData[uiBone].m_usVerts; 
                        usVert++)
                    {
                        pkBoneVertData[usVert].m_usVert = FindNewIndex(
                            pkBoneVertData[usVert].m_usVert, 
                            &(pOldPG[usSection]), &(pNewPG[usSection]));
                    }
                }

                NiSkinPartition* pkPartition = 
                    pkSkinInstance->GetSkinPartition();
                if (pkPartition)
                {
                }

                pkSkinData->SortAndMergeBoneData();
            }
        }
    }

    NiDelete [] pOldPG;
    NiDelete [] pNewPG;

    NiTriStrips* pkStrips = NiNew NiTriStrips(pkStripsData);
    NIASSERT(pkStrips != NULL);

    CopyMembers(pkStrips, pkTriShape);

    return pkStrips;
}
#endif  //#if defined(_STRIPIFY_USE_NVIDIA_)
//---------------------------------------------------------------------------
#if !defined(_STRIPIFY_USE_NVIDIA_)
NiTriStrips* NiStripify::Stripify_Xbox(NiTriShape* pkTriShape)
{
    //  We need to update the indices in a skinned object also
    NiSkinInstance* pkSkinInstance = pkTriShape->GetSkinInstance();

    if (ms_pfnOkayCallback != NULL &&
        !ms_pfnOkayCallback(pkTriShape, ms_pvCallbackData))
    {
        return 0;
    }

    unsigned int uiNumVerts;
    unsigned int uiIndices_In;
    unsigned short* pusIndices_In;
    DWORD   dwIndices_Out = 0;
    unsigned short* pusIndices_Out = 0;
    unsigned short* pusIndices;
    unsigned short* pusVertexPermutation = 0;
    unsigned short usShapeTris;
    NiTriStripsData* pkStripsData;
    NiTriShapeData* pkShapeData;

    uiNumVerts      = pkTriShape->GetVertexCount();
    uiIndices_In    = pkTriShape->GetTriListLength();
    pusIndices_In   = pkTriShape->GetTriList();
    usShapeTris     = pkTriShape->GetTriangleCount();

    // Special case for stripifying NiSkinPartitions with the 
    // Xbox algorithm 
    if (pkSkinInstance && pkSkinInstance->GetSkinPartition())
    {
        return StripifyPartitions_Xbox(pkSkinInstance->GetSkinPartition());
    }

    XBStripify(uiIndices_In / 3, pusIndices_In, &dwIndices_Out,
        &pusIndices_Out);

    XBComputeVertexPermutation(dwIndices_Out, pusIndices_Out, uiNumVerts,
        &pusVertexPermutation);

    // WARNING: NiTriStripsData only supports strip lengths that can
    // be contained in an unsigned short.  The stripify code might return
    // lengths longer than that.  Silently fail this with NULL.
    // Artists should separate their tri shapes into multiple nodes
    // or not use the "merge sibling tri shape" optimization to avoid
    // this happening.
    if ((dwIndices_Out - 2) > 0xFFFF)
    {
        NiExternalDelete[](pusIndices_Out);
        if (ms_pfnErrorCallback)
        {
            char acError[256];
            NiSprintf(acError, 256, "Stripped tri shape too large to stripify"
                " (%u out of %u max indices.)  Tri shape will not be "
                "stripped.  Separate large tri shapes into multiple "
                "nodes or turn off the \"merge sibling tri shapes\" "
                "optimization.", dwIndices_Out - 2, 0xFFFF);
            ms_pfnErrorCallback(STRIPPED_TRISHAPE_TOO_LARGE, pkTriShape,
                acError);

        }

        return NULL;
    }

    NiGeometryData::DataFlags eNBTMethod = 
        pkTriShape->GetNormalBinormalTangentMethod();

    // pusIncices_Out was externally allocated, so we can't pass it into
    // a new NiTriStripsData be cause the Gamebryo object would assume
    // responsibility for deleting it but it hasn't been tracked by the 
    // memory manager.  We need to allocate the data using the memory manager
    pusIndices = NiAlloc(unsigned short, dwIndices_Out);
    memcpy(pusIndices, pusIndices_Out, dwIndices_Out *
        sizeof(unsigned short));
    // usStripsTris does not necessarily equal usShapeTris, since the
    // stripping code will eliminate degenerate triangles, i.e. triangles 
    // that do not have 3 unique vertices.
    unsigned short* pusStripLengths = NiAlloc(unsigned short, 1);
    pusStripLengths[0] = (unsigned short)dwIndices_Out;
    pkStripsData = NiNew NiTriStripsData(uiNumVerts, NULL, NULL, NULL, NULL,
        0, eNBTMethod, (unsigned short)dwIndices_Out - 2, 1, pusStripLengths,
        pusIndices);
    NIASSERT(pkStripsData != NULL);

    // update stats
    ms_knVStats.m_uiShapeTriangles += usShapeTris;
    ms_kStats.m_uiShapeTriangles += usShapeTris;
    ms_kStats.m_uiStripsTriangles += (unsigned short)dwIndices_Out - 2;
    ms_kStats.m_uiStrips += 1;
    ms_kStats.m_uiConvertedShapes++;
    ms_kStats.m_uiStripLengthSum += dwIndices_Out;

    pkShapeData = (NiTriShapeData *) pkTriShape->GetModelData();
    NIASSERT(NiIsKindOf(NiTriShapeData, pkShapeData));

    pkStripsData->SetConsistency(pkShapeData->GetConsistency());
    pkStripsData->SetCompressFlags(pkShapeData->GetCompressFlags());
    pkStripsData->SetKeepFlags(pkShapeData->GetKeepFlags());

    pkStripsData->CreateVertices(false);

    //  Need to remap the vertices!
    unsigned int uiIndex, uiOldIndex;

    NiPoint3* pkShapeVerts = pkShapeData->GetVertices();
    NiPoint3* pkStripVerts = pkStripsData->GetVertices();

    NiPoint3* pkShapeNorms = 0;
    NiPoint3* pkStripNorms = 0;
    NiColorA* pkShapeColors = 0;
    NiColorA* pkStripColors = 0;
    NiPoint2* pkShapeUVs = 0;
    NiPoint2* pkStripUVs = 0;

    if (pkShapeData->GetNormals() != NULL)
    {
        pkStripsData->CreateNormals(false);
        pkShapeNorms = pkShapeData->GetNormals();
        pkStripNorms = pkStripsData->GetNormals();
    }

    if (pkShapeData->GetColors() != NULL)
    {
        pkStripsData->CreateColors(false);
        pkShapeColors = pkShapeData->GetColors();
        pkStripColors = pkStripsData->GetColors();
    }

    if (pkShapeData->GetTextures() != NULL)
    {
        pkStripsData->CreateTextures(false, 
            pkShapeData->GetTextureSets());

        pkShapeUVs = pkShapeData->GetTextures();
        pkStripUVs = pkStripsData->GetTextures();
    }

    unsigned int uiNormalFactor = 
        (eNBTMethod == NiGeometryData::NBT_METHOD_NONE ? 1 : 3);

    for (uiIndex = 0; uiIndex < uiNumVerts; uiIndex++)
    {
        // New index
        uiOldIndex = pusVertexPermutation[uiIndex];

        unsigned int uiSize = sizeof(NiPoint3);
        NiMemcpy(&pkStripVerts[uiIndex], &pkShapeVerts[uiOldIndex], uiSize);
        if (pkShapeNorms)
        {
            unsigned short usNorm;
            for (usNorm = 0; usNorm < uiNormalFactor; usNorm++)
            {
                unsigned int uiNNewIndex = uiIndex + usNorm * uiNumVerts;
                unsigned int uiNOldIndex = uiOldIndex + usNorm * uiNumVerts;
                pkStripNorms[uiNNewIndex] = pkShapeNorms[uiNOldIndex];
            }
        }
        if (pkShapeColors)
            pkStripColors[uiIndex] = pkShapeColors[uiOldIndex];
        if (pkShapeUVs)
        {
            unsigned short usUVSet;
            for (usUVSet = 0; usUVSet < pkShapeData->GetTextureSets(); 
                usUVSet++)
            {
                unsigned int uiTNewIndex = uiIndex + usUVSet * uiNumVerts;
                unsigned int uiTOldIndex = uiOldIndex + usUVSet * uiNumVerts;
                pkStripUVs[uiTNewIndex] = pkShapeUVs[uiTOldIndex];
            }
        }
    }
    pkStripsData->SetBound(pkShapeData->GetBound());

    // Check for a morpher controller
    NiTimeController* pkCtrl;
    for (pkCtrl = pkTriShape->GetControllers(); pkCtrl != NULL;
            pkCtrl = pkCtrl->GetNext())
    {
        if (NiIsKindOf(NiGeomMorpherController, pkCtrl))
        {
            NiGeomMorpherController* pkMorph = (NiGeomMorpherController*)
                pkCtrl;

            unsigned int uiTarget;
            for (uiTarget = 0; uiTarget < pkMorph->GetNumTargets();
                uiTarget++)
            {
                NiMorphData::MorphTarget* pkTarget = 
                    pkMorph->GetTarget(uiTarget);

                // Create the New Verts
                NiPoint3* pkNewVert = NiNew NiPoint3[uiNumVerts];
                NiPoint3* pkOldVert = pkTarget->GetTargetVerts();

                // ReOrder
                for (uiIndex = 0; uiIndex < uiNumVerts; uiIndex++)
                {
                    unsigned int uiSize = sizeof(NiPoint3);
                    NiMemcpy(&pkNewVert[uiIndex], 
                        &pkOldVert[pusVertexPermutation[uiIndex]], uiSize);
                }

                // Replace the Verts
                pkTarget->ReplaceTargetVerts(pkNewVert);
            }
        }
    }

    //  Need to adjust the indices in the skinned object!

    // Build reverse lookup list
    unsigned short* pusReversePermutation = NiAlloc(unsigned short,
        dwIndices_Out);
    for (uiIndex = 0; uiIndex < uiNumVerts; uiIndex++)
    {
        pusReversePermutation[pusVertexPermutation[uiIndex]] = uiIndex;
    }

    if (pkSkinInstance)
    {
        NiSkinData* pkSkinData = pkSkinInstance->GetSkinData();
        if (pkSkinData && 
            pkSkinData->GetBoneData()[0].m_pkBoneVertData != NULL)
        {
            unsigned int uiBone;
            unsigned int uiBoneCount = pkSkinData->GetBoneCount();
            NiSkinData::BoneData* pkBoneData = 
                pkSkinData->GetBoneData();

            for (uiBone = 0; uiBone < uiBoneCount; uiBone++)
            {
                NiSkinData::BoneVertData* pkBoneVertData = 
                    pkBoneData[uiBone].m_pkBoneVertData;

                unsigned short usVert;
                for (usVert = 0; usVert < pkBoneData[uiBone].m_usVerts; 
                    usVert++)
                {
                    pkBoneVertData[usVert].m_usVert = 
                        pusReversePermutation[
                            pkBoneVertData[usVert].m_usVert];
                }
            }

            pkSkinData->SortAndMergeBoneData();
        }
    }

    NiFree(pusReversePermutation);
    NiExternalDelete[](pusVertexPermutation);
    NiExternalDelete[](pusIndices_Out);

    NiTriStrips* pkStrips = NiNew NiTriStrips(pkStripsData);
    NIASSERT(pkStrips != NULL);

    CopyMembers(pkStrips, pkTriShape);

    return pkStrips;
}
#endif  //#if !defined(_STRIPIFY_USE_NVIDIA_)
//---------------------------------------------------------------------------
void NiStripify::Stripify(NiSkinPartition* pkSkinPartition)
{
    unsigned int uiPartitions = pkSkinPartition->GetPartitionCount();

    for (unsigned int i = 0; i < uiPartitions; i++)
    {
        NiSkinPartition::Partition* pkPartition =
            &pkSkinPartition->GetPartitions()[i];
        unsigned short usStripTris;
        unsigned short* pusStripLists;

        NiStripMaker kStrips(pkPartition->m_pusTriList,
            pkPartition->m_usTriangles);

        kStrips.GetStripData(usStripTris, pkPartition->m_usStrips,
            pkPartition->m_pusStripLengths, pusStripLists);

        NiDelete[] pkPartition->m_pusTriList;
        pkPartition->m_pusTriList = pusStripLists;
        pkPartition->m_usTriangles = usStripTris;
    }
}
//---------------------------------------------------------------------------
#if defined(_STRIPIFY_USE_NVIDIA_)
NiTriStrips* NiStripify::StripifyPartitions_nVidia(
    NiSkinPartition* pkSkinPartition)
{
    unsigned int uiPartitions = pkSkinPartition->GetPartitionCount();

    // Need to stripify the partitions themselves, as they pack
    // the vertices per partition.
    NiSkinPartition::Partition* pkPartitions = 
        pkSkinPartition->GetPartitions();
    for (unsigned int i = 0; i < uiPartitions; i++)
    {
        NiSkinPartition::Partition* pkPartition = &pkPartitions[i];

        unsigned short usStripsTris;
        unsigned short usStrips;
        unsigned short* pusStripLists;
        unsigned short usNumSections;
        unsigned short* pusStripLengths;
        PrimitiveGroup* pOldPG = 0;
        PrimitiveGroup* pNewPG = 0;

        Run_nVidia_Stripper(pkPartition->m_pusTriList, 
            pkPartition->m_usTriangles, pkPartition->m_usVertices, 
            &pOldPG, &pNewPG, usNumSections);

        if (!Get_nVidia_Stripper_Results(pOldPG, pNewPG, usNumSections, 
            usStripsTris, usStrips, pusStripLengths, pusStripLists))
        {
            //  Skip this one!
            continue;
        }

        NIASSERT(usNumSections == 1);

        NiDelete[] pkPartition->m_pusTriList;
        pkPartition->m_pusTriList = pusStripLists;
        pkPartition->m_usTriangles = usStripsTris;
        pkPartition->m_usStrips = usStrips;
        pkPartition->m_pusStripLengths = pusStripLengths;
        
        unsigned int ui;

#ifdef VERBOSE_LOGGING
        // Testing - Dump the index remapping
        char szTemp[1024];

        for (ui = 0; ui < pOldPG->numIndices; ui++)
        {
            NiSprintf(szTemp, 1024, "Index %5d - Old %5d - New %5d\n",
                ui, pOldPG->indices[ui], pNewPG->indices[ui]);
            OutputDebugString(szTemp);
        }
#endif

        // Remap the vertex mappings in the partition
        // We just want to remap the vertex mapping...
        unsigned short* pusVertexMap = 
            NiAlloc(unsigned short, pkPartition->m_usVertices);
        NIASSERT(pusVertexMap);

        unsigned short usBonePreVertex = pkPartition->m_usBonesPerVertex;
        float* pfWeights = 
            NiAlloc(float, usBonePreVertex * pkPartition->m_usVertices);
        NIASSERT(pfWeights);

        unsigned char* pucBonePalette = 0;
        if (pkPartition->m_pucBonePalette)
        {
            pucBonePalette = NiAlloc(unsigned char, usBonePreVertex * 
                pkPartition->m_usVertices);
            NIASSERT(pucBonePalette);
        }

        unsigned int uiOldIndex, uiNewIndex;

        for (ui = 0; ui < pNewPG->numIndices; ui++)
        {
            uiOldIndex = pOldPG->indices[ui];
            uiNewIndex = pNewPG->indices[ui];

            NIASSERT(uiOldIndex < pkPartition->m_usVertices);
            NIASSERT(uiNewIndex < pkPartition->m_usVertices);

            pusVertexMap[uiNewIndex] = 
                pkPartition->m_pusVertexMap[uiOldIndex];

            for (unsigned int uj = 0; uj < usBonePreVertex; uj++)
            {
                pfWeights[uiNewIndex * usBonePreVertex + uj] = 
                    pkPartition->m_pfWeights[
                        uiOldIndex * usBonePreVertex + uj];

                if (pucBonePalette)
                {
                    pucBonePalette[uiNewIndex * usBonePreVertex + uj] = 
                        pkPartition->m_pucBonePalette[
                            uiOldIndex * usBonePreVertex + uj];
                }
            }
        }
#ifdef VERBOSE_LOGGING
        for (ui = 0; ui < pkPartition->m_usVertices; ui++)
        {
            NiSprintf(szTemp, 1024, "VertexMapEntry %5d - Old %5d - New %5d\n",
                ui, pkPartition->m_pusVertexMap[ui], pusVertexMap[ui]);
            OutputDebugString(szTemp);
        }
#endif
        NiDelete [] pkPartition->m_pusVertexMap;
        pkPartition->m_pusVertexMap = pusVertexMap;
        NiDelete [] pkPartition->m_pfWeights;
        pkPartition->m_pfWeights = pfWeights;
        NiDelete [] pkPartition->m_pucBonePalette;
        pkPartition->m_pucBonePalette = pucBonePalette;
    }

    return NULL;
}
#endif  //#if defined(_STRIPIFY_USE_NVIDIA_)
//---------------------------------------------------------------------------
#if !defined(_STRIPIFY_USE_NVIDIA_)
NiTriStrips* NiStripify::StripifyPartitions_Xbox(
    NiSkinPartition* pkSkinPartition)
{
    unsigned int uiPartitions = pkSkinPartition->GetPartitionCount();

    // Need to stripify the partitions themselves, as they pack
    // the vertices per partition.
    NiSkinPartition::Partition* pkPartitions = 
        pkSkinPartition->GetPartitions();
    for (unsigned int i = 0; i < uiPartitions; i++)
    {
        NiSkinPartition::Partition* pkPartition = &pkPartitions[i];

        unsigned int uiNumVerts;
        unsigned int uiIndices_In;
        unsigned short* pusIndices_In;
        DWORD   dwIndices_Out = 0;
        unsigned short* pusIndices_Out = 0;
        unsigned short* pusIndices;
        unsigned short* pusVertexPermutation = 0;
        unsigned short usShapeTris;

        uiNumVerts = pkPartition->m_usVertices;
        uiIndices_In = pkPartition->m_usTriangles * 3;
        pusIndices_In = pkPartition->m_pusTriList;
        usShapeTris = pkPartition->m_usTriangles;

        XBStripify(usShapeTris, pusIndices_In, &dwIndices_Out,
            &pusIndices_Out);

        XBComputeVertexPermutation(dwIndices_Out, pusIndices_Out, uiNumVerts,
            &pusVertexPermutation);

        // WARNING: NiTriStripsData only supports strip lengths that can
        // be contained in an unsigned short.  The stripify code might return
        // lengths longer than that.  Silently fail this with NULL.
        // Artists should separate their tri shapes into multiple nodes
        // or not use the "merge sibling tri shape" optimization to avoid
        // this happening.
        if ((dwIndices_Out - 2) > 0xFFFF)
        {
            NiExternalDelete[](pusIndices_Out);
            if (ms_pfnErrorCallback)
            {
                char acError[256];
                NiSprintf(acError, 256, "Stripped partition too large "
                    "(%u out of %u max indices.) Partition will not be "
                    "stripped.", dwIndices_Out - 2, 0xFFFF);
                ms_pfnErrorCallback(STRIPPED_PARTITION_TOO_LARGE,
                    pkSkinPartition, acError);

            }
            continue;
        }

        unsigned short* pusStripLengths = NiAlloc(unsigned short, 1);
        pusStripLengths[0] = (unsigned short)dwIndices_Out;
    
        // pusIncices_Out was externally allocated, so we can't let the
        // partition take responsibility for deleting it because it hasn't
        // been tracked by the memory manager.  We need to allocate
        // the data using the memory manager
        pusIndices = NiAlloc(unsigned short, dwIndices_Out);
        memcpy(pusIndices, pusIndices_Out, dwIndices_Out * 
            sizeof(unsigned short));
        pkPartition->m_pusTriList = pusIndices;
        pkPartition->m_usTriangles = (unsigned short)dwIndices_Out - 2;
        pkPartition->m_usStrips = 1;
        pkPartition->m_pusStripLengths = pusStripLengths;

        unsigned int ui;

        NiFree(pusIndices_In);
        NiExternalDelete[](pusIndices_Out);

        // Remap the vertex mappings in the partition
        // We just want to remap the vertex mapping...
        unsigned short* pusVertexMap = 
            NiAlloc(unsigned short, pkPartition->m_usVertices);
        NIASSERT(pusVertexMap);

        unsigned short usBonePreVertex = pkPartition->m_usBonesPerVertex;
        float* pfWeights = 
            NiAlloc(float, usBonePreVertex * pkPartition->m_usVertices);
        NIASSERT(pfWeights);

        unsigned char* pucBonePalette = 0;
        if (pkPartition->m_pucBonePalette)
        {
            pucBonePalette = 
                NiAlloc(unsigned char, 
                usBonePreVertex * pkPartition->m_usVertices);
            NIASSERT(pucBonePalette);
        }

        unsigned int uiOldIndex;

        for (ui = 0; ui < uiNumVerts; ui++)
        {
            uiOldIndex = pusVertexPermutation[ui];

            NIASSERT(uiOldIndex < pkPartition->m_usVertices);

            pusVertexMap[ui] = pkPartition->m_pusVertexMap[uiOldIndex];

            for (unsigned int uj = 0; uj < usBonePreVertex; uj++)
            {
                pfWeights[ui * usBonePreVertex + uj] = 
                    pkPartition->m_pfWeights[
                        uiOldIndex * usBonePreVertex + uj];

                if (pucBonePalette)
                {
                    pucBonePalette[ui * usBonePreVertex + uj] = 
                        pkPartition->m_pucBonePalette[
                            uiOldIndex * usBonePreVertex + uj];
                }
            }
        }
#ifdef VERBOSE_LOGGING
        char szTemp[256];
        for (ui = 0; ui < pkPartition->m_usVertices; ui++)
        {
            NiSprintf(szTemp, 256, "VertexMapEntry %5d - Old %5d - New %5d\n",
                ui, pkPartition->m_pusVertexMap[ui], pusVertexMap[ui]);
            OutputDebugString(szTemp);
        }
#endif

        NiFree(pkPartition->m_pusVertexMap);
        pkPartition->m_pusVertexMap = pusVertexMap;
        NiFree(pkPartition->m_pfWeights);
        pkPartition->m_pfWeights = pfWeights;
        NiFree(pkPartition->m_pucBonePalette);
        pkPartition->m_pucBonePalette = pucBonePalette;
    }

    // Now, just create a dummy tristrip for this object...
    // This is the tricky part, as the partitions are the actual tri-stripped
    // geometry, not the data itself.

    return NULL;
}
#endif  //#if !defined(_STRIPIFY_USE_NVIDIA_)
//---------------------------------------------------------------------------
void NiStripify::SetOkayCallback(OkayCallback pfnCallback, void *pvData)
{
    ms_pfnOkayCallback = pfnCallback;
    ms_pvCallbackData = pvData;
}

//---------------------------------------------------------------------------
void NiStripify::GetOkayCallback(OkayCallback& pfnCallback, void*& pvData)
{
    pfnCallback = ms_pfnOkayCallback;
    pvData = ms_pvCallbackData;
}
//---------------------------------------------------------------------------
void NiStripify::SetFinishedCallback(FinishedCallback pfnCallback)
{
    ms_pfnFinishedCallback = pfnCallback;
}

//---------------------------------------------------------------------------
void NiStripify::GetFinishedCallback(FinishedCallback& pfnCallback)
{
    pfnCallback = ms_pfnFinishedCallback;
}
//---------------------------------------------------------------------------
void NiStripify::SetErrorCallback(ErrorCallback pfnCallback)
{
    ms_pfnErrorCallback = pfnCallback;
}
//---------------------------------------------------------------------------
void NiStripify::GetErrorCallback(ErrorCallback& pfnCallback)
{
    pfnCallback = ms_pfnErrorCallback;
}
//---------------------------------------------------------------------------
void NiStripify::GetStats(Stats& kStats)
{
    kStats = ms_kStats;
}
//---------------------------------------------------------------------------
void NiStripify::GetnVStats(Stats& kStats)
{
    kStats = ms_knVStats;
}
//---------------------------------------------------------------------------
void NiStripify::ResetStats()
{
    ms_kStats.Reset();
}
//---------------------------------------------------------------------------
void NiStripify::ResetnVStats()
{
    ms_knVStats.Reset();
}

//---------------------------------------------------------------------------
NiStripify::Stats::Stats()
{
    Reset();
}

//---------------------------------------------------------------------------
void NiStripify::Stats::Reset()
{
    m_uiConvertedShapes =
    m_uiShapeTriangles =
    m_uiStripsTriangles =
    m_uiStrips =
    m_uiStripLengthSum = 0;
}

//---------------------------------------------------------------------------
void NiStripify::ResetIndexSum(void)
{
    ms_uiIndexSum = 0;
}
//---------------------------------------------------------------------------
unsigned int NiStripify::GetIndexSum(void)
{
    return ms_uiIndexSum;
}
//---------------------------------------------------------------------------
void NiStripify::SetUse_NextGeneration(bool bUse_NextGeneration)
{
    ms_bUse_NextGeneration = bUse_NextGeneration;
}
//---------------------------------------------------------------------------
#if defined(_STRIPIFY_USE_NVIDIA_)
//---------------------------------------------------------------------------
void NiStripify::Run_nVidia_Stripper(const unsigned short* pusTriList, 
    unsigned int uiTriangles, unsigned int uiNumVerts, 
    PrimitiveGroup** ppOldPG, PrimitiveGroup** ppNewPG, 
    unsigned short& usNumSections)
{
    //stripify!
    GenerateStrips(pusTriList, uiTriangles * 3, ppOldPG, &usNumSections);
    
    //remap!
    RemapIndices(*ppOldPG, usNumSections, uiNumVerts, ppNewPG);

    //  Now, dump out the details...
    NIASSERT(*ppNewPG);
}
//---------------------------------------------------------------------------
bool NiStripify::Get_nVidia_Stripper_Results(PrimitiveGroup* pOldPG, 
    PrimitiveGroup* pNewPG, unsigned short& usNumSections, 
    unsigned short& usTriangles, unsigned short& usStrips, 
    unsigned short*& pusStripLengths, unsigned short*& pusStripLists)
{
    unsigned short usTriCount = 0;
    unsigned short usIndexCount = 0;

    ms_knVStats.m_uiConvertedShapes++;
    ms_knVStats.m_uiStrips += usNumSections;

    unsigned int ui;

#ifdef VERBOSE_LOGGING
    char szTemp[1024];
    NiSprintf(szTemp, 1024, "NumSections.................%d\n", usNumSections);
    NiOutputDebugString(szTemp);
#endif

    //  Sum up the totals for allocation sizes...
    for (ui = 0; ui < usNumSections; ui++)
    {
        if (pNewPG[ui].type == PT_LIST)
        {
            NIASSERT(0);
        }
        else
        if (pNewPG[ui].type == PT_STRIP)
        {
        }
        else
        if (pNewPG[ui].type == PT_FAN)
        {
            NIASSERT(0);
        }

        const unsigned int uiMaxUnsignedShort = (unsigned short) (~0);
        if ((unsigned int)(usIndexCount + pNewPG[ui].numIndices) > 
            uiMaxUnsignedShort)
        {
            NiOutputDebugString(" Stripify would exceed max unsigned short, "
                "aborting this object.\n");
            return false;
        }

        ms_knVStats.m_uiStripLengthSum += pNewPG[ui].numIndices;
        ms_knVStats.m_uiStripsTriangles += pNewPG[ui].numIndices - 2;

        ms_uiIndexSum += pNewPG[ui].numIndices;

        usIndexCount += pNewPG[ui].numIndices;
        usTriCount += pNewPG[ui].numIndices - 2;
#ifdef VERBOSE_LOGGING
        NiSprintf(szTemp, 1024, "    %d - Indices............%d\n", ui,
            pNewPG[ui].numIndices);
        NiOutputDebugString(szTemp);
#endif
    }

    usTriangles = usTriCount;
    usStrips = usNumSections;
    pusStripLengths = NiAlloc(unsigned short, usStrips);
    NIASSERT(pusStripLengths);
    pusStripLists = NiAlloc(unsigned short, usIndexCount);
    NIASSERT(pusStripLists);

    //  Copy the data
    usIndexCount = 0;

    for (ui = 0; ui < usNumSections; ui++)
    {
        pusStripLengths[ui] = pNewPG[ui].numIndices;

        unsigned short* pusTempList = &(pusStripLists[usIndexCount]);

        unsigned int uiSize = sizeof(unsigned short) * pNewPG[ui].numIndices;
        NiMemcpy(pusTempList, pNewPG[ui].indices, uiSize);

        usIndexCount += pNewPG[ui].numIndices;
    }

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiStripify::FindNewIndex(unsigned int uiOldIndex, 
    PrimitiveGroup* pOldPG, PrimitiveGroup* pNewPG)
{
    unsigned int uiIndex = 0xFFFFFFFF;

    for (unsigned int ui = 0; ui < pOldPG->numIndices; ui++)
    {
        if (pOldPG->indices[ui] == uiOldIndex)
            return pNewPG->indices[ui];
    }

    return uiIndex;
}
//---------------------------------------------------------------------------
#endif  //#if defined(_STRIPIFY_USE_NVIDIA_)
//---------------------------------------------------------------------------
