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
#include "NiBinaryShaderLibPCH.h"

#include "NSBPass.h"
#include "NSBStateGroup.h"
#include "NSBTexture.h"
#include "NSBTextureStage.h"
#include "NSBUtility.h"
#include "NSBShader.h"

#include <NiD3DUtility.h>
#include <NiD3DPass.h>
#include <NiD3DRendererHeaders.h>

//---------------------------------------------------------------------------
NSBPass::NSBPass() :
    m_pcName(0), 
    m_pkRenderStateGroup(0),
    m_pcVertexShaderProgramFileDX9(0),
    m_pcVertexShaderProgramFileXenon(0),
    m_pcVertexShaderProgramFilePS3(0),
    m_pcVertexShaderProgramFileD3D10(0),
    m_pcVSProgramEntryPoint(0),
    m_pcVSProgramTarget(0),
    m_pcGeometryShaderProgramFileDX9(0),
    m_pcGeometryShaderProgramFileXenon(0),
    m_pcGeometryShaderProgramFilePS3(0),
    m_pcGeometryShaderProgramFileD3D10(0),
    m_pcGSProgramEntryPoint(0),
    m_pcGSProgramTarget(0),
    m_pcPixelShaderProgramFileDX9(0), 
    m_pcPixelShaderProgramFileXenon(0), 
    m_pcPixelShaderProgramFilePS3(0), 
    m_pcPixelShaderProgramFileD3D10(0), 
    m_pcPSProgramEntryPoint(0),
    m_pcPSProgramTarget(0),
    m_bSoftwareVP(false)
{
    m_kVertexShaderConstantMaps.RemoveAll();
    m_kGeometryShaderConstantMaps.RemoveAll();
    m_kPixelShaderConstantMaps.RemoveAll();

    m_akStages.RemoveAll();
    m_akTextures.RemoveAll();
}
//---------------------------------------------------------------------------
NSBPass::~NSBPass()
{
    NiFree(m_pcName);
    NiDelete m_pkRenderStateGroup;
    NiFree(m_pcVertexShaderProgramFileDX9);
    NiFree(m_pcVertexShaderProgramFileXenon);
    NiFree(m_pcVertexShaderProgramFilePS3);
    NiFree(m_pcVertexShaderProgramFileD3D10);
    NiFree(m_pcVSProgramEntryPoint);
    NiFree(m_pcVSProgramTarget);
    NiFree(m_pcGeometryShaderProgramFileDX9);
    NiFree(m_pcGeometryShaderProgramFileXenon);
    NiFree(m_pcGeometryShaderProgramFilePS3);
    NiFree(m_pcGeometryShaderProgramFileD3D10);
    NiFree(m_pcGSProgramEntryPoint);
    NiFree(m_pcGSProgramTarget);
    NiFree(m_pcPixelShaderProgramFileDX9);
    NiFree(m_pcPixelShaderProgramFileXenon);
    NiFree(m_pcPixelShaderProgramFilePS3);
    NiFree(m_pcPixelShaderProgramFileD3D10);
    NiFree(m_pcPSProgramEntryPoint);
    NiFree(m_pcPSProgramTarget);

    // Clean up the shader constant maps.

    unsigned int uiSize = m_kVertexShaderConstantMaps.GetSize();
    for (unsigned int i = 0; i < uiSize; i++)
    {
        NSBConstantMap* pkCMap = m_kVertexShaderConstantMaps.GetAt(i);
        if (pkCMap)
        {
            NiDelete pkCMap;
            m_kVertexShaderConstantMaps.ReplaceAt(i, NULL);
        }
    }
    m_kVertexShaderConstantMaps.RemoveAll();
    
    uiSize = m_kGeometryShaderConstantMaps.GetSize();
    for (unsigned int i = 0; i < uiSize; i++)
    {
        NSBConstantMap* pkCMap = m_kGeometryShaderConstantMaps.GetAt(i);
        if (pkCMap)
        {
            NiDelete pkCMap;
            m_kGeometryShaderConstantMaps.ReplaceAt(i, NULL);
        }
    }
    m_kGeometryShaderConstantMaps.RemoveAll();

    uiSize = m_kPixelShaderConstantMaps.GetSize();
    for (unsigned int i = 0; i < uiSize; i++)
    {
        NSBConstantMap* pkCMap = m_kPixelShaderConstantMaps.GetAt(i);
        if (pkCMap)
        {
            NiDelete pkCMap;
            m_kPixelShaderConstantMaps.ReplaceAt(i, NULL);
        }
    }
    m_kPixelShaderConstantMaps.RemoveAll();

    // Clean up the stages.
    uiSize = m_akStages.GetSize();
    for (unsigned int i = 0; i < uiSize; i++)
    {
        NSBTextureStage* pkStage = m_akStages.GetAt(i);
        if (pkStage)
        {
            NiDelete pkStage;
            m_akStages.SetAt(i, 0);
        }
    }

    m_akStages.RemoveAll();

    // Clean up the textures.
    uiSize = m_akTextures.GetSize();
    for (unsigned int i = 0; i < uiSize; i++)
    {
        NSBTexture* pkTexture = m_akTextures.GetAt(i);
        if (pkTexture)
        {
            NiDelete pkTexture;
            m_akTextures.SetAt(i, 0);
        }
    }

    m_akTextures.RemoveAll();
}
//---------------------------------------------------------------------------
void NSBPass::SetName(const char* pcName)
{
    NiD3DUtility::SetString(m_pcName, 0, pcName);
}
//---------------------------------------------------------------------------
NSBStateGroup* NSBPass::GetRenderStateGroup()
{
    if (m_pkRenderStateGroup == 0)
        m_pkRenderStateGroup = NiNew NSBStateGroup();

    return m_pkRenderStateGroup;
}
//---------------------------------------------------------------------------
void NSBPass::SetVertexShaderProgramFileDX9(const char* pcPrgFileName)
{
    NiD3DUtility::SetString(m_pcVertexShaderProgramFileDX9, 0,
        pcPrgFileName);
}
//---------------------------------------------------------------------------
void NSBPass::SetVertexShaderProgramFileXenon(const char* pcPrgFileName)
{
    NiD3DUtility::SetString(m_pcVertexShaderProgramFileXenon, 0, 
        pcPrgFileName);
}
//---------------------------------------------------------------------------
void NSBPass::SetVertexShaderProgramFilePS3(const char* pcPrgFileName)
{
    NiD3DUtility::SetString(m_pcVertexShaderProgramFilePS3, 0, 
        pcPrgFileName);
}
//---------------------------------------------------------------------------
void NSBPass::SetVertexShaderProgramFileD3D10(const char* pcPrgFileName)
{
    NiD3DUtility::SetString(m_pcVertexShaderProgramFileD3D10, 0, 
        pcPrgFileName);
}
//---------------------------------------------------------------------------
void NSBPass::SetVertexShaderProgramEntryPoint(const char* pcEntryPoint)
{
    NiD3DUtility::SetString(m_pcVSProgramEntryPoint, 0, pcEntryPoint);
}
//---------------------------------------------------------------------------
void NSBPass::SetVertexShaderProgramShaderTarget(
    const char* pcShaderTarget)
{
    NiD3DUtility::SetString(m_pcVSProgramTarget, 0, pcShaderTarget);
}
//---------------------------------------------------------------------------
void NSBPass::SetGeometryShaderProgramFileDX9(
    const char* pcPrgFileName)
{
    NiD3DUtility::SetString(m_pcGeometryShaderProgramFileDX9, 0, 
        pcPrgFileName);
}
//---------------------------------------------------------------------------
void NSBPass::SetGeometryShaderProgramFileXenon(
    const char* pcPrgFileName)
{
    NiD3DUtility::SetString(m_pcGeometryShaderProgramFileXenon, 0, 
        pcPrgFileName);
}
//---------------------------------------------------------------------------
void NSBPass::SetGeometryShaderProgramFilePS3(
    const char* pcPrgFileName)
{
    NiD3DUtility::SetString(m_pcGeometryShaderProgramFilePS3, 0, 
        pcPrgFileName);
}
//---------------------------------------------------------------------------
void NSBPass::SetGeometryShaderProgramFileD3D10(
    const char* pcPrgFileName)
{
    NiD3DUtility::SetString(m_pcGeometryShaderProgramFileD3D10, 0, 
        pcPrgFileName);
}
//---------------------------------------------------------------------------
void NSBPass::SetGeometryShaderProgramEntryPoint(
    const char* pcEntryPoint)
{
    NiD3DUtility::SetString(m_pcGSProgramEntryPoint, 0, pcEntryPoint);
}
//---------------------------------------------------------------------------
void NSBPass::SetGeometryShaderProgramShaderTarget(
    const char* pcShaderTarget)
{
    NiD3DUtility::SetString(m_pcGSProgramTarget, 0, pcShaderTarget);
}
//---------------------------------------------------------------------------
void NSBPass::SetPixelShaderProgramFileDX9(const char* pcPrgFileName)
{
    NiD3DUtility::SetString(m_pcPixelShaderProgramFileDX9, 0,
        pcPrgFileName);
}
//---------------------------------------------------------------------------
void NSBPass::SetPixelShaderProgramFileXenon(const char* pcPrgFileName)
{
    NiD3DUtility::SetString(m_pcPixelShaderProgramFileXenon, 0,
        pcPrgFileName);
}
//---------------------------------------------------------------------------
void NSBPass::SetPixelShaderProgramFilePS3(const char* pcPrgFileName)
{
    NiD3DUtility::SetString(m_pcPixelShaderProgramFilePS3, 0,
        pcPrgFileName);
}
//---------------------------------------------------------------------------
void NSBPass::SetPixelShaderProgramFileD3D10(const char* pcPrgFileName)
{
    NiD3DUtility::SetString(m_pcPixelShaderProgramFileD3D10, 0,
        pcPrgFileName);
}
//---------------------------------------------------------------------------
void NSBPass::SetPixelShaderProgramEntryPoint(const char* pcEntryPoint)
{
    NiD3DUtility::SetString(m_pcPSProgramEntryPoint, 0, pcEntryPoint);
}
//---------------------------------------------------------------------------
void NSBPass::SetPixelShaderProgramShaderTarget(const char* pcShaderTarget)
{
    NiD3DUtility::SetString(m_pcPSProgramTarget, 0, pcShaderTarget);
}
//---------------------------------------------------------------------------
NSBConstantMap* NSBPass::GetVertexConstantMap(unsigned int uiIndex)
{
    NIASSERT(uiIndex <= m_kVertexShaderConstantMaps.GetSize());

    if (uiIndex == m_kVertexShaderConstantMaps.GetSize())
    {
        NSBConstantMap* pkVertexConstantMap = NiNew NSBConstantMap();
        NIASSERT(pkVertexConstantMap);
        pkVertexConstantMap->SetProgramType(NiGPUProgram::PROGRAM_VERTEX);
        m_kVertexShaderConstantMaps.Add(pkVertexConstantMap);
    }
    else if (uiIndex < m_kVertexShaderConstantMaps.GetSize())
    {
        NSBConstantMap* pkVertexConstantMap
            = m_kVertexShaderConstantMaps.GetAt(uiIndex);
        if (pkVertexConstantMap == NULL)
        {
            pkVertexConstantMap = NiNew NSBConstantMap();
            NIASSERT(pkVertexConstantMap);
            pkVertexConstantMap->SetProgramType(NiGPUProgram::PROGRAM_VERTEX);
            m_kVertexShaderConstantMaps.ReplaceAt(uiIndex,
                pkVertexConstantMap);
        }
    }

    return m_kVertexShaderConstantMaps.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
NSBConstantMap* NSBPass::GetGeometryConstantMap(
    unsigned int uiIndex)
{
    NIASSERT(uiIndex <= m_kGeometryShaderConstantMaps.GetSize());

    if (uiIndex == m_kGeometryShaderConstantMaps.GetSize())
    {
        NSBConstantMap* pkGeometryConstantMap = NiNew NSBConstantMap();
        NIASSERT(pkGeometryConstantMap);
        pkGeometryConstantMap->SetProgramType(NiGPUProgram::PROGRAM_GEOMETRY);
        m_kGeometryShaderConstantMaps.Add(pkGeometryConstantMap);
    }
    else if (uiIndex < m_kGeometryShaderConstantMaps.GetSize())
    {
        NSBConstantMap* pkGeometryConstantMap
            = m_kGeometryShaderConstantMaps.GetAt(uiIndex);
        if (pkGeometryConstantMap == NULL)
        {
            pkGeometryConstantMap = NiNew NSBConstantMap();
            NIASSERT(pkGeometryConstantMap);
            pkGeometryConstantMap->SetProgramType(
                NiGPUProgram::PROGRAM_GEOMETRY);
            m_kGeometryShaderConstantMaps.ReplaceAt(uiIndex,
                pkGeometryConstantMap);
        }
    }

    return m_kGeometryShaderConstantMaps.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
NSBConstantMap* NSBPass::GetPixelConstantMap(unsigned int uiIndex)
{
    NIASSERT(uiIndex <= m_kPixelShaderConstantMaps.GetSize());

    if (uiIndex == m_kPixelShaderConstantMaps.GetSize())
    {
        NSBConstantMap* pkPixelConstantMap = NiNew NSBConstantMap();
        NIASSERT(pkPixelConstantMap);
        pkPixelConstantMap->SetProgramType(NiGPUProgram::PROGRAM_PIXEL);
        m_kPixelShaderConstantMaps.Add(pkPixelConstantMap);
    }
    else if (uiIndex < m_kPixelShaderConstantMaps.GetSize())
    {
        NSBConstantMap* pkPixelConstantMap
            = m_kPixelShaderConstantMaps.GetAt(uiIndex);
        if (pkPixelConstantMap == NULL)
        {
            pkPixelConstantMap = NiNew NSBConstantMap();
            NIASSERT(pkPixelConstantMap);
            pkPixelConstantMap->SetProgramType(NiGPUProgram::PROGRAM_PIXEL);
            m_kPixelShaderConstantMaps.ReplaceAt(uiIndex, pkPixelConstantMap);
        }
    }

    return m_kPixelShaderConstantMaps.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
unsigned int NSBPass::GetStageCount()
{
    return m_akStages.GetSize();
}
//---------------------------------------------------------------------------
NSBTextureStage* NSBPass::GetStage(unsigned int uiIndex, bool bCreate)
{
    NSBTextureStage* pkStage = 0;
    
    if (m_akStages.GetSize() > uiIndex)
        pkStage = m_akStages.GetAt(uiIndex);
    if (!pkStage)
    {
        if (bCreate)
        {
            // Create all stages up to NiNew one, to ensure 
            // stage array is packed
            m_akStages.SetSize(uiIndex + 1);
            for (unsigned int i = m_akStages.GetSize(); i <= uiIndex; i++)
            {
                pkStage = NiNew NSBTextureStage();
                NIASSERT(pkStage);

                pkStage->SetStage(i);

                m_akStages.SetAt(i, pkStage);
            }
        }
    }

    return pkStage;
}
//---------------------------------------------------------------------------
unsigned int NSBPass::GetTextureCount()
{
    return m_akTextures.GetSize();
}
//---------------------------------------------------------------------------
NSBTexture* NSBPass::GetTexture(unsigned int uiIndex, bool bCreate)
{
    NSBTexture* pkTexture = 0;
    
    if (m_akTextures.GetSize() > uiIndex)
        pkTexture = m_akTextures.GetAt(uiIndex);
    if (!pkTexture)
    {
        if (bCreate)
        {
            // Create all stages up to NiNew one, to ensure 
            // stage array is packed
            m_akTextures.SetSize(m_akTextures.GetSize() + 1);
            for (unsigned int i = m_akTextures.GetSize(); i <= uiIndex; i++)
            {
                pkTexture = NiNew NSBTexture();
                NIASSERT(pkTexture);

                pkTexture->SetStage(i);

                m_akTextures.SetAt(i, pkTexture);
            }
        }
    }

    return pkTexture;
}
//---------------------------------------------------------------------------
bool NSBPass::SetupNiD3DPass(NiD3DPass& kPass, NiShaderDesc* pkShaderDesc)
{
    // Convert the NSB representation to a run-time NiD3DPass

    // RenderStateGroup
    NiD3DRenderStateGroup* pkRSGroup = 0;
    if (m_pkRenderStateGroup)
    {
        if (m_pkRenderStateGroup->GetStateCount())
        {
            pkRSGroup = NiD3DRenderStateGroup::GetFreeRenderStateGroup();
            NIASSERT(pkRSGroup);

            if (!m_pkRenderStateGroup->SetupRenderStateGroup(*pkRSGroup))
            {
                NiDelete pkRSGroup;
                return false;
            }
        }
    }
    kPass.SetRenderStateGroup(pkRSGroup);

    // Setup the 'local' constant mappings

    // Vertex
    if (m_kVertexShaderConstantMaps.GetSize() > 0)
    {
        NSBConstantMap* pkCM = m_kVertexShaderConstantMaps.GetAt(0);
        if (pkCM)
        {
            kPass.SetVertexConstantMap(pkCM->GetVertexConstantMap(
                pkShaderDesc));
        }
        else
        {
            kPass.SetVertexConstantMap(NULL);
        }
    }
    else
    {
        kPass.SetVertexConstantMap(NULL);
    }

    // Pixel
    if (m_kPixelShaderConstantMaps.GetSize() > 0)
    {
        NSBConstantMap* pkCM = m_kPixelShaderConstantMaps.GetAt(0);
        if (pkCM)
        {
            kPass.SetPixelConstantMap(pkCM->GetPixelConstantMap(
                pkShaderDesc));
        }
        else
        {
            kPass.SetPixelConstantMap(NULL);
        }
    }
    else
    {
        kPass.SetPixelConstantMap(NULL);
    }

    // Shader Programs

#if defined(_XENON)
    kPass.SetVertexShaderProgramFileName(m_pcVertexShaderProgramFileXenon);
    kPass.SetPixelShaderProgramFileName(m_pcPixelShaderProgramFileXenon);
#elif defined(WIN32)
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);   // A renderer is assumed to exist for the caller.
    if (NiIsKindOf(NiDX9Renderer, pkRenderer))
    {
        kPass.SetVertexShaderProgramFileName(m_pcVertexShaderProgramFileDX9);
        kPass.SetPixelShaderProgramFileName(m_pcPixelShaderProgramFileDX9);
    }
    else
    {
        NIASSERT(false && "A valid renderer was not found.");
        return NULL;
    }
#endif

    kPass.SetVertexShaderProgramEntryPoint(m_pcVSProgramEntryPoint);
    kPass.SetVertexShaderProgramShaderTarget(m_pcVSProgramTarget);
    kPass.SetPixelShaderProgramEntryPoint(m_pcPSProgramEntryPoint);
    kPass.SetPixelShaderProgramShaderTarget(m_pcPSProgramTarget);

    kPass.SetSoftwareVertexProcessing(m_bSoftwareVP);

    NSBTextureStage* pkStage;
    unsigned int uiStageCount = m_akStages.GetSize();
    if (uiStageCount > NiD3DPass::ms_uiMaxSamplers)
        return false;

    for (unsigned int ui = 0; ui < uiStageCount; ui++)
    {
        pkStage = m_akStages.GetAt(ui);
        if (pkStage)
        {
            NiD3DTextureStagePtr spD3DStage = kPass.GetStage(ui);
            if (!spD3DStage)
            {
                spD3DStage = NiNew NiD3DTextureStage();
                NIASSERT(spD3DStage);
            }

            if (!pkStage->SetupD3DTextureStage(*spD3DStage))
            {
                return false;
            }
            
            kPass.SetStage(ui, spD3DStage);
        }
    }

    // Textures should only modify existing stages
    NSBTexture* pkTexture;
    unsigned int uiTextureCount = m_akTextures.GetSize();
    if (uiTextureCount > uiStageCount)
        uiTextureCount = uiStageCount;

    for (unsigned int ui = 0; ui < uiTextureCount; ui++)
    {
        pkTexture = m_akTextures.GetAt(ui);

        if (pkTexture)
        {
            NiD3DTextureStagePtr spD3DStage = kPass.GetStage(ui);
            if (!spD3DStage)
            {
                continue;
            }

            if (!pkTexture->SetupD3DTextureStage(*spD3DStage))
            {
                return false;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
const char* NSBPass::GetVertexShaderProgramFile() const
{

#if defined(WIN32)
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);   // A renderer is assumed to exist for the caller.
    if (NiIsKindOf(NiDX9Renderer, pkRenderer))
    {
        return m_pcVertexShaderProgramFileDX9;
    }
///    else if (NiIsKindOf(NiD3D10Renderer, pkRenderer))
///    {
///        return m_pcVertexShaderProgramFileD3D10;
///    }
    else
    {
        NIASSERT(false && "A valid renderer was not found.");
        return NULL;
    }
#elif defined(_XENON)
    return m_pcVertexShaderProgramFileXenon;
#elif defined(_PS3)
    return m_pcVertexShaderProgramFilePS3;
#endif  //#if defined(_XENON)

    return NULL;
}
//---------------------------------------------------------------------------
const char* NSBPass::GetGeometryShaderProgramFile() const
{

#if defined(WIN32)
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);   // A renderer is assumed to exist for the caller.
    if (NiIsKindOf(NiDX9Renderer, pkRenderer))
    {
        return m_pcGeometryShaderProgramFileDX9;
    }
///    else if (NiIsKindOf(NiD3D10Renderer, pkRenderer))
///    {
///        return m_pcGeometryShaderProgramFileD3D10;
///    }
    else
    {
        NIASSERT(false && "A valid renderer was not found.");
        return NULL;
    }
#elif defined(_XENON)
    return m_pcGeometryShaderProgramFileXenon;
#elif defined(_PS3)
    return m_pcGeometryShaderProgramFilePS3;
#endif  //#if defined(_XENON)

    return NULL;
}
//---------------------------------------------------------------------------
const char* NSBPass::GetPixelShaderProgramFile() const
{

#if defined(WIN32)
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);   // A renderer is assumed to exist for the caller.
    if (NiIsKindOf(NiDX9Renderer, pkRenderer))
    {
        return m_pcPixelShaderProgramFileDX9;
    }
///    else if (NiIsKindOf(NiD3D10Renderer, pkRenderer))
///    {
///        return m_pcPixelShaderProgramFileD3D10;
///    }
    else
    {
        NIASSERT(false && "A valid renderer was not found.");
        return NULL;
    }
#elif defined(_XENON)
    return m_pcPixelShaderProgramFileXenon;
#elif defined(_PS3)
    return m_pcPixelShaderProgramFilePS3;
#endif  //#if defined(_XENON)

    return NULL;
}
//---------------------------------------------------------------------------
bool NSBPass::SaveBinary(NiBinaryStream& kStream)
{
    if (!NiD3DUtility::WriteString(kStream, m_pcName))
        return false;

    // Version 1.6 added support for user-defined data...
    unsigned int uiUDDSetPresent = 0;
    if (m_spUserDefinedDataSet)
    {
        uiUDDSetPresent = 1;
        NiStreamSaveBinary(kStream, uiUDDSetPresent);
        if (!m_spUserDefinedDataSet->SaveBinary(kStream))
            return false;
    }
    else
    {
        NiStreamSaveBinary(kStream, uiUDDSetPresent);
    }

    if (!NSBUtility::SaveBinaryStateGroup(kStream, m_pkRenderStateGroup))
        return false;
    if (!SaveBinaryPixelShaderProgram(kStream))
        return false;
    unsigned int uiConstantMapCount = m_kPixelShaderConstantMaps.GetSize();
    NiStreamSaveBinary(kStream, uiConstantMapCount);
    for (unsigned int i=0; i < uiConstantMapCount; i++)
    {
        if (!NSBUtility::SaveBinaryConstantMap(kStream,
            m_kPixelShaderConstantMaps.GetAt(i)))
        {
            return false;
        }
    }

    if (!SaveBinaryVertexShaderProgram(kStream))
        return false;
    uiConstantMapCount = m_kVertexShaderConstantMaps.GetSize();
    NiStreamSaveBinary(kStream, uiConstantMapCount);
    for (unsigned int i=0; i < uiConstantMapCount; i++)
    {
        if (!NSBUtility::SaveBinaryConstantMap(kStream,
            m_kVertexShaderConstantMaps.GetAt(i)))
        {
            return false;
        }
    }

    if (!SaveBinaryGeometryShaderProgram(kStream))
        return false;
    uiConstantMapCount = m_kGeometryShaderConstantMaps.GetSize();
    NiStreamSaveBinary(kStream, uiConstantMapCount);
    for (unsigned int i=0; i < uiConstantMapCount; i++)
    {
        if (!NSBUtility::SaveBinaryConstantMap(kStream,
            m_kGeometryShaderConstantMaps.GetAt(i)))
        {
            return false;
        }
    }

    unsigned int uiValue = m_bSoftwareVP ? 1 : 0;
    NiStreamSaveBinary(kStream, uiValue);

    if (!SaveBinaryStages(kStream))
        return false;

    if (!SaveBinaryTextures(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NSBPass::LoadBinary(NiBinaryStream& kStream)
{
    unsigned int uiStringSize = 0;
    if (!NiD3DUtility::AllocateAndReadString(kStream, m_pcName, 
        uiStringSize))
    {
        return false;
    }

    // Version 1.6 added support for user-defined data...
    if (NSBShader::GetReadVersion() >= 0x00010006)
    {
        unsigned int uiUDDSetPresent;
        NiStreamLoadBinary(kStream, uiUDDSetPresent);
        if (uiUDDSetPresent)
        {
            m_spUserDefinedDataSet = NiNew NSBUserDefinedDataSet();
            NIASSERT(m_spUserDefinedDataSet);
            if (!m_spUserDefinedDataSet->LoadBinary(kStream))
                return false;
        }
        else
        {
            m_spUserDefinedDataSet = 0;
        }
    }

    NiDelete m_pkRenderStateGroup;
    m_pkRenderStateGroup = 0;
    if (!NSBUtility::LoadBinaryStateGroup(kStream, m_pkRenderStateGroup))
        return false;


    if (NSBShader::GetReadVersion() < 0x00010013)
    {
        // Before 1.13, there was exactly one constant map for vertex and
        // pixel shaders.
        if (!LoadBinaryPixelShaderProgram(kStream))
            return false;
        m_kPixelShaderConstantMaps.RemoveAll();
        NSBConstantMap* pkPixelConstantMap = NULL;
        if (!NSBUtility::LoadBinaryConstantMap(kStream,
            pkPixelConstantMap))
        {
            return false;
        }
        m_kPixelShaderConstantMaps.Add(pkPixelConstantMap);

        if (!LoadBinaryVertexShaderProgram(kStream))
            return false;
        m_kVertexShaderConstantMaps.RemoveAll();
        NSBConstantMap* pkVertexConstantMap = NULL;
        if (!NSBUtility::LoadBinaryConstantMap(kStream,
            pkVertexConstantMap))
        {
            return false;
        }
        m_kVertexShaderConstantMaps.Add(pkVertexConstantMap);
    }
    else
    {
        // Version 1.13 added support for multiple constant maps, D3D10, and
        // geometry shaders.

        if (!LoadBinaryPixelShaderProgram(kStream))
            return false;
        m_kPixelShaderConstantMaps.RemoveAll();
        unsigned int uiConstantMapCount = 0;
        NiStreamLoadBinary(kStream, uiConstantMapCount);
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            NSBConstantMap* pkPixelConstantMap = NULL;
            if (!NSBUtility::LoadBinaryConstantMap(kStream,
                pkPixelConstantMap))
            {
                return false;
            }
            m_kPixelShaderConstantMaps.Add(pkPixelConstantMap);
        }
        
        if (!LoadBinaryVertexShaderProgram(kStream))
            return false;
        m_kVertexShaderConstantMaps.RemoveAll();
        uiConstantMapCount = 0;
        NiStreamLoadBinary(kStream, uiConstantMapCount);
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            NSBConstantMap* pkVertexConstantMap = NULL;
            if (!NSBUtility::LoadBinaryConstantMap(kStream,
                pkVertexConstantMap))
            {
                return false;
            }
            m_kVertexShaderConstantMaps.Add(pkVertexConstantMap);
        }
        
        if (!LoadBinaryGeometryShaderProgram(kStream))
            return false;
        m_kGeometryShaderConstantMaps.RemoveAll();
        uiConstantMapCount = 0;
        NiStreamLoadBinary(kStream, uiConstantMapCount);
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            NSBConstantMap* pkGeometryConstantMap = NULL;
            if (!NSBUtility::LoadBinaryConstantMap(kStream,
                pkGeometryConstantMap))
            {
                return false;
            }
            m_kGeometryShaderConstantMaps.Add(pkGeometryConstantMap);
        }
    }

    unsigned int uiSoftwareVP = 0;
    NiStreamLoadBinary(kStream, uiSoftwareVP);
    m_bSoftwareVP = (uiSoftwareVP != 0);

    if (!LoadBinaryStages(kStream))
        return false;

    if (NSBShader::GetReadVersion() >= 0x00010014)
    {
        // Version 1.14 added separate NSBTexture objects.

        if (!LoadBinaryTextures(kStream))
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBPass::SaveBinaryVertexShaderProgram(NiBinaryStream& kStream)
{
    if (!NiD3DUtility::WriteString(kStream,
        m_pcVertexShaderProgramFileDX9))
    {
        return false;
    }
    if (!NiD3DUtility::WriteString(kStream,
        m_pcVertexShaderProgramFileXenon))
    {
        return false;
    }
    if (!NiD3DUtility::WriteString(kStream,
        m_pcVertexShaderProgramFilePS3))
    {
        return false;
    }
    if (!NiD3DUtility::WriteString(kStream,
        m_pcVertexShaderProgramFileD3D10))
    {
        return false;
    }
    if (!NiD3DUtility::WriteString(kStream, m_pcVSProgramEntryPoint))
        return false;
    if (!NiD3DUtility::WriteString(kStream, m_pcVSProgramTarget))
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NSBPass::SaveBinaryGeometryShaderProgram(NiBinaryStream& kStream)
{
    if (!NiD3DUtility::WriteString(kStream,
        m_pcGeometryShaderProgramFileDX9))
    {
        return false;
    }
    if (!NiD3DUtility::WriteString(kStream,
        m_pcGeometryShaderProgramFileXenon))
    {
        return false;
    }
    if (!NiD3DUtility::WriteString(kStream,
        m_pcGeometryShaderProgramFilePS3))
    {
        return false;
    }
    if (!NiD3DUtility::WriteString(kStream,
        m_pcGeometryShaderProgramFileD3D10))
    {
        return false;
    }
    if (!NiD3DUtility::WriteString(kStream, m_pcGSProgramEntryPoint))
        return false;
    if (!NiD3DUtility::WriteString(kStream, m_pcGSProgramTarget))
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NSBPass::SaveBinaryPixelShaderProgram(NiBinaryStream& kStream)
{
    if (!NiD3DUtility::WriteString(kStream,
        m_pcPixelShaderProgramFileDX9))
    {
        return false;
    }
    if (!NiD3DUtility::WriteString(kStream,
        m_pcPixelShaderProgramFileXenon))
    {
        return false;
    }
    if (!NiD3DUtility::WriteString(kStream,
        m_pcPixelShaderProgramFilePS3))
    {
        return false;
    }
    if (!NiD3DUtility::WriteString(kStream,
        m_pcPixelShaderProgramFileD3D10))
    {
        return false;
    }
    if (!NiD3DUtility::WriteString(kStream, m_pcPSProgramEntryPoint))
        return false;
    if (!NiD3DUtility::WriteString(kStream, m_pcPSProgramTarget))
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NSBPass::SaveBinaryStages(NiBinaryStream& kStream)
{
    unsigned int uiCount = m_akStages.GetEffectiveSize();

    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;
    NSBTextureStage* pkStage;
    for (unsigned int ui = 0; ui < m_akStages.GetSize(); ui++)
    {
        pkStage = m_akStages.GetAt(ui);
        if (pkStage)
        {
            if (!pkStage->SaveBinary(kStream))
                return false;
            uiTestCount++;
        }
    }

    if (uiTestCount != uiCount)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NSBPass::SaveBinaryTextures(NiBinaryStream& kStream)
{
    unsigned int uiCount = m_akTextures.GetEffectiveSize();

    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;
    NSBTexture* pkTexture;
    for (unsigned int ui = 0; ui < m_akTextures.GetSize(); ui++)
    {
        pkTexture = m_akTextures.GetAt(ui);
        if (pkTexture)
        {
            if (!pkTexture->SaveBinary(kStream))
                return false;
            uiTestCount++;
        }
    }

    if (uiTestCount != uiCount)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NSBPass::LoadBinaryVertexShaderProgram(NiBinaryStream& kStream)
{
    unsigned int uiStringSize = 0;
    if (NSBShader::GetReadVersion() == 0x00010001)
    {
        // Version 1.1 only had a single shader file name.  Load first into
        // PS3 name (arbitrary choice - doesn't matter) and then copy into
        // other platform's names.
        if (!NiD3DUtility::AllocateAndReadString(kStream, 
            m_pcVertexShaderProgramFilePS3, uiStringSize))
        {
            return false;
        }
        NiD3DUtility::SetString(m_pcVertexShaderProgramFileDX9, 0,
            m_pcVertexShaderProgramFilePS3);
        NiD3DUtility::SetString(m_pcVertexShaderProgramFileXenon, 0,
            m_pcVertexShaderProgramFilePS3);
        NiD3DUtility::SetString(m_pcVertexShaderProgramFileD3D10, 0,
            m_pcVertexShaderProgramFilePS3);
    }
    else
    {
        if (NSBShader::GetReadVersion() < 0x00010008)
        {
            char* pcTemp = 0;
            // Removed Xbox support in version 1.8
            if (!NiD3DUtility::AllocateAndReadString(kStream, 
                pcTemp, uiStringSize))
            {
                return false;
            }
            NiFree(pcTemp);

            // Removed DX8 support in version 1.8
            pcTemp = 0;
            if (!NiD3DUtility::AllocateAndReadString(kStream, 
                pcTemp, uiStringSize))
            {
                return false;
            }
            NiFree(pcTemp);
        }
        if (!NiD3DUtility::AllocateAndReadString(kStream, 
            m_pcVertexShaderProgramFileDX9, uiStringSize))
        {
            return false;
        }
        if (NSBShader::GetReadVersion() >= 0x00010008)
        {
            // Added Xenon support in version 1.8
            if (!NiD3DUtility::AllocateAndReadString(kStream, 
                m_pcVertexShaderProgramFileXenon, uiStringSize))
            {
                return false;
            }
        }
        if (NSBShader::GetReadVersion() >= 0x00010012)
        {
            // Added PS3 support in version 1.12
            if (!NiD3DUtility::AllocateAndReadString(kStream, 
                m_pcVertexShaderProgramFilePS3, uiStringSize))
            {
                return false;
            }
        }
        if (NSBShader::GetReadVersion() >= 0x00010013)
        {
            // Added D3D10 and geometry shader support in version 1.13.
            if (!NiD3DUtility::AllocateAndReadString(kStream, 
                m_pcVertexShaderProgramFileD3D10, uiStringSize))
            {
                return false;
            }
        }
    }

    if (!NiD3DUtility::AllocateAndReadString(kStream, 
        m_pcVSProgramEntryPoint, uiStringSize))
    {
        return false;
    }

    if (!NiD3DUtility::AllocateAndReadString(kStream, m_pcVSProgramTarget, 
        uiStringSize))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBPass::LoadBinaryGeometryShaderProgram(NiBinaryStream& kStream)
{
    unsigned int uiStringSize = 0;

    if (NSBShader::GetReadVersion() < 0x00010013)
    {
        // Geometry shaders did not exist before version 1.13.  Initialize all
        // strings to NULL.
        m_pcGeometryShaderProgramFileDX9 = NULL;
        m_pcGeometryShaderProgramFileXenon = NULL;
        m_pcGeometryShaderProgramFilePS3 = NULL;
        m_pcGeometryShaderProgramFileD3D10 = NULL;
        m_pcGSProgramEntryPoint = NULL;
        m_pcGSProgramTarget = NULL;
    }
    else
    {
        // Added geometry shader support for DX9, Xenon, PS3, and D3D10 in
        // version 1.13.
        if (!NiD3DUtility::AllocateAndReadString(kStream, 
            m_pcGeometryShaderProgramFileDX9, uiStringSize))
        {
            return false;
        }
        if (!NiD3DUtility::AllocateAndReadString(kStream, 
            m_pcGeometryShaderProgramFileXenon, uiStringSize))
        {
            return false;
        }
        if (!NiD3DUtility::AllocateAndReadString(kStream, 
            m_pcGeometryShaderProgramFilePS3, uiStringSize))
        {
            return false;
        }
        if (!NiD3DUtility::AllocateAndReadString(kStream, 
            m_pcGeometryShaderProgramFileD3D10, uiStringSize))
        {
            return false;
        }
        if (!NiD3DUtility::AllocateAndReadString(kStream, 
            m_pcGSProgramEntryPoint, uiStringSize))
        {
            return false;
        }
        if (!NiD3DUtility::AllocateAndReadString(kStream,
            m_pcGSProgramTarget, uiStringSize))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBPass::LoadBinaryPixelShaderProgram(NiBinaryStream& kStream)
{
    unsigned int uiStringSize = 0;
    if (NSBShader::GetReadVersion() == 0x00010001)
    {
        // Version 1.1 only had a single shader file name
        if (!NiD3DUtility::AllocateAndReadString(kStream, 
            m_pcPixelShaderProgramFilePS3, uiStringSize))
        {
            return false;
        }

        NiD3DUtility::SetString(m_pcPixelShaderProgramFileDX9, 0,
            m_pcPixelShaderProgramFilePS3);
        NiD3DUtility::SetString(m_pcPixelShaderProgramFileXenon, 0,
            m_pcPixelShaderProgramFilePS3);
        NiD3DUtility::SetString(m_pcPixelShaderProgramFileD3D10, 0,
            m_pcPixelShaderProgramFilePS3);
    }
    else
    {
        if (NSBShader::GetReadVersion() < 0x00010008)
        {
            char* pcTemp = 0;
            // Removed Xbox support in version 1.8
            if (!NiD3DUtility::AllocateAndReadString(kStream, 
                pcTemp, uiStringSize))
            {
                return false;
            }
            NiFree(pcTemp);

            // Removed DX8 support in version 1.8
            pcTemp = 0;
            if (!NiD3DUtility::AllocateAndReadString(kStream, 
                pcTemp, uiStringSize))
            {
                return false;
            }
            NiFree(pcTemp);
        }

        if (!NiD3DUtility::AllocateAndReadString(kStream, 
            m_pcPixelShaderProgramFileDX9, uiStringSize))
        {
            return false;
        }

        if (NSBShader::GetReadVersion() >= 0x00010008)
        {
            // Added Xenon support in version 1.8
            if (!NiD3DUtility::AllocateAndReadString(kStream, 
                m_pcPixelShaderProgramFileXenon, uiStringSize))
            {
                return false;
            }
        }
        if (NSBShader::GetReadVersion() >= 0x00010012)
        {
            // Added PS3 support in version 1.12
            if (!NiD3DUtility::AllocateAndReadString(kStream, 
                m_pcPixelShaderProgramFilePS3, uiStringSize))
            {
                return false;
            }
        }
        if (NSBShader::GetReadVersion() >= 0x00010013)
        {
            // Added D3D10 and geometry shader support in version 1.13.
            if (!NiD3DUtility::AllocateAndReadString(kStream, 
                m_pcPixelShaderProgramFileD3D10, uiStringSize))
            {
                return false;
            }
        }
    }

    // Version 1.1 did not have entry points or targets.
    if (NSBShader::GetReadVersion() > 0x00010001)
    {
        if (!NiD3DUtility::AllocateAndReadString(kStream, 
            m_pcPSProgramEntryPoint, uiStringSize))
        {
            return false;
        }
        if (!NiD3DUtility::AllocateAndReadString(kStream, 
            m_pcPSProgramTarget, uiStringSize))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBPass::LoadBinaryStages(NiBinaryStream& kStream)
{
    unsigned int uiCount;

    NiStreamLoadBinary(kStream, uiCount);

    m_akStages.SetSize(uiCount);

    NSBTextureStage* pkStage;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkStage = NiNew NSBTextureStage();
        NIASSERT(pkStage);

        if (!pkStage->LoadBinary(kStream))
            return false;

        NIASSERT(ui == pkStage->GetStage());
        m_akStages.SetAt(ui, pkStage);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBPass::LoadBinaryTextures(NiBinaryStream& kStream)
{
    unsigned int uiCount;

    NiStreamLoadBinary(kStream, uiCount);

    m_akTextures.SetSize(uiCount);

    NSBTexture* pkTexture;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkTexture = NiNew NSBTexture();
        NIASSERT(pkTexture);

        if (!pkTexture->LoadBinary(kStream))
            return false;

        NIASSERT(ui == pkTexture->GetStage());
        m_akTextures.SetAt(ui, pkTexture);
    }

    return true;
}
//---------------------------------------------------------------------------
#if defined(_DEBUG)
//---------------------------------------------------------------------------
void NSBPass::Dump(FILE* pf)
{
    NSBUtility::Dump(pf, true, "Pass %s\n", m_pcName);
    NSBUtility::IndentInsert();

    if (m_pkRenderStateGroup)
    {
        NSBUtility::Dump(pf, true, "RenderStateGroup\n");
        NSBUtility::IndentInsert();
        m_pkRenderStateGroup->Dump(pf, NSBStateGroup::DUMP_RENDERSTATES);
        NSBUtility::IndentRemove();
    }
    else
    {
        NSBUtility::Dump(pf, true, "***   NO RS GROUP   ***\n");
    }

    NSBUtility::Dump(pf, true, "\n");
    NSBUtility::Dump(pf, true, "VertexShader Program File (DX9) : %s\n", 
        m_pcVertexShaderProgramFileDX9);
    NSBUtility::Dump(pf, true, "VertexShader Program File (Xenon) : %s\n", 
        m_pcVertexShaderProgramFileXenon);
    NSBUtility::Dump(pf, true, "VertexShader Program File (PS3) : %s\n", 
        m_pcVertexShaderProgramFilePS3);
    NSBUtility::Dump(pf, true, "VertexShader Program File (D3D10) : %s\n", 
        m_pcVertexShaderProgramFileD3D10);

    NSBUtility::Dump(pf, true, "\n");
    NSBUtility::Dump(pf, true, "VertexShader Program Entry Point: %s\n", 
        m_pcVSProgramEntryPoint);

    NSBUtility::Dump(pf, true, "\n");
    NSBUtility::Dump(pf, true, "VertexShader Program Shader Target: %s\n", 
        m_pcVSProgramTarget);

    unsigned int uiConstantMapCount = m_kVertexShaderConstantMaps.GetSize();
    if (uiConstantMapCount > 0)
    {
        NSBUtility::Dump(pf, true, "VertexConstantMaps\n");
        NSBUtility::IndentInsert();
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            m_kVertexShaderConstantMaps.GetAt(i)->Dump(pf);
        }
        NSBUtility::IndentRemove();
    }
    else
    {
        NSBUtility::Dump(pf, true, "*** NO VERTEX CMAPS  ***\n");
    }

    NSBUtility::Dump(pf, true, "\n");
    NSBUtility::Dump(pf, true, "GeometryShader Program File (DX9) : %s\n", 
        m_pcGeometryShaderProgramFileDX9);
    NSBUtility::Dump(pf, true, "GeometryShader Program File (Xenon) : %s\n", 
        m_pcGeometryShaderProgramFileXenon);
    NSBUtility::Dump(pf, true, "GeometryShader Program File (PS3) : %s\n", 
        m_pcGeometryShaderProgramFilePS3);
    NSBUtility::Dump(pf, true, "GeometryShader Program File (D3D10) : %s\n", 
        m_pcGeometryShaderProgramFileD3D10);

    NSBUtility::Dump(pf, true, "\n");
    NSBUtility::Dump(pf, true, "GeometryShader Program Entry Point: %s\n", 
        m_pcGSProgramEntryPoint);

    NSBUtility::Dump(pf, true, "\n");
    NSBUtility::Dump(pf, true, "GeometryShader Program Shader Target: %s\n", 
        m_pcGSProgramTarget);

    uiConstantMapCount = m_kGeometryShaderConstantMaps.GetSize();
    if (uiConstantMapCount > 0)
    {
        NSBUtility::Dump(pf, true, "GeometryConstantMaps\n");
        NSBUtility::IndentInsert();
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            m_kGeometryShaderConstantMaps.GetAt(i)->Dump(pf);
        }
        NSBUtility::IndentRemove();
    }
    else
    {
        NSBUtility::Dump(pf, true, "*** NO GEOMETRY CMAPS  ***\n");
    }

    NSBUtility::Dump(pf, true, "\n");
    NSBUtility::Dump(pf, true, "PixelShader Program File (DX9) : %s\n",
        m_pcPixelShaderProgramFileDX9);
    NSBUtility::Dump(pf, true, "PixelShader Program File (Xenon) : %s\n",
        m_pcPixelShaderProgramFileXenon);
    NSBUtility::Dump(pf, true, "PixelShader Program File (PS3) : %s\n",
        m_pcPixelShaderProgramFilePS3);
    NSBUtility::Dump(pf, true, "PixelShader Program File (D3D10) : %s\n",
        m_pcPixelShaderProgramFileD3D10);

    NSBUtility::Dump(pf, true, "\n");
    NSBUtility::Dump(pf, true, "PixelShader Program Entry Point: %s\n", 
        m_pcPSProgramEntryPoint);

    NSBUtility::Dump(pf, true, "\n");
    NSBUtility::Dump(pf, true, "PixelShader Program Shader Target: %s\n", 
        m_pcPSProgramTarget);

    uiConstantMapCount = m_kPixelShaderConstantMaps.GetSize();
    if (uiConstantMapCount > 0)
    {
        NSBUtility::Dump(pf, true, "PixelConstantMaps\n");
        NSBUtility::IndentInsert();
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            m_kPixelShaderConstantMaps.GetAt(i)->Dump(pf);
        }
        NSBUtility::IndentRemove();
    }
    else
    {
        NSBUtility::Dump(pf, true, "***  NO PIXEL CMAPS  ***\n");
    }

    NSBUtility::Dump(pf, true, "\n");
    NSBUtility::Dump(pf, true, "Software Vertex Processing: %s\n", 
        m_bSoftwareVP ? "True" : "False");

    unsigned int uiCount = m_akStages.GetSize();
    NSBUtility::Dump(pf, true, "Stage Count = %d\n", 
        m_akStages.GetEffectiveSize());
    NSBUtility::IndentInsert();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        NSBTextureStage* pkStage = m_akStages.GetAt(ui);
        if (pkStage)
            pkStage->Dump(pf);
    }
    NSBUtility::IndentRemove();

    uiCount = m_akTextures.GetSize();
    NSBUtility::Dump(pf, true, "Texture Count = %d\n", 
        m_akTextures.GetEffectiveSize());
    NSBUtility::IndentInsert();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        NSBTexture* pkTexture = m_akTextures.GetAt(ui);
        if (pkTexture)
            pkTexture->Dump(pf);
    }
    NSBUtility::IndentRemove();

    NSBUtility::IndentRemove();
}
//---------------------------------------------------------------------------
#endif  //#if defined(_DEBUG)
//---------------------------------------------------------------------------
