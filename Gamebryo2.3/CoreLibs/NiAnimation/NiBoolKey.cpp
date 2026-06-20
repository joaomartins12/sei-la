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
#include "NiAnimationPCH.h"

#include <NiStream.h>
#include <NiBool.h>
#include "NiBoolKey.h"

NiImplementAnimationStream(NiBoolKey,BOOLKEY,NOINTERP);

// default return value for GenInterp
bool NiBoolKey::ms_bDefault = false;

//---------------------------------------------------------------------------
void NiBoolKey::RegisterSupportedFunctions(KeyContent eContent,
    KeyType eType)
{
    SetCurvatureFunction(eContent, eType, ComputeMaxCurvature);
    SetInterpFunction(eContent, eType, Interpolate);
    SetInterpD1Function(eContent, eType, InterpolateD1);
    SetInterpD2Function(eContent, eType, InterpolateD2);
    SetEqualFunction(eContent, eType, Equal);
    SetFillDerivedValsFunction(eContent, eType, FillDerivedVals);
    SetInsertFunction(eContent, eType, NULL);
}
//---------------------------------------------------------------------------
bool NiBoolKey::GenInterp(float fTime, NiBoolKey* pkKeys,
    NiBoolKey::KeyType eType, unsigned int uiNumKeys, unsigned int& uiLastIdx,
    unsigned char ucSize)
{
    NIASSERT(uiNumKeys != 0);
    if (uiNumKeys == 1)
        return pkKeys->GetKeyAt(0, ucSize)->GetBool();

    unsigned int uiNumKeysM1 = uiNumKeys - 1;

    // This code assumes that the time values in the keys are ordered by
    // increasing value.  The search can therefore begin at uiLastIdx rather
    // than zero each time.  The idea is to provide an O(1) lookup based on
    // time coherency of the keys.

    float fLastTime = pkKeys->GetKeyAt(uiLastIdx, ucSize)->GetTime();
    if ( fTime < fLastTime )
    {
        uiLastIdx = 0;
        fLastTime = pkKeys->GetKeyAt(0, ucSize)->GetTime();
    }
    
    unsigned int uiNextIdx;
    float fNextTime = 0.0f;
    for (uiNextIdx = uiLastIdx + 1; uiNextIdx <= uiNumKeysM1; uiNextIdx++)
    {
        fNextTime = pkKeys->GetKeyAt(uiNextIdx, ucSize)->GetTime();
        if ( fTime <= fNextTime )
            break;

        uiLastIdx++;
        fLastTime = fNextTime;
    }

    NIASSERT(uiNextIdx < uiNumKeys);
    NIASSERT(fNextTime > fLastTime);

    // interpolate the keys, requires that the time is normalized to [0,1]
    NiBoolKey::InterpFunction interp = NiBoolKey::GetInterpFunction(eType);
    float fNormTime = (fTime - fLastTime)/(fNextTime - fLastTime);
    NIASSERT( interp );
    bool kResult;
    interp(fNormTime, pkKeys->GetKeyAt(uiLastIdx, ucSize),
        pkKeys->GetKeyAt(uiNextIdx, ucSize), &kResult);
    return kResult;
}
//---------------------------------------------------------------------------
bool NiBoolKey::GenInterpDefault(float fTime, NiBoolKey* pkKeys,
    KeyType eType, unsigned int uiNumKeys, unsigned char ucSize)
{
    if (uiNumKeys)
    {
        // GenInterp no longer handles out-of-range times
        if (fTime < pkKeys->GetKeyAt(0, ucSize)->GetTime())
            return pkKeys->GetKeyAt(0, ucSize)->GetBool();
        else if (fTime > pkKeys->GetKeyAt(uiNumKeys-1, ucSize)->GetTime())
            return pkKeys->GetKeyAt(uiNumKeys-1, ucSize)->GetBool();
        else
        {
            unsigned int uiLastIdx = 0;
            return NiBoolKey::GenInterp(fTime, pkKeys, eType, uiNumKeys,
                uiLastIdx, ucSize);
        }
    }
    else
    {
        return ms_bDefault;
    }
}
//---------------------------------------------------------------------------
void NiBoolKey::SetDefault(const bool bDefault)
{
    ms_bDefault = bDefault;
}
//---------------------------------------------------------------------------
bool NiBoolKey::Equal(const NiAnimationKey& key0, const NiAnimationKey& key1)
{
    if ( !NiAnimationKey::Equal(key0,key1) )
        return false;

    const NiBoolKey& Bool0 = (const NiBoolKey&) key0;
    const NiBoolKey& Bool1 = (const NiBoolKey&) key1;

    return Bool0.m_bBool == Bool1.m_bBool;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiAnimationKey* NiBoolKey::CreateFromStream(NiStream&, unsigned int)
{
    // This routine should never be called.
    NIASSERT(false);
    return 0;
}
//---------------------------------------------------------------------------
void NiBoolKey::SaveToStream(NiStream& stream, NiAnimationKey* pkKeys,
    unsigned int uiNumKeys)
{
    // This routine should never be called.
    NIASSERT(false);
}
//---------------------------------------------------------------------------
void NiBoolKey::LoadBinary(NiStream& stream)
{
    NiStreamLoadBinary(stream,m_fTime);

    // NiBool is used for portability of NIF files
    NiBool bBool;
    NiStreamLoadBinary(stream,bBool);
    m_bBool = (bBool != 0);
}
//---------------------------------------------------------------------------
void NiBoolKey::SaveBinary(NiStream& stream, NiAnimationKey* pkKey)
{
    NiAnimationKey::SaveBinary(stream, pkKey);

    NiBoolKey* pkBool = (NiBoolKey*) pkKey;
    // NiBool is used for portability of NIF files
    NiBool bBool = ( pkBool->m_bBool ? 1 : 0 );
    NiStreamSaveBinary(stream, bBool);
}
