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

#include "NiPhysXPCH.h"

#include "NiPhysX.h"

//---------------------------------------------------------------------------
void NiPhysXUserOutput::reportError(NxErrorCode code, const char *message,
    const char *file, int line)
{
    char* pcErrorText = "";
    switch (code)
    {
        case NXE_NO_ERROR:
            pcErrorText = "No Error";
            break;
        case NXE_INVALID_PARAMETER:
            pcErrorText = "Invalid Parameter";
            break;
        case NXE_INVALID_OPERATION:
            pcErrorText = "Invalid Operation";
            break;
        case NXE_OUT_OF_MEMORY:
            pcErrorText = "Out of Memory";
            break;
        case NXE_INTERNAL_ERROR:
            pcErrorText = "Internal Error";
            break;
        case NXE_ASSERTION:
            pcErrorText = "Assertion";
            break;
        case NXE_DB_INFO:
            pcErrorText = "Debug Info";
            break;
        case NXE_DB_WARNING:
            pcErrorText = "Debug Warning";
            break;
        case NXE_DB_PRINT:
            pcErrorText = "Debug Print";
            break;
    }
    unsigned short usLength = strlen(pcErrorText) + strlen(message)
        + strlen(file) + 40;
    char* pcString = (char*)NiMalloc(usLength);
    
    NiSprintf(pcString, usLength, "PhysX Error: %s: %s in %s line %d\n",
        pcErrorText, message, file, line);

    NiOutputDebugString(pcString);
            
    NiFree(pcString);
}
//--------------------------------------------------------------------------- 
NxAssertResponse NiPhysXUserOutput::reportAssertViolation (const char *message,
    const char *file, int line)
{
    unsigned short usLength = strlen(message) + strlen(file) + 40;
    char* pcString = (char*)NiMalloc(usLength);
    
    NiSprintf(pcString, usLength, "PhysX Assert Violation: %s in %s line %d\n",
        message, file, line);
    
    NiOutputDebugString(pcString);
    
    NiFree(pcString);
    
    NIASSERT(false);

    return NX_AR_CONTINUE;
}
//--------------------------------------------------------------------------- 
void NiPhysXUserOutput::print(const char *message)
{
    NiOutputDebugString(message);
}   
//--------------------------------------------------------------------------- 
