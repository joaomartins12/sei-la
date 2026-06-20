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

#ifndef FLTIMPORTPLUGIN_H
#define FLTIMPORTPLUGIN_H

// Include NiPluginToolkit headers.
#include <NiPluginToolkit.h>

#define USE_INTERNAL_TEXTURES "UseInternalTextures"
#define ADD_DEFAULT_LIGHTS "AddDefaultLights"
#define USE_TRUE_DOUBLESIDED "UseTrueDoubleSided"
#define FORCE_MERGE_SIBLING_SWITCHNODES "ForceMergeSiblingSwitchNodes"
#define FORCE_MERGE_SIBLING_LODNODES "ForceMergeSiblingLODNodes"

class FLTImportPlugin : public NiImportPlugin
{
public:
    NiDeclareRTTI;

    /// Default constructor.
    FLTImportPlugin();

    /// Required virtual function overrides from NiPlugin.
    virtual NiPluginInfo* GetDefaultPluginInfo(); 
    virtual bool HasManagementDialog();
    virtual bool DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent);

    // Required virtual function overrides from NiImportPlugin.
    virtual bool CanImport(NiImportPluginInfo* pkInfo);
    virtual NiExecutionResultPtr Import(const NiImportPluginInfo* pkInfo);
    virtual const char* GetFileDescriptorString();
    virtual const char* GetFileTypeString();
};

#endif 
      