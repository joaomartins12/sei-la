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
#include "NiD3D10BinaryShaderLibPCH.h"

#include "NSBD3D10Utility.h"
#include "NSBD3D10StateGroup.h"
#include "NSBD3D10ConstantMap.h"

#include <NiSystem.h>

bool NSBD3D10Utility::ms_bLogEnabled = true;

//---------------------------------------------------------------------------

#if defined(_DEBUG)
unsigned int NSBD3D10Utility::ms_uiIndent = 0;
#endif  //#if defined(_DEBUG)

//---------------------------------------------------------------------------
bool NSBD3D10Utility::SaveBinaryStateGroup(NiBinaryStream& kStream,
    NSBD3D10StateGroup* pkStateGroup)
{
    unsigned int uiValue;

    if (pkStateGroup)
    {
        uiValue = 1;
        NiStreamSaveBinary(kStream, uiValue);
        if (!pkStateGroup->SaveBinary(kStream))
            return false;
    }
    else
    {
        uiValue = 0;
        NiStreamSaveBinary(kStream, uiValue);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Utility::LoadBinaryStateGroup(NiBinaryStream& kStream, 
    NSBD3D10StateGroup*& pkStateGroup)
{
    unsigned int uiValue;
    NiStreamLoadBinary(kStream, uiValue);

    if (uiValue == 0)
    {
        // No render state group was written.
        return true;
    }

    pkStateGroup = NiNew NSBD3D10StateGroup();
    NIASSERT(pkStateGroup);

    if (!pkStateGroup->LoadBinary(kStream))
    {
        NiDelete pkStateGroup;
        pkStateGroup = 0;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Utility::SaveBinaryConstantMap(NiBinaryStream& kStream,
    NSBD3D10ConstantMap* pkMap)
{
    unsigned int uiValue;

    if (pkMap)
    {
        uiValue = 1;
        NiStreamSaveBinary(kStream, uiValue);

        if (!pkMap->SaveBinary(kStream))
            return false;
    }
    else
    {
        uiValue = 0;
        NiStreamSaveBinary(kStream, uiValue);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Utility::LoadBinaryConstantMap(NiBinaryStream& kStream,
    NSBD3D10ConstantMap*& pkMap)
{
    unsigned int uiValue;
    NiStreamLoadBinary(kStream, uiValue);

    if (uiValue == 0)
    {
        // No constant map stored.
        pkMap = 0;
        return true;
    }

    pkMap = NiNew NSBD3D10ConstantMap();
    NIASSERT(pkMap);

    if (!pkMap->LoadBinary(kStream))
    {
        NiDelete pkMap;
        pkMap = 0;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// This function assumes the following sizes:
//   pcDrive can hold _MAX_DRIVE    pcDir   can hold _MAX_PATH
//   pcFname can hold _MAX_PATH     pcExt   can hold _MAX_EXT
bool NSBD3D10Utility::GetSplitPath(const char* pcFilename, char* pcDrive, 
    char* pcDir, char* pcFname, char* pcExt)
{

#if _MSC_VER >= 1400
    _splitpath_s(pcFilename, pcDrive, _MAX_DRIVE, pcDir, _MAX_PATH,
        pcFname, _MAX_PATH, pcExt, _MAX_EXT);
#else //#if _MSC_VER >= 1400
    _splitpath(pcFilename, pcDrive, pcDir, pcFname, pcExt);
#endif //#if _MSC_VER >= 1400

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Utility::GetFilePath(const char* pcFilename, char* pcPath,
    unsigned int uiMaxLen)
{
    char acDrive[_MAX_DRIVE];
    char acDir[_MAX_PATH];
    char acFileName[_MAX_FNAME];
    char acFileExt[_MAX_EXT];

#if _MSC_VER >= 1400
    _splitpath_s(pcFilename, acDrive, _MAX_DRIVE, acDir, _MAX_PATH,
        acFileName, _MAX_FNAME, acFileExt, _MAX_EXT);
#else //#if _MSC_VER >= 1400
    _splitpath(pcFilename, acDrive, acDir, acFileName, acFileExt);
#endif //#if _MSC_VER >= 1400

    if ((strlen(acDrive) + strlen(acDir) + 1) > uiMaxLen)
        return false;

    NiSprintf(pcPath, uiMaxLen, "%s%s", acDrive, acDir);

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Utility::GetFileName(const char* pcFilename, char* pcName,
    unsigned int uiMaxLen)
{
    char acDrive[_MAX_DRIVE];
    char acDir[_MAX_PATH];
    char acFileName[_MAX_FNAME];
    char acFileExt[_MAX_EXT];

#if _MSC_VER >= 1400
    _splitpath_s(pcFilename, acDrive, _MAX_DRIVE, acDir, _MAX_PATH,
        acFileName, _MAX_FNAME, acFileExt, _MAX_EXT);
#else //#if _MSC_VER >= 1400
    _splitpath(pcFilename, acDrive, acDir, acFileName, acFileExt);
#endif //#if _MSC_VER >= 1400

    if ((strlen(acFileName) + 1) > uiMaxLen)
        return false;

    NiSprintf(pcName, uiMaxLen, "%s", acFileName);

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Utility::GetFileExtension(const char* pcFilename, char* pcExt, 
    unsigned int uiMaxLen)
{
    char acDrive[_MAX_DRIVE];
    char acDir[_MAX_PATH];
    char acFileName[_MAX_FNAME];
    char acFileExt[_MAX_EXT];

#if _MSC_VER >= 1400
    _splitpath_s(pcFilename, acDrive, _MAX_DRIVE, acDir, _MAX_PATH,
        acFileName, _MAX_FNAME, acFileExt, _MAX_EXT);
#else //#if _MSC_VER >= 1400
    _splitpath(pcFilename, acDrive, acDir, acFileName, acFileExt);
#endif //#if _MSC_VER >= 1400

    size_t stExtLen = strlen(acFileExt);
    NIASSERT(stExtLen < uiMaxLen);
    NiStrncpy(pcExt, uiMaxLen, acFileExt, stExtLen);

    return true;
}
//---------------------------------------------------------------------------
unsigned int NSBD3D10Utility::SetString(char*& pcDest,
    unsigned int uiDestSize, const char* pcSrc)
{
    if (pcSrc && pcSrc[0] != '\0')
    {
        unsigned int uiSrcSize = strlen(pcSrc) + 1;
        if (pcDest)
        {
            // See if the length is ok
            if (uiDestSize <= uiSrcSize)
            {
                NiFree(pcDest);
                pcDest = 0;
            }
        }

        if (!pcDest)
        {
            pcDest = NiAlloc(char, uiSrcSize);
            uiDestSize = uiSrcSize;
        }

        NiStrcpy(pcDest, uiDestSize, pcSrc);
        return uiDestSize;
    }
    else
    {
        NiFree(pcDest);
        pcDest = 0;
        return 0;
    }
}
//---------------------------------------------------------------------------
bool NSBD3D10Utility::WriteString(NiBinaryStream& kStream,
    const char* pcString)
{
    unsigned int uiLen = (pcString ? strlen(pcString) : 0);
    NiStreamSaveBinary(kStream, uiLen);

    if (uiLen)
    {
        NiStreamSaveBinary(kStream, pcString, uiLen);
    }
    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Utility::WriteData(NiBinaryStream& kStream, void* pvData, 
    unsigned int uiSize)
{
    NiStreamSaveBinary(kStream, uiSize);

    if (uiSize)
    {
        unsigned int uiWrote = kStream.Write(pvData, uiSize);
        if (uiWrote != uiSize)
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Utility::ReadString(NiBinaryStream& kStream, char* pcString,
    unsigned int& uiBufferSize)
{
    unsigned int uiLen;
    NiStreamLoadBinary(kStream, uiLen);
    
    if (uiLen >= uiBufferSize)
    {
        uiBufferSize = uiLen + 1;
        return false;
    }

    if (uiLen)
        NiStreamLoadBinary(kStream, pcString, uiLen);
    pcString[uiLen] = '\0';

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Utility::AllocateAndReadString(NiBinaryStream& kStream, 
    char*& pcString, unsigned int& uiStringSize)
{
    unsigned int uiLen;
    NiStreamLoadBinary(kStream, uiLen);

    if (uiLen)
    {
        NiFree(pcString);
        uiStringSize = uiLen + 1;
        pcString = NiAlloc(char, uiStringSize);
        NIASSERT(pcString);

        NiStreamLoadBinary(kStream, pcString, uiLen);
        pcString[uiLen] = '\0';
    }
    else
    {
        pcString = NULL;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Utility::ReadData(NiBinaryStream& kStream, void*& pvData,
    unsigned int& uiSize)
{
    unsigned int uiReadSize;
    NiStreamLoadBinary(kStream, uiReadSize);

    if (uiReadSize > uiSize)
    {
        uiSize = uiReadSize;
        return false;
    }

    if (uiSize)
    {
        unsigned int uiRead = kStream.Read(pvData, uiSize);
        if (uiRead != uiSize)
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Utility::AllocateAndReadData(NiBinaryStream& kStream,
    void*& pvData, unsigned int& uiDataSize)
{
    NiStreamLoadBinary(kStream, uiDataSize);

    if (uiDataSize)
    {
        pvData = NiAlloc(unsigned char, uiDataSize);
        NIASSERT(pvData);

        unsigned int uiRead = kStream.Read(pvData, uiDataSize);
        if (uiRead != uiDataSize)
            return false;
    }
    else
    {
        pvData = NULL;
    }

    return true;
}
//---------------------------------------------------------------------------
void NSBD3D10Utility::Log(char* pcFmt, ...)
{
    if (!ms_bLogEnabled)
        return;

    char acMessage[1024];

    va_list args;
    va_start(args, pcFmt);
    NiVsprintf(acMessage, 1024, pcFmt, args);
    va_end(args);

    NiOutputDebugString(acMessage);
}
//---------------------------------------------------------------------------
bool NSBD3D10Utility::GetLogEnabled()
{
    return ms_bLogEnabled;
}
//---------------------------------------------------------------------------
void NSBD3D10Utility::SetLogEnabled(bool bEnabled)
{
    ms_bLogEnabled = bEnabled;
}
//---------------------------------------------------------------------------

#if defined(_DEBUG)

//---------------------------------------------------------------------------
void NSBD3D10Utility::IndentInsert()
{
    ms_uiIndent += 4;
}
//---------------------------------------------------------------------------
void NSBD3D10Utility::IndentRemove()
{
    ms_uiIndent -= 4;
}
//---------------------------------------------------------------------------
void NSBD3D10Utility::Dump(FILE* pf, bool bIndent, char* pcFmt, ...)
{
    char acTemp[1024];
    char acMessage[1024];

    if (bIndent && (ms_uiIndent != 0))
    {
        acMessage[0] = ' ';
        acMessage[1] = 0;
        for (unsigned int ui = 1; ui < ms_uiIndent; ui++)
            NiStrcat(acMessage, 1024, " ");
    }
    else
    {
        acMessage[0] = 0;
    }

    va_list args;
    va_start(args, pcFmt);
    NiVsprintf(acTemp, 1024, pcFmt, args);
    va_end(args);

    NiStrcat(acMessage, 1024, acTemp);
    if (pf)
        fputs(acMessage, pf);
    else
        NiOutputDebugString(acMessage);
}
//---------------------------------------------------------------------------

#endif  //#if defined(_DEBUG)
