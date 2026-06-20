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

#include "NSBStateGroup.h"
#if defined(_DEBUG)
#include "NSBUtility.h"
#include "NSBStageAndSamplerStates.h"
#endif  //#if defined(_DEBUG)

#include <NiD3DUtility.h>
#include <NiD3DRenderStateGroup.h>
#include <NiD3DTextureStageGroup.h>
#include <NiD3DShaderFactory.h>

//---------------------------------------------------------------------------
void NSBStateGroup::NSBSGEntry::SetAttribute(const char* pcAttribute)
{
    NiD3DUtility::SetString(m_pcAttribute, 0, pcAttribute);
}
//---------------------------------------------------------------------------
bool NSBStateGroup::NSBSGEntry::SaveBinary(NiBinaryStream& kStream)
{
    NiStreamSaveBinary(kStream, m_uiFlags);
    NiStreamSaveBinary(kStream, m_uiState);
    NiStreamSaveBinary(kStream, m_uiValue);
    if (!NiD3DUtility::WriteString(kStream, m_pcAttribute))
        return false;
    return true;
}
//---------------------------------------------------------------------------
bool NSBStateGroup::NSBSGEntry::LoadBinary(NiBinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_uiFlags);
    NiStreamLoadBinary(kStream, m_uiState);
    NiStreamLoadBinary(kStream, m_uiValue);
    unsigned int uiStringSize = 0;
    if (!NiD3DUtility::AllocateAndReadString(kStream, m_pcAttribute, 
        uiStringSize))
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
NSBStateGroup::NSBStateGroup()
{
    m_kStateList.RemoveAll();
}
//---------------------------------------------------------------------------
NSBStateGroup::~NSBStateGroup()
{
    // Clean up the list
    NSBSGEntry* pkEntry;

    m_kStateIter = m_kStateList.GetHeadPos();
    while (m_kStateIter)
    {
        pkEntry = m_kStateList.GetNext(m_kStateIter);
        if (pkEntry)
            NiDelete pkEntry;
    }
    m_kStateList.RemoveAll();
}
//---------------------------------------------------------------------------
void NSBStateGroup::SetState(unsigned int uiState, unsigned int uiValue, 
    bool bSave, bool bUseMapValue)
{
    NSBSGEntry* pkEntry = FindStateInList(uiState);
    if (pkEntry)
    {
        NiD3DUtility::Log("Warning: NSBStateGroup::SetState> Overwriting "
            " State 0x%08x - original value 0x%08x - NiNew value 0x%08x\n",
            uiState, pkEntry->GetValue(), uiValue);

        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
            true, "* WARNING: NSBStateGroup::SetState\n"
            "    Overwriting State 0x%08x\n"
            "    Original Value = 0x%08x\n"
            "    New Value      = 0x%08x\n", 
            uiState, pkEntry->GetValue(), uiValue);

        pkEntry->SetValue(uiValue);
        pkEntry->SetSaved(bSave);
        pkEntry->SetUseAttribute(false);
        return;
    }

    // The entry didn't exist, so create one and add it
    pkEntry = NiNew NSBSGEntry();
    NIASSERT(pkEntry);

    pkEntry->SetState(uiState);
    pkEntry->SetValue(uiValue);
    pkEntry->SetSaved(bSave);
    pkEntry->SetUseAttribute(false);
    pkEntry->SetUseMapValue(bUseMapValue);

    m_kStateList.AddTail(pkEntry);
}
//---------------------------------------------------------------------------
void NSBStateGroup::SetState(unsigned int uiState, const char* pcAttribute, 
    bool bSave, bool bUseMapValue)
{
    NSBSGEntry* pkEntry = FindStateInList(uiState);
    if (pkEntry)
    {
        // We may want to generate a warning about this.
        pkEntry->SetAttribute(pcAttribute);
        pkEntry->SetSaved(bSave);
        pkEntry->SetUseAttribute(true);
        return;
    }

    // The entry didn't exist, so create one and add it
    pkEntry = NiNew NSBSGEntry();
    NIASSERT(pkEntry);

    pkEntry->SetState(uiState);
    pkEntry->SetAttribute(pcAttribute);
    pkEntry->SetSaved(bSave);
    pkEntry->SetUseAttribute(true);
    pkEntry->SetUseMapValue(bUseMapValue);

    m_kStateList.AddTail(pkEntry);
}
//---------------------------------------------------------------------------
unsigned int NSBStateGroup::GetStateCount()
{
    return m_kStateList.GetSize();
}
//---------------------------------------------------------------------------
NSBStateGroup::NSBSGEntry* NSBStateGroup::GetFirstState()
{
    NSBSGEntry* pkEntry = 0;

    m_kStateIter = m_kStateList.GetHeadPos();
    if (m_kStateIter)
        pkEntry = m_kStateList.GetNext(m_kStateIter);

    return pkEntry;
}
//---------------------------------------------------------------------------
NSBStateGroup::NSBSGEntry* NSBStateGroup::GetNextState()
{
    NSBSGEntry* pkEntry = 0;

    if (m_kStateIter)
        pkEntry = m_kStateList.GetNext(m_kStateIter);

    return pkEntry;
}
//---------------------------------------------------------------------------
bool NSBStateGroup::SetupRenderStateGroup(NiD3DRenderStateGroup& kRSGroup)
{

#if defined(WIN32)
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    return SetupRenderStateGroup_DX9(kRSGroup);
#elif defined(_XENON)
    return SetupRenderStateGroup_Xenon(kRSGroup);
#elif defined(_PS3)
    return SetupRenderStateGroup_PS3(kRSGroup);
#endif

    return false;

}
//---------------------------------------------------------------------------
bool NSBStateGroup::SetupTextureStageGroup(NiD3DTextureStageGroup& kTSGroup)
{

#if defined(WIN32)
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    return SetupTextureStageGroup_DX9(kTSGroup);
#elif defined(_XENON)
    return SetupTextureStageGroup_Xenon(kTSGroup);
#endif

}
//---------------------------------------------------------------------------
bool NSBStateGroup::SetupTextureSamplerGroup(
    NiD3DTextureStageGroup& kTSGroup)
{

#if defined(WIN32)
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    return SetupTextureSamplerGroup_DX9(kTSGroup);
#elif defined(_XENON)
    return SetupTextureSamplerGroup_Xenon(kTSGroup);
#elif defined(_PS3)
    return SetupTextureSamplerGroup_PS3(kTSGroup);
#endif

    return false;
}
//---------------------------------------------------------------------------
bool NSBStateGroup::SaveBinary(NiBinaryStream& kStream)
{
    unsigned int uiCount = m_kStateList.GetSize();
    NiStreamSaveBinary(kStream, uiCount);
    
    unsigned int uiTestCount = 0;

    NSBSGEntry* pkEntry = GetFirstState();
    while (pkEntry)
    {
        if (!pkEntry->SaveBinary(kStream))
            return false;
        uiTestCount++;
        pkEntry = GetNextState();
    }

    if (uiTestCount != uiCount)
        return false;
    return true;
}
//---------------------------------------------------------------------------
bool NSBStateGroup::LoadBinary(NiBinaryStream& kStream)
{
    unsigned int uiCount;
    NiStreamLoadBinary(kStream, uiCount);

    NSBSGEntry* pkEntry;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkEntry = NiNew NSBSGEntry();
        NIASSERT(pkEntry);

        if (!pkEntry->LoadBinary(kStream))
            return false;

        m_kStateList.AddTail(pkEntry);
    }

    return true;
}
//---------------------------------------------------------------------------
NSBStateGroup::NSBSGEntry* NSBStateGroup::FindStateInList(
    unsigned int uiState)
{
    NSBSGEntry* pkEntry = GetFirstState();
    while (pkEntry)
    {
        if (pkEntry->GetState() == uiState)
            return pkEntry;

        pkEntry = GetNextState();
    }

    return 0;
}
//---------------------------------------------------------------------------
#if defined(_DEBUG)
//---------------------------------------------------------------------------
void NSBStateGroup::Dump(FILE* pf, DumpMode eMode)
{
    NSBUtility::Dump(pf, true, "Entry Count = %d\n", GetStateCount());
    
    NSBUtility::IndentInsert();

    NSBSGEntry* pkEntry = GetFirstState();
    while (pkEntry)
    {
        if (eMode == DUMP_RENDERSTATES)
            DumpEntryAsRenderState(pf, pkEntry);
        else
        if (eMode == DUMP_STAGESTATES)
            DumpEntryAsStageState(pf, pkEntry);
        else
        if (eMode == DUMP_SAMPLERSTATES)
            DumpEntryAsSamplerState(pf, pkEntry);
        else
            DumpEntryAsUnknown(pf, pkEntry);

        pkEntry = GetNextState();
    }

    NSBUtility::IndentRemove();
}
//---------------------------------------------------------------------------
void NSBStateGroup::DumpEntryAsUnknown(FILE* pf, NSBSGEntry* pkEntry)
{
    if (!pkEntry)
        return;

    NSBUtility::Dump(pf, true, "0x%08x = 0x%08x %s\n", pkEntry->GetState(), 
        pkEntry->GetValue(), pkEntry->IsSaved() ? "SAVED" : "");
}
//---------------------------------------------------------------------------
void NSBStateGroup::DumpEntryAsRenderState(FILE* pf, NSBSGEntry* pkEntry)
{
    if (!pkEntry)
        return;

    const char* pcState = NSBRenderStates::LookupRenderStateString(
        (NSBRenderStates::NiD3DRenderState)pkEntry->GetState());
    NSBUtility::Dump(pf, true, "%32s = 0x%08x %s\n", pcState, 
        pkEntry->GetValue(), pkEntry->IsSaved() ? "SAVED" : "");
}
//---------------------------------------------------------------------------
void NSBStateGroup::DumpEntryAsStageState(FILE* pf, NSBSGEntry* pkEntry)
{
    if (!pkEntry)
        return;

    const char* pcState = 
        NSBStageAndSamplerStates::LookupTextureStageString(
            (NSBStageAndSamplerStates::NiD3DTextureStageState)
            pkEntry->GetState());
    const char* pcValue = 
        NSBStageAndSamplerStates::LookupTextureStageValueString(
            (NSBStageAndSamplerStates::NiD3DTextureStageState)pkEntry->
            GetState(), pkEntry->GetValue());
            
    NSBUtility::Dump(pf, true, "%32s = %32s %s\n", pcState, pcValue, 
        pkEntry->IsSaved() ? "SAVED" : "");
}
//---------------------------------------------------------------------------
void NSBStateGroup::DumpEntryAsSamplerState(FILE* pf, NSBSGEntry* pkEntry)
{
    if (!pkEntry)
        return;

    const char* pcState = 
        NSBStageAndSamplerStates::LookupTextureSamplerString(
        (NSBStageAndSamplerStates::NiD3DTextureSamplerState)
            pkEntry->GetState());
    const char* pcValue = 
        NSBStageAndSamplerStates::LookupTextureSamplerValueString(
            (NSBStageAndSamplerStates::NiD3DTextureSamplerState)pkEntry->
            GetState(), pkEntry->GetValue());
            
    NSBUtility::Dump(pf, true, "%32s = %32s %s\n", pcState, pcValue, 
        pkEntry->IsSaved() ? "SAVED" : "");
}
//---------------------------------------------------------------------------
#endif  //#if defined(_DEBUG)
//---------------------------------------------------------------------------
