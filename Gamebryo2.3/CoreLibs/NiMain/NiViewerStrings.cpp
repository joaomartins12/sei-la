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
#include "NiMainPCH.h"

#include "NiViewerStrings.h"
#include <NiSystem.h>

//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcRttiName)
{
    unsigned int uiLen = strlen(pcRttiName) + 9;
    char* pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "--- %s ---", pcRttiName);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, bool bValue)
{
    unsigned int uiLen = strlen(pcPrefix) + 9;
    char* pcString = NiAlloc(char, uiLen);
    if (bValue)
        NiSprintf(pcString, uiLen, "%s = true", pcPrefix);
    else
        NiSprintf(pcString, uiLen, "%s = false", pcPrefix);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, char cValue)
{
    unsigned int uiLen = strlen(pcPrefix) + 5;
    char* pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s = %c", pcPrefix, cValue);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, unsigned char ucValue)
{
    unsigned int uiLen = strlen(pcPrefix) + 7;
    char* pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s = %u", pcPrefix, (unsigned int)ucValue);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, short sValue)
{
    unsigned int uiLen = strlen(pcPrefix) + 10;
    char* pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s = %hd", pcPrefix, sValue);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, unsigned short usValue)
{
    unsigned int uiLen = strlen(pcPrefix) + 9;
    char* pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s = %hu", pcPrefix, usValue);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, int iValue)
{
    unsigned int uiLen = strlen(pcPrefix) + 15;
    char* pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s = %d", pcPrefix, iValue);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, unsigned int uiValue)
{
    unsigned int uiLen = strlen(pcPrefix) + 14;
    char* pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s = %u", pcPrefix, uiValue);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, long lValue)
{
    unsigned int uiLen = strlen(pcPrefix) + 15;
    char* pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s = %ld", pcPrefix, lValue);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, unsigned long ulValue)
{
    unsigned int uiLen = strlen(pcPrefix) + 14;
    char* pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s = %lu", pcPrefix, ulValue);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, float fValue)
{
    unsigned int uiLen = strlen(pcPrefix) + 20;
    char* pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s = %g", pcPrefix, fValue);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, double dValue)
{
    unsigned int uiLen = strlen(pcPrefix) + 20;
    char* pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s = %g", pcPrefix, dValue);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, void* pvValue)
{
    unsigned int uiLen = strlen(pcPrefix) + 16;
    char* pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s = %p", pcPrefix, pvValue);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, const char* pcValue)
{
    char* pcString;

    if (pcValue)
    {
        unsigned int uiLen = strlen(pcPrefix) + strlen(pcValue) + 4;
        pcString = NiAlloc(char, uiLen);
        NiSprintf(pcString, uiLen, "%s = %s", pcPrefix, pcValue);
    }
    else
    {
        unsigned int uiLen = strlen(pcPrefix) + 8;
        pcString = NiAlloc(char, uiLen);
        NiSprintf(pcString, uiLen, "%s = NULL", pcPrefix);
    }

    return pcString;
}
//---------------------------------------------------------------------------
