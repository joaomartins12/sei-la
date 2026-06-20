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
//---------------------------------------------------------------------------
#include "NiFXLShaderLibPCH.h"

#include "NiFXLLoader.h"
#include "NiFXLFactory.h"
#include "NiFXLShaderLibrary.h"

#include <NiD3DUtility.h>
#include <NiShaderLibraryDesc.h>

//---------------------------------------------------------------------------
NiFXLLoader* NiFXLLoader::ms_pkLoader = 0;

//---------------------------------------------------------------------------
NiFXLLoader::NiFXLLoader()
{
    m_kFXLList.RemoveAll();
}
//---------------------------------------------------------------------------
NiFXLLoader::~NiFXLLoader()
{
    m_kFXLListIter = m_kFXLList.GetHeadPos();
    while (m_kFXLListIter)
    {
        char* pcName = m_kFXLList.GetNext(m_kFXLListIter);
        if (pcName)
            NiFree(pcName);
    }
    m_kFXLList.RemoveAll();
}
//---------------------------------------------------------------------------
NiFXLLoader* NiFXLLoader::Create()
{
    if (ms_pkLoader)
    {
        NiTListIterator kIter = ms_pkLoader->m_kFXLList.GetHeadPos();
        while (kIter)
        {
            char* pcName = ms_pkLoader->m_kFXLList.GetNext(kIter);
            if (pcName)
                NiFree(pcName);
        }
        ms_pkLoader->m_kFXLList.RemoveAll();
    }
    else
    {
        ms_pkLoader = NiNew NiFXLLoader();
    }

    return ms_pkLoader;
}
//---------------------------------------------------------------------------
void NiFXLLoader::Destroy()
{
    if (ms_pkLoader)
        NiDelete ms_pkLoader;

    ms_pkLoader = 0;
}
//---------------------------------------------------------------------------
bool NiFXLLoader::LoadAllFXLFiles(
    NiFXLShaderLibrary* pkLibrary, const char* pcDirectory, 
    bool bRecurseSubDirs)
{
    FindAllFXLFiles(pcDirectory, bRecurseSubDirs);
    if (!LoadAllFXLFiles(pkLibrary))
        return false;

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiFXLLoader::GetFXLFileCount()
{
    return m_kFXLList.GetSize();
}
//---------------------------------------------------------------------------
const char* NiFXLLoader::GetFirstFXLFile()
{
    m_kFXLListIter = m_kFXLList.GetHeadPos();
    
    if (m_kFXLListIter)
        return m_kFXLList.GetNext(m_kFXLListIter);
    return 0;
}
//---------------------------------------------------------------------------
const char* NiFXLLoader::GetNextFXLFile()
{
    if (m_kFXLListIter)
        return m_kFXLList.GetNext(m_kFXLListIter);
    return 0;
}
//---------------------------------------------------------------------------
void NiFXLLoader::FindAllFXLFiles(const char* pcDirectory, 
    bool bRecurseDirectories)
{
    FindAllFXLFilesInDirectory(pcDirectory, ".FXL", bRecurseDirectories,
        &m_kFXLList);
}
//---------------------------------------------------------------------------
unsigned int NiFXLLoader::FindAllFXLFilesInDirectory(
    const char* pcDirectory, const char* pcExt, bool bRecurseDirectories, 
    NiTPointerList<char*>* pkFileList)
{
    if (!pcDirectory || (strcmp(pcDirectory, "") == 0))
        return 0;
    if (!pcExt || (strcmp(pcExt, "") == 0))
        return 0;

    char acFilePath[_MAX_PATH];
    {
        NiStrncpy(acFilePath, _MAX_PATH, pcDirectory, _MAX_PATH - 1);
        int iLen = strlen(acFilePath);
        if ((acFilePath[iLen - 1] != '\\') && (acFilePath[iLen - 1] != '/'))
        {
            acFilePath[iLen] = '\\';
            acFilePath[iLen + 1] = 0;
        }

        // replace / with \ .
        for (int iPos = 0; iPos < iLen; iPos++)
        {
            if (acFilePath[iPos] == '/')
                acFilePath[iPos] = '\\';
        }
    }

    char acFileName[_MAX_PATH];
    {
        NiStrcpy(acFileName, _MAX_PATH, pcDirectory);
        int iLen = strlen(acFileName);
        if ((acFileName[iLen - 1] != '\\') && (acFileName[iLen - 1] != '/'))
        {
            acFileName[iLen] = '\\';
            acFileName[iLen + 1] = 0;
        }

        // replace / with \ .
        for (int iPos = 0; iPos < iLen; iPos++)
        {
            if (acFileName[iPos] == '/')
                acFileName[iPos] = '\\';
        }
    }

    // This will cover the case when the directory is a mapped network
    // drive...
    NiStrcat(acFileName, _MAX_PATH, "*");

    unsigned int uiCount = 0;
    bool bDone = false;
    char acFileName2[_MAX_PATH];

    WIN32_FIND_DATA wfd;
    memset(&wfd, 0, sizeof(WIN32_FIND_DATA));
    HANDLE hFile = FindFirstFile(acFileName, &wfd);

    if (INVALID_HANDLE_VALUE != hFile)
    {
        NiStrcpy(acFileName2, _MAX_PATH, acFilePath);
        NiStrcat(acFileName2, _MAX_PATH, wfd.cFileName);

        while (!bDone)
        {
            NiStrcpy(acFileName2, _MAX_PATH, acFilePath);
            NiStrcat(acFileName2, _MAX_PATH, wfd.cFileName);

            if (hFile == INVALID_HANDLE_VALUE)
            {
                DWORD dwLastErr = GetLastError();
                NiD3DUtility::Log("Invalid handle on FindXXXXXFile\n");
                bDone = true;
            }
            else
            {
                DWORD dwAttrib = GetFileAttributes(acFileName2);
                if ((dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
                {
                    if (strcmp(wfd.cFileName, "."))
                    {
                        if (strcmp(wfd.cFileName, ".."))
                        {
                            // If we are recursing... do it
                            if (bRecurseDirectories)
                            {
                                NiStrcat(acFileName2, _MAX_PATH, "\\");
                                NiD3DUtility::Log("    Recurse directory %s\n",
                                    acFileName2);
                                uiCount += FindAllFXLFilesInDirectory(
                                    acFileName2, pcExt, bRecurseDirectories,
                                    pkFileList);
                            }
                        }
                    }
                }
                else
                {
                    if (ProcessFXLFile(acFileName2, pcExt, pkFileList))
                        uiCount++;
                }
            }

            if (FindNextFile(hFile, &wfd) == false)
                bDone = true;
        }

        FindClose(hFile);
    }

    return uiCount;
}
//---------------------------------------------------------------------------
bool NiFXLLoader::ProcessFXLFile(const char* pcFilename, 
    const char* pcExt, NiTPointerList<char*>* pkFileList)
{
    if (!pcFilename || (strcmp(pcFilename, "") == 0))
        return false;

    char acExt[_MAX_EXT];

    NiD3DUtility::GetFileExtension(pcFilename, acExt, _MAX_EXT);
    if (NiStricmp(acExt, pcExt) == 0)
    {
        NiD3DUtility::Log("        Found %s File %s\n", pcExt, pcFilename);

        // Add it to the list
        unsigned int uiLen = strlen(pcFilename) + 1;
        char* pcNew = NiAlloc(char, uiLen);
        NIASSERT(pcNew);

        NiStrcpy(pcNew, uiLen, pcFilename);

        pkFileList->AddTail(pcNew);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiFXLLoader::LoadAllFXLFiles(
    NiFXLShaderLibrary* pkLibrary)
{
    // Lock the renderer if necessary so that we can have unfettered 
    // access to Direct3D.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (pkRenderer)
        pkRenderer->LockRenderer();

    const char* pcFile = GetFirstFXLFile();
    while (pcFile)
    {
        NIASSERT(pkLibrary);

        // Parse the name and see if it's in the list.
        char acFileName[_MAX_PATH];
        NiD3DUtility::GetFileName(pcFile, acFileName, _MAX_PATH);

        if (pkLibrary->GetFXLFile(acFileName))
        {
            NiD3DUtility::Log("%s (%s) already loaded?\n", 
                acFileName, pcFile);
        }
        else
        {
            NiD3DUtility::Log("Attempting to load %s\n", pcFile);

            NiFXLFile* pkFXLFile = NULL;
            pkFXLFile = NiFXLFactory::CreateFXLFromFile(pcFile);
            if (pkFXLFile)
            {
                NiD3DUtility::Log("Loaded %s\n", pcFile);
                pkLibrary->InsertFXLFileIntoList(pkFXLFile);
            }
            else
            {
                NiD3DUtility::Log("Failed to load %s\n", pcFile);
            }
        }
        pcFile = GetNextFXLFile();
    }

    if (pkRenderer)
        pkRenderer->UnlockRenderer();

    // Always return true, even if no files successfully loaded
    return true;
}
//---------------------------------------------------------------------------
void* NiFXLLoader::CreateEffectFromFile(void* pvD3DDevice, 
    const char* pcShaderPath, unsigned int uiCreationFlags, char*& pcError)
{
    LPDIRECT3DDEVICE9 pkD3DDevice = (LPDIRECT3DDEVICE9)pvD3DDevice;

    LPD3DXEFFECT pkEffect;
    LPD3DXBUFFER pkErrors;

    // Create the effect from the file
    HRESULT eResult = ::D3DXCreateEffectFromFile(pkD3DDevice, pcShaderPath, 
        NULL, NULL, uiCreationFlags, NULL, &pkEffect, &pkErrors);

    if (FAILED(eResult))
    {
        if (pkErrors)
        {
            LPVOID pvBuff = pkErrors->GetBufferPointer();
            if (pvBuff)
            {
                unsigned int uiLen = pkErrors->GetBufferSize();
                pcError = NiAlloc(char, uiLen);
                NIASSERT(pcError);
                NiStrcpy(pcError, uiLen, (const char*)pvBuff);
            }
            pkErrors->Release();
        }
        else
        {
            NiOutputDebugString("No Error Reported\n");
        }
        return NULL;
    }

    if (pkErrors)
        pkErrors->Release();

    return pkEffect;
}
//---------------------------------------------------------------------------
void* NiFXLLoader::CreateEffectFromCode(void* pvD3DDevice, 
    void* pvCode, unsigned int uiCodeSize, unsigned int uiCreationFlags, 
    char*& pcError)
{
    LPDIRECT3DDEVICE9 pkD3DDevice = (LPDIRECT3DDEVICE9)pvD3DDevice;

    LPD3DXEFFECT pkEffect;
    LPD3DXBUFFER pkErrors;

    // Create the effect from the code
    HRESULT eResult = ::D3DXCreateEffect(pkD3DDevice, pvCode, uiCodeSize, 
        NULL, NULL, uiCreationFlags, NULL, &pkEffect, &pkErrors);

    if (FAILED(eResult))
    {
        if (pkErrors)
        {
            LPVOID pvBuff = pkErrors->GetBufferPointer();
            if (pvBuff)
            {
                unsigned int uiLen = pkErrors->GetBufferSize();
                pcError = NiAlloc(char, uiLen);
                NIASSERT(pcError);
                NiStrcpy(pcError, uiLen, (const char*)pvBuff);
            }
            pkErrors->Release();
        }
        else
        {
            NiOutputDebugString("No Error Reported\n");
        }
        return NULL;
    }

    if (pkErrors)
        pkErrors->Release();

    return pkEffect;
}
//---------------------------------------------------------------------------
