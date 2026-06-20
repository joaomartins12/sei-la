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

#include "stdafx.h"
#include "resource.h"
#include "NiNormalVisualizerPlugin.h"

NiImplementRTTI(NiNormalVisualizerPlugin, NiPlugin);

float NiNormalVisualizerPlugin::ms_fNormalScale = 0.1f;
float NiNormalVisualizerPlugin::ms_fBoundScale = 1.0f;
//---------------------------------------------------------------------------
NiNormalVisualizerPlugin::NiNormalVisualizerPlugin() :
    NiPlugin("Normal Visualizer", "1.0", 
        "Creates geometry representing the normals of an object.",
        "Searches the scene graph for geometry and creates geometry "
        "representing the normals of an object")
{
}
//---------------------------------------------------------------------------
bool NiNormalVisualizerPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "NiNormalVisualizerPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiNormalVisualizerPlugin::FindGeometry(NiAVObject* pkRoot, 
    NiTList<NiTriBasedGeom*>& kGeoms)
{
    if(NiIsKindOf(NiNode, pkRoot))
    {
        NiNode* pkNode = (NiNode*) pkRoot;

        for(unsigned int ui=0; ui < pkNode->GetArrayCount(); ui++)
        {
            if(pkNode->GetAt(ui))
            {
                FindGeometry(pkNode->GetAt(ui), kGeoms);
            }
        }
    }
    else if (NiIsKindOf(NiTriBasedGeom, pkRoot))
    {
        NiTriBasedGeom* pkGeom = (NiTriBasedGeom*)pkRoot;
        kGeoms.AddTail(pkGeom);
    }
}

//---------------------------------------------------------------------------
NiExecutionResultPtr NiNormalVisualizerPlugin::Execute(
    const NiPluginInfo* pkInfo)
{
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*)
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));

    if (!pkSGSharedData)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    NiTList<NiTriBasedGeom*> kGeoms;
    for (unsigned int ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spScene = pkSGSharedData->GetRootAt(ui);
        
        if (spScene)
            ms_fBoundScale = spScene->GetWorldBound().GetRadius() * 2.0f;
        
        FindGeometry(spScene, kGeoms);
    }

    NiTListIterator kIter = kGeoms.GetHeadPos();
    while(kIter)
    {
        NiTriBasedGeom* pkGeom = kGeoms.GetNext(kIter);
        const unsigned int uiGeomVerts = pkGeom->GetVertexCount();
        const unsigned int uiGeomTris = pkGeom->GetTriangleCount();
        NiPoint3* pkSrcNormals = pkGeom->GetNormals();
        NiPoint3* pkSrcVertices = pkGeom->GetVertices();

        NiNode* pkParent = pkGeom->GetParent();

        if (pkSrcNormals == NULL)
            continue;

        if (pkParent == NULL)
            continue;

        unsigned short usVertices = 2*uiGeomVerts; 
        NiPoint3* pkVertex = NiNew NiPoint3[usVertices]; 
        NiColorA* pkColor = NiNew NiColorA[usVertices]; 
        NiPoint2* pkTexture = NULL; 
        unsigned short usNumTextureSets = 0; 
        NiGeometryData::DataFlags eNBTMethod = NiGeometryData::NBT_METHOD_NONE;
        NiBool* pbFlags = NiAlloc(NiBool, usVertices);

        float fObjScale = pkGeom->GetScale();
        unsigned int uj = 0;
        for (unsigned int ui = 0; ui < uiGeomVerts; ui++)
        {
            NiPoint3 kCurrentNormal = pkSrcNormals[ui];
            NiPoint3 kBeginLinePos = pkSrcVertices[ui];
            kCurrentNormal.Unitize();
            NiPoint3 kEndLinePos = kBeginLinePos + ((ms_fNormalScale * 
                ms_fBoundScale * 1.0f/fObjScale) * kCurrentNormal);
            pkVertex[uj] = kBeginLinePos;
            pbFlags[uj] = true;
            pkColor[uj] = NiColorA(1.0f, 0.0f, 0.0f, 1.0f);
            uj++;
            pkVertex[uj] = kEndLinePos;
            pbFlags[uj] = false;
            pkColor[uj] = NiColorA(1.0f, 0.0f, 0.0f, 1.0f);
            uj++;
        }

        NiLines* pkNormalLines = NiNew NiLines(usVertices, pkVertex, pkColor, 
            pkTexture, usNumTextureSets, eNBTMethod, pbFlags);

        pkNormalLines->SetTranslate(pkGeom->GetTranslate());
        pkNormalLines->SetRotate(pkGeom->GetRotate());
        pkNormalLines->SetScale(pkGeom->GetScale());

        pkParent->AttachChild(pkNormalLines);

    }
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
NiPluginInfo* NiNormalVisualizerPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NiNormalVisualizerPlugin");
    pkPluginInfo->SetType("PROCESS");
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NiNormalVisualizerPlugin::HasManagementDialog()
{
    return false;
}
//---------------------------------------------------------------------------
bool NiNormalVisualizerPlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
    NiWindowRef hWndParent)
{
    return false;
}
//---------------------------------------------------------------------------
      