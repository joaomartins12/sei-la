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

#include "NSBPackingDef.h"
#include <NiD3DUtility.h>
#include <NiD3DShaderFactory.h>

//---------------------------------------------------------------------------
bool NSBPackingDef::NSBPDEntry::SaveBinary(NiBinaryStream& kStream)
{
    NiStreamSaveBinary(kStream, m_uiStream);
    NiStreamSaveBinary(kStream, m_uiRegister);
    NiStreamSaveBinary(kStream, m_uiInput);

    unsigned int uiValue;
    uiValue = (unsigned int)m_eType;
    NiStreamSaveBinary(kStream, uiValue);
    uiValue = (unsigned int)m_eTesselator;
    NiStreamSaveBinary(kStream, uiValue);
    uiValue = (unsigned int)m_eUsage;
    NiStreamSaveBinary(kStream, uiValue);

    NiStreamSaveBinary(kStream, m_uiUsageIndex);

    return true;
}
//---------------------------------------------------------------------------
bool NSBPackingDef::NSBPDEntry::LoadBinary(NiBinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_uiStream);
    NiStreamLoadBinary(kStream, m_uiRegister);
    NiStreamLoadBinary(kStream, m_uiInput);

    unsigned int uiValue;
    NiStreamLoadBinary(kStream, uiValue);
    m_eType = (NiD3DPackingDefType)uiValue;
    NiStreamLoadBinary(kStream, uiValue);
    m_eTesselator = 
        (NiD3DShaderDeclaration::ShaderParameterTesselator)uiValue;
    NiStreamLoadBinary(kStream, uiValue);
    m_eUsage = (NiD3DShaderDeclaration::ShaderParameterUsage)uiValue;
    
    NiStreamLoadBinary(kStream, m_uiUsageIndex);

    return true;
}
//---------------------------------------------------------------------------
NSBPackingDef::NSBPackingDef() :
    m_pcName(0), 
    m_bFixedFunction(false)
{
    m_kEntryList.RemoveAll();
}
//---------------------------------------------------------------------------
NSBPackingDef::~NSBPackingDef()
{
    NSBPDEntry* pkEntry;
    NiTListIterator kIter = m_kEntryList.GetHeadPos();
    while (kIter)
    {
        pkEntry = m_kEntryList.GetNext(kIter);
        if (pkEntry)
            NiDelete pkEntry;
    }
    m_kEntryList.RemoveAll();

    NiFree(m_pcName);
}
//---------------------------------------------------------------------------
const char* NSBPackingDef::GetName()
{
    return m_pcName;
}
//---------------------------------------------------------------------------
void NSBPackingDef::SetName(const char* pcName)
{
    NiD3DUtility::SetString(m_pcName, 0, pcName);
}
//---------------------------------------------------------------------------
bool NSBPackingDef::GetFixedFunction()
{
    return m_bFixedFunction;
}
//---------------------------------------------------------------------------
void NSBPackingDef::SetFixedFunction(bool bFixedFunction)
{
    m_bFixedFunction = bFixedFunction;
}
//---------------------------------------------------------------------------
bool NSBPackingDef::AddPackingEntry(unsigned int uiStream, 
    unsigned int uiRegister, unsigned int uiInput, NiD3DPackingDefType eType,
    NiD3DShaderDeclaration::ShaderParameterTesselator eTesselator, 
    NiD3DShaderDeclaration::ShaderParameterUsage eUsage, 
    unsigned int uiUsageIndex)
{
    bool bCreatedEntry = false;

    // Check for the entry in the list...
    NSBPDEntry* pkEntry = GetFirstEntry();
    while (pkEntry)
    {
        if ((pkEntry->GetStream() == uiStream) &&
            (pkEntry->GetRegister() == uiRegister))
        {
            // Exists... just overwrite
            NiD3DUtility::Log("WARNING> PackingDef %s - Entry stream %d "
                "register %d being written over!\n", m_pcName, 
                uiStream, uiRegister);
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                true, "* WARNING: NSBPackingDef::AddPackingEntry\n"
                "    %s - overwriting stream %d, register %d\n", 
                m_pcName, uiStream, uiRegister);
            break;
        }
        pkEntry = GetNextEntry();
    }

    if (pkEntry == 0)
    {
        pkEntry = NiNew NSBPDEntry();
        if (!pkEntry)
            return false;
        bCreatedEntry = true;
    }

    pkEntry->SetStream(uiStream);
    pkEntry->SetRegister(uiRegister);
    pkEntry->SetInput(uiInput);
    pkEntry->SetType(eType);
    pkEntry->SetTesselator(eTesselator);
    pkEntry->SetUsage(eUsage);
    pkEntry->SetUsageIndex(uiUsageIndex);

    if (bCreatedEntry)
        m_kEntryList.AddTail(pkEntry);

    return true;
}
//---------------------------------------------------------------------------
const char* NSBPackingDef::GetTypeName(
    NSBPackingDef::NiD3DPackingDefType eType)
{
    switch (eType)
    {
    case NID3DPDT_FLOAT1:       return "Float1";
    case NID3DPDT_FLOAT2:       return "Float2";
    case NID3DPDT_FLOAT3:       return "Float3";
    case NID3DPDT_FLOAT4:       return "Float4";
    case NID3DPDT_UBYTECOLOR:   return "UByteColor";
    case NID3DPDT_UBYTE4:       return "UByte4";
    case NID3DPDT_SHORT1:       return "Short1";
    case NID3DPDT_SHORT2:       return "Short2";
    case NID3DPDT_SHORT3:       return "Short3";
    case NID3DPDT_SHORT4:       return "Short4";
    case NID3DPDT_NORMSHORT1:   return "NormShort1";
    case NID3DPDT_NORMSHORT2:   return "NormShort2";
    case NID3DPDT_NORMSHORT3:   return "NormShort3";
    case NID3DPDT_NORMSHORT4:   return "NormShort4";
    case NID3DPDT_NORMPACKED3:  return "NormPacked3";
    case NID3DPDT_PBYTE1:       return "PByte1";
    case NID3DPDT_PBYTE2:       return "PByte2";
    case NID3DPDT_PBYTE3:       return "PByte3";
    case NID3DPDT_PBYTE4:       return "PByte4";
    case NID3DPDT_FLOAT2H:      return "Float2H";
    default:                    return "UNKNOWN";
    }
}
//---------------------------------------------------------------------------
const char* NSBPackingDef::GetParameterName(
    NiD3DShaderDeclaration::ShaderParameter eParam)
{
    if ((eParam & NiD3DShaderDeclaration::SHADERPARAM_EXTRA_DATA_MASK) != 0)
    {
        return "ExtraData";
    }

    switch (eParam)
    {
    case NiD3DShaderDeclaration::SHADERPARAM_NI_POSITION:
        return "NiPosition";
    case NiD3DShaderDeclaration::SHADERPARAM_NI_BLENDWEIGHT:
        return "NiBlendWeight";
    case NiD3DShaderDeclaration::SHADERPARAM_NI_BLENDINDICES:
        return "NiBlendIndices";
    case NiD3DShaderDeclaration::SHADERPARAM_NI_NORMAL:
        return "NiNormal";
    case NiD3DShaderDeclaration::SHADERPARAM_NI_COLOR:
        return "NiColor";
    case NiD3DShaderDeclaration::SHADERPARAM_NI_TEXCOORD0:
        return "NiTexCoord0";
    case NiD3DShaderDeclaration::SHADERPARAM_NI_TEXCOORD1:
        return "NiTexCoord1";
    case NiD3DShaderDeclaration::SHADERPARAM_NI_TEXCOORD2:
        return "NiTexCoord2";
    case NiD3DShaderDeclaration::SHADERPARAM_NI_TEXCOORD3:
        return "NiTexCoord3";
    case NiD3DShaderDeclaration::SHADERPARAM_NI_TEXCOORD4:
        return "NiTexCoord4";
    case NiD3DShaderDeclaration::SHADERPARAM_NI_TEXCOORD5:
        return "NiTexCoord5";
    case NiD3DShaderDeclaration::SHADERPARAM_NI_TEXCOORD6:
        return "NiTexCoord6";
    case NiD3DShaderDeclaration::SHADERPARAM_NI_TEXCOORD7:
        return "NiTexCoord7";
    case NiD3DShaderDeclaration::SHADERPARAM_NI_TANGENT:
        return "NiTangent";
    case NiD3DShaderDeclaration::SHADERPARAM_NI_BINORMAL:
        return "NiBinormal";
    default:
        return "UNKNOWN";
    }
}
//---------------------------------------------------------------------------
NiD3DShaderDeclaration* NSBPackingDef::GetShaderDeclaration(
    NiShader::Platform ePlatform, NiD3DRenderer* pkRenderer)
{

#if defined(WIN32)
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    return GetShaderDeclaration_DX9(pkRenderer);
#elif defined(_XENON)
    return GetShaderDeclaration_Xenon(pkRenderer);
#elif defined(_PS3)
    return GetShaderDeclaration_PS3(pkRenderer);
#endif

    return 0;
}
//---------------------------------------------------------------------------
NiD3DShaderDeclaration::ShaderParameterType 
    NSBPackingDef::ConvertPackingDefType(NiD3DPackingDefType eType)
{

#if defined(WIN32)
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    return ConvertPackingDefType_DX9(eType);
#elif defined(_XENON)
    return ConvertPackingDefType_Xenon(eType);
#elif defined(_PS3)
    return ConvertPackingDefType_PS3(eType);
#endif

    return NiD3DShaderDeclaration::SPTYPE_NONE;
}
//---------------------------------------------------------------------------
bool NSBPackingDef::SaveBinary(NiBinaryStream& kStream)
{
    if (!NiD3DUtility::WriteString(kStream, m_pcName))
        return false;

    unsigned int uiValue = m_bFixedFunction ? 1 : 0;
    NiStreamSaveBinary(kStream, uiValue);

    unsigned int uiCount = m_kEntryList.GetSize();
    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;
    NSBPDEntry* pkEntry = GetFirstEntry();
    while (pkEntry)
    {
        if (pkEntry->SaveBinary(kStream))
            uiTestCount++;
        pkEntry = GetNextEntry();
    }

    if (uiTestCount != uiCount)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NSBPackingDef::LoadBinary(NiBinaryStream& kStream)
{
    unsigned int uiStringSize = 0;
    if (!NiD3DUtility::AllocateAndReadString(kStream, m_pcName, uiStringSize))
        return false;

    unsigned int uiValue;
    NiStreamLoadBinary(kStream, uiValue);
    m_bFixedFunction = (uiValue != 0);

    unsigned int uiCount;
    NiStreamLoadBinary(kStream, uiCount);

    NSBPDEntry* pkEntry;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkEntry = NiNew NSBPDEntry();
        NIASSERT(pkEntry);

        if (!pkEntry->LoadBinary(kStream))
            return false;

        m_kEntryList.AddTail(pkEntry);
    }

    return true;
}
//---------------------------------------------------------------------------
NSBPackingDef::NSBPDEntry* NSBPackingDef::GetFirstEntry()
{
    NSBPDEntry* pkEntry = 0;

    m_kEntryIter = m_kEntryList.GetHeadPos();
    if (m_kEntryIter)
        pkEntry = m_kEntryList.GetNext(m_kEntryIter);

    return pkEntry;
}
//---------------------------------------------------------------------------
NSBPackingDef::NSBPDEntry* NSBPackingDef::GetNextEntry()
{
    NSBPDEntry* pkEntry = 0;

    if (m_kEntryIter)
        pkEntry = m_kEntryList.GetNext(m_kEntryIter);

    return pkEntry;
}
//---------------------------------------------------------------------------
void NSBPackingDef::GetStreamInfo(unsigned int& uiStreamCount, 
    unsigned int& uiMaxStreamEntryCount)
{
    // Should use the MAX_STREAM here, but this will suffice for Xenon
    // and DX9
    const unsigned int LOCAL_MAX_STREAM = 64;
    unsigned int auiStreamEntryCount[LOCAL_MAX_STREAM];

    uiStreamCount = 0;
    uiMaxStreamEntryCount = 0;

    unsigned int ui;

    // Initialize the 'seen' array
    for (ui = 0; ui < LOCAL_MAX_STREAM; ui++)
        auiStreamEntryCount[ui] = 0;
   
    // Now cycle through and count the streams
    NSBPDEntry* pkEntry = GetFirstEntry();
    while (pkEntry)
    {
        unsigned int uiStream = pkEntry->GetStream();
        // Safety catch!
        NIASSERT(uiStream < LOCAL_MAX_STREAM);

        if (auiStreamEntryCount[uiStream] == 0)
            uiStreamCount++;
        auiStreamEntryCount[uiStream] += 1;

        pkEntry = GetNextEntry();
    }

    // Now, determine the max entry count
    for (ui = 0; ui < LOCAL_MAX_STREAM; ui++)
    {
        if (auiStreamEntryCount[ui] > uiMaxStreamEntryCount)
            uiMaxStreamEntryCount = auiStreamEntryCount[ui];
    }
}
//---------------------------------------------------------------------------
