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

//
// MyiInitPlugin.cpp
// =================
//
//      Handles the (un)initialization of the Plugin.  
//

// Precompiled Headers
#include "MayaPluginPCH.h"
#include "GetShadowTechniquesCommand.h"
#include "GetDefaultShadowBias.h"


//  ========================================================================
//  The following string is used to define the version number to the Maya
//  plug-ins Manager.  Change this whenever the translator is modified.
//  The algorithm for the number is "Gamebryo + 5". Last changed 02/28/06.
//  Also change line 60 in NifExportPlugin::NifExportPlugin() in MyiPlugin.cpp.
char    *pcMyImmerseVersion = "7.3";
int     iInfinity;
int     iState;

//---------------------------------------------------------------------------
void fnCallBeforeExport( void* clientData )
{
    MString MayaScriptString;
    MCommandResult MayaCommandResult;
    MStatus MayaStatus;

    // Save user preferences
    MayaScriptString = "undoInfo -q -infinity";
    MayaStatus = MGlobal::executeCommand(MayaScriptString, MayaCommandResult, 
        false, false);
    MayaCommandResult.getResult(iInfinity);
    
    MayaScriptString = "undoInfo -q -state";
    MayaStatus = MGlobal::executeCommand(MayaScriptString, MayaCommandResult, 
        false, false);
    MayaCommandResult.getResult(iState);

    // Turn the undo queue ON and to INFINITE
    // For some reason - turning undo on doesn't actually work
    // even though turning infinity on does and it returns a OK message...?
    MString MayaScriptString2;
    MayaScriptString2 = "undoInfo -state 1 -infinity 1";
    MayaStatus = MGlobal::executeCommand(MayaScriptString2,false,false);

//MayaScriptString = "undoInfo -infinity on";
//MayaStatus = MGlobal::executeCommand(MayaScriptString, MayaCommandResult,
    //  false, false);

    if (MayaStatus == MS::kSuccess)
    {
        MStatus pstat;
        MString Mstring;

        if (MayaCommandResult.resultType(&pstat) == MCommandResult::kInt)
        {
        }
    }
}
//---------------------------------------------------------------------------
MStatus initializePlugin (MObject obj)
{

    MStatus status;
    char acVersion[512];

    NiSprintf( acVersion, 512, 
        "Gamebryo Version:%s\n Exporter Plugin Version:%s\n", 
        GAMEBRYO_SDK_VERSION_STRING, pcMyImmerseVersion);

    MFnPlugin plugin (obj, "NIF Translator for Maya", acVersion, "Any");

    //Register the translator with the system
    status = plugin.registerFileTranslator ("Nif","MyImmerseIcon1.bmp",
                            NifExportPlugin::creator,"MyImmerseGUI",
                            NULL,false /*true*/);

    if (!status)
    {
        status.perror ("registerFileTranslator");
        return status;
    }

    //Register the translator with the system
    status = plugin.registerCommand("GetShadowTechniques", 
        GetShadowTechniques::creator);

    if (!status)
    {
        status.perror ("registerCommand");
        return status;
    }

    //Register the translator with the system
    status = plugin.registerCommand("GetDefaultShadowBias", 
        GetDefaultShadowBias::creator);

    if (!status)
    {
        status.perror ("registerCommand");
        return status;
    }

    return status;
}
//---------------------------------------------------------------------------
MStatus uninitializePlugin (MObject obj)
{
    MStatus         status;
    MFnPlugin       plugin (obj);

    status = plugin.deregisterFileTranslator ("Nif");
    if (!status)
        status.perror ("deregisterFileTranslator");

    status = plugin.deregisterCommand("GetShadowTechniques");
    if (!status)
        status.perror ("deregisterGetShadowTechniques");

    status = plugin.deregisterCommand("GetDefaultShadowBias");
    if (!status)
        status.perror ("deregisterGetDefaultShadowBias");
    
    return status;
}
//---------------------------------------------------------------------------
