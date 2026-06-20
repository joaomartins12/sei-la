#include "stdafx.h"
#include "..\Engine.h"

#ifndef _LIB
    #define _LIB
#endif

#include <NSBShaderLib.h>
#include <NSFParserLib.h>
#include <NiD3DXEffectShaderLib.h>

#pragma comment(lib, "NiBinaryShaderLib.lib")
#pragma comment(lib, "NiD3D10BinaryShaderLib.lib")
#pragma comment(lib, "NSBShaderLib.lib")
#pragma comment(lib, "NSFParserLib.lib")

///////////////////////////////////////////////////////////////////////

void CEngine::_CreateShaderSystem()
{
    NiShaderFactory::RegisterErrorCallback(ShaderCallback_Errors);

    const char* pcShaderDir = "Data/Shaders";   
    NiShaderFactory::AddShaderProgramFileDirectory("Data/Shaders/Common");

	if( _RunShaderParsers( pcShaderDir ) == false )
    {
        NiMessageBox("Failed to run shader parsers!", "ERROR");
    }

    if( _RunShaderLibraries( pcShaderDir ) == false)
    {
        NiMessageBox("Failed to run shader parsers!", "ERROR");
    }
	
}
//---------------------------------------------------------------------------
bool CEngine::_RunShaderParsers(const char* pcShaderDir)
{
    NiShaderFactory::RegisterRunParserCallback(ShaderCallback_RunParser);

    const char* pcLibName = NULL;

    NiShaderFactory::LoadAndRunParserLibrary(pcLibName, pcShaderDir, true);
    return true;
}
//---------------------------------------------------------------------------
bool CEngine::_RunShaderLibraries(const char* pcShaderDir)
{
    NiShaderFactory::RegisterClassCreationCallback( ShaderCallback_LibraryClassCreate );

    unsigned int uiCount = 1;
    char* apcDirectories[1];
    apcDirectories[0] = (char*)pcShaderDir;

    const char* pcLibName = NULL;

    if (!NiShaderFactory::LoadAndRegisterShaderLibrary(pcLibName, uiCount, apcDirectories, true))
    {
        NiMessageBox("Failed to load shader library!", "ERROR");
        return false;
    }

	NiShaderFactory::RegisterClassCreationCallback(FXLibraryClassCreate);

	if (!NiShaderFactory::LoadAndRegisterShaderLibrary(pcLibName, uiCount, apcDirectories, true))
	{
		NiMessageBox("Failed to load Fx shader library!", "ERROR");
		return false;
	}

    return true;
}
//---------------------------------------------------------------------------
bool CEngine::ShaderCallback_LibraryClassCreate(const char* pcLibFile, 
    NiRenderer* pkRenderer, int iDirectoryCount, char* apcDirectories[], 
    bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary)
{
    // The library variable tells the shader factory to insert the shader
    // library into its list of known shader libraries. Set it to NULL
    // by default just in case the subsequent function call fails.
    *ppkLibrary = NULL;

    // Call the shader library loader on the specified directory.
    // Note that if multiple libraries were supported in this app, we would
    // need to compare against the pcLibFile.
    if (!NSBShaderLib_LoadShaderLibrary(pkRenderer, iDirectoryCount, 
        apcDirectories, bRecurseSubFolders, ppkLibrary))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// sms
bool CEngine::FXLibraryClassCreate(const char* pcLibFile, 
								   NiRenderer* pkRenderer, int iDirectoryCount, char* apcDirectories[], 
								   bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary)
{
	*ppkLibrary = NULL;

	// Create the FX Shader Library.
	return NiD3DXEffectShaderLib_LoadShaderLibrary(pkRenderer, 
		iDirectoryCount, apcDirectories, bRecurseSubFolders, ppkLibrary);
}
//---------------------------------------------------------------------------
unsigned int CEngine::ShaderCallback_RunParser(const char* pcLibFile, 
    NiRenderer* pkRenderer, const char* pcDirectory, 
    bool bRecurseSubFolders)
{
    // Call the shader parser on the specified directory.
    // Note that if multiple parsers were supported in this app, we would
    // need to compare against the pcLibFile.
    return NSFParserLib_RunShaderParser(pcDirectory, bRecurseSubFolders);
}
//---------------------------------------------------------------------------
unsigned int CEngine::ShaderCallback_Errors(const char* pcError, 
    NiShaderError eError, bool bRecoverable)
{
    // Report any errors to the user
    NiOutputDebugString("ERROR: ");
    NiOutputDebugString(pcError);
    return 0;
}


//---------------------------------------------------------------------------
void CEngine::ConfirmShader(NiAVObject* pkObject)
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (NiIsKindOf(NiDX9Renderer, pkRenderer))
    {
        NiDX9Renderer* pkDX9Renderer = (NiDX9Renderer*)pkRenderer;
        const D3DCAPS9* pkCaps = pkDX9Renderer->GetDeviceCaps();

        if (pkCaps->MaxSimultaneousTextures < 4 ||
            pkCaps->MaxTextureBlendStages < 4)
        {
            // Need to use simpler 2-pass shader that uses 
            // only 2 textures/stages per pass
            RecursiveConfirmShader(pkObject);
        }
    }
}
//---------------------------------------------------------------------------
void CEngine::RecursiveConfirmShader(NiAVObject* pkObject)
{
    if (NiIsKindOf(NiGeometry, pkObject))
    {
        NiGeometry* pkGeom = (NiGeometry*)pkObject;

        const NiMaterial* pkMaterial = pkGeom->GetActiveMaterial();
        if (pkMaterial && NiIsKindOf(NiSingleShaderMaterial, pkMaterial))
        {
            // Implementation 2 of this shader is the 2-pass version
            pkGeom->RemoveMaterial(pkMaterial->GetName());
            const char* pcMaterialName = pkMaterial->GetName();
            pkGeom->ApplyAndSetActiveMaterial(pcMaterialName, 2);
        }
    }
    else if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*)pkObject;
        unsigned int uiArraySize = pkNode->GetArrayCount();
        for (unsigned int i = 0; i < uiArraySize; i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);
            if (pkChild)
                RecursiveConfirmShader(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
void CEngine::SetGaussianBlurVal( float fOrgVal, float fBlurVal, float fFillter )
{
	m_vGaussianBlurVal.x = fOrgVal;
	m_vGaussianBlurVal.y = fBlurVal;
	m_vGaussianBlurVal.z = fBlurVal;

	if( m_pRT_GB_Blur )
		NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_GaussianVal", sizeof( float )*4, &m_vGaussianBlurVal );
}