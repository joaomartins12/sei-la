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

#include "NiFXLShaderLibrary.h"
#include "NiFXLFactory.h"
#include "NiFXLFile.h"
#include "NiFXLLoader.h"
#include "NiFXLTechnique.h"
#include "NiFXLTechniqueSet.h"
#include <NiShaderLibraryDesc.h>
#include <NiD3DUtility.h>

//---------------------------------------------------------------------------
NiFXLShaderLibrary::NiFXLShaderLibrary() :
    NiD3DShaderLibrary("NiFXLShaderLib")
{
}
//---------------------------------------------------------------------------
NiFXLShaderLibrary::~NiFXLShaderLibrary()
{
    NiOutputDebugString("Releasing FXL Shaders!\n");

    NiTListIterator kIter = m_kFXLFileList.GetHeadPos();
    while (kIter)
    {
        NiFXLFile* pkFile = m_kFXLFileList.RemovePos(kIter);
        NiDelete pkFile;
    }
}
//---------------------------------------------------------------------------
NiFXLShaderLibrary* NiFXLShaderLibrary::Create(
    NiD3DRenderer* pkD3DRenderer, int iDirectoryCount, char* apcDirectories[], 
    bool bRecurseSubFolders) 
{
    NiFXLShaderLibrary* pkShaderLibrary = 
        NiNew NiFXLShaderLibrary();

    if (pkShaderLibrary)
    {
        pkShaderLibrary->SetRenderer(pkD3DRenderer);

        for (int i = 0; i < iDirectoryCount; i++)
        {
            bool bSuccess = pkShaderLibrary->LoadFXLFiles(
                apcDirectories[i], bRecurseSubFolders);
            if (!bSuccess)
            {
                NiD3DUtility::Log("LoadFXLFiles failed on %s\n",
                    apcDirectories[i]);
            }
        }

        if (pkShaderLibrary->GetFXLFileCount() == 0)
        {
            NiDelete pkShaderLibrary;
            pkShaderLibrary = 0;
        }
    }

    return pkShaderLibrary;
}
//---------------------------------------------------------------------------
void NiFXLShaderLibrary::Shutdown()
{ /* */ }
//---------------------------------------------------------------------------
NiShaderLibraryDesc* NiFXLShaderLibrary::GetShaderLibraryDesc()
{
    return NiD3DShaderLibrary::GetShaderLibraryDesc();
}
//---------------------------------------------------------------------------
void NiFXLShaderLibrary::SetShaderLibraryDesc(
    NiShaderLibraryDesc* pkDesc)
{
    NiD3DShaderLibrary::SetShaderLibraryDesc(pkDesc);
}
//---------------------------------------------------------------------------
void NiFXLShaderLibrary::SetRenderer(NiD3DRenderer* pkRenderer)
{
    if (m_pkD3DRenderer != pkRenderer)
    {
        NiD3DShaderLibrary::SetRenderer(pkRenderer);
    }
}
//---------------------------------------------------------------------------
NiShader* NiFXLShaderLibrary::GetShader(NiRenderer* pkRenderer, 
    const char* pcName, unsigned int uiImplementation)
{
    NIASSERT(NiIsKindOf(NiD3DRenderer, pkRenderer));
    SetRenderer((NiD3DRenderer*)pkRenderer);

    NiFXLFile* pkFile = NULL;
    if (!m_kFXLTechniqueMap.GetAt(pcName, pkFile) || pkFile == NULL)
        return NULL;

    return (NiShader*)pkFile->GetD3DShader((NiD3DRenderer*)pkRenderer, pcName, 
        uiImplementation);
    return NULL;
}
//---------------------------------------------------------------------------
bool NiFXLShaderLibrary::ReleaseShader(const char* pcName, 
    unsigned int uiImplementation)
{
    NiFXLFile* pkFile = NULL;
    if (!m_kFXLTechniqueMap.GetAt(pcName, pkFile) || pkFile == NULL)
        return false;

    return pkFile->ReleaseShader(pcName, uiImplementation);
}
//---------------------------------------------------------------------------
bool NiFXLShaderLibrary::ReleaseShader(NiShader* pkShader)
{
    if (pkShader == NULL)
        return false;
    return ReleaseShader(pkShader->GetName(), pkShader->GetImplementation());
}
//---------------------------------------------------------------------------
bool NiFXLShaderLibrary::InsertFXLFileIntoList(
    NiFXLFile* pkFXLFile)
{
    if (!pkFXLFile)
        return false;

    NiFXLFile* pkCheckFile = GetFXLFile(
        pkFXLFile->GetName());
    if (pkCheckFile)
    {
        NiD3DUtility::Log("File %s already in list??\n", 
            pkFXLFile->GetName());
        return false;
    }

    m_kFXLFileList.AddTail(pkFXLFile);

    // Add it to the description
    if (m_spShaderLibraryDesc == 0)
        m_spShaderLibraryDesc = NiNew NiShaderLibraryDesc();
    NIASSERT(m_spShaderLibraryDesc);

    NiShaderDesc* pkDesc = pkFXLFile->GetFirstShaderDesc();
    while (pkDesc)
    {
        m_spShaderLibraryDesc->AddShaderDesc(pkDesc);

        m_spShaderLibraryDesc->AddPixelShaderVersion(
            pkDesc->GetMinPixelShaderVersion());
        m_spShaderLibraryDesc->AddPixelShaderVersion(
            pkDesc->GetMaxPixelShaderVersion());
        m_spShaderLibraryDesc->AddVertexShaderVersion(
            pkDesc->GetMinVertexShaderVersion());
        m_spShaderLibraryDesc->AddVertexShaderVersion(
            pkDesc->GetMaxVertexShaderVersion());
        m_spShaderLibraryDesc->AddUserDefinedVersion(
            pkDesc->GetMinUserDefinedVersion());
        m_spShaderLibraryDesc->AddUserDefinedVersion(
            pkDesc->GetMaxUserDefinedVersion());
        m_spShaderLibraryDesc->AddPlatformFlags(
            pkDesc->GetPlatformFlags());
        
        pkDesc = pkFXLFile->GetNextShaderDesc();
    };

    NiFXLTechnique* pkTech = pkFXLFile->GetFirstTechnique();
    while (pkTech)
    {
        m_kFXLTechniqueMap.SetAt(pkTech->GetName(), pkFXLFile);
        pkTech = pkFXLFile->GetNextTechnique();
    }

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiFXLShaderLibrary::GetFXLFileCount()
{
    return m_kFXLFileList.GetSize();
}
//---------------------------------------------------------------------------
NiFXLFile* NiFXLShaderLibrary::GetFXLFile(
    const char* pcName)
{
    NiTListIterator kIter = m_kFXLFileList.GetHeadPos();
    while (kIter)
    {
        NiFXLFile* pkFile = m_kFXLFileList.GetNext(kIter);
        if (pkFile->GetName() == pcName)
            return pkFile;
    }
    return NULL;
}
//---------------------------------------------------------------------------
bool NiFXLShaderLibrary::LoadFXLFiles(const char* pcDirectory, 
    bool bRecurseSubFolders)
{
    bool bResult = true;

    NiFXLLoader* pkLoader = NiFXLLoader::Create();
    NIASSERT(pkLoader);

    bResult = pkLoader->LoadAllFXLFiles(this, pcDirectory, 
        bRecurseSubFolders);

    return bResult;
}
//---------------------------------------------------------------------------

