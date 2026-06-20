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

/*
 * Standard DLL routines.
 */
#include "MaxImmerse.h"
#include "MiResource.h"
#include "MiUtils.h"
#include "MiViewerPlugin.h"
#include "MiExportPlugin.h"
#include "NiMaxShader.h"
#include <NiStringTokenizer.h>
#include "NiMAXPlatformMaterial.h"
#include <NiTAbstractPoolAllocator.h>


HINSTANCE hInstance;
static int giControlsInit = FALSE;

// forward-declared functions
__declspec(dllexport) int LibInitialize(void);
__declspec(dllexport) int LibShutdown(void);

//---------------------------------------------------------------------------
BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved) 
{
    hInstance = hinstDLL;

    if (!giControlsInit) 
    {
        giControlsInit = TRUE;
        
        InitCommonControls();
    }

    switch(fdwReason)
    {
    case DLL_PROCESS_ATTACH:
#if MAX_RELEASE <= 8000
        LibInitialize();
#endif
        break;
    case DLL_PROCESS_DETACH:
#if MAX_RELEASE <= 8000
        LibShutdown();
#endif
        break;
    };

    return (TRUE);
}

//---------------------------------------------------------------------------
__declspec( dllexport ) const TCHAR* LibDescription() 
{ 
    return GetString(IDS_NI_LIB_DESC); 
}

//---------------------------------------------------------------------------
__declspec( dllexport ) int LibNumberClasses() 
{
    return 4;
}

//---------------------------------------------------------------------------
__declspec( dllexport ) ClassDesc* LibClassDesc(int i) 
{
    switch(i) 
    {
        case 0: return GetMAXViewerClassDesc();
        case 1: return GetMAXExportClassDesc();
        case 2: return GetGamebryoShaderDesc();
        case 3: return GetNiMAXPlatformMaterialDesc();
        default: return 0;
    }

}

//---------------------------------------------------------------------------
__declspec( dllexport ) ULONG LibVersion() 
{ 
    return VERSION_3DSMAX; 
}
//---------------------------------------------------------------------------
__declspec(dllexport) int LibInitialize(void)
{
    char pcFileString[1024];
#if MAX_RELEASE <= 8000
    NiSprintf(pcFileString, 1024, "%s%s", 
        GetCOREInterface()->GetDir(APP_MAXROOT_DIR),
        "plugins\\MaxImmerse\\");
#else // #if MAX_RELEASE <= 8000
    NiSprintf(pcFileString, 1024, "%s%s", 
        GetCOREInterface()->GetDir(APP_MAX_SYS_ROOT_DIR),
        "plugins\\MaxImmerse\\");
#endif // #if MAX_RELEASE <= 8000
    NiFramework::InitFramework(pcFileString, "Gamebryo Max Plugin", 
        MAXIMMERSEVERSION);
    char acScriptDir[1024];

    // Get plug-in directory.
#if defined(_MSC_VER) && _MSC_VER >= 1400
    char acTemp[NI_MAX_PATH];
    size_t stBufferLength = 0;
    if (getenv_s(&stBufferLength, NULL, 0, 
        "EGB_SHADER_LIBRARY_PATH") == 0 && stBufferLength > 0)
    {
        getenv_s(&stBufferLength, acTemp, NI_MAX_PATH, 
            "EGB_SHADER_LIBRARY_PATH");
    }
    else
    {
        acTemp[0] = '\0';
    }
    NiString strShaderPath(acTemp);
    if (getenv_s(&stBufferLength, NULL, 0, 
        "EGB_TOOL_PLUGIN_PATH") == 0 && stBufferLength > 0)
    {
        getenv_s(&stBufferLength, acTemp, NI_MAX_PATH, 
            "EGB_TOOL_PLUGIN_PATH");
    }
    else
    {
        acTemp[0] = '\0';
    }
    NiString strDllPath(acTemp);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    NiString strDllPath = getenv("EGB_TOOL_PLUGIN_PATH");
    NiString strShaderPath = getenv("EGB_SHADER_LIBRARY_PATH");
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

    if (strDllPath.IsEmpty())
    {
        MessageBox(NULL, 
            "The environment variable EGB_TOOL_PLUGIN_PATH "
            "must\nbe defined for this application to properly"
            " execute.",
            "Missing Environment Variable", MB_OK | MB_ICONERROR);
    }

    if (strShaderPath.IsEmpty())
    {
        MessageBox(NULL, 
            "The environment variable EGB_SHADER_LIBRARY_PATH "
            "must\nbe defined for this application to properly"
            " execute.",
            "Missing Environment Variable", MB_OK | MB_ICONERROR);
    }

    NiStringTokenizer strTok(strDllPath);
    while (!strTok.IsEmpty())
    {
        NiString strPath = strTok.GetNextToken(";");
        int iPtr = strPath.FindReverse('\\');
        NIASSERT(iPtr >= 0);
        if ((unsigned int) iPtr < strPath.Length() - 1)
        {
            strPath += '\\';
        }

#if _MSC_VER == 1310 //VC7.1
        strPath += "Lib\\VC71\\";
#elif _MSC_VER == 1400 //VC8.0
        strPath += "Lib\\VC80\\";
#else
#error Unsupported version of Visual Studio
#endif
        NiPluginManager::GetInstance()->AddPluginDirectory(strPath);
    }
    
    NiMaterialToolkit* pkMaterialKit =
        NiMaterialToolkit::CreateToolkit();

    NiStringTokenizer strShaderTok(strShaderPath);
    bool bFirst = true;
    while (!strShaderTok.IsEmpty())
    {
        NiString strPath = strShaderTok.GetNextToken(";");
        int iPtr = strPath.FindReverse('\\');
        NIASSERT(iPtr >= 0);
        if ((unsigned int) iPtr < strPath.Length() - 1)
        {
            strPath += '\\';
        }

        if (bFirst)
        {
            NiString strFile = strPath + "Data";
            pkMaterialKit->SetMaterialDirectory(strFile, true);
            bFirst = false;
        }

        pkMaterialKit->LoadFromDLL(strPath);
    }
    
    ReturnCode eNetworkReturnCode = SUCCESS;
    ReturnCode eLocalReturnCode = SUCCESS;

    NiString strNetworkPath = 
        NiMAXOptions::GetNetworkScriptDirectory();
    if (!strNetworkPath.IsEmpty())
    {
        eNetworkReturnCode = NiScriptTemplateManager::GetInstance()->
            AddScriptDirectory(strNetworkPath, true, false);
    }

    NiSprintf(acScriptDir, 1024, "%s%s", pcFileString, "Scripts\\");
    eLocalReturnCode = NiScriptTemplateManager::GetInstance()->
        AddScriptDirectory(acScriptDir, true, false);
    
    if (eLocalReturnCode != SUCCESS && eNetworkReturnCode != SUCCESS)
    {
        NiString strMessageBoxTxt = "Unable to load scripts from"
            " the following directories:\n";
        strMessageBoxTxt += acScriptDir;
        strMessageBoxTxt += "\n" + strNetworkPath;
        NiMessageBox(strMessageBoxTxt, "Gamebryo Art Script Loading"
            " Error");
    }

    NiMAXOptions::CreateOptions();
    NiTexture::SetMipmapByDefault(true);

    NiShadowManager::Initialize();

    NiString strName = pcFileString;
    strName += "GamebryoMaxPlugin.log";
    int iLogID = NiLogger::OpenLog(strName, NiLogger::OPEN_OVERWRITE, 
        false, false);
    NiLogger::SetOutputToLogFile(NIMESSAGE_GENERAL_0, iLogID);
    SetLastError(0);

    strName = pcFileString;
    strName += "GamebryoOutputLog.xml";
    NiXMLLogger *pkLogger = NiMAXConverter::GetXMLLogger();
    if (pkLogger)
        pkLogger->CreateLog(strName);
    return 1;
}
//---------------------------------------------------------------------------
__declspec(dllexport) int LibShutdown(void)
{
    NiXMLLogger *pkLogger = NiMAXConverter::GetXMLLogger();
    if (pkLogger)
        pkLogger->CloseLog();

    NiShadowManager::Shutdown();

    NiMaterialToolkit::DestroyToolkit();
    NiMAXOptions::DestroyOptions();
    MiViewerPlugin::CleanUp();
    NiFramework::ShutdownFramework();
    return 1;
}
//---------------------------------------------------------------------------

