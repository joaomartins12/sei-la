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

#include "NiRTLib.h"
#include "NiSystem.h"
#include "NiSystemTimeLock.h"

//---------------------------------------------------------------------------
void ReadNDLDevLock(char* pcResult)
{
#if _MSC_VER >= 1400
    size_t stSize = 0;
    getenv_s(&stSize, NULL, 0, "ndldevlock");
    if (stSize == 17)
    {
        if (getenv_s(&stSize, pcResult, 17, "ndldevlock") == 0)
            return;
    }

    pcResult[0] =  '\0';
#else //#if _MSC_VER >= 1400
    char* pcDevLock = getenv("ndldevlock");
    if (pcDevLock)
        strcpy(pcResult, pcDevLock);
    else
        pcResult[0] = '\0';
#endif //#if _MSC_VER >= 1400
}
//---------------------------------------------------------------------------
void NormalizeExpireTime(unsigned long& ulTime)
{/**/}
//---------------------------------------------------------------------------
void DialogBoxExit(const char* pcText, const char* pcCaption)
{
    NiMessageBox(pcText,pcCaption);
    exit(0);
}
//---------------------------------------------------------------------------
bool CheckForTimeReset(unsigned int uiDecExpire)
{
    //struct _finddata_t info;
    // int _wfindnext(
    // intptr_t handle,
    struct _wfinddata_t info;
    wchar_t wcPath[] = L"c:/windows/*.*";
    size_t stLength = wcslen(wcPath);

    long handle = _wfindfirst(&wcPath[0],&info);
    if ( handle != -1 )
    {
        while ( _wfindnext(handle,&info) != -1 )
        {
            // If any file is more than 1 hour newer than the expiration time,
            // somebody is probably being a very bad bunny...
            // 3600 = 1 hr * 60 * 60
            if ( unsigned(info.time_write) > uiDecExpire+3600 )
                return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------