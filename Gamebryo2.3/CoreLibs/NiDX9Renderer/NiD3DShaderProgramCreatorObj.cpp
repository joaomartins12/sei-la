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

// Precompiled Header
#include "NiD3DRendererPCH.h"

#include "NiD3DShaderProgramCreatorObj.h"
#include "NiD3DShaderFactory.h"

NiD3DShaderProgramCreatorObj* 
    NiD3DShaderProgramCreatorObj::ms_pkCreator = NULL;

//---------------------------------------------------------------------------
bool NiD3DShaderProgramCreatorObj::LoadShaderCodeFromFile(
    const char* pcFileName, void*& pvCode, unsigned int& uiCodeSize, 
    void*& pvConstantData, unsigned int& uiConstantSize, bool bRecoverable)
{
    pvCode = NULL;
    uiCodeSize = 0;
    pvConstantData = NULL;
    uiConstantSize = 0;

    // Verify the name
    if (pcFileName == NULL || pcFileName[0] == '\0')
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
            "Invalid shader file name\n");
        return false;
    }

    // Resolve shader program file
    char acShaderPath[_MAX_PATH];
    if (!NiD3DShaderProgramFactory::ResolveShaderFileName(pcFileName, 
        acShaderPath, _MAX_PATH))
    {
        // Can't resolve the shader!
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
            "Failed to find shader program file %s\n", pcFileName);
        return false;
    }

    //  Open the file
    HANDLE hFile = CreateFile(acShaderPath, GENERIC_READ, FILE_SHARE_READ, 
        NULL, OPEN_EXISTING, 0, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
            "Invalid shader file %s\n", acShaderPath);
        return false;
    }

    //  Get the file size and allocate memory to hold the shader file
    DWORD dwSize = GetFileSize(hFile, NULL);
    BYTE* pbyData = NiAlloc(BYTE, dwSize + 4);
    if (NULL == pbyData)
        return false;

    memset(pbyData, 0, dwSize + 4);

    //  Read the vertex shader
    DWORD dwBytesRead;

    ReadFile(hFile, pbyData, dwSize, &dwBytesRead, NULL);
    CloseHandle(hFile);
    if (dwBytesRead != dwSize)
    {
        NiFree(pbyData);
        return false;
    }

    uiCodeSize = dwSize;
    pvCode = (void*)pbyData;

    return true;
}
//---------------------------------------------------------------------------
bool NiD3DShaderProgramCreatorObj::LoadShaderCodeFromBuffer(
    const void* pvBuffer, unsigned int uiBufferSize, void*& pvCode, 
    unsigned int& uiCodeSize, void*& pvConstantData, 
    unsigned int& uiConstantSize, bool bRecoverable)
{
    pvCode = NULL;
    uiCodeSize = 0;
    pvConstantData = NULL;
    uiConstantSize = 0;

    // Verify the buffer
    if (pvBuffer == NULL || uiBufferSize == 0)
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
            "Invalid shader buffer\n");
        return false;
    }

    pvCode = NiAlloc(BYTE, uiBufferSize);
    NiMemcpy(pvCode, pvBuffer, uiBufferSize);
    uiCodeSize = uiBufferSize;

    return true;
}
//---------------------------------------------------------------------------
