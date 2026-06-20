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

#include "NiColor.h"

const NiColor NiColor::BLACK(0.0f,0.0f,0.0f);
const NiColor NiColor::WHITE(1.0f,1.0f,1.0f);
const NiColorA NiColorA::BLACK(0.0f,0.0f,0.0f,1.0f);
const NiColorA NiColorA::WHITE(1.0f,1.0f,1.0f,1.0f);

//---------------------------------------------------------------------------
void NiColor::Clamp()
{
    // Clamp to [0,1]^3.  Assumes that (r,g,b) >= (0,0,0).  This is a
    // reasonable assumption since colors are only added or multiplied in
    // the lighting system.  Note that clamping can cause significant
    // changes in the final color.
    if ( r > 1.0f )
        r = 1.0f;
    if ( g > 1.0f )
        g = 1.0f;
    if ( b > 1.0f )
        b = 1.0f;
}
//---------------------------------------------------------------------------
void NiColor::Scale()
{
    // Scale down by maximum component (preserves the final color).
    float fMax = r;
    if ( g > fMax )
        fMax = g;
    if ( b > fMax )
        fMax = b;

    if ( fMax > 1.0f )
    {
        float fInvMax = 1.0f/fMax;
        r *= fInvMax;
        g *= fInvMax;
        b *= fInvMax;
    }
}
//---------------------------------------------------------------------------
void NiColorA::Clamp()
{
    // Clamp to [0,1]^4.  Assumes that (r,g,b,a) >= (0,0,0).  This is a
    // reasonable assumption since colors are only added or multiplied in
    // the lighting system.  Note that clamping can cause significant
    // changes in the final color.
    if ( r > 1.0f )
        r = 1.0f;
    if ( g > 1.0f )
        g = 1.0f;
    if ( b > 1.0f )
        b = 1.0f;
    if ( a > 1.0f )
        a = 1.0f;
}
//---------------------------------------------------------------------------
void NiColorA::Scale()
{
    // Scale down by maximum component (preserves the final color).
    float fMax = r;
    if ( g > fMax )
        fMax = g;
    if ( b > fMax )
        fMax = b;

    if ( fMax > 1.0f )
    {
        float fInvMax = 1.0f/fMax;
        r *= fInvMax;
        g *= fInvMax;
        b *= fInvMax;
    }

    if ( a > 1.0f )
        a = 1.0f;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiColor::LoadBinary(NiStream& stream)
{
    NiStreamLoadBinary(stream,r);
    NiStreamLoadBinary(stream,g);
    NiStreamLoadBinary(stream,b);
}
//---------------------------------------------------------------------------
void NiColor::SaveBinary(NiStream& stream)
{
    NiStreamSaveBinary(stream,r);
    NiStreamSaveBinary(stream,g);
    NiStreamSaveBinary(stream,b);
}
//---------------------------------------------------------------------------
char* NiColor::GetViewerString(const char* pPrefix) const
{
    unsigned int uiLen = strlen(pPrefix) + 28;
    char* pString = NiAlloc(char, uiLen);
    NiSprintf(pString, uiLen, "%s = (%5.3f,%5.3f,%5.3f)", pPrefix, r, g, b);
    return pString;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void NiColorA::LoadBinary(NiStream& stream)
{
    NiStreamLoadBinary(stream,r);
    NiStreamLoadBinary(stream,g);
    NiStreamLoadBinary(stream,b);
    NiStreamLoadBinary(stream,a);
}
//---------------------------------------------------------------------------
void NiColorA::SaveBinary(NiStream& stream)
{
    NiStreamSaveBinary(stream,r);
    NiStreamSaveBinary(stream,g);
    NiStreamSaveBinary(stream,b);
    NiStreamSaveBinary(stream,a);
}
//---------------------------------------------------------------------------
char* NiColorA::GetViewerString(const char* pPrefix) const
{
    unsigned int uiLen = strlen(pPrefix) + 36;
    char* pString = NiAlloc(char, uiLen);
    NiSprintf(pString, uiLen, "%s = (%5.3f,%5.3f,%5.3f,%5.3f)",
        pPrefix, r, g, b, a);
    return pString;
}
//---------------------------------------------------------------------------
