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

#ifndef ASSETANALYZERPLUGIN_H
#define ASSETANALYZERPLUGIN_H

#include "NiTPointerList.h"

/// This is our Asset Analyzer process plug-in. It is used to examine art 
/// during export and report when unexpected things appear. These conditions
/// are not strictly errors, they are expected results from artists.
class AssetAnalyzerPlugin : public NiPlugin
{
public:
    /// RTTI declaration macro.
    NiDeclareRTTI;

    /// Default constructor.
    AssetAnalyzerPlugin();

    //-----------------------------------------------------------------------
    // Required virtual function overrides from NiPlugin.
    //-----------------------------------------------------------------------
    /// Returns a plug-in script with default parameters.
    virtual NiPluginInfo* GetDefaultPluginInfo(); 

    /// Does this plug-in have editable options?
    virtual bool HasManagementDialog();
    
    /// Bring up the options dialog and handle user interaction.
    virtual bool DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent);

    /// Determines whether or not this plug-in can handle this plug-in info
    /// object.
    virtual bool CanExecute(NiPluginInfo* pkInfo);
    
    /// Execute the NiPluginInfo script.
    virtual NiExecutionResultPtr Execute(const NiPluginInfo* pkInfo);

protected:

    bool MaxTriangleCount(int iMaxTriangles);
    int RecursiveMaxTriangleCount(NiAVObject* pkObject);
    int GetTriangleCount(NiGeometry* pkGeometry);

    bool MaxTextureSize(NiAVObject* pkObject, int iMaxTextureSize);
    bool CheckTextureSize(NiTexturingProperty::Map* pkMap,
        int iSize, const char* pcMapName, const char* pcObjectName );

    bool MaxSceneMemorySize(int iSceneMemorySize);
    int RecursiveMaxSceneMemorySize(NiAVObject* pkObject);
    int ComputeGeometryDataSize(NiAVObject* pkObjecct);
    int ComputeAnimationDataSize(NiAVObject* pkObject);
    int ComputeAnimationDataSize(NiTimeController* pkCtlr);

    bool MaxTotalTextureSize(int iTotalTextureSize);
    void RecursiveMaxTotalTextureSize(NiAVObject* pkObject, 
        NiTPointerList<NiTexture*>& kTextureList);
    void InsertUniqueTexture(NiTPointerList<NiTexture*>& kTextureList,
        NiTexturingProperty::Map* pkMap);
    void InsertUniqueTexture(NiTPointerList<NiTexture*>& kTextureList,
        NiTexture* pkTexture);
    int ComputeTotalTextureSize(NiTPointerList<NiTexture*>& kTextureList);

    bool MaxObjectCount(int iMaxObjectCount);
    int RecursiveMaxObjectCount(NiAVObject* pkObject);

    bool MinTriangleTrishapeRatio(float fRatio);
    void RecursiveMinTriangleTrishapeRatio(NiAVObject* pkObject, 
        int& iTriangles, int& iTrishapes);
   
    bool MinTriangleToStripRatio(float fRatio);
    void RecursiveMinTriangleToStripRatio(NiAVObject* pkObject,
        int& iTriangles, int& iStrips);
   
    bool MultiSubObjectWarnings(NiAVObject* pkObject, int iMaxSubObjects);
    bool CheckForMultiSubObject(NiAVObject* pkObject, int iMaxSubObjects);

    bool MaxMorphingVertexPerObject(NiAVObject* pkObject, int iMaxVertexes);

    bool MaxTriangleCountPerObject(NiAVObject* pkObject, int iMaxTriangles);

    bool MaxSceneGraphDepth(NiAVObject* pkObject, int iMaxDepth);

    bool MaxLightsPerObject(NiAVObject* pkObject, int iMaxLights);

    bool RequiredObjects(NiAVObject* pkObject,
        NiTPointerList<NiString*>* pkObjectList);
    void BuildRequireObjectList(NiTPointerList<NiString*>* pkObjectList,
        const NiPluginInfo* pkInfo);


    bool NoSpotLights(NiAVObject* pkObject);

    bool NoPointLights(NiAVObject* pkObject);

    bool NoDirectionalLights(NiAVObject* pkObject);

    bool NoAmbientLights(NiAVObject* pkObject);

    void ClearAllErrors();
    void ClearAllSectionErrors();
    void AppendAllSectionErrors();
    void ShowAllErrors();

    NiSceneGraphSharedData* m_pkSGSharedData;
    NiTPointerList<NiString*> m_pkErrors;
    NiTPointerList<NiString*> m_pkSectionErrors;

};

#endif  // #ifndef ASSETANALYZERPLUGIN_H
