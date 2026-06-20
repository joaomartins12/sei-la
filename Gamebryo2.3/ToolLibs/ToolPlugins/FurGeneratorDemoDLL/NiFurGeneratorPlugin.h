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

#ifndef NIFURGENERATORPLUGIN_H
#define NIFURGENERATORPLUGIN_H

struct FinSkinHelper
{
    NiTPrimitiveSet<unsigned int>* m_kaEdgeVertIDs;
    NiTPrimitiveSet<unsigned int>* m_kaBoneID;
    NiTPrimitiveSet<float>* m_fBoneWeight;
};


class NiFurGeneratorPlugin : public NiPlugin
{
public:
    NiDeclareRTTI;

    /// Default constructor.
    NiFurGeneratorPlugin();

    /// Required virtual function overrides from NiPlugin.
    virtual bool CanExecute(NiPluginInfo* pkInfo);
    virtual NiExecutionResultPtr Execute(const NiPluginInfo* pkInfo);
    virtual NiPluginInfo* GetDefaultPluginInfo(); 
    virtual bool HasManagementDialog();
    virtual bool DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent);

    static bool PerObjectCallback(NiAVObject* pkObject);
    static void ErrorCallback(unsigned int uiCode, NiObject* pkObject,
        const char* pcMessage);

protected:

    NiTPointerList<NiTriShape*>* m_pkTriShapeList;

    void Init();
    void Destroy();
    void BuildTriShapeList(NiSceneGraphSharedData* pkSGSharedData);
    void RecusiveTriShapeSearch(NiNode* pkNode);

    void RemoveDetailMap(NiAVObject* pkNode);
    bool HasFurData(NiAVObject* pkNode);

    void GenerateFur();

    void GenerateShellTexture();
    void GenerateShellsFromShape(NiTriShape* pkShape, 
        NiSortAdjustNode* pkSortNode, char* cpName);
    NiSkinInstance* CloneSkinningForShells(NiSkinInstance* pkInSkinInstance, 
        unsigned int uiShellCount, unsigned int uiBaseVertCount,
        NiPoint3* pkShellVerts);



    bool IsEdgeInList(WORD* testEdge, WORD* edgeList, int numberOfEdges);
    void GenerateFinsFromShape(NiTriShape* pkShape, 
        NiSortAdjustNode* pkSortNode, char* cpName);
    NiSkinInstance* CloneSkinningForFins(NiSkinInstance* pkInSkinInstance, 
        unsigned int uiBaseVertCount, FinSkinHelper* pkFinShinHelper, 
        NiPoint3* pkFinVerts);

    // Fur generation parameters
    unsigned int m_uiNumberOfShells;
    float m_fFurLength;                 
    float m_fTextureTilesPerFinUnit;    // Amount to tile the fin texture
    float m_fShellTextureSize;
    unsigned int m_uiShellTextureCellSize;
    unsigned int m_uiShellLevelCount;

    NiProgressDialog* m_pkProgressBar;
    static NiFurGeneratorPlugin* ms_pkThis;

    NiSourceTexture* m_pkShellTexture;
};

#endif  // #ifndef NIFURGENERATORPLUGIN_H
