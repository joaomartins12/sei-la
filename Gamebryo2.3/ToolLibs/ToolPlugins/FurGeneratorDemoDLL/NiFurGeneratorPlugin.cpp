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
#include "NiFurGeneratorPlugin.h"
#include "NiFurGeneratorDLLDefines.h"
#include <NiExporterOptionsSharedData.h>

#include <NiShaderFactory.h>


struct RGBAPixel
{
    unsigned char ucRed;
    unsigned char ucGreen;
    unsigned char ucBlue; 
    unsigned char ucAlpha;
};

// This must match what is in NiOptimization library file: 
// NiOptimize_HelpFunc.cpp
#define NI_BONES_PER_PARTITION_KEY "NiBonesPerPartition"

NiImplementRTTI(NiFurGeneratorPlugin, NiPlugin);
NiFurGeneratorPlugin* NiFurGeneratorPlugin::ms_pkThis = NULL;

//---------------------------------------------------------------------------
NiFurGeneratorPlugin::NiFurGeneratorPlugin() :
    NiPlugin("Lenguins Fur Generator Demo", "1.0", 
    "Generates fur data for use in the Lenguins on Ice.",
    "This a specialized plug-in used to generate fur geometry and a shell "
    "texture atlas to solely be used by the fur in the Lenguins on Ice Demo."),
    m_pkProgressBar(NULL)
{
}
//---------------------------------------------------------------------------
bool NiFurGeneratorPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "NiFurGeneratorPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiFurGeneratorPlugin::PerObjectCallback(NiAVObject* pkObject)
{

    return true;
}
//---------------------------------------------------------------------------
void NiFurGeneratorPlugin::ErrorCallback(unsigned int uiCode, 
    NiObject* pkObj, const char* pcMessage)
{

}
//---------------------------------------------------------------------------

NiExecutionResultPtr NiFurGeneratorPlugin::Execute(
    const NiPluginInfo* pkInfo)
{
    ms_pkThis = this;
    // Get the Scene graph from the shared data
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));
    if (!pkSGSharedData)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    bool bViewerRun = false;
    NiExporterOptionsSharedData* pkExporterSharedData = 
        (NiExporterOptionsSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiExporterOptionsSharedData));


    // Check the PluginInfo to double check the type
    if(pkInfo->GetType() != "PROCESS")
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    Init();


    m_pkProgressBar = NiNew NiProgressDialog("Generating Fur");
    m_pkProgressBar->Create();
    m_pkProgressBar->SetPosition(0);

    // Set fur parameters
    m_fFurLength = 3.5f;
    m_fTextureTilesPerFinUnit = 0.25f;
    m_fShellTextureSize = 512.0f;
    m_uiShellTextureCellSize = (unsigned int)m_fShellTextureSize / 4;
    m_uiShellLevelCount = 16;

    BuildTriShapeList(pkSGSharedData);
    GenerateShellTexture();
    GenerateFur();

    m_pkProgressBar->Destroy();
    NiDelete m_pkProgressBar;
    m_pkProgressBar = NULL;

    Destroy();
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
NiPluginInfo* NiFurGeneratorPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NiFurGeneratorPlugin");
    pkPluginInfo->SetType("PROCESS");
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NiFurGeneratorPlugin::HasManagementDialog()
{
    return false;
}
//---------------------------------------------------------------------------
bool NiFurGeneratorPlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent)
{
    return false;
}
//---------------------------------------------------------------------------
void NiFurGeneratorPlugin::Init()
{
    m_pkTriShapeList = NiNew  NiTPointerList<NiTriShape*>;
}
//---------------------------------------------------------------------------
void NiFurGeneratorPlugin::Destroy()
{
    m_pkTriShapeList->RemoveAll();
    NiDelete m_pkTriShapeList;
}
//---------------------------------------------------------------------------
void NiFurGeneratorPlugin::BuildTriShapeList(NiSceneGraphSharedData* 
    pkSGSharedData)
{
    // Search through the scene for TriShape Objects with "fur" extra data
    for (unsigned int ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spScene = pkSGSharedData->GetRootAt(ui);
        RecusiveTriShapeSearch(spScene);
    }
}
//---------------------------------------------------------------------------
bool NiFurGeneratorPlugin::HasFurData(NiAVObject* pkNode)
{
    if (!pkNode)
        return false;

    const unsigned int uiSize = pkNode->GetExtraDataSize();

    for(unsigned int ui = 0; ui < uiSize; ui++)
    {
      NiStringExtraData* pkExtraData = NiDynamicCast(NiStringExtraData,
          pkNode->GetExtraDataAt(ui));
        
      if (pkExtraData && pkExtraData->GetValue().Contains("fur"))
        return true;
           
    }

    return false;
}
//---------------------------------------------------------------------------
void NiFurGeneratorPlugin::RecusiveTriShapeSearch(NiNode* pkNode)
{

    for (unsigned int ui = 0; ui < pkNode->GetChildCount(); ui++) 
    {
        NiAVObject* pkObject = pkNode->GetAt(ui);
        
        if (NiIsKindOf(NiNode, pkObject))
        {
            RecusiveTriShapeSearch( NiDynamicCast(NiNode,pkObject) );
        }
        else if (NiIsKindOf(NiTriShape, pkObject))
        {
           if(HasFurData(pkObject) || HasFurData(pkObject->GetParent()))
           {
                // Found a TriShape 
                m_pkTriShapeList->AddTail(NiDynamicCast(NiTriShape,pkObject));
           }
        }
        
    }
}
//---------------------------------------------------------------------------
void NiFurGeneratorPlugin::GenerateShellsFromShape(NiTriShape* pkShape,
    NiSortAdjustNode* pkSortNode, char* cpName)
{
    NiTriShapeData* pkTriData = NiDynamicCast(NiTriShapeData,
        pkShape->GetModelData());

    unsigned short usVertCount = pkTriData->GetVertexCount();
    unsigned short usTextureSets = pkTriData->GetTextureSets();
    unsigned int usIndexCount = pkTriData->GetTriangleCount() * 3;

    unsigned short usNewVertCount = usVertCount * m_uiNumberOfShells;
    unsigned int usNewIndexCount = usIndexCount * m_uiNumberOfShells;

    // Must have two texture sets, one for the base texture coordinated, and 
    // another for the shell texture coordinates.
    NIASSERT(usTextureSets >= 2);

    // Use the texture index assigned to the detail map to determine which 
    // texture coordinate set will be used to assign texture coordinates to the
    // shells.
    NiTexturingProperty* pkTextureProp = 
        (NiTexturingProperty*)pkShape->GetProperty(NiProperty::TEXTURING);

    const NiTexturingProperty::Map* pkDetailMap = 
        pkTextureProp->GetDetailMap();

    NIASSERT(pkDetailMap);

    const unsigned int uiDetailMapIndex = pkDetailMap->GetTextureIndex();

    NiPoint3* pkVerts = pkTriData->GetVertices();
    NiPoint3* pkNormals = pkTriData->GetNormals();
    NiPoint2* pkBaseTexCoords = pkTriData->GetTextureSet(0); 
    NiColorA* pkColors = pkTriData->GetColors();
    unsigned short* psTriList =  pkTriData->GetTriList();

    NiPoint3* pkNewVerts = NiNew NiPoint3[usNewVertCount];
    NiPoint3* pkNewNormals = NiNew NiPoint3[usNewVertCount];
    NiPoint2* pkNewTexCoords = NiNew NiPoint2[usNewVertCount*3];
    NiColorA* pkNewColors = NiNew NiColorA[usNewVertCount];

    unsigned short* psNewTriList = 
        (unsigned short*)NiMalloc(sizeof(unsigned short) * usNewIndexCount);

    unsigned short usCurrentVert = 0;
    unsigned int uiCurrentIndex = 0;
    float fShellLength = m_fFurLength / m_uiNumberOfShells;

    for(unsigned int uiShell = 0; uiShell < m_uiNumberOfShells; uiShell++)
    {
        // Duplicate Geometry Data
        for(unsigned short usVID = 0; usVID < usVertCount; usVID++)
        {
            pkNewVerts[usCurrentVert] = pkVerts[usVID];
            pkNewNormals[usCurrentVert] = pkNormals[usVID];
            pkNewTexCoords[usCurrentVert] = pkBaseTexCoords[usVID];
            pkNewTexCoords[usCurrentVert+usNewVertCount] = 
                pkBaseTexCoords[usVID+(uiDetailMapIndex *usVertCount)];

            if (pkColors)
                pkNewColors[usCurrentVert] = pkColors[usVID];
            else
                pkNewColors[usCurrentVert] = NiColorA(1,1,1,1);

            pkNewVerts[usCurrentVert].x += pkNormals[usVID].x * 
                (float)(uiShell+1) * fShellLength;
            pkNewVerts[usCurrentVert].y += pkNormals[usVID].y * 
                (float)(uiShell+1) * fShellLength;
            pkNewVerts[usCurrentVert].z += pkNormals[usVID].z * 
                (float)(uiShell+1) * fShellLength;

         usCurrentVert++;
        }
    
        // Duplicate TriList Data
        for(unsigned int uiIndex = 0; uiIndex < usIndexCount; uiIndex++)
        {
            psNewTriList[uiCurrentIndex++] = psTriList[uiIndex] +
                (usVertCount * uiShell);
        }
    }

    // Generate Texture Coordinates. Two texture coordinates sets are used for
    // the shell texture, the first is the raw texture coordinates pulled in 
    // from the model, the second set is used by the shader to determine which
    // cell in the shell texture atlas to use.
    for(unsigned int uiTri = 0; uiTri < usNewIndexCount;  uiTri+=3)
    {
        unsigned int uiIndex1 = psNewTriList[uiTri];
        unsigned int uiIndex2 = psNewTriList[uiTri+1];
        unsigned int uiIndex3 = psNewTriList[uiTri+2];

        NiPoint3 kPoint1 = pkNewVerts[ uiIndex1 ];
        NiPoint3 kPoint2 = pkNewVerts[ uiIndex2 ];
        NiPoint3 kPoint3 = pkNewVerts[ uiIndex3 ];
    
        uiIndex1 += usNewVertCount * 2;
        uiIndex2 += usNewVertCount * 2;
        uiIndex3 += usNewVertCount * 2;

        unsigned int uiShelLevel = uiTri  / usIndexCount;

        unsigned int xOffset = (uiShelLevel%4) * m_uiShellTextureCellSize;
        unsigned int yOffset = ((uiShelLevel-(uiShelLevel%4))/4) * 
            m_uiShellTextureCellSize;

        xOffset += m_uiShellTextureCellSize / 2;
        yOffset += m_uiShellTextureCellSize / 2;

        pkNewTexCoords[uiIndex1].x = (float)xOffset / m_fShellTextureSize;
        pkNewTexCoords[uiIndex1].y = (float)yOffset / m_fShellTextureSize;

        pkNewTexCoords[uiIndex2].x = (float)xOffset / m_fShellTextureSize;
        pkNewTexCoords[uiIndex2].y = (float)yOffset / m_fShellTextureSize;

        pkNewTexCoords[uiIndex3].x = (float)xOffset / m_fShellTextureSize;
        pkNewTexCoords[uiIndex3].y = (float)yOffset / m_fShellTextureSize;
    }


    // Note Shell Data is created as dynamic data so that application can 
    // easily reduce the active triangle count by the number of triangles 
    // in the base object times the number of shells desired to render
    NiTriShapeDynamicData* pkNewTriData = NiNew NiTriShapeDynamicData(
        usNewVertCount, pkNewVerts, pkNewNormals, pkNewColors, pkNewTexCoords,
        3, NiGeometryData::NBT_METHOD_NONE, usNewIndexCount/3, psNewTriList,
        usNewVertCount, usNewIndexCount/3);


    NiTriShape* pkNewTriShape = NiNew NiTriShape(pkNewTriData);
    pkNewTriShape->SetName(cpName);


    NiVertexColorProperty* pkVCProp = NiNew NiVertexColorProperty();
    pkVCProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E_A_D);
    pkVCProp->SetSourceMode(NiVertexColorProperty::SOURCE_AMB_DIFF);
    pkNewTriShape->AttachProperty(pkVCProp);

    NiMaterialProperty* pkMatProp = NiNew NiMaterialProperty();
    pkNewTriShape->AttachProperty(pkMatProp);


    NiTexturingProperty* pkInTexProp =
        (NiTexturingProperty*)pkShape->GetProperty(NiProperty::TEXTURING);

    NiTexturingProperty* pkOutTexProp = NiNew NiTexturingProperty();

    pkOutTexProp->SetBaseTexture(pkInTexProp->GetBaseTexture());

    
    NiTexturingProperty::ShaderMap* pkShaderMap = 
        NiNew NiTexturingProperty::ShaderMap(m_pkShellTexture,0);
    pkOutTexProp->SetShaderMap(0,pkShaderMap);

    pkNewTriShape->AttachProperty(pkOutTexProp);


    NiSkinInstance* pkSkinInstance = pkShape->GetSkinInstance();

    // Assign shell fur shader to geometry
    if (pkSkinInstance)
    {
        NiSkinInstance* pkNewSkinInstance = 
            CloneSkinningForShells(pkSkinInstance, m_uiNumberOfShells, 
            usVertCount, pkNewTriData->GetVertices());

        pkNewTriShape->SetSkinInstance(pkNewSkinInstance);

        // if the geometry uses a different number of bones per partition than
        // the default, that value will need to be carried over
        NiIntegerExtraData* pkIntED = NiDynamicCast(NiIntegerExtraData, 
            pkShape->GetExtraData(NI_BONES_PER_PARTITION_KEY));
        if (pkIntED)
        {
            pkNewTriShape->AddExtraData((NiExtraData*)pkIntED->Clone());
        }

        NiSingleShaderMaterial* pkShaderMat = 
            NiSingleShaderMaterial::Create("FurShellsSkinning",false);
        pkNewTriShape->ApplyMaterial(pkShaderMat);
        pkNewTriShape->SetActiveMaterial(pkShaderMat);
    }
    else
    {
        NiSingleShaderMaterial* pkShaderMat = 
            NiSingleShaderMaterial::Create("FurShells",false);
        pkNewTriShape->ApplyMaterial(pkShaderMat);
        pkNewTriShape->SetActiveMaterial(pkShaderMat);
    }


    pkSortNode->AttachChild(pkNewTriShape);
}
//---------------------------------------------------------------------------
void NiFurGeneratorPlugin::RemoveDetailMap(NiAVObject* pkNode)
{
    NiTexturingProperty* pkTexProp = 
        (NiTexturingProperty*)pkNode->GetProperty(NiProperty::TEXTURING);

    NIASSERT(pkTexProp);

    // Remove the detail map is it is no longer needed. It only served as way
    // of defining which texture coordinate set to use for the shells and for 
    // visualization purposes while modeling.
    pkTexProp->SetDetailMap(NULL);
}
//---------------------------------------------------------------------------
void NiFurGeneratorPlugin::GenerateFur()
{
    NiTListIterator kPos = m_pkTriShapeList->GetHeadPos();
    
    while (kPos)
    {
        NiTriShape* pkTriShape = m_pkTriShapeList->GetNext(kPos);

        // To ensure correct render order (object->shells->fins) all objects
        // are placed under a NiSortAdjustNode with sorting turned off
        NiSortAdjustNode* pkSortNode = NiNew NiSortAdjustNode();
        pkSortNode->SetSortingMode(NiSortAdjustNode::SORTING_OFF);

        NiNode* pkShapeParent = pkTriShape->GetParent();
        pkShapeParent->DetachChild(pkTriShape);

        pkShapeParent->AttachChild(pkSortNode);
        pkSortNode->AttachChild(pkTriShape);

        m_uiNumberOfShells = 12;
        GenerateShellsFromShape(pkTriShape,pkSortNode, "Shells0");

        m_uiNumberOfShells = 8;
        GenerateShellsFromShape(pkTriShape,pkSortNode, "Shells1");

        m_uiNumberOfShells = 6;
        GenerateShellsFromShape(pkTriShape,pkSortNode, "Shells2");

        m_uiNumberOfShells = 4;
        GenerateShellsFromShape(pkTriShape,pkSortNode, "Shells3");

        GenerateFinsFromShape(pkTriShape,pkSortNode, "Fins");

        // Remove the Detail Map used to store shell the texture coordinate set
        RemoveDetailMap(pkTriShape);
    }
}
//---------------------------------------------------------------------------
void NiFurGeneratorPlugin::GenerateShellTexture()
{
    RGBAPixel* pkShellBase = (RGBAPixel*)NiMalloc(sizeof(RGBAPixel) 
        * m_uiShellTextureCellSize * m_uiShellTextureCellSize);

    // Create base shell texture that all other shell textures will work from
    for(unsigned int uiX = 0; uiX < m_uiShellTextureCellSize; uiX++)
    {
        for(unsigned int uiY = 0; uiY < m_uiShellTextureCellSize; uiY++)
        {
            if (NiRand() % 10 == 0)
            {
                pkShellBase[(uiY*m_uiShellTextureCellSize)+uiX].ucRed = 255;
                pkShellBase[(uiY*m_uiShellTextureCellSize)+uiX].ucGreen = 255;
                pkShellBase[(uiY*m_uiShellTextureCellSize)+uiX].ucBlue = 255;
                pkShellBase[(uiY*m_uiShellTextureCellSize)+uiX].ucAlpha = 255;
            }
            else
            {
                pkShellBase[(uiY*m_uiShellTextureCellSize)+uiX].ucRed = 0;
                pkShellBase[(uiY*m_uiShellTextureCellSize)+uiX].ucGreen = 0;
                pkShellBase[(uiY*m_uiShellTextureCellSize)+uiX].ucAlpha = 0;
            }
        }
    }

    NiPixelData* pkPixData = NiNew NiPixelData(
        (unsigned int)m_fShellTextureSize, (unsigned int)m_fShellTextureSize, 
        NiPixelFormat::RGBA32);

    RGBAPixel* pkPixels = (RGBAPixel*)pkPixData->GetPixels();

    // Create a texture atlas that contains the all the textures for the shells
    for(unsigned int uiShell = 0; uiShell < m_uiShellLevelCount; uiShell++)
    {
        unsigned int xOffset = (uiShell%4) * m_uiShellTextureCellSize;
        unsigned int yOffset = ((uiShell-(uiShell%4)) / 4) * 
            m_uiShellTextureCellSize;

        float fAlpha = (float)uiShell / (float)m_uiShellLevelCount;
        fAlpha = 1.0f - fAlpha;
        unsigned char ucAlpha = (char)(fAlpha * 255.0f);


        for(unsigned int uiX  = 0; uiX < m_uiShellTextureCellSize; uiX++)
        {
            for(unsigned int uiY = 0; uiY < m_uiShellTextureCellSize; uiY++)
            {

                float fDownScale = (float)(rand()%100) / 1000.0f;
                fDownScale = 1 - fDownScale;

                pkShellBase[(uiY * m_uiShellTextureCellSize) + uiX].ucAlpha = 
                    (unsigned char) ((float)pkShellBase[
                        (uiY * m_uiShellTextureCellSize) + uiX].ucAlpha * 
                            fDownScale);

                pkPixels[((yOffset+uiY)*(unsigned int)m_fShellTextureSize)
                    +uiX+xOffset] = pkShellBase[
                        (uiY * m_uiShellTextureCellSize) + uiX];
            }
        }

    }
    
    pkPixData->MarkAsChanged();

    m_pkShellTexture = NiSourceTexture::Create(pkPixData);

    NiFree(pkShellBase);

}
//---------------------------------------------------------------------------
bool NiFurGeneratorPlugin::IsEdgeInList(unsigned short* testEdge, 
    unsigned short* edgeList, int numberOfEdges)
{
    // See if edge is already in the list
    for(int i = 0; i < numberOfEdges; i++)
    {
        if(edgeList[(i<<1)] == testEdge[0] &&
            edgeList[(i<<1)+1] == testEdge[1])
        {
            return true;
        }
        else if(edgeList[(i<<1)] == testEdge[1] && 
            edgeList[(i<<1)+1] == testEdge[0])
        {
            return true;
        }

    }
    return false;
}
//---------------------------------------------------------------------------
void NiFurGeneratorPlugin::GenerateFinsFromShape(NiTriShape* pkShape, 
    NiSortAdjustNode* pkSortNode, char* cpName)
{      
    NiTriShapeData* pkTriData = NiDynamicCast(NiTriShapeData,
        pkShape->GetModelData());

    unsigned short usVertCount = pkTriData->GetVertexCount();
    unsigned int usIndexCount = pkTriData->GetTriangleCount() * 3;
    unsigned int uiPolyCount = pkTriData->GetTriangleCount();
    unsigned short usTextureSets = pkTriData->GetTextureSets();

    NIASSERT(usTextureSets >= 1);

    NiPoint3* pkInVerts = pkTriData->GetVertices();
    NiPoint3* pkInNormals = pkTriData->GetNormals();
    NiPoint2* pkInTexCoords = pkTriData->GetTextureSet(0); 
    NiColorA* pkColors = pkTriData->GetColors();
    unsigned short* psInTriList =  pkTriData->GetTriList();

    unsigned short usaEdge1[2];
    unsigned short usaEdge2[2];
    unsigned short usaEdge3[2];

    unsigned short* uspEdgeList = (unsigned short*)
        NiMalloc(sizeof(unsigned short) * usIndexCount * 2);

    unsigned int uiEgdgeCount = 0;

    // Create a list of all the edges in the mesh
    for(unsigned int uiPoly = 0; uiPoly < uiPolyCount; uiPoly++)
    { 
        usaEdge1[0] = psInTriList[(uiPoly * 3)];
        usaEdge1[1] = psInTriList[(uiPoly * 3)+1];

        usaEdge2[0] = psInTriList[(uiPoly * 3)+1];
        usaEdge2[1] = psInTriList[(uiPoly * 3)+2];

        usaEdge3[0] = psInTriList[(uiPoly * 3)+2];
        usaEdge3[1] = psInTriList[(uiPoly * 3)];

        // See if edge1 is already in the list, if not add it
        if(!IsEdgeInList(usaEdge1, uspEdgeList, uiEgdgeCount))
        {
            uspEdgeList[(uiEgdgeCount<<1)] = usaEdge1[0];
            uspEdgeList[(uiEgdgeCount<<1)+1] = usaEdge1[1];
            uiEgdgeCount++;
        }

        // See if edge1 is already in the list, if not add it
        if(!IsEdgeInList(usaEdge2, uspEdgeList, uiEgdgeCount))
        {
            uspEdgeList[(uiEgdgeCount<<1)] = usaEdge2[0];
            uspEdgeList[(uiEgdgeCount<<1)+1] = usaEdge2[1];
            uiEgdgeCount++;
        }

        // See if edge1 is already in the list, if not add it
        if(!IsEdgeInList(usaEdge3, uspEdgeList, uiEgdgeCount))
        {
            uspEdgeList[(uiEgdgeCount<<1)] = usaEdge3[0];
            uspEdgeList[(uiEgdgeCount<<1)+1] = usaEdge3[1];
            uiEgdgeCount++;
        }
    }


    // Now that we have a list of all the edges in the TriShape we can cycle
    // through each one and generate a fin for the corresponding edge
    unsigned int uiOutVertCount = uiEgdgeCount*4;
    unsigned int uiOutIndexCount = uiEgdgeCount*6;

    NiPoint3* pkOutVerts = NiNew NiPoint3[uiOutVertCount];
    NiPoint3* pkOutNormals = NiNew NiPoint3[uiOutVertCount];
    NiPoint2* pkOutTexCoords = NiNew NiPoint2[uiOutVertCount*2];
    NiColorA* pkNewColors = NiNew NiColorA[uiOutVertCount];

    unsigned short* psOutTriList = 
        (unsigned short*)NiMalloc(sizeof(unsigned short) * uiOutIndexCount);


    NiPoint3 kaVerts[4];
    NiPoint3 kaNormals[4];
    NiPoint2 kaTexCoords[4];
    NiColorA kaColors[4];

    unsigned int uiTotalIndecies = 0;
    unsigned int uiTotalVerts = 0;


    FinSkinHelper* pkFinShinHelper = 0;
    NiSkinInstance* pkInSkinInstance = pkShape->GetSkinInstance();

    if (pkInSkinInstance)
    {
        pkFinShinHelper = (FinSkinHelper*)NiMalloc(sizeof(FinSkinHelper) * 
            usVertCount);
        for(unsigned int ui = 0; ui < usVertCount; ui++)
        {
            pkFinShinHelper[ui].m_fBoneWeight = NiNew NiTPrimitiveSet<float>();
            pkFinShinHelper[ui].m_kaEdgeVertIDs = 
                NiNew NiTPrimitiveSet<unsigned int>();
            pkFinShinHelper[ui].m_kaBoneID = 
                NiNew NiTPrimitiveSet<unsigned int>();
        }
    }


    // From the list of triangle edges create a quad for each edge extruding 
    // out in the direction of the normals on that edge
    for(unsigned int e = 0; e < uiEgdgeCount; e++)
    {
        kaVerts[0] = pkInVerts[ uspEdgeList[(e<<1)] ];
        kaVerts[1] = pkInVerts[ uspEdgeList[(e<<1)+1] ];
        kaVerts[2] = pkInVerts[ uspEdgeList[(e<<1)] ];
        kaVerts[3] = pkInVerts[ uspEdgeList[(e<<1)+1] ];

        kaNormals[0] = pkInNormals[ uspEdgeList[(e<<1)] ];
        kaNormals[1] = pkInNormals[ uspEdgeList[(e<<1)+1] ];

        kaNormals[2] = pkInNormals[ uspEdgeList[(e<<1)] ];
        kaNormals[3] = pkInNormals[ uspEdgeList[(e<<1)+1] ];

        if (pkColors)
        {
            kaColors[0] = pkColors[ uspEdgeList[(e<<1)] ];
            kaColors[1] = pkColors[ uspEdgeList[(e<<1)+1] ];
            kaColors[2] = pkColors[ uspEdgeList[(e<<1)] ];
            kaColors[3] = pkColors[ uspEdgeList[(e<<1)+1] ];
        }
        else
        {
            kaColors[0] = NiColorA(1,1,1,1);
            kaColors[1] = NiColorA(1,1,1,1);
            kaColors[2] = NiColorA(1,1,1,1);
            kaColors[3] = NiColorA(1,1,1,1);
        }


        // Get Dist between edge points. This is used to ensure that the fin
        // texture is tiled equal along each fin.
        NiPoint3 kEdgeVector = kaVerts[1] - kaVerts[0];
        float fDist = kEdgeVector.Length();

        // Setup Texture Coordinates
        kaTexCoords[0].x = 0.0f;
        kaTexCoords[0].y = 0.0f;
        kaTexCoords[1].x = fDist * (float)m_fTextureTilesPerFinUnit;
        kaTexCoords[1].y = 0.0f;

        kaTexCoords[2].x = 0.0f;
        kaTexCoords[2].y = 0.95f;
        kaTexCoords[3].x = fDist * (float)m_fTextureTilesPerFinUnit;
        kaTexCoords[3].y = 0.95f;


        // Extrude Verts 2 and 3 towards normal direction
        kaVerts[2].x += kaNormals[2].x * m_fFurLength;
        kaVerts[2].y += kaNormals[2].y * m_fFurLength;
        kaVerts[2].z += kaNormals[2].z * m_fFurLength;

        kaVerts[3].x += kaNormals[3].x * m_fFurLength;
        kaVerts[3].y += kaNormals[3].y * m_fFurLength;
        kaVerts[3].z += kaNormals[3].z * m_fFurLength;

        // Copy Data into output data
        for(int i = 0; i < 4; i++)
        {
            pkOutVerts[(e*4)+i].x = kaVerts[i].x;
            pkOutVerts[(e*4)+i].y = kaVerts[i].y;
            pkOutVerts[(e*4)+i].z = kaVerts[i].z;

            pkOutNormals[(e*4)+i].x = kaNormals[i].x;
            pkOutNormals[(e*4)+i].y = kaNormals[i].y;
            pkOutNormals[(e*4)+i].z = kaNormals[i].z;

            pkNewColors[(e*4)+i] = kaColors[i];

            // Determine which end of the fin the vert belongs too.
            if (i%2==0)
            {
                pkOutTexCoords[(e*4)+i].x = 
                    pkInTexCoords[uspEdgeList[(e<<1)]].x;
                pkOutTexCoords[(e*4)+i].y = 
                    pkInTexCoords[uspEdgeList[(e<<1)]].y;

                if (pkInSkinInstance)
                {
                    pkFinShinHelper[ uspEdgeList[(e<<1)] ].
                        m_kaEdgeVertIDs->Add((e*4)+i);
                }

            }
            else
            {
                pkOutTexCoords[(e*4)+i].x = 
                    pkInTexCoords[uspEdgeList[(e<<1)+1]].x;
                pkOutTexCoords[(e*4)+i].y = 
                    pkInTexCoords[uspEdgeList[(e<<1)+1]].y;

                if (pkInSkinInstance)
                {
                    pkFinShinHelper[ uspEdgeList[(e<<1)+1] ].
                        m_kaEdgeVertIDs->Add((e*4)+i);
                }

            }

            pkOutTexCoords[((e*4)+i)+uiOutVertCount].x = kaTexCoords[i].x;
            pkOutTexCoords[((e*4)+i)+uiOutVertCount].y = kaTexCoords[i].y;   
        }
            
        uiTotalVerts +=4;

        // Tri1 of fin
        psOutTriList[uiTotalIndecies] = (e*4);
        psOutTriList[uiTotalIndecies+1] = (e*4)+1;
        psOutTriList[uiTotalIndecies+2] = (e*4)+2;

        // Tri2 of fin
        psOutTriList[uiTotalIndecies+3] = (e*4)+1;
        psOutTriList[uiTotalIndecies+4] = (e*4)+3;
        psOutTriList[uiTotalIndecies+5] = (e*4)+2;  

        uiTotalIndecies += 6;
    }


    // Create New Fin TriShape
    NiTriShapeData* pkNewTriData = NiNew NiTriShapeData(uiTotalVerts, 
        pkOutVerts, pkOutNormals, pkNewColors, pkOutTexCoords, 2, 
        NiGeometryData::NBT_METHOD_NONE, uiTotalIndecies/3, psOutTriList);

    NiTriShape* pkNewTriShape = NiNew NiTriShape(pkNewTriData);
    pkNewTriShape->SetName(cpName);


    NiTexturingProperty* pkInTexProp =
        (NiTexturingProperty*)pkShape->GetProperty(NiProperty::TEXTURING);

    NiTexturingProperty* pkOutTexProp = NiNew NiTexturingProperty();

    pkOutTexProp->SetBaseTexture(pkInTexProp->GetBaseTexture());

    // Warning: The file name for the fin texture is hard coded. This will not
    // cause any issues during the export process if such a file does not
    // exist, but does require that the fin texture use the file name "fin.dds"
    NiSourceTexture* pkFinTexture = NiSourceTexture::Create("fin.dds");

    NiTexturingProperty::ShaderMap* pkShaderMap = 
        NiNew NiTexturingProperty::ShaderMap(pkFinTexture,0);
    pkOutTexProp->SetShaderMap(0,pkShaderMap);

    pkNewTriShape->AttachProperty(pkOutTexProp);

    if (pkInSkinInstance)
    {
        NiSkinInstance* pkNewSkinInstance = 
            CloneSkinningForFins(pkInSkinInstance, usVertCount, 
            pkFinShinHelper, pkOutVerts);

        pkNewTriShape->SetSkinInstance(pkNewSkinInstance);

        // if the geometry uses a different number of bones per partition than
        // the default, that value will need to be carried over
        NiIntegerExtraData* pkIntED = NiDynamicCast(NiIntegerExtraData, 
            pkShape->GetExtraData(NI_BONES_PER_PARTITION_KEY));
        if (pkIntED)
        {
            pkNewTriShape->AddExtraData((NiExtraData*)pkIntED->Clone());
        }

        NiSingleShaderMaterial* pkShaderMat = 
            NiSingleShaderMaterial::Create("FurFinsSkinning",false);
        pkNewTriShape->ApplyMaterial(pkShaderMat);
        pkNewTriShape->SetActiveMaterial(pkShaderMat);
    }
    else
    {
        NiSingleShaderMaterial* pkShaderMat = 
            NiSingleShaderMaterial::Create("FurFins",false);
        pkNewTriShape->ApplyMaterial(pkShaderMat);
        pkNewTriShape->SetActiveMaterial(pkShaderMat);
    }

    pkSortNode->AttachChild(pkNewTriShape);

    if (pkInSkinInstance)
    {
        for(unsigned int ui = 0; ui < usVertCount; ui++)
        {
            NiDelete pkFinShinHelper[ui].m_fBoneWeight;
            NiDelete pkFinShinHelper[ui].m_kaEdgeVertIDs;
            NiDelete pkFinShinHelper[ui].m_kaBoneID;
        }
        NiFree(pkFinShinHelper);
    }

    NiFree(uspEdgeList);
}
//---------------------------------------------------------------------------
NiSkinInstance* NiFurGeneratorPlugin::CloneSkinningForShells(
    NiSkinInstance* pkInSkinInstance, unsigned int uiShellCount, 
    unsigned int uiBaseVertCount, NiPoint3* pkShellVerts)
{
    NiSkinData* pkInSkinData = pkInSkinInstance->GetSkinData();
    NIASSERT(pkInSkinData);

    unsigned int uiBoneCount = pkInSkinData->GetBoneCount();
    NiAVObject*const* ppkInBones = pkInSkinInstance->GetBones();

    NiAVObject** ppkOutBones = (NiAVObject**)
        NiMalloc(sizeof(NiAVObject**) * uiBoneCount);
    
    // Copy Pointers to bones, but do not copy the actual data!
    NiMemcpy(ppkOutBones,ppkInBones,sizeof(NiAVObject**) * uiBoneCount);
    

    NiSkinData::BoneData* pkInBoneData = pkInSkinData->GetBoneData();
    NiSkinData::BoneData* pkOutBoneData = 
        NiNew NiSkinData::BoneData[uiBoneCount];


    // Create new bone data based on the bone data from the original TriShape
    for (unsigned int uiBoneID = 0; uiBoneID < uiBoneCount; uiBoneID++)
    {
        NiSkinData::BoneData* pkInBone = &pkInBoneData[uiBoneID];
        NiSkinData::BoneData* pkOutBone = &pkOutBoneData[uiBoneID];

        pkOutBone->m_usVerts =  pkInBone->m_usVerts * uiShellCount;
        pkOutBone->m_kSkinToBone =  pkInBone->m_kSkinToBone;
        pkOutBone->m_kBound =  pkInBone->m_kBound;

        pkOutBone->m_pkBoneVertData = 
            NiNew NiSkinData::BoneVertData[pkOutBone->m_usVerts * 
            uiShellCount];

        for( unsigned int uiShell = 0; uiShell < uiShellCount; uiShell++)
        {

            for(unsigned short usVertID = 0; usVertID < pkInBone->m_usVerts; 
                usVertID++)
            {
                unsigned short usOutVertID = 
                    (pkInBone->m_usVerts * uiShell) + usVertID ;

                pkOutBone->m_pkBoneVertData[usOutVertID].m_fWeight = 
                    pkInBone->m_pkBoneVertData[usVertID].m_fWeight;
                
                pkOutBone->m_pkBoneVertData[usOutVertID].m_usVert = 
                    pkInBone->m_pkBoneVertData[usVertID].m_usVert +
                    (uiBaseVertCount * uiShell);
            }

        }

    }

    NiSkinData* pkOutSkinData = NiNew NiSkinData(uiBoneCount, pkOutBoneData,
        pkInSkinData->GetRootParentToSkin(), pkShellVerts);

    return NiNew NiSkinInstance(pkOutSkinData, 
        pkInSkinInstance->GetRootParent(), ppkOutBones);
}
//---------------------------------------------------------------------------
NiSkinInstance* NiFurGeneratorPlugin::CloneSkinningForFins(
    NiSkinInstance* pkInSkinInstance, unsigned int uiBaseVertCount,  
    FinSkinHelper* pkFinShinHelper, NiPoint3* pkFinVerts)
{
    NiSkinData* pkInSkinData = pkInSkinInstance->GetSkinData();
    NIASSERT(pkInSkinData);

    unsigned int uiBoneCount = pkInSkinData->GetBoneCount();
    NiAVObject*const* ppkInBones = pkInSkinInstance->GetBones();

    NiAVObject** ppkOutBones = (NiAVObject**)
        NiMalloc(sizeof(NiAVObject**) * uiBoneCount);

    // Copy Pointers to bones, but do not copy the actual data!
    NiMemcpy(ppkOutBones,ppkInBones,sizeof(NiAVObject**) * uiBoneCount);


    NiSkinData::BoneData* pkInBoneData = pkInSkinData->GetBoneData();
    NiSkinData::BoneData* pkOutBoneData = 
        NiNew NiSkinData::BoneData[uiBoneCount];


    // Fill in the rest of FinSkinHelper with all the bone data
    for (unsigned int uiBoneID = 0; uiBoneID < uiBoneCount; uiBoneID++)
    {
        NiSkinData::BoneData* pkBone = &pkInBoneData[uiBoneID];
        pkOutBoneData[uiBoneID].m_usVerts = 0;

        pkOutBoneData[uiBoneID].m_kSkinToBone =  pkBone->m_kSkinToBone;
        pkOutBoneData[uiBoneID].m_kBound =  pkBone->m_kBound;

        for (unsigned short usVert = 0; usVert < pkBone->m_usVerts; usVert++)
        {
            unsigned short usVertID = 
                pkBone->m_pkBoneVertData[usVert].m_usVert;

            pkFinShinHelper[usVertID].m_kaBoneID->Add(uiBoneID);
            pkFinShinHelper[usVertID].m_fBoneWeight->
                Add(pkBone->m_pkBoneVertData[usVert].m_fWeight);
        }

    }

    // Scan to find the total number of verts used by each bone
    for( unsigned int uiVert = 0; uiVert < uiBaseVertCount; uiVert++)
    {
        unsigned int uiBoneCount = 
            pkFinShinHelper[uiVert].m_kaBoneID->GetSize();

        for (unsigned int uiBone = 0; uiBone < uiBoneCount; uiBone++)
        {
            unsigned int uiBoneID = 
                pkFinShinHelper[uiVert].m_kaBoneID->GetAt(uiBone);

            NiSkinData::BoneData* pkBone = &pkOutBoneData[uiBoneID];
            NIASSERT(pkBone);

            unsigned int uiFinVertCount = 
                pkFinShinHelper[uiVert].m_kaEdgeVertIDs->GetSize();

            pkBone->m_usVerts += uiFinVertCount;
        }
    }


    // Get memory for bones
    for (unsigned int uiBone = 0; uiBone < uiBoneCount; uiBone++)
    {
        NiSkinData::BoneData* pkBone = &pkOutBoneData[uiBone];
        NIASSERT(pkBone);

        pkBone->m_pkBoneVertData = 
            NiNew NiSkinData::BoneVertData[pkBone->m_usVerts];

        // Reset number of verts to zero so it can be used to index
        // the current vert in the next code block
        pkBone->m_usVerts = 0;
    }


    // Assign new bone vert data based on vert data from the orginal object
    for (unsigned int uiVert = 0; uiVert < uiBaseVertCount; uiVert++)
    {
        unsigned int uiFinVertCount = 
            pkFinShinHelper[uiVert].m_kaEdgeVertIDs->GetSize();

        for (unsigned int uiFinVert = 0; uiFinVert < uiFinVertCount; 
            uiFinVert++)
        {
            unsigned int uiFinVertID = 
                pkFinShinHelper[uiVert].m_kaEdgeVertIDs->GetAt(uiFinVert);

            unsigned int uiVertBoneCount = 
                pkFinShinHelper[uiVert].m_kaBoneID->GetSize();

            for (unsigned int uiBone = 0; uiBone < uiVertBoneCount; uiBone++)
            {
                unsigned int uiBoneID = 
                    pkFinShinHelper[uiVert].m_kaBoneID->GetAt(uiBone);
                NiSkinData::BoneData* pkBone = &pkOutBoneData[uiBoneID];

                pkBone->m_pkBoneVertData[pkBone->m_usVerts].m_usVert = 
                    uiFinVertID;


                pkBone->m_pkBoneVertData[pkBone->m_usVerts].m_fWeight = 
                    pkFinShinHelper[uiVert].m_fBoneWeight->GetAt(uiBone);


                pkBone->m_usVerts++;
            }

        }

    }

    NiSkinData* pkOutSkinData = NiNew NiSkinData(uiBoneCount, pkOutBoneData,
        pkInSkinData->GetRootParentToSkin(), pkFinVerts);

    return NiNew NiSkinInstance(pkOutSkinData, 
        pkInSkinInstance->GetRootParent(), ppkOutBones);
}
//---------------------------------------------------------------------------






