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

#include "NiMainPCH.h"
#include "NiString.h"
#include <NiPoint3.h>
#include <NiColor.h>
#include <stdio.h>

#define NI_BUFFER_SIZE_IDX    0
#define NI_REF_COUNT_IDX      1
#define NI_STRING_LENGTH_IDX  2
#define NI_HEADER_OFFSET  (3*sizeof(size_t))

//---------------------------------------------------------------------------
NiString::StringHandle NiString::Allocate(size_t stStrLength)
{
    if (stStrLength == 0)
        stStrLength = 1;

    size_t stBufferSize = stStrLength * sizeof(char) + NI_HEADER_OFFSET + 
        sizeof(char);
    stBufferSize = GetBestBufferSize(stBufferSize);

    char* pcBuffer = NiAlloc(char, stBufferSize);
    size_t* pkBuffer = (size_t*) pcBuffer;
    pkBuffer[NI_BUFFER_SIZE_IDX] = stBufferSize;
    pkBuffer[NI_REF_COUNT_IDX] = 1;
    pkBuffer[NI_STRING_LENGTH_IDX] = 0;
    return (StringHandle) (pcBuffer + NI_HEADER_OFFSET);
}
//---------------------------------------------------------------------------
NiString::StringHandle NiString::AllocateAndCopy(const char* pcString)
{
    if (pcString == NULL)
        return (StringHandle) NULL;

    size_t stLength = strlen(pcString);
    StringHandle kHandle = Allocate(stLength);
    if (kHandle == (StringHandle) NULL)
        return (StringHandle) NULL;

    NiMemcpy(kHandle, pcString, stLength + 1);
    NIASSERT(strcmp(kHandle, pcString) == 0);

    size_t* pkBuffer = (size_t*)GetRealBufferStart(kHandle);
    pkBuffer[NI_STRING_LENGTH_IDX] = stLength;
    return kHandle;
}
//---------------------------------------------------------------------------
NiString::StringHandle NiString::AllocateAndCopyHandle(StringHandle kHandle)
{
    if (kHandle == NULL)
        return (StringHandle) NULL;

    size_t stLength = GetLength(kHandle);
    size_t stBufferSize = GetAllocationSize(kHandle);
    char* pcBuffer = NiAlloc(char, stBufferSize);
    size_t* pkBuffer = (size_t*) pcBuffer;
    pkBuffer[NI_BUFFER_SIZE_IDX] = stBufferSize;
    pkBuffer[NI_REF_COUNT_IDX] = 1;
    pkBuffer[NI_STRING_LENGTH_IDX] = stLength;
    StringHandle kNewHandle = (StringHandle) (pcBuffer + NI_HEADER_OFFSET);
    NiMemcpy(kNewHandle, stLength + 1, kHandle, stLength + 1);
    NIASSERT(strcmp(kNewHandle, kHandle) == 0);
    return kNewHandle;
}
//---------------------------------------------------------------------------
void NiString::Deallocate(StringHandle& kHandle)
{
    if (kHandle != (StringHandle) NULL)
    {
        char* pcMem = GetRealBufferStart(kHandle); 
        NiFree(pcMem);
        kHandle = (StringHandle) NULL;
    }
}
//---------------------------------------------------------------------------
char* NiString::GetRealBufferStart(const StringHandle& kHandle)
{
    NIASSERT(kHandle != (StringHandle) NULL);
    return ((char*)kHandle - NI_HEADER_OFFSET);
}
//---------------------------------------------------------------------------
void NiString::IncRefCount(StringHandle& kHandle, bool bValidate)
{
    if (kHandle == (StringHandle) NULL)
        return;

#ifdef _DEBUG
    if(bValidate)
        NIASSERT(ValidateString(kHandle));
#endif
    size_t* pkMem = (size_t*)GetRealBufferStart(kHandle);
    NiAtomicIncrement(pkMem[NI_REF_COUNT_IDX]);
}
//---------------------------------------------------------------------------
void NiString::DecRefCount(StringHandle& kHandle, bool bValidate)
{
    if (kHandle == (StringHandle) NULL)
        return;

#ifdef _DEBUG
    if(bValidate)
        NIASSERT(ValidateString(kHandle));
#endif
    size_t* pkMem = (size_t*)GetRealBufferStart(kHandle);
    NiAtomicDecrement(pkMem[NI_REF_COUNT_IDX]);

    if (pkMem[NI_REF_COUNT_IDX] == 0)
    {
        Deallocate(kHandle);
    }

    kHandle = (StringHandle) NULL;
}
//---------------------------------------------------------------------------
void NiString::Swap(StringHandle& kHandle, const char* pcNewValue,
    bool bValidate)
{
    if (pcNewValue == NULL)
    {
        DecRefCount(kHandle, bValidate);
        return;
    }
    
    if (kHandle == (StringHandle) NULL)
    {
        kHandle = AllocateAndCopy(pcNewValue);
        return;
    }
    

#ifdef _DEBUG
    if(bValidate)
        NIASSERT(ValidateString(kHandle));
#endif

    size_t* pkMem = (size_t*)GetRealBufferStart(kHandle);
    NiAtomicDecrement(pkMem[NI_REF_COUNT_IDX]);

    if (pkMem[NI_REF_COUNT_IDX] == 0)
    {
        size_t stLength = strlen(pcNewValue);
        if (pkMem[NI_BUFFER_SIZE_IDX] >= (stLength + 1 + NI_HEADER_OFFSET))
        {
            pkMem[NI_REF_COUNT_IDX] = 1;
        }
        else
        {
            Deallocate(kHandle);
            kHandle = Allocate(stLength);
            pkMem = (size_t*)GetRealBufferStart(kHandle);
        }

        NiMemcpy(kHandle, pcNewValue, stLength + 1);
        NIASSERT(strcmp(kHandle, pcNewValue) == 0);

        pkMem[NI_STRING_LENGTH_IDX] = stLength;
    }
    else
    {
        kHandle = AllocateAndCopy(pcNewValue);
    }
}
//---------------------------------------------------------------------------
char* NiString::GetString(const StringHandle& kHandle, bool bValidate) 
{
#ifdef _DEBUG
    if(bValidate)
        NIASSERT(ValidateString(kHandle));
#endif
    // No need to perform an if NULL check, because
    // it will correctly return NULL if kHandle == NULL
    return (char*) kHandle;
}
//---------------------------------------------------------------------------
size_t NiString::GetLength(const StringHandle& kHandle, bool bValidate) 
{
    if (kHandle == (StringHandle) NULL)
    {
        return 0;
    }
    else
    {
#ifdef _DEBUG
        if(bValidate)
            NIASSERT(ValidateString(kHandle));
#endif
        size_t* pkMem = (size_t*)GetRealBufferStart(kHandle);
        return pkMem[NI_STRING_LENGTH_IDX];
    }
}
//---------------------------------------------------------------------------
size_t NiString::GetAllocationSize(const StringHandle& kHandle, bool bValidate)
{
    if (kHandle == (StringHandle) NULL)
    {
        return 0;
    }
    else
    {
#ifdef _DEBUG
        if(bValidate)
            NIASSERT(ValidateString(kHandle));
#endif
        size_t* pkMem = (size_t*)GetRealBufferStart(kHandle);
        return pkMem[NI_BUFFER_SIZE_IDX];
    }
}
//---------------------------------------------------------------------------
size_t NiString::GetBufferSize(const StringHandle& kHandle, bool bValidate) 
{
    if (kHandle == (StringHandle) NULL)
    {
        return 0;
    }
    else
    {
#ifdef _DEBUG
        if(bValidate)
            NIASSERT(ValidateString(kHandle));
#endif
        size_t* pkMem = (size_t*)GetRealBufferStart(kHandle);
        return pkMem[NI_BUFFER_SIZE_IDX] - NI_HEADER_OFFSET;
    }
}
//---------------------------------------------------------------------------
size_t NiString::GetRefCount(const StringHandle& kHandle, bool bValidate) 
{
    if (kHandle == (StringHandle) NULL)
    {
        return 0;
    }
    else
    {
#ifdef _DEBUG
        if(bValidate)
            NIASSERT(ValidateString(kHandle));
#endif
        size_t* pkMem = (size_t*)GetRealBufferStart(kHandle);
        return pkMem[NI_REF_COUNT_IDX];
    }
}
//---------------------------------------------------------------------------
void NiString::SetLength(const StringHandle& kHandle, size_t stLength)
{
    if (kHandle == (StringHandle) NULL)
        return;

    size_t* pkMem = (size_t*)GetRealBufferStart(kHandle);
    pkMem[NI_STRING_LENGTH_IDX] = stLength;
    NIASSERT(pkMem[NI_STRING_LENGTH_IDX] <= pkMem[NI_BUFFER_SIZE_IDX]);
}
//---------------------------------------------------------------------------
bool NiString::ValidateString(const StringHandle& kHandle)
{
    if (kHandle == (StringHandle) NULL)
        return true;

    size_t* pkMem = (size_t*)GetRealBufferStart(kHandle);
    size_t stLength = pkMem[NI_STRING_LENGTH_IDX];

    if (stLength != strlen((const char*)kHandle))
        return false;

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiString::GetBestBufferSize(unsigned int uiReqSize)
{
    if (uiReqSize < 32)
        return 32;
    else if (uiReqSize < 64)
        return 64;
    else if (uiReqSize < 128)
        return 128;
    else if (uiReqSize < 255)
        return 255;
    else if (uiReqSize < 512)
        return 512;
    else if (uiReqSize < 1024)
        return 1024;
    else 
        return uiReqSize + 1;
}
//---------------------------------------------------------------------------
void NiString::CalcLength()
{
    if (m_kHandle == NULL)
        return;

    SetLength(m_kHandle, strlen(m_kHandle));
}
//---------------------------------------------------------------------------
void NiString::SetBuffer(StringHandle& kHandle)
{
    if (kHandle == m_kHandle)
        return;

    DecRefCount(m_kHandle);
    IncRefCount(kHandle);
    m_kHandle = kHandle;
}
//---------------------------------------------------------------------------
void NiString::CopyOnWrite(bool bForceCopy)
{
    if(GetRefCount(m_kHandle) > 1 || bForceCopy)
    {
        StringHandle kHandle = m_kHandle;
        m_kHandle = AllocateAndCopyHandle(m_kHandle);
        DecRefCount(kHandle);
    }
}
//---------------------------------------------------------------------------
void NiString::CopyOnWriteAndResize(unsigned int uiSizeDelta, bool bForceCopy)
{
    if (Resize(uiSizeDelta))
        return;

    if (GetRefCount(m_kHandle) > 1 || bForceCopy)
    {
        StringHandle kHandle = m_kHandle;
        m_kHandle = AllocateAndCopyHandle(m_kHandle);
        DecRefCount(kHandle);
    }
}
//---------------------------------------------------------------------------
NiString::NiString(char ch)
{
    m_kHandle = Allocate(1);
    char* pcString = (char*) m_kHandle;
    pcString[0] = ch;
    pcString[1] = '\0';
    CalcLength();
}
//---------------------------------------------------------------------------
NiString::NiString(size_t stBuffLength)
{ 
    m_kHandle = Allocate(stBuffLength);
    char* pcString = (char*) m_kHandle;
    pcString[0] = '\0';
    CalcLength();
}
//---------------------------------------------------------------------------
NiString::NiString(int iBuffLength)
{ 
    if (iBuffLength < 0)
        iBuffLength = 0;

    m_kHandle = Allocate((size_t)iBuffLength);
    char* pcString = (char*) m_kHandle;
    pcString[0] = '\0';
    CalcLength();
}
//---------------------------------------------------------------------------
NiString::NiString(const char* pcStr)
{ 
    m_kHandle = AllocateAndCopy(pcStr);
    CalcLength();
}
//---------------------------------------------------------------------------
NiString::NiString(const NiFixedString& kStr)
{ 
    m_kHandle = AllocateAndCopy((const char*) kStr);
    CalcLength();
}
//---------------------------------------------------------------------------
NiString::NiString(const NiString& kString)
{
    m_kHandle = kString.m_kHandle;
    IncRefCount(m_kHandle);
}
//---------------------------------------------------------------------------
NiString::~NiString()
{
    DecRefCount(m_kHandle);
}
//---------------------------------------------------------------------------
// Gets the length of the string
unsigned int NiString::Length() const
{
    return GetLength(m_kHandle);
}
//---------------------------------------------------------------------------
// Tests whether a NiString object contains no characters.
bool NiString::IsEmpty() const
{ 
    return Length() == 0;
}
//---------------------------------------------------------------------------
// Forces a string to have 0 length. 
void NiString::Empty()
{
    DecRefCount(m_kHandle);
} 
//---------------------------------------------------------------------------
// Returns the character at a given position. 
char NiString::GetAt(unsigned int ui) const
{
    char c;
    if (ui >= Length())
        c = '\0';
    else
        c = GetString(m_kHandle)[ui];
    return c;
}
//---------------------------------------------------------------------------
// Sets a character at a given position
void NiString::SetAt(unsigned int ui, char c)
{
    if (ui >= Length())
        return;
    
    CopyOnWrite();
    GetString(m_kHandle)[ui] = c;
    if (c == '\0')
    {
        // we have altered the length of the string, update the stored length
        SetLength(m_kHandle, ui);
    }
}
//---------------------------------------------------------------------------
// Compare with another string
int NiString::Compare(const char* pcString) const
{ 
    if (pcString == NULL && m_kHandle == (StringHandle) NULL)
        return 0;
    else if (pcString == NULL)
        return GetString(m_kHandle)[0];
    else if (m_kHandle == (StringHandle) NULL)
        return -pcString[0];
    return strcmp(GetString(m_kHandle), pcString);
}
//---------------------------------------------------------------------------
int NiString::CompareNoCase(const char* pcString) const
{
    if (pcString == NULL && m_kHandle == (StringHandle) NULL)
        return 0;
    else if (pcString == NULL)
        return GetString(m_kHandle)[0];
    else if (m_kHandle == (StringHandle) NULL)
        return -pcString[0];
    return NiStricmp(GetString(m_kHandle), pcString);
}
//---------------------------------------------------------------------------
bool NiString::Equals(const char* pcStr) const
{
    if (pcStr == NULL)
        return m_kHandle == (StringHandle)NULL;
    else if (strlen(pcStr) != Length())
        return false;

    return Compare(pcStr) == 0;
}
//---------------------------------------------------------------------------
bool NiString::EqualsNoCase(const char* pcStr) const
{
    if (pcStr == NULL)
        return m_kHandle == (StringHandle)NULL;
    else if (strlen(pcStr) != Length())
        return false;
    return CompareNoCase(pcStr) == 0;
}
//---------------------------------------------------------------------------
bool NiString::Contains(const char* pcStr) const
{
    return Find(pcStr) != INVALID_INDEX;
}
//---------------------------------------------------------------------------
bool NiString::ContainsNoCase(const char* pcStr) const
{
    if (m_kHandle == pcStr && pcStr != NULL)
        return true;

    // The previous check would return true if BOTH were NULL. Now check 
    // if the const char* is NULL
    if (pcStr == NULL  || m_kHandle == NULL)
        return false;

    const char* pcMyString = (const char*) m_kHandle;
    size_t stOtherLength = strlen(pcStr);
    for (unsigned int uiMyIdx = 0; uiMyIdx < Length(); uiMyIdx++)
    {
        if (NiStrnicmp(&pcMyString[uiMyIdx], pcStr, stOtherLength) == 0)
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
// Get a separate copy of a substring
NiString NiString::GetSubstring(unsigned int uiBegin, 
    unsigned int uiEnd) const
{ 
    if (uiBegin < uiEnd && uiBegin < Length() && 
        uiEnd <= Length() && m_kHandle != NULL)
    {
        NiString kString((size_t)(uiEnd - uiBegin + 2));
        NiStrncpy(kString.m_kHandle,  
            kString.GetBufferSize(kString.m_kHandle) - 1,
            GetString(m_kHandle) + uiBegin, uiEnd - uiBegin);
        kString.m_kHandle[uiEnd - uiBegin] = '\0';
        kString.CalcLength();
        return kString;
    }
    else
    {
        return (const char*)NULL;
    }
}
//---------------------------------------------------------------------------
NiString NiString::GetSubstring(unsigned int uiBegin, 
    const char* pcDelimiters) const
{ 
    if (uiBegin < Length() && m_kHandle != NULL && pcDelimiters != NULL)
    {
        char* pcStr = strpbrk(GetString(m_kHandle) + uiBegin, pcDelimiters);
        if (pcStr == NULL)
            return (const char*)NULL;
        else
            return GetSubstring(uiBegin, pcStr - GetString(m_kHandle));
    }
    else
    {
        return (const char*)NULL;
    }
}
//---------------------------------------------------------------------------
// Find a substring within this string and return its index
unsigned int NiString::Find(char c, unsigned int uiStart) const
{ 
    if (uiStart >= Length())
        return INVALID_INDEX;

    char* pcStr = strchr(GetString(m_kHandle) + uiStart, c);
    if (pcStr == NULL)
        return INVALID_INDEX;
    
    int iReturn = pcStr - GetString(m_kHandle);
    if (iReturn < 0)
        return INVALID_INDEX;
    else
        return (unsigned int) iReturn;
}
//---------------------------------------------------------------------------
unsigned int NiString::Find(const char* pcStr, unsigned int uiStart) const
{ 
    if (uiStart >= Length())
        return INVALID_INDEX;
    if (pcStr == NULL || pcStr[0] == '\0')
        return INVALID_INDEX;
    char* pcSubStr = strstr(GetString(m_kHandle) + uiStart, pcStr);
    if (pcSubStr == NULL)
        return INVALID_INDEX;

    int iReturn = pcSubStr - GetString(m_kHandle);
    if (iReturn < 0)
        return INVALID_INDEX;
    else
        return (unsigned int) iReturn;
}
//---------------------------------------------------------------------------
unsigned int NiString::FindReverse(char c) const
{ 
    if (m_kHandle == NULL)
        return INVALID_INDEX;

    char* pcStr = strrchr(GetString(m_kHandle), c);
    if (pcStr == NULL)
        return INVALID_INDEX;
    else if (c == '\0')
        return INVALID_INDEX;
    int iReturn = pcStr - GetString(m_kHandle);
    if (iReturn < 0)
        return INVALID_INDEX;
    else
        return (unsigned int) iReturn;
}
//---------------------------------------------------------------------------
unsigned int NiString::FindReverse(char c, unsigned int uiStartIdx) const
{ 
    if (m_kHandle == NULL || c == '\0')
        return INVALID_INDEX;

    if (uiStartIdx >= Length())
        uiStartIdx = Length() - 1;

    const char* pcValue = GetString(m_kHandle);
    while (uiStartIdx != UINT_MAX)
    {
        if (pcValue[uiStartIdx] == c)
            return uiStartIdx;
        --uiStartIdx;
    }

    return INVALID_INDEX;
}
//---------------------------------------------------------------------------
unsigned int NiString::FindOneOf(const char* pcStr, 
    unsigned int uiIndex) const
{ 
    if (pcStr == NULL || pcStr == '\0' || m_kHandle == NULL)
        return INVALID_INDEX;
    char* pcSubStr = strpbrk(GetString(m_kHandle) + uiIndex, pcStr);
    if (pcSubStr == NULL)
        return INVALID_INDEX;
    int iReturn = pcSubStr - GetString(m_kHandle);
    if (iReturn < 0)
        return INVALID_INDEX;
    else
        return (unsigned int) iReturn;
}
//---------------------------------------------------------------------------
// Convert the string to uppercase internally
void NiString::ToUpper()
{ 
    if (m_kHandle == NULL)
        return;

    CopyOnWrite();
    for ( unsigned int ui = 0; ui < Length() ; ui++)
    {
        char c = GetString(m_kHandle)[ui];
        if ( ('a' <= c) && (c <= 'z') )
            GetString(m_kHandle)[ui] -= 'a' - 'A';
    }
}
//---------------------------------------------------------------------------
// Convert the string to lowercase internally
void NiString::ToLower()
{
    if (m_kHandle == NULL)
        return;

    CopyOnWrite();
    for ( unsigned int ui = 0; ui < Length() ; ui++)
    {
        char c = GetString(m_kHandle)[ui];
        if ( ('A' <= c) && (c <= 'Z') )
            GetString(m_kHandle)[ui] -= 'A' - 'a';
    }
}
//---------------------------------------------------------------------------
// Reverse the string internally
void NiString::Reverse()
{ 
    if (Length() < 2)
        return;

    CopyOnWrite();
    unsigned int uiEnd = Length() - 1;
    for (unsigned int uiBegin = 0; uiBegin < uiEnd; 
        uiBegin++, uiEnd--)
    {
        char c = GetString(m_kHandle)[uiBegin];
        GetString(m_kHandle)[uiBegin] = GetString(m_kHandle)[uiEnd];
        GetString(m_kHandle)[uiEnd] = c;
    }
}

//---------------------------------------------------------------------------
// Trim this character from the beginning of the string
void NiString::TrimLeft(char c)
{ 
    if ( c == '\0' || m_kHandle == NULL)
        return;

    CopyOnWrite();
    unsigned int uiTrimCount = 0;
    bool bContinue = true;
    for (unsigned int ui = 0; 
        ui < Length() && bContinue; ui++)
    {
        if (GetString(m_kHandle)[ui] == c)
            uiTrimCount++;
        else
            bContinue = false;
    }

    if (uiTrimCount > 0)
    {
        memmove(GetString(m_kHandle) , GetString(m_kHandle) + uiTrimCount,
            Length() - uiTrimCount + 1);
        CalcLength();
    }
}
//---------------------------------------------------------------------------
// Trim this character from the end of the string
void NiString::TrimRight(char c)
{ 
    if ( c == '\0' || m_kHandle == NULL)
        return;
    
    CopyOnWrite();
    unsigned int uiTrimCount = 0;
    bool bContinue = true;
    for(int i = Length() - 1; i >= 0 && bContinue; i--)
    {
        if (GetString(m_kHandle)[i] == c)
            uiTrimCount++;
        else
            bContinue = false;
    }

    if (uiTrimCount > 0)
    {
        char* pcStr = GetString(m_kHandle) + Length() - 
            uiTrimCount;
        pcStr[0] = '\0';
        CalcLength();
    }
}
//---------------------------------------------------------------------------
// Add the char* to the end of this string
void NiString::Concatenate(const char* pcStr)
{ 
    if (pcStr == NULL)
        return;

    unsigned int uiLen = strlen(pcStr);
    if (uiLen > 0)
    {
        StringHandle kHandle = m_kHandle;
        bool bInternalIncrementRefCount = false;
        if (pcStr == GetString(m_kHandle))
        {
            IncRefCount(kHandle);
            CopyOnWriteAndResize(uiLen, true);
            bInternalIncrementRefCount = true;
        }
        else
        {
            CopyOnWriteAndResize(uiLen, false);
        }

        NiStrcpy(GetString(m_kHandle) + Length(), 
            uiLen + 1, pcStr);
        CalcLength();
        if (bInternalIncrementRefCount)
            DecRefCount(kHandle);
    }
}
//---------------------------------------------------------------------------
bool NiString::Resize(unsigned int uiDelta)
{
    size_t stNewStringLength = Length() + uiDelta;
    if (stNewStringLength + sizeof(char) > 
        GetBufferSize(m_kHandle))
    {
        StringHandle kNewHandle = Allocate(stNewStringLength);
        unsigned int uiEndLoc = Length();
        NiMemcpy(kNewHandle, stNewStringLength, m_kHandle, Length());
        kNewHandle[uiEndLoc] = '\0';
        DecRefCount(m_kHandle);
        m_kHandle = kNewHandle;
        CalcLength();
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
// Add the char* to the end of this string
void NiString::Concatenate(char ch)
{ 
    CopyOnWriteAndResize(1);
    size_t stLength = Length();

    m_kHandle[stLength] = ch;
    m_kHandle[stLength+1] = '\0';
    CalcLength();

}
//---------------------------------------------------------------------------
char* NiString::ConsumeWhitespace(const char* pcStr, unsigned int uiLength)
{
    bool bContinue = true;
    unsigned int uiIdx = 0;
    for(uiIdx = 0; uiIdx < uiLength && bContinue; uiIdx++)
    {
        if (pcStr[uiIdx] != '\t' &&
            pcStr[uiIdx] != ' ')
            bContinue = false;
    }
    return (char*)pcStr + uiIdx - 1;
}
//---------------------------------------------------------------------------
// Convert the string into a boolean. true if successful, false if not.
bool NiString::ToBool(bool& b)
{ 
    char* pcStr = ConsumeWhitespace(GetString(m_kHandle), 
        Length());

    if (strlen(pcStr) < 4)
    {
        return false;
    }
    else if(NiStrnicmp(pcStr, "true", 4) == 0)
    {
        b = true;
        return true;
    }
    else if(NiStrnicmp(pcStr, "false", 4) == 0)
    {
        b = false;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
// Convert the string into a float. true if successful, false if not.
bool NiString::ToFloat(float& f)
{ 

    char* pcString = GetString(m_kHandle);

    if (pcString == NULL)
        return false;

    f = (float) atof(pcString);
    if (f == 0)
    {
        if (FindOneOf("-0") != INVALID_INDEX)
            return true;
        else
            return false;
    }
    return true;
}
//---------------------------------------------------------------------------
// Convert the string into an int. true if successful, false if not.
bool NiString::ToInt(int& i)
{
    char* pcString = GetString(m_kHandle);

    if (pcString == NULL)
        return false;

    i = atoi(pcString);
    if (i == 0)
    {
        if (FindOneOf("-0") != INVALID_INDEX)
            return true;
        else
            return false;
    }
    return true;
}
//---------------------------------------------------------------------------
// Convert the string into an NiPoint3. true if successful, false if not.
bool NiString::ToPoint3(NiPoint3& kPoint)
{ 
    const char acSeps [] = " \t(,)";
    char* pcContext;
    StringHandle kHandle = AllocateAndCopyHandle(m_kHandle);
    NiString kString(NiStrtok(GetString(kHandle), acSeps, &pcContext));
    if (!kString.ToFloat(kPoint.x))
    {
        Deallocate(kHandle);
        return false;
    }

    NiString kString2(NiStrtok(NULL, acSeps, &pcContext));
    if (!kString2.ToFloat(kPoint.y))
    {
        Deallocate(kHandle);
        return false;
    }

    NiString kString3(NiStrtok(NULL, acSeps, &pcContext));
    if (!kString3.ToFloat(kPoint.z))
    {
        Deallocate(kHandle);
        return false;
    }

    Deallocate(kHandle);
    return true;
}
//---------------------------------------------------------------------------
// Convert the string into an NiColor. true if successful, false if not.
bool NiString::ToRGB(NiColor& kColor)
{ 
    const char acSeps [] = " \t(,)";
    char* pcContext; 
    StringHandle kHandle = AllocateAndCopyHandle(m_kHandle);
    NiString kColorString(NiStrtok(GetString(kHandle), acSeps, &pcContext));
    if (!kColorString.Equals("#RGB"))
    {
        Deallocate(kHandle);
        return false;
    }

    NiString kString(NiStrtok(NULL, acSeps, &pcContext));
    if (!kString.ToFloat(kColor.r))
    {
        Deallocate(kHandle);
        return false;
    }

    if (kColor.r < 0.0f || kColor.r > 1.0f)
    {
        Deallocate(kHandle);
        return false;
    }

    NiString kString2(NiStrtok(NULL, acSeps, &pcContext));
    if (!kString2.ToFloat(kColor.g))
    {
        Deallocate(kHandle);
        return false;
    }

    if (kColor.g < 0.0f || kColor.g > 1.0f)
    {
        Deallocate(kHandle);
        return false;
    }

    NiString kString3(NiStrtok(NULL, acSeps, &pcContext));
    if (!kString3.ToFloat(kColor.b))
    {
        Deallocate(kHandle);
        return false;
    }

    if (kColor.b < 0.0f || kColor.b > 1.0f)
    {
        Deallocate(kHandle);
        return false;
    }

    Deallocate(kHandle);
    return true;
}
//---------------------------------------------------------------------------
// Convert the string into an NiColorA. true if successful, false if not.
bool NiString::ToRGBA(NiColorA& kColor)
{ 
    const char acSeps [] = " \t(,)";
    StringHandle kHandle = AllocateAndCopyHandle(m_kHandle);
    char* pcContext;

    NiString kColorString(NiStrtok(GetString(kHandle), acSeps, &pcContext));
    if (!kColorString.Equals("#RGBA"))
    {
        Deallocate(kHandle);
        return false;
    }

    NiString kString(NiStrtok(NULL, acSeps, &pcContext));
    if (!kString.ToFloat(kColor.r))
    {
        Deallocate(kHandle);
        return false;
    }

    if (kColor.r < 0.0f || kColor.r > 1.0f)
    {
        Deallocate(kHandle);
        return false;
    }
    NiString kString2(NiStrtok(NULL, acSeps, &pcContext));
    if (!kString2.ToFloat(kColor.g))
    {
        Deallocate(kHandle);
        return false;
    }

    if (kColor.g < 0.0f || kColor.g > 1.0f)
    {
        Deallocate(kHandle);
        return false;
    }

    NiString kString3(NiStrtok(NULL, acSeps, &pcContext));
    if (!kString3.ToFloat(kColor.b))
    {
        Deallocate(kHandle);
        return false;
    }

    if (kColor.b < 0.0f || kColor.b > 1.0f)
    {
        Deallocate(kHandle);
        return false;
    }

    NiString kString4(NiStrtok(NULL, acSeps, &pcContext));
    if (!kString4.ToFloat(kColor.a))
    {
        Deallocate(kHandle);
        return false;
    }

    if (kColor.a < 0.0f || kColor.a > 1.0f)
    {
        Deallocate(kHandle);
        return false;
    }

    Deallocate(kHandle);
    return true;
}
//---------------------------------------------------------------------------
// Returns the NiString representation of this object.
NiString NiString::FromBool(bool b)
{ 
    if (b)
        return NiString("TRUE");
    else
        return NiString("FALSE");
}
//---------------------------------------------------------------------------
// Returns the NiString representation of this object.
NiString NiString::FromFloat(float f)
{ 
    char acString[50];
    NiSprintf(acString, 50, "%f", f);
    NiString kString(acString);
    kString.Replace(",", ".");
    return kString;
}
//---------------------------------------------------------------------------
// Returns the NiString representation of this object.
NiString NiString::FromInt(int i)
{ 
    char acString[32];
    NiSprintf(acString, 32, "%d", i);
    return NiString(acString);
}
//---------------------------------------------------------------------------
// Returns the NiString representation of this object.
NiString NiString::FromPoint3(NiPoint3& kPt)
{ 
    char acString[128];
    NiSprintf(acString, 128, "(%f, %f, %f)", kPt.x, kPt.y, kPt.z);
    NiString kString(acString);
    return kString;
}
//---------------------------------------------------------------------------
// Returns the NiString representation of this object.
NiString NiString::FromRGB(NiColor& kColor)
{
    char acString[128];
    NiSprintf(acString, 128, "#RGB(%f, %f, %f)", kColor.r , kColor.g , 
        kColor.b);
    NiString kString(acString);
    return kString;
}
//---------------------------------------------------------------------------
// Returns the NiString representation of this object.
NiString NiString::FromRGBA(NiColorA& kColor)
{ 
    char acString[160];
    NiSprintf(acString, 160, "#RGBA(%f, %f, %f, %f)", kColor.r, kColor.g, 
        kColor.b, kColor.a);
    NiString kString(acString);
    return kString;
}
//---------------------------------------------------------------------------
NiString::operator const char*() const
{
    return GetString(m_kHandle);
}
//---------------------------------------------------------------------------
void NiString::Format(const char* pcFormatString ...)
{
    // Since the format string may include itself, we must be sure to save a 
    // copy of the buffer internally.
    StringHandle kHandle = m_kHandle;
    IncRefCount(kHandle);

    // Make the active copy unique so that we can write into it.
    CopyOnWrite(true);
    NIASSERT(kHandle != m_kHandle || m_kHandle == NULL);

    va_list argList;
    va_start(argList, pcFormatString);
    m_kHandle = vformat(pcFormatString, argList);

    va_end(argList);
    DecRefCount(kHandle);
}
//---------------------------------------------------------------------------
NiString::StringHandle NiString::vformat(const char* fmt, va_list argPtr) 
{
    StringHandle kHandle = m_kHandle;
    size_t stBufferSize = GetBufferSize(kHandle);
    NIASSERT(GetRefCount(kHandle) < 2);

    int numChars = -1;
    if (stBufferSize != 0)
    {
        NiVsprintf(kHandle, stBufferSize, fmt, argPtr);
    }

    while (numChars < 0 || kHandle == NULL ||
        (unsigned int) numChars == stBufferSize -1)
    {
        // We need to throw away the old buffer
        Deallocate(kHandle);

        // Allocate the larger buffer
        kHandle = Allocate(stBufferSize * 2);
        stBufferSize = GetBufferSize(kHandle, false);
        numChars = NiVsnprintf(kHandle, stBufferSize, stBufferSize - 1, fmt,
            argPtr);
    }

    kHandle[numChars] = '\0';
    SetLength(kHandle, numChars);
    return kHandle;
}
//---------------------------------------------------------------------------
NiString& NiString::operator=(NiString& stringSrc)
{
    if (GetString(stringSrc.m_kHandle) == GetString(m_kHandle))
        return *this;

    SetBuffer(stringSrc.m_kHandle);
    return *this;
}
//---------------------------------------------------------------------------
NiString& NiString::operator=(const char* lpsz)
{
    if (lpsz == GetString(m_kHandle))
        return *this;

    Swap(m_kHandle, lpsz);
    return *this;
}
//---------------------------------------------------------------------------
NiString& NiString::operator=(char ch)
{ 
    char acString[2];
    acString[0] = ch;
    acString[1] = '\0';
    return NiString::operator=((const char*)&acString[0]);
}
//---------------------------------------------------------------------------
NiString& NiString::operator=(char* lpsz)
{
    return NiString::operator=((const char*)lpsz);
}
//---------------------------------------------------------------------------
NiString& NiString::operator=(const NiFixedString& kString)
{
    return NiString::operator=((const char*)kString);
}
//---------------------------------------------------------------------------
NiString& NiString::operator+=(char ch)
{
     Concatenate(ch);
     return *this;
}
//---------------------------------------------------------------------------
NiString& NiString::operator+=(NiString& stringSrc)
{
    Concatenate(stringSrc);
    return *this;
}
//---------------------------------------------------------------------------
NiString& NiString::operator+=(char* lpsz)
{
     Concatenate(lpsz);
     return *this;
}
//---------------------------------------------------------------------------
NiString& NiString::operator+=(const char* pcString)
{
    Concatenate(pcString);
    return *this;
}
//---------------------------------------------------------------------------
NiString& NiString::operator+=(const NiFixedString& kString)
{
    Concatenate((const char*) kString);
    return *this;
}
//---------------------------------------------------------------------------
NiString  operator+(const NiString& string, char ch)
{
   NiString kString(string);
   kString.Concatenate(ch);
   return kString;
}
//---------------------------------------------------------------------------
NiString  operator+(char ch, const NiString& string)
{ 
   NiString kString(ch);
   kString.Concatenate(string);
   return kString;
}
//---------------------------------------------------------------------------
bool  operator==(const NiString& s1, const NiString& s2)
{ 
    return s1.Compare(s2) == 0; 
}
//---------------------------------------------------------------------------
bool  operator==(const NiString& s1, const char* s2)
{ 
    return s1.Compare(s2) == 0; 
}
//---------------------------------------------------------------------------
bool  operator==(const NiString& s1, const NiFixedString& s2)
{ 
    return s1.Compare((const char*)s2) == 0; 
}
//---------------------------------------------------------------------------
bool  operator==(const char* s1, const NiString& s2)
{ 
    return s2.Compare(s1) == 0; 
}
//---------------------------------------------------------------------------
bool operator==(const NiFixedString& s1, const NiString& s2)
{ 
    return s2.Compare((const char*)s1) == 0; 
}
//---------------------------------------------------------------------------
bool  operator!=(const NiString& s1, const NiString& s2)
{ 
    return s1.Compare(s2) != 0; 
}
//---------------------------------------------------------------------------
bool  operator!=(const NiString& s1, const char* s2)
{ 
    return s1.Compare(s2) != 0; 
}
//---------------------------------------------------------------------------
bool  operator!=(const NiString& s1, const NiFixedString& s2)
{ 
    return s1.Compare((const char*)s2) != 0; 
}
//---------------------------------------------------------------------------
bool  operator!=(const char* s1, const NiString& s2)
{ 
    return s2.Compare(s1) != 0; 
}
//---------------------------------------------------------------------------
bool  operator!=(const NiFixedString& s1, const NiString& s2)
{ 
    return s2.Compare((const char*)s1) != 0; 
}
//---------------------------------------------------------------------------
bool  operator<(const NiString& s1, const NiString& s2)
{ 
    return s1.Compare(s2) < 0; 
}
//---------------------------------------------------------------------------
bool  operator<(const NiString& s1, const char* s2)
{ 
    return s1.Compare(s2) < 0; 
}
//---------------------------------------------------------------------------
bool  operator<(const NiString& s1, const NiFixedString& s2)
{ 
    return s1.Compare((const char*)s2) < 0; 
}
//---------------------------------------------------------------------------
bool  operator<(const char* s1, const NiString& s2)
{ 
    return s2.Compare(s1) > 0; 
}
//---------------------------------------------------------------------------
bool  operator<(const NiFixedString& s1, const NiString& s2)
{ 
    return s2.Compare((const char*)s1) > 0; 
}
//---------------------------------------------------------------------------
bool  operator>(const NiString& s1, const NiString& s2)
{ 
    return s1.Compare(s2) > 0; 
}
//---------------------------------------------------------------------------
bool  operator>(const NiString& s1, const char* s2)
{ 
    return s1.Compare(s2) > 0; 
}
//---------------------------------------------------------------------------
bool  operator>(const NiString& s1, const NiFixedString& s2)
{ 
    return s1.Compare((const char*)s2) > 0; 
}
//---------------------------------------------------------------------------
bool  operator>(const char* s1, const NiString& s2)
{ 
    return s2.Compare(s1) < 0; 
}
//---------------------------------------------------------------------------
bool  operator>(const NiFixedString& s1, const NiString& s2)
{ 
    return s2.Compare((const char*)s1) < 0; 
}
//---------------------------------------------------------------------------
bool  operator<=(const NiString& s1, const NiString& s2)
{ 
    return s1.Compare(s2) <= 0; 
}
//---------------------------------------------------------------------------
bool  operator<=(const NiString& s1, const char* s2)
{ 
    return s1.Compare(s2) <= 0; 
}
//---------------------------------------------------------------------------
bool  operator<=(const NiString& s1, const NiFixedString& s2)
{ 
    return s1.Compare((const char*)s2) <= 0; 
}
//---------------------------------------------------------------------------
bool  operator<=(const char* s1, const NiString& s2)
{
    return s2.Compare(s1) >= 0; 
}
//---------------------------------------------------------------------------
bool  operator<=(const NiFixedString& s1, const NiString& s2)
{
    return s2.Compare((const char*)s1) >= 0; 
}
//---------------------------------------------------------------------------
bool  operator>=(const NiString& s1, const NiString& s2)
{ 
    return s1.Compare(s2) >= 0; 
}
//---------------------------------------------------------------------------
bool  operator>=(const NiString& s1, const char* s2)
{ 
    return s1.Compare(s2) >= 0; 
}
//---------------------------------------------------------------------------
bool  operator>=(const NiString& s1, const NiFixedString& s2)
{ 
    return s1.Compare((const char*)s2) >= 0; 
}
//---------------------------------------------------------------------------
bool  operator>=(const char* s1, const NiString& s2)
{ 
    return s2.Compare(s1) <= 0; 
}
//---------------------------------------------------------------------------
bool  operator>=(const NiFixedString& s1, const NiString& s2)
{ 
    return s2.Compare((const char*)s1) <= 0; 
}
//---------------------------------------------------------------------------
NiString  operator+(const NiString& string1, const NiString& string2)
{
    NiString kString(string1);
    kString.Concatenate(string2);
    return kString;
}
//---------------------------------------------------------------------------
NiString  operator+(const NiString& string1, char* pcStr)
{
    NiString kString(string1);
    kString.Concatenate(pcStr);
    return kString;
}
//---------------------------------------------------------------------------
NiString  operator+(const NiString& string1, const NiFixedString& kStr)
{
    NiString kString(string1);
    kString.Concatenate((const char*) kStr);
    return kString;
}
//---------------------------------------------------------------------------
NiString  operator+(char* pcStr, const NiString& string1)
{
    NiString kString(pcStr);
    kString.Concatenate(string1);
    return kString;
}
//---------------------------------------------------------------------------
NiString  operator+(const NiFixedString& kStr, const NiString& string1)
{
    NiString kString((const char*)kStr);
    kString.Concatenate(string1);
    return kString;
}
//---------------------------------------------------------------------------
char* NiString::MakeExternalCopy()
{
    unsigned int uiSize = Length()+1;
    char* pcStr = NiAlloc(char, uiSize);
    NiStrcpy(pcStr, uiSize, GetString(m_kHandle));
    return pcStr;
}
//---------------------------------------------------------------------------
// Extracts the middle part of a string 
NiString NiString::Mid(unsigned int uiBegin, unsigned int uiCount) const
{
    unsigned int uiEnd = uiBegin + uiCount;
    if (uiEnd > Length() || uiCount == NiString::INVALID_INDEX)
        uiEnd = Length();
    if (uiBegin < uiEnd && uiBegin < Length() && 
        uiEnd <= Length())
    {
        NiString kString((size_t)(uiEnd - uiBegin + 2));
        NiStrncpy(kString.m_kHandle, 
            kString.GetBufferSize(kString.m_kHandle) - 1,
            GetString(m_kHandle) + uiBegin, uiEnd - uiBegin);
        kString.m_kHandle[uiEnd - uiBegin] = '\0';
        kString.CalcLength();
        return kString;
    }
    else
    {
        return (const char*)NULL;
    }
}
//---------------------------------------------------------------------------
// Extracts the left part of a string 
NiString NiString::Left( unsigned int uiCount ) const
{
    unsigned int uiBegin = 0;
    unsigned int uiEnd = uiBegin + uiCount;
    if (uiEnd > Length())
        uiEnd = Length();
    if (uiBegin < uiEnd && uiBegin < Length() && 
        uiEnd <= Length())
    {
        NiString kString((size_t)(uiEnd - uiBegin + 2));
        NiStrncpy(kString.m_kHandle,  
            kString.GetBufferSize(kString.m_kHandle) - 1,
            GetString(m_kHandle) + uiBegin, uiEnd - uiBegin);
        kString.m_kHandle[uiEnd - uiBegin] = '\0';
        kString.CalcLength();
        return kString;
    }
    else
    {
        return (const char*)NULL;
    }
}
//---------------------------------------------------------------------------
// Extracts the right part of a string
NiString NiString::Right( unsigned int uiCount ) const
{
    unsigned int uiEnd = Length();
    int iBegin = uiEnd - uiCount;
    unsigned int uiBegin = 0;

    if (iBegin > 0)
        uiBegin = (unsigned int) iBegin;

    if (uiEnd > Length())
        uiEnd = Length();
    if (uiBegin < uiEnd && uiBegin < Length() && 
        uiEnd <= Length())
    {
        NiString kString((size_t)(uiEnd - uiBegin + 2));
        NiStrncpy(kString.m_kHandle,  
            kString.GetBufferSize(kString.m_kHandle) - 1,
            GetString(m_kHandle) + uiBegin, uiEnd - uiBegin);
        kString.m_kHandle[uiEnd - uiBegin] = '\0';
        kString.CalcLength();
        return kString;
    }
    else
    {
        return (const char*)NULL;
    }
}
//---------------------------------------------------------------------------
// Replace all exact matches of pcWhatToReplace in this string with
// pcReplaceWith
void NiString::Replace(const char* pcWhatToReplace, const char* pcReplaceWith)
{
    if (pcWhatToReplace == NULL || pcReplaceWith == NULL)
        return;

    unsigned int uiWhatToReplaceLength = strlen(pcWhatToReplace);
    unsigned int uiReplaceWithLength = strlen(pcReplaceWith);
    unsigned int uiOccuranceCount = 0;
    unsigned int uiOriginalLength = Length();

    unsigned int ui = 0;
    for (; ui < uiOriginalLength && ui != INVALID_INDEX;)
    {
        ui = Find(pcWhatToReplace, ui);
        if (ui != INVALID_INDEX)
        {
            uiOccuranceCount++;
            ui += uiWhatToReplaceLength;
        }
    }

    if (uiOccuranceCount == 0)
        return;

    unsigned int uiNumCharsToReplace = 
        uiOccuranceCount * uiWhatToReplaceLength;
    unsigned int uiNumCharsNeededToReplace = 
        uiOccuranceCount * uiReplaceWithLength;
    unsigned int uiSizeNeeded = 
        uiOriginalLength - uiNumCharsToReplace + uiNumCharsNeededToReplace;
    
    if (uiSizeNeeded > uiOriginalLength)
        CopyOnWriteAndResize(uiSizeNeeded - uiOriginalLength);
    else
        CopyOnWrite();

    for (ui = 0; ui < uiSizeNeeded && uiOccuranceCount > 0; uiOccuranceCount--)
    {
        char* pcStart = strstr(m_kHandle + ui, pcWhatToReplace);
        NIASSERT(pcStart != NULL);
        char* pcEnd = pcStart + uiReplaceWithLength;
        memmove(pcStart + uiReplaceWithLength, pcStart + uiWhatToReplaceLength,
            strlen(pcStart + uiWhatToReplaceLength) + 1);
        memcpy(pcStart, pcReplaceWith, uiReplaceWithLength);
        ui = pcEnd - m_kHandle;
    }

    m_kHandle[uiSizeNeeded] = '\0';

    CalcLength();
}
//---------------------------------------------------------------------------
void NiString::Insert(const char* pcInsertString, unsigned int uiIndex)
{
    if (pcInsertString == NULL)
        return;

    size_t stOriginalLength = Length();
    size_t stIndex = (size_t)uiIndex;
    size_t stInsertLength = strlen(pcInsertString);
    // Test to make sure the insertion string isn't a substring of me
#if defined(_DEBUG)
    const char* pcOriginalString = GetString(m_kHandle);
    NIASSERT(!(pcInsertString >= pcOriginalString && 
        pcInsertString <= pcOriginalString + stOriginalLength));
#endif

    if (stIndex >= stOriginalLength)
    {
        Concatenate(pcInsertString);
        return;
    }

    size_t stNewSize = stInsertLength + stOriginalLength + 1;

    CopyOnWriteAndResize(stInsertLength);
    char* pcBuffer = GetString(m_kHandle);
    NiMemmove(&pcBuffer[stIndex + stInsertLength], 
        stNewSize - stIndex - stInsertLength,
        &pcBuffer[stIndex], stOriginalLength - stIndex + 1);
    NiMemmove(&pcBuffer[stIndex], stNewSize - stIndex,
        pcInsertString, stInsertLength);
    CalcLength();
}
//---------------------------------------------------------------------------
void NiString::RemoveRange(unsigned int uiStartIdx, unsigned int uiNumChars)
{
    unsigned int uiEnd = Length();
    if (uiStartIdx >= uiEnd)
        return;

    if (uiStartIdx + uiNumChars > uiEnd)
        uiNumChars = uiEnd - uiStartIdx;

    CopyOnWrite();
    char* pcBuffer = GetString(m_kHandle);
    NiMemmove(&pcBuffer[uiStartIdx], uiEnd - uiStartIdx + 1,
        &pcBuffer[uiStartIdx + uiNumChars], 
        uiEnd - uiStartIdx - uiNumChars + 1);
    CalcLength();
}
//---------------------------------------------------------------------------
bool IsTrue(NiString strStr)
{
    bool b = false;
    if (strStr.ToBool(b))
        return b;

    NIASSERT(strStr.ToBool(b));
    return false;
}

//---------------------------------------------------------------------------
