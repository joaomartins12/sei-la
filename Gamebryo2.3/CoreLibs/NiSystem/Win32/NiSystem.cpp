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
#include "NiSystemPCH.h"

#include "NiSystem.h"
#include "NiVersion.h"
#include "NiFile.h"

//---------------------------------------------------------------------------
// The following copyright notice may not be removed.
static char EmergentCopyright[] NI_UNUSED =
    "Copyright 2007 Emergent Game Technologies";
//---------------------------------------------------------------------------
static char acGamebryoVersion[] NI_UNUSED = 
    GAMEBRYO_MODULE_VERSION_STRING(NiSystem);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
float NiGetCurrentTimeInSec()
{
    static bool bFirst = true;
    static LARGE_INTEGER freq;
    static LARGE_INTEGER initial;

    if (bFirst)
    {
        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&initial);
        bFirst = false;
    }

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (float)((long double)
        (counter.QuadPart - initial.QuadPart) / 
        (long double) freq.QuadPart);
}
//---------------------------------------------------------------------------
unsigned int NiGetPerformanceCounter()
{
    LARGE_INTEGER counter;

    QueryPerformanceCounter(&counter);
    return counter.LowPart;
}
//---------------------------------------------------------------------------
int NiStricmp(const char* s1, const char* s2)
{
#if _MSC_VER >= 1400
    return _stricmp(s1, s2);
#else // #if _MSC_VER >= 1400
    return stricmp(s1, s2);
#endif // #if _MSC_VER >= 1400
}
//---------------------------------------------------------------------------
int NiStrnicmp(const char* s1, const char* s2, size_t n)
{
#if _MSC_VER >= 1400
    return _strnicmp(s1, s2, n);
#else // #if _MSC_VER >= 1400
    return strnicmp(s1, s2, n);
#endif // #if _MSC_VER >= 1400
}
//---------------------------------------------------------------------------
char* NiStrdup(const char* pcString)
{
    if (pcString == NULL)
        return NULL;

    unsigned int uiLen = strlen(pcString);
    char* pcReturn = NiAlloc(char, uiLen+1);
    memcpy(pcReturn, pcString, uiLen);
    pcReturn[uiLen] = '\0';
    return pcReturn;
}
//---------------------------------------------------------------------------
// _rotr operated on unsigned int (4 bytes on a PC).  So the non-Windows
// version should be 32 bits as well.
unsigned int NiRotr(unsigned int x, int n)
{
    return _rotr(x, n);
}
//---------------------------------------------------------------------------
unsigned int NiGetFileSize (const char* pFilename)
{
    // get file statistics
    struct _stat info;
    int result = _stat(pFilename,&info);

    if ( result != 0 )
        // could not get statistics (maybe file does not exist)
        return 0;

    return int(info.st_size);
}
//---------------------------------------------------------------------------
