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
// Precompiled Header
#include "NiFXLShaderLibPCH.h"

#include "NiFXLFactory.h"
#include "NiFXLFile.h"
#include "NiFXLInclude.h"

#include <NiD3DShaderFactory.h>
#include <NiD3DShaderProgramCreator.h>
#include <NiD3DUtility.h>

//---------------------------------------------------------------------------
NiFXLFactory* NiFXLFactory::ms_pkFactory = 0;
char* NiFXLFactory::ms_pcFXLDirectory = 0;

unsigned int NiFXLFactory::ms_uiCreationFlags = 0;

//---------------------------------------------------------------------------
// Device and renderer access functions
//---------------------------------------------------------------------------
NiFXLFactory::NiFXLFactory()
{
    // Maps for FXL names to shaders.
    m_pkFXLMap = NiNew NiTStringPointerMap<NiFXLFile*>(59, false);
    NIASSERT(m_pkFXLMap);
}
//---------------------------------------------------------------------------
NiFXLFactory::~NiFXLFactory()
{
    DestroyRendererData();

    if (m_pkFXLMap)
        m_pkFXLMap->RemoveAll();
    NiDelete m_pkFXLMap;
}
//---------------------------------------------------------------------------
NiFXLFactory* NiFXLFactory::GetInstance(bool bCreateNew)
{
    if (bCreateNew && ms_pkFactory == NULL)
        ms_pkFactory = NiNew NiFXLFactory;
    return ms_pkFactory;
}
//---------------------------------------------------------------------------
NiFXLFile* NiFXLFactory::CreateFXLFromFile(const char* pcFileName)
{
    if (!ms_pkFactory)
        GetInstance();

    if (!pcFileName || (strcmp(pcFileName, "") == 0))
        return 0;

    // See if it exists already, and return it
    NiFXLFile* pkFXLFile = ms_pkFactory->GetFXLFile(pcFileName);
    if (pkFXLFile)
    {
        LPD3DXEFFECT pkEffect = NULL;//pkFXLFile->GetRenderableEffect();
        if (pkEffect == NULL)
            ms_pkFactory->LoadFXLFromFile(pcFileName, pkFXLFile);

        return pkFXLFile;
    }

    if (!ms_pkFactory->LoadFXLFromFile(pcFileName, pkFXLFile))
    {
        return 0;
    }

    NIASSERT(pkFXLFile);

    pkFXLFile->SetDXVersion(NiShader::NISHADER_XENON);

    //if (pkFXLFile->GetRenderableEffect() == NULL)
      //  pkFXLFile->DestroyRendererData();

    // Load the code from the file

    // Insert it in the list
    ms_pkFactory->InsertFXLFileIntoMap(pkFXLFile);

    return pkFXLFile;
}
//---------------------------------------------------------------------------
NiFXLFile* NiFXLFactory::CreateFXLFromCode(const char* pcEffectName, 
    unsigned int uiCodeSize, void* pvCode)
{
    if (!ms_pkFactory)
        return 0;

    if (!pcEffectName || (strcmp(pcEffectName, "") == 0))
        return 0;

    // See if it exists already, and return it
    NiFXLFile* pkFXLFile = ms_pkFactory->GetFXLFile(pcEffectName);
    if (pkFXLFile)
        return pkFXLFile;

    if (NiXenonRenderer::GetRenderer()->GetD3DDevice() == NULL)
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
            "No D3D device exists for creating D3D Effects\n");
        return 0;
    }

    // Load the code from the binary
    if (!ms_pkFactory->LoadFXLFromCode(pcEffectName, uiCodeSize, 
        pvCode, pkFXLFile))
    {
        return 0;
    }

    // Insert it in the list
    ms_pkFactory->InsertFXLFileIntoMap(pkFXLFile);

    return pkFXLFile;
}
//---------------------------------------------------------------------------
const char* NiFXLFactory::GetFXLDirectory()
{
    return ms_pcFXLDirectory;
}
//---------------------------------------------------------------------------
void NiFXLFactory::SetFXLDirectory(const char* pcDirectory)
{
    NiDelete[] ms_pcFXLDirectory;
    ms_pcFXLDirectory = 0;

    if (pcDirectory && pcDirectory[0] != '\0')
    {
        unsigned int uiLen = strlen(pcDirectory) + 1;
        ms_pcFXLDirectory = NiAlloc(char, uiLen);
        NIASSERT(ms_pcFXLDirectory);
        NiStrcpy(ms_pcFXLDirectory, uiLen, pcDirectory);
    }
}
//---------------------------------------------------------------------------
bool NiFXLFactory::LoadFXLFromFile(const char* pcFileName, 
    NiFXLFile*& pkFXLFile)
{
    // Verify the name
    if (!pcFileName || (strcmp(pcFileName, "") == 0))
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
            "Missing FXL file name\n");
        return false;
    }

    // 'Parse' off the extension
    char acExt[_MAX_EXT];

    NiD3DUtility::GetFileExtension(pcFileName, acExt, _MAX_EXT);
    if (NiStricmp(acExt, ".fxl") == 0)
    {
        char acShaderPath[_MAX_PATH];

        if (!ResolveFXLFileName(pcFileName, acShaderPath, _MAX_PATH))
        {
            // Can't resolve the effect!
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "Failed to find FXL file %s\n", pcFileName);
            return false;
        }

        unsigned int uiLength = 0;
        char* pcText = GetShaderText(acShaderPath, uiLength);

        if (pcText == NULL)
        {
            // Can't open the effect and get text!
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "Failed to get text for FXL file %s\n", pcFileName);
            return false;
        }

        NiFXLInclude kFXLInclude(acShaderPath);

        ID3DXBuffer* pkBuffer, *pkErrors;
        HRESULT eResult = FXLCompileEffect(pcText, uiLength, NULL,
            &kFXLInclude, D3DXSHADER_FXLPARAMETERS_AS_VARIABLE_NAMES,
            &pkBuffer, &pkErrors);

        if (FAILED(eResult))
        {
            // Compilation failed.
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "Failed to compile FXL file %s\n", pcFileName);
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                (char*)pkErrors->GetBufferPointer());
            return false;
        }

        NiFree(pcText);

        if (pkErrors != NULL)
        {
            pkErrors->Release();
            pkErrors = 0;
        }

        FXLEffect* pkEffect = NULL;
        eResult = FXLCreateEffect(
            NiXenonRenderer::GetRenderer()->GetD3DDevice(), 
            pkBuffer->GetBufferPointer(), NULL, &pkEffect);
        pkBuffer->Release();

        if (FAILED(eResult))
        {
            // Creation Failed
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "Failed to create FXL interface for %s\n", pcFileName);
            return false;
        }

        if (pkFXLFile == NULL)
        {
            pkFXLFile = NiNew NiFXLFile();
            NIASSERT(pkFXLFile);
            pkFXLFile->SetName(pcFileName);
        }

        bool bSuccess = pkFXLFile->Initialize(pkEffect);
        NIASSERT(bSuccess);

        return true;
    }

    pkFXLFile = NULL;

    NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
        "Invalid FXL file %s\n", pcFileName);

    return false;
}
//---------------------------------------------------------------------------
bool NiFXLFactory::LoadFXLFromCode(const char* pcEffectName, 
    unsigned int uiCodeSize, void* pvCode, NiFXLFile*& pkFXLFile)
{
    ID3DXBuffer* pkBuffer, *pkErrors;
    HRESULT eResult = FXLCompileEffect((const char*)pvCode, uiCodeSize, NULL, 
        NULL, D3DXSHADER_FXLPARAMETERS_AS_VARIABLE_NAMES, &pkBuffer, 
        &pkErrors);

    if (FAILED(eResult))
    {
        // Compilation failed.
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
            "Failed to compile FXL file %s\n", pcEffectName);
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
            (char*)pkErrors->GetBufferPointer());
        return false;
    }

    if (pkErrors != NULL)
    {
        pkErrors->Release();
        pkErrors = 0;
    }

    FXLEffect* pkEffect = NULL;
    eResult = FXLCreateEffect(NiXenonRenderer::GetRenderer()->GetD3DDevice(),
        pkBuffer->GetBufferPointer(), NULL, &pkEffect);
    pkBuffer->Release();

    if (FAILED(eResult))
    {
        // Creation Failed
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
            "Failed to create FXL interface for %s\n", pcEffectName);
        return false;
    }
 
    if (pkFXLFile == NULL)
    {
        pkFXLFile = NiNew NiFXLFile();
    }
    NIASSERT(pkFXLFile);

    pkFXLFile->SetName(pcEffectName);
    bool bSuccess = pkFXLFile->Initialize(pkEffect);
    NIASSERT(bSuccess);

    return true;
}
//---------------------------------------------------------------------------
NiFXLFile* NiFXLFactory::GetFXLFile(
    const char* pcShaderName) const
{
    NiFXLFile* spFXLFile = NULL;
    if (m_pkFXLMap->GetAt(pcShaderName, spFXLFile))
        return spFXLFile;

    return NULL;
}
//---------------------------------------------------------------------------
void NiFXLFactory::InsertFXLFileIntoMap(NiFXLFile* pkFXLFile)
{
    if (pkFXLFile == NULL)
        return;

    const char* pcName = pkFXLFile->GetName();

    if (!pcName || strcmp(pcName, "") == 0)
        return;

    if (!m_pkFXLMap)
    {
        NIASSERT(!"InsertFXLFileIntoMap> Invalid Map!");
        return;
    }

    m_pkFXLMap->SetAt(pcName, pkFXLFile);
}
//---------------------------------------------------------------------------
void NiFXLFactory::RemoveFXLFileFromMap(const char* pcName)
{
    if (!pcName)
        return;

    if (!m_pkFXLMap)
    {
        NIASSERT(!"RemoveFXLFromMap> Invalid Map!");
        return;
    }

    m_pkFXLMap->RemoveAt(pcName);
}

//---------------------------------------------------------------------------
bool NiFXLFactory::ResolveFXLFileName(
    const char* pcOriginalName, char* pcTrueName, unsigned int uiBufferSize)
{
    // See if the file exists as-is
    if (NiFile::Access(pcOriginalName, NiFile::READ_ONLY))
    {
        NiStrcpy(pcTrueName, uiBufferSize, pcOriginalName);
    }
    else
    {
        if (!ms_pkFactory)
        {
            // We don't have a valid factory, so we can't grab the directory
            // the app set to check for the file.
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "No valid FXL factory\n");
            return false;
        }

        // Path was not valid. 
        char acDrive[_MAX_DRIVE];
        char acDir[_MAX_PATH];
        char acFileName[_MAX_PATH];
        char acFileExt[_MAX_EXT];

        NiD3DUtility::GetSplitPath(pcOriginalName, acDrive, acDir, 
            acFileName, acFileExt);

        // Try the current shader directory
        const char* pcProgDir = GetFXLDirectory();
        if (!pcProgDir || (strcmp(pcProgDir, "") == 0))
        {
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "Invalid or no FXL directory\n");
            return false;
        }

        if ((pcProgDir[strlen(pcProgDir) - 1] != '/') &&
            (pcProgDir[strlen(pcProgDir) - 1] != '\\'))
        {
            NiSprintf(pcTrueName, uiBufferSize, "%s\\%s%s", pcProgDir, 
                acFileName, acFileExt);
        }
        else
        {
            NiSprintf(pcTrueName, uiBufferSize, "%s%s%s", pcProgDir, 
                acFileName, acFileExt);
        }

        if (!NiFile::Access(pcTrueName, NiFile::READ_ONLY))
        {
            // Not found!
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "FXL file not found %s\n", pcOriginalName);
            return false;
        }
    }

    // It can be assumed the file was found at this point!
    return true;
}
//---------------------------------------------------------------------------
void NiFXLFactory::DestroyRendererData()
{
    NiTMapIterator kIter = m_pkFXLMap->GetFirstPos();
    while (kIter)
    {
        const char* pcName = NULL;
        NiFXLFile* pkFile;
        m_pkFXLMap->GetNext(kIter, pcName, pkFile);
        pkFile->DestroyRendererData();
    }
}
//---------------------------------------------------------------------------
void NiFXLFactory::_SDMInit()
{
    // Nothing to do in the init at the moment
}
//---------------------------------------------------------------------------
void NiFXLFactory::_SDMShutdown()
{
    // Free the D3DXEffect directory
    NiDelete ms_pkFactory;
    ms_pkFactory = 0;
    SetFXLDirectory(0);
}
//---------------------------------------------------------------------------
unsigned int NiFXLFactory::GetFXLCreationFlags()
{
    return ms_uiCreationFlags;
}
//---------------------------------------------------------------------------
void NiFXLFactory::SetFXLCreationFlags(
    unsigned int uiFlags)
{
    ms_uiCreationFlags = uiFlags;
}
//---------------------------------------------------------------------------
char* NiFXLFactory::GetShaderText(const char* pcShaderName, 
    unsigned int& uiLength)
{
    // Open a file. We'll use the default buffer of 32KB because the odds
    // of FXL code being longer than that are fairly low.
    NiFile* pkFile = NiNew NiFile(pcShaderName, NiFile::READ_ONLY);

    // Test that the file allocated and that it opened. The boolean operator
    // on an NiFile object returns the m_bGood flag from that object.
    if (!pkFile || !*pkFile)
    {
        uiLength = 0;
        return NULL;
    }

    uiLength = pkFile->GetFileSize();

    char* pkText = NULL;
    if (uiLength != 0)
    {
        pkText = NiAlloc(char, uiLength);
        pkFile->Read((void*)pkText, uiLength);
    }

    NiDelete pkFile;

    return pkText;
}
//---------------------------------------------------------------------------
