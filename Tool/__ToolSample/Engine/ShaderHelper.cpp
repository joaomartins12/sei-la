// NUMERICAL DESIGN LIMITED PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Numerical Design Limited and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2005 Numerical Design Limited.
//      All Rights Reserved.
//
// Numerical Design Limited, Chapel Hill, North Carolina 27514
// http://www.ndl.com
//---------------------------------------------------------------------------
#include "stdafx.h"
#include "ShaderHelper.h"

// This sample only has static library configurations
#define _LIB
    #include <NSBShaderLib.h>
    #include <NSFParserLib.h>
	#include <NiD3DXEffectShaderLib.h>

//	#include <ShaderLibraryLib.h>
#undef _LIB


//#pragma comment(lib, "ShaderLibraryDX9.lib")


//---------------------------------------------------------------------------
bool ShaderHelper::SetupShaderSystem(
    char* apcProgramDirectory[], unsigned int uiNumProgramDirectories, 
    char* apcShaderDirectories[], unsigned int uiNumShaderDirectories)
{
    assert (NiD3DShaderProgramFactory::GetInstance());

	for (unsigned int i = 0; i < uiNumProgramDirectories; i++)
	{
		NiD3DShaderProgramFactory::GetInstance()->AddProgramDirectory(
			apcProgramDirectory[i]);
	}

	m_uiShaderDirectoryCount = uiNumShaderDirectories;
	m_ppcShaderDirectories = apcShaderDirectories;

	m_uiShaderDirectoryCount = uiNumShaderDirectories;
	m_ppcShaderDirectories = apcShaderDirectories;

    // First, we will run the NSF parser. This is done to make sure that
    // any text-based shader files that have been modified are re-compiled
    // to binary before loading all the binary representations.
    if (!RunShaderParsers())
    {
        NiMessageBox("Failed to run shader parsers!", "ERROR");
        return false;
    }

    if (!RegisterShaderLibraries())
    {
        NiMessageBox("Failed to register shader libraries!", "ERROR");
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool ShaderHelper::RunShaderParsers()
{
	NiShaderFactory::RegisterRunParserCallback(NSFRunParser);

    unsigned int uiCount = 0;
    for (unsigned int i = 0; i < m_uiShaderDirectoryCount; i++)
    {
        uiCount += NiShaderFactory::LoadAndRunParserLibrary(
            NULL, m_ppcShaderDirectories[i], true);
	}

#ifndef NI_LOGGER_DISABLE
	if (uiCount == 0)
	{
		NiOutputDebugString("parser library failed to parse "
			"any shaders!");
	}
	else
	{
		char acTemp[256];
		NiSprintf(acTemp, 256, "Parsed %d shaders from NSFShaderParser!\n", 
			uiCount);
		NiOutputDebugString(acTemp);
	}
#endif //NI_LOGGER_DISABLE

    return true;
}
//---------------------------------------------------------------------------
bool ShaderHelper::RegisterShaderLibraries()
{
	NiShaderFactory::RegisterClassCreationCallback(NSBLibraryClassCreate);
	if (!NiShaderFactory::LoadAndRegisterShaderLibrary(NULL, 
		m_uiShaderDirectoryCount, m_ppcShaderDirectories, true))
	{
		NiMessageBox("Failed to load shader library!", "ERROR");
		return false;
	}

	NiShaderFactory::RegisterClassCreationCallback(FXLibraryClassCreate);
    if (!NiShaderFactory::LoadAndRegisterShaderLibrary(NULL,
        m_uiShaderDirectoryCount, m_ppcShaderDirectories, true))
    {
        NiMessageBox("Failed to load Fx shader library!", "ERROR");
        return false;
    }

	NiShaderFactory::RegisterClassCreationCallback(LibraryClassCreate);
	const char* const acShaderLibName = "ShaderLibraryDX9" NI_DLL_SUFFIX
		".dll";
	if (!NiShaderFactory::LoadAndRegisterShaderLibrary(acShaderLibName,
		m_uiShaderDirectoryCount, m_ppcShaderDirectories, true))
	{
		//NiMessageBox("Failed to load shader library!", "ERROR");
		//return false;
	}

    return true;
}
//---------------------------------------------------------------------------
bool ShaderHelper::NSBLibraryClassCreate(const char* pcLibFile, 
										 NiRenderer* pkRenderer, int iDirectoryCount, char* apcDirectories[], 
										 bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary)
{
	*ppkLibrary = NULL;

	// Create the NSB Shader Library.
	return NSBShaderLib_LoadShaderLibrary(pkRenderer, iDirectoryCount, 
		apcDirectories, bRecurseSubFolders, ppkLibrary);
}
//---------------------------------------------------------------------------
bool ShaderHelper::FXLibraryClassCreate(const char* pcLibFile, 
										NiRenderer* pkRenderer, int iDirectoryCount, char* apcDirectories[], 
										bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary)
{
	*ppkLibrary = NULL;

	// Create the FX Shader Library.
	return NiD3DXEffectShaderLib_LoadShaderLibrary(pkRenderer, 
		iDirectoryCount, apcDirectories, bRecurseSubFolders, ppkLibrary);
}
//---------------------------------------------------------------------------
bool ShaderHelper::FXLiteLibraryClassCreate(const char* pcLibFile, 
											NiRenderer* pkRenderer, int iDirectoryCount, char* apcDirectories[], 
											bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary)
{
	*ppkLibrary = NULL;

	return false;
}
//---------------------------------------------------------------------------
unsigned int ShaderHelper::NSFRunParser(const char* pcLibFile, 
										NiRenderer* pkRenderer, const char* pcDirectory, 
										bool bRecurseSubFolders)
{
	// Run the NSF Parser.
	return NSFParserLib_RunShaderParser(pcDirectory, bRecurseSubFolders);
}
//---------------------------------------------------------------------------
bool ShaderHelper::LibraryClassCreate(const char* pcLibFile, 
									   NiRenderer* pkRenderer, int iDirectoryCount, char* apcDirectories[], 
									   bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary)
{
	*ppkLibrary = NULL;

	// Create the Shader Library...
	//return ShaderLibraryLib_LoadShaderLibrary(pkRenderer, 
	//	iDirectoryCount, apcDirectories, bRecurseSubFolders, ppkLibrary);
	return false;
}
//---------------------------------------------------------------------------
