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
#include "AssetAnalyzerPlugin.h"
#include "AssetAnalyzerDialog.h"
#include "AssetAnalyzerResultsDialog.h"
#include "AssetAnalyzerPluginDefines.h"
#include "NiAnimation.h"
#include "NiGeomMorpherController.h"
// RTTI implementation macro.
NiImplementRTTI(AssetAnalyzerPlugin, NiPlugin);

//---------------------------------------------------------------------------
AssetAnalyzerPlugin::AssetAnalyzerPlugin() : NiPlugin(
    "Asset Analyzer Process Plug-in",                // name
    "1.0",                                          // version
    "An Asset Analyzer process plug-in",              // short description
    "An Asset Analyzer process plug-in that examines the scene to "
    "determine if it meets requirements for your game.")  // long description
{

}
//---------------------------------------------------------------------------
NiPluginInfo* AssetAnalyzerPlugin::GetDefaultPluginInfo()
{
    // Create the default parameters for this plug-in.
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("AssetAnalyzerPlugin");
    pkPluginInfo->SetType("PROCESS");
    

    pkPluginInfo->SetBool(ASSET_ANALYZER_USE_MAX_TRIANGLE_COUNT, true);
    pkPluginInfo->SetInt(ASSET_ANALYZER_MAX_TRIANGLE_COUNT, 300000);

    pkPluginInfo->SetBool(ASSET_ANALYZER_USE_MAX_TEXTURE_SIZE, true);
    pkPluginInfo->SetInt(ASSET_ANALYZER_MAX_TEXTURE_SIZE, 512);

    pkPluginInfo->SetBool(ASSET_ANALYZER_USE_MAX_SCENE_MEMORY_SIZE, true);
    pkPluginInfo->SetInt(ASSET_ANALYZER_MAX_SCENE_MEMORY_SIZE, 65536000);

    pkPluginInfo->SetBool(ASSET_ANALYZER_USE_MAX_TOTAL_TEXTURE_SIZE, true);
    pkPluginInfo->SetInt(ASSET_ANALYZER_MAX_TOTAL_TEXTURE_SIZE, 32768000);

    pkPluginInfo->SetBool(ASSET_ANALYZER_USE_MAX_OBJECT_COUNT, true);
    pkPluginInfo->SetInt(ASSET_ANALYZER_MAX_OBJECT_COUNT, 416);

    pkPluginInfo->SetBool(
        ASSET_ANALYZER_USE_MINIMUM_TRIANGLE_TO_TRISHAPE_RATIO, true);
    pkPluginInfo->SetFloat(
        ASSET_ANALYZER_MINIMUM_TRIANGLE_TO_TRISHAPE_RATIO, 20.0f);

    pkPluginInfo->SetBool(
        ASSET_ANALYZER_USE_MIMIMUM_TRIANGLE_TO_STRIP_RATIO, true);
    pkPluginInfo->SetFloat(
        ASSET_ANALYZER_MINIMUM_TRIANGLE_TO_STRIP_RATIO, 20.0f);

    pkPluginInfo->SetBool(
        ASSET_ANALYZER_USE_MULTI_SUB_OBJECT_WARNINGS, true);
    pkPluginInfo->SetInt(
        ASSET_ANALYZER_MULTI_SUB_OBJECT_WARNINGS, 4);

    pkPluginInfo->SetBool(
        ASSET_ANALYZER_USE_MAXIMUM_MORPHING_VERTEX_PER_OBJECT, true);
    pkPluginInfo->SetInt(
        ASSET_ANALYZER_MAXIMUM_MORPHING_VERTEX_PER_OBJECT, 200);

    pkPluginInfo->SetBool(
        ASSET_ANALYZER_USE_MAXIMUM_TRIANGLE_COUNT_PER_OBJECT, true);
    pkPluginInfo->SetInt(
        ASSET_ANALYZER_MAXIMUM_TRIANGLE_COUNT_PER_OBJECT, 16000);

    pkPluginInfo->SetBool(ASSET_ANALYZER_USE_MAXIMUM_SCENE_GRAPH_DEPTH, true);
    pkPluginInfo->SetInt(ASSET_ANALYZER_MAXIMUM_SCENE_GRAPH_DEPTH, 20);

    pkPluginInfo->SetBool(ASSET_ANALYZER_USE_MAXIMUM_LIGHTS_PER_OBJECT, true);
    pkPluginInfo->SetInt(ASSET_ANALYZER_MAXIMUM_LIGHTS_PER_OBJECT, 2);

    pkPluginInfo->SetBool(ASSET_ANALYZER_USE_REQUIRED_OBJECTS, false);
    pkPluginInfo->SetInt(ASSET_ANALYZER_NUMBER_OF_REQUIRED_OBJECTS, 0);

    pkPluginInfo->SetBool(ASSET_ANALYZER_NO_SPOT_LIGHTS, true);
    pkPluginInfo->SetBool(ASSET_ANALYZER_NO_POINT_LIGHTS, true);
    pkPluginInfo->SetBool(ASSET_ANALYZER_NO_DIRECTIONAL_LIGHTS, false);
    pkPluginInfo->SetBool(ASSET_ANALYZER_NO_AMBIENT_LIGHTS, false);

    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::HasManagementDialog()
{
    // Since we have parameters that can be changed, we provide a management
    // dialog, otherwise we would return false.
    return true;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
    NiWindowRef hWndParent)
{
    // Handle the user interface for manipulating the execution of this
    // NiPlugin.
    // - If the user clicks 'OK', make the input pkInitialInfo match the new
    //   set of parameters and return true.
    // - If the user clicks 'Cancel', then leave the input pkInitialInfo as
    //   it is and return false.

    AssetAnalyzerDialog kDialog(m_hModule, hWndParent, pkInitialInfo);

    switch (kDialog.DoModal())
    {
        case IDOK:
            // Copy over the results.
            pkInitialInfo->RemoveAllParameters();
            kDialog.GetResults()->Clone(pkInitialInfo);
            break;
        case IDCANCEL:
            return false;
            break;
        default:
            NIASSERT(false);
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    // Verify that the NiPluginInfo object can actually be executed by
    // this plug-in. If we were to support previous versions, this is where
    // we would agree to handle the NiPluginInfo. Conversion would come in the
    // actual Execute call.

    // Other useful values to check would be:
    // - the name of the plug-in,
    // - the application name if your plug-in is application-dependent 
    //   (which you can get from NiFramework)
    // - if any required parameters exist in this NiPluginInfo

    if (pkInfo->GetClassName() == "AssetAnalyzerPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr AssetAnalyzerPlugin::Execute(
    const NiPluginInfo* pkInfo)
{

    // Save the old image converter
    NiImageConverterPtr spOldConverter =
        NiImageConverter::GetImageConverter();

    // Set the converter so we can load textures
    NiImageConverter::SetImageConverter(NiNew NiDevImageConverter);


    // Coming into this method, CanExecute has already been called.
    // Therefore, we know that we can actually execute pkInfo.

    // We'll likely want to use the scene graph in this process plug-in,
    // so we'll go ahead and get the scene graph shared data.
    m_pkSGSharedData = (NiSceneGraphSharedData*) 
        GetSharedData(NiGetSharedDataType(NiSceneGraphSharedData));

    // If we do not find scene graph shared data, we cannot complete the
    // processing of the scene graph. Return failure.
    if (!m_pkSGSharedData)
    {
        // If we want to, we can pop up an NiMessageBox giving a more
        // useful warning if not running in silent mode.
        NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
        NIASSERT(pkDataList);
        NiExporterOptionsSharedData* pkExporterSharedData = 
            (NiExporterOptionsSharedData*) 
            pkDataList->Get(NiGetSharedDataType(NiExporterOptionsSharedData));
        bool bWriteResultsToLog;
        NiXMLLogger* pkLogger;

        if (pkExporterSharedData)
        {
            bWriteResultsToLog = pkExporterSharedData->GetWriteResultsToLog();
            pkLogger = pkExporterSharedData->GetXMLLogger();
        }

        if ((bWriteResultsToLog) && (pkLogger))
        {
            pkLogger->LogElement("AssetAnalyzerError", "The Asset Analyzer "
                "Process Plug-in could not find the NiSceneGraphSharedData!");
        }
        else
        {
            NiMessageBox("The Asset Analyzer Process Plug-in could not find "
                "the NiSceneGraphSharedData!","Scene Graph Shared Data "
                "Missing");
        }

        // Reset the converter
        NiImageConverter::SetImageConverter(spOldConverter);

        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }


    // While 3ds max and Maya by default only generate one root, other 
    // plug-ins in the pipeline could potentially add new roots.
    for (unsigned int ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);

        spRoot->UpdateProperties();
        spRoot->UpdateEffects();
    }


    // Get the actual parameters for the NiPluginInfo parameter map.
    // These functions will assert in debug mode if the key doesn't exist or
    // if the value is not convertable to the correct primitive type.
    bool bFoundError = false;

    if (pkInfo->GetBool(ASSET_ANALYZER_USE_MAX_TRIANGLE_COUNT))
    {
        bFoundError |= MaxTriangleCount(
            pkInfo->GetInt(ASSET_ANALYZER_MAX_TRIANGLE_COUNT));
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_USE_MAX_TEXTURE_SIZE))
    {
        bFoundError |= MaxTextureSize(NULL,
            pkInfo->GetInt(ASSET_ANALYZER_MAX_TEXTURE_SIZE));
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_USE_MAX_SCENE_MEMORY_SIZE))
    {
        bFoundError |= MaxSceneMemorySize(
            pkInfo->GetInt(ASSET_ANALYZER_MAX_SCENE_MEMORY_SIZE));
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_USE_MAX_TOTAL_TEXTURE_SIZE))
    {
        bFoundError |= MaxTotalTextureSize(
            pkInfo->GetInt(ASSET_ANALYZER_MAX_TOTAL_TEXTURE_SIZE));
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_USE_MAX_OBJECT_COUNT))
    {
        bFoundError |= MaxObjectCount(
            pkInfo->GetInt(ASSET_ANALYZER_MAX_OBJECT_COUNT));
    }

    if (pkInfo->GetBool(
        ASSET_ANALYZER_USE_MINIMUM_TRIANGLE_TO_TRISHAPE_RATIO))
    {
        bFoundError |= MinTriangleTrishapeRatio(
            pkInfo->GetFloat(
                ASSET_ANALYZER_MINIMUM_TRIANGLE_TO_TRISHAPE_RATIO));
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_USE_MIMIMUM_TRIANGLE_TO_STRIP_RATIO))
    {
        bFoundError |= MinTriangleToStripRatio(
            pkInfo->GetFloat(ASSET_ANALYZER_MINIMUM_TRIANGLE_TO_STRIP_RATIO));
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_USE_MULTI_SUB_OBJECT_WARNINGS))
    {
        bFoundError |= MultiSubObjectWarnings(NULL,
            pkInfo->GetInt(ASSET_ANALYZER_MULTI_SUB_OBJECT_WARNINGS));
    }

    if (pkInfo->GetBool(
        ASSET_ANALYZER_USE_MAXIMUM_MORPHING_VERTEX_PER_OBJECT))
    {
        bFoundError |= MaxMorphingVertexPerObject(NULL,
            pkInfo->GetInt(
            ASSET_ANALYZER_MAXIMUM_MORPHING_VERTEX_PER_OBJECT));
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_USE_MAXIMUM_TRIANGLE_COUNT_PER_OBJECT))
    {
        bFoundError |= MaxTriangleCountPerObject(NULL,
            pkInfo->GetInt(ASSET_ANALYZER_MAXIMUM_TRIANGLE_COUNT_PER_OBJECT));
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_USE_MAXIMUM_SCENE_GRAPH_DEPTH))
    {
        bFoundError |= MaxSceneGraphDepth(NULL,
            pkInfo->GetInt(ASSET_ANALYZER_MAXIMUM_SCENE_GRAPH_DEPTH));
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_USE_MAXIMUM_LIGHTS_PER_OBJECT))
    {
        bFoundError |= MaxLightsPerObject(NULL,
            pkInfo->GetInt(ASSET_ANALYZER_MAXIMUM_LIGHTS_PER_OBJECT));
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_USE_REQUIRED_OBJECTS))
    {
        // Build the Require Object List
        NiTPointerList<NiString*>* pkObjectList =
            NiNew NiTPointerList<NiString*>;
        BuildRequireObjectList(pkObjectList, pkInfo);

        // Search for any Missing Object
        bFoundError |= RequiredObjects(NULL, pkObjectList);

        // Clean up
        while(!pkObjectList->IsEmpty())
        {
            NiDelete pkObjectList->RemoveHead();
        }
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_NO_SPOT_LIGHTS))
    {
        bFoundError |= NoSpotLights(NULL);
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_NO_POINT_LIGHTS))
    {
        bFoundError |= NoPointLights(NULL);
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_NO_DIRECTIONAL_LIGHTS))
    {
        bFoundError |= NoDirectionalLights(NULL);
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_NO_AMBIENT_LIGHTS))
    {
        bFoundError |= NoAmbientLights(NULL);
    }


    // Show any errors which have occured.
    if (bFoundError)
        ShowAllErrors();

    // Reset the converter
    NiImageConverter::SetImageConverter(spOldConverter);

    // Presumably, we have correctly executed our actions on the scene
    // graph. We return success in this case.
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MaxTriangleCount( int iMaxTriangles)
{
    int iTotal = 0;

    // While 3ds max and Maya by default only generate one root, other 
    // plug-ins in the pipeline could potentially add new roots.
    for (unsigned int ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
        
        iTotal += RecursiveMaxTriangleCount(spRoot);
    }


    if (iTotal > iMaxTriangles)
    {
        NiString* pkString = NiNew NiString("MAX TRIANGLE COUNT");
        m_pkErrors.AddTail(pkString);

        char acBuffer[256];
        NiSprintf(acBuffer, 256, "\tFound %d Triangles     Limit %d", iTotal,
            iMaxTriangles);

        pkString = NiNew NiString(acBuffer);
        m_pkErrors.AddTail(pkString);
        m_pkErrors.AddTail(NiNew NiString(""));
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
int AssetAnalyzerPlugin::RecursiveMaxTriangleCount( NiAVObject* pkObject)
{
    int iTotal = 0;

    // Total up the number of Triangles
    if (NiIsKindOf(NiGeometry, pkObject))
    {
        iTotal = GetTriangleCount((NiGeometry*)pkObject);
    }

    // Loop over all children 

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount();
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                iTotal += RecursiveMaxTriangleCount(pkChild);
            }
        }
    }

    return iTotal;
}
//---------------------------------------------------------------------------
int AssetAnalyzerPlugin::GetTriangleCount(NiGeometry* pkGeometry)
{

    if (NiIsKindOf(NiTriBasedGeom, pkGeometry))
    {
        NiTriBasedGeom* pkTriGeom = (NiTriBasedGeom*)pkGeometry;
        int iTriangleCount = pkTriGeom->GetTriangleCount();
        
        // Look for a standard Trishape
        if (NiIsKindOf(NiTriShape, pkTriGeom))
        {
            return iTriangleCount;
        }
        
        int iNonDegenerateTriangles = 0;
        unsigned short usVert0;
        unsigned short usVert1;
        unsigned short usVert2;
        
        // Look for Degenerate Triangles
        for(unsigned short usLoop = 0; (int)usLoop < iTriangleCount; usLoop++)
        {
            pkTriGeom->GetTriangleIndices(usLoop, usVert0, usVert1, usVert2);
            
            // Only count non-degenerate triangles
            if ( (usVert0 != usVert1) && (usVert0 != usVert2) && 
                (usVert1 != usVert2))
            {
                iNonDegenerateTriangles++;
            }
        }
        
        return iNonDegenerateTriangles;
    }

    return 0;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MaxTextureSize( NiAVObject* pkObject, 
    int iMaxTextureSize)
{
    bool bFound = false;


    // Start the Recursion.
    if (pkObject == NULL)
    {
        // While 3ds max and Maya by default only generate one root, other 
        // plug-ins in the pipeline could potentially add new roots.
        for (unsigned int ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
        {
            NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
            
            bFound |= MaxTextureSize(spRoot, iMaxTextureSize);
        }

        if (bFound)
        {
            m_pkErrors.AddTail( NiNew NiString("MAX TEXTURE SIZE") );
            AppendAllSectionErrors();
            m_pkErrors.AddTail(NiNew NiString(""));

        }

        return bFound;
    }

    // Check to see if the object has a texturing property
    NiTexturingProperty* pkTextureProp = 
        (NiTexturingProperty*)pkObject->GetProperty(NiProperty::TEXTURING);
    if (pkTextureProp != NULL)
    {

        bFound |= CheckTextureSize(pkTextureProp->GetBaseMap(), 
            iMaxTextureSize, "BaseMap", pkObject->GetName());

        bFound |= CheckTextureSize(pkTextureProp->GetDarkMap(), 
            iMaxTextureSize, "DarkMap", pkObject->GetName());

        bFound |= CheckTextureSize(pkTextureProp->GetDetailMap(),
            iMaxTextureSize, "DetailMap", pkObject->GetName());

        bFound |= CheckTextureSize(pkTextureProp->GetGlossMap(),
            iMaxTextureSize, "GlossMap", pkObject->GetName());

        bFound |= CheckTextureSize(pkTextureProp->GetGlowMap(),
            iMaxTextureSize, "GlowMap", pkObject->GetName());

        bFound |= CheckTextureSize(pkTextureProp->GetBumpMap(),
            iMaxTextureSize, "BumpMap", pkObject->GetName());

        unsigned int uiLoop = 0;
        for(; uiLoop < pkTextureProp->GetDecalMapCount(); uiLoop++)
        {
            bFound |= CheckTextureSize(pkTextureProp->GetDecalMap(uiLoop),
                iMaxTextureSize, "DecalMap", pkObject->GetName());
        }

        for(uiLoop = 0; uiLoop < pkTextureProp->GetDecalMapCount(); uiLoop++)
        {
            bFound |= CheckTextureSize(pkTextureProp->GetShaderMap(uiLoop),
                iMaxTextureSize, "ShaderMap", pkObject->GetName());
        }
    }



   // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount();
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                bFound |= MaxTextureSize(pkChild, iMaxTextureSize);
            }
        }
    }


    return bFound;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::CheckTextureSize(NiTexturingProperty::Map* pkMap,
       int iSize, const char* pcMapName, const char* pcObjectName)
{
    // Check the Map
    if (pkMap != NULL)
    {
        NiTexture* pkTexture = pkMap->GetTexture();

        if (pkTexture == NULL)
            return false;

        if ((pkTexture->GetWidth() > (unsigned int)iSize) ||
             (pkTexture->GetHeight() > (unsigned int)iSize) )
        {
            char acBuffer[512];


            NiSprintf(acBuffer, 512, "\tObject Name:: %s", pcObjectName);
            NiString* pkString = NiNew NiString(acBuffer);
            m_pkSectionErrors.AddTail(pkString);

            NiSprintf(acBuffer, 512, "\tMap Name:: %s", pcMapName);
            pkString = NiNew NiString(acBuffer);
            m_pkSectionErrors.AddTail(pkString);

            NiSprintf(acBuffer, 512, "\tSize:: %d/%d  Limit %d/%d", 
                pkTexture->GetWidth(), pkTexture->GetHeight(), iSize, iSize);
            pkString = NiNew NiString(acBuffer);
            m_pkSectionErrors.AddTail(pkString);

            m_pkSectionErrors.AddTail(NiNew NiString(""));
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MaxSceneMemorySize(int iMaxSceneMemorySize)
{
    int iTotal = 0;
    NiTPointerList<NiTexture*> kTextureList;


    // While 3ds max and Maya by default only generate one root, other 
    // plug-ins in the pipeline could potentially add new roots.
    for (unsigned int ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);

        // Collect All of the Textures
        RecursiveMaxTotalTextureSize(spRoot, kTextureList);

        iTotal += RecursiveMaxSceneMemorySize(spRoot);
    }

    // Add in the total size of textures
    iTotal += ComputeTotalTextureSize(kTextureList);



    if (iTotal > iMaxSceneMemorySize)
    {
        NiString* pkString = NiNew NiString("MAX SCENE MEMORY SIZE");
        m_pkErrors.AddTail(pkString);

        char acBuffer[256];
        NiSprintf(acBuffer, 256, "\tMemory Size %d bytes    Limit %d bytes",
            iTotal, iMaxSceneMemorySize);

        pkString = NiNew NiString(acBuffer);
        m_pkErrors.AddTail(pkString);
        m_pkErrors.AddTail(NiNew NiString(""));
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
int AssetAnalyzerPlugin::RecursiveMaxSceneMemorySize( NiAVObject* pkObject)
{
    int iTotal = 0;

    // Determine the Geometry size
    iTotal += ComputeGeometryDataSize(pkObject);

    // Determine the size of any animations
    iTotal += ComputeAnimationDataSize(pkObject);


    // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount();
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                iTotal += RecursiveMaxSceneMemorySize( pkChild);
            }
        }
    }

    return iTotal;
}
//---------------------------------------------------------------------------
int AssetAnalyzerPlugin::ComputeGeometryDataSize(NiAVObject* pkObject)
{
    int iTotal = 0;

    // Determine the size of the Geometry
    if (NiIsKindOf(NiGeometry, pkObject))
    {
        NiGeometry* pkGeometry = (NiGeometry*)pkObject;

        // Total up the vertexes
        iTotal += pkGeometry->GetVertexCount() * sizeof(NiPoint3);

        // Total up the Normals
        if (pkGeometry->GetNormals() != NULL)
        {
            iTotal += pkGeometry->GetVertexCount() * sizeof(NiPoint3);
        }

        // Total up the Colors
        if (pkGeometry->GetColors() != NULL)
        {
            iTotal += pkGeometry->GetVertexCount() * sizeof(NiColorA);
        }

        // Total up the Texture Sets
        iTotal += pkGeometry->GetTextureSets() * pkGeometry->GetVertexCount()
            * sizeof(NiPoint2);

        // Check for a TriShape
        if (NiIsKindOf(NiTriShape, pkObject))
        {
            iTotal += ((NiTriShape*)pkObject)->GetTriListLength() * 
                sizeof(unsigned short);
        }

        // Check for TriStrips
        if (NiIsKindOf(NiTriStrips, pkObject))
        {
            NiTriStripsData* pkData =
                (NiTriStripsData*)pkGeometry->GetModelData();

            iTotal += pkData->GetStripLengthSum() * sizeof(unsigned short);
        }
    }

    return iTotal;
}
//---------------------------------------------------------------------------
int AssetAnalyzerPlugin::ComputeAnimationDataSize(NiAVObject* pkObject)
{
    int iTotal = 0;

    if (!pkObject)
        return 0;

    NiTimeController* pkCtlr = pkObject->GetControllers();
    while (pkCtlr)
    {
        iTotal += ComputeAnimationDataSize(pkCtlr);
        pkCtlr = pkCtlr->GetNext();
    }

    NiPropertyList& kPropList = pkObject->GetPropertyList();
    NiTListIterator kIter = kPropList.GetHeadPos();
    while (kIter)
    {
        NiProperty* pkProp = kPropList.GetNext(kIter);
        if (pkProp)
        {
            NiTimeController* pkCtlr = pkProp->GetControllers();
            while (pkCtlr)
            {
                iTotal += ComputeAnimationDataSize(pkCtlr);
                pkCtlr = pkCtlr->GetNext();
            }
        }
    }
    
    return iTotal;
}

//---------------------------------------------------------------------------
int AssetAnalyzerPlugin::ComputeAnimationDataSize(NiTimeController* pkCtlr)
{
    if (NiIsKindOf(NiInterpController, pkCtlr))
    {
        NiInterpController* pkInterpCtlr = (NiInterpController*) pkCtlr;
        for (unsigned int us = 0; us <  pkInterpCtlr->
            GetInterpolatorCount(); us++)
        {
            NiInterpolator* pkInterp = pkInterpCtlr->GetInterpolator(us);
            if (NiIsKindOf(NiKeyBasedInterpolator, pkInterp))
            {
                NiKeyBasedInterpolator* pkKeyInterp =
                    (NiKeyBasedInterpolator*) pkInterp;
                for (unsigned short usc = 0; usc < pkKeyInterp->
                    GetKeyChannelCount(); usc++)
                {
                    return pkKeyInterp->GetAllocatedSize(usc);
                }
            }
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MaxTotalTextureSize(int iMaxTotalTextureSize)
{
    NiTPointerList<NiTexture*> kTextureList;


    // While 3ds max and Maya by default only generate one root, other 
    // plug-ins in the pipeline could potentially add new roots.
    for (unsigned int ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
        
        RecursiveMaxTotalTextureSize(spRoot, kTextureList);
    }

    int iTotal = ComputeTotalTextureSize(kTextureList);

    if (iTotal > iMaxTotalTextureSize)
    {
        NiString* pkString = NiNew NiString("MAX SCENE TEXTURE SIZE");
        m_pkErrors.AddTail(pkString);

        char acBuffer[256];
        NiSprintf(acBuffer, 256, "\tScene Texture Size %d bytes    Limit %d "
            "bytes", iTotal, iMaxTotalTextureSize);

        pkString = NiNew NiString(acBuffer);
        m_pkErrors.AddTail(pkString);
        m_pkErrors.AddTail(NiNew NiString(""));
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void AssetAnalyzerPlugin::RecursiveMaxTotalTextureSize(NiAVObject* pkObject,
    NiTPointerList<NiTexture*>& kTextureList)
{

    // Check to see if the object has a texturing property
    NiTexturingProperty* pkTextureProp =
        (NiTexturingProperty*)pkObject->GetProperty(NiProperty::TEXTURING);
    if (pkTextureProp != NULL)
    {

        InsertUniqueTexture(kTextureList, pkTextureProp->GetBaseMap());

        InsertUniqueTexture(kTextureList, pkTextureProp->GetDarkMap());

        InsertUniqueTexture(kTextureList, pkTextureProp->GetDetailMap());

        InsertUniqueTexture(kTextureList, pkTextureProp->GetGlossMap());

        InsertUniqueTexture(kTextureList, pkTextureProp->GetGlowMap());

        InsertUniqueTexture(kTextureList, pkTextureProp->GetBumpMap());

        unsigned int uiLoop = 0;
        for(; uiLoop < pkTextureProp->GetDecalMapCount(); uiLoop++)
        {
            InsertUniqueTexture(kTextureList,
                pkTextureProp->GetDecalMap(uiLoop));
        }

        for(uiLoop = 0; uiLoop < pkTextureProp->GetShaderArrayCount();
            uiLoop++)
        {
            if (pkTextureProp->GetShaderMap(uiLoop) != NULL) 
            {
                InsertUniqueTexture(kTextureList,
                    pkTextureProp->GetShaderMap(uiLoop));
            }
        }


        // Check for Flip Controllers
        NiTimeController* pkController = pkTextureProp->GetControllers();

        while (pkController != NULL)
        {
            NiFlipController* pkFlip = NiDynamicCast(NiFlipController,
                pkController);
            if (pkFlip != NULL)
            {
                for (unsigned int uiLoop = 0;
                    uiLoop < pkFlip->GetTextureArraySize(); uiLoop++)
                {
                    InsertUniqueTexture(kTextureList,
                        pkFlip->GetTextureAt(uiLoop));
                }
            }

            pkController = pkController->GetNext();
        }

    }
    
    // Check for A Texture Effect
    if (NiIsKindOf(NiTextureEffect, pkObject))
    {
        InsertUniqueTexture(kTextureList,
            ((NiTextureEffect*)pkObject)->GetEffectTexture());
    }


    // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount();
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                RecursiveMaxTotalTextureSize( pkChild, kTextureList);
            }
        }
    }
}
//---------------------------------------------------------------------------
void AssetAnalyzerPlugin::InsertUniqueTexture(
    NiTPointerList<NiTexture*>& kTextureList, NiTexture* pkTexture)
{
    if (pkTexture != NULL)
    {
        
        // Scan the list to make sure the texture isn't already there
        NiTListIterator kIter = kTextureList.GetHeadPos();
        while (kIter != NULL)
        {
            // Quit if we found it
            if (kTextureList.GetNext(kIter) == pkTexture)
            {
                return;
            }
        }
        
        // Add to the list
        kTextureList.AddTail(pkTexture);
    }
}
//---------------------------------------------------------------------------
void AssetAnalyzerPlugin::InsertUniqueTexture(
    NiTPointerList<NiTexture*>& kTextureList, NiTexturingProperty::Map* pkMap)
{
    if (pkMap != NULL)
    {
        NiTexture* pkTexture = pkMap->GetTexture();
        if (pkTexture != NULL)
        {
            InsertUniqueTexture(kTextureList, pkTexture);
        }
    }
}
//---------------------------------------------------------------------------
int AssetAnalyzerPlugin::ComputeTotalTextureSize(
    NiTPointerList<NiTexture*>& kTextureList)
{
    int iTotal = 0;

    NiTListIterator kIter = kTextureList.GetHeadPos();

    // Loop though all of the textures
    while (kIter != NULL)
    {
        NiTexture* kTexture = kTextureList.GetNext(kIter);

        // Total up the size of source textures
        if (NiIsKindOf(NiSourceTexture, kTexture))
        {
            NiSourceTexture* pkSourceTexture = (NiSourceTexture*)kTexture;
            NiPixelData* pkData = pkSourceTexture->GetSourcePixelData();

            // Check for Pixel Data
            if (pkData != NULL)
            {
                iTotal += pkData->GetTotalSizeInBytes();
            }
            else
            {
                // Attempt to Load the Pixel Data
                pkSourceTexture->LoadPixelDataFromFile();

                pkData = pkSourceTexture->GetSourcePixelData();

                // Check for Pixel Data
                if (pkData != NULL)
                {
                    iTotal += pkData->GetTotalSizeInBytes();
                    pkSourceTexture->DestroyAppPixelData();
                }
                else
                {
                    NIASSERT(false);
                }
            }
        }
    }

    return iTotal;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MaxObjectCount(int iMaxObjectCount)
{
    int iTotal = 0;

    // While 3ds max and Maya by default only generate one root, other 
    // plug-ins in the pipeline could potentially add new roots.
    for (unsigned int ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
        
        iTotal += RecursiveMaxObjectCount(spRoot);
    }



    if (iTotal > iMaxObjectCount)
    {
        NiString* pkString = NiNew NiString("MAX OBJECT COUNT");
        m_pkErrors.AddTail(pkString);

        char acBuffer[256];
        NiSprintf(acBuffer, 256, "\tObject Count %d     Limit %d", iTotal,
            iMaxObjectCount);

        pkString = NiNew NiString(acBuffer);
        m_pkErrors.AddTail(pkString);
        m_pkErrors.AddTail(NiNew NiString(""));
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
int AssetAnalyzerPlugin::RecursiveMaxObjectCount( NiAVObject* pkObject)
{
    int iTotal = 0;

    // Check for a trishape
    if (NiIsKindOf(NiTriShape, pkObject))
    {
        iTotal++;
    }

    // Count each strip of a tristrip
    if (NiIsKindOf(NiTriStrips, pkObject))
    {
        NiTriStripsData* pkData = NiDynamicCast(NiTriStripsData, 
            ((NiTriStrips*)pkObject)->GetModelData());

        if (pkData != NULL)
        {
            iTotal += pkData->GetStripCount();
        }

    }

    // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount();
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                iTotal += RecursiveMaxObjectCount(pkChild);
            }
        }
    }

    return iTotal;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MinTriangleTrishapeRatio(float fRatio)
{
    int iTriangles = 0;
    int iTrishapes = 0;

    // While 3ds max and Maya by default only generate one root, other 
    // plug-ins in the pipeline could potentially add new roots.
    for (unsigned int ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
        
        RecursiveMinTriangleTrishapeRatio(spRoot, iTriangles, iTrishapes);
    }


    float fTriangleToTrishape = (float)iTriangles / (float)iTrishapes;

    if (fTriangleToTrishape < fRatio)
    {
        NiString* pkString = 
            NiNew NiString("MINIMUM TRIANGLE TO TRISHAPE RATIO");
        m_pkErrors.AddTail(pkString);

        char acBuffer[256];
        NiSprintf(acBuffer, 256, "\tRatio Found %f     Limit %f", 
            fTriangleToTrishape, fRatio);

        pkString = NiNew NiString(acBuffer);
        m_pkErrors.AddTail(pkString);
        m_pkErrors.AddTail(NiNew NiString(""));
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void AssetAnalyzerPlugin::RecursiveMinTriangleTrishapeRatio(
    NiAVObject* pkObject, int& iTriangles, int& iTrishapes)
{

    // Determine the number of objects drawing
    if (NiIsKindOf(NiTriShape, pkObject))
    {
        iTrishapes++;
    }

    // Count each strip of a tristrip
    if (NiIsKindOf(NiTriStrips, pkObject))
    {
        NiTriStripsData* pkData = NiDynamicCast(NiTriStripsData, 
            ((NiTriStrips*)pkObject)->GetModelData());

        if (pkData != NULL)
        {
            iTrishapes += pkData->GetStripCount();
        }

    }

    // Add in the number of triangles
    if (NiIsKindOf(NiGeometry, pkObject))
    {
        iTriangles += GetTriangleCount((NiGeometry*)pkObject);
    }


    // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount(); 
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                RecursiveMinTriangleTrishapeRatio(pkChild, iTriangles,
                    iTrishapes);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MinTriangleToStripRatio(float fRatio)
{
    int iTriangles = 0;
    int iStrips = 0;

    // While 3ds max and Maya by default only generate one root, other 
    // plug-ins in the pipeline could potentially add new roots.
    for (unsigned int ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
        
        RecursiveMinTriangleToStripRatio(spRoot, iTriangles, iStrips);
    }


    float fVertexToStrip = (float)iTriangles / (float)iStrips;

    if (fVertexToStrip < fRatio)
    {
        NiString* pkString = NiNew NiString(
            "MINIMUM TRIANGLE TO STRIP RATIO");
        m_pkErrors.AddTail(pkString);

        char acBuffer[256];
        NiSprintf(acBuffer, 256, "\tRatio Found %f     Limit %f", 
            fVertexToStrip, fRatio);

        pkString = NiNew NiString(acBuffer);
        m_pkErrors.AddTail(pkString);
        m_pkErrors.AddTail(NiNew NiString(""));
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void AssetAnalyzerPlugin::RecursiveMinTriangleToStripRatio(
    NiAVObject* pkObject, int& iTriangles, int& iStrips)
{
    // Count each strip of a tristrip
    if (NiIsKindOf(NiTriStrips, pkObject))
    {
        NiTriStripsData* pkData = NiDynamicCast(NiTriStripsData, 
            ((NiTriStrips*)pkObject)->GetModelData());

        if (pkData != NULL)
        {
            iStrips += pkData->GetStripCount();
        }

    }

    // Add in the number of triangles
    if (NiIsKindOf(NiGeometry, pkObject))
    {
        iTriangles += GetTriangleCount((NiGeometry*)pkObject);
    }



    // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount();
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                RecursiveMinTriangleToStripRatio(pkChild, iTriangles,
                    iStrips);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MultiSubObjectWarnings( NiAVObject* pkObject, 
    int iMaxSubObjects)
{
    bool bFound = false;


    // Start the Recursion.
    if (pkObject == NULL)
    {
        // While 3ds max and Maya by default only generate one root, other 
        // plug-ins in the pipeline could potentially add new roots.
        for (unsigned int ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
        {
            NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
            
            bFound |= MultiSubObjectWarnings(spRoot, iMaxSubObjects);
        }

        if (bFound)
        {
            m_pkErrors.AddTail(NiNew NiString("EXCESSIVE MULTI SUB OBJECTS"));
            AppendAllSectionErrors();
            m_pkErrors.AddTail(NiNew NiString(""));
        }

        return bFound;
    }


    // Check this object for Multi-Sub Objects
    bFound = CheckForMultiSubObject(pkObject, iMaxSubObjects);

    // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount(); 
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                bFound |= MultiSubObjectWarnings(pkChild, iMaxSubObjects);
            }
        }
    }

    return bFound;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::CheckForMultiSubObject(NiAVObject* pkObject, 
    int iMaxSubObjects)
{
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode == NULL)
        return false;

    int iSubObjectChildren = 0;


    // This is the Maya way of doing Multi Sub Objects
    char acBaseName[512] = "";

    // Count the Children with Name:Number
    for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
    {
        NiAVObject* pkChild = pkNode->GetAt(uiLoop); 

        if ((pkChild != NULL) &&
            NiIsKindOf(NiGeometry, pkChild) &&
            (pkChild->GetName().Exists()) &&
            (pkNode->GetName().Exists())) 
        {

            // Find the possible base name
            if (!strcmp(acBaseName, ""))
            {
                // Clear off the : and end of the string
                if (strstr(pkChild->GetName(), ":") != NULL)
                {
                    NiStrcpy(acBaseName, 512, pkChild->GetName());
                    
                    char* pcTemp = strstr(acBaseName, ":");
                    *pcTemp = '\0';
                }
            }

                // Node Name is at the front
            if (strstr(pkChild->GetName(), acBaseName) == (NiString) 
                pkChild->GetName())
            {
                const char* pcTemp = ((const char*)pkChild->GetName())
                    + strlen(acBaseName);

                // Check for the :
                if (*pcTemp++ == ':')
                {

                    // Make sure the rest of the string is a number
                    NiString kNumberString(pcTemp);
                    int iInt;

                    if (kNumberString.ToInt(iInt))
                    {
                        iSubObjectChildren++;
                    }
                }
            }
        }
    }




    if (iSubObjectChildren > iMaxSubObjects)
    {
        char acBuffer[512];
    
        NiSprintf(acBuffer, 512, "\tObject:: %s    Has %d SubObjects    "
            "Limit %d", pkNode->GetName(), iSubObjectChildren,
            iMaxSubObjects);
        NiString* pkString = NiNew NiString(acBuffer);
        m_pkSectionErrors.AddTail(pkString);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MaxMorphingVertexPerObject( NiAVObject* pkObject, 
    int iMaxVertexes)
{
    bool bFound = false;


    // Start the Recursion.
    if (pkObject == NULL)
    {
        // While 3ds max and Maya by default only generate one root, other 
        // plug-ins in the pipeline could potentially add new roots.
        for (unsigned int ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
        {
            NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
            
            bFound |= MaxMorphingVertexPerObject(spRoot, iMaxVertexes);
        }

        if (bFound)
        {
            m_pkErrors.AddTail(NiNew NiString(
                "MAX MORPHING VERTEX PER OBJECT"));
            AppendAllSectionErrors();
            m_pkErrors.AddTail(NiNew NiString(""));
        }

        return bFound;
    }


    // Check for a Morpher Controller
    NiGeomMorpherController* pkMorph =
        (NiGeomMorpherController*)pkObject->
        GetController(&NiGeomMorpherController::ms_RTTI);

    if (pkMorph != NULL)
    {
        if (NiIsKindOf(NiGeometry, pkObject))
        {
            NiGeometryData* pkData = ((NiGeometry*)pkObject)->GetModelData();

            if ((pkData != NULL) &&
                (pkData->GetVertexCount() > iMaxVertexes))
            {
                char acBuffer[512];
                
                NiSprintf(acBuffer, 512, "\tMorphing Object:: %s    Has %d "
                    "Vertices    Limit %d", pkObject->GetName(), 
                    pkData->GetVertexCount(), iMaxVertexes );
                NiString* pkString = NiNew NiString(acBuffer);
                m_pkSectionErrors.AddTail(pkString);
                bFound = true;
            }
        }
    }



   // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount(); 
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                bFound |= MaxMorphingVertexPerObject(pkChild, iMaxVertexes);
            }
        }
    }

    return bFound;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MaxTriangleCountPerObject( NiAVObject* pkObject, 
    int iMaxTriangles)
{
    bool bFound = false;


    // Start the Recursion.
    if (pkObject == NULL)
    {
        // While 3ds max and Maya by default only generate one root, other 
        // plug-ins in the pipeline could potentially add new roots.
        for (unsigned int ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
        {
            NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
            
            bFound |= MaxTriangleCountPerObject(spRoot, iMaxTriangles);
        }

        if (bFound)
        {
            m_pkErrors.AddTail(NiNew NiString(
                "MAX TRIANGLE COUNT PER OBJECT"));
            AppendAllSectionErrors();
            m_pkErrors.AddTail(NiNew NiString(""));
        }

        return bFound;
    }


    // Add in the number of triangles
    if (NiIsKindOf(NiGeometry, pkObject)) 
    {
        int iTriangles = GetTriangleCount((NiGeometry*)pkObject);
        if (iTriangles > iMaxTriangles)
        {
            char acBuffer[512];
            
            NiSprintf(acBuffer, 512, "\tObject:: %s    Has %d Triangles    "
                "Limit %d", pkObject->GetName(), iTriangles, iMaxTriangles);
            NiString* pkString = NiNew NiString(acBuffer);
            m_pkSectionErrors.AddTail(pkString);
            bFound = true;
        }
    }



   // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount();
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                bFound |= MaxTriangleCountPerObject(pkChild, iMaxTriangles);
            }
        }
    }

    return bFound;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MaxSceneGraphDepth( NiAVObject* pkObject, 
    int iMaxDepth)
{
    // Return if we have gotten down far enough
    if (iMaxDepth == 0)
        return true;


    // Start the Recursion.
    if (pkObject == NULL)
    {
        bool bFound = false;


        // While 3ds max and Maya by default only generate one root, other 
        // plug-ins in the pipeline could potentially add new roots.
        for (unsigned int ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
        {
            NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
            
            bFound |= MaxSceneGraphDepth(spRoot, iMaxDepth);
        }

        if (bFound)
        {
            m_pkErrors.AddTail(NiNew NiString("MAX SCENE GRAPH DEPTH")); 

            char acBuffer[256];
            NiSprintf(acBuffer, 256, "\tExceeds Depth Limit %d", iMaxDepth);
            m_pkErrors.AddTail(NiNew NiString(acBuffer));
            m_pkErrors.AddTail(NiNew NiString(""));
        }

        return bFound;
    }


   // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount(); 
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if ((pkChild != NULL) &&
                MaxSceneGraphDepth(pkChild, iMaxDepth - 1))
            {
                return true;
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MaxLightsPerObject( NiAVObject* pkObject, 
    int iMaxLights)
{
    bool bFound = false;


    // Start the Recursion.
    if (pkObject == NULL)
    {
        // While 3ds max and Maya by default only generate one root, other 
        // plug-ins in the pipeline could potentially add new roots.
        for (unsigned int ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
        {
            NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
            
            bFound |= MaxLightsPerObject(spRoot, iMaxLights);
        }

        if (bFound)
        {
            m_pkErrors.AddTail( NiNew NiString("MAX LIGHTS PER OBJECT") );
            AppendAllSectionErrors();
            m_pkErrors.AddTail(NiNew NiString(""));
         }

        return bFound;
    }


    NiGeometry* pkGeometry = NiDynamicCast(NiGeometry, pkObject);
    if (pkGeometry != NULL)
    {
        // Count the attached lights
        int iLights = 0;
        NiDynamicEffectState* pkState = pkGeometry->GetEffectState();

        if (pkState != NULL)
        {
            for(NiDynEffectStateIter kCurrLight = pkState->GetLightHeadPos();
            kCurrLight != NULL; pkState->GetNextLight(kCurrLight))
            {
                iLights++;
            }
        }

        if (iLights >iMaxLights)
        {
            char acBuffer[256];
            NiSprintf(acBuffer, 256, "\tObject %s \tHas %d Lights \tLimit %d",
                pkObject->GetName(), iLights, iMaxLights);
            m_pkSectionErrors.AddTail(NiNew NiString(acBuffer));
            m_pkSectionErrors.AddTail(NiNew NiString(""));
            bFound = true;
        }
    }

   // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount(); 
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                bFound |= MaxLightsPerObject(pkChild, iMaxLights);
            }
        }
    }

    return bFound;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::RequiredObjects( NiAVObject* pkObject, 
    NiTPointerList<NiString*>* pkObjectList)
{


    // Start the Recursion.
    if (pkObject == NULL)
    {
        // While 3ds max and Maya by default only generate one root, other 
        // plug-ins in the pipeline could potentially add new roots.
        for (unsigned int ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
        {
            NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
            
            RequiredObjects(spRoot, pkObjectList);
        }

        if (pkObjectList->IsEmpty())
            return false;

        m_pkErrors.AddTail( NiNew NiString("MISSING REQUIRED OBJECTS") );

        // Loop through all of the object not found
        NiTListIterator kIter = pkObjectList->GetHeadPos();
        while(kIter != NULL)
        {
            NiString* kString = NiNew NiString("\t");
            *kString += *pkObjectList->GetNext(kIter);
            m_pkErrors.AddTail(kString);
        }

        m_pkErrors.AddTail(NiNew NiString(""));
        

        return true;
    }



    // Look to see if it is in the list
    if (pkObject->GetName().Exists())
    {
        NiTListIterator kIter = pkObjectList->GetHeadPos();
        while(kIter != NULL)
        {
            // check for a matching name
            if ((*pkObjectList->Get(kIter)) == pkObject->GetName())
            {
                // Remove from the list and quit looking
                pkObjectList->RemovePos(kIter);
                kIter = NULL;
            }
            else
            {
                // Incroment the iterator
                kIter = pkObjectList->GetNextPos(kIter);
            }
        }
    }



   // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount(); 
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                RequiredObjects(pkChild, pkObjectList);

                // Quit because we are out of stuff to find
                if (pkObjectList->IsEmpty())
                    return false;
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void AssetAnalyzerPlugin::BuildRequireObjectList(
    NiTPointerList<NiString*>* pkObjectList, const NiPluginInfo* pkInfo)
{

    int iNumRequiredObjects = pkInfo->GetInt(
        ASSET_ANALYZER_NUMBER_OF_REQUIRED_OBJECTS);
    
    for(int iLoop = 0; iLoop < iNumRequiredObjects; iLoop++)
    {
        // Create the Key
        NiString kKey(ASSET_ANALYZER_REQUIRED_OBJECTS);
        kKey += NiString::FromInt(iLoop);

        // Read the Value
        NiString* pkValue = NiNew NiString(pkInfo->GetValue(kKey));

        pkObjectList->AddTail(pkValue);
    }

}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::NoSpotLights(NiAVObject* pkObject)
{
    bool bFound = false;


    // Start the Recursion.
    if (pkObject == NULL)
    {
        // While 3ds max and Maya by default only generate one root, other 
        // plug-ins in the pipeline could potentially add new roots.
        for (unsigned int ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
        {
            NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
            
            bFound |= NoSpotLights(spRoot);
        }

        if (bFound)
        {
            m_pkErrors.AddTail( NiNew NiString("NO SPOT LIGHTS") );
            AppendAllSectionErrors();
            m_pkErrors.AddTail(NiNew NiString(""));
         }

        return bFound;
    }


    // Check for a light
    if (NiIsExactKindOf(NiSpotLight, pkObject)) 
    {
        char acBuffer[512];
        
        if (pkObject->GetName().Exists())
        {
            NiSprintf(acBuffer, 512, "\tNiSpotLight:: %s", 
                pkObject->GetName());
        }
        else
        {
            NiSprintf(acBuffer, 512, "\tNiSpotLight:: ** NO NAME **", 
                pkObject->GetName());
        }

        NiString* pkString = NiNew NiString(acBuffer);
        m_pkSectionErrors.AddTail(pkString);
        bFound = true;
    }


   // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount(); 
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                bFound |= NoSpotLights(pkChild);
            }
        }
    }

    return bFound;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::NoPointLights(NiAVObject* pkObject)
{
    bool bFound = false;


    // Start the Recursion.
    if (pkObject == NULL)
    {
        // While 3ds max and Maya by default only generate one root, other 
        // plug-ins in the pipeline could potentially add new roots.
        for (unsigned int ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
        {
            NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
            
            bFound |= NoPointLights(spRoot);
        }

        if (bFound)
        {
            m_pkErrors.AddTail( NiNew NiString("NO POINT LIGHTS") );
            AppendAllSectionErrors();
            m_pkErrors.AddTail(NiNew NiString(""));
         }

        return bFound;
    }

    // Check for a light
    if (NiIsExactKindOf(NiPointLight, pkObject)) 
    {
        char acBuffer[512];
        
        if (pkObject->GetName().Exists())
        {
            NiSprintf(acBuffer, 512, "\tNiPointLight:: %s", 
                pkObject->GetName());
        }
        else
        {
            NiSprintf(acBuffer, 512, "\tNiPointLight:: ** NO NAME **", 
                pkObject->GetName());
        }

        NiString* pkString = NiNew NiString(acBuffer);
        m_pkSectionErrors.AddTail(pkString);
        bFound = true;
    }


   // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount(); 
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                bFound |= NoPointLights(pkChild);
            }
        }
    }

    return bFound;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::NoDirectionalLights(NiAVObject* pkObject)
{
    bool bFound = false;


    // Start the Recursion.
    if (pkObject == NULL)
    {
        // While 3ds max and Maya by default only generate one root, other 
        // plug-ins in the pipeline could potentially add new roots.
        for (unsigned int ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
        {
            NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
            
            bFound |= NoDirectionalLights(spRoot);
        }

        if (bFound)
        {
            m_pkErrors.AddTail( NiNew NiString("NO DIRECTIONAL LIGHTS") );
            AppendAllSectionErrors();
            m_pkErrors.AddTail(NiNew NiString(""));
         }

        return bFound;
    }

    // Check for a light
    if (NiIsExactKindOf(NiDirectionalLight, pkObject)) 
    {
        char acBuffer[512];
        
        if (pkObject->GetName().Exists())
        {
            NiSprintf(acBuffer, 512, "\tNiDirectionalLight:: %s", 
                pkObject->GetName());
        }
        else
        {
            NiSprintf(acBuffer, 512, "\tNiDirectionalLight:: ** NO NAME **",
                pkObject->GetName());
        }

        NiString* pkString = NiNew NiString(acBuffer);
        m_pkSectionErrors.AddTail(pkString);
        bFound = true;
    }


   // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount(); 
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                bFound |= NoDirectionalLights(pkChild);
            }
        }
    }

    return bFound;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::NoAmbientLights(NiAVObject* pkObject)
{
    bool bFound = false;


    // Start the Recursion.
    if (pkObject == NULL)
    {
        // While 3ds max and Maya by default only generate one root, other 
        // plug-ins in the pipeline could potentially add new roots.
        for (unsigned int ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
        {
            NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
            
            bFound |= NoAmbientLights(spRoot);
        }

        if (bFound)
        {
            m_pkErrors.AddTail( NiNew NiString("NO AMBIENT LIGHTS") );
            AppendAllSectionErrors();
            m_pkErrors.AddTail(NiNew NiString(""));
         }

        return bFound;
    }

    // Check for a light
    if (NiIsExactKindOf(NiAmbientLight, pkObject)) 
    {
        char acBuffer[512];
        
        if (pkObject->GetName().Exists())
        {
            NiSprintf(acBuffer, 512, "\tNiAmbientLight:: %s", 
                pkObject->GetName());
        }
        else
        {
            NiSprintf(acBuffer, 512, "\tNiAmbientLight:: ** NO NAME **");
        }

        NiString* pkString = NiNew NiString(acBuffer);
        m_pkSectionErrors.AddTail(pkString);
        bFound = true;
    }


   // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount(); 
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                bFound |= NoAmbientLights(pkChild);
            }
        }
    }

    return bFound;
}
//---------------------------------------------------------------------------
void AssetAnalyzerPlugin::ClearAllErrors()
{
    // remove all errors deleting the strings
    while (!m_pkErrors.IsEmpty())
    {
        NiDelete m_pkErrors.RemoveHead();
    }
}
//---------------------------------------------------------------------------
void AssetAnalyzerPlugin::ClearAllSectionErrors()
{
    // remove all errors deleting the strings
    while (!m_pkSectionErrors.IsEmpty())
    {
        NiDelete m_pkSectionErrors.RemoveHead();
    }
}
//---------------------------------------------------------------------------
void AssetAnalyzerPlugin::AppendAllSectionErrors()
{
    // remove all errors deleting the strings
    while (!m_pkSectionErrors.IsEmpty())
    {
        m_pkErrors.AddTail( m_pkSectionErrors.RemoveHead() );;
    }
}
//---------------------------------------------------------------------------
void AssetAnalyzerPlugin::ShowAllErrors()
{
    if (m_pkErrors.IsEmpty())
        return;
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    NiExporterOptionsSharedData* pkExporterSharedData = 
        (NiExporterOptionsSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiExporterOptionsSharedData));
    bool bWriteResultsToLog;
    NiXMLLogger* pkLogger;

    if (pkExporterSharedData)
    {
        bWriteResultsToLog = pkExporterSharedData->GetWriteResultsToLog();
        pkLogger = pkExporterSharedData->GetXMLLogger();
    }

    if ((bWriteResultsToLog) && (pkLogger))
    {
        // Convert to a single string
        NiString kErrorText;
        NiTListIterator kIter = m_pkErrors.GetHeadPos();

        while (kIter != NULL)
        {
            // Append into the Text String
            kErrorText += *m_pkErrors.GetNext(kIter);
            kErrorText += "\r\n";
        }

        pkLogger->LogElement("AssetAnalyzerResults", kErrorText);
    }
    else
    {
        AssetAnalyzerResultsDialog kDialog(m_hModule, NULL, &m_pkErrors );

        kDialog.DoModal();
    }

    ClearAllErrors();
    ClearAllSectionErrors();
}
//---------------------------------------------------------------------------
