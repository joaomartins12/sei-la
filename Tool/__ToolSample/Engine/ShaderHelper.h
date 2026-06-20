
#ifndef SHADERHELPER_H
#define SHADERHELPER_H

#include "NiDirectXVersion.h"
#include "NiD3DDefines.h"

#include "NiShader.h"
#include "NiShaderFactory.h"
#include "NiShaderLibrary.h"
#include "NiShaderLibraryDesc.h"
#include "NiD3DShaderProgramFactory.h"
#include "NiD3DRendererHeaders.h"

#pragma comment(lib, "NiBinaryShaderLibDX9.lib")
#pragma comment(lib, "NSBShaderLibDX9.lib")
#pragma comment(lib, "NSFParserLibDX9.lib")
#pragma comment(lib, "NiD3DXEffectShaderLibDX9.lib")


class ShaderHelper
{
public:
	bool SetupShaderSystem(
		char* apcProgramDirectories[], unsigned int uiNumProgramDirectories,
		char* apcShaderDirectories[], unsigned int uiNumShaderDirectories);
    bool RunShaderParsers();
    bool RegisterShaderLibraries();

protected:
	static bool NSBLibraryClassCreate(const char* pcLibFile,
		NiRenderer* pkRenderer, int iDirectoryCount, char* apcDibrectories[], 
		bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary);
	static bool FXLibraryClassCreate(const char* pcLibFile,
		NiRenderer* pkRenderer, int iDirectoryCount, char* apcDirectories[], 
		bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary);
	static bool FXLiteLibraryClassCreate(const char* pcLibFile,
		NiRenderer* pkRenderer, int iDirectoryCount, char* apcDirectories[], 
		bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary);
	static unsigned int NSFRunParser(const char* pcLibFile, 
		NiRenderer* pkRenderer, const char* pcDirectory, 
		bool bRecurseSubFolders);
	static bool LibraryClassCreate(const char* pcLibFile, 
		NiRenderer* pkRenderer, int iDirectoryCount, char* apcDirectories[], 
		bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary);

	static char* ms_apcShaderParsers[];
	static char* ms_apcShaderLibraries[];

	unsigned int m_uiShaderDirectoryCount;
	char** m_ppcShaderDirectories;
};

#endif
